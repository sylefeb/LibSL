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
// LibSL::GPUHelpers::Shapes
// ------------------------------------------------------
//
// Simple shape drawers  (cube, cylinders, ...)
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2007-02-16
// ------------------------------------------------------

#pragma once

#include <LibSL/LibSL.common.h>

#include <LibSL/GPUTex/GPUTex.h>
#include <LibSL/GPUMesh/GPUMesh.h>
#include <LibSL/Errors/Errors.h>
#include <LibSL/System/Types.h>
#include <LibSL/Memory/Pointer.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Matrix4x4.h>
#include <LibSL/Geometry/Polygon.h>

using namespace LibSL::System::Types;

#include <vector>

// ------------------------------------------------------

namespace LibSL {
  namespace GPUHelpers {
    namespace Shapes {

      using namespace LibSL::Mesh;

      typedef MVF2(mvf_position_3f,mvf_normal_3f)                                     mvf_shape;
      typedef LibSL::GPUMesh::GPUMesh_VertexBuffer<mvf_shape>                         Shape;
      typedef LibSL::GPUMesh::GPUMesh_Indexed_VertexBuffer<mvf_shape,ushort>          IndexedShape;

      typedef MVF3(mvf_position_3f,mvf_normal_3f,mvf_texcoord0_4f)                    mvf_textured_shape;
      typedef LibSL::GPUMesh::GPUMesh_VertexBuffer<mvf_textured_shape>                TexturedShape;
      typedef LibSL::GPUMesh::GPUMesh_Indexed_VertexBuffer<mvf_textured_shape,ushort> IndexedTexturedShape;

      class LIBSL_DLL Box
      {
      protected:
        static uint s_UsageCounter;
        void        lock();
        void        unlock();
      public:
        Box()  { lock();   }
        ~Box() { unlock(); }
        void   render();
      };

      class LIBSL_DLL Square
      {
      protected:
        static uint s_UsageCounter;
        void        lock();
        void        unlock();
      public:
        Square()  { lock();   }
        ~Square() { unlock(); }
        void   render();
      };

      class LIBSL_DLL Polygon
      {
      protected:
        LibSL::Memory::Pointer::AutoPtr<Shape> m_Shape;
      public:
        Polygon(const LibSL::Geometry::Polygon<3,LibSL::Math::Tuple<float,3> >& poly);
        void   render();
      };

      class LIBSL_DLL Grid
      {
      protected:
        LibSL::Memory::Pointer::AutoPtr<IndexedShape> m_Shape;
      public:
        Grid(const LibSL::Memory::Array::Array2D<std::pair<LibSL::Math::v3f,LibSL::Math::v3f> >& grid);
        void   render();
      };

    } //namespace LibSL::GPUHelpers::Shape
  } //namespace LibSL::GPUHelpers
} //namespace LibSL

// ------------------------------------------------------
