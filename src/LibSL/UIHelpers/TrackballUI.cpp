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
#include "LibSL.precompiled.h"
//---------------------------------------------------------------------------

#ifdef WIN32
#include <windows.h>
#endif

#include "TrackballUI.h"
#include "Trackball.h"

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

#ifdef OPENGL
#include <GL/gl.h>
#ifdef USE_GLUT
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#endif
#endif

//---------------------------------------------------------------------------

#define NAMESPACE LibSL::UIHelpers::TrackballUI

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
{ }
static void defaultOnMouseMotion(uint,uint)
{}
static void defaultOnMouseWheel(int)
{}
static void defaultOnAnimate(double, float)
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
// Trackball link
//---------------------------------------------------------------------------

static Trackball          s_Trackball;
static bool               s_Freezed   = false;
static bool               s_CtrlDown  = false;
static bool               s_ShiftDown = false;

static std::vector<v3f>        s_PathEyeAt[2];
static BezierCurve<v3f,1024>   s_Path     [2];
static float                   s_PathArcLen        = 0;
static bool                    s_PlayPath          = false;
static float                   s_PlayPathPos       = 0;
static float                   s_PlayPathSpeed     = 0;
static float                   s_PathTensil        = 0.1f;
static int                     s_PathFixedTimeStep = -1;

//---------------------------------------------------------------------------

static void addViewtoPath(const v3f& eye,const v3f& at);
static void preparePath();
static void resetPath();

//---------------------------------------------------------------------------

void NAMESPACE::setRadius(float r)
{
  s_Trackball.setRadius(r);
}

//---------------------------------------------------------------------------

void NAMESPACE::setCenter(const v3f& center)
{
  s_Trackball.setCenter(center);
}

//---------------------------------------------------------------------------

const quatf& NAMESPACE::rotation()
{
  return (s_Trackball.rotation());
}

//---------------------------------------------------------------------------

const v3f& NAMESPACE::translation()
{
  return (s_Trackball.translation());
}

//---------------------------------------------------------------------------

const m4x4f NAMESPACE::matrix()
{
  if (s_PlayPath) {
    float param  = s_Path[0].arc2param( s_PlayPathPos );
    return lookatMatrix( s_Path[0].pointAt(param) , s_Path[1].pointAt(param) , V3F(0,1,0) );
  } else {
    return (s_Trackball.matrix());
  }
}

//---------------------------------------------------------------------------

Trackball&  NAMESPACE::trackball()
{
  return (s_Trackball);
}

//---------------------------------------------------------------------------


void  NAMESPACE::stopMotion()
{
  s_Trackball.stopMotion();
}

//---------------------------------------------------------------------------

void  NAMESPACE::freezeTrackball()
{
  s_Trackball.stopMotion();
  s_Freezed = true;
}

//---------------------------------------------------------------------------

void  NAMESPACE::unfreezeTrackball()
{
  s_Freezed = false;
}

//---------------------------------------------------------------------------

void NAMESPACE::trackballOnKeyPressed(uchar key)
{
  if (key == 'w') {
    s_Trackball.goForward();
  } else if (key == 's') {
    s_Trackball.goBackward();
  } else if (key == 'a') {
    s_Trackball.goLeft();
  } else if (key == 'd') {
    s_Trackball.goRight();
  }
  NAMESPACE::onKeyPressed(key);
}

//---------------------------------------------------------------------------

void NAMESPACE::trackballOnKeyUnpressed(uchar key)
{
  if (key == 'w') {
    s_Trackball.stopFB();
  } else if (key == 's') {
    s_Trackball.stopFB();
  } else if (key == 'a') {
    s_Trackball.stopLR();
  } else if (key == 'd') {
    s_Trackball.stopLR();
  }
  NAMESPACE::onKeyUnpressed(key);
}

//---------------------------------------------------------------------------

void NAMESPACE::trackballLoad(const char *fname)
{
  FILE *f = NULL;
  // read from file
	fopen_s(&f, fname, "rb");
  if (f != NULL) {
    v3f ctr = 0;
    fread(&ctr,sizeof(v3f),1,f);
    TrackballUI::trackball().setCenter(ctr);
    fread(&TrackballUI::trackball().rotation()   ,sizeof(quatf),1,f);
    fread(&TrackballUI::trackball().translation(),sizeof(v3f  ),1,f);
    fclose(f);
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::trackballSave(const char *fname)
{
  FILE *f = NULL;
  // write to file
	fopen_s(&f, fname, "wb");
  if (f != NULL) {
    v3f ctr = TrackballUI::trackball().getCenter();
    fwrite(&ctr,sizeof(v3f),1,f);
    fwrite(&TrackballUI::trackball().rotation()   ,sizeof(quatf),1,f);
    fwrite(&TrackballUI::trackball().translation(),sizeof(v3f  ),1,f);
    fclose(f);
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::trackballOnScanCodePressed(uint sc)
{
#ifndef USE_GLUT
  if (sc == LIBSL_KEY_CTRL) {
    s_CtrlDown = true;
  } else if (sc >= LIBSL_KEY_F1 && sc <= LIBSL_KEY_F1 + 12) {
    int fk = sc - LIBSL_KEY_F1 + 1;
#else
  if (sc >= GLUT_KEY_F1 && sc <= GLUT_KEY_F12) {
    s_CtrlDown = glutGetModifiers() & GLUT_ACTIVE_CTRL;
    int fk     = sc - GLUT_KEY_F1 + 1;
#endif
    if (fk < 8) {
    // viewpoint
      FILE *f = NULL;
      if (!s_CtrlDown) {
        trackballLoad(sprint("trackball.F%02d",fk));
      } else {
        trackballSave(sprint("trackball.F%02d",fk));
      }
    } else {
      switch(fk)
      {
      case 8:
        if (s_PlayPath) {
          std::cerr << "[TrackballUI::Path] Cannot reset while playing." << std::endl;
        } else {
          resetPath();
        }
        break;
      case 9:
        if (s_PlayPath) {
          std::cerr << "[TrackballUI::Path] Cannot add view while playing." << std::endl;
        } else {
          addViewtoPath( trackball().eye() , trackball().eye() + trackball().forward() );
        }
        break;
      case 10:
        if (s_PlayPath) {
          s_PlayPath = false;
        } else {
          std::cerr << "[TrackballUI::Path] Playing path." << std::endl;
          playPath( true, 2.0f );
        }
        break;
      case 11:
        savePath("trackball.path");
        break;
      case 12:
        loadPath("trackball.path");
        break;
      }
    }
  }
#ifndef USE_GLUT
  if (sc == LIBSL_KEY_SHIFT) {
    s_ShiftDown = true;
  }
#endif
  NAMESPACE::onScanCodePressed(sc);
}

//---------------------------------------------------------------------------

void NAMESPACE::trackballOnScanCodeUnpressed(uint sc)
{
#ifndef USE_GLUT
  if (sc == LIBSL_KEY_CTRL) {
    s_CtrlDown = false;
  } else if (sc == LIBSL_KEY_SHIFT) {
    s_ShiftDown = false;
  }
#endif
  NAMESPACE::onScanCodeUnpressed(sc);
}

//---------------------------------------------------------------------------

void NAMESPACE::trackballOnMouseButtonPressed(uint x,uint y,uint button,uint flags)
{
  NAMESPACE::onMouseButtonPressed(x,y,button,flags); // Call before, in case 'freezeTrackball' is called

#ifdef USE_GLUT
  s_ShiftDown = glutGetModifiers() & GLUT_ACTIVE_SHIFT;
#endif
#ifndef WIN32
  if (!s_Freezed) s_Trackball.setForceZoom(s_ShiftDown); // SL: disabled for release as there remains a bug where this becomes sticky
#endif
  if (flags & LIBSL_BUTTON_DOWN) {
    if (!s_Freezed) s_Trackball.buttonPressed(button,x,y);
  } else if (flags & LIBSL_BUTTON_UP) {
    if (!s_Freezed) s_Trackball.buttonReleased(button);
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::trackballOnMouseMotion(uint x,uint y)
{
  NAMESPACE::onMouseMotion(x,y);                    // Call before, in case 'freezeTrackball' is called
  if (!s_Freezed) s_Trackball.update(x,y);
}

//---------------------------------------------------------------------------

void NAMESPACE::trackballOnMouseWheel(int incr)
{
  NAMESPACE::onMouseWheel(incr);
}

//---------------------------------------------------------------------------

void NAMESPACE::trackballOnReshape(uint w,uint h)
{
  s_Trackball.setWindowSize(w,h);
  NAMESPACE::onReshape(w,h);
}

//---------------------------------------------------------------------------

void NAMESPACE::setPathTensil(float t)
{
  s_PathTensil = t;
}

//---------------------------------------------------------------------------

static void preparePath()
{
  if (s_PathEyeAt[0].size() < 3) {
    ForIndex(p,2) {
      s_Path[p].clear();
    }
    return;
  }
  ForIndex(p,2) {
    s_Path[p].clear();
    v3f tgt;
    const float tensil = s_PathTensil;
    tgt = s_PathEyeAt[p][1] - s_PathEyeAt[p][0];
    s_Path[p].addControlPoint(
      s_PathEyeAt[p][0] - tgt*tensil,
      s_PathEyeAt[p][0],
      s_PathEyeAt[p][0] + tgt*tensil );
    ForRange(i,1,s_PathEyeAt[p].size()-2) {
      v3f tgt = (s_PathEyeAt[p][i+1] - s_PathEyeAt[p][i-1]);
      s_Path[p].addControlPoint(
        s_PathEyeAt[p][i] - tgt*tensil,
        s_PathEyeAt[p][i],
        s_PathEyeAt[p][i] + tgt*tensil );
    }
    tgt = s_PathEyeAt[p][s_PathEyeAt[p].size()-1] - s_PathEyeAt[p][s_PathEyeAt[p].size()-2];
    s_Path[p].addControlPoint(
      s_PathEyeAt[p][s_PathEyeAt[p].size()-1] - tgt*tensil,
      s_PathEyeAt[p][s_PathEyeAt[p].size()-1],
      s_PathEyeAt[p][s_PathEyeAt[p].size()-1] + tgt*tensil );
  }
  s_PathArcLen = s_Path[0].arclength();
  std::cerr << "[TrackballUI::Path] path arc length = " << s_PathArcLen << std::endl;
}

//---------------------------------------------------------------------------

static void resetPath()
{
  s_PathEyeAt[0].clear();
  s_PathEyeAt[1].clear();
  preparePath();
}

//---------------------------------------------------------------------------

static void addViewtoPath(const v3f& eye,const v3f& at)
{
  s_PathEyeAt[0].push_back(eye);
  s_PathEyeAt[1].push_back(at );
  preparePath();
}

//---------------------------------------------------------------------------

void NAMESPACE::addCurrentViewToPath()
{
  addViewtoPath( trackball().eye() , trackball().eye() + trackball().forward() );
}

//---------------------------------------------------------------------------

void NAMESPACE::playPath(bool on_off,float speed,int fixedTimeStep)
{
  s_PlayPath = on_off;
  if (s_PlayPath) {
    std::cerr << "[TrackballUI::Path] Start playing path." << std::endl;
    s_PlayPathPos       = 0;
    s_PlayPathSpeed     = speed;
    s_PathFixedTimeStep = fixedTimeStep;
  }
}

//---------------------------------------------------------------------------

bool NAMESPACE::reachedPathEnd()
{
  return !(s_PlayPathPos < s_PathArcLen);
}

//---------------------------------------------------------------------------

void NAMESPACE::changePathPlaySpeed(float speed)
{
  s_PlayPathSpeed = speed;
}

//---------------------------------------------------------------------------

void NAMESPACE::savePath(const char *fname)
{
  if (s_PathEyeAt[0].empty()) {
    return;
  }
  FILE *f;
  fopen_s(&f, fname, "wb");
  ForIndex(i,2) {
    uint n = (uint)s_PathEyeAt[i].size();
    fwrite(&n,sizeof(uint),1,f);
    fwrite(&(s_PathEyeAt[i][0][0]),sizeof(v3f),s_PathEyeAt[i].size(),f);
  }
  fclose(f);
}

//---------------------------------------------------------------------------

void NAMESPACE::loadPath(const char *fname,bool revert,bool startFromPreviousEnd)
{
  v3f prev0 = 0;
  v3f prev1 = 0;
  if (startFromPreviousEnd) {
    sl_assert( ! s_PathEyeAt[0].empty() );
    prev0 = s_PathEyeAt[0].back();
    prev1 = s_PathEyeAt[1].back();
  }
  FILE *f;
	fopen_s(&f, fname, "rb");
  ForIndex(i,2) {
    uint n = 0;
    fread(&n,sizeof(uint),1,f);
    s_PathEyeAt[i].resize(n);
    fread(&(s_PathEyeAt[i][0][0]),sizeof(v3f),s_PathEyeAt[i].size(),f);
  }
  fclose(f);
  if (revert) {
    reverse(s_PathEyeAt[0].begin(),s_PathEyeAt[0].end());
    reverse(s_PathEyeAt[1].begin(),s_PathEyeAt[1].end());
  }
  if (startFromPreviousEnd) {
    s_PathEyeAt[0].insert(s_PathEyeAt[0].begin(),prev0);
    s_PathEyeAt[1].insert(s_PathEyeAt[1].begin(),prev1);
  }
  preparePath();
}

//---------------------------------------------------------------------------
#ifdef OPENGL
void trackballUI_debug()
{
  static float arc = 0.0f;
   // draw path
  if (!s_PlayPath) {
    if (s_Path[0].isReady()) {
      // CORE-PROFILE 2018-03-20
      glLineWidth(/*3.0f*/1.0f);
      glDisable(GL_LIGHTING);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_ALPHA_TEST);
      glDisable(GL_BLEND);
      glColor3f(0,0,1);

      glBegin(GL_LINE_STRIP);
      ForIndex(i,100) {
        v3f p = s_Path[0].pointAt(float(i) / 100.0f);
        glVertex3fv(&p[0]);
      }
      glEnd();

      glColor3f(1,1,0);
      glBegin(GL_LINE_STRIP);
      ForIndex(i,100) {
        v3f p = s_Path[1].pointAt(float(i) / 100.0f);
        glVertex3fv(&p[0]);
      }
      glEnd();

      int N = 1024;
      glPointSize(5.0f);
      glColor3f(0,0,0);
      glBegin(GL_POINTS);
      ForIndex(i,N) {
        float a0 = float(i)   * s_PathArcLen / (float)N;
        float a1 = float(i+1) * s_PathArcLen / (float)N;
        float t0 = s_Path[0].arc2param(a0);
        float t1 = s_Path[0].arc2param(a1);
        v3f p0   = s_Path[0].pointAt(t0);
        v3f p1   = s_Path[0].pointAt(t1);
        glVertex3fv(&p0[0]);
      }
      glColor3f(1,0,0);
      arc += 0.1f;
      if (arc > s_PathArcLen) {
        arc = 0;
      }
      float t = s_Path[0].arc2param(arc);
      v3f e   = s_Path[0].pointAt(t);
      v3f l   = s_Path[1].pointAt(t);
      glVertex3fv(&e[0]);
      glVertex3fv(&l[0]);
      glEnd();
    }
  }
}
#endif

void NAMESPACE::trackballOnAnimate(double time,float elapsed)
{
  if (elapsed > 0) {
    // trackball
    s_Trackball.animate(elapsed);
    // camera path
    if (s_PlayPath) {
      if (s_PlayPathPos >= s_PathArcLen) {
        s_PlayPathPos = s_PathArcLen;
      } else {
        if (s_PathFixedTimeStep > -1) {
          s_PlayPathPos +=  s_PlayPathSpeed * s_PathFixedTimeStep / 1000.0f;
        } else {
          s_PlayPathPos +=  s_PlayPathSpeed * elapsed / 1000.0f;
        }
        s_PlayPathPos  = Math::min( s_PlayPathPos , s_PathArcLen );
      }
    }
  }
  NAMESPACE::onAnimate(time,elapsed);
}

//---------------------------------------------------------------------------

void NAMESPACE::trackballOnRender()
{
  NAMESPACE::onRender();
}

//---------------------------------------------------------------------------

void NAMESPACE::init(uint width,uint height,const char *title,char **argv,int argc,bool frameLess,bool fullScreen)
{
  // chain callbacks with SimpleUI
  SimpleUI::onRender             = trackballOnRender;
  SimpleUI::onKeyPressed         = trackballOnKeyPressed;
  SimpleUI::onKeyUnpressed       = trackballOnKeyUnpressed;
  SimpleUI::onScanCodePressed    = trackballOnScanCodePressed;
  SimpleUI::onScanCodeUnpressed  = trackballOnScanCodeUnpressed;
  SimpleUI::onMouseButtonPressed = trackballOnMouseButtonPressed;
  SimpleUI::onMouseMotion        = trackballOnMouseMotion;
  SimpleUI::onMouseWheel         = trackballOnMouseWheel;
  SimpleUI::onAnimate            = trackballOnAnimate;
  SimpleUI::onReshape            = trackballOnReshape;
  SimpleUI::init(width,height,title,argv,argc,frameLess,false,fullScreen);
}

#ifdef DIRECT3D
void NAMESPACE::init(HWND hwnd,LPDIRECT3DDEVICE9 device)
{
  SimpleUI::init(hwnd,device);
}
#endif

void NAMESPACE::loop()
{
  SimpleUI::loop();
}

void NAMESPACE::shutdown()
{
  SimpleUI::shutdown();
}

void NAMESPACE::exit()
{
  SimpleUI::exit();
}

uint NAMESPACE::screenWidth()
{
  return (SimpleUI::screenWidth());
}

uint NAMESPACE::screenHeight()
{
  return (SimpleUI::screenHeight());
}

//---------------------------------------------------------------------------

#ifdef WIN32

void NAMESPACE::setCustomCallbackMsgProc(WNDPROC proc)
{
  SimpleUI::setCustomCallbackMsgProc(proc);
}

#endif

//---------------------------------------------------------------------------
