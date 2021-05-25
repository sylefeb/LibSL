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
// LibSL::MeshRenderer
// ------------------------------------------------------
//
// Renderer for meshes
//
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-15-11
// ------------------------------------------------------

#pragma once

#include <LibSL/Mesh/Mesh.h>

namespace LibSL {
  namespace Mesh {

    template <typename T_VertexFormat>
    class MeshRenderer
    {
    public:

      typedef LibSL::Memory::Pointer::AutoPtr<MeshRenderer>                t_AutoPtr;
      typedef GPUMesh::GPUMesh_Indexed_VertexBuffer<T_VertexFormat,uint>   t_RenderMesh;

    protected:

      LibSL::Memory::Pointer::AutoPtr<t_RenderMesh> m_RenderMesh;

    public:

      MeshRenderer(LibSL::Mesh::TriangleMesh *m)
      {
        if (m->sizeOfVertexData() != MVF_sizeof<T_VertexFormat>::value) {
          throw Fatal("[MeshRenderer::MeshRenderer] - vertex format non compatible with vertex data! (sizeof(vf)=%d sizeof(vd)=%d)",MVF_sizeof<T_VertexFormat>::value,m->sizeOfVertexData());
        }
        // build mesh
        m_RenderMesh = LibSL::Memory::Pointer::AutoPtr<t_RenderMesh>(new t_RenderMesh());
        m_RenderMesh->begin(GPUMESH_TRIANGLELIST);
        ForIndex(n,m->numVertices()) {
          m_RenderMesh->vertex_raw( m->vertexDataAt(n),m->sizeOfVertexData());
        }
        ForIndex(n,m->numTriangles()) {
          m_RenderMesh->index(m->triangleAt(n)[0]);
          m_RenderMesh->index(m->triangleAt(n)[1]);
          m_RenderMesh->index(m->triangleAt(n)[2]);
        }
        m_RenderMesh->end();
      }

      void render()
      {
        m_RenderMesh->render();
      }

      void instantiate(uint count)
      {
        m_RenderMesh->instantiate(count);
      }

      void instantiate(uint count, uint base)
      {
        m_RenderMesh->instantiate(count, base);
      }

      GLuint VAO()
      {
        return m_RenderMesh->descriptor().uiVA;
      }
    };

  } //namespace LibSL::Mesh
} //namespace LibSL
