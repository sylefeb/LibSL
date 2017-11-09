// GLPanelBinder.cpp : Defines the exported functions for the DLL application.
//

#include <Windows.h>
#include "GLPanelBinder.h"

#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>

using namespace LibSL;

// This is an example of an exported variable
GLPANELBINDER_API int nGLPanelBinder = 0;

#define NAMESPACE GLPanelBinder

//---------------------------------------------------------------------------

static void defaultOnRender(HWND,uint w,uint h)
{
  GPUHelpers::clearScreen( LIBSL_COLOR_BUFFER, 1.0f, 1.0f, 0.0f);

  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_CULL_FACE);
  static float t = 0;
  t += 0.01f;
  GPUHelpers::Transform::ortho2D(LIBSL_PROJECTION_MATRIX,0,1,0,1);
  GPUHelpers::Transform::identity(LIBSL_MODELVIEW_MATRIX);
  glTranslatef(0.5f + cos(t)*0.2f,0.5f + sin(t)*0.2f,0);
  glScalef(0.1f,0.1f,0.1f);
  glColor3f(0.5f,0.5f,0.5f);
  glBegin(GL_QUADS);
  glVertex2i(0,0);  glVertex2i(0,1);
  glVertex2i(1,1);  glVertex2i(1,0);
  glEnd();
}

//---------------------------------------------------------------------------

static void defaultOnReshape(HWND)
{

}

//---------------------------------------------------------------------------

static void defaultOnMouseMotion(HWND,uint,uint,uint,uint)
{

}

//---------------------------------------------------------------------------

static void defaultOnMouseButtonPressed(HWND,uint,uint,uint,uint,uint,uint)
{

}

//---------------------------------------------------------------------------

std::map<HWND,NAMESPACE::f_onRender>             NAMESPACE::onRender             ;
std::map<HWND,NAMESPACE::f_onReshape>            NAMESPACE::onReshape            ;
std::map<HWND,NAMESPACE::f_onMouseMotion>        NAMESPACE::onMouseMotion        ;
std::map<HWND,NAMESPACE::f_onMouseButtonPressed> NAMESPACE::onMouseButtonPressed ;

std::map<HWND,uint>               NAMESPACE::ScreenW;
std::map<HWND,uint>               NAMESPACE::ScreenH;

//---------------------------------------------------------------------------

std::map<HWND,HDC>   g_HDC;
std::map<HWND,HGLRC> g_GLRC;

GLPANELBINDER_API void __stdcall GLPanelInit(HWND hwnd)
{
  PIXELFORMATDESCRIPTOR pfd;
  int format = 0;

  // get the device context (DC)
  g_HDC[hwnd] = GetDC( hwnd );

  // set the pixel format for the DC
  ZeroMemory( &pfd, sizeof( pfd ) );
  pfd.nSize        = sizeof( pfd );
  pfd.nVersion     = 1;
  pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType   = PFD_TYPE_RGBA;
  pfd.cColorBits   = 24;
  pfd.cDepthBits   = 24;
  pfd.cStencilBits = 8;
  pfd.iLayerType   = PFD_MAIN_PLANE;
  format = ChoosePixelFormat( g_HDC[hwnd], &pfd );
  SetPixelFormat( g_HDC[hwnd], format, &pfd );

  // create and enable the render context (RC)
  HGLRC ctx = wglCreateContext( g_HDC[hwnd] );
  // share with other
  //for ( std::map<HWND,HGLRC>::const_iterator C=g_GLRC.begin() ; C!=g_GLRC.end() ; C++ ) {
  //  wglShareLists(ctx,(*C).second);
  //}
  wglMakeCurrent( NULL, NULL );
  if (!g_GLRC.empty()) {
    wglShareLists( (*g_GLRC.begin()).second , ctx );
  }
  g_GLRC[hwnd] = ctx;
  // make current
  wglMakeCurrent( g_HDC[hwnd], g_GLRC[hwnd] );

  // init defaults
  NAMESPACE::onRender[hwnd] = defaultOnRender;
  NAMESPACE::onReshape[hwnd] = defaultOnReshape;
  NAMESPACE::onMouseMotion[hwnd] = defaultOnMouseMotion;
  NAMESPACE::onMouseButtonPressed[hwnd] = defaultOnMouseButtonPressed;
  NAMESPACE::ScreenW[hwnd] = 0;
  NAMESPACE::ScreenH[hwnd] = 0;

  nGLPanelBinder = 1;
}

GLPANELBINDER_API void __stdcall GLPaint(HWND hwnd,int w,int h)
{
  sl_assert( g_HDC.find(hwnd) != g_HDC.end() );

  wglMakeCurrent( g_HDC[hwnd], g_GLRC[hwnd] );
  glViewport(0,0,w,h);

  NAMESPACE::onRender[hwnd](hwnd,w,h);

  SwapBuffers(g_HDC[hwnd]);
}

GLPANELBINDER_API void __stdcall GLReshape(HWND hwnd,int w,int h)
{
  sl_assert( g_HDC.find(hwnd) != g_HDC.end() );

  NAMESPACE::ScreenW[hwnd] = w;
  NAMESPACE::ScreenH[hwnd] = h;
  NAMESPACE::onReshape[hwnd](hwnd);
}

GLPANELBINDER_API void __stdcall GLMouseMotion(HWND hwnd,int x,int y,int w,int h)
{
  sl_assert( g_HDC.find(hwnd) != g_HDC.end() );

  NAMESPACE::onMouseMotion[hwnd](hwnd,(uint)x,(uint)y,(uint)w,(uint)h);
}

GLPANELBINDER_API void __stdcall GLMouseButtonPressed(HWND hwnd,int x,int y,int w,int h,int btn,int flags)
{
  sl_assert( g_HDC.find(hwnd) != g_HDC.end() );

  NAMESPACE::onMouseButtonPressed[hwnd](hwnd,(uint)x,(uint)y,(uint)w,(uint)h,(uint)btn,(uint)flags);
}

GLPANELBINDER_API void __stdcall GLPanelTerminate(HWND hwnd)
{
  sl_assert( g_HDC.find(hwnd) != g_HDC.end() );

  wglMakeCurrent( NULL, NULL );
  wglDeleteContext( g_GLRC[hwnd] );
  ReleaseDC( hwnd, g_HDC[hwnd] );
}

//---------------------------------------------------------------------------
