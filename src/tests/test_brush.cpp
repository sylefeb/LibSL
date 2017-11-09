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

static vector<AutoPtr<Shapes::Polygon> > g_Polygons;

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
    0,0,1);

  GPUHelpers::Transform::set(LIBSL_MODELVIEW_MATRIX,TrackballUI::matrix()*translationMatrix(V3F(0,0,-1)));

  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);

  glColor3f    (0,0,0);
  glDisable    (GL_CULL_FACE);
  glPolygonMode(GL_FRONT,GL_FILL);
  glPolygonMode(GL_BACK ,GL_LINE);
  ForIndex(p,g_Polygons.size()) {
    g_Polygons[p]->render();
  }
  
  glColor3f   (1,1,1);
  glEnable    (GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  ForIndex(p,g_Polygons.size()) {
    g_Polygons[p]->render();
  }

}

// --------------------------------------------------------

void test_brush()
{
  try {

    /// init simple UI (glut clone for both GL and D3D)
    TrackballUI::onRender             = mainRender;
    TrackballUI::onKeyPressed         = mainKeyboard;
    TrackballUI::onMouseButtonPressed = mainOnMouseButtonPressed;
    TrackballUI::onMouseMotion        = mainOnMouseMotion;
    TrackballUI::init(g_WinWidth,g_WinHeight);

    /// help
    printf("[q]     - quit\n");

    Transform::perspective(LIBSL_PROJECTION_MATRIX,3.14f/2.0f,1,0.1f,100.0f);
    Transform::identity   (LIBSL_MODELVIEW_MATRIX);

    Geometry::Brush b;
    b.addPlaneCCW(Plane<3>(V3F(0,0,0),V3F(0,-1,0)));
    b.addPlaneCCW(Plane<3>(V3F(0,0,0),V3F(-1,0,0)));
    b.addPlaneCCW(Plane<3>(V3F(0,0,0),V3F(0,0,-1)));
    b.addPlaneCCW(Plane<3>(V3F(1,1,1),V3F(0,1,0)));
    b.addPlaneCCW(Plane<3>(V3F(1,1,1),V3F(1,0,0)));
    b.addPlaneCCW(Plane<3>(V3F(1,1,1),V3F(0,0,1)));
    b.addPlaneCCW(Plane<3>(V3F(0.85,0.85,0.85),normalize(V3F(1,1,1))));

    ForIndex(i,b.polygons().size()) {
      AutoPtr<Shapes::Polygon> poly = new Shapes::Polygon(b.polygons()[i]);
      g_Polygons.push_back(poly);
    }

    /// main loop
    TrackballUI::loop();

    // shutdown SimpleUI
    TrackballUI::shutdown();

  } catch (Fatal& e) {
    cerr << e.message() << endl;
  }

}

/* -------------------------------------------------------- */
