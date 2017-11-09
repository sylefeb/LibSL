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
// LibSL::GPUTex::GL_type
// ------------------------------------------------------
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-24
// ------------------------------------------------------

#pragma once

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
#include <GL/gl.h>
#endif

// ------------------------------------------------------

#ifndef EMSCRIPTEN
#ifdef USE_GLUX
# include <glux.h>
# include "GL_ARB_half_float_pixel.h"
GLUX_LOAD(GL_ARB_half_float_pixel)
#endif
#else
#define GL_HALF_FLOAT_OES 0x8D61
#endif

// ------------------------------------------------------

namespace LibSL  {
  namespace GPUTex {

    template <typename T> class GL_type;

    template <> class GL_type<unsigned char>
    {
    public:
      enum {type=GL_UNSIGNED_BYTE};
    };

    template <> class GL_type<unsigned short>
    {
    public:
      enum {type=GL_UNSIGNED_SHORT};
    };

    template <> class GL_type<short>
    {
    public:
      enum {type=GL_SHORT};
    };
#ifdef EMSCRIPTEN
    template <> class GL_type<half>
    {
    public:
      enum { type = GL_HALF_FLOAT_OES };
    };
#else
    template <> class GL_type<half>
    {
    public:
      enum { type = GL_HALF_FLOAT_ARB };
    };
#endif
    template <> class GL_type<float>
    {
    public:
      enum {type=GL_FLOAT};
    };

    template <> class GL_type<depth32>
    {
    public:
      enum {type=GL_UNSIGNED_INT};
    };

    template <> class GL_type<depth24>
    {
    public:
      enum {type=GL_UNSIGNED_INT};
    };

    template <> class GL_type<depth16>
    {
    public:
      enum {type=GL_UNSIGNED_SHORT};
    };

  } // namespace LibSL::GPUTex
} // namespace LibSL

// ------------------------------------------------------
