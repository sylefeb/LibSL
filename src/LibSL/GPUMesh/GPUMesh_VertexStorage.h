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
// ----------------------------------------------------------------------------
// VertexStorage.h
//
//   A VertexStorage policy is used to specify how verticies should be stored 
//   by a RenderMesh class. There is basicly two ways of storing verticies and 
//   their attributes:
//
//   - interlaced (see VertexStorageInterlaced)
//     example:
//      P0_xyz N0_xyz T0_uv P1_xyz N1_xyz T1_uv ... Pn_xyz Nn_xyz Tn_uv
//   - separated  (see VertexStorageSeparateAttributes)
//     example:
//      P0_xyz P1_xyz ... Pn_xyz N0_xyz N1_xyz ... Nn_xyz T0_uv T1_uv ... Tn_uv
//
//   An additional policy, VertexCopy, is used to control vertex copy inside 
//   the storage policy. It allows to support plateform specific storage (like 
//   converting colors from RGBA to BGRA with Direct3D).
//
//
//                                                    (c) Sylvain Lefebvre 2003
// ----------------------------------------------------------------------------
#pragma once
// -------------------------------------------------
#include <LibSL/Mesh/VertexFormat.h>
// -------------------------------------------------
#include <vector>
// -------------------------------------------------
namespace LibSL {
namespace GPUMesh {
// -------------------------------------------------

template <class VertexFormat,typename vertex_data> class VertexCopyDefaultPolicy
{
public:
  static void copy(const vertex_data& src,vertex_data& dst) { dst=src; }
};

// -------------------------------------------------
// Store each attributes in separate vectors
// -------------------------------------------------

template <class VertexFormat,
          typename vertex_data,
          template <class,class> class VertexCopyPolicy = VertexCopyDefaultPolicy>
class VertexStorageSeparateAttributes
{
public:

  typedef LibSL::Mesh::FormatDescriptor<VertexFormat> vertex_format_desc;

  enum {nb_attributes=Loki::TL::Length<VertexFormat>::value}; 

  std::vector<unsigned char> m_Attributes[nb_attributes];
  int                        m_iVertices;

  VertexStorageSeparateAttributes() : m_iVertices(0) 
  {}

  void push_vertex(const vertex_data& v)
  {
    static vertex_data tmp;

    VertexCopyPolicy<VertexFormat,vertex_data>::copy(v,tmp);
    if (vertex_format_desc::has_normal)
    {
      for (int i=0;i<vertex_format_desc::type_normal::size_of;i++)
        m_Attributes[vertex_format_desc::idx_normal].push_back(tmp.data[vertex_format_desc::offset_normal+i]);
    }
    if (vertex_format_desc::has_color0)
    {
      for (int i=0;i<vertex_format_desc::type_color0::size_of;i++)
        m_Attributes[vertex_format_desc::idx_color0].push_back(tmp.data[vertex_format_desc::offset_color0+i]);
    }
    if (vertex_format_desc::has_color1)
    {
      for (int i=0;i<vertex_format_desc::type_color1::size_of;i++)
        m_Attributes[vertex_format_desc::idx_color1].push_back(tmp.data[vertex_format_desc::offset_color1+i]);
    }

#define PUSH_TC(n) \
  if (vertex_format_desc::has_texcoord##n) \
    { \
      for (int i=0;i<vertex_format_desc::type_texcoord##n::size_of;i++) \
      m_Attributes[vertex_format_desc::idx_texcoord##n].push_back(tmp.data[vertex_format_desc::offset_texcoord##n+i]); \
    }

    PUSH_TC(0);
    PUSH_TC(1);
    PUSH_TC(2);
    PUSH_TC(3);
    PUSH_TC(4);
    PUSH_TC(5);
    PUSH_TC(6);
    PUSH_TC(7);

    if (vertex_format_desc::has_position)
    {
      for (int i=0;i<vertex_format_desc::type_position::size_of;i++)
        m_Attributes[vertex_format_desc::idx_position].push_back(tmp.data[vertex_format_desc::offset_position+i]);
    }    
    m_iVertices++;
  }

  void update_vertex(int vid,const vertex_data& v)
  {
    static vertex_data tmp;

    VertexCopyPolicy<VertexFormat,vertex_data>::copy(v,tmp);

    if (vertex_format_desc::has_normal)
    {
      int szof = vertex_format_desc::type_normal::size_of;
      memcpy(&m_Attributes[vertex_format_desc::idx_normal][vid*szof],&tmp.data[vertex_format_desc::offset_normal],szof);
    }
    if (vertex_format_desc::has_color0)
    {
      int szof = vertex_format_desc::type_color0::size_of;
      memcpy(&m_Attributes[vertex_format_desc::idx_color0][vid*szof],&tmp.data[vertex_format_desc::offset_color0],szof);
    }
    if (vertex_format_desc::has_color1)
    {
      int szof = vertex_format_desc::type_color1::size_of;
      memcpy(&m_Attributes[vertex_format_desc::idx_color1][vid*szof],&tmp.data[vertex_format_desc::offset_color1],szof);
    }

#define UPDT_TC(n) \
  if (vertex_format_desc::has_texcoord##n) \
    { \
      int szof = vertex_format_desc::type_texcoord##n::size_of; \
      memcpy(&m_Attributes[vertex_format_desc::idx_texcoord##n][vid*szof],&tmp.data[vertex_format_desc::offset_texcoord##n],szof); \
    }

    UPDT_TC(0);
    UPDT_TC(1);
    UPDT_TC(2);
    UPDT_TC(3);
    UPDT_TC(4);
    UPDT_TC(5);
    UPDT_TC(6);
    UPDT_TC(7);

    if (vertex_format_desc::has_position)
    {
      int szof = vertex_format_desc::type_position::size_of;
      memcpy(&m_Attributes[vertex_format_desc::idx_position][vid*szof],&tmp.data[vertex_format_desc::offset_position],szof);
    }    

  }

  void clear()
  {
    for (int i=0;i<nb_attributes;i++)
      m_Attributes[i].clear();
    m_iVertices=0;
  }

  int numVertices() const {return (m_iVertices);}
};

// -------------------------------------------------
// Store each vertex independently - attributes are interlaced
// -------------------------------------------------

template <class VertexFormat,
          typename vertex_data,
          template <class,class> class VertexCopyPolicy = VertexCopyDefaultPolicy>
class VertexStorageInterlaced
{
public:

  std::vector<vertex_data> m_Vertices;

  void push_vertex(const vertex_data& v)
  {
    static vertex_data tmp;
    VertexCopyPolicy<VertexFormat,vertex_data>::copy(v,tmp);
    m_Vertices.push_back(tmp);
  }

  void update_vertex(int vid,const vertex_data& v)
  {
    static vertex_data tmp;
    VertexCopyPolicy<VertexFormat,vertex_data>::copy(v,tmp);
    m_Vertices[vid] = tmp;
  }

  void clear()
  {
    m_Vertices.clear();
  }

  int numVertices() const {return ((int)m_Vertices.size());}
// TODO: rename as numVertices()

};
// -------------------------------------------------
} // namespace GPUmesh
} // namespace LibSL
// -------------------------------------------------
