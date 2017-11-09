// ------------------------------------------------------
// LibSL::Math::PCA
// ------------------------------------------------------
//
// Principal Component Analysis classes and 3 implementations
// 
// ------------------------------------------------------
// Christian Eisenacher - 2006-11-19
// ------------------------------------------------------

#pragma once

#include <LibSL/Math/Math.h>
#include <LibSL/System/Types.h>
#include <LibSL/Memory/Pointer.h>
#include <vector>

#include <newmat/newmatap.h>
#define PCA_EPSILON 0.0000001

namespace LibSL {
	namespace Math {

		// **********************************************************************************
		// PCA: Principal Component Analysis: The following implementations are provided:
		//   1.) BatchPCA: GroundTruth
		//   2.) IPCA: Incremenal estimation of mean and covariance Matrix
		//   3.) CCIPCA: Incremental estimation of Eigenvevtors without covariance Matrix
		//       (for _really_ large feature dimensions like eigenfaces, but you
		//        might consider to use a real Matrix library in that situation!)
		//   4.) PersistentPCA: Restored from disk.
		//       addSamples(), computeTranspromation(), finalize() are inert
		//
		// Speed Comparison: 512^2 samples, 75D each;
		// Timing for generation of samples, computing of PCA and projection to 10 dimensions
		// IPCA:     26 sec
		// BatchPCA: 63 sec
		// CCIPCA:  157 sec
		// Note: The two incremental implementations have aproximation errors when used with
		//       few samples!
		// **********************************************************************************

		class PCA
		{
		protected:
			uint m_SampleDimensions;
			uint m_TargetDimensions;

			NEWMAT::ColumnVector m_Mean; // substract to make zero mean dataset
			NEWMAT::Matrix m_Projection;

			void setupProjections();

		public:
			typedef LibSL::Memory::Pointer::AutoPtr<PCA> t_AutoPtr;

			virtual ~PCA() {};

			uint sampleDimensions() const {return m_SampleDimensions;}
			uint targetDimensions() const {return m_TargetDimensions;}

			const NEWMAT::Matrix&       getProjectionMatrix()       const {return m_Projection;}
			const NEWMAT::ColumnVector& getMean()                   const {return m_Mean;};

			virtual void addSample(const NEWMAT::ColumnVector& sampleVec) = 0;

			virtual void computeTransformation()                          = 0;
			virtual void finalize()                                       = 0;

			virtual NEWMAT::Matrix      getNormalizedEigenVectors() const = 0;
			virtual NEWMAT::RowVector   getEigenValues()            const = 0;
			virtual NEWMAT::RowVector   getVariances()              const {return getEigenValues();}
			NEWMAT::RowVector           getStdDeviations()          const;

			NEWMAT::ColumnVector project(const NEWMAT::ColumnVector& vec) const;

			NEWMAT::ColumnVector unProject(const NEWMAT::ColumnVector& vec) const;

			virtual uint suggestDimensionsForPercentage(double percentage = 0.9) const;

			virtual void setTargetDimensions(const uint dim){
				m_TargetDimensions = LibSL::Math::min<uint>(dim, m_SampleDimensions);
				setupProjections();
			}
		};
		typedef PCA::t_AutoPtr                       PCA_Ptr;





		// **********************************************************************************
		// Batch PCA: Three processing steps:
		//   1.) Collects a number of sample Vectors (can me really huge)
		//   2.) Compute Eigenvectors/values (needed Covariance Matrix can also be huge)
		//   3.) Compute Projection Matrices
		// **********************************************************************************

		class BatchPCA : public PCA
		{
		protected:
			NEWMAT::Matrix m_EigenVectors;
			NEWMAT::RowVector m_EigenValues;

			std::vector<NEWMAT::ColumnVector> m_Samples; //store samples to compute PCA from

			void m_Init(const uint sampleDim, const uint targetDim);

		public:
			BatchPCA(const uint sampleDim){
				m_Init(sampleDim, sampleDim);
			}
			BatchPCA(const uint sampleDim, const uint targetDim){
				m_Init(sampleDim, targetDim);
			}
			~BatchPCA() {};

			void addSample(const NEWMAT::ColumnVector& sampleVec) {m_Samples.push_back(sampleVec);}

			void discardSamples() {m_Samples.clear();}

			void computeTransformation();
			void finalize() {computeTransformation(); setupProjections(); discardSamples();}

			NEWMAT::Matrix    getNormalizedEigenVectors() const {return m_EigenVectors;}
			NEWMAT::RowVector getEigenValues()            const {return m_EigenValues;}
		};
		typedef BatchPCA::t_AutoPtr                       BatchPCA_Ptr;




		// **********************************************************************************
		// Incremental PCA: Three processing steps:
		//   1.) Collects a number of sample Vectors and update CovarianceMatrix/mean for each
		//   2.) Compute Eigenvectors/values (needed Covariance Matrix can also be huge)
		//   3.) Compute Projection Matrices
		// Note: This thing converges to the right solution if enough samples are given
		//       Major culprit is the incremental mean used to obtain a estimate for the Variance
		// **********************************************************************************

		class IPCA : public PCA
		{
		protected:
			NEWMAT::Matrix m_Covariance;
			NEWMAT::Matrix m_EigenVectors;
			NEWMAT::RowVector m_EigenValues;
			uint m_N;   //nr of samples

			void m_Init(const uint sampleDim, const uint targetDim);

		public:
			IPCA(const uint sampleDim){
				m_Init(sampleDim, sampleDim);
			}
			IPCA(const uint sampleDim, const uint targetDim){
				m_Init(sampleDim, targetDim);
			}
			~IPCA() {};

			void addSample(const NEWMAT::ColumnVector& sampleVec);

			void discardSamples() {m_Covariance = NEWMAT::Matrix();}

			void computeTransformation();
			void finalize() {computeTransformation(); setupProjections(); discardSamples();}

			NEWMAT::Matrix    getNormalizedEigenVectors() const {return m_EigenVectors;}
			NEWMAT::RowVector getEigenValues()            const {return m_EigenValues;}
		};
		typedef IPCA::t_AutoPtr                       IPCA_Ptr;





		// **********************************************************************************
		// CCI PCA: (Covariance Free Incremental PCA) Two processing steps:
		//   1.) Collects a number of sample Vectors and update Eigenvectors for each.
		//   2.) Compute Projection Matrices
		// Note: To speed things up, the maximal number of target dimensions can be
		//       specified at instanciation time.
		// Implementation follows the original paper as closly as possible:
		// http://web.cse.msu.edu/~zhangyil/papers/tpami2003.pdf
		// Small changes:
		//  - Change the initialisation to use Cartesian Coordinate system as initial set of eigenvectors
		//  - divide by zero checks
		// Note: Pretty slow unless samples with large dimensions and few Eigenvectors are used
		// **********************************************************************************

		class CCIPCA : public PCA
		{
		protected:
			uint m_MaxTargetDimensions;
			uint m_N;   //nr of samples
			double m_L; // Relaxation parameter [1..4] speeds up convergence

			NEWMAT::Matrix m_EigenVectors; //not normalized!

			void m_Init(const uint sampleDim,const  uint targetDimMax);

		public:
			CCIPCA(const uint sampleDim){
				m_Init(sampleDim, sampleDim);
			}
			CCIPCA(const uint sampleDim, const uint targetDimMax){
				m_Init(sampleDim, targetDimMax);
			}
			~CCIPCA() {};

			void addSample(const NEWMAT::ColumnVector& sampleVec);

			void computeTransformation() {}
			void finalize() {setupProjections();}

			NEWMAT::Matrix     getNormalizedEigenVectors() const;
			NEWMAT::RowVector  getEigenValues()            const;

			void setTargetDimensions(const uint dim){
				m_TargetDimensions = LibSL::Math::min<uint>(dim, m_MaxTargetDimensions);
				setupProjections();
			};
		};
		typedef CCIPCA::t_AutoPtr                       CCIPCA_Ptr;





		// **********************************************************************************
		//  PersistentPCA can only be used for projections.
		//  Note: Target Dimensions can be changed however
		// **********************************************************************************

		class PersistentPCA : public PCA
		{
		protected:
			NEWMAT::Matrix m_NormalizedEigenVectors;
			NEWMAT::RowVector m_EigenValues;
	
			void m_Init(const char* filename);

		public:
			PersistentPCA(const char* filename){
				m_Init(filename);
			}

			virtual void addSample(const NEWMAT::ColumnVector& sampleVec) {}

			virtual void computeTransformation()                          {}
			virtual void finalize()                                       {}

			virtual NEWMAT::Matrix      getNormalizedEigenVectors() const {return m_NormalizedEigenVectors;}
			virtual NEWMAT::RowVector   getEigenValues()            const {return m_EigenValues;}

			virtual void setTargetDimensions(const uint dim){
				m_TargetDimensions = LibSL::Math::min<uint>(dim, getNormalizedEigenVectors().Ncols());
				setupProjections();
			}
		};
		typedef PersistentPCA::t_AutoPtr                       PersistentPCA_Ptr;


		void saveAsPersistentPCA(const char* name, const PCA_Ptr pca);

	} //namespace LibSL::Math
} //namespace LibSL
