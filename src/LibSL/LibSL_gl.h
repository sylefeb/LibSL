/* --------------------------------------------------------------------
Author: Sylvain Lefebvre    sylvain.lefebvre@inria.fr

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
// LibSL_gl - main include file for OpenGL apps
// ------------------------------------------------------
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-04-03
// ------------------------------------------------------

#pragma once

#define LIBSL_GL_INCLUDED

// #pragma message("Including LibSL_gl.h")

#include <LibSL/LibSL_gl.config.h>

#ifndef OPENGL
#define OPENGL
#endif

#ifdef OPENGL
#ifdef DIRECT3D
ERROR_____either_OPENGL_or_DIRECT3D_must_be_defined__not_both _;
#endif
#endif

#define LIBSL_OPENGL_MAJOR_VERSION 1
#define LIBSL_OPENGL_MINOR_VERSION 1

#ifndef LIBSL_CORE_INCLUDED
#include <LibSL/LibSL.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#ifdef __APPLE__
#include "apple_gl.h"
#include <OpenGL/glu.h>
#else
#ifndef EMSCRIPTEN
#ifdef ANDROID
#include <GLES2/gl2.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif
#endif

#ifndef EMSCRIPTEN
#ifdef USE_GLUX
#include <glux.h>
#endif
#endif

#include <LibSL/GPUTex/GPUTex_gl.h>
#include <LibSL/GPUMesh/GPUMesh_gl.h>

#include <LibSL/GLHelpers/GLHelpers.h>

#include <LibSL/Mesh/MeshRenderer.h>
#include <LibSL/Mesh/TexturedMeshRenderer.h>

// Cg dependencies
// #include <LibSL/CgHelpers/CgHelpers.h>
// #include <LibSL/CgHelpers/CgImageProcessing.h>
// #include <LibSL/Mesh/AnimatedMeshCgRenderer.h>
// #include <LibSL/GPUHelpers/GPUFillArray2D.h>
// #include <LibSL/Shaders/Nature/Sky.h>

#include <LibSL/GPUHelpers/GPUHelpers.h>
#include <LibSL/GPUHelpers/GPUHelpers_gl.h>

#include <LibSL/GPUHelpers/Font.h>
#include <LibSL/GPUHelpers/Profiler.h>

#include <LibSL/UIHelpers/SimpleUI.h>
#include <LibSL/UIHelpers/TrackballUI.h>
#include <LibSL/UIHelpers/Manipulator.h>
#include <LibSL/UIHelpers/BindAntTweakBar.h>
#include <LibSL/UIHelpers/BindImGui.h>

#ifndef LIBSL_SAFE_NAMESPACE

using namespace LibSL::GPUMesh;
using namespace LibSL::GPUTex;
using namespace LibSL::GLHelpers;
using namespace LibSL::GPUHelpers;
using namespace LibSL::UIHelpers;
// using namespace LibSL::Shaders::Nature;
// using namespace LibSL::CgHelpers;

#endif
