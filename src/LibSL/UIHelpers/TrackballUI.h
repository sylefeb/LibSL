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
// LibSL::TrackballUI
// ------------------------------------------------------
//
// Trackball UI, build upon glut / DXUT
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-06-04
// ------------------------------------------------------

#ifdef _MSC_VER
#pragma warning ( disable : 4996 )
#endif

#pragma once

#include <LibSL/LibSL.common.h>
#include <LibSL/UIHelpers/Trackball.h>

#include "SimpleUI.h"

namespace LibSL {
  namespace UIHelpers {

    namespace TrackballUI {

      typedef void (*f_onRender)(void);
      typedef void (*f_onAnimate)(double time,float elapsed);
      typedef void (*f_onKeyPressed)(uchar k);
      typedef void (*f_onKeyUnpressed)(uchar k);
      typedef void (*f_onScanCodePressed)(uint sc);
      typedef void (*f_onScanCodeUnpressed)(uint sc);
      typedef void (*f_onMouseButtonPressed)(uint x,uint y,uint button,uint flags);
      typedef void (*f_onMouseMotion)(uint x,uint y);
      typedef void (*f_onMouseWheel)(int increment);
      typedef void (*f_onReshape)(uint x, uint y);

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

      LIBSL_DLL void  setRadius(float r);
      LIBSL_DLL void  setCenter(const LibSL::Math::v3f& center);
      LIBSL_DLL void  stopMotion();
      LIBSL_DLL void  freezeTrackball();
      LIBSL_DLL void  unfreezeTrackball();

      LIBSL_DLL const LibSL::Math::quatf&           rotation();
      LIBSL_DLL const LibSL::Math::v3f&             translation();
      LIBSL_DLL const LibSL::Math::m4x4f            matrix();
      LIBSL_DLL LibSL::UIHelpers::Trackball&        trackball();

      LIBSL_DLL void init(uint width=256,uint height=256,const char *title=NULL,char **argv=NULL,int argc=0,bool frameLess=false,bool fullScreen=false);
      LIBSL_DLL void loop();
      LIBSL_DLL void exit();
      LIBSL_DLL void shutdown();
      LIBSL_DLL uint screenWidth();
      LIBSL_DLL uint screenHeight();

      LIBSL_DLL void trackballOnKeyPressed(uchar key);
      LIBSL_DLL void trackballOnKeyUnpressed(uchar key);
      LIBSL_DLL void trackballOnScanCodePressed(uint sc);
      LIBSL_DLL void trackballOnScanCodeUnpressed(uint sc);
      LIBSL_DLL void trackballOnMouseButtonPressed(uint x,uint y,uint button,uint flags);
      LIBSL_DLL void trackballOnMouseMotion(uint x,uint y);
      LIBSL_DLL void trackballOnMouseWheel(int incr);
      LIBSL_DLL void trackballOnReshape(uint w,uint h);
      LIBSL_DLL void trackballOnAnimate(double time,float elapsed);
      LIBSL_DLL void trackballOnRender();
      LIBSL_DLL void trackballSave(const char*);
      LIBSL_DLL void trackballLoad(const char*);

      LIBSL_DLL void savePath(const char *);
      LIBSL_DLL void loadPath(const char *,bool revert = false,bool startFromPreviousEnd = false);
      LIBSL_DLL void playPath(bool on_off,float speed = 1.0f,int fixedTimeStep=-1);
      LIBSL_DLL bool reachedPathEnd();
      LIBSL_DLL void changePathPlaySpeed(float speed);
      LIBSL_DLL void setPathTensil(float t);
      LIBSL_DLL void addCurrentViewToPath();

#ifdef DIRECT3D
      LIBSL_DLL void init(HWND hwnd,LPDIRECT3DDEVICE9 device);
#endif

#ifdef WIN32
      LIBSL_DLL void setCustomCallbackMsgProc(WNDPROC);
#endif
    } //namespace LibSL::UIHelpers::TrackballUI

  } //namespace LibSL::UIHelpers
} //namespace LibSL

// ------------------------------------------------------
