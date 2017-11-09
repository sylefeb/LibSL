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

#pragma once

#include "GPUMesh_D3D_SortTypelist.h"
#include "GPUMesh_D3D_types.h"
#include "GPUMesh_primitives.h"
#include "GPUMesh_VertexStorage.h"


// ----------------------------------------------
// D3D needs a special vertex copy policy because
// of color components ordering (see VertexStorage.h)
// ----------------------------------------------

namespace LibSL {
namespace GPUMesh {

  /// Helper class to copy a vertex

  template <class T_VertexFormat,typename vertex_data> class VertexCopyD3DPolicy
  {
  private:
    typedef LibSL::Mesh::FormatDescriptor<T_VertexFormat> vertex_format_desc;
  public:
    static void copy(const vertex_data& src,vertex_data& dst) 
    { 
      // copy
      dst=src;
      // then correct colors (BGR instead of RGB)
      if (vertex_format_desc::has_color0)
      { 
        unsigned char tmp=dst.data[vertex_format_desc::offset_color0  ];
        dst.data[vertex_format_desc::offset_color0  ]=dst.data[vertex_format_desc::offset_color0+2];
        dst.data[vertex_format_desc::offset_color0+2]=tmp;
      }
      if (vertex_format_desc::has_color1)
      { 
        unsigned char tmp=dst.data[vertex_format_desc::offset_color1  ];
        dst.data[vertex_format_desc::offset_color1  ]=dst.data[vertex_format_desc::offset_color1+2];
        dst.data[vertex_format_desc::offset_color1+2]=tmp;
      }
    }
  };

  /// CompilePolicy for Direct3D

  template <class T_VertexFormat,class T_IndexType> 
  class CompilePolicy_D3D
  {
  public:

    CompilePolicy_D3D() { }

    // sort format to comply with D3D
    typedef typename MVF_D3D_sort_items<T_VertexFormat>::Result                         SortedVertexFormat;

    // defines for GPUMesh
    typedef struct { unsigned char data[LibSL::Mesh::MVF_sizeof<SortedVertexFormat>::value]; }       vertex_data;
    typedef LibSL::Mesh::FormatDescriptor<SortedVertexFormat>                                        vertex_format_desc;
    typedef VertexStorageInterlaced<SortedVertexFormat,vertex_data,VertexCopyD3DPolicy> vertex_storage;
    typedef std::vector<T_IndexType>                                                    index_storage;

    enum {nb_vertex_attrib = Loki::TL::Length<SortedVertexFormat>::value};

    typedef struct
    {
      LPDIRECT3DDEVICE9       pDevice;
      LPDIRECT3DVERTEXBUFFER9 pVB;
      DWORD  fvf;
      int    iVertices;
      int    iPrimitives;
      int    iPrimType;
      int    iSizeOfFVF;
    }mesh_descriptor;

    // compile mesh
    static void compile(
      int               flags,
      int               primtype,
      vertex_storage&   storage,
      index_storage&    indices,
      mesh_descriptor& _mesh)
    {
      const int       vfsz = Loki::TL::Length<SortedVertexFormat>::value;
      const int       szof = MVF_sizeof<SortedVertexFormat>::value;

      int nbv           = (int)storage.m_Vertices.size();
      _mesh.iVertices   = nbv;
      _mesh.iPrimType   = primtype;
      PrimitiveNfo nfo(primtype,nbv);    
      _mesh.iPrimitives = nfo.nbPrim();
      // create FVF
      _mesh.fvf        = fvf();
      _mesh.iSizeOfFVF = vertex_format_desc::size_of;

      //std::cerr << "Mesh fvf sizeof = " << vertex_format_desc::size_of << std::endl;
      //std::cerr << "Mesh num vert = " << _mesh.iVertices << std::endl;
      //std::cerr << "Mesh fvf = " << _mesh.fvf << std::endl;

      // copy data into buffer
      _mesh.pDevice->CreateVertexBuffer( _mesh.iVertices*_mesh.iSizeOfFVF,
        D3DUSAGE_WRITEONLY, 0 /*_mesh.fvf*/,
        D3DPOOL_DEFAULT, &_mesh.pVB, NULL );

      unsigned char* pVertices=NULL;
      _mesh.pVB->Lock( 0, 0, (void**)&pVertices, 0 );
      // using the sorted vertex format, we can directly send the verticies !
      memcpy(pVertices,&(storage.m_Vertices.front()),_mesh.iVertices*_mesh.iSizeOfFVF);
      _mesh.pVB->Unlock();

      // free memory
      storage.clear();
      indices.clear();
    }


    static void     bind(const mesh_descriptor& m)
    {
      m.pDevice->SetFVF(m.fvf);
      m.pDevice->SetStreamSource(0,m.pVB,0,m.iSizeOfFVF);
    }


    static void     unbind(const mesh_descriptor& m)
    {
    }


    static void     free(const mesh_descriptor& m)
    {
      m.pVB->Release();  
    }


    static void     draw(const mesh_descriptor& m)
    {
      m.pDevice->DrawPrimitive((D3DPRIMITIVETYPE)m.iPrimType,0,m.iPrimitives);
    }


    static DWORD fvf()
    {
      LOKI_STATIC_CHECK((vertex_format_desc::type_position::components != 2),ERROR_D3D_vertex_format_does_not_support_vertex2);

      DWORD d3d_fvf=0;

      if (vertex_format_desc::has_position)
      {
        if (vertex_format_desc::type_position::components == 4)
          d3d_fvf |= D3DFVF_XYZW;
        else
          d3d_fvf |= D3DFVF_XYZ;
      }

      if (vertex_format_desc::has_normal)
        d3d_fvf |= D3DFVF_NORMAL;

      if (vertex_format_desc::has_color0)
        d3d_fvf |= D3DFVF_DIFFUSE;

      if (vertex_format_desc::has_color1)
        d3d_fvf |= D3DFVF_SPECULAR;

      int numtex=0;

      if (vertex_format_desc::has_texcoord0)
      {
        numtex++;
        switch (vertex_format_desc::type_texcoord0::components)
        {
        case 1: d3d_fvf |= D3DFVF_TEXCOORDSIZE1(0); break;
        case 2: d3d_fvf |= D3DFVF_TEXCOORDSIZE2(0); break;
        case 3: d3d_fvf |= D3DFVF_TEXCOORDSIZE3(0); break;
        case 4: d3d_fvf |= D3DFVF_TEXCOORDSIZE4(0); break;
        }
      }

      if (vertex_format_desc::has_texcoord1)
      {
        numtex++;
        switch (vertex_format_desc::type_texcoord1::components)
        {
        case 1: d3d_fvf |= D3DFVF_TEXCOORDSIZE1(1); break;
        case 2: d3d_fvf |= D3DFVF_TEXCOORDSIZE2(1); break;
        case 3: d3d_fvf |= D3DFVF_TEXCOORDSIZE3(1); break;
        case 4: d3d_fvf |= D3DFVF_TEXCOORDSIZE4(1); break;
        }
      }

      if (vertex_format_desc::has_texcoord2)
      {
        numtex++;
        switch (vertex_format_desc::type_texcoord2::components)
        {
        case 1: d3d_fvf |= D3DFVF_TEXCOORDSIZE1(2); break;
        case 2: d3d_fvf |= D3DFVF_TEXCOORDSIZE2(2); break;
        case 3: d3d_fvf |= D3DFVF_TEXCOORDSIZE3(2); break;
        case 4: d3d_fvf |= D3DFVF_TEXCOORDSIZE4(2); break;
        }
      }

      if (vertex_format_desc::has_texcoord3)
      {
        numtex++;
        switch (vertex_format_desc::type_texcoord3::components)
        {
        case 1: d3d_fvf |= D3DFVF_TEXCOORDSIZE1(3); break;
        case 2: d3d_fvf |= D3DFVF_TEXCOORDSIZE2(3); break;
        case 3: d3d_fvf |= D3DFVF_TEXCOORDSIZE3(3); break;
        case 4: d3d_fvf |= D3DFVF_TEXCOORDSIZE4(3); break;
        }
      }

      if (vertex_format_desc::has_texcoord4)
      {
        numtex++;
        switch (vertex_format_desc::type_texcoord4::components)
        {
        case 1: d3d_fvf |= D3DFVF_TEXCOORDSIZE1(4); break;
        case 2: d3d_fvf |= D3DFVF_TEXCOORDSIZE2(4); break;
        case 3: d3d_fvf |= D3DFVF_TEXCOORDSIZE3(4); break;
        case 4: d3d_fvf |= D3DFVF_TEXCOORDSIZE4(4); break;
        }
      }

      if (vertex_format_desc::has_texcoord5)
      {
        numtex++;
        switch (vertex_format_desc::type_texcoord5::components)
        {
        case 1: d3d_fvf |= D3DFVF_TEXCOORDSIZE1(5); break;
        case 2: d3d_fvf |= D3DFVF_TEXCOORDSIZE2(5); break;
        case 3: d3d_fvf |= D3DFVF_TEXCOORDSIZE3(5); break;
        case 4: d3d_fvf |= D3DFVF_TEXCOORDSIZE4(5); break;
        }
      }

      if (vertex_format_desc::has_texcoord6)
      {
        numtex++;
        switch (vertex_format_desc::type_texcoord6::components)
        {
        case 1: d3d_fvf |= D3DFVF_TEXCOORDSIZE1(6); break;
        case 2: d3d_fvf |= D3DFVF_TEXCOORDSIZE2(6); break;
        case 3: d3d_fvf |= D3DFVF_TEXCOORDSIZE3(6); break;
        case 4: d3d_fvf |= D3DFVF_TEXCOORDSIZE4(6); break;
        }
      }

      if (vertex_format_desc::has_texcoord7)
      {
        numtex++;
        switch (vertex_format_desc::type_texcoord7::components)
        {
        case 1: d3d_fvf |= D3DFVF_TEXCOORDSIZE1(7); break;
        case 2: d3d_fvf |= D3DFVF_TEXCOORDSIZE2(7); break;
        case 3: d3d_fvf |= D3DFVF_TEXCOORDSIZE3(7); break;
        case 4: d3d_fvf |= D3DFVF_TEXCOORDSIZE4(7); break;
        }
      }

      switch (numtex) {
        case 0: d3d_fvf |= D3DFVF_TEX0; break;
        case 1: d3d_fvf |= D3DFVF_TEX1; break;
        case 2: d3d_fvf |= D3DFVF_TEX2; break;
        case 3: d3d_fvf |= D3DFVF_TEX3; break;
        case 4: d3d_fvf |= D3DFVF_TEX4; break;
        case 5: d3d_fvf |= D3DFVF_TEX5; break;
        case 6: d3d_fvf |= D3DFVF_TEX6; break;
        case 7: d3d_fvf |= D3DFVF_TEX7; break;
        case 8: d3d_fvf |= D3DFVF_TEX8; break;
      }

      return (d3d_fvf);
    }

  };

  /// Indexed mesh policy

  template <class T_VertexFormat,class T_IndexType> 
  class CompilePolicy_Indexed_D3D
  {
  public:

    CompilePolicy_Indexed_D3D() { }

    // sort format to comply with D3D
    typedef typename MVF_D3D_sort_items<T_VertexFormat>::Result                         SortedVertexFormat;

    // defines for GPUMesh
    typedef struct { unsigned char data[LibSL::Mesh::MVF_sizeof<SortedVertexFormat>::value]; }       vertex_data;
    typedef LibSL::Mesh::FormatDescriptor<SortedVertexFormat>                                        vertex_format_desc;
    typedef VertexStorageInterlaced<SortedVertexFormat,vertex_data,VertexCopyD3DPolicy> vertex_storage;
    typedef std::vector<T_IndexType>                                                    index_storage;

    enum {nb_vertex_attrib = Loki::TL::Length<SortedVertexFormat>::value};

    typedef struct
    {
      LPDIRECT3DDEVICE9       pDevice;
      LPDIRECT3DVERTEXBUFFER9 pVB;
      LPDIRECT3DINDEXBUFFER9  pIB;
      DWORD  fvf;
      int    iVertices;
      int    iIndices;
      int    iPrimitives;
      int    iPrimType;
      int    iSizeOfFVF;
    }mesh_descriptor;


    static void compile(
      int               flags,
      int               primtype,
      vertex_storage&   storage,
      index_storage&    indices,
      mesh_descriptor& _mesh)
    {
      const int       vfsz=Loki::TL::Length<SortedVertexFormat>::value;
      const int       szof=MVF_sizeof<SortedVertexFormat>::value;

      // create vertex buffer
      int nbv=(int)storage.m_Vertices.size();
      _mesh.iVertices=nbv;
      _mesh.iPrimType=primtype;
      PrimitiveNfo nfo(primtype,(int)indices.size());    
      _mesh.iPrimitives=nfo.nbPrim();
      // create FVF
      _mesh.fvf=CompilePolicy_D3D<T_VertexFormat,T_IndexType>::fvf();
      _mesh.iSizeOfFVF=vertex_format_desc::size_of;
      // copy data into buffer
      _mesh.pDevice->CreateVertexBuffer( _mesh.iVertices*_mesh.iSizeOfFVF,
        D3DUSAGE_WRITEONLY, /*_mesh.fvf*/0,
        D3DPOOL_DEFAULT, &_mesh.pVB, NULL );

      unsigned char* pVertices=NULL;
      _mesh.pVB->Lock( 0, 0, (void**)&pVertices, 0 );
      // using the sorted vertex format, we can directly send the verticies !
      memcpy(pVertices,&(storage.m_Vertices.front()),_mesh.iVertices*_mesh.iSizeOfFVF);
      _mesh.pVB->Unlock();

      // create index buffer
      _mesh.iIndices=(int)indices.size();
      _mesh.pDevice->CreateIndexBuffer(_mesh.iIndices*sizeof(T_IndexType),
        D3DUSAGE_WRITEONLY, (D3DFORMAT)D3DIndexTypes<T_IndexType>::d3d_define,
        D3DPOOL_DEFAULT, &_mesh.pIB, NULL );
      unsigned char* pIndices=NULL;
      _mesh.pIB->Lock( 0, 0, (void**)&pIndices, 0 );
      memcpy(pIndices,&(indices.front()),_mesh.iIndices*sizeof(T_IndexType));
      _mesh.pIB->Unlock();

      // free memory
      storage.clear();
      indices.clear();
    }


    static void     bind(const mesh_descriptor& m)
    {
      m.pDevice->SetFVF(m.fvf);
      m.pDevice->SetStreamSource(0,m.pVB,0,m.iSizeOfFVF);
      m.pDevice->SetIndices(m.pIB);
    }


    static void     unbind(const mesh_descriptor& m)
    {
    }


    static void     free(const mesh_descriptor& m)
    {
      m.pVB->Release();    
      m.pIB->Release();    
    }


    static void     draw(const mesh_descriptor& m)
    {
      m.pDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)m.iPrimType,
        0,0,
        m.iVertices,0,
        m.iPrimitives);
    }

  };

  /// D3D Mesh with vertex buffer
  template <class VertexFormat> 
  class GPUMesh_D3D
    : public GPUMesh_generic<CompilePolicy_D3D,VertexFormat> 
  {
  public:
    GPUMesh_D3D(LPDIRECT3DDEVICE9 d3d)
    {
      m_Mesh.pDevice=d3d;
    }
  };

  /// D3D Mesh with indexed vertex buffer
  template <class T_VertexFormat,class T_IndexType> 
  class GPUMesh_Indexed_D3D
    : public GPUMesh_generic<CompilePolicy_Indexed_D3D,T_VertexFormat,T_IndexType> 
  {
  public:
    GPUMesh_Indexed_D3D(LPDIRECT3DDEVICE9 d3d)
    {
      m_Mesh.pDevice=d3d;
    }
  };

} // namespace GPUMesh
} // namespace LibSL

