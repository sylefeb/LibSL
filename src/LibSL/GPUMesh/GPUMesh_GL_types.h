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
// GPUMesh_GL_types.h
//
// The goal of this template class is to build a template mechanism
// above the OpenGL type definition.
// The run time overhead is null as everything is resolved at compile time.
//
// Exemple: GLTypes<float>::gl_define is equal to the GL_FLOAT define.
//
//                                                    (c) Sylvain Lefebvre 2003
// ----------------------------------------------------------------------------

#pragma once

#ifdef __APPLE__
#include "apple_gl.h"
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

namespace LibSL {
namespace GPUMesh {

template <typename T> struct GLTypes;

template <> struct GLTypes<float>
{
  enum    {gl_define = GL_FLOAT};
};

#ifndef EMSCRIPTEN
#ifndef ANDROID
template <> struct GLTypes<double>
{
  enum    {gl_define = GL_DOUBLE};
};
#endif
#endif

template <> struct GLTypes<unsigned char>
{
  enum    {gl_define = GL_UNSIGNED_BYTE};
};

template <> struct GLTypes<unsigned int>
{
  enum    {gl_define = GL_UNSIGNED_INT};
};

template <> struct GLTypes<unsigned short>
{
  enum    {gl_define = GL_UNSIGNED_SHORT};
};

template <> struct GLTypes<LibSL::Mesh::MVF_null>
{
  enum    {gl_define = 0xFFFFFFFF};
};

} // namespace GPUMesh
} // namespace LibSL
