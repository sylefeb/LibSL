/* --------------------------------------------------------------------
Author: Sylvain Lefebvre    sylvain.lefebvre@sophia.inria.fr

Simple Library for Graphics (LibSL)

This software is a computer program whose purpose is to offer a set of
tools to simplify programming real-time computer graphics applications
under OpenGL and DirectX.

This software is governed by the CeCILL-C license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL-C
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-C license and that you accept its terms.
-------------------------------------------------------------------- */
// ------------------------------------------------------
// LibSL::GPUHelpers::BezierCurve
// ------------------------------------------------------
//
// Hermite curve
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2004-06-01
//                    included in LibSL 2007-06-06
// ------------------------------------------------------

#pragma once

#include <LibSL/Errors/Errors.h>
#include <LibSL/System/Types.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Matrix4x4.h>
using namespace LibSL::System::Types;

// ------------------------------------------------------

namespace LibSL {
	namespace Geometry {

		template <typename T_Vertex,int T_ArcLenSampling = 64>
		class BezierCurve
		{
		public:

			class ControlPoint
			{
			public:
				T_Vertex t0;
				T_Vertex t1;
				T_Vertex p;

				ControlPoint() {}
				ControlPoint(const T_Vertex& tt0,const T_Vertex& pp,const T_Vertex& tt1) // point p and the two control points t0, t1
					: t0(tt0), t1(tt1), p(pp) {}
			};


		protected:

			std::vector<ControlPoint>            m_ControlPoints;
			T_Vertex                             m_In;
			T_Vertex                             m_Out;
			std::vector<std::pair<float,float> > m_Param2Arc;
			float                                m_ArcLen;
			bool                                 m_Ready;
			bool                                 m_Dirty;
			bool                                 m_Closed;

			int locateParam(float q)             const;
			int locateParam(float q,int l,int r) const;

			int locateArc(float q)               const;
			int locateArc(float q,int l,int r)   const;

			float moduloParam(float p) const {return (p);}
			float moduloArc(float a)   const {return (a);}

		public:

			BezierCurve();

			// creation
			void   addControlPoint(const T_Vertex& t0,const T_Vertex& p,const T_Vertex& t1);
			void   computeArcLength();

			// evaluation
			T_Vertex pointAt     (float)   const;
			T_Vertex pointAtArc  (float p)        {return (pointAt  (arc2param(p)));}
			T_Vertex tangentAt   (float)   const;
			T_Vertex tangentAtArc(float p)        {return (tangentAt(arc2param(p)));}
			float param2arc(float);
			float arc2param(float);

			// tesselation
			void  tesselate(uint res,LibSL::Memory::Array::Array<T_Vertex>&);

			// size (number of control points)
			uint  size()          const {return (m_ControlPoints.size());}

			// clear (remove all control point)
			void  clear();

			// arclenth
			float arclength()           { if (m_Dirty) {computeArcLength();} return (m_ArcLen);}

			// is the curve ready?
			bool  isReady()       const {return (m_ControlPoints.size() >= 2);}

			// in/out points
			const T_Vertex& in()  const {return (m_In);}
			const T_Vertex& out() const {return (m_Out);}

			// control points
			std::vector<ControlPoint>& controlPoints() { m_Dirty = true; return m_ControlPoints; }

      // closed loop
      bool isClosed() const  { return m_Closed; }
      void setClosed(bool b) { m_Dirty = m_Closed ^ b; m_Closed = b; }

		};

	} // namespace LibSL::Geometry
} // namespace LibSL

// --------------------------------------------------------

#include <LibSL/Geometry/BezierCurve.inl>

// --------------------------------------------------------
