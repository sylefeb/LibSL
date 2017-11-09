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
// LibSL::GPUTex::D3D_CompilePolicy
// ------------------------------------------------------
//
// TODO: 2d render targets, 3d textures
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-06-05
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

#include <d3d9.h>
#include <d3dx9.h>

#include <LibSL/GPUHelpers/GPUHelpers.h>
#include <LibSL/D3DHelpers/D3DHelpers.h>

#include <LibSL/GPUTex/GPUTex_D3D_formats.h>

// ------------------------------------------------------

namespace LibSL  {
  namespace GPUTex {

    enum {D3D_MAX_RENDER_TARGETS=4};

    /// API policy
    class D3D_APIPolicy
    {
    public:

      /// Handle to textures
      typedef LPDIRECT3DTEXTURE9        t_APIHandle2D;
      typedef LPDIRECT3DVOLUMETEXTURE9  t_APIHandle3D;

      typedef LPDIRECT3DTEXTURE9        t_Handle2D;
      typedef LPDIRECT3DVOLUMETEXTURE9  t_Handle3D;
      typedef LPDIRECT3DCUBETEXTURE9    t_HandleCubeMap;

      typedef struct s_HandleRT2D {
        LPDIRECT3DTEXTURE9 textures[D3D_MAX_RENDER_TARGETS];
        LPDIRECT3DSURFACE9 targets[D3D_MAX_RENDER_TARGETS];
        LPDIRECT3DSURFACE9 previous[D3D_MAX_RENDER_TARGETS]; // Use to protect previous targets
      } t_HandleRT2D;

      static void               bindTexture2D(const t_APIHandle2D& handle) 
      { LIBSL_DIRECT3D_DEVICE->SetTexture(0,handle); }

      static t_APIHandle2D renderTarget2DTexture(const t_HandleRT2D& rt,uint target) 
      {sl_assert(target<D3D_MAX_RENDER_TARGETS); return rt.textures[target];}

      static t_APIHandle2D apiHandle2D(t_Handle2D h) { return t_APIHandle2D(h); }
      static t_APIHandle3D apiHandle3D(t_Handle3D h) { return t_APIHandle3D(h); }

    };


    /// Compile policy
    template <class T_APIPolicy,class T_PixelFormat>
    class D3D_CompilePolicy
    {
    public:

      typedef LibSL::Memory::Array::Array<T_PixelFormat>   t_PixelArray1D;
      typedef LibSL::Memory::Array::Array2D<T_PixelFormat> t_PixelArray2D;
      typedef LibSL::Memory::Array::Array3D<T_PixelFormat> t_PixelArray3D;

      /// Create 2D texture
      static typename T_APIPolicy::t_Handle2D 
        create2D(const t_PixelArray2D& array,uint flags)
      {
        LPDIRECT3DTEXTURE9 tex = NULL;
        LPDIRECT3DDEVICE9  d3d = LIBSL_DIRECT3D_DEVICE;
        //std::cerr << LibSL::CppHelpers::sprint(" D3D create2D %dx%d\n",w,h);
        LIBSL_D3D_CHECK_ERROR(d3d->CreateTexture(
          array.xsize(),array.ysize(),
          0,
          (flags & GPUTEX_AUTOGEN_MIPMAP) ? D3DUSAGE_AUTOGENMIPMAP : 0,
          (D3DFORMAT)D3D_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::d3dfmt,
          D3DPOOL_MANAGED,
          &tex,NULL));
        send2D(tex,array,flags);
        return (tex);
      }

      /// Create 2D texture
      static typename T_APIPolicy::t_Handle2D 
        create2D(const LibSL::Memory::Array::Array<t_PixelArray2D>& miparray,uint flags)
      {
        LPDIRECT3DTEXTURE9 tex = NULL;
        LPDIRECT3DDEVICE9  d3d = LIBSL_DIRECT3D_DEVICE;
        // std::cerr << LibSL::CppHelpers::sprint(" D3D create2D %dx%d from mipmap array\n",miparray[0].xsize(),miparray[0].ysize());
        LIBSL_D3D_CHECK_ERROR(d3d->CreateTexture(
          miparray[0].xsize(),miparray[0].ysize(),
          miparray.size(),
          0,
          (D3DFORMAT)D3D_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::d3dfmt,
          D3DPOOL_MANAGED,
          &tex,NULL));
        send2Dmipmap(tex,miparray,flags);
        return (tex);
      }

      /// Send 2D texture to GPU memory
      static void
        send2D(typename T_APIPolicy::t_Handle2D tex,
        const t_PixelArray2D& array,
        uint /*flags - UNUSED for now */)
      {
        // NOTE: if this fail, it means the compiler is building non-byte aligned Tuples
        sl_assert(sizeof(T_PixelFormat) == sizeof(typename T_PixelFormat::t_Element)*T_PixelFormat::e_Size);
        // lock
        D3DLOCKED_RECT rect;
        LIBSL_D3D_CHECK_ERROR(tex->LockRect(0,&rect,NULL,D3DLOCK_DISCARD));
        typename T_PixelFormat::t_Element *data = (typename T_PixelFormat::t_Element *)rect.pBits;
        uint stride = rect.Pitch/sizeof(T_PixelFormat::t_Element);
        ForArray2D(array,i,j) {
          ForIndex(n,T_PixelFormat::e_Size) {
            uint o = D3D_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::ordering(n);
            data[i*T_PixelFormat::e_Size + j*stride + n] = array.get(i,j)[o];
          }
        }
        LIBSL_D3D_CHECK_ERROR(tex->UnlockRect(0));
      }


      /// Send 2D texture to GPU memory with mipmap levels
      static 
        void
        send2Dmipmap(
        typename T_APIPolicy::t_Handle2D tex,
        const LibSL::Memory::Array::Array<t_PixelArray2D>& miparray,
        uint /*flags - UNUSED for now */)
      {
        // NOTE: if this fail, it means the compiler is building non-byte aligned Tuples
        sl_assert(sizeof(T_PixelFormat) == sizeof(typename T_PixelFormat::t_Element)*T_PixelFormat::e_Size);
        ForArray(miparray,l) {
          // sanity
          D3DSURFACE_DESC desc;
          LIBSL_D3D_CHECK_ERROR(tex->GetLevelDesc(l,&desc));
          if (!(desc.Width == miparray[l].xsize() && desc.Height == miparray[l].ysize())) {
            throw Fatal("Tex2D: one of the given mipmap level has incorrect size!");
          }
          // lock level
          D3DLOCKED_RECT rect;
          LIBSL_D3D_CHECK_ERROR(tex->LockRect(l,&rect,NULL,D3DLOCK_DISCARD));
          typename T_PixelFormat::t_Element *data = (typename T_PixelFormat::t_Element *)rect.pBits;
          uint stride = rect.Pitch/sizeof(T_PixelFormat::t_Element);
          ForArray2D(miparray[l],i,j) {
            ForIndex(n,T_PixelFormat::e_Size) {
              uint o = D3D_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::ordering(n);
              data[i*T_PixelFormat::e_Size + j*stride + n] = miparray[l].get(i,j)[o];
            }
          }
          LIBSL_D3D_CHECK_ERROR(tex->UnlockRect(l));
        }
      }


      /// Send a subpart of a 2D texture to the GPU
      static 
        void
        sendSub2D(typename T_APIPolicy::t_Handle2D id,
        uint x,uint y,uint z,
        const t_PixelArray2D& array,
        uint /*flags - UNUSED for now */)
      {
        // NYI
        sl_assert(false);
      }

      /// Destroy 2D texture
      static void destroy2D(typename T_APIPolicy::t_Handle2D tex)
      {
        LIBSL_D3D_SAFE_RELEASE(tex);
      }


      /// Create 2D render target
      //   - num render targets are created (enables multiple render targets)
      //   NOTE: - it is possible to bind both a color RT and a depth RT (does NOT work with OpenGL)
      //         - depth buffers are not MIP-mapped (they have only 1 level)
      static 
        typename T_APIPolicy::t_HandleRT2D 
        createRenderTarget2D(uint w,uint h,uint flags,uint num)
      {
        bool isdepth = (D3D_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::isdepth != 0);
        sl_assert(num <= D3D_MAX_RENDER_TARGETS);
        sl_assert((!isdepth) || (num == 1)); // no multiple targets if depth texture
        typename T_APIPolicy::t_HandleRT2D rts;
        ForIndex(n,D3D_MAX_RENDER_TARGETS) {
          rts.textures[n] = NULL;
          rts.targets [n] = NULL;
          rts.previous[n] = NULL;
        }
        LPDIRECT3DDEVICE9  d3d=LIBSL_DIRECT3D_DEVICE;
        ForIndex(n,num) {
          LIBSL_D3D_CHECK_ERROR(d3d->CreateTexture(
            w,h,
            (isdepth ? 1 : 0),
            ((isdepth) ? D3DUSAGE_DEPTHSTENCIL  : D3DUSAGE_RENDERTARGET)
            | ((flags & GPUTEX_AUTOGEN_MIPMAP) ? D3DUSAGE_AUTOGENMIPMAP : 0),
            (D3DFORMAT)D3D_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::d3dfmt,
            D3DPOOL_DEFAULT,
            &(rts.textures[n]),NULL));
          LIBSL_D3D_CHECK_ERROR(rts.textures[n]->GetSurfaceLevel(0,&(rts.targets[n])));
        }
        return (rts);
      }


      /// Read back 2D render target data
      static void readBackRenderTarget2D(
        const typename T_APIPolicy::t_HandleRT2D& rt,
        t_PixelArray2D&                           array,
        uint                                      target)
      {
        sl_assert(target < D3D_MAX_RENDER_TARGETS);
        sl_assert(rt.targets[target] != NULL);
        // makes sure Tuple have the correct size (read back relies on pointers)
        sl_assert(sizeof(T_PixelFormat) == sizeof(typename T_PixelFormat::t_Element)*T_PixelFormat::e_Size);
        // read back using a temporary buffer
        LPDIRECT3DDEVICE9  d3d    = LIBSL_DIRECT3D_DEVICE;
        LPDIRECT3DTEXTURE9 tmp    = NULL;
        LPDIRECT3DSURFACE9 tmpsrf = NULL;
        // LPDIRECT3DSURFACE9 srf    = NULL;
        D3DSURFACE_DESC    desc;
        rt.targets[target]->GetDesc(&desc);
        LIBSL_D3D_CHECK_ERROR(d3d->CreateTexture(
          desc.Width,
          desc.Height,
          1,0,
          desc.Format,
          D3DPOOL_SYSTEMMEM,
          &tmp,NULL));
        LIBSL_D3D_CHECK_ERROR(tmp->GetSurfaceLevel(0,&tmpsrf));
        LIBSL_D3D_CHECK_ERROR(d3d->GetRenderTargetData(rt.targets[target],tmpsrf));
        D3DLOCKED_RECT rect;
        sl_assert( tmp != NULL );
        LIBSL_D3D_CHECK_ERROR(tmp->LockRect(0,&rect,NULL,D3DLOCK_READONLY));
        sl_assert(array.xsize() == desc.Width && array.ysize() == desc.Height);
        ForIndex(j,array.ysize()) {
          uint offset = j * rect.Pitch;
          const typename T_PixelFormat::t_Element *row =
            (const typename T_PixelFormat::t_Element *)((uchar*)rect.pBits + offset);
          ForIndex(i,array.xsize()) {
            ForIndex(n,T_PixelFormat::e_Size) {
              array.set(i,j)[n] = row[i * T_PixelFormat::e_Size + n];
            }
          }
        }
        tmp->UnlockRect(0);
        LIBSL_D3D_SAFE_RELEASE(tmpsrf);
        LIBSL_D3D_SAFE_RELEASE(tmp);
      }


      /// Bind 2D render target
      static void bindRenderTarget2D(typename T_APIPolicy::t_HandleRT2D& rt)
      {
        LPDIRECT3DDEVICE9  d3d = LIBSL_DIRECT3D_DEVICE;
        bool isdepth = (D3D_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::isdepth != 0);
        if (isdepth) {
          // depth
          LIBSL_D3D_CHECK_ERROR(d3d->GetDepthStencilSurface(&(rt.previous[0])));
          LIBSL_D3D_CHECK_ERROR(d3d->SetDepthStencilSurface(rt.targets[0]));
        } else {
          // color
          // -> store previous targets
          ForIndex(n,D3D_MAX_RENDER_TARGETS) {
            HRESULT r = d3d->GetRenderTarget(n,&(rt.previous[n]));
            if (r == D3DERR_NOTFOUND) {
              rt.previous[n] = NULL;
            } else if (r != D3D_OK) {
              sl_assert(false);
            }
          }
          // -> set new targets
          ForIndex(n,D3D_MAX_RENDER_TARGETS) {
            if (rt.targets[n] != NULL) {
              LIBSL_D3D_CHECK_ERROR(d3d->SetRenderTarget(n,rt.targets[n]));
            }
          }
        }
      }

      /// Unbind 2D render target
      static void unbindRenderTarget2D(typename T_APIPolicy::t_HandleRT2D& rt)
      {
        LPDIRECT3DDEVICE9  d3d = LIBSL_DIRECT3D_DEVICE;
        bool isdepth = (D3D_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::isdepth != 0);
        if (isdepth) {
          // depth
          LIBSL_D3D_CHECK_ERROR(d3d->SetDepthStencilSurface(rt.previous[0]));
          LIBSL_D3D_SAFE_RELEASE(rt.previous[0]);
        } else {
          // color
          ForIndex(n,D3D_MAX_RENDER_TARGETS) {
            if (rt.previous[n] != NULL) {
              // -> restore previous target
              LIBSL_D3D_CHECK_ERROR(d3d->SetRenderTarget(n,rt.previous[n]));
              LIBSL_D3D_SAFE_RELEASE(rt.previous[n]);
            } else {
              // -> no previous, set to null
              LIBSL_D3D_CHECK_ERROR(d3d->SetRenderTarget(n,NULL));
            }
          }
        }
      }


      /// Clear 2D render target
      /// TODO: clear to a given color
      static void clearRenderTarget2D(typename T_APIPolicy::t_HandleRT2D& rt,const T_PixelFormat& v)
      {
        LPDIRECT3DDEVICE9  d3d = LIBSL_DIRECT3D_DEVICE;
        bindRenderTarget2D(rt);
        d3d->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L);
        unbindRenderTarget2D(rt);
      }


      /// Destroy 2D render target
      static void destroyRenderTarget2D(typename T_APIPolicy::t_HandleRT2D& rt)
      {
        ForIndex(n,D3D_MAX_RENDER_TARGETS) {
          if (rt.textures[n] != NULL) {
            LIBSL_D3D_SAFE_RELEASE(rt.targets[n]);
            LIBSL_D3D_SAFE_RELEASE(rt.textures[n]);
          }
        }
      }




      /// Create 3D texture
      static 
        typename T_APIPolicy::t_Handle3D 
        create3D(uint w,uint h,uint d,uint flags)
      {
        LPDIRECT3DVOLUMETEXTURE9 tex=NULL;
        LPDIRECT3DDEVICE9 d3d=LIBSL_DIRECT3D_DEVICE;
        //std::cerr << LibSL::CppHelpers::sprint(" D3D create3D %dx%dx%d\n",w,h,d);
        LIBSL_D3D_CHECK_ERROR(d3d->CreateVolumeTexture(
          w,h,d,
          0,
          (flags & GPUTEX_AUTOGEN_MIPMAP) ? D3DUSAGE_AUTOGENMIPMAP : 0,
          (D3DFORMAT)D3D_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::d3dfmt,
          D3DPOOL_MANAGED,
          &tex,NULL));
        return (tex);
      }


      /// Send 3D texture to GPU memory
      static 
        void
        send3D(typename T_APIPolicy::t_Handle3D tex,
        const t_PixelArray3D& array,
        uint /*flags - UNUSED for now */)
      {

        // NOTE: if this fail, it means the compiler is building non-byte aligned Tuples
        sl_assert(sizeof(T_PixelFormat) == sizeof(typename T_PixelFormat::t_Element)*T_PixelFormat::e_Size);
        // lock
        D3DLOCKED_BOX box;
        LIBSL_D3D_CHECK_ERROR(tex->LockBox(0,&box,NULL,/*D3DLOCK_DISCARD*/0));
        typename T_PixelFormat::t_Element *data=
          (typename T_PixelFormat::t_Element *)box.pBits;
        uint stride_row  =box.RowPitch  /sizeof(T_PixelFormat::t_Element);
        uint stride_slice=box.SlicePitch/sizeof(T_PixelFormat::t_Element);
        ForArray3D(array,i,j,k) {
          ForIndex(n,T_PixelFormat::e_Size) {
            uint o=D3D_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::ordering(n);
            data[i*T_PixelFormat::e_Size + j*stride_row + + k*stride_slice + n]=array.get(i,j,k)[o];
          }
        }
        LIBSL_D3D_CHECK_ERROR(tex->UnlockBox(0));

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
        LIBSL_D3D_SAFE_RELEASE(handle);
      }


    };

  } // namespace LibSL::GPUTex
} // namespace LibSL

// ------------------------------------------------------
