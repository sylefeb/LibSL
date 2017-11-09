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
#include "precompiled.h"
#include <LibSL/LibSL_gl.h>

// --------------------------------------------------------

#include <iostream>
#include <ctime>

using namespace std;

static int        g_WinWidth  = 512;
static int        g_WinHeight = 512;

static int        g_MouseX;
static int        g_MouseY;

v2f               g_P;

BezierCurve<v2f>  g_Curve;

// --------------------------------------------------------

static void mainKeyboard(unsigned char key) 
{
  if (key == 'q') {
    SimpleUI::exit();
  } else if (key == ' ') {
    g_Curve.computeArcLength();
  }
}

// --------------------------------------------------------

static void mainOnMouseButtonPressed(uint x,uint y,uint btn,uint flags)
{
  g_MouseX = x;
  g_MouseY = y;
  v2f p    = V2F(x/float(g_WinWidth),1.0f-y/float(g_WinHeight));

  if (flags == LIBSL_BUTTON_DOWN) {
    static bool first = true;
    if (first) {
      g_P   = p;
      first = false;
    }
    g_Curve.addControlPoint(p-(p-g_P)*0.5f,p,p+(p-g_P)*0.5f);
    // g_Curve.addControlPoint(p);
  }

  g_P      = p;
}

// --------------------------------------------------------

static void mainOnMouseMotion(uint x,uint y)
{
  g_MouseX = x;
  g_MouseY = y;
}

// --------------------------------------------------------

static void mainRender()
{
  // clear screen
  GPUHelpers::clearScreen(LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER,
    0,0,0);

  if (g_Curve.isReady()) {

    Array<v2f> curve;
    g_Curve.tesselate(100, curve);

    glBegin(GL_LINE_STRIP);
    ForArray(curve,n) {
      glColor3d(n/float(curve.size()),1.0-n/float(curve.size()),0.0);
      v2f p = curve[n];
      glVertex2f(p[0],p[1]);
    }
    glEnd();

    glPointSize(2.0);
    glBegin(GL_POINTS);
    glColor3d(1.0,1.0,1.0);
    ForArray(curve,n) {
      v2f p = curve[n];
      glVertex2f(p[0],p[1]);
    }
    glEnd();
    glPointSize(1.0);

  }

  // render Bezier quadratic segment

  int sz = 20;

  glBegin(GL_LINE_STRIP);
  for (int i=0;i<sz;i++) {
    glColor3d(i/float(sz),1.0-i/float(sz),0.0);
    v2f p = Bezier::bezierSegmentQuadratic(i/float(sz-1),V2F(0,0),V2F(0.5,0.5),V2F(1.0,0.3));
    glVertex2f(p[0],p[1]);
  }
  glEnd();

  // render Bezier cubic segment

  glBegin(GL_LINE_STRIP);
  for (int i=0;i<sz;i++) {
    glColor3d(i/float(sz),1.0-i/float(sz),0.0);
    v2f p = Bezier::bezierSegmentCubic(i/float(sz-1),V2F(0,0),V2F(0.25,0.4),V2F(0.75,0.1),V2F(1.0,0.7));
    glVertex2f(p[0],p[1]);
  }
  glEnd();
}

// --------------------------------------------------------

void test_hermitcurve()
{
  try {
    
    /// init simple UI (glut clone for both GL and D3D)
    SimpleUI::onRender             = mainRender;
    SimpleUI::onKeyPressed         = mainKeyboard;
    SimpleUI::onMouseButtonPressed = mainOnMouseButtonPressed;
    SimpleUI::onMouseMotion        = mainOnMouseMotion;
    SimpleUI::init(g_WinWidth,g_WinHeight);
    
    /// help
    printf("[q]     - quit\n");
    
    /// gl init
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    Transform::ortho2D(LIBSL_PROJECTION_MATRIX,0,1,0,1);
    Transform::identity(LIBSL_MODELVIEW_MATRIX);

    /// main loop
    SimpleUI::loop();

    // shutdown SimpleUI
    SimpleUI::shutdown();
  
  } catch (Fatal& e) {
    cerr << e.message() << endl;
  }

}

/* -------------------------------------------------------- */
