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
#include <GL/gl.h>
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


#ifdef WIN32
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

//---------------------------------------------------------------------------

#ifdef WIN32

static WNDPROC SimpleUI_Custom_WndProc = NULL;

void NAMESPACE::setCustomCallbackMsgProc(WNDPROC wProc)
{
  SimpleUI_Custom_WndProc = wProc;
}

#endif

//---------------------------------------------------------------------------
// Common implementation
//---------------------------------------------------------------------------

static uint s_ScreenW = 0;
static uint s_ScreenH = 0;
static bool s_AlwaysRefresh = true;
static bool s_FullScreen    = false;

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

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifdef OPENGL

//---------------------------------------------------------------------------
// OpenGL implementation
//---------------------------------------------------------------------------

#ifdef USE_GLUT

#ifdef __APPLE__

#include <GLUT/glut.h>

#else

#include <GL/glut.h>
#ifndef EMSCRIPTEN
#ifdef FREEGLUT
#include <GL/freeglut_ext.h>
#endif
#endif

#endif

//---------------------------------------------------------------------------
// Linux implementation - uses glut
//---------------------------------------------------------------------------

uint glut_to_LibSL_scancode(uchar key,uint sc)
{
  //std::cout << "glut_to_LibSL_scancode k=" << (int)key << " sc=" << sc << std::endl;
  switch (sc)
  {
  case GLUT_KEY_F1: return LIBSL_KEY_F1;
  case GLUT_KEY_F2: return LIBSL_KEY_F2;
  case GLUT_KEY_F3: return LIBSL_KEY_F3;
  case GLUT_KEY_F4: return LIBSL_KEY_F4;
  case GLUT_KEY_F5: return LIBSL_KEY_F5;
  case GLUT_KEY_F6: return LIBSL_KEY_F6;
  case GLUT_KEY_F7: return LIBSL_KEY_F7;
  case GLUT_KEY_F8: return LIBSL_KEY_F8;
  case GLUT_KEY_F9: return LIBSL_KEY_F9;
  case GLUT_KEY_F10: return LIBSL_KEY_F10;
  case GLUT_KEY_F11: return LIBSL_KEY_F11;
  case GLUT_KEY_F12: return LIBSL_KEY_F12;
  // case GLUT_RETURN: return LIBSL_KEY_ENTER;
  case GLUT_KEY_UP: return LIBSL_KEY_UP;
  case GLUT_KEY_RIGHT: return LIBSL_KEY_RIGHT;
  case GLUT_KEY_LEFT: return LIBSL_KEY_LEFT;
  case GLUT_KEY_DOWN: return LIBSL_KEY_DOWN;
  case GLUT_KEY_PAGE_UP: return LIBSL_KEY_PAGE_UP;
  case GLUT_KEY_PAGE_DOWN: return LIBSL_KEY_PAGE_DOWN;
  case GLUT_KEY_HOME: return LIBSL_KEY_HOME;
  case GLUT_KEY_END: return LIBSL_KEY_END;
  case GLUT_KEY_INSERT: return LIBSL_KEY_INSERT;
#ifndef EMSCRIPTEN
  case GLUT_KEY_DELETE: return LIBSL_KEY_DELETE;
#endif
  case 112: return LIBSL_KEY_SHIFT;
  case 114: return LIBSL_KEY_CTRL;
  case 116: return LIBSL_KEY_ALT;
  case 120: return LIBSL_KEY_BK_SPACE;
  }
  switch (key)
  {
  case '\t': return LIBSL_KEY_TAB;
  case 27: return LIBSL_KEY_ESC;
  case 8: return LIBSL_KEY_BK_SPACE;
  }
  return 0;
}

static void glutMouse(int glut_btn, int glut_state, int x, int y)
{
  if (  (glut_btn & GLUT_LEFT_BUTTON)
    &&  (glut_btn & GLUT_RIGHT_BUTTON) ) {
    glut_btn = GLUT_MIDDLE_BUTTON;
  }

  uint button=0,flags=0;
  if      (glut_btn == GLUT_LEFT_BUTTON)   button=LIBSL_LEFT_BUTTON;
  else if (glut_btn == GLUT_MIDDLE_BUTTON) button=LIBSL_MIDDLE_BUTTON;
  else if (glut_btn == GLUT_RIGHT_BUTTON)  button=LIBSL_RIGHT_BUTTON;
#ifdef EMSCRIPTEN
  else if (glut_btn == 3)  button = LIBSL_WHEEL_UP;
  else if (glut_btn == 4)  button = LIBSL_WHEEL_DOWN;
#endif
  if (glut_state == GLUT_DOWN) {
    flags |= LIBSL_BUTTON_DOWN;
  } else if (glut_state == GLUT_UP) {
    flags |= LIBSL_BUTTON_UP;
  }

  NAMESPACE::onMouseButtonPressed(x,y,button,flags);
}


static void glutMotion(int x,int y)
{
  NAMESPACE::onMouseMotion(x,y);
}

static void glutKeyboard(uchar k, int x, int y)
{
  // std::cout << "glutKeyboard '" << k << "'" << std::endl;
  uint sc = glut_to_LibSL_scancode(k,0);
  if (sc) {
    NAMESPACE::onScanCodePressed(sc);
  } else {
    NAMESPACE::onKeyPressed(k);
  }
}

static void glutKeyboardUp (uchar k, int x, int y)
{
  uint sc = glut_to_LibSL_scancode(k,0);
  if (sc) {
    NAMESPACE::onScanCodeUnpressed(sc);
  } else {
    NAMESPACE::onKeyUnpressed(k);
  }
}

static void glutKeyboardSpecial(int k, int x, int y)
{
  // std::cout << "glutKeyboardSpecial '" << k << "'" << std::endl;
  uint code = 0;
  code = glut_to_LibSL_scancode(0,k);
  NAMESPACE::onScanCodePressed(code);
}

static void glutKeyboardSpecialUp(int k, int x, int y)
{
  int code = 0;
  code = glut_to_LibSL_scancode(0,k);
  NAMESPACE::onScanCodeUnpressed(code = glut_to_LibSL_scancode(0,k));
}

static void glutRender()
{
  static double last = LibSL::System::Time::milliseconds();
  double now         = LibSL::System::Time::milliseconds();
  float  elapsed     = float(now - last);

  if (elapsed > 0) {
    NAMESPACE::onAnimate( now, elapsed );
    last = now;
  }

#ifdef EMSCRIPTEN
  LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->begin();
#endif

  NAMESPACE::onRender();

#ifdef EMSCRIPTEN
  LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->end();
#endif

  glutSwapBuffers();
}

static void glutReshape(int w,int h)
{
  NAMESPACE::onReshape(w,h);
}

static void glutIdle()
{
  if (s_AlwaysRefresh) {
    glutPostRedisplay();
  }
}

#ifndef EMSCRIPTEN
#ifdef FREEGLUT
static void glutClose()
{
  glutLeaveMainLoop();
}
#endif
#endif

void NAMESPACE::refresh()
{
  glutPostRedisplay();
}

//#ifdef EMSCRIPTEN
//static void glutMouseWheel(int button, int dir, int x, int y)
//{
//  NAMESPACE::onMouseWheel(dir);
//}
//#endif

void NAMESPACE::init(uint width,uint height,const char *title,char **argv,int argc,bool frameLess,bool hidden,bool fullscreen)
{
  sl_assert(!hidden);    // not supported
  sl_assert(!frameLess); // not supported
  sl_assert(!fullscreen); // not supported

  glutInit              (&argc, argv);

#ifdef OPENGLCORE
  glutInitContextVersion(LIBSL_OPENGL_MAJOR_VERSION,LIBSL_OPENGL_MINOR_VERSION);
  glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
  glutInitContextProfile(GLUT_CORE_PROFILE);
#endif

  glutInitDisplayMode   (GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_ALPHA);
  glutInitWindowPosition(0,0);
  glutInitWindowSize    (width,height);
  glutCreateWindow      (title);
#ifndef EMSCRIPTEN
	glutSetIconTitle      (title);
#endif
  glutDisplayFunc       (glutRender);
  glutIdleFunc          (glutIdle);
  glutMotionFunc        (glutMotion);
  glutPassiveMotionFunc (glutMotion);
  glutMouseFunc         (glutMouse);
  glutKeyboardFunc      (glutKeyboard);
  glutKeyboardUpFunc	(glutKeyboardUp);
  glutSpecialFunc       (glutKeyboardSpecial);
  glutSpecialUpFunc     (glutKeyboardSpecialUp);
  glutReshapeFunc       (glutReshape);
#ifndef EMSCRIPTEN
#ifdef FREEGLUT
  glutCloseFunc         (glutClose);
  glutSetOption         (GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
#endif
#endif
//#ifdef EMSCRIPTEN
//  glutMouseWheelFunc    (glutMouseWheel);
//#endif
#ifndef EMSCRIPTEN
#ifdef USE_GLUX
  gluxInit();
#endif
#endif

  LibSL::GLHelpers::GLBasicPipeline::init();
#ifdef EMSCRIPTEN
  LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->begin();
#endif

  glutReshape(width,height);

  glEnable(GL_DEPTH_TEST);
}

void NAMESPACE::loop()
{
  glutMainLoop();
}

void NAMESPACE::shutdown()
{
  LibSL::GLHelpers::GLBasicPipeline::terminate();
}

void NAMESPACE::exit()
{
  LibSL::GLHelpers::GLBasicPipeline::terminate();

  ::exit (0);
}

void NAMESPACE::glSwapBuffers()
{
  glutSwapBuffers();
}

void NAMESPACE::glShowWindow(bool hide)
{
  if (hide)
    glutHideWindow();
  else
    glutShowWindow();
}

#else

#include <winuser.h>

//---------------------------------------------------------------------------
// Windows implementation - native
//---------------------------------------------------------------------------

static HWND  s_hWnd = NULL;
static HDC   s_hDC  = NULL;
static HGLRC s_hRC  = NULL;

static void SimpleUI_gl_Render()
{
  static time_t last = LibSL::System::Time::milliseconds();
  time_t now         = LibSL::System::Time::milliseconds();
  time_t elapsed     = (now - last);

  if (elapsed > 0) {
    NAMESPACE::onAnimate( (double)now, (float)elapsed );
    last = now;
  }

  NAMESPACE::onRender();

}

uint win32_VirtualKey_to_LibSL_scancode(uint vk)
{
  switch (vk)
  {
  case VK_F1: return LIBSL_KEY_F1;
  case VK_F2: return LIBSL_KEY_F2;
  case VK_F3: return LIBSL_KEY_F3;
  case VK_F4: return LIBSL_KEY_F4;
  case VK_F5: return LIBSL_KEY_F5;
  case VK_F6: return LIBSL_KEY_F6;
  case VK_F7: return LIBSL_KEY_F7;
  case VK_F8: return LIBSL_KEY_F8;
  case VK_F9: return LIBSL_KEY_F9;
  case VK_F10: return LIBSL_KEY_F10;
  case VK_F11: return LIBSL_KEY_F11;
  case VK_F12: return LIBSL_KEY_F12;
  case VK_RETURN: return LIBSL_KEY_ENTER;
  case VK_UP: return LIBSL_KEY_UP;
  case VK_RIGHT: return LIBSL_KEY_RIGHT;
  case VK_LEFT: return LIBSL_KEY_LEFT;
  case VK_DOWN: return LIBSL_KEY_DOWN;
  case VK_PRIOR: return LIBSL_KEY_PAGE_UP;
  case VK_NEXT: return LIBSL_KEY_PAGE_DOWN;
  case VK_HOME: return LIBSL_KEY_HOME;
  case VK_END: return LIBSL_KEY_END;
  case VK_INSERT: return LIBSL_KEY_INSERT;
  case VK_CONTROL: return LIBSL_KEY_CTRL;
  case VK_RMENU: return LIBSL_KEY_ALT;
  case VK_LMENU: return LIBSL_KEY_ALT;
  case VK_SHIFT: return LIBSL_KEY_SHIFT;
  case VK_TAB: return LIBSL_KEY_TAB;
  case VK_BACK: return LIBSL_KEY_BK_SPACE;
  case VK_ESCAPE: return LIBSL_KEY_ESC;
  case VK_DELETE: return LIBSL_KEY_DELETE;
  }
  return 0;
}


static LRESULT CALLBACK SimpleUI_gl_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  if (SimpleUI_Custom_WndProc != NULL) {
    if (SimpleUI_Custom_WndProc(hWnd,message,wParam,lParam)) {
      return 0;
    }
  }
  switch (message)
  {
  case WM_ERASEBKGND:
    // Do nothing = do not erase the background
    return 0;

  case WM_PAINT:
    // FC: adding missing BeginPaint/EndPaint
    // Windows continuously sends WM_PAINT messages if the WM_PAINT handler doesn't call BeginPaint/EndPaint,
    // eg. WM_PAINT messages are continuously transiting through the message loop and PeekMessage always returns TRUE.
    // This causes the code supposed to refresh the window ( if (s_AlwaysRefresh) ) to _never_ be called, but the window
    // is still continuously being repainted as WM_PAINT messages are processed.
    // Not calling BeginPaint/EndPaint causes some issues on my system on startup.
    // Calling GetUpdateRect is required as per strictly mentionned in the WM_PAINT handling spec.
    
    if (GetUpdateRect(hWnd, NULL, FALSE)) /// SL: test
    {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps);
      SimpleUI_gl_Render();
      SwapBuffers(s_hDC);
      EndPaint(hWnd, &ps);
      return 0;
    }
    //else
    //  return DefWindowProc(hWnd, message, wParam, lParam);

  case WM_CREATE:
    return 0;

  case WM_CLOSE:
    PostQuitMessage( 0 );
    return 0;

  case WM_DESTROY:
    return 0;

  case WM_KEYDOWN:
    {
      uint scancode = (lParam >> 16) & 0xFF;

      uint vk = MapVirtualKey(scancode, MAPVK_VSC_TO_VK);
      uint lsc = win32_VirtualKey_to_LibSL_scancode(vk);

      BYTE keyboardState[256];
      GetKeyboardState(keyboardState);
      char charvalue[2];
      int toasc = ToAsciiEx((uint)wParam, scancode, keyboardState, (LPWORD)&charvalue[0], 0, GetKeyboardLayout(0));
      if (toasc == 1 && charvalue[0] != 8 /*backspace*/) {
        //std::cerr << "charvalue = " << (int)charvalue[0] << std::endl;
        NAMESPACE::onKeyPressed(charvalue[0]);
      } else if (lsc > 0) {
        //std::cerr << "scancode = " << lsc << std::endl;
        NAMESPACE::onScanCodePressed(lsc);
      }

      return 0;
    }

  case WM_KEYUP:
    {
      uint scancode = (lParam >> 16) & 0xFF;

      uint vk = MapVirtualKey(scancode, MAPVK_VSC_TO_VK);
      uint lsc = win32_VirtualKey_to_LibSL_scancode(vk);

      BYTE keyboardState[256];
      GetKeyboardState(keyboardState);
      char charvalue[2];
      int toasc = ToAsciiEx((uint)wParam, scancode, keyboardState, (LPWORD)&charvalue[0], 0, GetKeyboardLayout(0));
      if (toasc == 1 && charvalue[0] != 8 /*backspace*/) {
        //std::cerr << "charvalue = " << (int)charvalue[0] << std::endl;
        NAMESPACE::onKeyUnpressed(charvalue[0]);
      } else if (lsc > 0) {
        //std::cerr << "scancode = " << lsc << std::endl;
        NAMESPACE::onScanCodeUnpressed(lsc);
      }

      return 0;
    }

  case WM_MOUSEMOVE:
    {
      int xPos = LOWORD(lParam);
      int yPos = HIWORD(lParam);
      NAMESPACE::onMouseMotion(xPos,yPos);
    }
    return 0;

  case WM_LBUTTONDOWN: {
    int xPos = LOWORD(lParam);
    int yPos = HIWORD(lParam);
    NAMESPACE::onMouseButtonPressed(xPos,yPos,LIBSL_LEFT_BUTTON,LIBSL_BUTTON_DOWN);
                       } return 0;
  case WM_LBUTTONUP: {
    int xPos = LOWORD(lParam);
    int yPos = HIWORD(lParam);
    NAMESPACE::onMouseButtonPressed(xPos,yPos,LIBSL_LEFT_BUTTON,LIBSL_BUTTON_UP);
                     } return 0;

  case WM_MBUTTONDOWN: {
    int xPos = LOWORD(lParam);
    int yPos = HIWORD(lParam);
    NAMESPACE::onMouseButtonPressed(xPos,yPos,LIBSL_MIDDLE_BUTTON,LIBSL_BUTTON_DOWN);
                       } return 0;
  case WM_MBUTTONUP: {
    int xPos = LOWORD(lParam);
    int yPos = HIWORD(lParam);
    NAMESPACE::onMouseButtonPressed(xPos,yPos,LIBSL_MIDDLE_BUTTON,LIBSL_BUTTON_UP);
                     } return 0;

  case WM_RBUTTONDOWN: {
    int xPos = LOWORD(lParam);
    int yPos = HIWORD(lParam);
    NAMESPACE::onMouseButtonPressed(xPos,yPos,LIBSL_RIGHT_BUTTON,LIBSL_BUTTON_DOWN);
                       } return 0;
  case WM_RBUTTONUP: {
    int xPos = LOWORD(lParam);
    int yPos = HIWORD(lParam);
    NAMESPACE::onMouseButtonPressed(xPos,yPos,LIBSL_RIGHT_BUTTON,LIBSL_BUTTON_UP);
                     } return 0;

  case WM_MOUSEWHEEL: {
    NAMESPACE::onMouseWheel( GET_WHEEL_DELTA_WPARAM(wParam) );
                     } return 0;

  case WM_SIZE:
    s_ScreenW = LOWORD(lParam);
    s_ScreenH = HIWORD(lParam);
    NAMESPACE::onReshape(s_ScreenW,s_ScreenH);
    return 0;

  default:
    return DefWindowProc( hWnd, message, wParam, lParam );
  }
}

// Enable OpenGL
static bool enableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC)
{
  PIXELFORMATDESCRIPTOR pfd;
  int format;

  // get the device context (DC)
  *hDC = GetDC( hWnd );
  if (!hDC) {
    return false;
  }

  // set the pixel format for the DC
  ZeroMemory( &pfd, sizeof( pfd ) );
  pfd.nSize        = sizeof( pfd );
  pfd.nVersion     = 1;
  pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType   = PFD_TYPE_RGBA;
  pfd.cColorBits   = 24;
  pfd.cDepthBits   = 24;
  pfd.cStencilBits = 8;
  pfd.cAlphaBits   = 8;
  pfd.iLayerType = PFD_MAIN_PLANE;
  format = ChoosePixelFormat( *hDC, &pfd );
  if (!format) {
    return false;
  }
  SetPixelFormat( *hDC, format, &pfd );

  // create and enable the render context (RC)
  *hRC = wglCreateContext( *hDC );
  if (!*hRC) {
    return false;
  }

  wglMakeCurrent( *hDC, *hRC );

  return true;
}

#ifdef OPENGLCORE

// see https://mariuszbartosik.com/opengl-4-x-initialization-in-windows-without-a-framework/
static bool enabelCoreProfile(HWND * hWnd, HDC * hDC, HGLRC * hRC, HWND hWndCP)
{
  PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));

  const int pixelAttribs[] = {
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
    WGL_COLOR_BITS_ARB, 24,
    WGL_ALPHA_BITS_ARB, 8,
    WGL_DEPTH_BITS_ARB, 24,
    WGL_STENCIL_BITS_ARB, 8,
    0
  };
  int contextAttributes[] = {
    WGL_CONTEXT_MAJOR_VERSION_ARB, LIBSL_OPENGL_MAJOR_VERSION,
    WGL_CONTEXT_MINOR_VERSION_ARB, LIBSL_OPENGL_MINOR_VERSION,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, 
    0 };

  HDC hDCCP = GetDC(hWndCP);
  PIXELFORMATDESCRIPTOR PFD;
  int pixelFormatID; UINT numFormats;
  BOOL status = wglChoosePixelFormatARB(hDCCP, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats);
  DescribePixelFormat(hDCCP, pixelFormatID, sizeof(PFD), &PFD);
  SetPixelFormat(hDCCP, pixelFormatID, &PFD);
  HGLRC hRCCP = wglCreateContextAttribsARB(hDCCP, NULL, contextAttributes);

  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(*hRC);
  ReleaseDC(*hWnd, *hDC);
  DestroyWindow(*hWnd);

  if (!wglMakeCurrent(hDCCP, hRCCP) || hRCCP == NULL) {
    wglDeleteContext(hRCCP);
    ReleaseDC(hWndCP, hDCCP);
    *hWnd = hWndCP;
    return false;
  }

  *hWnd = hWndCP;
  *hDC = hDCCP;
  *hRC = hRCCP;
  return true;
}

#endif

// Disable OpenGL
static void disableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( hRC );
	ReleaseDC( hWnd, hDC );
}

HMONITOR      g_HMonitors[16];
MONITORINFOEX g_Monitors[16];
uint          g_NumMonitors = 0;

BOOL CALLBACK monitorEnumProc(
   HMONITOR hMonitor,
   HDC hdcMonitor,
   LPRECT lprcMonitor,
   LPARAM dwData
)
{
  sl_assert( g_NumMonitors < 16 );
  g_HMonitors[ g_NumMonitors ]        = hMonitor;
  g_Monitors [ g_NumMonitors ].cbSize = sizeof(MONITORINFOEX);
  GetMonitorInfo( hMonitor, &g_Monitors[ g_NumMonitors ] );
  g_NumMonitors ++;
  return TRUE;
}

void NAMESPACE::init(
  uint width, uint height,
  const char *title,
  char **argv, int argc,
  bool frameLess,
  bool hidden,
  bool fullscreen)
{
  WNDCLASS wc;

  // register window class
  HINSTANCE hInstance = GetModuleHandle(NULL);
  wc.style = CS_OWNDC;
  wc.lpfnWndProc = SimpleUI_gl_WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = frameLess ? NULL : LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = L"SimpleUI::GL";
  RegisterClass(&wc);

  RECT rc;
  rc.top = 0; rc.left = 0;
  rc.right = width; rc.bottom = height;

  if (fullscreen) { // Full screen from NeHe tutorial

    g_NumMonitors = 0;
    EnumDisplayMonitors(NULL, NULL, monitorEnumProc, NULL);
    ForIndex(i, g_NumMonitors) {
      if (!(g_Monitors[i].dwFlags & MONITORINFOF_PRIMARY)) {
        // prefer secondary monitor
        rc = g_Monitors[i].rcMonitor;
        std::cerr << sprint("[SimpleUI] selected monitor %ls\n", g_Monitors[i].szDevice);
        std::cerr << sprint("[SimpleUI] %d %d %d %d\n", g_Monitors[i].rcMonitor.left, g_Monitors[i].rcMonitor.right, g_Monitors[i].rcMonitor.top, g_Monitors[i].rcMonitor.bottom);
        break;
      }
    }

    /*
    DEVMODE dmScreenSettings;								// Device Mode
    memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
    dmScreenSettings.dmSize=sizeof(dmScreenSettings);		  // Size Of The Devmode Structure
    dmScreenSettings.dmPelsWidth	= width;				        // Selected Screen Width
    dmScreenSettings.dmPelsHeight	= height;				        // Selected Screen Height
    dmScreenSettings.dmBitsPerPel	= 32;	          				// Selected Bits Per Pixel
    dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
    if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
      // failed
      throw Fatal("Cannot change display settings to fullscreen.");
    }
    */
  }

  DWORD style = 0;
  DWORD exStyle = 0;
  if (fullscreen) {
    style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;	// Windows Style
    exStyle = WS_EX_APPWINDOW;								// Window Extended Style
    // ShowCursor(FALSE);	// Hide Mouse Pointer
  } else {
    // create main window
    style = frameLess ? (WS_POPUP) : (WS_CAPTION | WS_POPUPWINDOW | /*WS_BORDER |*/ WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
    exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
  }
  style |= CS_OWNDC;

  RECT before = rc;
  AdjustWindowRectEx(&rc, style, false, exStyle);
  rc.bottom += (before.top - rc.top);
  rc.right += (before.left - rc.left);
  rc.top = before.top;
  rc.left = before.left;

  s_hWnd = CreateWindowEx(
    0,
    L"SimpleUI::GL", toUnicode(title),
    style,
    rc.left, rc.top,
    rc.right - rc.left, rc.bottom - rc.top,
    NULL, NULL, hInstance, NULL);

  if (!s_hWnd) {
    // failed
    ChangeDisplaySettings(NULL, 0);
    throw Fatal("Cannot change display settings to fullscreen.");
  }

  // enable OpenGL for the window
  bool success = enableOpenGL(s_hWnd, &s_hDC, &s_hRC);
  if (!success) {
    // failed
    ChangeDisplaySettings(NULL, 0);
    throw Fatal("Cannot enable opengl.");
  }

#ifdef OPENGLCORE
  HWND hWnd = CreateWindowEx(
    0, 
    L"SimpleUI::GL", toUnicode(title), 
    style, rc.left, rc.top, 
    rc.right - rc.left, rc.bottom - rc.top, 
    NULL, NULL, hInstance, NULL);
  success = enabelCoreProfile(&s_hWnd, &s_hDC, &s_hRC, hWnd);
  if (!success) {
    // failed core-profile intialization, retry with compatibility mode
    success = enableOpenGL(s_hWnd, &s_hDC, &s_hRC);
    if (!success) {
      ChangeDisplaySettings(NULL, 0);
      throw Fatal("Cannot enable opengl.");
    }
  }
#endif
  
  s_FullScreen = fullscreen;

#ifndef EMSCRIPTEN
#ifdef USE_GLUX
  gluxInit();
#endif
#endif

  NAMESPACE::onReshape(width,height);

  LibSL::GLHelpers::GLBasicPipeline::init();

  glEnable(GL_DEPTH_TEST);

}

void NAMESPACE::refresh()
{
  InvalidateRect(s_hWnd, NULL, FALSE);
  UpdateWindow(s_hWnd);
}

void NAMESPACE::loop()
{
  // show window
  ShowWindow(s_hWnd,SW_SHOW);
  // program main loop
  bool quit = false;
	while ( !quit ) {
		// check for messages
	  MSG msg;
  	if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )  ) {
			// handle or dispatch messages
      if ( msg.message == WM_QUIT ) {
				quit = true;
			} else {
				TranslateMessage( &msg );
				DispatchMessage ( &msg );
			}
		} else {
      if (s_AlwaysRefresh) { // this code is currently never executed (see the way WM_PAINT is handled in this file). Change WM_PAINT handler to fix (and match Linux/Glut implementation)
        NAMESPACE::refresh();
      }
		}
	}

}

void NAMESPACE::shutdown()
{
  LibSL::GLHelpers::GLBasicPipeline::terminate();

  // shutdown OpenGL
	disableOpenGL( s_hWnd, s_hDC, s_hRC );
	// destroy the window explicitly
	DestroyWindow( s_hWnd );

  if (s_FullScreen) {
    // restore display
    ChangeDisplaySettings(NULL,0);
    s_FullScreen = false;
  }
}

void NAMESPACE::exit()
{
  PostQuitMessage(0);
}

void *NAMESPACE::glContext()
{
  return (s_hRC);
}

void NAMESPACE::glMakeCurrent()
{
  wglMakeCurrent( s_hDC, s_hRC );
}

void  NAMESPACE::glShowWindow(bool hide)
{
  ShowWindow(s_hWnd,hide ? SW_HIDE : SW_SHOW);
}

void  NAMESPACE::glSwapBuffers()
{
  wglMakeCurrent( s_hDC, s_hRC );
  SwapBuffers( s_hDC );
}

HWND NAMESPACE::getHWND()
{
  return s_hWnd;
}

#endif

#endif // OPENGL

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifdef DIRECT3D
#define DIRECT3D_OR_DIRECTX10
#endif

#ifdef DIRECTX10
#define DIRECT3D_OR_DIRECTX10
#endif

#ifdef DIRECT3D_OR_DIRECTX10

//---------------------------------------------------------------------------
// DirectX common implementation - uses DXUT
//---------------------------------------------------------------------------

static VOID APIENTRY dxutOnDestroyDevice(void* pUserContext)
{

}

static VOID APIENTRY dxutOnLostDevice(void* pUserContext)
{

}

static bool s_MouseButtonPreviousState[3]={false,false,false};

static VOID APIENTRY dxutOnMouse(
                          bool bLeftButtonDown,
                          bool bRightButtonDown,
                          bool bMiddleButtonDown,
                          bool bSideButton1Down,
                          bool bSideButton2Down,
                          INT nMouseWheelDelta,
                          INT xPos,
                          INT yPos,
                          void* pUserContext)
{
  uint button=0,flags=0;

  if (bLeftButtonDown && bRightButtonDown) {
    bLeftButtonDown=bRightButtonDown=false;
    bMiddleButtonDown=true;
  }

  if (bLeftButtonDown  )       button=LIBSL_LEFT_BUTTON;
  else if (bMiddleButtonDown)  button=LIBSL_MIDDLE_BUTTON;
  else if (bRightButtonDown )  button=LIBSL_RIGHT_BUTTON;

  // cerr << sprint(" %d %d %d \n",bLeftButtonDown,bMiddleButtonDown,bRightButtonDown);

  if ( (bLeftButtonDown   && !s_MouseButtonPreviousState[0])
    || (bMiddleButtonDown && !s_MouseButtonPreviousState[1])
    || (bRightButtonDown  && !s_MouseButtonPreviousState[2])
       ) {
    flags |= LIBSL_BUTTON_DOWN;
  }

  if (s_MouseButtonPreviousState[0] && !bLeftButtonDown) {
    button = LIBSL_LEFT_BUTTON;
    flags |= LIBSL_BUTTON_UP;
  }
  if (s_MouseButtonPreviousState[1] && !bMiddleButtonDown) {
    button = LIBSL_MIDDLE_BUTTON;
    flags |= LIBSL_BUTTON_UP;
  }
  if (s_MouseButtonPreviousState[2] && !bRightButtonDown) {
    button = LIBSL_RIGHT_BUTTON;
    flags |= LIBSL_BUTTON_UP;
  }

  if ((flags & LIBSL_BUTTON_DOWN) || (flags & LIBSL_BUTTON_UP)) {
    NAMESPACE::onMouseButtonPressed(xPos,yPos,button,flags);
  }

  s_MouseButtonPreviousState[0] = bLeftButtonDown;
  s_MouseButtonPreviousState[1] = bMiddleButtonDown;
  s_MouseButtonPreviousState[2] = bRightButtonDown;

  NAMESPACE::onMouseMotion(xPos,yPos);

  if (nMouseWheelDelta != 0) {
    NAMESPACE::onMouseWheel( nMouseWheelDelta );
  }
}


static VOID APIENTRY dxutOnKeyboard(
                             UINT nChar,
                             bool bKeyDown,
                             bool bAltDown,
                             void* pUserContext)
{
  if (bKeyDown) {
    NAMESPACE::onKeyPressed  (nChar);
  } else {
    NAMESPACE::onKeyUnpressed(nChar);
  }
}

LRESULT CALLBACK dxutMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
  *pbNoFurtherProcessing = false;

  if (SimpleUI_Custom_WndProc != NULL) {
    if (SimpleUI_Custom_WndProc(hWnd,uMsg,wParam,lParam)) {
      return 0;
    }
  }

  switch (uMsg)
  {
  case WM_KEYDOWN:
    NAMESPACE::onScanCodePressed((lParam >> 16) & 0xFF);
    break;

  case WM_KEYUP:
    NAMESPACE::onScanCodeUnpressed((lParam >> 16) & 0xFF);
    break;
  }
  return S_OK;
}

void NAMESPACE::loop()
{
  DXUTMainLoop(NULL);
}

void NAMESPACE::shutdown()
{
  DXUTShutdown();
}

void NAMESPACE::exit()
{
  PostMessage(DXUTGetHWND(),WM_QUIT,0,0);
}

HWND NAMESPACE::getHWND()
{
  return DXUTGetHWND();
}

#endif

#ifdef DIRECT3D

//---------------------------------------------------------------------------
// DirectX 9 implementation - uses DXUT
//---------------------------------------------------------------------------

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxut.h>

#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/D3DHelpers/D3DHelpers.h>

static VOID APIENTRY dxutOnFrameRender(
                                IDirect3DDevice9 * pd3dDevice,
                                DOUBLE fTime,
                                FLOAT fElapsedTime,
                                void* pUserContext)
{
  static double last = (double)LibSL::System::Time::milliseconds();
  double now         = (double)LibSL::System::Time::milliseconds();
  float  elapsed     = float(now - last);

  if (elapsed > 0) {
    NAMESPACE::onAnimate( now, elapsed );
    last = now;
  }

  LIBSL_D3D_CHECK_ERROR(LIBSL_DIRECT3D_DEVICE->BeginScene());
  NAMESPACE::onRender();
  LIBSL_D3D_CHECK_ERROR(LIBSL_DIRECT3D_DEVICE->EndScene());

}

static HRESULT APIENTRY dxutOnResetDevice(
                                   IDirect3DDevice9 * pd3dDevice,
                                   CONST D3DSURFACE_DESC * pBackBufferSurfaceDesc,
                                   void* pUserContext)
{
  return S_OK;
}

static void dxutGetSupportedTextureFormat( IDirect3D9 *pD3D, D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
				    D3DFORMAT* pfmtTexture, D3DFORMAT* pfmtCubeMap )
{
  D3DFORMAT fmtTexture = D3DFMT_UNKNOWN;
  D3DFORMAT fmtCubeMap = D3DFMT_UNKNOWN;

  // check for linear filtering support of signed formats
  fmtTexture = D3DFMT_UNKNOWN;
  if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,AdapterFormat,
    D3DUSAGE_QUERY_FILTER,D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F))) {
    fmtTexture = D3DFMT_A16B16G16R16F;
  }
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,AdapterFormat,
    D3DUSAGE_QUERY_FILTER,D3DRTYPE_TEXTURE, D3DFMT_Q16W16V16U16))) {
    fmtTexture = D3DFMT_Q16W16V16U16;
  }
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType, AdapterFormat,
    D3DUSAGE_QUERY_FILTER,D3DRTYPE_TEXTURE, D3DFMT_Q8W8V8U8))) {
    fmtTexture = D3DFMT_Q8W8V8U8;
  }
  // no support for linear filtering of signed, just checking for format support now
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType, AdapterFormat,
    0, D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F))) {
    fmtTexture = D3DFMT_A16B16G16R16F;
  }
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType, AdapterFormat,
    0, D3DRTYPE_TEXTURE, D3DFMT_Q16W16V16U16))) {
    fmtTexture = D3DFMT_Q16W16V16U16;
  }
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType, AdapterFormat,
    0, D3DRTYPE_TEXTURE, D3DFMT_Q8W8V8U8))) {
    fmtTexture = D3DFMT_Q8W8V8U8;
  }

  // check for support linear filtering of signed format cubemaps
  fmtCubeMap = D3DFMT_UNKNOWN;
  if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,AdapterFormat,
    D3DUSAGE_QUERY_FILTER, D3DRTYPE_CUBETEXTURE, D3DFMT_A16B16G16R16F))) {
    fmtCubeMap = D3DFMT_A16B16G16R16F;
  }
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,AdapterFormat,
    D3DUSAGE_QUERY_FILTER,D3DRTYPE_CUBETEXTURE, D3DFMT_Q16W16V16U16))) {
    fmtCubeMap = D3DFMT_Q16W16V16U16;
  }
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,AdapterFormat,
    D3DUSAGE_QUERY_FILTER,D3DRTYPE_CUBETEXTURE, D3DFMT_Q8W8V8U8))) {
    fmtCubeMap = D3DFMT_Q8W8V8U8;
  }
  // no support for linear filtering of signed formats, just checking for format support now
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,AdapterFormat,
    0,D3DRTYPE_CUBETEXTURE, D3DFMT_A16B16G16R16F))) {
    fmtCubeMap = D3DFMT_A16B16G16R16F;
  }
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,AdapterFormat,
    0,D3DRTYPE_CUBETEXTURE, D3DFMT_Q16W16V16U16))) {
    fmtCubeMap = D3DFMT_Q16W16V16U16;
  }
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,AdapterFormat,
    0,D3DRTYPE_CUBETEXTURE, D3DFMT_Q8W8V8U8))) {
    fmtCubeMap = D3DFMT_Q8W8V8U8;
  }

  if( pfmtTexture ) *pfmtTexture = fmtTexture;
  if( pfmtCubeMap ) *pfmtCubeMap = fmtCubeMap;
}



static bool CALLBACK dxutIsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
                                 D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
  // Skip backbuffer formats that don't support alpha blending
#ifdef DXUT_OLD_VERSION
  IDirect3D9* pD3D = DXUTGetD3DObject();
#else
  IDirect3D9* pD3D = DXUTGetD3D9Object();
#endif
  if( FAILED( pD3D->CheckDeviceFormat(
    pCaps->AdapterOrdinal, pCaps->DeviceType,
    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
    return false;

  // Determine texture support.  Fail if not good enough
  D3DFORMAT fmtTexture, fmtCubeMap;
  dxutGetSupportedTextureFormat( pD3D, pCaps, AdapterFormat, &fmtTexture, &fmtCubeMap );
  if (D3DFMT_UNKNOWN == fmtTexture || D3DFMT_UNKNOWN == fmtCubeMap)
    return false;

  // Requires pixel shader 3.0
  if(pCaps->PixelShaderVersion < D3DPS_VERSION(3,0))
    return false;

  return true;
}



static bool CALLBACK dxutModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
#ifdef DXUT_OLD_VERSION
  pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D24S8;
#else
  pDeviceSettings->d3d9.pp.AutoDepthStencilFormat = D3DFMT_D24S8;
#endif
  return true;
}


static HRESULT APIENTRY dxutOnCreateDevice(
                                    IDirect3DDevice9 * pd3dDevice,
                                    CONST D3DSURFACE_DESC * pBackBufferSurfaceDesc,
                                    void* pUserContext)
{
  s_ScreenW = pBackBufferSurfaceDesc->Width;
  s_ScreenH = pBackBufferSurfaceDesc->Height;
  return S_OK;
}


void NAMESPACE::init(uint width,uint height,const char *title,char **argv,int argc,bool frameLess,bool hidden,bool fullScreen)
{
  sl_assert( ! fullScreen );

  DXUTInit(false/*parse cmd line*/,true/*show msg on error*/,L"foo -forceapi:9");
#ifdef DXUT_OLD_VERSION
  DXUTSetCallbackDeviceCreated      ( dxutOnCreateDevice );
  DXUTSetCallbackDeviceReset        ( dxutOnResetDevice );
  DXUTSetCallbackDeviceLost         ( dxutOnLostDevice );
  DXUTSetCallbackDeviceDestroyed    ( dxutOnDestroyDevice );
  DXUTSetCallbackFrameRender        ( dxutOnFrameRender );
#else
  DXUTSetCallbackD3D9DeviceCreated  ( dxutOnCreateDevice );
  DXUTSetCallbackD3D9DeviceReset    ( dxutOnResetDevice );
  DXUTSetCallbackD3D9DeviceLost     ( dxutOnLostDevice );
  DXUTSetCallbackD3D9DeviceDestroyed( dxutOnDestroyDevice );
  DXUTSetCallbackD3D9FrameRender    ( dxutOnFrameRender );
#endif
  if (title == NULL) {
    DXUTCreateWindow(L"LibSL::SimpleUI (DX9)");
  } else {
    DXUTCreateWindow(CppHelpers::toUnicode(title));
  }
#ifdef DXUT_OLD_VERSION
  DXUTCreateDevice( D3DADAPTER_DEFAULT, true /*windowed*/, width, height);
  LIBSL_D3D_DEVICE = DXUTGetD3DDevice();
#else
  DXUTCreateDevice( true /*windowed*/, width, height);
  LIBSL_D3D_DEVICE = DXUTGetD3D9Device();
#endif
  DXUTSetCallbackKeyboard( dxutOnKeyboard );
  DXUTSetCallbackMouse( dxutOnMouse , true );
  DXUTSetCallbackMsgProc( dxutMsgProc );
  NAMESPACE::onReshape(width,height);
}

void NAMESPACE::init(HWND hwnd,LPDIRECT3DDEVICE9 device)
{
  // DXUTSetWindow(hwnd,hwnd,hwnd,true);
  // DXUTSetD3D9Device(device);
  LIBSL_D3D_DEVICE = device;

/*
#ifdef DXUT_OLD_VERSION
  DXUTSetCallbackDeviceCreated      ( dxutOnCreateDevice );
  DXUTSetCallbackDeviceReset        ( dxutOnResetDevice );
  DXUTSetCallbackDeviceLost         ( dxutOnLostDevice );
  DXUTSetCallbackDeviceDestroyed    ( dxutOnDestroyDevice );
  DXUTSetCallbackFrameRender        ( dxutOnFrameRender );
#else
  DXUTSetCallbackD3D9DeviceCreated  ( dxutOnCreateDevice );
  DXUTSetCallbackD3D9DeviceReset    ( dxutOnResetDevice );
  DXUTSetCallbackD3D9DeviceLost     ( dxutOnLostDevice );
  DXUTSetCallbackD3D9DeviceDestroyed( dxutOnDestroyDevice );
  DXUTSetCallbackD3D9FrameRender    ( dxutOnFrameRender );
#endif

  DXUTSetCallbackKeyboard( dxutOnKeyboard );
  DXUTSetCallbackMsgProc( dxutMsgProc );
  DXUTSetCallbackMouse( dxutOnMouse , true );
  // NAMESPACE::onReshape(width,height);
*/
}

#endif // DIRECT3D

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifdef DIRECTX10

//---------------------------------------------------------------------------
// DirectX 10 implementation - uses DXUT
//---------------------------------------------------------------------------

#include <windows.h>
#include <dxut.h>

#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/D3DHelpers/D3DHelpers.h>

static VOID APIENTRY dxutOnFrameRender(
                                ID3D10Device* pd3dDevice,
                                DOUBLE fTime,
                                FLOAT fElapsedTime,
                                void* pUserContext)
{
  static double last = (double)LibSL::System::Time::milliseconds();
  double now         = (double)LibSL::System::Time::milliseconds();
  float  elapsed     = float(now - last);

  if (elapsed > 0) {
    NAMESPACE::onAnimate( now, elapsed );
    last = now;
  }

  NAMESPACE::onRender();

}

bool CALLBACK IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
  return true;
}

HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC *pBackBufferSurfaceDesc, void* pUserContext )
{
  return S_OK;
}

HRESULT CALLBACK OnD3D10ResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain *pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
  NAMESPACE::onReshape(pBackBufferSurfaceDesc->Width,pBackBufferSurfaceDesc->Height);
  return S_OK;
}

void CALLBACK OnD3D10ReleasingSwapChain( void* pUserContext )
{

}

void CALLBACK OnD3D10DestroyDevice( void* pUserContext )
{

}

void NAMESPACE::init(uint width,uint height,const char *title,char **argv,int argc,bool frameLess,bool hidden,bool fulscreen)
{
  // Set DXUT callbacks
  DXUTInit( true, true, NULL );
  DXUTSetCursorSettings( true, true );

  DXUTSetCallbackKeyboard ( dxutOnKeyboard );
  DXUTSetCallbackMsgProc( dxutMsgProc );
  DXUTSetCallbackMouse    ( dxutOnMouse , true );

  DXUTSetCallbackD3D10DeviceAcceptable  ( IsD3D10DeviceAcceptable );
  DXUTSetCallbackD3D10SwapChainResized  ( OnD3D10ResizedSwapChain );
  DXUTSetCallbackD3D10FrameRender       ( dxutOnFrameRender );
  DXUTSetCallbackD3D10SwapChainReleasing( OnD3D10ReleasingSwapChain );
  DXUTSetCallbackD3D10DeviceDestroyed   ( OnD3D10DestroyDevice );
  DXUTSetCallbackD3D10DeviceCreated     ( OnD3D10CreateDevice );

  if (title == NULL) {
    DXUTCreateWindow(L"LibSL::SimpleUI (DX10)");
  } else {
    DXUTCreateWindow(CppHelpers::toUnicode(title));
  }

  DXUTCreateDevice( true, width, height );
  LIBSL_D3D_DEVICE = DXUTGetD3D10Device();

  NAMESPACE::onReshape(width,height);
}

#endif // DIRECTX10

//---------------------------------------------------------------------------
