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
// LibSL::GPUTex::DX10_CompilePolicy
// ------------------------------------------------------
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-08-23
// ------------------------------------------------------

#pragma once

// ------------------------------------------------------

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>

#include <LibSL/System/Types.h>
using namespace LibSL::System::Types;

#include <LibSL/Memory/Array2D.h>
#include <LibSL/Memory/Array3D.h>

// ------------------------------------------------------

#include <d3d10_1.h>
#include <d3dx10.h>

#include <LibSL/GPUHelpers/GPUHelpers.h>
#include <LibSL/D3DHelpers/D3DHelpers.h>

#include <LibSL/GPUTex/GPUTex_dx10_formats.h>

// ------------------------------------------------------

#define GPUTEX_DX10_NORM          0
#define GPUTEX_DX10_INT           GPUTEX_INTEGER    // GPUTEX_DX10_INT is deprecated

// ------------------------------------------------------

namespace LibSL  {
  namespace GPUTex {

    enum {DX10_MAX_RENDER_TARGETS = D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT};

    /// API policy
    class DX10_APIPolicy
    {
    public:

      /// Handle to textures
      typedef ID3D10ShaderResourceView* t_APIHandle2D;
      typedef ID3D10ShaderResourceView* t_APIHandle3D;
      typedef ID3D10ShaderResourceView* t_APIHandle2DArray;

      typedef struct { ID3D10Buffer    *tex; ID3D10ShaderResourceView *view; } t_HandleBuffer;
      typedef struct { ID3D10Texture2D *tex; ID3D10ShaderResourceView *view; } t_Handle2D;
      typedef struct { ID3D10Texture3D *tex; ID3D10ShaderResourceView *view; } t_Handle3D;
      typedef struct { ID3D10Texture2D *tex; ID3D10ShaderResourceView *view; } t_HandleCubeMap;
      typedef struct { ID3D10Texture2D *tex; ID3D10ShaderResourceView *view; } t_Handle2DArray;

      typedef struct s_HandleRT2D {
        uint                      numtargets;
        uint                      flags;
        ID3D10Texture2D          *textures [DX10_MAX_RENDER_TARGETS];
        ID3D10ShaderResourceView *resources[DX10_MAX_RENDER_TARGETS];
        ID3D10RenderTargetView   *targets  [DX10_MAX_RENDER_TARGETS];
        ID3D10RenderTargetView   *previous [DX10_MAX_RENDER_TARGETS]; // Use to protect previous targets
        ID3D10DepthStencilView   *previous_depth;
      } t_HandleRT2D;

      static void               bindTexture2D(const t_APIHandle2D& handle) 
      { sl_assert(false); /* DX10 always requires a shader, there is no fixed pipeline */ }

      static t_APIHandle2D renderTarget2DTexture(const t_HandleRT2D& rt,uint target) 
      { sl_assert(target < rt.numtargets); return rt.resources[target];}

      static t_APIHandle2D apiHandle2D(t_Handle2D h) { return h.view; }
      static t_APIHandle3D apiHandle3D(t_Handle3D h) { return h.view; }

      static t_APIHandle2DArray apiHandle2DArray(t_Handle2DArray h) { return h.view; }


    };

    /// Compile policy
    template <class T_APIPolicy,class T_PixelFormat>
    class DX10_CompilePolicy
    {
    public:

      typedef LibSL::Memory::Array::Array  <T_PixelFormat> t_PixelArray1D;
      typedef LibSL::Memory::Array::Array2D<T_PixelFormat> t_PixelArray2D;
      typedef LibSL::Memory::Array::Array3D<T_PixelFormat> t_PixelArray3D;

      /// Create 2D texture
      static 
        typename T_APIPolicy::t_Handle2D 
        create2D(const t_PixelArray2D& array,uint flags)
      {
        T_APIPolicy::t_Handle2D r;

        D3D10_TEXTURE2D_DESC             tex2dDesc;
        D3D10_SHADER_RESOURCE_VIEW_DESC  tex2dRVDesc;
        Array<D3D10_SUBRESOURCE_DATA>    texData;

        uint num_miplvls = 0;
        if (flags & GPUTEX_AUTOGEN_MIPMAP) {
          uint maxsz       = Math::max(array.xsize(),array.ysize());
          while (maxsz > 0) { // compute required number of levels
            maxsz = maxsz >> 1;
            num_miplvls ++;
          }
        } else {
          num_miplvls = 1;
        }
        texData.allocate(num_miplvls);
        ForIndex(l,num_miplvls) {
          uint lvl_w = Math::max(1,array.xsize() >> l);
          uint lvl_h = Math::max(1,array.ysize() >> l);
          // NOTE: We rely on GenerateMips to compute coarse mip levels.
          //       For init, we simply use data from finest level - just because
          //       we have to give something to DX10
          texData[l].pSysMem          = (uchar *)(array.raw()); 
          texData[l].SysMemPitch      = lvl_w*sizeof(typename T_PixelFormat::t_Element)*T_PixelFormat::e_Size;
          texData[l].SysMemSlicePitch = 0;
        }

        if (flags & GPUTEX_INTEGER) {
          tex2dDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_int;
        } else {
          tex2dDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_norm;
        }
        tex2dDesc.ArraySize			      = 1;
        tex2dDesc.Width				        = array.xsize();
        tex2dDesc.Height			        = array.ysize();
        tex2dDesc.MipLevels			      = num_miplvls;
        tex2dDesc.MiscFlags			      = (flags & GPUTEX_AUTOGEN_MIPMAP ? D3D10_RESOURCE_MISC_GENERATE_MIPS : 0);
        tex2dDesc.SampleDesc.Count    = 1;
        tex2dDesc.SampleDesc.Quality  = 0;
        tex2dDesc.CPUAccessFlags	    = 0;
        tex2dDesc.Usage				        = D3D10_USAGE_DEFAULT;
        tex2dDesc.BindFlags			      = (flags & GPUTEX_AUTOGEN_MIPMAP ? D3D10_BIND_RENDER_TARGET  : 0) | D3D10_BIND_SHADER_RESOURCE;
        r.tex  = NULL;
        r.view = NULL;
        LIBSL_D3D_CHECK_ERROR( LIBSL_D3D_DEVICE->CreateTexture2D( &tex2dDesc, texData.raw(), &(r.tex) ) );

        // -> create 2d texture view
        if (flags & GPUTEX_INTEGER) {
          tex2dRVDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_int;
        } else {
          tex2dRVDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_norm;
        }
        tex2dRVDesc.ViewDimension             = D3D10_SRV_DIMENSION_TEXTURE2D;
        tex2dRVDesc.Texture2D.MipLevels       = num_miplvls;
        tex2dRVDesc.Texture2D.MostDetailedMip = 0;
        LIBSL_D3D_CHECK_ERROR( LIBSL_D3D_DEVICE->CreateShaderResourceView( r.tex, &tex2dRVDesc, &r.view ) );

        if (flags & GPUTEX_AUTOGEN_MIPMAP) {
          LIBSL_D3D_DEVICE->GenerateMips(r.view);
        }

        return (r);
      }

      /// Create 2D texture from mipmap array
      static 
        typename T_APIPolicy::t_Handle2D 
        create2D(const LibSL::Memory::Array::Array<t_PixelArray2D>& miparray,uint flags)
      {
        sl_assert(!(flags & GPUTEX_AUTOGEN_MIPMAP));

        T_APIPolicy::t_Handle2D r;

        D3D10_TEXTURE2D_DESC             tex2dDesc;
        D3D10_SHADER_RESOURCE_VIEW_DESC  tex2dRVDesc;

        Array<D3D10_SUBRESOURCE_DATA>    texData;
        texData.allocate(miparray.size());
        ForIndex(l,miparray.size()) {
          uint lvl_w                  = miparray[l].xsize();
          uint lvl_h                  = miparray[l].ysize();
          texData[l].pSysMem          = (uchar *)(miparray[l].raw()); 
          texData[l].SysMemPitch      = lvl_w*sizeof(typename T_PixelFormat::t_Element)*T_PixelFormat::e_Size;
          texData[l].SysMemSlicePitch = 0;
        }

        if (flags & GPUTEX_INTEGER) {
          tex2dDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_int;
        } else {
          tex2dDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_norm;
        }
        tex2dDesc.ArraySize			      = 1;
        tex2dDesc.Width				        = miparray[0].xsize();
        tex2dDesc.Height			        = miparray[0].ysize();
        tex2dDesc.MipLevels			      = miparray.size();
        tex2dDesc.MiscFlags			      = 0;
        tex2dDesc.SampleDesc.Count    = 1;
        tex2dDesc.SampleDesc.Quality  = 0;
        tex2dDesc.CPUAccessFlags	    = 0;
        tex2dDesc.Usage				        = D3D10_USAGE_DEFAULT;
        tex2dDesc.BindFlags			      = D3D10_BIND_SHADER_RESOURCE;
        r.tex  = NULL;
        r.view = NULL;
        LIBSL_D3D_CHECK_ERROR( LIBSL_D3D_DEVICE->CreateTexture2D( &tex2dDesc, texData.raw(), &(r.tex) ) );

        // -> create 2d texture view
        if (flags & GPUTEX_INTEGER) {
          tex2dRVDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_int;
        } else {
          tex2dRVDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_norm;
        }
        tex2dRVDesc.ViewDimension             = D3D10_SRV_DIMENSION_TEXTURE2D;
        tex2dRVDesc.Texture2D.MipLevels       = miparray.size();
        tex2dRVDesc.Texture2D.MostDetailedMip = 0;
        LIBSL_D3D_CHECK_ERROR( LIBSL_D3D_DEVICE->CreateShaderResourceView( r.tex, &tex2dRVDesc, &r.view ) );

        return (r);
      }

      /// Send 2D texture to GPU memory
      static void
        send2D(
        typename T_APIPolicy::t_Handle2D tex,
        const t_PixelArray2D& array,
        uint flags)
      {
        // not available
        sl_assert(false);
      }


      /// Send a subpart of a 2D texture to the GPU
      static void
        sendSub2D(
        typename T_APIPolicy::t_Handle2D id,
        uint x,uint y,uint z,
        const t_PixelArray2D& array,
        uint flags)
      {
        // not available
        sl_assert(false);
      }

      /// Destroy 2D texture
      static void destroy2D(typename T_APIPolicy::t_Handle2D r)
      {
        LIBSL_D3D_SAFE_RELEASE(r.tex);
        LIBSL_D3D_SAFE_RELEASE(r.view);
      }


      /// Create 2D render target
      //   - num render targets are created (enables multiple render targets)
      static 
        typename T_APIPolicy::t_HandleRT2D 
        createRenderTarget2D(uint w,uint h,uint flags,uint num)
      {
        bool isdepth = (DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::isdepth != 0);
        sl_assert(num <= DX10_MAX_RENDER_TARGETS);
        sl_assert((!isdepth) || (num == 1)); // no multiple targets if depth texture

        T_APIPolicy::t_HandleRT2D r;

        r.flags           = flags;
        r.numtargets      = num;
        r.previous_depth  = NULL;
        ForIndex(n,DX10_MAX_RENDER_TARGETS) {
          r.previous  [n] = NULL;
          r.textures  [n] = NULL;
          r.targets   [n] = NULL;
          r.resources[n] = NULL;
        }

        ForIndex(n,num) {

          D3D10_TEXTURE2D_DESC             tex2dDesc;
          D3D10_SHADER_RESOURCE_VIEW_DESC  tex2dRVDesc;
          D3D10_RENDER_TARGET_VIEW_DESC    tex2dRTDesc;

          uint num_miplvls = 0;
          if (flags & GPUTEX_AUTOGEN_MIPMAP) {
            uint maxsz       = Math::max(w,h);
            while (maxsz > 0) { // compute required number of levels
              maxsz = maxsz >> 1;
              num_miplvls ++;
            }
          } else {
            num_miplvls = 1;
          }

          if (flags & GPUTEX_INTEGER) {
            tex2dDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_int;
          } else {
            tex2dDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_norm;
          }
          tex2dDesc.ArraySize			      = 1;
          tex2dDesc.Width				        = w;
          tex2dDesc.Height			        = h;
          tex2dDesc.MipLevels			      = num_miplvls;
          tex2dDesc.MiscFlags			      = (flags & GPUTEX_AUTOGEN_MIPMAP ? D3D10_RESOURCE_MISC_GENERATE_MIPS : 0);
          tex2dDesc.SampleDesc.Count    = 1;
          tex2dDesc.SampleDesc.Quality  = 0;
          tex2dDesc.CPUAccessFlags	    = 0;
          tex2dDesc.Usage				        = D3D10_USAGE_DEFAULT;
          tex2dDesc.BindFlags			      = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
          LIBSL_D3D_CHECK_ERROR( LIBSL_D3D_DEVICE->CreateTexture2D( &tex2dDesc, NULL, &(r.textures[n]) ) );

          // -> create shader resource view
          if (flags & GPUTEX_INTEGER) {
            tex2dRVDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_int;
          } else {
            tex2dRVDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_norm;
          }
          tex2dRVDesc.ViewDimension             = D3D10_SRV_DIMENSION_TEXTURE2D;
          tex2dRVDesc.Texture2D.MipLevels       = num_miplvls;
          tex2dRVDesc.Texture2D.MostDetailedMip = 0;
          LIBSL_D3D_CHECK_ERROR( LIBSL_D3D_DEVICE->CreateShaderResourceView( r.textures[n], &tex2dRVDesc, &r.resources[n] ) );

          // -> create render target view
          if (flags & GPUTEX_INTEGER) {
            tex2dRTDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_int;
          } else {
            tex2dRTDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_norm;
          }
          tex2dRTDesc.ViewDimension             = D3D10_RTV_DIMENSION_TEXTURE2D;
          tex2dRTDesc.Texture2D.MipSlice        = 0;
          LIBSL_D3D_CHECK_ERROR( LIBSL_D3D_DEVICE->CreateRenderTargetView( r.textures[n], &tex2dRTDesc, &r.targets[n] ) );
        }

        return (r);
      }


      /// Read back 2D render target data
      static void readBackRenderTarget2D(const typename T_APIPolicy::t_HandleRT2D& rt,
        t_PixelArray2D& array,
        uint target)
      {
        sl_assert(target < rt.numtargets);
        sl_assert(rt.textures[target] != NULL);
        // makes sure Tuple have the correct size (read back relies on pointers)
        sl_assert(sizeof(T_PixelFormat) == sizeof(typename T_PixelFormat::t_Element)*T_PixelFormat::e_Size);

        // create temporary ressource for readback
        D3D10_TEXTURE2D_DESC             srcDesc;
        rt.textures[target]->GetDesc(&srcDesc);
        D3D10_TEXTURE2D_DESC             tex2dDesc;
        tex2dDesc.Format              = srcDesc.Format;
        tex2dDesc.ArraySize			      = 1;
        tex2dDesc.Width				        = srcDesc.Width;
        tex2dDesc.Height			        = srcDesc.Height;
        tex2dDesc.MipLevels			      = srcDesc.MipLevels;
        tex2dDesc.MiscFlags			      = 0;
        tex2dDesc.SampleDesc.Count    = 1;
        tex2dDesc.SampleDesc.Quality  = 0;
        tex2dDesc.CPUAccessFlags	    = D3D10_CPU_ACCESS_READ;
        tex2dDesc.Usage				        = D3D10_USAGE_STAGING;
        tex2dDesc.BindFlags			      = 0;
        
        ID3D10Texture2D *tmp_tex = NULL;
        LIBSL_D3D_CHECK_ERROR( LIBSL_D3D_DEVICE->CreateTexture2D( &tex2dDesc, NULL, &(tmp_tex) ) );

        LIBSL_D3D_DEVICE->CopyResource(tmp_tex,rt.textures[target]);

        D3D10_MAPPED_TEXTURE2D mapping;
        LIBSL_D3D_CHECK_ERROR( tmp_tex->Map(0,D3D10_MAP_READ,0,&mapping) );
        const typename T_PixelFormat::t_Element *data = (const typename T_PixelFormat::t_Element *)mapping.pData;
        uint stride = mapping.RowPitch / sizeof(T_PixelFormat::t_Element);
        ForArray2D(array,i,j) {
          ForIndex(n,T_PixelFormat::e_Size) {
            array.set(i,j)[n] = (data[i*T_PixelFormat::e_Size + j*stride + n]);
          }
        }
        tmp_tex->Unmap(0);

        LIBSL_D3D_SAFE_RELEASE(tmp_tex);
      }


      /// Bind 2D render target
      static void bindRenderTarget2D(typename T_APIPolicy::t_HandleRT2D& rt)
      {
        bool isdepth = (DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::isdepth != 0);
        if (isdepth) {
          // depth
          sl_assert(false);
        } else {
          // color
          // -> store previous targets
          LIBSL_D3D_DEVICE->OMGetRenderTargets(DX10_MAX_RENDER_TARGETS,rt.previous,&rt.previous_depth);
          // -> set new targets
          LIBSL_D3D_DEVICE->OMSetRenderTargets(rt.numtargets,rt.targets,NULL);
        }
      }

      /// Unbind 2D render target
      static void unbindRenderTarget2D(typename T_APIPolicy::t_HandleRT2D& rt)
      {
        bool isdepth = (DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::isdepth != 0);
        if (isdepth) {
          // depth
          sl_assert(false);
        } else {
          // color
          // -> set previous back
          LIBSL_D3D_DEVICE->OMSetRenderTargets(DX10_MAX_RENDER_TARGETS,rt.previous,rt.previous_depth);
          // -> release previous
          ForIndex(n,DX10_MAX_RENDER_TARGETS) {
            LIBSL_D3D_SAFE_RELEASE( rt.previous[n] );
          }
          LIBSL_D3D_SAFE_RELEASE( rt.previous_depth );
          // -> gen mipmaps if needed
          if (rt.flags & GPUTEX_AUTOGEN_MIPMAP) {
            ForIndex(n,rt.numtargets) {
              LIBSL_D3D_DEVICE->GenerateMips(rt.resources[n]);
            }
          }
        }
      }


      /// Clear 2D render target
      /// TODO: clear to a given color
      static void clearRenderTarget2D(typename T_APIPolicy::t_HandleRT2D& rt,const T_PixelFormat& v)
      {
        bool isdepth = (DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::isdepth != 0);
        if (isdepth) {
          // depth
          sl_assert(false);
          // LIBSL_D3D_DEVICE->ClearDepthStencilView( rt.targets[0] , D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0, 0 );
        } else {
          // color
          float clr[4] = {0,0,0,0};
          ForIndex(i,T_PixelFormat::e_Size) {
            clr[i] = v[i];
          }
          ForIndex(n,rt.numtargets) {
            LIBSL_D3D_DEVICE->ClearRenderTargetView( rt.targets[n] , clr );
          }
        }
      }


      /// Destroy 2D render target
      static void destroyRenderTarget2D(typename T_APIPolicy::t_HandleRT2D& rt)
      {
        ForIndex(n,rt.numtargets) {
          LIBSL_D3D_SAFE_RELEASE( rt.resources[n] );
          LIBSL_D3D_SAFE_RELEASE( rt.targets  [n] );
          LIBSL_D3D_SAFE_RELEASE( rt.textures [n] );
        }
      }




      /// Create 3D texture
      static 
        typename T_APIPolicy::t_Handle3D
        create3D(uint w,uint h,uint d,uint flags)
      {
        // NYI
        sl_assert(false);
      }


      /// Send 3D texture to GPU memory
      static 
        void
        send3D(typename T_APIPolicy::t_Handle3D tex,
        const t_PixelArray3D& array,
        uint /*flags - UNUSED for now */)
      {
        // NYI
        sl_assert(false);
      }

      /// Send a subpart of a 3D texture to the GPU
      static 
        void
        sendSub3D(typename T_APIPolicy::t_Handle3D id,
        uint x,uint y,uint z,
        const t_PixelArray3D& array,
        uint /*flags - UNUSED for now */)
      {
        // NYI
        sl_assert(false);
      }

      /// Destroy 3D texture
      static void destroy3D(typename T_APIPolicy::t_Handle3D handle)
      {

      }


      /// Create 2D texture array
      static 
        typename T_APIPolicy::t_Handle2DArray 
        create2DArray(const LibSL::Memory::Array::Array<t_PixelArray2D>& texarray,uint flags)
      {
        T_APIPolicy::t_Handle2DArray r;

        D3D10_TEXTURE2D_DESC             tex2dDesc;
        D3D10_SHADER_RESOURCE_VIEW_DESC  tex2dRVDesc;
        Array<D3D10_SUBRESOURCE_DATA>    texData;

        uint num_miplvls = 0;
        if (flags & GPUTEX_AUTOGEN_MIPMAP) {
          uint maxsz       = Math::max(texarray[0].xsize(),texarray[0].ysize());
          while (maxsz > 0) { // compute required number of levels
            maxsz = maxsz >> 1;
            num_miplvls ++;
          }
        } else {
          num_miplvls = 1;
        }
        texData.allocate(texarray.size()*num_miplvls);
        ForIndex(n,texarray.size()) {
          ForIndex(l,num_miplvls) {
            uint lvl_w = Math::max(1,array.xsize() >> l);
            uint lvl_h = Math::max(1,array.ysize() >> l);
            // NOTE: We rely on GenerateMips to compute coarse mip levels.
            //       For init, we simply use data from finest level - just because
            //       we have to give something to DX10
            texData[l].pSysMem          = (uchar *)(texarray.raw()); 
            texData[l].SysMemPitch      = lvl_w * sizeof(typename T_PixelFormat::t_Element) * T_PixelFormat::e_Size;
            texData[l].SysMemSlicePitch = 0;
          }
        }

        if (flags & GPUTEX_INTEGER) {
          tex2dDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_int;
        } else {
          tex2dDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_norm;
        }
        tex2dDesc.ArraySize			      = texarray.size();
        tex2dDesc.Width				        = texarray[0].xsize();
        tex2dDesc.Height			        = texarray[0].ysize();
        tex2dDesc.MipLevels			      = num_miplvls;
        tex2dDesc.MiscFlags			      = (flags & GPUTEX_AUTOGEN_MIPMAP ? D3D10_RESOURCE_MISC_GENERATE_MIPS : 0);
        tex2dDesc.SampleDesc.Count    = 1;
        tex2dDesc.SampleDesc.Quality  = 0;
        tex2dDesc.CPUAccessFlags	    = 0;
        tex2dDesc.Usage				        = D3D10_USAGE_DEFAULT;
        tex2dDesc.BindFlags			      = (flags & GPUTEX_AUTOGEN_MIPMAP ? D3D10_BIND_RENDER_TARGET  : 0) | D3D10_BIND_SHADER_RESOURCE;
        r.tex  = NULL;
        r.view = NULL;
        LIBSL_D3D_CHECK_ERROR( LIBSL_D3D_DEVICE->CreateTexture2D( &tex2dDesc, texData.raw(), &(r.tex) ) );

        // -> create 2d texture view
        if (flags & GPUTEX_INTEGER) {
          tex2dRVDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_int;
        } else {
          tex2dRVDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_norm;
        }
        tex2dRVDesc.ViewDimension                  = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
        tex2dRVDesc.Texture2DArray.MipLevels       = num_miplvls;
        tex2dRVDesc.Texture2DArray.MostDetailedMip = 0;
        tex2dRVDesc.Texture2DArray.ArraySize       = texarray.size();
        tex2dRVDesc.Texture2DArray.FirstArraySlice = 0;
        LIBSL_D3D_CHECK_ERROR( LIBSL_D3D_DEVICE->CreateShaderResourceView( r.tex, &tex2dRVDesc, &r.view ) );

        if (flags & GPUTEX_AUTOGEN_MIPMAP) {
          LIBSL_D3D_DEVICE->GenerateMips(r.view);
        }

        return (r);
      }

      /// Create 2D texture array
      static 
        typename T_APIPolicy::t_Handle2DArray 
        create2DArray(const LibSL::Memory::Array::Array<LibSL::Memory::Array::Array<t_PixelArray2D> >& texmiparray,uint flags)
      {
        T_APIPolicy::t_Handle2DArray r;

        sl_assert(!(flags & GPUTEX_AUTOGEN_MIPMAP));

        D3D10_TEXTURE2D_DESC             tex2dDesc;
        D3D10_SHADER_RESOURCE_VIEW_DESC  tex2dRVDesc;
        Array<D3D10_SUBRESOURCE_DATA>    texData;

        uint num_miplvls = texmiparray[0].size();
        texData.allocate(texmiparray.size()*num_miplvls);
        ForIndex(n,texmiparray.size()) {
          ForIndex(l,texmiparray[n].size()) {
            uint lvl_w = texmiparray[n][l].xsize();
            uint lvl_h = texmiparray[n][l].ysize();
            texData[n*texmiparray[n].size()+l].pSysMem          = (uchar *)(texmiparray[n][l].raw()); 
            texData[n*texmiparray[n].size()+l].SysMemPitch      = lvl_w * sizeof(typename T_PixelFormat::t_Element) * T_PixelFormat::e_Size;
            texData[n*texmiparray[n].size()+l].SysMemSlicePitch = 0;
          }
        }

        if (flags & GPUTEX_INTEGER) {
          tex2dDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_int;
        } else {
          tex2dDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_norm;
        }
        tex2dDesc.ArraySize			      = texmiparray.size();
        tex2dDesc.Width				        = texmiparray[0][0].xsize();
        tex2dDesc.Height			        = texmiparray[0][0].ysize();
        tex2dDesc.MipLevels			      = num_miplvls;
        tex2dDesc.MiscFlags			      = 0;
        tex2dDesc.SampleDesc.Count    = 1;
        tex2dDesc.SampleDesc.Quality  = 0;
        tex2dDesc.CPUAccessFlags	    = 0;
        tex2dDesc.Usage				        = D3D10_USAGE_DEFAULT;
        tex2dDesc.BindFlags			      = D3D10_BIND_SHADER_RESOURCE;
        r.tex  = NULL;
        r.view = NULL;
        LIBSL_D3D_CHECK_ERROR( LIBSL_D3D_DEVICE->CreateTexture2D( &tex2dDesc, texData.raw(), &(r.tex) ) );

        // -> create 2d texture view
        if (flags & GPUTEX_INTEGER) {
          tex2dRVDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_int;
        } else {
          tex2dRVDesc.Format = (DXGI_FORMAT)DX10_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::dx10fmt_norm;
        }
        tex2dRVDesc.ViewDimension                  = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
        tex2dRVDesc.Texture2DArray.MipLevels       = num_miplvls;
        tex2dRVDesc.Texture2DArray.MostDetailedMip = 0;
        tex2dRVDesc.Texture2DArray.ArraySize       = texmiparray.size();
        tex2dRVDesc.Texture2DArray.FirstArraySlice = 0;
        LIBSL_D3D_CHECK_ERROR( LIBSL_D3D_DEVICE->CreateShaderResourceView( r.tex, &tex2dRVDesc, &r.view ) );

        return (r);
      }

      /// Destroy 2D texture array
      static void destroy2DArray(typename T_APIPolicy::t_Handle2DArray r)
      {
        LIBSL_D3D_SAFE_RELEASE(r.tex);
        LIBSL_D3D_SAFE_RELEASE(r.view);
      }

    };

  } // namespace LibSL::GPUTex
} // namespace LibSL

// ------------------------------------------------------
