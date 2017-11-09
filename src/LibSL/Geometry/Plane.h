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
// LibSL::Geometry::Plane
// ------------------------------------------------------
//
// Plane class
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-11-15
// ------------------------------------------------------

#pragma once

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Math/Math.h>

#include <vector>

namespace LibSL {
  namespace Geometry {

    /*!

    \class Plane
    \brief Plane

    */
    template <uint T_NumDim>
    class Plane
    {
    public:

      typedef LibSL::Math::Tuple<float,T_NumDim> t_Vertex;

    protected:

      float     m_D;
      t_Vertex  m_N;

    public:

      Plane()
      {   }

      Plane(const t_Vertex& p,const t_Vertex& n)
      {
        m_N = normalize_safe(n);
        m_D = dot(m_N,p);
      }

      Plane(float d,const t_Vertex& n)
      {
        m_D = d;
        m_N = n;
      }

      Plane(const t_Vertex& p0,const t_Vertex& p1,const t_Vertex& p2)
      {
        m_N = normalize(cross((p1-p0),(p2-p0)));
        m_D = dot(m_N,p0);
      }

      t_Vertex o() const
      {
        return (m_D * m_N);
      }

      float distance(const t_Vertex& p) const
      {
        return (dot(p,m_N) - m_D);
      }

      t_Vertex closest(const t_Vertex& p) const
      {
        return ( p - dot(p-o(),n()) * n() );
      }

      float intersect(const t_Vertex& origin,const t_Vertex& dir) const
      {
        float l = dot(origin,m_N) - m_D;
        float d = dot(dir   ,m_N);
        if (Math::abs(d) < 1e-10f) {
          return (-1e16f);
        }
        float t = l / (- d);
        return t;
      }

      bool             isSimilar(const Plane& p,float dist_tolerance = 1e-3f,float nrm_tolerance = 0.1f) const
      { 
        int s = dot(p.m_N,m_N) >= 0.0 ? 1 : -1;
        return ((dot(p.m_N,m_N) >= 1.0f-nrm_tolerance) && Math::abs(m_D - s*p.m_D) <= dist_tolerance);
      }

      bool             isSimilarAround(const Plane& p,const t_Vertex& pt,float dist_tolerance = 1e-3f,float nrm_tolerance = 0.1f) const
      { 
        return ((dot(p.m_N,m_N) >= 1.0f-nrm_tolerance) && Math::abs(distance(pt) - p.distance(pt)) <= dist_tolerance);
      }

      bool             isSimilarAroundNonOriented(const Plane& p,const t_Vertex& pt,float dist_tolerance = 1e-3f,float nrm_tolerance = 0.1f) const
      { 
        Plane np = p;
        np.flip();
        return isSimilarAround(p,pt,dist_tolerance,nrm_tolerance) || isSimilarAround(np,pt,dist_tolerance,nrm_tolerance);
      }

      void flip()
      {
        m_D = - m_D;
        m_N = - m_N;
      }

      float&          d()       {return (m_D);}
      const float&    d() const {return (m_D);}

      t_Vertex&       n()       {return (m_N);}
      const t_Vertex& n() const {return (m_N);}

    };

  } //namespace LibSL::Geometry
} //namespace LibSL

// ------------------------------------------------------
