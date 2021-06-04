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
//---------------------------------------------------------------------------
#include "LibSL.h"
//---------------------------------------------------------------------------

#ifdef OPENGL

#ifdef EMSCRIPTEN
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#else // EMSCRIPTEN
#ifdef USE_GLUX
#include <glux.h>
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GL/gl.h>
#endif
#endif
#endif // EMSCRIPTEN

#ifdef OPENGL4
#ifdef OPENGLCORE
#include "LibSL_gl4core.h"
#else
#include "LibSL_gl4.h"
#endif
#else
#include "LibSL_gl.h"
#endif // OPENGL4

#endif // OPENGL


#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <windowsx.h>
#endif

#ifdef DIRECT3D
#include "LibSL_d3d.h"
#endif

#ifdef DIRECTX10
#include "LibSL_dx10.h"
#endif

#include <LibSL/Errors/Errors.h>
using namespace LibSL::Errors;
#include <LibSL/Memory/Array.h>
using namespace LibSL::Memory::Array;
#include <LibSL/Memory/Pointer.h>
using namespace LibSL::Memory::Pointer;
#include <LibSL/CppHelpers/CppHelpers.h>
using namespace LibSL::CppHelpers;
#include <LibSL/System/System.h>
using namespace LibSL::System;
using namespace LibSL::System::Time;
#include <LibSL/Math/Quaternion.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Math/Math.h>
using namespace LibSL::Math;
using namespace LibSL::UIHelpers;

//---------------------------------------------------------------------------

#define NAMESPACE LibSL::UIHelpers::SimpleUI

//---------------------------------------------------------------------------

static void defaultOnRender()
{}
static void defaultOnKeyPressed(uchar)
{}
static void defaultOnKeyUnpressed(uchar)
{}
static void defaultOnScanCodePressed(uint)
{}
static void defaultOnScanCodeUnpressed(uint)
{}
static void defaultOnMouseButtonPressed(uint,uint,uint,uint)
{}
static void defaultOnMouseMotion(uint,uint)
{}
static void defaultOnMouseWheel(int)
{}
static void defaultOnAnimate(double,float)
{}
static void defaultOnReshape(uint,uint)
{}
static void defaultOnDragDrop(uint,const char**)
{}

//---------------------------------------------------------------------------

NAMESPACE::f_onRender             NAMESPACE::onRender             = defaultOnRender;
NAMESPACE::f_onKeyPressed         NAMESPACE::onKeyPressed         = defaultOnKeyPressed;
NAMESPACE::f_onKeyUnpressed       NAMESPACE::onKeyUnpressed       = defaultOnKeyUnpressed;
NAMESPACE::f_onScanCodePressed    NAMESPACE::onScanCodePressed    = defaultOnScanCodePressed;
NAMESPACE::f_onScanCodeUnpressed  NAMESPACE::onScanCodeUnpressed  = defaultOnScanCodeUnpressed;
NAMESPACE::f_onMouseButtonPressed NAMESPACE::onMouseButtonPressed = defaultOnMouseButtonPressed;
NAMESPACE::f_onMouseMotion        NAMESPACE::onMouseMotion        = defaultOnMouseMotion;
NAMESPACE::f_onMouseWheel         NAMESPACE::onMouseWheel         = defaultOnMouseWheel;
NAMESPACE::f_onAnimate            NAMESPACE::onAnimate            = defaultOnAnimate;
NAMESPACE::f_onReshape            NAMESPACE::onReshape            = defaultOnReshape;
NAMESPACE::f_onDragDrop           NAMESPACE::onDragDrop           = defaultOnDragDrop;

//---------------------------------------------------------------------------

#if defined(_WIN32) || defined(_WIN64)

static WNDPROC SimpleUI_Custom_WndProc = NULL;

void NAMESPACE::setCustomCallbackMsgProc(WNDPROC wProc)
{
  SimpleUI_Custom_WndProc = wProc;
}

#endif

//---------------------------------------------------------------------------
// Common implementation
//---------------------------------------------------------------------------

static uint s_ScreenW          = 0;
static uint s_ScreenH          = 0;
static bool s_AlwaysRefresh    = true;
static bool s_FullScreen       = false;
static float s_MouseWheelScale = 0.01f;

uint NAMESPACE::screenWidth()
{
  return (s_ScreenW);
}

uint NAMESPACE::screenHeight()
{
  return (s_ScreenH);
}

void NAMESPACE::setAlwaysRefresh(bool r)
{
  s_AlwaysRefresh = r;
}

void NAMESPACE::setMouseWheelScale(float scale)
{
  s_MouseWheelScale = scale;
}

LIBSL_DLL float NAMESPACE::mouseWheelScale()
{
  return s_MouseWheelScale;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Select platform implementation
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifdef OPENGL

#ifdef USE_GLFW

#include "SimpleUI_glfw.cpp"

#elif USE_GLUT

#include "SimpleUI_glut.cpp"

#elif defined(WIN32) | defined(_WIN64)

#include "SimpleUI_win_gl.cpp"

#endif

#else // DIRECT3D or DIRECTX10

#include "SimpleUI_win_d3d.cpp"

#endif


//---------------------------------------------------------------------------
