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

// #pragma message("Including VBO GL 4.x")

#ifdef USE_GLUX
# include <glux.h>
#include "GL_ARB_vertex_buffer_object.h"
#include "GL_ARB_vertex_array_object.h"
GLUX_LOAD(GL_ARB_vertex_buffer_object)
GLUX_LOAD(GL_ARB_vertex_array_object)
#endif

#include <LibSL/Mesh/VertexFormat.h>

#include "GPUMesh_VertexStorage.h"
#include "GPUMesh_GL_types.h"
#include "GPUMesh_GL_funcs.h"
#include "GPUMesh_primitives.h"

#define BUFFER_OFFSET(o) ((reinterpret_cast<char*>(static_cast<size_t>(o))) )

namespace LibSL {
namespace GPUMesh {

  using namespace LibSL::Mesh;

  namespace gl4 {
  /// Attribute locations

    template <class MVF_base_attribute> class mvf_attrib_location;
    template < > class mvf_attrib_location<MVF_BASE_POSITION>     { public: enum { value =  0};  };
    template < > class mvf_attrib_location<MVF_BASE_NORMAL>       { public: enum { value =  2};  };
    template < > class mvf_attrib_location<MVF_BASE_COLOR0>       { public: enum { value =  3};  };
    template < > class mvf_attrib_location<MVF_BASE_COLOR1>       { public: enum { value =  4};  };
    template < > class mvf_attrib_location<MVF_BASE_TEXCOORD0>    { public: enum { value =  8};  };
    template < > class mvf_attrib_location<MVF_BASE_TEXCOORD1>    { public: enum { value =  9};  };
    template < > class mvf_attrib_location<MVF_BASE_TEXCOORD2>    { public: enum { value = 10};  };
    template < > class mvf_attrib_location<MVF_BASE_TEXCOORD3>    { public: enum { value = 11};  };
    template < > class mvf_attrib_location<MVF_BASE_TEXCOORD4>    { public: enum { value = 12};  };
    template < > class mvf_attrib_location<MVF_BASE_TEXCOORD5>    { public: enum { value = 13};  };
    template < > class mvf_attrib_location<MVF_BASE_TEXCOORD6>    { public: enum { value = 14};  };
    template < > class mvf_attrib_location<MVF_BASE_TEXCOORD7>    { public: enum { value = 15};  };
    //template < > class mvf_attrib_location<MVF_BASE_BINORMAL>     { public: enum { value = 12};  };
    //template < > class mvf_attrib_location<MVF_BASE_TANGENT>      { public: enum { value = 13};  };
    //template < > class mvf_attrib_location<MVF_BASE_BONE_INDICES> { public: enum { value = 14};  };
    //template < > class mvf_attrib_location<MVF_BASE_BONE_WEIGHTS> { public: enum { value = 15};  };

  }

  /// GL mesh policy for VBO

  template <class VertexFormat,class T_IndexType>
  class CompilePolicy_GL_VBO
  {
  public:

    typedef struct { unsigned char data[LibSL::Mesh::MVF_sizeof<VertexFormat>::value]; } vertex_data;
    typedef LibSL::Mesh::FormatDescriptor<VertexFormat>                                  vertex_format_desc;
    typedef VertexStorageSeparateAttributes<VertexFormat,vertex_data>       vertex_storage;
    typedef std::vector<T_IndexType>                                        index_storage;

    enum {nb_vertex_attrib = Loki::TL::Length<VertexFormat>::value};

    class mesh_descriptor
    {
    public:
      GLuint uiVB;
      GLuint uiVA;
      int    iVertices;
      int    iPrimType;
      int    offsets[nb_vertex_attrib];
    };


    static void compile(
      int               flags,
      int               primtype,
      vertex_storage&   storage,
      index_storage&    indices,
      mesh_descriptor& _mesh)
    {
      const int       vfsz=Loki::TL::Length<VertexFormat>::value;

      int nbv=storage.m_iVertices;
      _mesh.iVertices=nbv;
      _mesh.iPrimType=primtype;
      // compute offsets
      for (int i=0;i<vfsz;i++)
      {
        int offset=vertex_format_desc::offset(i);
        _mesh.offsets[i]=offset*nbv;
      }
      // create VAO
      glGenVertexArrays(1, &_mesh.uiVA);
      glBindVertexArray(_mesh.uiVA);
      // create VBO
      glGenBuffersARB(1,&_mesh.uiVB);
      glBindBufferARB(GL_ARRAY_BUFFER_ARB,_mesh.uiVB);
      // send data to graphics card
      glBufferDataARB(GL_ARRAY_BUFFER_ARB,nbv*vertex_format_desc::size_of,NULL,(flags & GPUMESH_DYNAMIC ? GL_DYNAMIC_DRAW_ARB : GL_STATIC_DRAW_ARB));
      for (int i=0;i<vertex_storage::nb_attributes;i++)
      {
        glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
          _mesh.offsets[i],
          storage.m_Attributes[i].size(),
          (storage.m_Attributes[i].empty() ? nullptr : &(storage.m_Attributes[i].front())));
      }
      glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
      glBindVertexArray(0);
      // free memory
      if (!(flags & GPUMESH_DYNAMIC)) {
        storage.clear();
      }
      indices.clear();
    }

    static void update(
      vertex_storage&   storage,
      mesh_descriptor& _mesh)
    {
      glBindVertexArray(_mesh.uiVA);
      glBindBufferARB(GL_ARRAY_BUFFER_ARB,_mesh.uiVB);
      for (int i=0;i<vertex_storage::nb_attributes;i++) {
        glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
          _mesh.offsets[i],
          storage.m_Attributes[i].size(),
          &(storage.m_Attributes[i].front()));
      }
      glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
      glBindVertexArray(0);
    }

    static void     bind(const mesh_descriptor& m)
    {
      glBindVertexArray(m.uiVA);
      glBindBufferARB(GL_ARRAY_BUFFER_ARB,m.uiVB);
      if (vertex_format_desc::has_position) {
        glVertexAttribPointerARB(gl4::mvf_attrib_location<MVF_BASE_POSITION>::value,vertex_format_desc::type_position::components,GLTypes<typename vertex_format_desc::type_position::type>::gl_define,
          false,0,BUFFER_OFFSET(m.offsets[vertex_format_desc::idx_position]));
        glEnableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_POSITION>::value);
      }
      if (vertex_format_desc::has_normal) {
        glVertexAttribPointerARB(gl4::mvf_attrib_location<MVF_BASE_NORMAL>::value,vertex_format_desc::type_normal::components,GLTypes<typename vertex_format_desc::type_normal::type>::gl_define,
          false,0,BUFFER_OFFSET(m.offsets[vertex_format_desc::idx_normal]));
        glEnableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_NORMAL>::value);
      }
      if (vertex_format_desc::has_color0) {
        glVertexAttribPointerARB(gl4::mvf_attrib_location<MVF_BASE_COLOR0>::value,vertex_format_desc::type_color0::components,GLTypes<typename vertex_format_desc::type_color0::type>::gl_define,
          false,0,BUFFER_OFFSET(m.offsets[vertex_format_desc::idx_color0]));
        glEnableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_COLOR0>::value);
      }
      if (vertex_format_desc::has_color1) {
        glVertexAttribPointerARB(gl4::mvf_attrib_location<MVF_BASE_COLOR1>::value,vertex_format_desc::type_color1::components,GLTypes<typename vertex_format_desc::type_color1::type>::gl_define,
          false,0,BUFFER_OFFSET(m.offsets[vertex_format_desc::idx_color1]));
        glEnableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_COLOR1>::value);
      }
      if (vertex_format_desc::has_texcoord0) {
        glVertexAttribPointerARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD0>::value,vertex_format_desc::type_texcoord0::components,GLTypes<typename vertex_format_desc::type_texcoord0::type>::gl_define,
          false,0,BUFFER_OFFSET(m.offsets[vertex_format_desc::idx_texcoord0]));
        glEnableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD0>::value);
      }
      if (vertex_format_desc::has_texcoord1) {
        glVertexAttribPointerARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD1>::value,vertex_format_desc::type_texcoord1::components,GLTypes<typename vertex_format_desc::type_texcoord1::type>::gl_define,
          false,0,BUFFER_OFFSET(m.offsets[vertex_format_desc::idx_texcoord1]));
        glEnableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD1>::value);
      }
      if (vertex_format_desc::has_texcoord2) {
        glVertexAttribPointerARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD2>::value,vertex_format_desc::type_texcoord2::components,GLTypes<typename vertex_format_desc::type_texcoord2::type>::gl_define,
          false,0,BUFFER_OFFSET(m.offsets[vertex_format_desc::idx_texcoord2]));
        glEnableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD2>::value);
      }
      if (vertex_format_desc::has_texcoord3) {
        glVertexAttribPointerARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD3>::value,vertex_format_desc::type_texcoord3::components,GLTypes<typename vertex_format_desc::type_texcoord3::type>::gl_define,
          false,0,BUFFER_OFFSET(m.offsets[vertex_format_desc::idx_texcoord3]));
        glEnableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD3>::value);
      }
      if (vertex_format_desc::has_texcoord4) {
        glVertexAttribPointerARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD4>::value,vertex_format_desc::type_texcoord4::components,GLTypes<typename vertex_format_desc::type_texcoord4::type>::gl_define,
          false,0,BUFFER_OFFSET(m.offsets[vertex_format_desc::idx_texcoord4]));
        glEnableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD4>::value);
      }
      if (vertex_format_desc::has_texcoord5) {
        glVertexAttribPointerARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD5>::value,vertex_format_desc::type_texcoord5::components,GLTypes<typename vertex_format_desc::type_texcoord5::type>::gl_define,
          false,0,BUFFER_OFFSET(m.offsets[vertex_format_desc::idx_texcoord5]));
        glEnableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD5>::value);
      }
      if (vertex_format_desc::has_texcoord6) {
        glVertexAttribPointerARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD6>::value,vertex_format_desc::type_texcoord6::components,GLTypes<typename vertex_format_desc::type_texcoord6::type>::gl_define,
          false,0,BUFFER_OFFSET(m.offsets[vertex_format_desc::idx_texcoord6]));
        glEnableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD6>::value);
      }
      if (vertex_format_desc::has_texcoord7) {
        glVertexAttribPointerARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD7>::value,vertex_format_desc::type_texcoord7::components,GLTypes<typename vertex_format_desc::type_texcoord7::type>::gl_define,
          false,0,BUFFER_OFFSET(m.offsets[vertex_format_desc::idx_texcoord7]));
        glEnableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD7>::value);
      }
    }


    static void     unbind(const mesh_descriptor&)
    {
      if (vertex_format_desc::has_position) {
        glDisableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_POSITION>::value);
      }
      if (vertex_format_desc::has_normal) {
        glDisableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_NORMAL>::value);
      }
      if (vertex_format_desc::has_color0) {
        glDisableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_COLOR0>::value);
      }
      if (vertex_format_desc::has_color1) {
        glDisableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_COLOR1>::value);
      }
      if (vertex_format_desc::has_texcoord0) {
        glDisableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD0>::value);
      }
      if (vertex_format_desc::has_texcoord1) {
        glDisableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD1>::value);
      }
      if (vertex_format_desc::has_texcoord2) {
        glDisableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD2>::value);
      }
      if (vertex_format_desc::has_texcoord3) {
        glDisableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD3>::value);
      }
      if (vertex_format_desc::has_texcoord4) {
        glDisableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD4>::value);
      }
      if (vertex_format_desc::has_texcoord5) {
        glDisableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD5>::value);
      }
      if (vertex_format_desc::has_texcoord6) {
        glDisableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD6>::value);
      }
      if (vertex_format_desc::has_texcoord7) {
        glDisableVertexAttribArrayARB(gl4::mvf_attrib_location<MVF_BASE_TEXCOORD7>::value);
      }
      glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
      glBindVertexArray(0);
    }


    static void     free(const mesh_descriptor& m)
    {
      glDeleteBuffersARB(1,&m.uiVB);
      glDeleteVertexArrays(1,&m.uiVA);
    }


    static void     draw(const mesh_descriptor& m)
    {
      glDrawArrays(m.iPrimType,0,m.iVertices);
    }
  };


  /// GL Mesh policy for VBO / indexed mesh

  template <class T_VertexFormat,class T_IndexType>
  class CompilePolicy_Indexed_GL_VBO : public CompilePolicy_GL_VBO<T_VertexFormat,T_IndexType>
  {
    // TODO: add static check with index type - right now it is not working (e.g. int)

  public:

    typedef struct { unsigned char data[LibSL::Mesh::MVF_sizeof<T_VertexFormat>::value]; } vertex_data;
    typedef LibSL::Mesh::FormatDescriptor<T_VertexFormat>                                  vertex_format_desc;
    typedef CompilePolicy_GL_VBO<T_VertexFormat,T_IndexType>                  parent_policy;
    typedef VertexStorageSeparateAttributes<T_VertexFormat,vertex_data>       vertex_storage;
    typedef std::vector<T_IndexType>                                          index_storage;

    enum {nb_vertex_attrib = Loki::TL::Length<T_VertexFormat>::value};

    class mesh_descriptor : public parent_policy::mesh_descriptor
    {
    public:
      GLuint uiIB;
      int    iIndices;
    };


    static void compile(
      int               flags,
      int               primtype,
      vertex_storage&   storage,
      index_storage&    indices,
      mesh_descriptor& _mesh)
    {
      const int       vfsz=Loki::TL::Length<T_VertexFormat>::value;
      // create vertex buffer
      int nbv=storage.m_iVertices;
      _mesh.iVertices=nbv;
      _mesh.iPrimType=primtype;
      // compute offsets
      for (int i=0;i<vfsz;i++)
      {
        int offset=vertex_format_desc::offset(i);
        _mesh.offsets[i]=offset*nbv;
      }
      // create VAO
      glGenVertexArrays(1, &_mesh.uiVA);
      glBindVertexArray(_mesh.uiVA);
      // create VBO
      glGenBuffersARB(1,&_mesh.uiVB);
      glBindBufferARB(GL_ARRAY_BUFFER_ARB,_mesh.uiVB);
      // send to graphics card
      glBufferDataARB(GL_ARRAY_BUFFER_ARB,nbv*vertex_format_desc::size_of,NULL,(flags & GPUMESH_DYNAMIC ? GL_DYNAMIC_DRAW_ARB : GL_STATIC_DRAW_ARB));
      for (int i=0;i<vertex_storage::nb_attributes;i++) {
        glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
          _mesh.offsets[i],
          storage.m_Attributes[i].size(),
          &(storage.m_Attributes[i].front())
          );
      }
      // create index buffer
      _mesh.iIndices=int(indices.size());
      glGenBuffersARB(1,&_mesh.uiIB);
      glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,_mesh.uiIB);
      glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
        indices.size()*sizeof(T_IndexType),
        &(indices.front()),
        GL_STATIC_DRAW_ARB);
      // free memory
      if (!(flags & GPUMESH_DYNAMIC)) {
        storage.clear();
      }
      indices.clear();
    }

    static void update(
      vertex_storage&   storage,
      mesh_descriptor& _mesh)
    {
      glBindVertexArray(_mesh.uiVA);
      glBindBufferARB(GL_ARRAY_BUFFER_ARB,_mesh.uiVB);
      for (int i=0;i<vertex_storage::nb_attributes;i++) {
        glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
          _mesh.offsets[i],
          storage.m_Attributes[i].size(),
          &(storage.m_Attributes[i].front()));
      }
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
      glBindVertexArray(0);
    }

    static void     bind(const mesh_descriptor& m)
    {
      parent_policy::bind(m);
      glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,m.uiIB);
    }


    static void     unbind(const mesh_descriptor& m)
    {
      parent_policy::unbind(m);
      glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
    }


    static void     free(const mesh_descriptor& m)
    {
      glDeleteBuffersARB(1,&m.uiIB);
      parent_policy::free(m); // deletes VB,VA
    }


    static void     draw(const mesh_descriptor& m)
    {
      glDrawElements(m.iPrimType,m.iIndices,GLTypes<T_IndexType>::gl_define,NULL);
    }

  };

  /// GL Mesh with VBO
  template <class T_VertexFormat>
    class GPUMesh_GL_VBO
    : public GPUMesh_generic< CompilePolicy_GL_VBO , T_VertexFormat >
    {
    public:
      GPUMesh_GL_VBO(int flags = 0) : GPUMesh_generic< CompilePolicy_GL_VBO , T_VertexFormat >(flags) {}
    };

  /// GL Mesh with VBO / indexed mesh
  template <class T_VertexFormat,class T_IndexType>
  class GPUMesh_Indexed_GL_VBO
    : public GPUMesh_generic< CompilePolicy_Indexed_GL_VBO , T_VertexFormat , T_IndexType >
  {
  public:
    GPUMesh_Indexed_GL_VBO(int flags = 0) : GPUMesh_generic< CompilePolicy_Indexed_GL_VBO , T_VertexFormat , T_IndexType >(flags) {}
  };

} // namespace GPUMesh
} // namespace LibSL
