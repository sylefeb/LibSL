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
// LibSL::GPUTex
// ------------------------------------------------------
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-24
// ------------------------------------------------------

#pragma once

// ------------------------------------------------------

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/Memory/TraceLeaks.h>

#include <LibSL/System/Types.h>
using namespace LibSL::System::Types;

#include <LibSL/Math/Tuple.h>

#include <LibSL/Memory/Pointer.h>
#include <LibSL/Memory/Array2D.h>

// ------------------------------------------------------

typedef LibSL::Math::Tuple<uchar,4> depth32;
typedef LibSL::Math::Tuple<uchar,3> depth24;
typedef LibSL::Math::Tuple<uchar,2> depth16;

// ------------------------------------------------------

#define GPUTEX_AUTOGEN_MIPMAP 1
#define GPUTEX_INTEGER        64

// ------------------------------------------------------

namespace LibSL  {
  namespace GPUTex {

    /// Texture 2D interface
    template <class T_APIPolicy>
    class Tex2D_interface
      //      : public LibSL::Memory::TraceLeaks::LeakProbe<Tex2D_interface<T_APIPolicy> >
    {
    public:

      typedef T_APIPolicy                                      t_API;
      typedef typename T_APIPolicy::t_APIHandle2D              t_APIHandle;
      typedef typename T_APIPolicy::t_Handle2D                 t_Handle;
      typedef LibSL::Memory::Pointer::AutoPtr<Tex2D_interface> t_AutoPtr;

    protected:

      virtual ~Tex2D_interface() {}

      t_Handle m_Handle;
      uint     m_W;
      uint     m_H;

      void setW(uint w)            { m_W=w; }
      void setH(uint h)            { m_H=h; }
      void setHandle(t_Handle hdl) { m_Handle = hdl; }

    public:

      t_Handle     handle()  const {return (m_Handle);}
      t_APIHandle  texture() const {return T_APIPolicy::apiHandle2D(m_Handle);}
      void         bind()    { T_APIPolicy::bindTexture2D(m_Handle); }
      uint w()     const     {return (m_W);}
      uint h()     const     {return (m_H);}
    };

    /// Texture 2D generic
    template <
      class T_APIPolicy,
        template <class,class> class T_CompilePolicy,
      class T_PixelFormat>
    class Tex2D_generic : public Tex2D_interface<T_APIPolicy>
    {
    public:

      typedef T_CompilePolicy<T_APIPolicy,T_PixelFormat>     t_CompilePolicy;
      typedef T_PixelFormat                                  t_PixelFormat;
      typedef T_PixelFormat                                  t_Pixel;
      typedef typename t_CompilePolicy::t_PixelArray2D       t_PixelArray;
      typedef typename T_APIPolicy::t_Handle2D               t_Handle;
      typedef LibSL::Memory::Pointer::AutoPtr<Tex2D_generic> t_AutoPtr;

    private:

      Tex2D_generic(const Tex2D_generic&); // disable copy constructor

    protected:

      uint m_Flags;

    public:

      Tex2D_generic(const t_PixelArray& array,uint flags)
      {
        m_Flags=flags;
        Tex2D_interface<T_APIPolicy>::setW(array.xsize());
        Tex2D_interface<T_APIPolicy>::setH(array.ysize());
        LIBSL_CATCH_AND_BOUNCE(
          Tex2D_interface<T_APIPolicy>::setHandle(
            t_CompilePolicy::create2D(array,m_Flags)
          )
        );
      }

      Tex2D_generic(const LibSL::Memory::Array::Array<t_PixelArray>& miparray,uint flags)
      {
        m_Flags = flags;
        Tex2D_interface<T_APIPolicy>::setW(miparray[0].xsize());
        Tex2D_interface<T_APIPolicy>::setH(miparray[0].ysize());
        LIBSL_CATCH_AND_BOUNCE(
          Tex2D_interface<T_APIPolicy>::setHandle(
            t_CompilePolicy::create2D(miparray,m_Flags)
          )
        );
      }

      void update(const t_PixelArray& array)
      {
        LIBSL_CATCH_AND_BOUNCE( t_CompilePolicy::send2D(Tex2D_interface<T_APIPolicy>::handle(),array,m_Flags) );
      }

      void update(uint x,uint y,const t_PixelArray& array)
      {
        sl_assert(x+array.xsize() <= Tex2D_interface<T_APIPolicy>::w());
        sl_assert(y+array.ysize() <= Tex2D_interface<T_APIPolicy>::h());
        LIBSL_CATCH_AND_BOUNCE( t_CompilePolicy::sendSub2D(Tex2D_interface<T_APIPolicy>::handle(),x,y,array,m_Flags) );
      }

      virtual ~Tex2D_generic()
      {
        t_CompilePolicy::destroy2D(Tex2D_interface<T_APIPolicy>::handle());
      }

    };

    /// Render target 2D interface
    template <class T_APIPolicy>
    class RenderTarget2D_interface
      //      : public LibSL::Memory::TraceLeaks::LeakProbe<RenderTarget2D_interface<T_APIPolicy> >
    {
    public:

      typedef typename T_APIPolicy::t_HandleRT2D  t_HandleRT;
      typedef typename T_APIPolicy::t_APIHandle2D t_APIHandle;
      typedef LibSL::Memory::Pointer::Pointer<
        RenderTarget2D_interface,
        LibSL::Memory::Pointer::CheckValid,
        LibSL::Memory::Pointer::TransferRefCountUInt> t_AutoPtr;

    protected:

      t_HandleRT m_Handle;
      uint       m_W;
      uint       m_H;

      void setW(uint w) {m_W=w;}
      void setH(uint h) {m_H=h;}
      void setHandle(t_HandleRT hdl) {m_Handle=hdl;}

    public:

      virtual ~RenderTarget2D_interface() {}

      t_HandleRT& handle()               {return (m_Handle);}
      t_APIHandle texture(uint target=0) {return (T_APIPolicy::renderTarget2DTexture(m_Handle,target));}

      uint w() const {return (m_W);}
      uint h() const {return (m_H);}

    };

    /// Render target 2D generic
    template <
      class T_APIPolicy,
        template <class,class> class T_CompilePolicy,
      class T_PixelFormat>
    class RenderTarget2D_generic : public RenderTarget2D_interface<T_APIPolicy>
    {

    public:

      typedef T_CompilePolicy<T_APIPolicy,T_PixelFormat>              t_CompilePolicy;
      typedef typename t_CompilePolicy::t_PixelArray2D                t_PixelArray;
      typedef typename T_APIPolicy::t_HandleRT2D                      t_HandleRT;
      typedef LibSL::Memory::Pointer::AutoPtr<RenderTarget2D_generic> t_AutoPtr;

    private:

      RenderTarget2D_generic(const RenderTarget2D_generic&); // disable copy constructor

    public:

      RenderTarget2D_generic(uint w,uint h,uint flags=0,uint num=1)
      {
        RenderTarget2D_interface<T_APIPolicy>::setW(w);
        RenderTarget2D_interface<T_APIPolicy>::setH(h);
        RenderTarget2D_interface<T_APIPolicy>::setHandle(t_CompilePolicy::createRenderTarget2D(w,h,flags,num));
      }

      ~RenderTarget2D_generic()
      {
        t_CompilePolicy::destroyRenderTarget2D(RenderTarget2D_interface<T_APIPolicy>::handle());
      }

      void bind()
      {
        t_CompilePolicy::bindRenderTarget2D(RenderTarget2D_interface<T_APIPolicy>::handle());
      }

      void unbind()
      {
        t_CompilePolicy::unbindRenderTarget2D(RenderTarget2D_interface<T_APIPolicy>::handle());
      }

      void clear(const T_PixelFormat& v=0)
      {
        t_CompilePolicy::clearRenderTarget2D(RenderTarget2D_interface<T_APIPolicy>::handle(),v);
      }

      void readBack(t_PixelArray& array,uint target=0)
      {
        if (!array.empty()) {
          if ( array.xsize() != RenderTarget2D_interface<T_APIPolicy>::w() || array.ysize() != RenderTarget2D_interface<T_APIPolicy>::h()) {
            throw LibSL::Errors::Fatal("RenderTarget2D::readBack - array is not empty and has wrong size!");
          }
        } else {
          array.allocate(RenderTarget2D_interface<T_APIPolicy>::w(),RenderTarget2D_interface<T_APIPolicy>::h());
        }
        t_CompilePolicy::readBackRenderTarget2D(RenderTarget2D_interface<T_APIPolicy>::handle(),array,target);
      }

    };

    /// Texture 3D interface
    template <class T_APIPolicy>
    class Tex3D_interface
    {
    public:

      typedef typename T_APIPolicy::t_APIHandle3D              t_APIHandle;
      typedef typename T_APIPolicy::t_Handle3D                 t_Handle;
      typedef LibSL::Memory::Pointer::AutoPtr<Tex3D_interface> t_AutoPtr;

    protected:

      t_Handle m_Handle;
      uint     m_W;
      uint     m_H;
      uint     m_D;

      void setW(uint w) {m_W=w;}
      void setH(uint h) {m_H=h;}
      void setD(uint d) {m_D=d;}
      void setHandle(t_Handle hdl) {m_Handle=hdl;}

    public:

      virtual ~Tex3D_interface() {}

      t_Handle handle()   {return (m_Handle);}
      t_Handle texture()  {return T_APIPolicy::apiHandle3D(m_Handle);}
      uint w() const {return (m_W);}
      uint h() const {return (m_H);}
      uint d() const {return (m_D);}

    };

    /// Texture 3D generic
    template <
      class T_APIPolicy,
        template <class,class> class T_CompilePolicy,
      class T_PixelFormat>
    class Tex3D_generic : public Tex3D_interface<T_APIPolicy>
    {
    public:

      typedef T_CompilePolicy<T_APIPolicy,T_PixelFormat>     t_CompilePolicy;
      typedef typename t_CompilePolicy::t_PixelArray3D       t_PixelArray;
      typedef typename T_APIPolicy::t_Handle3D               t_Handle;
      typedef LibSL::Memory::Pointer::AutoPtr<Tex3D_generic> t_AutoPtr;

    protected:

      uint m_Flags;

    private:

      Tex3D_generic(const Tex3D_generic&); // disable copy constructor

    public:

      Tex3D_generic(const t_PixelArray& array,uint flags)
      {
        m_Flags=flags;
        Tex3D_interface<T_APIPolicy>::setW(array.xsize());
        Tex3D_interface<T_APIPolicy>::setH(array.ysize());
        Tex3D_interface<T_APIPolicy>::setD(array.zsize());
        LIBSL_CATCH_AND_BOUNCE( Tex3D_interface<T_APIPolicy>::setHandle(t_CompilePolicy::create3D(array,m_Flags)) );
      }

      void update(const t_PixelArray& array)
      {
        LIBSL_CATCH_AND_BOUNCE( t_CompilePolicy::send3D(Tex3D_interface<T_APIPolicy>::handle(),array,m_Flags) );
      }

      void update(uint x,uint y,uint z,const t_PixelArray& array)
      {
        sl_assert(x+array.xsize() <= Tex3D_interface<T_APIPolicy>::w());
        sl_assert(y+array.ysize() <= Tex3D_interface<T_APIPolicy>::h());
        sl_assert(z+array.zsize() <= Tex3D_interface<T_APIPolicy>::d());
        LIBSL_CATCH_AND_BOUNCE( t_CompilePolicy::sendSub3D(Tex3D_interface<T_APIPolicy>::handle(),x,y,z,array,m_Flags) );
      }

      virtual ~Tex3D_generic()
      {
        t_CompilePolicy::destroy3D(Tex3D_interface<T_APIPolicy>::handle());
      }

    };




    /// Cube map
    class CubeMap
    {

    };

    /// Cube map render target
    class CubeMapRenderTarget
    {

    };


    /// Texture 2D Array interface
    template <class T_APIPolicy>
    class Tex2DArray_interface
      //      : public LibSL::Memory::TraceLeaks::LeakProbe<Tex2DArray_interface<T_APIPolicy> >
    {
    public:

      typedef typename T_APIPolicy::t_APIHandle2DArray              t_APIHandle;
      typedef typename T_APIPolicy::t_Handle2DArray                 t_Handle;
      typedef LibSL::Memory::Pointer::AutoPtr<Tex2DArray_interface> t_AutoPtr;

    protected:

      t_Handle m_Handle;
      uint     m_W;
      uint     m_H;
      uint     m_Size;

      void setW(uint w)    {m_W=w;}
      void setH(uint h)    {m_H=h;}
      void setSize(uint n) {m_Size=n;}
      void setHandle(t_Handle hdl) { m_Handle = hdl; }

    public:

      virtual ~Tex2DArray_interface() {}

      t_Handle     handle()  {return (m_Handle);}
      t_APIHandle  texture() {return T_APIPolicy::apiHandle2DArray(m_Handle);}
      uint w()     const {return (m_W);}
      uint h()     const {return (m_H);}
    };

    /// Texture 2D array generic
    template <
      class T_APIPolicy,
        template <class,class> class T_CompilePolicy,
      class T_PixelFormat>
    class Tex2DArray_generic : public Tex2DArray_interface<T_APIPolicy>
    {
    public:

      typedef T_CompilePolicy<T_APIPolicy,T_PixelFormat>     t_CompilePolicy;
      typedef T_PixelFormat                                  t_PixelFormat;
      typedef typename t_CompilePolicy::t_PixelArray2D       t_PixelArray;
      typedef typename T_APIPolicy::t_Handle2D               t_Handle;
      typedef LibSL::Memory::Pointer::AutoPtr<Tex2DArray_generic> t_AutoPtr;

    protected:

      uint m_Flags;

    private:

      Tex2DArray_generic(const Tex2DArray_generic&); // disable copy constructor

    public:

      Tex2DArray_generic(const LibSL::Memory::Array::Array<t_PixelArray>& texarray,uint flags)
      {
        m_Flags = flags;
        sl_assert(texarray.size() > 0);
        Tex2DArray_interface<T_APIPolicy>::setSize(texarray.size());
        Tex2DArray_interface<T_APIPolicy>::setW   (texarray[0].xsize());
        Tex2DArray_interface<T_APIPolicy>::setH   (texarray[0].ysize());
        LIBSL_CATCH_AND_BOUNCE(
          Tex2DArray_interface<T_APIPolicy>::setHandle(
            t_CompilePolicy::create2DArray(texarray,m_Flags)
          )
        );
      }

      Tex2DArray_generic(const LibSL::Memory::Array::Array<LibSL::Memory::Array::Array<t_PixelArray> >& texmiparray,uint flags)
      {
        m_Flags = flags;
        sl_assert(texmiparray.size() > 0);
        Tex2DArray_interface<T_APIPolicy>::setSize(texmiparray.size());
        Tex2DArray_interface<T_APIPolicy>::setW   (texmiparray[0][0].xsize());
        Tex2DArray_interface<T_APIPolicy>::setH   (texmiparray[0][0].ysize());
        LIBSL_CATCH_AND_BOUNCE(
          Tex2DArray_interface<T_APIPolicy>::setHandle(
            t_CompilePolicy::create2DArray(texmiparray,m_Flags)
          )
        );
      }

      ~Tex2DArray_generic()
      {
        t_CompilePolicy::destroy2DArray(Tex2DArray_interface<T_APIPolicy>::handle());
      }

    };


  } // namespace LibSL::GPUTex
} // namespace LibSL

// ------------------------------------------------------


#ifndef LIBSL_NO_GRAPHICS_API

#ifdef DIRECT3D
//#include <LibSL/LibSL_d3d.h>
#include <LibSL/GPUTex/GPUTex_d3d.h>
#endif

#ifdef DIRECTX10
//#include <LibSL/LibSL_dx10.h>
#include <LibSL/GPUTex/GPUTex_dx10.h>
#endif

#ifdef OPENGL
#ifdef OPENGL4
//#include <LibSL/LibSL_gl4.h>
#include <LibSL/GPUTex/GPUTex_gl.h>
#else
//#include <LibSL/LibSL_gl.h>
#include <LibSL/GPUTex/GPUTex_gl.h>
#endif
#endif

#endif


// ------------------------------------------------------
