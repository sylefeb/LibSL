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
// --------------------------------------------------------------

#include <iostream>
#include <ctime>
#include <cmath>

// --------------------------------------------------------------

#include <LibSL/LibSL_gl.h>

// --------------------------------------------------------------

using namespace std;

// --------------------------------------------------------------

LIBSL_WIN32_FIX

// --------------------------------------------------------------

#define SCREEN_W 1024
#define SCREEN_H 1024
#define FOV      float(M_PI/4.0)
#define ZNEAR    0.01f
#define ZFAR     100.0f

TriangleMesh_Ptr                                       g_Mesh;
AutoPtr<MeshRenderer<MeshFormat_3DS::t_VertexFormat> > g_Renderer;

AutoPtr<Manipulator> g_Manipulator;
m4x4f                g_Projection;

// --------------------------------------------------------------

void mainKeyboard(unsigned char key) 
{
  if        (key == 'q') {
    TrackballUI::exit();
  } else if (key == 'v') {
    cerr << TrackballUI::translation() << endl;
    cerr << TrackballUI::rotation()    << endl;
  } else if (key == 'l') {
    static bool swap = true;
    if (swap) {
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }
    swap = !swap;
  }
}

// --------------------------------------------------------------

void mainRender()
{
  glViewport(0,0,SCREEN_W,SCREEN_H);

  LibSL::GPUHelpers::clearScreen(LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER, 0,0,0);

  LibSL::GPUHelpers::Transform::set(LIBSL_PROJECTION_MATRIX, g_Projection);

  LibSL::GPUHelpers::Transform::set(LIBSL_MODELVIEW_MATRIX, TrackballUI::matrix() * g_Manipulator->toMatrix());
  g_Renderer   ->render();

  g_Manipulator->setViewParams(SCREEN_W,SCREEN_H,ZNEAR,ZFAR,g_Projection,TrackballUI::matrix());
  g_Manipulator->draw();
  
}

/* -------------------------------------------------------- */

void mainMouseButton(uint x,uint y,uint button,uint flags)
{
  g_Manipulator->mouseButton(x,y,button,flags);
  if (g_Manipulator->selected()) {
    TrackballUI::stopMotion();
  }
}

/* -------------------------------------------------------- */

void mainMouseMotion(uint x,uint y)
{
  g_Manipulator->mouseMotion(x,y);
}

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  try {

    if (argc < 2) {
      cerr << "Usage: TrackballUI <mesh file>\n" << endl;
      return (0);
    }

    /// Init TrackballUI
    cerr << "Init TrackballUI   ";
    TrackballUI::onRender             = mainRender;
    TrackballUI::onKeyPressed         = mainKeyboard;
    TrackballUI::onMouseButtonPressed = mainMouseButton;
    TrackballUI::onMouseMotion        = mainMouseMotion;
    TrackballUI::init(SCREEN_W,SCREEN_H);

    TrackballUI::trackball().translation() = V3F(0,0,-3.0f);

    cerr << "[OK]" << endl;

    /// help
    printf("[q]     - quit\n");

#ifdef OPENGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
#endif

    // load mesh
    cerr << "Loading mesh      ";
    g_Mesh     = loadTriangleMesh(argv[1]);
    cerr << "[OK]" << endl;
    cerr << "  mesh bbox : " << g_Mesh->bbox().minCorner() << " - " << g_Mesh->bbox().maxCorner() << endl;
    g_Mesh->scaleToUnitCube();
    g_Mesh->centerOn(v3f(0));
    cerr << sprint("  mesh contains %d vertices and %d triangles\n",g_Mesh->numVertices(),g_Mesh->numTriangles());

    cerr << "Creating renderer ";
    g_Renderer    = new MeshRenderer<MeshFormat_3DS::t_VertexFormat>(g_Mesh);
    cerr << "[OK]" << endl;

    g_Manipulator = new Manipulator();

    // setup view
    g_Projection = perspectiveMatrix(FOV,SCREEN_W/float(SCREEN_H),ZNEAR,ZFAR);

    /// main loop
    TrackballUI::loop();

    /// clean exit
    g_Mesh        = NULL;
    g_Renderer    = NULL;
    g_Manipulator = NULL;

    // shutdown SimpleUI
    TrackballUI::shutdown();

  } catch (Fatal& e) {
    cerr << e.message() << endl;
    return (-1);
  }

  return (0);
}

/* -------------------------------------------------------- */
