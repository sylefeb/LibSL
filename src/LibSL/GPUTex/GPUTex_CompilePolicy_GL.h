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
// LibSL::GPUTex::GL_CompilePolicy
// ------------------------------------------------------
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-24
// ------------------------------------------------------

#pragma once

// ------------------------------------------------------

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#ifdef EMSCRIPTEN
#define GL_GLEXT_PROTOTYPES
#else
#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GL/gl.h>
#endif
#endif
#endif

// ------------------------------------------------------

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/GLHelpers/GLHelpers.h>

#include <LibSL/System/Types.h>
using namespace LibSL::System::Types;

#include <LibSL/Memory/Array2D.h>
#include <LibSL/Memory/Array3D.h>

// ------------------------------------------------------

#include <LibSL/GPUTex/GPUTex_GL_types.h>
#include <LibSL/GPUTex/GPUTex_GL_formats.h>

// ------------------------------------------------------

#if !defined(EMSCRIPTEN) && !defined(ANDROID)
#ifdef USE_GLUX
#include <glux.h>
#include "GL_EXT_framebuffer_object.h"
#include "GL_ARB_draw_buffers.h"
#include "GL_EXT_texture3D.h"
GLUX_LOAD(GL_EXT_framebuffer_object)
GLUX_LOAD(GL_ARB_draw_buffers)
GLUX_LOAD(GL_EXT_texture3D)
#endif // USE_GLUX
#else // EMSCRIPTEN + ANDROID
#define GLhandleARB GLuint
#define glRenderbufferStorageEXT glRenderbufferStorage
//#define GL_GENERATE_MIPMAP_SGIS GL_GENERATE_MIPMAP_HINT
//#define GL_FRAMEBUFFER_EXT  GL_FRAMEBUFFER
//#define GL_STENCIL_ATTACHMENT_EXT  GL_STENCIL_ATTACHMENT
//#define GL_DEPTH_ATTACHMENT_EXT  GL_DEPTH_ATTACHMENT
//#define GL_COLOR_ATTACHMENT0_EXT  GL_COLOR_ATTACHMENT0
#define glCheckFramebufferStatusEXT glCheckFramebufferStatus
//#define GL_FRAMEBUFFER_COMPLETE_EXT GL_FRAMEBUFFER_COMPLETE
//#define GL_FRAMEBUFFER_UNSUPPORTED_EXT GL_FRAMEBUFFER_UNSUPPORTED
#define glBindFramebufferEXT glBindFramebuffer
#define glGenerateMipmapEXT glGenerateMipmap
#define glGenRenderbuffersEXT glGenRenderbuffers
#define glBindRenderbufferEXT glBindRenderbuffer
#define glDeleteRenderbuffersEXT glDeleteRenderbuffers
#define glGenFramebuffersEXT glGenFramebuffers
#define glDeleteFramebuffersEXT glDeleteFramebuffers
#define glFramebufferTexture2DEXT glFramebufferTexture2D
#define glFramebufferRenderbufferEXT glFramebufferRenderbuffer
#ifdef ANDROID
#define GL_RENDERBUFFER_EXT GL_RENDERBUFFER
#define GL_FRAMEBUFFER_EXT  GL_FRAMEBUFFER
#define GL_DEPTH_ATTACHMENT_EXT  GL_DEPTH_ATTACHMENT
#define GL_DEPTH_STENCIL_ATTACHMENT_EXT  GL_DEPTH_STENCIL_ATTACHMENT
#define GL_FRAMEBUFFER_COMPLETE_EXT GL_FRAMEBUFFER_COMPLETE
#define GL_FRAMEBUFFER_UNSUPPORTED_EXT GL_FRAMEBUFFER_UNSUPPORTED
#endif
#ifdef EMSCRIPTEN
#define glDrawBuffersARB glDrawBuffers
#endif
#endif

// ------------------------------------------------------

namespace LibSL  {
  namespace GPUTex {

    using namespace LibSL::GLHelpers;

    /// API policy
    class GL_APIPolicy
    {
    public:

      /// Handle to textures
      typedef GLuint t_APIHandle2D;
      typedef GLuint t_APIHandle3D;
      typedef GLuint t_APIHandle2DArray;

      typedef GLuint t_Handle2D;
      typedef GLuint t_Handle3D;
      typedef GLuint t_HandleCubeMap;
      typedef GLuint t_Handle2DArray;

      typedef struct s_HandleRT2D {
      public:
        GLuint fbo;
        GLuint depth_rb;
        GLuint stencil_rb;
        GLuint textures[4];
        uint   numtargets;
        bool   autoMIPMAP;
      }t_HandleRT2D;

      static void               bindTexture2D(const t_APIHandle2D& handle) { glBindTexture(GL_TEXTURE_2D,handle); }

      static t_APIHandle2D      renderTarget2DTexture(const t_HandleRT2D& rt,uint target) {sl_assert(target < rt.numtargets); return rt.textures[target];}

      static t_APIHandle2D      apiHandle2D(t_Handle2D h)           { return t_APIHandle2D(h); }
      static t_APIHandle3D      apiHandle3D(t_Handle3D h)           { return t_APIHandle3D(h); }
      static t_APIHandle2DArray apiHandle2DArray(t_Handle2DArray h) { return t_APIHandle2DArray(h); }
    };


    /// Compile policy
    template <class T_APIPolicy,class T_PixelFormat>
    class GL_CompilePolicy
    {
    public:

      typedef LibSL::Memory::Array::Array<T_PixelFormat>   t_PixelArray1D;
      typedef LibSL::Memory::Array::Array2D<T_PixelFormat> t_PixelArray2D;
      typedef LibSL::Memory::Array::Array3D<T_PixelFormat> t_PixelArray3D;

      /// Create 2D texture
      static typename T_APIPolicy::t_Handle2D
        create2D(const t_PixelArray2D& array,uint flags)
      {
        GLuint id = 0;
        LIBSL_GL_CHECK_ERROR;
        glGenTextures(1,&id);
        glBindTexture(GL_TEXTURE_2D,id);
        if (flags & GPUTEX_AUTOGEN_MIPMAP) {
          if (flags & GPUTEX_INTEGER) {
            throw GLException("GLPolicy::create2D - to the best of our knowledge, GL does not support filtering of integer textures");
          }
          //glTexParameteri(GL_TEXTURE_2D,GL_GENERATE_MIPMAP_SGIS,GL_TRUE);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        } else {
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        }
        send2D(id,array,flags);
        LIBSL_GL_CHECK_ERROR;
        return (id);
      }

      /// Create 2D texture with mipmap
      static typename T_APIPolicy::t_Handle2D
        create2D(const LibSL::Memory::Array::Array<t_PixelArray2D>& miparray,uint flags)
      {
        GLuint id = 0;
        LIBSL_GL_CHECK_ERROR;
        glGenTextures(1,&id);
        glBindTexture(GL_TEXTURE_2D,id);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        send2Dmipmap(id,miparray,flags);
        LIBSL_GL_CHECK_ERROR;
        return (id);
      }

      /// Send 2D texture to GPU memory
      static void
        send2D(
        typename T_APIPolicy::t_Handle2D id,
        const t_PixelArray2D& array,
        uint flags)
      {
        LIBSL_GL_CHECK_ERROR;
        if (flags & GPUTEX_INTEGER) {
          if (GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::int_internal_format < 0) {
            throw GLException("GLPolicy::send2D - format does not support integer mapping");
          }
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_PACK_ALIGNMENT,   1);
        glBindTexture(GL_TEXTURE_2D,id);
        // Assert memory layout of tuples is valid
        // NOTE: if this fail, it means the compiler is building non-byte aligned Tuples
        sl_assert(sizeof(T_PixelFormat) == sizeof(typename T_PixelFormat::t_Element)*T_PixelFormat::e_Size);
        //std::cerr << "GPUTex: Creating texture " << array.xsize() << "x" << array.ysize() << std::endl;
        glTexImage2D(GL_TEXTURE_2D,
          0,
          (flags & GPUTEX_INTEGER)
          ? (int)GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::int_internal_format
          : (int)GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::internal_format,
          array.xsize(), array.ysize(),
          0,
          (flags & GPUTEX_INTEGER)
          ? (int)GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::int_format
          : (int)GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::format,
          GL_type<typename T_PixelFormat::t_Element>::type,
          array.raw());
        if (flags & GPUTEX_AUTOGEN_MIPMAP) {
#if defined(EMSCRIPTEN) | defined(ANDROID)
#define isPowerOfTwo(x) ((x != 0) && ((x & (~x + 1)) == x))
          if (isPowerOfTwo(array.xsize()) && isPowerOfTwo(array.ysize()))
#endif
          {
            glGenerateMipmapEXT(GL_TEXTURE_2D);
          }
        }
        LIBSL_GL_CHECK_ERROR;
      }

      /// Send 2D texture to GPU memory, each mipmap is specified
      static void
        send2Dmipmap(
        typename T_APIPolicy::t_Handle2D id,
        const LibSL::Memory::Array::Array<t_PixelArray2D>& miparray,
        uint flags)
      {
        LIBSL_GL_CHECK_ERROR;
        if (flags & GPUTEX_INTEGER) {
          throw GLException("GLPolicy::send2Dmipmap - to the best of our knowledge, GL does not support filtering of integer textures");
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_PACK_ALIGNMENT,   1);
        glBindTexture(GL_TEXTURE_2D,id);
        // Assert memory layout of tuples is valid
        // NOTE: if this fail, it means the compiler is building non-byte aligned Tuples
        sl_assert(sizeof(T_PixelFormat) == sizeof(typename T_PixelFormat::t_Element)*T_PixelFormat::e_Size);
        //std::cerr << "GPUTex: Creating texture " << array.xsize() << "x" << array.ysize() << std::endl;
        ForArray(miparray,l) {
          glTexImage2D(GL_TEXTURE_2D,
            l,
            GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::internal_format,
            miparray[l].xsize(), miparray[l].ysize(),
            0,
            GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::format,
            GL_type<typename T_PixelFormat::t_Element>::type,
            miparray[l].raw());
        }
        LIBSL_GL_CHECK_ERROR;
      }

      /// Send a subpart of a 2D texture to the GPU
      static void
        sendSub2D(
        typename T_APIPolicy::t_Handle2D id,
        uint x,uint y,
        const t_PixelArray2D& array,
        uint /*flags - UNUSED for now */)
      {
        LIBSL_GL_CHECK_ERROR;
        glBindTexture(GL_TEXTURE_2D,id);
        glTexSubImage2D(GL_TEXTURE_2D,0,
          x,y,
          array.xsize(),array.ysize(),
          GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::format,
          GL_type<typename T_PixelFormat::t_Element>::type,
          array.raw());
        // test for error
        LIBSL_GL_CHECK_ERROR;
      }

      /// Destroy 2D texture
      static void
        destroy2D(typename T_APIPolicy::t_Handle2D handle)
      {
        LIBSL_GL_CHECK_ERROR;
        glDeleteTextures(1,&handle);
        LIBSL_GL_CHECK_ERROR;
      }

#define ADD_DEPTH   1
#define ADD_STENCIL 1

      /// Create 2D render target
      ///   - num render targets are created (enables multiple render targets).
      ///   - in OpenGL a render target always has an associated depth buffer
      static
        typename T_APIPolicy::t_HandleRT2D
        createRenderTarget2D(uint w,uint h,uint flags,uint num)
      {
        bool is_depth = (GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::isdepth != 0);
        typename T_APIPolicy::t_HandleRT2D handle;
        handle.depth_rb   = 0;
        handle.stencil_rb = 0;
#if /*!defined(EMSCRIPTEN) &&*/ !defined(ANDROID)
        int maxRenterTargets = 0;
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxRenterTargets);
        sl_assert(num <uint(maxRenterTargets) && num > 0);
        sl_assert(!is_depth || num == 1);
#else
        sl_assert(!is_depth && num == 1);
#endif
        if (flags & GPUTEX_INTEGER) {
          if (GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::int_internal_format < 0) {
            throw GLException("GLPolicy::createRenderTarget2D - format does not support integer mapping");
          }
        }
        // gen objects
        glGenFramebuffersEXT(1, &(handle.fbo));            // frame buffer
        if (!is_depth) {
          if (ADD_DEPTH) {
            glGenRenderbuffersEXT(1, &(handle.depth_rb)); // depth buffer
            if (ADD_STENCIL) {
              handle.stencil_rb = handle.depth_rb;
            }
          }
        }
        handle.numtargets = num;
        handle.autoMIPMAP = ((flags & GPUTEX_AUTOGEN_MIPMAP) != 0);
        ForIndex(n,num) {
          glGenTextures(1, &(handle.textures[n]));
        }
        // textures
        ForIndex(n,num) {
          glBindTexture(GL_TEXTURE_2D,handle.textures[n]);
          glTexImage2D(GL_TEXTURE_2D,
            0,
            (flags & GPUTEX_INTEGER)
            ? (int)GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::int_internal_format
            : (int)GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::internal_format,
            w,h,
            0,
            (flags & GPUTEX_INTEGER)
            ? (int)GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::int_format
            : (int)GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::format,
            GL_type<typename T_PixelFormat::t_Element>::type,
            NULL);
          //	if (!handle.autoMIPMAP) {
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
          //	} else { // FIXME: this crashes with 16F RT
          //	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
          //	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
          //	}
        }

        if (!is_depth) {
          // color rt + depth
          // -> depth
#if defined(EMSCRIPTEN) | defined(ANDROID)
          if (ADD_DEPTH) {
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, handle.depth_rb);
#ifndef ANDROID
            if (ADD_STENCIL) {
              glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL, w, h);
            } else
#endif
            {
              glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT16, w, h);
            }
          }
#else
          if (ADD_DEPTH) {
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, handle.depth_rb);
            if (ADD_STENCIL) {
              glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, w, h);
            } else {
              glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT32, w, h);
            }
          }
#endif
          // -> attach everything together
          glBindFramebufferEXT        (GL_FRAMEBUFFER_EXT , handle.fbo);
          ForIndex(n,num) {
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
              GL_COLOR_ATTACHMENT0_EXT+n,
              GL_TEXTURE_2D,
              handle.textures[n], 0);
          }
#if defined(EMSCRIPTEN) | defined(ANDROID)
          if (handle.depth_rb) {
#ifndef ANDROID
            if (handle.stencil_rb) {
              glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER_EXT, handle.depth_rb);
            } else
#endif
            {
              glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, handle.depth_rb);
            }
          }
#else
          if (handle.depth_rb) {
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, handle.depth_rb);
          }
          if (handle.stencil_rb) {
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, handle.stencil_rb);
          }
#endif
        } else {
          // -> attach everything together
          glBindFramebufferEXT     (GL_FRAMEBUFFER_EXT , handle.fbo);
          glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT , GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, handle.textures[0], 0);
#if /*!defined(EMSCRIPTEN) &&*/ !defined(EMSCRIPTEN_WEBGL1) && !defined(ANDROID)
          glDrawBuffer(GL_NONE);
          glReadBuffer(GL_NONE);
#endif
          //          GLenum drawbuffers = GL_DEPTH_ATTACHMENT_EXT;
          //          glDrawBuffersARB(1,&drawbuffers);
        }
        LIBSL_GL_CHECK_ERROR;
        // check status
        LIBSL_CATCH_AND_BOUNCE({
          GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
          if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
            switch (status) {
            case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
              throw LibSL::Errors::Fatal("GL_CompilePolicy::createRenderTarget2D - cannot create FBO - format not supported (WARNING: check driver version!!)");
              break;
            default:
              throw LibSL::Errors::Fatal("GL_CompilePolicy::createRenderTarget2D - cannot create FBO (unknow reason)");
              break;
            }
          }
        });

        // gen MIPmaps immediately
        if (handle.autoMIPMAP) {
          ForIndex(n,handle.numtargets) {
            glBindTexture(GL_TEXTURE_2D,handle.textures[n]);
            glGenerateMipmapEXT(GL_TEXTURE_2D);
          }
        }

        // bind screen
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);

        // done
        return (handle);
      }


      /// Read back 2D render target data
      static void readBackRenderTarget2D(const typename T_APIPolicy::t_HandleRT2D& rt,
        t_PixelArray2D& array,
        uint target)
      {
        sl_assert(target < rt.numtargets);
        // makes sure Tuple have the correct size (read back relies on pointers)
        sl_assert(sizeof(T_PixelFormat) == sizeof(typename T_PixelFormat::t_Element)*T_PixelFormat::e_Size);
        sl_assert( glGetError() == 0 );
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, rt.textures[target]);
        int w, h;
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&h);
        sl_assert( uint(w*h) <= array.xsize()*array.ysize() );
        glGetTexImage(GL_TEXTURE_2D,
          0,
          GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::format,
          GL_type<typename T_PixelFormat::t_Element>::type,
          &(array.set(0,0))
          );
#else
        bindRenderTarget2D(rt);
        glReadPixels(0,0,array.xsize(),array.ysize(),
          GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::format,
          GL_type<typename T_PixelFormat::t_Element>::type,
          &(array.set(0,0))
        );
        unbindRenderTarget2D(rt);
#endif
        if (0) {
          // flip data   // FIXME TODO necessary ?   YES but test vs DX + Backward compatibility ????
          ForIndex(j,array.ysize()/2) {
            ForIndex(i,array.xsize()) {
              typename t_PixelArray2D::t_Element tmp = array.at(i,j);
              array.at(i,j)=array.at(i,array.ysize()-1-j);
              array.at(i,array.ysize()-1-j) = tmp;
            }
          }
        }
      }


      /// Bind 2D render target
      static void bindRenderTarget2D(const typename T_APIPolicy::t_HandleRT2D& rt)
      {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,rt.fbo);
#if /*!defined(EMSCRIPTEN) &&*/ !defined(EMSCRIPTEN_WEBGL1) && !defined(ANDROID)
        bool is_depth = (GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::isdepth != 0);
        if (!is_depth) {
          if (rt.numtargets > 0) {
            GLenum drawbuffers[32];
            ForIndex(i, 32) {
              drawbuffers[i] = GL_COLOR_ATTACHMENT0_EXT + i;
            }
            glDrawBuffersARB(rt.numtargets,drawbuffers);
          }
        } else {
          glDrawBuffer(GL_NONE);
          glReadBuffer(GL_NONE);
          //GLenum drawbuffers = GL_DEPTH_ATTACHMENT_EXT;
          //glDrawBuffersARB(1,&drawbuffers);
        }
#endif
      }



      /// Unbind 2D render target
      static void unbindRenderTarget2D(const typename T_APIPolicy::t_HandleRT2D& rt)
      {
        if (rt.autoMIPMAP) {
          ForIndex(n,rt.numtargets) {
            glBindTexture(GL_TEXTURE_2D,rt.textures[n]);
            glGenerateMipmapEXT(GL_TEXTURE_2D);
          }
        }
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
      }


      /// Clear 2D render target
      /// NOTE: - changes glClearColor
      static void clearRenderTarget2D(const typename T_APIPolicy::t_HandleRT2D& rt,const T_PixelFormat& v)
      {
        bindRenderTarget2D(rt);
        if (T_PixelFormat::e_Size == 1) {
          glClearColor(v[0],0,0,0);
        } else if (T_PixelFormat::e_Size == 2) {
          glClearColor(v[0],v[1],0,0);
        } else if (T_PixelFormat::e_Size == 3) {
          glClearColor(v[0],v[1],v[2],0);
        } else if (T_PixelFormat::e_Size == 4) {
          glClearColor(v[0],v[1],v[2],v[3]);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        unbindRenderTarget2D(rt);
      }


      /// Destroy 2D render target
      static void destroyRenderTarget2D(const typename T_APIPolicy::t_HandleRT2D& rt)
      {
        ForIndex(n,rt.numtargets) {
          glDeleteTextures(1,&(rt.textures[n]));
        }
        glDeleteFramebuffersEXT(1,&(rt.fbo));
        glDeleteRenderbuffersEXT(1,&(rt.depth_rb));
      }


#if !defined(EMSCRIPTEN_WEBGL1) && !defined(ANDROID)

      /// Create 3D texture
      static
        typename T_APIPolicy::t_Handle3D
        create3D(const t_PixelArray3D& array,uint flags)
      {
        GLuint id;
        glGenTextures(1,&id);
        glBindTexture(GL_TEXTURE_3D,id);
        if (flags & GPUTEX_AUTOGEN_MIPMAP) {
          glTexParameteri(GL_TEXTURE_3D,GL_GENERATE_MIPMAP_SGIS,GL_TRUE);
          glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
          glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        } else {
          glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
          glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        }
        LIBSL_GL_CHECK_ERROR;
        send3D(id,array,flags);
        return (id);
      }


      /// Send 3D texture to GPU memory
      static void
        send3D(typename T_APIPolicy::t_Handle3D id,
        const t_PixelArray3D& array,
        uint flags)
      {
        if (flags & GPUTEX_INTEGER) {
          if (GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::int_internal_format < 0) {
            throw GLException("GLPolicy::send3D - format does not support integer mapping");
          }
        }
        glBindTexture(GL_TEXTURE_3D,id);
        // Assert memory layout of tuples is valid
        // NOTE: if this fail, it means the compiler is building non-byte aligned Tuples
        sl_assert(sizeof(T_PixelFormat) == sizeof(typename T_PixelFormat::t_Element)*T_PixelFormat::e_Size);
        //std::cerr << "GPUTex: Creating texture " << array.xsize() << "x" << array.ysize() << "z" << array.ysize() << std::endl;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
#ifdef USE_GLUX
        glTexImage3DEXT(GL_TEXTURE_3D,
#else
        glTexImage3D(GL_TEXTURE_3D,
#endif
          0,
          (flags & GPUTEX_INTEGER)
          ? GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::int_internal_format
          : GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::internal_format,
          array.xsize(), array.ysize(), array.zsize(),
          0,
          (flags & GPUTEX_INTEGER)
          ? GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::int_format
          : GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::format,
          GL_type<typename T_PixelFormat::t_Element>::type,
          array.raw());
        LIBSL_GL_CHECK_ERROR;
      }

      /// Send a subpart of a 3D texture to the GPU
      static void
        sendSub3D(
        typename T_APIPolicy::t_Handle3D id,
        uint x,uint y,uint z,
        const t_PixelArray3D& array,
        uint /*flags - UNUSED for now */)
      {
        glBindTexture(GL_TEXTURE_3D,id);
        glTexSubImage3DEXT(GL_TEXTURE_3D,0,
          x,y,z,
          array.xsize(), array.ysize(), array.zsize(),
          GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::format,
          GL_type<typename T_PixelFormat::t_Element>::type,
          array.raw());
        // test for error
        LIBSL_GL_CHECK_ERROR;
      }

      /// Destroy 3D texture
      static void destroy3D(typename T_APIPolicy::t_Handle3D handle)
      {
        LIBSL_GL_CHECK_ERROR;
        glDeleteTextures(1,&handle);
        LIBSL_GL_CHECK_ERROR;
      }



      /// Create 2D texture array
      static typename T_APIPolicy::t_Handle2DArray
        create2DArray(const LibSL::Memory::Array::Array<t_PixelArray2D>& texarray,uint flags)
      {
        GLuint id;
        LIBSL_GL_CHECK_ERROR;
        glGenTextures(1,&id);
        glBindTexture(GL_TEXTURE_2D_ARRAY_EXT,id);
        if (flags & GPUTEX_AUTOGEN_MIPMAP) {
          glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT,GL_GENERATE_MIPMAP_SGIS,GL_TRUE);
          glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
          glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        } else {
          glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        }
        send2DArray(id,texarray,flags);
        return (id);
      }

      /// Create 2D texture with mipmap
      static typename T_APIPolicy::t_Handle2DArray
        create2DArray(const LibSL::Memory::Array::Array<LibSL::Memory::Array::Array<t_PixelArray2D> >& texmiparray,uint flags)
      {
        GLuint id;
        LIBSL_GL_CHECK_ERROR;
        glGenTextures(1,&id);
        glBindTexture(GL_TEXTURE_2D_ARRAY_EXT,id);
        if (flags & GPUTEX_AUTOGEN_MIPMAP) {
          glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT,GL_GENERATE_MIPMAP_SGIS,GL_TRUE);
          glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
          glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        } else {
          glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT,GL_TEXTURE_MAG_FILTER,GL_NEAREST_MIPMAP_NEAREST);
        }
        send2DArrayMipmap(id,texmiparray,flags);
        return (id);
      }


      /// Send 2D texture to GPU memory
      static void
        send2DArray(
        typename T_APIPolicy::t_Handle2DArray id,
        const LibSL::Memory::Array::Array<t_PixelArray2D>& texarray,
        uint flags)
      {
        if (flags & GPUTEX_INTEGER) {
          if (GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::int_internal_format < 0) {
            throw GLException("GLPolicy::send2DArray - format does not support integer mapping");
          }
        }
        ForRange(n,1,texarray.size()-1) {
          sl_assert(texarray[n].xsize() == texarray[0].xsize() && texarray[n].ysize() == texarray[0].ysize());
        }
        LIBSL_GL_CHECK_ERROR;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_PACK_ALIGNMENT,   1);
        glBindTexture(GL_TEXTURE_2D_ARRAY_EXT,id);
        // Assert memory layout of tuples is valid
        // NOTE: if this fail, it means the compiler is building non-byte aligned Tuples
        sl_assert(sizeof(T_PixelFormat) == sizeof(typename T_PixelFormat::t_Element)*T_PixelFormat::e_Size);
        glTexImage3DEXT(GL_TEXTURE_2D_ARRAY_EXT,
          0,
          (flags & GPUTEX_INTEGER)
          ? GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::int_internal_format
          : GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::internal_format,
          texarray[0].xsize(), texarray[0].ysize(), texarray.size(),
          0,
          (flags & GPUTEX_INTEGER)
          ? GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::int_format
          : GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::format,
          GL_type<typename T_PixelFormat::t_Element>::type,
          NULL);
        LIBSL_GL_CHECK_ERROR;
        ForIndex(n,texarray.size()) {
          glTexSubImage3DEXT(GL_TEXTURE_2D_ARRAY_EXT,
            0,
            0,0,n,
            texarray[n].xsize(),texarray[n].ysize(),1,
            GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::format,
            GL_type<typename T_PixelFormat::t_Element>::type,
            texarray[n].raw());
          LIBSL_GL_CHECK_ERROR;
        }
      }

      /// Send 2D texture to GPU memory, each mipmap is specified
      static void
        send2DArrayMipmap(
        typename T_APIPolicy::t_Handle2DArray id,
        const LibSL::Memory::Array::Array<LibSL::Memory::Array::Array<t_PixelArray2D> >& texmiparray,
        uint flags)
      {
        if (flags & GPUTEX_INTEGER) {
          throw GLException("GLPolicy::create2D - to the best of our knowledge, GL does not support fitlering of integer textures");
        }
        ForRange(n,1,texmiparray.size()-1) {
          sl_assert(texmiparray[n].size() > 0);
          sl_assert(texmiparray[n].size() == texmiparray[0].size());
          ForIndex(l,texmiparray[0].size()) {
            sl_assert(texmiparray[n][l].xsize() == texmiparray[0][l].xsize() && texmiparray[n][l].ysize() == texmiparray[0][l].ysize());
          }
        }
        LIBSL_GL_CHECK_ERROR;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_PACK_ALIGNMENT,   1);
        glBindTexture(GL_TEXTURE_2D_ARRAY_EXT,id);
        // Assert memory layout of tuples is valid
        // NOTE: if this fail, it means the compiler is building non-byte aligned Tuples
        sl_assert(sizeof(T_PixelFormat) == sizeof(typename T_PixelFormat::t_Element)*T_PixelFormat::e_Size);
        uint num_miplvls = texmiparray[0].size();
        ForIndex(l,num_miplvls) {
          glTexImage3DEXT(GL_TEXTURE_2D_ARRAY_EXT,
            l,
            GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::internal_format,
            texmiparray[0][l].xsize(), texmiparray[0][l].ysize(), texmiparray.size(),
            0,
            GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::format,
            GL_type<typename T_PixelFormat::t_Element>::type,
            NULL);
          LIBSL_GL_CHECK_ERROR;
          ForIndex(n,texmiparray.size()) {
            glTexSubImage3DEXT(GL_TEXTURE_2D_ARRAY_EXT,
              l,
              0,0,n,
              texmiparray[n][l].xsize(),texmiparray[n][l].ysize(),1,
              GL_format<typename T_PixelFormat::t_Element,T_PixelFormat::e_Size>::format,
              GL_type<typename T_PixelFormat::t_Element>::type,
              texmiparray[n][l].raw());
            LIBSL_GL_CHECK_ERROR;
          }
        }
      }

      /// Destroy 2D texture array
      static void
        destroy2DArray(typename T_APIPolicy::t_Handle2DArray handle)
      {
        glDeleteTextures(1,&handle);
        LIBSL_GL_CHECK_ERROR;
      }

#endif

    };

  } // namespace LibSL::GPUTex
} // namespace LibSL

// ------------------------------------------------------
