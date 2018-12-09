// ------------------------------------------------------
#include "LibSL.precompiled.h"
// ------------------------------------------------------

#include "PCA.h"

// ------------------------------------------------------

#define NAMESPACE LibSL::Math

using namespace NEWMAT;

// ------------------------------------------------------

void NAMESPACE::PCA::setupProjections(){

	m_Projection = getNormalizedEigenVectors().Columns(1,m_TargetDimensions).t();
/*
  //compute pseudoinverse! will converge to inverse if it exists
	Matrix U(m_SampleDimensions, m_SampleDimensions);
	U = 0.0;
	U.Rows(1,m_TargetDimensions) = m_Projection;
	Matrix V;
	DiagonalMatrix D;
	SVD(U, D, U, V); //to save allocation for another matrix
	for (int i = 0; i < D.Ncols(); i++){
		if (D[i] > PCA_EPSILON){
			D[i] = 1.0/D[i];
		}
	}
	m_BackProjection = V * D * U.t();
*/

};

// ------------------------------------------------------

uint NAMESPACE::PCA::suggestDimensionsForPercentage(double percentage) const{
	//Covariance Matrix is positive semidefinit i.e. all eigenvalues are >=0!
	RowVector eigenValues = getEigenValues();
	double desiredVariance = eigenValues.Sum() * percentage;

	uint targetDimension = 0;
	for (int i = 0; i < eigenValues.Ncols(); i++){
		if (desiredVariance > 0){
			targetDimension++;
		}
		desiredVariance -= eigenValues[i];
	}
	return targetDimension;
}

// ------------------------------------------------------

RowVector NAMESPACE::PCA::getStdDeviations() const{
	RowVector eigenValues = getEigenValues();
	for (int i = 0; i < eigenValues.Ncols(); i++){
		eigenValues[i] = sqrt(eigenValues[i]);
	}
	return eigenValues;
};

// ------------------------------------------------------

ColumnVector NAMESPACE::PCA::project(const ColumnVector& vec) const {
	return m_Projection * (vec - m_Mean);
  /*
  ColumnVector res(m_SampleDimensions);
  ForIndex(r, getNormalizedEigenVectors().Nrows()){
    res[r] = 0.0;
		ForIndex(c, getNormalizedEigenVectors().Ncols()){
      res[r] += (vec[c] - m_Mean[c]) * getNormalizedEigenVectors()[c][r];
    }
  }
  return res;
  */
};

// ------------------------------------------------------

ColumnVector NAMESPACE::PCA::unProject(const ColumnVector& vec) const {
	ColumnVector res(m_SampleDimensions);
  ForIndex(j, m_SampleDimensions) {
    res[j] = 0; 
  }
  ForIndex(c, getNormalizedEigenVectors().Ncols()){
    ForIndex(r, getNormalizedEigenVectors().Nrows()){
      res[r] += vec[c] * getNormalizedEigenVectors()[r][c];
    }
  }
  res = res + m_Mean;
  return res;
}

// ------------------------------------------------------
/*
ColumnVector NAMESPACE::PCA::unProject(const ColumnVector& vec) const {
	ColumnVector tmp(m_SampleDimensions);
	tmp = 0.0;
	tmp.Rows(1,vec.Nrows()) = vec;
	return (m_BackProjection * tmp) + m_Mean;
}
*/
// ------------------------------------------------------
// ------------------------------------------------------


void NAMESPACE::BatchPCA::m_Init(uint sampleDim, uint targetDim){
	m_SampleDimensions = sampleDim;
	m_TargetDimensions = min<uint>(sampleDim,targetDim);
	m_Mean = ColumnVector(m_SampleDimensions);
	m_Mean = 0.0;
	m_Projection = IdentityMatrix(m_SampleDimensions);
//	m_BackProjection = IdentityMatrix(m_SampleDimensions);
	m_EigenVectors = IdentityMatrix(m_SampleDimensions);
	m_EigenValues = RowVector(m_SampleDimensions);
	m_EigenValues = 0.0;
}

// ------------------------------------------------------





void NAMESPACE::BatchPCA::computeTransformation()
{
	if (m_Samples.size() < 2){// need at least 2 samples to get any variance and even that is not guaranteed if it is twice the same sample!
		return;
	}
	//get the mean
	m_Mean = 0.0;
	for(uint s = 0 ; s < m_Samples.size(); s++) {
		m_Mean += m_Samples[s];
	}
	m_Mean /= (RBD_COMMON::Real)m_Samples.size();

	//compute the covariance matrix
	Matrix temp(m_SampleDimensions, m_SampleDimensions);
	temp = 0.0;
	for(uint s = 0; s < m_Samples.size(); s++) {
		ColumnVector sample = m_Samples[s] - m_Mean;
		temp += sample * sample.t();
	}
	SymmetricMatrix covariance;
	covariance << (temp / (RBD_COMMON::Real)m_Samples.size());


	// ****************************
	// Compute EigenVectors/-values
	// ****************************
	DiagonalMatrix D; //will contain the eigenvalues in _ascending_ order!
	EigenValues(covariance, D, temp);

	//reverse sorting due to _ascending_ library output!
	m_EigenVectors = Matrix(m_SampleDimensions,0);
	for (uint i = m_SampleDimensions; i > 0; i--){
		m_EigenValues(m_SampleDimensions-i+1) = D(i);
		m_EigenVectors |= temp.Column(i);
	}
};


// ------------------------------------------------------
// ------------------------------------------------------


void NAMESPACE::CCIPCA::m_Init(uint sampleDim, uint targetDimMax){
	m_SampleDimensions = sampleDim;
	m_TargetDimensions = min<uint>(sampleDim,targetDimMax);
	m_MaxTargetDimensions = min<uint>(sampleDim,targetDimMax);

	m_EigenVectors = IdentityMatrix(m_SampleDimensions).Columns(1,m_MaxTargetDimensions);

	m_Projection = IdentityMatrix(m_SampleDimensions);
//	m_BackProjection = IdentityMatrix(m_SampleDimensions);

	m_Mean = ColumnVector(m_SampleDimensions);
	m_Mean = 0.0;
	m_N = 0;
	m_L = 2.5; // Speedup convergence. Avoid integers!!!!

	m_Projection = IdentityMatrix(m_SampleDimensions);
//	m_BackProjection = IdentityMatrix(m_SampleDimensions);
}

// ------------------------------------------------------

#define V(I) m_EigenVectors.Column(i)

// ------------------------------------------------------

void NAMESPACE::CCIPCA::addSample(const ColumnVector& sampleVec) {
	m_N++;

	m_Mean = (((m_N-1.0)/m_N) * m_Mean) + ((1.0/m_N) * sampleVec);	

	ColumnVector u = sampleVec - m_Mean;
	for (uint i = 1; i <= min(m_MaxTargetDimensions, m_N); i++){
		if (i == m_N){
			//V(I) = u; // first mean will be zero, keeping a cartesian base vector is much better for initialisation.
		}
		else{
			double length = V(i).NormFrobenius();
			if (length > PCA_EPSILON){
				//V(i) = (m_N - 1 - m_L)/m_N * V(i) + (1 + m_L)/m_N * u * u.t() * V(i)/V(i).NormFrobenius();
				//slightly rewritten/rebracketed to be more computationally efficient
				V(i) = ((m_N - 1 - m_L)/m_N) * V(i) + (((1 + m_L)/m_N) * DotProduct(u.t(), V(i)) / length) * u;

				length = V(i).SumSquare();
				if (length > PCA_EPSILON){
					//u = u - u.t()* V(i)/V(i).NormFrobenius()* V(i)/V(i).NormFrobenius();
					//slightly rewritten/rebracketed to be more computationally efficient
					u -= (DotProduct(u.t(), V(i)) / length) * V(i);
				}
			}
		}
	}
}

// ------------------------------------------------------

Matrix NAMESPACE::CCIPCA::getNormalizedEigenVectors() const{
	Matrix eigenVectors = Matrix(m_SampleDimensions,0);
	for (uint i = 1; i <= m_MaxTargetDimensions; i++){
		double length = m_EigenVectors.Column(i).NormFrobenius();
		if (length > PCA_EPSILON){
			eigenVectors |= m_EigenVectors.Column(i)/length;
		}
		else{
			eigenVectors |= m_EigenVectors.Column(i);
		}
	}
	return eigenVectors;
}

// ------------------------------------------------------

RowVector NAMESPACE::CCIPCA::getEigenValues() const{
	RowVector eigenValues = RowVector(m_MaxTargetDimensions);
	for (uint i = 1; i <= m_MaxTargetDimensions; i++){
		eigenValues(i) = m_EigenVectors.Column(i).NormFrobenius();
	}
	return eigenValues;
}



// ------------------------------------------------------
// ------------------------------------------------------




void NAMESPACE::IPCA::m_Init(uint sampleDim, uint targetDim){
	m_SampleDimensions = sampleDim;
	m_TargetDimensions = min<uint>(sampleDim,targetDim);
	m_N = 0;
	m_Mean = ColumnVector(m_SampleDimensions);
	m_Mean = 0.0;
	m_Covariance = Matrix(m_SampleDimensions, m_SampleDimensions);
	m_Covariance = 0.0;
	m_Projection = IdentityMatrix(m_SampleDimensions);
//	m_BackProjection = IdentityMatrix(m_SampleDimensions);
	m_EigenVectors = IdentityMatrix(m_SampleDimensions);
	m_EigenValues = RowVector(m_SampleDimensions);
	m_EigenValues = 0.0;
}

// ------------------------------------------------------





void NAMESPACE::IPCA::computeTransformation()
{
	if (m_N < 2){// need at least 2 samples to get any variance and even that is not guaranteed if it is twice the same sample!
		return;
	}
	//compute the covariance matrix
	Matrix temp(m_SampleDimensions, m_SampleDimensions);
	SymmetricMatrix covariance;
	covariance << (m_Covariance / m_N);


	// ****************************
	// Compute EigenVectors/-values
	// ****************************
	DiagonalMatrix D; //will contain the eigenvalues in _ascending_ order!
	EigenValues(covariance, D, temp);

	//reverse sorting due to _ascending_ library output!
	m_EigenVectors = Matrix(m_SampleDimensions,0);
	for (uint i = m_SampleDimensions; i > 0; i--){
		m_EigenValues(m_SampleDimensions-i+1) = D(i);
		m_EigenVectors |= temp.Column(i);
	}
}

// ------------------------------------------------------

void NAMESPACE::IPCA::addSample(const ColumnVector& sampleVec) {
	m_N++;

	m_Mean = (((m_N-1.0)/m_N) * m_Mean) + ((1.0/m_N) * sampleVec);	

	ColumnVector sample = sampleVec - m_Mean;

	m_Covariance += sample * sample.t();
}



// ------------------------------------------------------
// ------------------------------------------------------





void NAMESPACE::PersistentPCA::m_Init(const char* filename){
	std::ifstream IMAP(filename);

	IMAP >> m_SampleDimensions;

	IMAP >> m_TargetDimensions;

	int dim;
	IMAP >> dim;
	m_Mean = NEWMAT::ColumnVector(dim);
	ForIndex(c, m_Mean.Nrows()){
		IMAP >> m_Mean[c];
	}

	IMAP >> dim;
	m_EigenValues = NEWMAT::RowVector(dim);
	ForIndex(c, m_EigenValues.Ncols()){
		IMAP >> m_EigenValues[c];
	}

	int nrow, ncol;
	IMAP >> nrow;
	IMAP >> ncol;

	m_NormalizedEigenVectors = NEWMAT::Matrix(nrow, ncol);
	ForIndex(i, m_NormalizedEigenVectors.Nrows()){
		ForIndex(j, m_NormalizedEigenVectors.Ncols()){
			IMAP >> m_NormalizedEigenVectors[i][j];
		}
	}

	IMAP.close();

	setupProjections();
}

// ------------------------------------------------------

void NAMESPACE::saveAsPersistentPCA(const char* name, const PCA_Ptr pca){
	std::ofstream OMAP(name);

	OMAP << pca->sampleDimensions()<<" "<<pca->targetDimensions()<<endl;

	OMAP << pca->getMean().Nrows()<< " ";
	ForIndex(c, pca->getMean().Nrows()){
		OMAP << pca->getMean()[c];
		if( c == pca->getMean().Nrows() -1 ){
			OMAP << endl;
		}else{
			OMAP << " ";
		}
	}

	OMAP << pca->getEigenValues().Ncols()<< " ";
	ForIndex(c, pca->getEigenValues().Ncols()){
		OMAP << pca->getEigenValues()[c];
		if( c == pca->getEigenValues().Ncols() -1 ){
			OMAP << endl;
		}else{
			OMAP << " ";
		}
	}

	OMAP << pca->getNormalizedEigenVectors().Nrows()<< " " << pca->getNormalizedEigenVectors().Ncols()<< endl;
	ForIndex(i, pca->getNormalizedEigenVectors().Nrows()){
		ForIndex(j, pca->getNormalizedEigenVectors().Ncols()){
			OMAP << pca->getNormalizedEigenVectors()[i][j];
			if( j == pca->getNormalizedEigenVectors().Ncols() -1 ){
				OMAP << endl;
			}else{
				OMAP << " ";
			}
		}
	}

	OMAP.close();
}

