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
// LibSL::Geometry::Polygon
// ------------------------------------------------------
//
// Polygon class
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-11-15
// ------------------------------------------------------

#pragma once

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Math.h>
#include <LibSL/Geometry/Plane.h>
#include <LibSL/Geometry/AAB.h>

// ------------------------------------------------------

#define LIBSL_POLYGON_MAX_VERTICES  24     // TODO: template this
#define LIBSL_POLYGON_ON_EPSILON    1e-9
#define LIBSL_POLYGON_ON            0
#define LIBSL_POLYGON_FRONT         1
#define LIBSL_POLYGON_BACK          2
#define LIBSL_POLYGON_CUT          -1
#define LIBSL_POLYGON_SIDE_ON       0
#define LIBSL_POLYGON_SIDE_FRONT    1
#define LIBSL_POLYGON_SIDE_BACK     2

// ------------------------------------------------------

namespace LibSL {
  namespace Geometry {

    template <uint T_NumDim,typename T_Vertex>
    class Polygon
    {
    protected:

      typedef LibSL::Math::Tuple<float ,T_NumDim> t_fPos;
      typedef LibSL::Math::Tuple<double,T_NumDim> t_dPos;
      typedef LibSL::Math::v2f                    v2f;
      typedef LibSL::Math::v3f                    v3f;
      typedef LibSL::Math::v3d                    v3d;

    public:

      T_Vertex    m_Pts[LIBSL_POLYGON_MAX_VERTICES];
      uint        m_NumPts;

      Polygon()
      {
        m_NumPts = 0;
      }

      Polygon(const T_Vertex& p0,const T_Vertex& p1,const T_Vertex& p2)
      {
        m_NumPts = 0;
        add(p0); add(p1); add(p2);
      }

      Polygon(const T_Vertex& p0,const T_Vertex& p1,const T_Vertex& p2,const T_Vertex& p3)
      {
        m_NumPts = 0;
        add(p0); add(p1); add(p2); add(p3);
      }

      Polygon(const LibSL::Geometry::Plane<T_NumDim>& p,float size)
      {
        std::pair<v3f,v3f> uv = frame(p.n());
        T_Vertex p0           = T_Vertex(p.o() - uv.first * size - uv.second * size);
        T_Vertex p1           = T_Vertex(p.o() + uv.first * size - uv.second * size);
        T_Vertex p2           = T_Vertex(p.o() + uv.first * size + uv.second * size);
        T_Vertex p3           = T_Vertex(p.o() - uv.first * size + uv.second * size);
        m_NumPts = 0;
        add(p0); add(p1); add(p2); add(p3);
      }

      //! Add a vertex to the polygon
      void add(const T_Vertex& p)
      {
        sl_assert(m_NumPts < LIBSL_POLYGON_MAX_VERTICES);
        m_Pts[m_NumPts++] = p;
      }

      //! Returns polygon center
      T_Vertex center() const
      {
        T_Vertex ctr=0;
        if (m_NumPts < 1) {
          return (ctr);
        }
        ForIndex(i,m_NumPts) {
          ctr = ctr + m_Pts[i];
        }
        return (ctr / float(m_NumPts));
      }

      //! Returns polygon center of mass
      T_Vertex centroid() const
      {
        sl_assert( T_NumDim == 2 ); //// TODO: generalize!
        v2f ctr = 0;
        float a = 0;
        ForIndex(i,size()) {
          v2f p_i   = vertexAt(i);
          v2f p_ip1 = vertexAt((i+1)%size());
          a      += 0.5f * ( p_i[0]*p_ip1[1] - p_ip1[0]*p_i[1] );
          ctr[0] += ( p_i[0] + p_ip1[0] ) * (p_i[0]*p_ip1[1] - p_ip1[0]*p_i[1]);
          ctr[1] += ( p_i[1] + p_ip1[1] ) * (p_i[0]*p_ip1[1] - p_ip1[0]*p_i[1]);
        }
        if (Math::abs(a) > 1e-9f) {
          ctr = ctr / (6.0f * a);
        }
        return ctr;
      }

      //! Returns polygon bounding box
      AAB<T_NumDim> box() const
      {
        AAB<T_NumDim> bx;
        ForIndex(p,size()) {
          bx.addPoint( vertexAt(p) );
        }
        return bx;
      }

      //! Returns polygon area
      float area() const
      {
        if (m_NumPts < 3) {
          return (0);
        }
        float a = 0;
        for (int i=1;i<int(m_NumPts)-1;i++) {
          t_fPos u = LibSL::Math::Tuple<float,T_NumDim>((m_Pts[i  ]-m_Pts[0]));
          t_fPos v = LibSL::Math::Tuple<float,T_NumDim>((m_Pts[i+1]-m_Pts[0]));
          a       += length(cross(u,v));
        }
        return (a);
      }

      //! Returns polygon normal
      LibSL::Math::Tuple<float,3> normal() const
      {
        if (m_NumPts < 3) {
          return v3f(0);
        }
        v3f p0 = LibSL::Math::Tuple<float,3>(m_Pts[0]);
        v3f p1 = LibSL::Math::Tuple<float,3>(m_Pts[1]);
        v3f p2 = LibSL::Math::Tuple<float,3>(m_Pts[2]);
        return normalize_safe(cross(p1-p0,p2-p0));
      }

      //! Polygon cut
      int cut(
        const LibSL::Geometry::Plane<T_NumDim>& p,
        Polygon&     _front,
        Polygon&     _back) const
      {
        float                 dists[LIBSL_POLYGON_MAX_VERTICES+1];
        //int                   ndists;
        int                   sides[LIBSL_POLYGON_MAX_VERTICES+1];
        //int                   nsides;
        int                   counts[3];
        float                 dt;
        int                   numpt = m_NumPts;
        int                   i;
        T_Vertex              p1,p2,pi,pn;

        // get ready
        //ndists = 0;
        //nsides = 0;
        // init resulting polys
        _back .m_NumPts = 0;
        _front.m_NumPts = 0;
        // compute side of each points
        counts[0] = counts[1] = counts[2] = 0;
        i = 0;
        for (;i<numpt;i++) {
          dt         = dot( m_Pts[i],p.n() ) - p.d();
          dists[i]   = dt;
          if (dt  > LIBSL_POLYGON_ON_EPSILON) {
            sides[i] = LIBSL_POLYGON_SIDE_FRONT;
          } else if (dt < - LIBSL_POLYGON_ON_EPSILON) {
            sides[i] = LIBSL_POLYGON_SIDE_BACK;
          } else {
            sides[i] = LIBSL_POLYGON_SIDE_ON;
          }
          counts[sides[i]]++;
        }
        sides[i] = sides[0];
        dists[i] = dists[0];
        if (counts[LIBSL_POLYGON_SIDE_ON] > 2) {
          _front.m_NumPts   = m_NumPts;
          ForIndex(i,m_NumPts) {
            _front.m_Pts[i] = m_Pts[i];
          }
          _back.m_NumPts    = m_NumPts;
          ForIndex(i,m_NumPts) {
            _back.m_Pts[i]  = m_Pts[i];
          }
          return (LIBSL_POLYGON_ON);
        }
        if (counts[LIBSL_POLYGON_SIDE_FRONT] == 0) {
          _back.m_NumPts    = m_NumPts;
          ForIndex(i,m_NumPts) {
            _back.m_Pts[i]  = m_Pts[i];
          }
          return (LIBSL_POLYGON_BACK);
        }
        if (counts[LIBSL_POLYGON_SIDE_BACK] == 0) {
          _front.m_NumPts   =  m_NumPts;
          ForIndex(i,m_NumPts) {
            _front.m_Pts[i] = m_Pts[i];
          }
          return (LIBSL_POLYGON_FRONT);
        }
        // compute new vertices
        ForIndex(i,numpt) {
          pi = m_Pts[i];
          if (sides[i] == LIBSL_POLYGON_SIDE_ON) {
            _front.add(pi);
            _back .add(pi);
            continue;
          }
          if (sides[i] == LIBSL_POLYGON_SIDE_FRONT) {
            _front.add(pi);
          } else {
            _back.add(pi);
          }
          if (sides[i+1] == LIBSL_POLYGON_SIDE_ON || sides[i+1] == sides[i]) {
            continue;
          }
          // compute cut vertex
          p1  = m_Pts[i];
          p2  = m_Pts[(i+1)%numpt];
          dt  = float(dists[i]) / float(dists[i]-dists[i+1]);
          pn  = p1 + (p2 - p1) * dt;
          _front.add(pn);
          _back .add(pn);
        }
        return (LIBSL_POLYGON_CUT);
      }

      //! True if polygon is empty or degenerated
      bool isEmpty() const {return (m_NumPts < 3);}

      uint size()    const {return (m_NumPts);}

      const T_Vertex& vertexAt(uint n) const {sl_assert(n>=0&&n<m_NumPts); return (m_Pts[n]);}
      T_Vertex&       vertexAt(uint n)       {sl_assert(n>=0&&n<m_NumPts); return (m_Pts[n]);}

      Polygon<T_NumDim,T_Vertex> operator + (const T_Vertex& v)
      {
        Polygon<T_NumDim,T_Vertex> p = (*this);
        ForIndex(i,size()) {
          p.m_Pts[i] = m_Pts[i] + v;
        }
        return p;
      }

    };

  } // namespace LibSL::Geometry
} // namespace LibSL
