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
// LibSL::Geometry::ImplicitShape
// ------------------------------------------------------
//
//  Produce a mesh from an implicit function
//    the function must be defined in [0..1]^3
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2009-01-11
// ------------------------------------------------------

#pragma once

#include <LibSL/LibSL.common.h>

#include <LibSL/Memory/Pointer.h>
#include <LibSL/Memory/Array.h>
#include <LibSL/Math/Matrix4x4.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Math/Quaternion.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Mesh/Mesh.h>
#include <LibSL/Mesh/MeshFormat_mesh.h>
#include <LibSL/Geometry/AAB.h>

#include <LibSL/Geometry/MarchingCubes.h>

namespace LibSL {
  namespace Geometry {

    class ImplicitShape 
    {

    public:

      typedef struct
      {
        LibSL::Math::v3f pos;
        LibSL::Math::v3f nrm;
        LibSL::Math::v2f uv;
      } t_VertexData;

      typedef MVF3(LibSL::Mesh::mvf_position_3f,LibSL::Mesh::mvf_normal_3f,LibSL::Mesh::mvf_texcoord0_2f) t_VertexFormat;
      typedef LibSL::Mesh::TriangleMesh_generic<t_VertexData>                                             t_Mesh;

      class ImplicitFunction
      {
      public:
        virtual float operator()(const LibSL::Math::v3f& p) const = 0;
      };

    protected:

      const ImplicitFunction&                        m_Implicit;
      LibSL::Memory::Pointer::AutoPtr<MarchingCubes::MarchingCubes> m_MC;
      int                                            m_Resolution;
      LibSL::Geometry::AAB<3>                        m_Box;

      void                           initGrid();
      void                           initMarchingCube();

      /**
      Computes the distance field (slow)
      */
      void            init();

    public:
      
      ImplicitShape(
        const ImplicitFunction& f,
        int           resolution,
        const LibSL::Geometry::AAB<3>& box = LibSL::Geometry::AAB<3>(0.0f,1.0f) )
        : m_Implicit(f), m_Resolution(resolution), m_Box(box) { }
      ~ImplicitShape();

      /**
      Generates the mesh from an iso value. 
      
        - First call is slow since it evaluates the function.
        - Subsequent calls are faster: the function may be called several times 
          for fast isovalue exploration.
        - Mesh is embbeded within box given as parameter to the constructor
        - Returns NULL if there is no intersection with the isosurface.
      */
      t_Mesh *generateShape(float iso = 0.5f);

    };

  } //namespace LibSL::Geometry
} //namespace LibSL


// ------------------------------------------------------
