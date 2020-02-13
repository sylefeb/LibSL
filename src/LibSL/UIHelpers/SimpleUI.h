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
// LibSL::SimpleUI
// ------------------------------------------------------
//
// Simple UI, build upon glfw / DXUT
//
// TODO / FIXME How to deal with scan codes across linux-glut / windows?
//              Introduce translation table? 
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-06-04
// ------------------------------------------------------

#pragma once

#include <LibSL/LibSL.common.h>

#include <LibSL/Memory/TraceLeaks.h>
#include <LibSL/Memory/Pointer.h>
#include <LibSL/Math/Quaternion.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/System/System.h>

#ifndef OPENGL
#ifndef DIRECT3D
#ifndef DIRECTX10
ERROR_____OPENGL_or_DIRECT3D_or_DIRECTX10_must_be_defined _;
#endif
#endif
#endif

#ifdef OPENGL
#ifdef DIRECT3D
ERROR_____either_OPENGL_or_DIRECT3D_must_be_defined__not_both _;
#endif
#ifdef DIRECTX10
ERROR_____either_OPENGL_or_DIRECTX10_must_be_defined__not_both _;
#endif
#endif

#ifdef DIRECT3D
#ifdef DIRECTX10
ERROR_____either_DIRECTX10_or_DIRECT3D_must_be_defined__not_both _;
#endif
#endif

namespace LibSL {
  namespace UIHelpers {

    namespace SimpleUI {

      typedef void (*f_onRender)(void);
      typedef void (*f_onAnimate)(double time,float elapsed);
      typedef void (*f_onKeyPressed)  (uchar k);
      typedef void (*f_onKeyUnpressed)(uchar k);
      typedef void (*f_onScanCodePressed)(uint sc);
      typedef void (*f_onScanCodeUnpressed)(uint sc);
      typedef void (*f_onMouseButtonPressed)(uint x,uint y,uint button,uint flags);
      typedef void (*f_onMouseMotion)(uint x,uint y);
      typedef void (*f_onMouseWheel)(int increment);
      typedef void (*f_onReshape)(uint x,uint y);
      typedef void (*f_onDragDrop)(uint nfiles,const char** files);

      extern f_onRender             onRender;
      extern f_onKeyPressed         onKeyPressed;
      extern f_onKeyUnpressed       onKeyUnpressed;
      extern f_onScanCodePressed    onScanCodePressed;
      extern f_onScanCodeUnpressed  onScanCodeUnpressed;
      extern f_onMouseButtonPressed onMouseButtonPressed;
      extern f_onMouseMotion        onMouseMotion;
      extern f_onMouseWheel         onMouseWheel;
      extern f_onAnimate            onAnimate;
      extern f_onReshape            onReshape;
      extern f_onDragDrop           onDragDrop;

      LIBSL_DLL void init(uint width=256,uint height=256,
        const char *title=NULL,
        char **argv=NULL,int argc=0,
        bool frameLess=false,bool hidden=false,bool fullscreen=false);
      LIBSL_DLL void loop();
      LIBSL_DLL void exit();
      LIBSL_DLL void shutdown();
      LIBSL_DLL uint screenWidth();
      LIBSL_DLL uint screenHeight();

      LIBSL_DLL void showCursor(bool show);
      LIBSL_DLL void setAlwaysRefresh(bool r);
      LIBSL_DLL void refresh();

#if defined(_WIN32) || defined(_WIN64)
      LIBSL_DLL void setCustomCallbackMsgProc(WNDPROC);
      LIBSL_DLL HWND getHWND();
#endif

#ifdef OPENGL
      /// Helper function to allow low access to OpenGL. Handle with care!
      LIBSL_DLL void *glContext();
      LIBSL_DLL void  glMakeCurrent();
      LIBSL_DLL void  glShowWindow(bool hide = false);
      LIBSL_DLL void  glSwapBuffers();
#endif
#ifdef DIRECT3D
      LIBSL_DLL void init(HWND hwnd,LPDIRECT3DDEVICE9 device);
#endif
    } //namespace LibSL::UIHelpers::SimpleUI

  } //namespace LibSL::UIHelpers
} //namespace LibSL

// ------------------------------------------------------

#define LIBSL_KEY_F1         1
#define LIBSL_KEY_F2         2
#define LIBSL_KEY_F3         3
#define LIBSL_KEY_F4         4
#define LIBSL_KEY_F5         5
#define LIBSL_KEY_F6         6
#define LIBSL_KEY_F7         7
#define LIBSL_KEY_F8         8
#define LIBSL_KEY_F9         9
#define LIBSL_KEY_F10       10
#define LIBSL_KEY_F11       11
#define LIBSL_KEY_F12       12
#define LIBSL_KEY_ENTER     13
#define LIBSL_KEY_UP        14
#define LIBSL_KEY_RIGHT     15
#define LIBSL_KEY_LEFT      16
#define LIBSL_KEY_DOWN      17
#define LIBSL_KEY_PAGE_UP   18
#define LIBSL_KEY_PAGE_DOWN 19
#define LIBSL_KEY_HOME      20
#define LIBSL_KEY_END       21
#define LIBSL_KEY_INSERT    22
#define LIBSL_KEY_CTRL      23
#define LIBSL_KEY_ALT       24
#define LIBSL_KEY_SHIFT     25
#define LIBSL_KEY_TAB       26
#define LIBSL_KEY_BK_SPACE  28
#define LIBSL_KEY_ESC       27
#define LIBSL_KEY_SPACE     32
#define LIBSL_KEY_DELETE    127

//---------------------------------------------------------------------------
