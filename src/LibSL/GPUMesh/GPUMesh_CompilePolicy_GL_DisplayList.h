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
// ----------------------------------------------
#pragma once

#if !defined(EMSCRIPTEN) && !defined(ANDROID)

#include <assert.h>

#ifdef USE_GLUX
#  include <glux.h>
#endif

#include "GPUMesh_VertexStorage.h"
#include "GPUMesh_GL_types.h"
#include "GPUMesh_GL_funcs.h"

namespace LibSL {
namespace GPUMesh {


  /// GL Mesh policy for display list

  template <class VertexFormat,class T_IndexType> 
  class CompilePolicy_GL_DisplayList
  {
  public:

    typedef struct { unsigned char data[LibSL::Mesh::MVF_sizeof<VertexFormat>::value]; } vertex_data;
    typedef LibSL::Mesh::FormatDescriptor<VertexFormat>                                  vertex_format_desc;
    typedef VertexStorageInterlaced<VertexFormat,vertex_data>               vertex_storage;
    typedef std::vector<T_IndexType>                                        index_storage;

    enum {nb_vertex_attrib = Loki::TL::Length<VertexFormat>::value};

    typedef struct
    {
      GLuint uiDP;
    }mesh_descriptor;

    static void     add_vertex(const vertex_storage& storage,int i)
    {
      assert(i >= 0 && i < (int)storage.m_Vertices.size());

      if (vertex_format_desc::has_normal)
      {
        (GLFuncs<typename vertex_format_desc::type_normal::type>::getNormalFct(vertex_format_desc::type_normal::components - 1))
          ((typename vertex_format_desc::type_normal::type *)&(storage.m_Vertices[i].data[vertex_format_desc::offset_normal]));
      }
      if (vertex_format_desc::has_color0)
      {
        (GLFuncs<typename vertex_format_desc::type_color0::type>::getColor0Fct(vertex_format_desc::type_color0::components - 1))
          ((typename vertex_format_desc::type_color0::type *)&(storage.m_Vertices[i].data[vertex_format_desc::offset_color0]));
      }
      if (vertex_format_desc::has_color1)
      {
        (GLFuncs<typename vertex_format_desc::type_color1::type>::getAttributeFct(vertex_format_desc::type_color1::components - 1))
          (4,(typename vertex_format_desc::type_color1::type *)&(storage.m_Vertices[i].data[vertex_format_desc::offset_color1]));
      }

      if (vertex_format_desc::has_texcoord0)
      {
        (GLFuncs<typename vertex_format_desc::type_texcoord0::type>::getTexCoordnFct(vertex_format_desc::type_texcoord0::components - 1))
          (GL_TEXTURE0_ARB,(typename vertex_format_desc::type_texcoord0::type *)&(storage.m_Vertices[i].data[vertex_format_desc::offset_texcoord0]));
      }

      if (vertex_format_desc::has_texcoord1)
      {
        (GLFuncs<typename vertex_format_desc::type_texcoord1::type>::getTexCoordnFct(vertex_format_desc::type_texcoord1::components - 1))
          (GL_TEXTURE1_ARB,(typename vertex_format_desc::type_texcoord1::type *)&(storage.m_Vertices[i].data[vertex_format_desc::offset_texcoord1]));
      }

      if (vertex_format_desc::has_texcoord2)
      {
        (GLFuncs<typename vertex_format_desc::type_texcoord2::type>::getTexCoordnFct(vertex_format_desc::type_texcoord2::components - 1))
          (GL_TEXTURE2_ARB,(typename vertex_format_desc::type_texcoord2::type *)&(storage.m_Vertices[i].data[vertex_format_desc::offset_texcoord2]));
      }

      if (vertex_format_desc::has_texcoord3)
      {
        (GLFuncs<typename vertex_format_desc::type_texcoord3::type>::getTexCoordnFct(vertex_format_desc::type_texcoord3::components - 1))
          (GL_TEXTURE3_ARB,(typename vertex_format_desc::type_texcoord3::type *)&(storage.m_Vertices[i].data[vertex_format_desc::offset_texcoord3]));
      }

      if (vertex_format_desc::has_texcoord4)
      {
        (GLFuncs<typename vertex_format_desc::type_texcoord4::type>::getTexCoordnFct(vertex_format_desc::type_texcoord4::components - 1))
          (GL_TEXTURE4_ARB,(typename vertex_format_desc::type_texcoord4::type *)&(storage.m_Vertices[i].data[vertex_format_desc::offset_texcoord4]));
      }

      if (vertex_format_desc::has_texcoord5)
      {
        (GLFuncs<typename vertex_format_desc::type_texcoord5::type>::getTexCoordnFct(vertex_format_desc::type_texcoord5::components - 1))
          (GL_TEXTURE5_ARB,(typename vertex_format_desc::type_texcoord5::type *)&(storage.m_Vertices[i].data[vertex_format_desc::offset_texcoord5]));
      }

      if (vertex_format_desc::has_texcoord6)
      {
        (GLFuncs<typename vertex_format_desc::type_texcoord6::type>::getTexCoordnFct(vertex_format_desc::type_texcoord6::components - 1))
          (GL_TEXTURE6_ARB,(typename vertex_format_desc::type_texcoord6::type *)&(storage.m_Vertices[i].data[vertex_format_desc::offset_texcoord6]));
      }

      if (vertex_format_desc::has_texcoord7)
      {
        (GLFuncs<typename vertex_format_desc::type_texcoord7::type>::getTexCoordnFct(vertex_format_desc::type_texcoord7::components - 1))
          (GL_TEXTURE7_ARB,(typename vertex_format_desc::type_texcoord7::type *)&(storage.m_Vertices[i].data[vertex_format_desc::offset_texcoord7]));
      }

      if (vertex_format_desc::has_position)
      {
        (GLFuncs<typename vertex_format_desc::type_position::type>::getVertexFct(vertex_format_desc::type_position::components - 1))
          ((typename vertex_format_desc::type_position::type *)&(storage.m_Vertices[i].data[vertex_format_desc::offset_position]));
      }
    }


    static void compile(
      int               flags,
      int               primtype,
      vertex_storage&   storage,
      index_storage&    indices,
      mesh_descriptor& _mesh)
    {
      // create display list
      _mesh.uiDP=glGenLists(1);
      glNewList(_mesh.uiDP,GL_COMPILE);
      glBegin(primtype);
      for (unsigned int i=0;i<storage.m_Vertices.size();i++)
      {
        add_vertex(storage,i);
      }
      glEnd();
      glEndList();

      // free memory
      storage.clear();
      indices.clear();
    }


    static void     bind(const mesh_descriptor& m)
    {
    }


    static void     unbind(const mesh_descriptor& m)
    {
    }


    static void     free(const mesh_descriptor& m)
    {
      glDeleteLists(m.uiDP,1);
    }


    static void     draw(const mesh_descriptor& m)
    {
      glCallList(m.uiDP);
    }
  };


  /// GL Mesh policy for display list / indexed mesh

  template <class T_VertexFormat,class T_IndexType> 
  class CompilePolicy_Indexed_GL_DisplayList : public CompilePolicy_GL_DisplayList<T_VertexFormat,T_IndexType>
  {
  public:
    typedef CompilePolicy_GL_DisplayList<T_VertexFormat,T_IndexType> parent_policy;
    typedef typename parent_policy::vertex_storage  vertex_storage;
    typedef typename parent_policy::index_storage   index_storage;
    typedef typename parent_policy::mesh_descriptor mesh_descriptor;
  public:
    static void compile(
      int               flags,
      int               primtype,
      vertex_storage&   storage,
      index_storage&    indices,
      mesh_descriptor& _mesh)
    {
      // create display list
      _mesh.uiDP=glGenLists(1);
      glNewList(_mesh.uiDP,GL_COMPILE);
      glBegin(primtype);
      unsigned int nb = (unsigned int)(storage.m_Vertices.size());
      for (unsigned int i=0;i<indices.size();i++) {
        add_vertex(storage,indices[i]);
      }
      glEnd();
      glEndList();

      // free memory
      storage.clear();
      indices.clear();
    }
  };


  /// GL Mesh with display list

  template <class VertexFormat> 
  class GPUMesh_GL_DisplayList
    : public GPUMesh_generic< CompilePolicy_GL_DisplayList , VertexFormat > {};

  /// GL Mesh with display list / indexed mesh

  template <class T_VertexFormat,class T_IndexType> 
  class GPUMesh_Indexed_GL_DisplayList
    : public GPUMesh_generic< CompilePolicy_Indexed_GL_DisplayList , T_VertexFormat , T_IndexType > {};


} // namespace GPUMesh
} // namespace LibSL

#endif
