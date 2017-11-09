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

#include "GPUMesh_primitives.h"
#include "GPUMesh_VertexStorage.h"

#include "LibSL/D3DHelpers/D3DHelpers.h"

// ----------------------------------------------
// dx10 compile policy
// ----------------------------------------------

namespace LibSL {
namespace GPUMesh {

  /// Creates a fake shader and obtain a shader signature
  class ShaderSignature
  {
  protected:
      ID3D10Effect    *m_Effect;
      D3D10_PASS_DESC  m_PassDesc;
  public:
    ShaderSignature(ID3D10Device *dev,std::string custom_def)
    {
        ID3D10Blob *errors;
        D3D10CreateBlob(1024,&errors);
        std::string code = " \
struct VS_IN \
{"
    + custom_def +
"};\
\
struct VS_OUT \
{\
    float4 Pos   : POSITION;\
}; \
\
VS_OUT VS( VS_IN In )\
{\
    VS_OUT Output = (VS_OUT)0;\
    return Output;     \
}\
\
float4 PS( VS_OUT In ) : SV_Target \
{ \
    return 0;\
}\
technique10 t_main\
{\
    pass P0\
    {          \
        SetVertexShader( CompileShader( vs_4_0, VS() ) ); \
        SetGeometryShader( NULL ); \
        SetPixelShader( CompileShader( ps_4_0, PS() ) ); \
    }\
}";
        LIBSL_D3D_CHECK_ERROR_DISPLAY( D3DX10CreateEffectFromMemory(
          code.c_str(),(UINT)strlen(code.c_str()),
          NULL,
          NULL,NULL,"fx_4_0",0,0,dev,NULL,NULL,
          &m_Effect,&errors,NULL) );
        sl_assert(m_Effect != NULL);
        sl_assert(m_Effect->GetTechniqueByIndex(0) != NULL);
        sl_assert(m_Effect->GetTechniqueByIndex(0)->GetPassByIndex(0) != NULL);
        LIBSL_D3D_CHECK_ERROR( m_Effect->GetTechniqueByIndex(0)->GetPassByIndex(0)->GetDesc(&m_PassDesc) );
      }
      
      ~ShaderSignature() { m_Effect->Release(); }  
      
      BYTE *inputSignature()     const { return (m_PassDesc.pIAInputSignature); }
      uint  inputSignatureSize() const { return uint(m_PassDesc.IAInputSignatureSize); }
  };

  /// CompilePolicy for DirectX10

  template <class T_VertexFormat,class T_IndexType> 
  class CompilePolicy_dx10
  {
  public:

    CompilePolicy_dx10() { }

    // defines for GPUMesh
    typedef struct { unsigned char data[LibSL::Mesh::MVF_sizeof<T_VertexFormat>::value]; } vertex_data;
    typedef LibSL::Mesh::FormatDescriptor<T_VertexFormat>                                  vertex_format_desc;
    typedef VertexStorageInterlaced<T_VertexFormat,vertex_data>               vertex_storage;
    typedef std::vector<T_IndexType>                                          index_storage;

    typedef struct
    {
      ID3D10Device*      device;
      ID3D10Buffer*      vb;
      ID3D10Buffer*      ib;
      ID3D10InputLayout* layout;
      int                iVertices;
      int                iIndices;
      int                iPrimitives;
      int                iPrimType;
    } mesh_descriptor;

    // compile mesh
    static void compile(
      int             flags,
      int             primtype,
      vertex_storage& storage,
      index_storage&  indices,
      mesh_descriptor& _mesh)
    {
      const int       vfsz = Loki::TL::Length<T_VertexFormat>::value;
      const int       szof = MVF_sizeof<T_VertexFormat>::value;

      int nbv           = (int)storage.m_Vertices.size();
      _mesh.iVertices   = nbv;
      _mesh.iPrimType   = primtype;
      PrimitiveNfo nfo(primtype,nbv);    
      _mesh.iPrimitives = nfo.nbPrim();
      
      // create layout
      _mesh.layout = NULL;
      layoutFromVertexFormat(_mesh,&(_mesh.layout));

      // create vertex buffer
      D3D10_BUFFER_DESC bufferDesc;
      bufferDesc.Usage 		  	  = D3D10_USAGE_DEFAULT;
      bufferDesc.ByteWidth 		  = szof * _mesh.iVertices;
      bufferDesc.BindFlags 		  = D3D10_BIND_VERTEX_BUFFER;
      bufferDesc.CPUAccessFlags = 0;
      bufferDesc.MiscFlags 		  = 0;

      D3D10_SUBRESOURCE_DATA initData;
      initData.pSysMem          = &(storage.m_Vertices.front());
      initData.SysMemPitch      = 0;
      initData.SysMemSlicePitch = 0;
      LIBSL_D3D_CHECK_ERROR( _mesh.device->CreateBuffer( &bufferDesc, &initData, &_mesh.vb ) );

      // free memory
      storage.clear();
      indices.clear();
    }


    static void     bind(const mesh_descriptor& m)
    {
      m.device->IASetInputLayout( m.layout );
      // set VB
      const UINT stride = MVF_sizeof<T_VertexFormat>::value;
      const UINT offset = 0;
      m.device->IASetVertexBuffers( 
        0,                // the first input slot for binding
        1,                // the number of buffers in the array
        &m.vb,            // the array of vertex buffers
        &stride,          // array of stride values, one for each buffer
        &offset );        // array of offset values, one for each buffer
      // set topology
      m.device->IASetPrimitiveTopology( (D3D10_PRIMITIVE_TOPOLOGY)m.iPrimType );
    }


    static void     unbind(const mesh_descriptor& m)
    {
    }


    static void     free(mesh_descriptor& m)
    {
      LIBSL_D3D_SAFE_RELEASE(m.layout);
      LIBSL_D3D_SAFE_RELEASE(m.vb);
    }


    static void     draw(const mesh_descriptor& m)
    {
       m.device->Draw(m.iVertices,0);
    }

    inline static D3D10_INPUT_ELEMENT_DESC element(const char *name,uint sidx, DXGI_FORMAT fmt, uint slot, uint offs, const D3D10_INPUT_CLASSIFICATION& classif, uint step)
    {
      D3D10_INPUT_ELEMENT_DESC desc;
      desc.SemanticName         = name;
      desc.SemanticIndex        = sidx;
      desc.Format               = fmt;
      desc.InputSlot            = slot;
      desc.AlignedByteOffset    = D3D10_APPEND_ALIGNED_ELEMENT;
      desc.InputSlotClass       = classif;
      desc.InstanceDataStepRate = step;
      return desc;
    }

    static void layoutFromVertexFormat(const mesh_descriptor& m,ID3D10InputLayout **layout)
    {
      const uint MAX_ELEMENTS = 64;
      static D3D10_INPUT_ELEMENT_DESC vertex_layout[MAX_ELEMENTS];
      uint num    = 0;
      std::string custom_def = "";

      LOKI_STATIC_CHECK((vertex_format_desc::type_position::components != 2),ERROR_DX10_vertex_format_does_not_support_vertex2);

      if (vertex_format_desc::has_position) {
        if (vertex_format_desc::type_position::components == 4) {
          vertex_layout[num] = element( "POSITION",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, -1,  D3D10_INPUT_PER_VERTEX_DATA, 0 );
          custom_def += "float4 pos : POSITION;\n";
        } else if (vertex_format_desc::type_position::components == 3) {
          vertex_layout[num] = element( "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, -1,  D3D10_INPUT_PER_VERTEX_DATA, 0 );
          custom_def += "float3 pos : POSITION;\n";
        } else {
          sl_assert(false);
        }
        num ++;
      }

      if (vertex_format_desc::has_normal) {
        vertex_layout[num] = element( "NORMAL",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, -1, D3D10_INPUT_PER_VERTEX_DATA, 0 );
        custom_def += "float3 nrm : NORMAL;\n";
        num ++;
      }

      if (vertex_format_desc::has_color0) {
        if        (vertex_format_desc::type_color0::components == 4) {
          vertex_layout[num] = element( "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, -1,  D3D10_INPUT_PER_VERTEX_DATA, 0 );
          custom_def += "float4 clr0 : COLOR0;\n";
        } else if (vertex_format_desc::type_color0::components == 3) {
          vertex_layout[num] = element( "COLOR",  0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, -1,  D3D10_INPUT_PER_VERTEX_DATA, 0 );
          custom_def += "float3 clr0 : COLOR0;\n";
        } else {
          sl_assert(false);
        }
        num ++;
      }

      if (vertex_format_desc::has_color1) {
        if        (vertex_format_desc::type_color1::components == 4) {
          vertex_layout[num] = element( "COLOR",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, -1,  D3D10_INPUT_PER_VERTEX_DATA, 0 );
          custom_def += "float4 clr1 : COLOR1;\n";
        } else if (vertex_format_desc::type_color1::components == 3) {
          vertex_layout[num] = element( "COLOR",  1, DXGI_FORMAT_R32G32B32_FLOAT   , 0, -1,  D3D10_INPUT_PER_VERTEX_DATA, 0 );
          custom_def += "float3 clr1 : COLOR1;\n";
        } else {
          sl_assert(false);
        }
        num ++;
      }

#define DX10_TEXCOORD(N) \
  if (vertex_format_desc::has_texcoord##N) { \
    if        (vertex_format_desc::type_texcoord##N ::components == 4) { \
      vertex_layout[num] = element( "TEXCOORD", N, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, -1,  D3D10_INPUT_PER_VERTEX_DATA, 0 ); \
      custom_def += "float4 tex" #N " : TEXCOORD" #N ";\n"; \
    } else if (vertex_format_desc::type_texcoord##N ::components == 3) { \
      vertex_layout[num] = element( "TEXCOORD", N, DXGI_FORMAT_R32G32B32_FLOAT, 0, -1,  D3D10_INPUT_PER_VERTEX_DATA, 0 ); \
      custom_def += "float3 tex" #N " : TEXCOORD" #N ";\n"; \
    } else if (vertex_format_desc::type_texcoord##N ::components == 2) { \
      vertex_layout[num] = element( "TEXCOORD", N, DXGI_FORMAT_R32G32_FLOAT, 0, -1,  D3D10_INPUT_PER_VERTEX_DATA, 0 ); \
      custom_def += "float2 tex" #N " : TEXCOORD" #N ";\n"; \
    } else if (vertex_format_desc::type_texcoord##N ::components == 1) { \
      vertex_layout[num] = element( "TEXCOORD", N, DXGI_FORMAT_R32_FLOAT, 0, -1,  D3D10_INPUT_PER_VERTEX_DATA, 0 ); \
      custom_def += "float1 tex" #N " : TEXCOORD" #N ";\n"; \
    } else { \
      sl_assert(false); \
    } \
    num ++; \
  }
      DX10_TEXCOORD(0);
      DX10_TEXCOORD(1);
      DX10_TEXCOORD(2);
      DX10_TEXCOORD(3);
      DX10_TEXCOORD(4);
      DX10_TEXCOORD(5);
      DX10_TEXCOORD(6);
      DX10_TEXCOORD(7);

      ShaderSignature sig(m.device,custom_def);
      LIBSL_D3D_CHECK_ERROR( m.device->CreateInputLayout( vertex_layout, num, sig.inputSignature(),sig.inputSignatureSize(), layout ) );

    }

  };

  /// Indexed mesh policy

  template <class T_VertexFormat,class T_IndexType> 
  class CompilePolicy_Indexed_dx10
  {
  public:

    CompilePolicy_Indexed_dx10() { }

    // defines for GPUMesh
    typedef struct { unsigned char data[LibSL::Mesh::MVF_sizeof<T_VertexFormat>::value]; }  vertex_data;
    typedef LibSL::Mesh::FormatDescriptor<T_VertexFormat>                                   vertex_format_desc;
    typedef VertexStorageInterlaced<T_VertexFormat,vertex_data>                vertex_storage;
    typedef std::vector<T_IndexType>                                           index_storage;

    typedef typename CompilePolicy_dx10<T_VertexFormat,T_IndexType>::mesh_descriptor mesh_descriptor;

    static void compile(
      int               flags,
      int               primtype,
      vertex_storage&   storage,
      index_storage&    indices,
      mesh_descriptor& _mesh)
    {
      const int       vfsz = Loki::TL::Length<T_VertexFormat>::value;
      const int       szof = MVF_sizeof<T_VertexFormat>::value;

      int nbv           = (int)storage.m_Vertices.size();
      _mesh.iVertices   = nbv;
      _mesh.iIndices    = (int)indices.size();
      _mesh.iPrimType   = primtype;
      PrimitiveNfo nfo(primtype,nbv);    
      _mesh.iPrimitives = nfo.nbPrim();
      
      // create layout
      CompilePolicy_dx10<T_VertexFormat,T_IndexType>::layoutFromVertexFormat(_mesh,&(_mesh.layout));

      // create vertex buffer
      D3D10_BUFFER_DESC bufferDesc;
      bufferDesc.Usage 		  	  = D3D10_USAGE_DEFAULT;
      bufferDesc.ByteWidth 		  = szof * _mesh.iVertices;
      bufferDesc.BindFlags 		  = D3D10_BIND_VERTEX_BUFFER;
      bufferDesc.CPUAccessFlags = 0;
      bufferDesc.MiscFlags 		  = 0;

      D3D10_SUBRESOURCE_DATA initData;
      initData.pSysMem          = &(storage.m_Vertices.front());
      initData.SysMemPitch      = 0;
      initData.SysMemSlicePitch = 0;
      LIBSL_D3D_CHECK_ERROR( _mesh.device->CreateBuffer( &bufferDesc, &initData, &_mesh.vb ) );

      // create index buffer
      bufferDesc.Usage           = D3D10_USAGE_DEFAULT;
      bufferDesc.ByteWidth       = sizeof(T_IndexType) * uint(indices.size());
      bufferDesc.BindFlags 		   = D3D10_BIND_INDEX_BUFFER;
      bufferDesc.CPUAccessFlags  = 0;
      bufferDesc.MiscFlags 		   = 0;

      initData.pSysMem            = &(indices.front());
      initData.SysMemPitch        = 0;
      initData.SysMemSlicePitch   = 0;
      LIBSL_D3D_CHECK_ERROR( _mesh.device->CreateBuffer( &bufferDesc, &initData, &(_mesh.ib) ) );

      // free memory
      storage.clear();
      indices.clear();
    }


    static void     bind(const mesh_descriptor& m)
    {
      m.device->IASetInputLayout( m.layout );
      // set VB
      const UINT stride = MVF_sizeof<T_VertexFormat>::value;
      const UINT offset = 0;
      m.device->IASetVertexBuffers( 
        0,                // the first input slot for binding
        1,                // the number of buffers in the array
        &m.vb,            // the array of vertex buffers
        &stride,          // array of stride values, one for each buffer
        &offset );        // array of offset values, one for each buffer
      // set IB
      if (sizeof(T_IndexType) == sizeof(uint)) {
        m.device->IASetIndexBuffer( m.ib, DXGI_FORMAT_R32_UINT, 0 );
      } else if (sizeof(T_IndexType) == sizeof(ushort)) {
        m.device->IASetIndexBuffer( m.ib, DXGI_FORMAT_R16_UINT, 0 );
      } else {
        sl_assert(false);
      }
      // set topology
      m.device->IASetPrimitiveTopology( (D3D10_PRIMITIVE_TOPOLOGY)m.iPrimType );
    }


    static void     unbind(const mesh_descriptor& m)
    {
    }


    static void     free(mesh_descriptor& m)
    {
      LIBSL_D3D_SAFE_RELEASE(m.layout);
      LIBSL_D3D_SAFE_RELEASE(m.vb);
      LIBSL_D3D_SAFE_RELEASE(m.ib);
    }


    static void     draw(const mesh_descriptor& m)
    {
      m.device->DrawIndexed(m.iIndices,0,0);
    }

  };

  /// dx10 Mesh with vertex buffer
  template <class VertexFormat> 
  class GPUMesh_dx10
    : public GPUMesh_generic<CompilePolicy_dx10,VertexFormat> 
  {
  public:
    GPUMesh_dx10(ID3D10Device *dx10)
    {
      m_Mesh.device   = dx10;
    }
  };

  /// DX10 Mesh with indexed vertex buffer
  template <class T_VertexFormat,class T_IndexType> 
  class GPUMesh_Indexed_dx10
    : public GPUMesh_generic<CompilePolicy_Indexed_dx10,T_VertexFormat,T_IndexType> 
  {
  public:
    GPUMesh_Indexed_dx10(ID3D10Device *dx10)
    {
      m_Mesh.device = dx10;
    }
  };

} // namespace GPUMesh
} // namespace LibSL