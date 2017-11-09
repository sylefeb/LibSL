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
// LibSL::GPUHelpers::Bezier
// ------------------------------------------------------
//
// Bezier segments and patches
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2007-06-06
// ------------------------------------------------------

#pragma once

#include <LibSL/Errors/Errors.h>
#include <LibSL/System/Types.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Math/Matrix4x4.h>
using namespace LibSL::System::Types;

// ------------------------------------------------------

namespace LibSL {
  namespace Geometry {
    namespace Bezier {

      template <typename T_Vertex>
      T_Vertex bezierSegmentQuadratic(float u,const T_Vertex& p0,const T_Vertex& p1,const T_Vertex& p2)
      {
        float    u2 = u  * u;
        T_Vertex v  = u2*(p0-2.0f*p1+p2) + 2.0f*u*(p1-p0) + p0;
        return (v);
      }

      template <typename T_Vertex>
      T_Vertex bezierSegmentQuadratic_tangent(float u,const T_Vertex& p0,const T_Vertex& p1,const T_Vertex& p2)
      {
        T_Vertex v  = 2.0f*u*(p0-2.0f*p1+p2) + 2.0f*(p1-p0);
        return (v);
      }

      template <typename T_Vertex>
      T_Vertex bezierSegmentCubic(float u,const T_Vertex& p0,const T_Vertex& p1,const T_Vertex& p2,const T_Vertex& p3)
      {
        float    u2 = u  * u;
        float    u3 = u2 * u;
        T_Vertex v  = u3*(p3-3.0f*p2+3.0f*p1-p0) + 3.0f*u2*(p2-2.0f*p1+p0) + 3.0f*u*(p1-p0) + p0;
        return (v);
      }

      template <typename T_Vertex>
      T_Vertex bezierSegmentCubic_tangent(float u,const T_Vertex& p0,const T_Vertex& p1,const T_Vertex& p2,const T_Vertex& p3)
      {
        float    u2 = u  * u;
        T_Vertex v  = 3.0f*u2*(p3-3.0f*p2+3.0f*p1-p0) + 6.0f*u*(p2-2.0f*p1+p0) + 3.0f*(p1-p0);
        return (v);
      }

      template <typename T_Vertex>
      T_Vertex bezierPatchQuadratic(float u,float v,const LibSL::Memory::Array::Array2D<T_Vertex>& pts,uint offsi = 0,uint offsj = 0)
      {
        T_Vertex p;
        T_Vertex c[3];
        ForIndex(j,3) {
          T_Vertex p0 = pts.at(offsi  ,offsj+j);
          T_Vertex p1 = pts.at(offsi+1,offsj+j);
          T_Vertex p2 = pts.at(offsi+2,offsj+j);
          c[j]        = bezierSegmentQuadratic(u,p0,p1,p2);
        }
        p             = bezierSegmentQuadratic(v,c[0],c[1],c[2]);
        return (p);
      }

      template <typename T_Vertex>
      T_Vertex bezierPatchQuadratic_normal(float u,float v,const LibSL::Memory::Array::Array2D<T_Vertex>& pts,uint offsi = 0,uint offsj = 0)
      {
        T_Vertex tu,tv;
        T_Vertex c[3];
        
        ForIndex(i,3) {
          T_Vertex p0 = pts.at(offsi+i,offsj  );
          T_Vertex p1 = pts.at(offsi+i,offsj+1);
          T_Vertex p2 = pts.at(offsi+i,offsj+2);
          c[i]        = bezierSegmentQuadratic(v,p0,p1,p2);
        }
        tu            = bezierSegmentQuadratic_tangent(u,c[0],c[1],c[2]);

        ForIndex(j,3) {
          T_Vertex p0 = pts.at(offsi  ,offsj+j);
          T_Vertex p1 = pts.at(offsi+1,offsj+j);
          T_Vertex p2 = pts.at(offsi+2,offsj+j);
          c[j]        = bezierSegmentQuadratic(u,p0,p1,p2);
        }
        tv            = bezierSegmentQuadratic_tangent(v,c[0],c[1],c[2]);

        return (cross(normalize_safe(tu),normalize_safe(tv)));
      }

      template <typename T_Vertex>
      class BezierSurfaceQuadratic
      {
      protected:

        const LibSL::Memory::Array::Array2D<T_Vertex>& m_ControlPoints;
      
        void tesselatePatch(
          uint resx ,uint resy,
          uint offsi,uint offsj,
          LibSL::Memory::Array::Array2D<std::pair<T_Vertex,T_Vertex> >& tess)
        {
          // reallocate grid only if necessary
          if (tess.xsize() != resx || tess.ysize() != resy) {
            tess.erase();
            tess.allocate(resx,resy);
          }
          // tesselate
          ForArray2D(tess,i,j) {
            float fi            = i / float(tess.xsize()-1);
            float fj            = j / float(tess.ysize()-1);
            tess.at(i,j).first  = Bezier::bezierPatchQuadratic       (fi,fj,m_ControlPoints,offsi,offsj);
            tess.at(i,j).second = Bezier::bezierPatchQuadratic_normal(fi,fj,m_ControlPoints,offsi,offsj);
          }
        }

        std::pair<float,float> computeError(
          uint offsi,uint offsj,
          const LibSL::Memory::Array::Array2D<std::pair<T_Vertex,T_Vertex> >& tess)
        {
          float erru=0,errv=0;
          for (uint j=0;j<tess.ysize()-1;j++) {
            for (uint i=0;i<tess.xsize()-1;i++) {
              float fi     = (i       ) / float(tess.xsize()-1);
              float fj     = (j       ) / float(tess.ysize()-1);
              float fmi    = (i + 0.5f) / float(tess.xsize()-1);
              float fmj    = (j + 0.5f) / float(tess.ysize()-1);
              T_Vertex  mx = 0.5f * (tess.at(i,j).first + tess.at(i+1,j).first);
              T_Vertex  my = 0.5f * (tess.at(i,j).first + tess.at(i,j+1).first);
              T_Vertex  vx = Bezier::bezierPatchQuadratic(fmi,fj,m_ControlPoints,offsi,offsj);
              T_Vertex  vy = Bezier::bezierPatchQuadratic(fi,fmj,m_ControlPoints,offsi,offsj);
              erru         = Math::max(erru, length(vx-mx) );
              errv         = Math::max(errv, length(vy-my) );
            }
          }
          return (std::make_pair(erru,errv));
        }

      public:
      
        BezierSurfaceQuadratic(const LibSL::Memory::Array::Array2D<T_Vertex>& cp) : m_ControlPoints(cp)
        {  }

        void tesselateFixed(
          uint                                       resx,
          uint                                       resy,
          std::vector<T_Vertex>&                    _vertices,
          std::vector<T_Vertex>&                    _normals,
          std::vector<LibSL::Math::Tuple<int,3> >&  _triangles)
        {
          LibSL::Memory::Array::Array2D<std::pair<T_Vertex,T_Vertex> > tess(resx,resy);
          // tesselate
          for (uint kj=0 ; kj+2<m_ControlPoints.ysize() ; kj+=2) {
            for (uint ki=0 ; ki+2<m_ControlPoints.xsize() ; ki+=2) {
              // tesselate sub-patches
              tesselatePatch(resx,resy,ki,kj, tess);
              // add vertices
              uint v_start = _vertices.size();
              ForArray2D(tess,i,j) {
                _vertices.push_back( tess.at(i,j).first );
                _normals .push_back( tess.at(i,j).second);
              }
              // add triangles
              for (uint j=0;j<tess.ysize()-1;j++) {
                for (uint i=0;i<tess.xsize()-1;i++) {
                  _triangles.push_back(LibSL::Math::V3I(
                    v_start+(i  )+(j  )*tess.xsize(),
                    v_start+(i+1)+(j  )*tess.xsize(),
                    v_start+(i+1)+(j+1)*tess.xsize()));
                  _triangles.push_back(LibSL::Math::V3I(
                    v_start+(i  )+(j  )*tess.xsize(),
                    v_start+(i+1)+(j+1)*tess.xsize(),
                    v_start+(i  )+(j+1)*tess.xsize()));
                }
              }
            } // ki
          } // kj
        }

        void tesselateAdaptive(
          uint                                       resx,
          uint                                       resy,
          float                                      thres,
          std::vector<T_Vertex>&                    _vertices,
          std::vector<T_Vertex>&                    _normals,
          std::vector<T_Vertex>&                    _uvs,         //// NOT YET IMPLEMENTED
          std::vector<LibSL::Math::Tuple<int,3> >&  _triangles)
        {
          LibSL::Memory::Array::Array2D<std::pair<T_Vertex,T_Vertex> > tess(resx,resy);
          // tesselate
          for (uint kj=0 ; kj+2<m_ControlPoints.ysize() ; kj+=2) {
            for (uint ki=0 ; ki+2<m_ControlPoints.xsize() ; ki+=2) {
              // tesselate sub-patches
              uint rx = resx;
              uint ry = resy;
              do {
                tesselatePatch(rx,ry,ki,kj,tess);
                std::pair<float,float> err = computeError(ki,kj,tess);
                if (err.first > thres) {
                  rx *= 2;
                }
                if (err.second > thres) {
                  ry *= 2;
                }
              } while ((rx > tess.xsize() || ry > tess.ysize()) && rx < 64 && ry < 64 );
              // add vertices
              uint v_start = uint(_vertices.size());
              ForArray2D(tess,i,j) {
                _vertices.push_back( tess.at(i,j).first );
                _normals .push_back( tess.at(i,j).second);
                _uvs     .push_back( 0 ); /////////////////// 
              }
              // add triangles
              for (uint j=0;j<tess.ysize()-1;j++) {
                for (uint i=0;i<tess.xsize()-1;i++) {
                  _triangles.push_back(LibSL::Math::V3I(
                    v_start+(i  )+(j  )*tess.xsize(),
                    v_start+(i+1)+(j  )*tess.xsize(),
                    v_start+(i+1)+(j+1)*tess.xsize()));
                  _triangles.push_back(LibSL::Math::V3I(
                    v_start+(i  )+(j  )*tess.xsize(),
                    v_start+(i+1)+(j+1)*tess.xsize(),
                    v_start+(i  )+(j+1)*tess.xsize()));
                }
              }
            } // ki
          } // kj
        }
      };

    } // namespace LibSL::Geometry::Bezier
  } // namespace LibSL::Geometry
} // namespace LibSL

// --------------------------------------------------------
