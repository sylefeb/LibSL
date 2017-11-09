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
// ----------------------------------------------


#include <assert.h>
#include <vector>
#include <loki/Typelist.h>
#include <loki/static_check.h>

#define ASSERT_TYPELIST(x)

#include <LibSL/Mesh/Mesh.h>
#include <LibSL/Mesh/VertexFormat.h>

#define GPUMESH_DYNAMIC 1

namespace LibSL {
namespace GPUMesh {

  using namespace Mesh;

  template <template <class,class> class T_CompilePolicy,
  class                        T_VertexFormat,
  class                        T_IndexType=Loki::NullType>
  class GPUMesh_generic
  {
  public:

    typedef T_CompilePolicy<T_VertexFormat,T_IndexType>  compile_policy;
    typedef T_VertexFormat                               vertex_format;
    typedef typename compile_policy::vertex_data         vertex_data;
    typedef typename compile_policy::vertex_format_desc  vertex_format_desc;
    typedef typename compile_policy::mesh_descriptor     mesh_descriptor;

  protected:

    typename compile_policy::mesh_descriptor m_Mesh;
    typename compile_policy::vertex_storage  m_Storage;
    vertex_data                              m_CurrentVertex;
    int                                      m_iPrimType;
    bool                                     m_Empty;
    int                                      m_Flags;
    typename compile_policy::index_storage   m_Indices;

    bool                                     m_Updating;
    int                                      m_UpdateCount;

    int push_current()
    {
      if (!m_Updating) {
        m_Storage.push_vertex(m_CurrentVertex);
        return (m_Storage.numVertices()-1);
      } else {
        sl_assert(m_UpdateCount < m_Storage.numVertices());
        m_Storage.update_vertex(m_UpdateCount ++,m_CurrentVertex);
        return (m_UpdateCount - 1);
      }
    }

    void clear()
    {
      m_Updating    = false;
      m_UpdateCount = 0;
      for (int i=0;i<MVF_sizeof<T_VertexFormat>::value;i++) {
        m_CurrentVertex.data[i] = 0;
      }
    }

  public:

    GPUMesh_generic(int flags = 0) : m_iPrimType(-1), m_Empty(true) 
    {
      // check if the vertex format defines a position !
      LOKI_STATIC_CHECK(FormatDescriptor<T_VertexFormat>::has_position,vertex_format_should_contain_a_position);
      m_Flags = flags;
      clear();
    }

    ~GPUMesh_generic()
    {
      if (!m_Empty) {
        compile_policy::free(m_Mesh);
      }
    }

    void begin(int ptype)
    {
      if (!m_Empty) {
        compile_policy::free(m_Mesh);
      }
      clear();
      m_Storage.clear();
      m_Indices.clear();
      m_iPrimType = ptype;
    }

    void end()
    {
      compile_policy::compile(m_Flags,m_iPrimType,m_Storage,m_Indices,m_Mesh);
      m_Empty = false;
    }

    void finalize()
    {
      clear();
      m_Storage.clear();
      m_Indices.clear();
    }

    void draw()
    {
      compile_policy::draw(m_Mesh);
    }

    void bind()
    {
      compile_policy::bind(m_Mesh);
    }

    void unbind()
    {
      compile_policy::unbind(m_Mesh);
    }

    void     render()
    {
      bind();
      draw();
      unbind();
    }

    void     instantiate(uint count)
    {
      bind();
      compile_policy::instantiate(m_Mesh,count);
      unbind();
    }

    bool isEmpty() const { return m_Empty; }

    /// advanced methods

    const mesh_descriptor& descriptor() const {
      return (m_Mesh);
    }

    /// vertex methods

    int vertex_2(
      typename vertex_format_desc::type_position::type x,
      typename vertex_format_desc::type_position::type y)
    {
      if (!vertex_format_desc::has_position)
        return (-1);
      LOKI_STATIC_CHECK(vertex_format_desc::type_position::components == 2,vertex_format_does_not_support_vertex2);
      typename vertex_format_desc::type_position t;
      t.data[0]=x;
      t.data[1]=y;
      memcpy(&(m_CurrentVertex.data[vertex_format_desc::offset_position]),
        t.data,
        vertex_format_desc::type_position::size_of);

      return (push_current());
    }

    int vertex_3(
      typename vertex_format_desc::type_position::type x,
      typename vertex_format_desc::type_position::type y,
      typename vertex_format_desc::type_position::type z)
    {
      if (!vertex_format_desc::has_position)
        return (-1);
      LOKI_STATIC_CHECK(vertex_format_desc::type_position::components == 3,vertex_format_does_not_support_vertex3);
      typename vertex_format_desc::type_position t;
      t.data[0]=x;
      t.data[1]=y;
      t.data[2]=z;
      assert(sizeof(t.data) == vertex_format_desc::type_position::size_of);
      memcpy(&(m_CurrentVertex.data[vertex_format_desc::offset_position]),
        t.data,
        vertex_format_desc::type_position::size_of);

      return (push_current());
    }

    int vertex_4(
      typename vertex_format_desc::type_position::type x,
      typename vertex_format_desc::type_position::type y,
      typename vertex_format_desc::type_position::type z,
      typename vertex_format_desc::type_position::type w)
    {
      if (!vertex_format_desc::has_position)
        return (-1);
      LOKI_STATIC_CHECK(vertex_format_desc::type_position::components == 4,vertex_format_does_not_support_vertex4);
      typename vertex_format_desc::type_position t;
      t.data[0]=x;
      t.data[1]=y;
      t.data[2]=z;
      t.data[3]=z;
      memcpy(&(m_CurrentVertex.data[vertex_format_desc::offset_position]),
        t.data,
        vertex_format_desc::type_position::size_of);

      return (push_current());
    }

    template <typename T_VertexData>
    int vertex_ptr(const T_VertexData *data)
    {
      if (!vertex_format_desc::has_position) {
        return (-1);
      }
      LOKI_STATIC_CHECK(sizeof(T_VertexData) == MVF_sizeof<vertex_format>::value,vertex_data_does_not_match_vertex_format);
      memcpy(&(m_CurrentVertex.data[0]),data,MVF_sizeof<vertex_format>::value);

      return (push_current());
    }

    int vertex_raw(const void *data,unsigned int size)
    {
      if (!vertex_format_desc::has_position) {
        return (-1);
      }
      assert(size == MVF_sizeof<vertex_format>::value);
      memcpy(&(m_CurrentVertex.data[0]),data,MVF_sizeof<vertex_format>::value);
      return (push_current());
    }

    /// vertex update methods
    //  vertices must be re-sent in same order as before
    //  vertex_skip() may be used on unchanged vertices
    void beginVertexUpdate()
    {
      sl_assert(m_Flags & GPUMESH_DYNAMIC); // use GPUMESH_DYNAMIC for dynamic meshes
      sl_assert(!m_Updating);
      m_Updating    = true;
      m_UpdateCount = 0;
    }

    void endVertexUpdate()
    {
      sl_assert(m_Updating);
      sl_assert(m_UpdateCount == m_Storage.numVertices());
      m_Updating    = false;
      compile_policy::update(m_Storage,m_Mesh);
    }

    void vertex_skip()
    {
      sl_assert(m_Updating);
      sl_assert(m_UpdateCount < m_Storage.numVertices());
      m_UpdateCount ++;
    }

    // define some ugly macros to ease method creation for attributes

#define DATA_METHOD_1(func_name,name) void func_name(typename vertex_format_desc::type_##name::type x) { \
  if (!vertex_format_desc::has_##name)				\
  return;							\
  if (vertex_format_desc::type_##name::components != 1)		\
  return;							\
  typename vertex_format_desc::type_##name t;			\
  t.data[0]=x;							\
  memcpy(&(m_CurrentVertex.data[vertex_format_desc::offset_##name]), \
  t.data,							\
  vertex_format_desc::type_##name::size_of); }		\
  \
  void func_name(LibSL::Mesh::MVF_empty x) {}

#define DATA_METHOD_2(func_name,name) void func_name(typename vertex_format_desc::type_##name::type x, \
  typename vertex_format_desc::type_##name::type y) { \
  if (!vertex_format_desc::has_##name)				\
  return;							\
  if (vertex_format_desc::type_##name::components != 2)		\
  return;							\
  typename vertex_format_desc::type_##name t;			\
  t.data[0]=x;							\
  t.data[1]=y;							\
  memcpy(&(m_CurrentVertex.data[vertex_format_desc::offset_##name]), \
  t.data,							\
  vertex_format_desc::type_##name::size_of); }		\
  \
  void func_name(LibSL::Mesh::MVF_empty x,LibSL::Mesh::MVF_empty y) {}

#define DATA_METHOD_3(func_name,name) void func_name(typename vertex_format_desc::type_##name::type x, \
  typename vertex_format_desc::type_##name::type y, \
  typename vertex_format_desc::type_##name::type z) { \
  if (!vertex_format_desc::has_##name)				\
  return;							\
  if (vertex_format_desc::type_##name::components != 3)		\
  return;							\
  typename vertex_format_desc::type_##name t;			\
  t.data[0]=x;							\
  t.data[1]=y;							\
  t.data[2]=z;							\
  memcpy(&(m_CurrentVertex.data[vertex_format_desc::offset_##name]), \
  t.data,							\
  vertex_format_desc::type_##name::size_of); }		\
  \
  void func_name(LibSL::Mesh::MVF_empty x,LibSL::Mesh::MVF_empty y,LibSL::Mesh::MVF_empty z) {}

#define DATA_METHOD_4(func_name,name) void func_name(typename vertex_format_desc::type_##name::type x, \
  typename vertex_format_desc::type_##name::type y, \
  typename vertex_format_desc::type_##name::type z, \
  typename vertex_format_desc::type_##name::type w) { \
  if (!vertex_format_desc::has_##name)				\
  return;							\
  if (vertex_format_desc::type_##name::components != 4)		\
  return;							\
  typename vertex_format_desc::type_##name t;			\
  t.data[0]=x;							\
  t.data[1]=y;							\
  t.data[2]=z;							\
  t.data[3]=w;							\
  memcpy(&(m_CurrentVertex.data[vertex_format_desc::offset_##name]), \
  t.data,							\
  vertex_format_desc::type_##name::size_of); }		\
  \
  void func_name(LibSL::Mesh::MVF_empty x,LibSL::Mesh::MVF_empty y,LibSL::Mesh::MVF_empty z,LibSL::Mesh::MVF_empty w) {}

    // use macros to define attribute methods

      DATA_METHOD_3(normal,normal)
      DATA_METHOD_3(color0_3,color0)
      DATA_METHOD_4(color0_4,color0)
      DATA_METHOD_3(color1_3,color1)
      DATA_METHOD_4(color1_4,color1)
      DATA_METHOD_1(texcoord0_1,texcoord0)
      DATA_METHOD_2(texcoord0_2,texcoord0)
      DATA_METHOD_3(texcoord0_3,texcoord0)
      DATA_METHOD_4(texcoord0_4,texcoord0)
      DATA_METHOD_1(texcoord1_1,texcoord1)
      DATA_METHOD_2(texcoord1_2,texcoord1)
      DATA_METHOD_3(texcoord1_3,texcoord1)
      DATA_METHOD_4(texcoord1_4,texcoord1)
      DATA_METHOD_1(texcoord2_1,texcoord2)
      DATA_METHOD_2(texcoord2_2,texcoord2)
      DATA_METHOD_3(texcoord2_3,texcoord2)
      DATA_METHOD_4(texcoord2_4,texcoord2)
      DATA_METHOD_1(texcoord3_1,texcoord3)
      DATA_METHOD_2(texcoord3_2,texcoord3)
      DATA_METHOD_3(texcoord3_3,texcoord3)
      DATA_METHOD_4(texcoord3_4,texcoord3)
      DATA_METHOD_1(texcoord4_1,texcoord4)
      DATA_METHOD_2(texcoord4_2,texcoord4)
      DATA_METHOD_3(texcoord4_3,texcoord4)
      DATA_METHOD_4(texcoord4_4,texcoord4)
      DATA_METHOD_1(texcoord5_1,texcoord5)
      DATA_METHOD_2(texcoord5_2,texcoord5)
      DATA_METHOD_3(texcoord5_3,texcoord5)
      DATA_METHOD_4(texcoord5_4,texcoord5)
      DATA_METHOD_1(texcoord6_1,texcoord6)
      DATA_METHOD_2(texcoord6_2,texcoord6)
      DATA_METHOD_3(texcoord6_3,texcoord6)
      DATA_METHOD_4(texcoord6_4,texcoord6)
      DATA_METHOD_1(texcoord7_1,texcoord7)
      DATA_METHOD_2(texcoord7_2,texcoord7)
      DATA_METHOD_3(texcoord7_3,texcoord7)
      DATA_METHOD_4(texcoord7_4,texcoord7)

    // index methods

    void index(T_IndexType idx)
    {
      // TODO: assert idx within range ?
      if (Loki::IsSameType<T_IndexType,Loki::NullType>::value)
        return;
      m_Indices.push_back(idx);
    }

  };

} // namespace GPUMesh
} // namespace LibSL

// ----------------------------------------------
// For backward compatibility

#define GPUMESH_MVF1  LOKI_TYPELIST_1
#define GPUMESH_MVF2  LOKI_TYPELIST_2
#define GPUMESH_MVF3  LOKI_TYPELIST_3
#define GPUMESH_MVF4  LOKI_TYPELIST_4
#define GPUMESH_MVF5  LOKI_TYPELIST_5
#define GPUMESH_MVF6  LOKI_TYPELIST_6
#define GPUMESH_MVF7  LOKI_TYPELIST_7
#define GPUMESH_MVF8  LOKI_TYPELIST_8
#define GPUMESH_MVF9  LOKI_TYPELIST_9
#define GPUMESH_MVF10 LOKI_TYPELIST_10
#define GPUMESH_MVF11 LOKI_TYPELIST_11
#define GPUMESH_MVF12 LOKI_TYPELIST_12
#define GPUMESH_MVF13 LOKI_TYPELIST_13
#define GPUMESH_MVF14 LOKI_TYPELIST_14
#define GPUMESH_MVF15 LOKI_TYPELIST_15
#define GPUMESH_MVF16 LOKI_TYPELIST_16
#define GPUMESH_MVF17 LOKI_TYPELIST_17
#define GPUMESH_MVF18 LOKI_TYPELIST_18
#define GPUMESH_MVF19 LOKI_TYPELIST_19
#define GPUMESH_MVF20 LOKI_TYPELIST_20
#define GPUMESH_MVF21 LOKI_TYPELIST_21
#define GPUMESH_MVF22 LOKI_TYPELIST_22
#define GPUMESH_MVF23 LOKI_TYPELIST_23
#define GPUMESH_MVF24 LOKI_TYPELIST_24
#define GPUMESH_MVF25 LOKI_TYPELIST_25
#define GPUMESH_MVF26 LOKI_TYPELIST_26
#define GPUMESH_MVF27 LOKI_TYPELIST_27
#define GPUMESH_MVF28 LOKI_TYPELIST_28
#define GPUMESH_MVF29 LOKI_TYPELIST_29
#define GPUMESH_MVF30 LOKI_TYPELIST_30
#define GPUMESH_MVF31 LOKI_TYPELIST_31
#define GPUMESH_MVF32 LOKI_TYPELIST_32
#define GPUMESH_MVF33 LOKI_TYPELIST_33
#define GPUMESH_MVF34 LOKI_TYPELIST_34
#define GPUMESH_MVF35 LOKI_TYPELIST_35
#define GPUMESH_MVF36 LOKI_TYPELIST_36
#define GPUMESH_MVF37 LOKI_TYPELIST_37
#define GPUMESH_MVF38 LOKI_TYPELIST_38
#define GPUMESH_MVF39 LOKI_TYPELIST_39


#ifndef LIBSL_NO_GRAPHICS_API

#ifdef DIRECT3D
//#include <LibSL/LibSL_d3d.h>
#include <LibSL/GPUMesh/GPUMesh_d3d.h>
#endif

#ifdef DIRECTX10
//#include <LibSL/LibSL_dx10.h>
#include <LibSL/GPUMesh/GPUMesh_dx10.h>
#endif

#ifdef OPENGL
#ifdef OPENGL4
//#include <LibSL/LibSL_gl4.h>
#include <LibSL/GPUMesh/GPUMesh_gl4.h>
#else
//#include <LibSL/LibSL_gl.h>
#include <LibSL/GPUMesh/GPUMesh_gl.h>
#endif
#endif

#endif


// ----------------------------------------------
