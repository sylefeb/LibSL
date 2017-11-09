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

#include <iostream>
#include <ctime>
#include <vector>

using namespace std;

static int g_WinWidth =512;
static int g_WinHeight=512;

typedef Geometry::Polygon<3,Tuple<float,6> > t_Polygon;

static vector<t_Polygon> g_Polygons;

static v3f      g_P0=0,g_P1=0;

// --------------------------------------------------------

static void mainKeyboard(unsigned char key) 
{
  if (key == 'q') {
    SimpleUI::exit();
  }
}

// --------------------------------------------------------

static void mainOnMouseButtonPressed(uint x,uint y,uint btn,uint flags)
{
  v3f p = V3F(x/float(g_WinWidth),1.0f-y/float(g_WinHeight),0);
  if (btn == LIBSL_LEFT_BUTTON  && flags == LIBSL_BUTTON_DOWN) {
    g_P0   = p;
  }
  if (btn == LIBSL_RIGHT_BUTTON && flags == LIBSL_BUTTON_DOWN) {
    g_P1   = p;
  }
}

// --------------------------------------------------------

static void mainOnMouseMotion(uint x,uint y)
{

}

// --------------------------------------------------------

static void mainRender()
{
  // clear screen
  GPUHelpers::clearScreen(LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER,
    0,0,0);

  v3f m       = tupleMin(g_P0,g_P1);
  v3f M       = tupleMax(g_P0,g_P1);
  AABox box   = AABox(m,M);
  
  ForIndex(p,g_Polygons.size()) {
    if (Intersections::Polygon_AABox(g_Polygons[p],box)) {
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glBegin(GL_POLYGON);
      ForIndex(k,g_Polygons[p].size()) {
        glColor3fv (&(g_Polygons[p].vertexAt(k)[3]));
        glVertex3fv(&(g_Polygons[p].vertexAt(k)[0]));
      }
      glEnd();
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glColor3f(0,0,0);
      glBegin(GL_POLYGON);
      ForIndex(k,g_Polygons[p].size()) {
        glVertex3fv(&(g_Polygons[p].vertexAt(k)[0]));
      }
      glEnd();
    } else {
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glBegin(GL_POLYGON);
      ForIndex(k,g_Polygons[p].size()) {
        glColor3fv (&(g_Polygons[p].vertexAt(k)[3]));
        glVertex3fv(&(g_Polygons[p].vertexAt(k)[0]));
      }
      glEnd();
    }
  }

  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glBegin(GL_QUADS);
  glColor3f(1,1,1);
  glVertex2f(m[0],m[1]);
  glVertex2f(m[0],M[1]);
  glVertex2f(M[0],M[1]);
  glVertex2f(M[0],m[1]);
  glEnd();
}

// --------------------------------------------------------

void test_polygon()
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

    Tuple<float,6> pts[4];
#define SET_P(i, x,y,z, r,g,b) pts[i][0]=x; pts[i][1]=y; pts[i][2]=z; pts[i][3]=r; pts[i][4]=g; pts[i][5]=b;
    SET_P(0, 0,0,0, 1,1,1);
    SET_P(1, 0,1,0, 1,0,0);
    SET_P(2, 1,1,0, 0,1,0);
    SET_P(3, 1,0,0, 0,0,1);

    t_Polygon poly;
    ForIndex(n,4) {
      poly.add(pts[n]);
    }
    g_Polygons.push_back(poly);

    // random cuts
    ForIndex(n,1000) {
      // random dir
      v3f dir = V3F((rand()&255)/255.0f-0.5f,(rand()&255)/255.0f-0.5f,0);
      dir = normalize(dir);
      // select a polygon
      uint r = uint(rand() % g_Polygons.size());
      v3f pos = Tuple<float,3>(g_Polygons[r].center());
      Plane<3> pl(pos,dir);
      t_Polygon fr,bk;
      if (g_Polygons[r].cut(pl,fr,bk) == LIBSL_POLYGON_CUT) {
        g_Polygons[r]=fr;
        g_Polygons.push_back(bk);
      }
    }

    /// main loop
    SimpleUI::loop();

    // shutdown SimpleUI
    SimpleUI::shutdown();

  } catch (Fatal& e) {
    cerr << e.message() << endl;
  }

}

/* -------------------------------------------------------- */
