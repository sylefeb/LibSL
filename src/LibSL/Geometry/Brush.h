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
// LibSL::Geometry::Brush
// ------------------------------------------------------
//
// Brush class
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-06-06
// ------------------------------------------------------

#pragma once

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Math/Math.h>
#include <LibSL/Geometry/Plane.h>
#include <LibSL/Geometry/Polygon.h>

#include <vector>

// ------------------------------------------------------

#define LIBSL_BRUSH_PLANE_SIZE 1e6f

// ------------------------------------------------------

namespace LibSL {
  namespace Geometry {

    /*!

    \class Brush
    \brief Brush

    */
    class Brush
    {
    public:

      typedef LibSL::Math::v3f                     t_Vertex;
      typedef LibSL::Math::Tuple<float,6>          t_v6f;
      typedef LibSL::Geometry::Polygon<3,t_Vertex> t_Polygon;
      typedef LibSL::Geometry::Plane<3>            t_Plane;

    protected:

      std::vector<t_Polygon>                       m_Polygons;
      std::vector<t_Plane>                         m_Planes;
      std::vector<std::string>                     m_Materials;
      std::vector<t_v6f>                           m_UVTrsfms;

    public:

      Brush()
      {   }

      void addPlaneCW(const t_Plane& pl,const std::string& mat = "",const t_v6f& uvtrsf = 0)
      {
        // cut existing polygons by this plane
        uint n = 0;
        while (n < m_Polygons.size()) {
          t_Polygon f,b;
          switch (m_Polygons[n].cut(pl, f,b)) 
          {
          case LIBSL_POLYGON_ON:
            // do nothing
            n ++; // next
            break;
          case LIBSL_POLYGON_BACK:
            // erase current
            m_Polygons [n] = m_Polygons.back();
            m_Planes   [n] = m_Planes.back();
            m_Materials[n] = m_Materials.back();
            m_UVTrsfms [n] = m_UVTrsfms.back();
            m_Polygons     .pop_back();
            m_Planes       .pop_back();
            m_Materials    .pop_back();
            m_UVTrsfms     .pop_back();
            break;
          case LIBSL_POLYGON_CUT:
          case LIBSL_POLYGON_FRONT:
            // keep front only
            m_Polygons[n] = f;
            n ++; // next
            break;
          }
        }
        try {
          // create polygon for new plane
          t_Polygon p(pl,LIBSL_BRUSH_PLANE_SIZE);
          // cut by others
          ForArray(m_Planes,n) {
            t_Polygon f,b;
            switch (p.cut(m_Planes[n], f,b)) 
            {
            case LIBSL_POLYGON_ON:
              // do nothing
              break;
            case LIBSL_POLYGON_BACK:
              // forget about this plane
              return;
              break;
            case LIBSL_POLYGON_CUT:
            case LIBSL_POLYGON_FRONT:
              // keep front only
              p = f;
              break;
            }
          }
          m_Polygons .push_back(p);
          m_Planes   .push_back(pl);
          m_Materials.push_back(mat);
          m_UVTrsfms .push_back(uvtrsf);
        } catch (LibSL::Errors::Fatal &f) {
          std::cerr << f.message() << std::endl;
        }
        sl_assert(m_Planes.size() == m_Polygons .size());
        sl_assert(m_Planes.size() == m_Materials.size());
        sl_assert(m_Planes.size() == m_UVTrsfms .size());
      }

      void addPlaneCCW(const t_Plane& pl,const std::string& mat = "",const t_v6f& uvtrsf = 0)
      {
        // cut existing polygons by this plane
        uint n = 0;
        while (n < m_Polygons.size()) {
          t_Polygon f,b;
          switch (m_Polygons[n].cut(pl, f,b)) 
          {
          case LIBSL_POLYGON_ON:
            // do nothing
            n ++; // next
            break;
          case LIBSL_POLYGON_FRONT:
            // erase current
            m_Polygons [n] = m_Polygons.back();
            m_Planes   [n] = m_Planes.back();
            m_Materials[n] = m_Materials.back();
            m_UVTrsfms [n] = m_UVTrsfms.back();
            m_Polygons     .pop_back();
            m_Planes       .pop_back();
            m_Materials    .pop_back();
            m_UVTrsfms     .pop_back();
            break;
          case LIBSL_POLYGON_CUT:
          case LIBSL_POLYGON_BACK:
            // keep back only
            m_Polygons[n] = b;
            n ++; // next
            break;
          }
        }
        try {
          // create polygon for new plane
          t_Polygon p(pl,LIBSL_BRUSH_PLANE_SIZE);
          // cut by others
          ForArray(m_Planes,n) {
            t_Polygon f,b;
            switch (p.cut(m_Planes[n], f,b)) 
            {
            case LIBSL_POLYGON_ON:
              // do nothing
              break;
            case LIBSL_POLYGON_FRONT:
              // forget about this plane
              return;
              break;
            case LIBSL_POLYGON_CUT:
            case LIBSL_POLYGON_BACK:
              // keep back only
              p = b;
              break;
            }
          }
          m_Polygons .push_back(p);
          m_Planes   .push_back(pl);
          m_Materials.push_back(mat);
          m_UVTrsfms .push_back(uvtrsf);
        } catch (LibSL::Errors::Fatal &f) {
          std::cerr << f.message() << std::endl;
        }
        sl_assert(m_Planes.size() == m_Polygons .size());
        sl_assert(m_Planes.size() == m_Materials.size());
        sl_assert(m_Planes.size() == m_UVTrsfms .size());
      }

      const std::vector<t_Polygon>&   polygons()  const { return (m_Polygons); }
      std::vector<t_Polygon>&         polygons()        { return (m_Polygons); }

      const std::vector<std::string>& materials() const { return (m_Materials); }
      std::vector<std::string>&       materials()       { return (m_Materials); }

      const std::vector<t_Plane>&     planes()    const { return (m_Planes); }
      std::vector<t_Plane>&           planes()          { return (m_Planes); }

      const std::vector<t_v6f>&       uvTrsfms()  const { return (m_UVTrsfms); }
      std::vector<t_v6f>&             uvTrsfms()        { return (m_UVTrsfms); }

      bool  isClosed()
      {
        ForArray(m_Polygons,p) {
          ForArray(m_Polygons[p],i) {
            if ( m_Polygons[p].vertexAt(i).oneGteq( LIBSL_BRUSH_PLANE_SIZE)
              || m_Polygons[p].vertexAt(i).oneLteq(-LIBSL_BRUSH_PLANE_SIZE)) {
                return (false);
            }
          }
        }
        return (true);
      }

    };

  } //namespace LibSL::Geometry
} //namespace LibSL

// ------------------------------------------------------
