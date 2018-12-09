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
// LibSL::GPUTex::GL_format
// ------------------------------------------------------
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-24
// ------------------------------------------------------

#pragma once

// #pragma message("Including GPUTex_GL_formats.h")

// ------------------------------------------------------

#include <LibSL/System/Types.h>
using namespace LibSL::System::Types;

// ------------------------------------------------------

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#ifdef EMSCRIPTEN
#define GL_GLEXT_PROTOTYPES
#endif
#ifdef ANDROID
#include <GLES2/gl2.h>
#else
#include <GL/gl.h>
#endif
#endif

#ifndef EMSCRIPTEN
#ifdef USE_GLUX
# include <glux.h>
# include "GL_ARB_texture_float.h"
# include "GL_ATI_texture_float.h"
# include "GL_NV_texture_shader2.h"
GLUX_LOAD(GL_ARB_texture_float)
GLUX_LOAD(GL_ATI_texture_float)
GLUX_LOAD(GL_NV_texture_shader2)
#else
#ifdef __APPLE__
#include <OpenGL/glext.h>
#else
#ifdef ANDROID
#include <GLES2/gl2ext.h>
#else
#include <GL/glext.h>
#endif
#endif
#endif
#endif

// ------------------------------------------------------

namespace LibSL  {
  namespace GPUTex {

    template <typename T_Type,int T_Num> class GL_format;

#ifndef EMSCRIPTEN
#ifndef ANDROID
    template <> class GL_format<unsigned char,1>
    {
    public:
      enum {internal_format     = GL_R8,
            format              = GL_RED,
            int_internal_format = GL_R8UI,
#ifndef __APPLE__
            int_format          = GL_RED_INTEGER,
#endif
            isdepth             = 0};
    };

    template <> class GL_format<unsigned char,2>
    {
    public:
      enum {internal_format     = GL_RG8,
            format              = GL_RG,
            int_internal_format = GL_RG8UI,
            int_format          = GL_RG_INTEGER,
            isdepth             = 0};
    };
#endif
#endif

    template <> class GL_format<unsigned char,3>
    {
    public:
      enum {
#if defined(EMSCRIPTEN) | defined(ANDROID)
            internal_format     = GL_RGB,
            format              = GL_RGB,
            int_internal_format = 0,
            int_format          = 0,
#else
            internal_format     = GL_RGB8,
            format              = GL_RGB,
            int_internal_format = GL_RGB8UI_EXT,
            int_format          = GL_RGB_INTEGER_EXT,
#endif
            isdepth             = 0};
    };

    template <> class GL_format<unsigned char,4>
    {
    public:
      enum {
#if defined(EMSCRIPTEN) | defined(ANDROID)
            internal_format     = GL_RGBA,
            format              = GL_RGBA,
            int_internal_format = 0,
            int_format          = 0,
#else
            internal_format     = GL_RGBA8,
            format              = GL_RGBA,
            int_internal_format = GL_RGBA8UI_EXT,
            int_format          = GL_RGBA_INTEGER_EXT,
#endif
            isdepth             = 0};
    };

    
#ifndef EMSCRIPTEN
#ifndef ANDROID
#ifndef __APPLE__
    template <> class GL_format<unsigned short,1>
    {
    public:
      enum {internal_format     = GL_ALPHA16_EXT,
            format              = GL_ALPHA,
            int_internal_format = GL_ALPHA16UI_EXT,
            int_format          = GL_ALPHA_INTEGER_EXT,
            isdepth             = 0};
    };

    template <> class GL_format<unsigned short,2>
    {
    public:
      enum {internal_format = GL_HILO16_NV,  /// FIXME this is a problem, since HiLo is .xy and LUM_ALPHA is .xw
            format          = GL_HILO_NV,
//            internal_format     = GL_LUMINANCE16_ALPHA16_EXT,
//            format              = GL_LUMINANCE_ALPHA,
            int_internal_format = GL_LUMINANCE_ALPHA16UI_EXT,
            int_format          = GL_LUMINANCE_ALPHA_INTEGER_EXT,
            isdepth         = 0};
    };

    template <> class GL_format<short,2>
    {
    public:
      enum {internal_format = GL_SIGNED_HILO16_NV,  /// FIXME this is a problem, since HiLo is .xy and LUM_ALPHA is .xw
            format          = GL_HILO_NV,
//            internal_format     = GL_LUMINANCE16_ALPHA16_EXT,
//            format              = GL_LUMINANCE_ALPHA,
            int_internal_format = GL_LUMINANCE_ALPHA16I_EXT,
            int_format          = GL_LUMINANCE_ALPHA_INTEGER_EXT,
            isdepth             = 0};
    };

    template <> class GL_format<unsigned short,3>
    {
    public:
      enum {internal_format     = GL_RGB16_EXT,
            format              = GL_RGB,
            int_internal_format = GL_RGB16UI_EXT,
            int_format          = GL_RGB_INTEGER_EXT,
            isdepth             = 0};
    };

    template <> class GL_format<unsigned short,4>
    {
    public:
      enum {internal_format     = GL_RGBA16_EXT,
            format              = GL_RGBA,
            int_internal_format = GL_RGBA16UI_EXT,
            int_format          = GL_RGBA_INTEGER_EXT,
            isdepth             = 0};
    };
#endif

    template <> class GL_format<half,1>
    {
    public:
      enum {internal_format     = GL_LUMINANCE16F_ARB,
            format              = GL_LUMINANCE,
            int_internal_format = -1,
            int_format          = -1,
            isdepth             = 0};
    };

#endif
#endif

    template <> class GL_format<half,2>
    {
    public:
      enum {
#if defined(EMSCRIPTEN) | defined(ANDROID)
            internal_format     = GL_LUMINANCE_ALPHA,
            format              = GL_LUMINANCE_ALPHA,
#else
            internal_format     = GL_RG16F,
            format              = GL_RG,
#endif
            int_internal_format = -1,
            int_format          = -1,
            isdepth             = 0};
    };

#ifndef EMSCRIPTEN
#ifndef ANDROID
    template <> class GL_format<half, 3>
    {
    public:
      enum {internal_format     = GL_RGB16F_ARB,
            format              = GL_RGB,
            int_internal_format = -1,
            int_format          = -1,
            isdepth             = 0};
    };

#endif
#endif

#ifndef ANDROID
    template <> class GL_format<half, 4>
    {
    public:
      enum {
            internal_format     = GL_RGBA16F_ARB,
            format              = GL_RGBA,
            int_internal_format = -1,
            int_format          = -1,
            isdepth             = 0};
    };
#endif

#ifndef EMSCRIPTEN
#ifndef ANDROID

    template <> class GL_format<float, 1>
    {
    public:
		enum {internal_format=GL_R32F,
      //       internal_format=GL_LUMINANCE32F_ARB,
      //       internal_format     = GL_LUMINANCE_FLOAT32_ATI,
          format              = GL_LUMINANCE,
          int_internal_format = -1,
          int_format          = -1,
          isdepth = 0};
    };
    /*
    template <> class GL_format<float,2>
    {
    public:
      enum {internal_format = ,
            format          = ,
            isdepth         = };
    };
    */
    template <> class GL_format<float,3>
    {
    public:
      enum {internal_format     = GL_RGB32F_ARB,
            format              = GL_RGB,
            int_internal_format = -1,
            int_format          = -1,
            isdepth             = 0};
    };

#endif
#endif

    template <> class GL_format<float,4>
    {
    public:
      //enum {internal_format=GL_RGBA32F_ARB};
      enum {
#if defined(EMSCRIPTEN) | defined(ANDROID)
            internal_format     = GL_RGBA,
            format              = GL_RGBA,
#else
            internal_format     = GL_RGBA_FLOAT32_ATI,
            format              = GL_RGBA,
#endif
            int_internal_format = -1,
            int_format          = -1,
            isdepth             = 0};
    };

#ifndef EMSCRIPTEN
#ifndef ANDROID
    // depth texture format

    template <> class GL_format<depth32,1>
    {
    public:
      enum {internal_format     = GL_DEPTH_COMPONENT32,
            format              = GL_DEPTH_COMPONENT,
            int_internal_format = -1,
            int_format          = -1,
            isdepth             = 1};
    };

    template <> class GL_format<depth24,1>
    {
    public:
      enum {internal_format     = GL_DEPTH_COMPONENT24,
            format              = GL_DEPTH_COMPONENT,
            int_internal_format = -1,
            int_format          = -1,
            isdepth             = 1};
    };

    template <> class GL_format<depth16,1>
    {
    public:
      enum {internal_format     = GL_DEPTH_COMPONENT16,
            format              = GL_DEPTH_COMPONENT,
            int_internal_format = -1,
            int_format          = -1,
            isdepth             = 1};
    };
#endif
#endif

  } // namespace LibSL::GPUTex
} // namespace LibSL

// ------------------------------------------------------
