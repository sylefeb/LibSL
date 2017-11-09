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
// Triangle Mesh tutorial
// --------------------------------------------------------------

#include <iostream>
#include <ctime>
#include <cmath>

// --------------------------------------------------------------

#include <LibSL/LibSL.h>

#ifdef OPENGL
#include <LibSL/LibSL_gl.h>
#endif

#ifdef DIRECT3D
#include <LibSL/LibSL_d3d.h>
#endif

using namespace LibSL::Mesh;
using namespace LibSL;
// --------------------------------------------------------------

using namespace std;

// --------------------------------------------------------------

LIBSL_WIN32_FIX

// --------------------------------------------------------------

#define SCREEN_W 1024
#define SCREEN_H 768

TriangleMesh_Ptr                                       g_Mesh;
MeshRenderer<MeshFormat_3DS::t_VertexFormat>*  g_Renderer;

// --------------------------------------------------------------

void mainKeyboard(unsigned char key) 
{
  static float speed=1.0f;
  static char last=' ';

  if (key == 'q') {
    TrackballUI::exit();
  } else if (key == ' ') {
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

void mainAnimate(double time,float elapsed)
{
}

// --------------------------------------------------------------

void mainRender()
{
  /// render on screen

  GPUHelpers::clearScreen(LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER,
    0,0,1);

  GPUHelpers::Transform::set(LIBSL_MODELVIEW_MATRIX,TrackballUI::matrix());

  g_Renderer->render();
  
}

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  try {

    if (argc < 2) {
      cerr << "Usage: %s <mesh file>\n" << endl;
      return (0);
    }

    /// init TrackballUI UI (glut clone for both GL and D3D, with a trackball)
    cerr << "Init TrackballUI   ";
    TrackballUI::onRender     = mainRender;
    TrackballUI::onKeyPressed = mainKeyboard;
    TrackballUI::onAnimate    = mainAnimate;
    TrackballUI::init(SCREEN_W,SCREEN_H);
    TrackballUI::setCenter(V3F(.5f,.5f,.5f));
    cerr << "[OK]" << endl;

    /// help
    printf("[q]     - quit\n");

#ifdef DIRECT3D
    /// d3d init
    LPDIRECT3DDEVICE9 d3d=LIBSL_DIRECT3D_DEVICE;
    d3d->SetRenderState(D3DRS_LIGHTING,FALSE);
    d3d->SetRenderState(D3DRS_ZENABLE,TRUE);
    d3d->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
#endif

#ifdef OPENGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
#endif

    // open mesh
    cerr << "Loading mesh      ";
    g_Mesh     = TriangleMesh_Ptr(loadTriangleMesh(argv[1]));
    g_Mesh->scaleToUnitCube();
    cerr << "[OK]" << endl;
    cerr << sprint("  mesh contains %d vertices and %d triangles\n",g_Mesh->numVertices(),g_Mesh->numTriangles());

    cerr << "Creating renderer ";
    g_Renderer = new MeshRenderer<MeshFormat_3DS::t_VertexFormat>(g_Mesh.raw());
    cerr << "[OK]" << endl;

    // setup view
    Transform::perspective(LIBSL_PROJECTION_MATRIX,float(M_PI/4.0),SCREEN_W/float(SCREEN_H),0.001f,100.0f);

    // init trackball viewpoint
    TrackballUI::trackball().rotation()    = quatf(-0.0006f,-0.9992f,-0.0349f,0.0185f);
    TrackballUI::trackball().translation() = V3F  (-0.479f,-0.408f,-2.263f);

    /// main loop
    TrackballUI::loop();

    /// clean exit

    // shutdown UI
    TrackballUI::shutdown();

  } catch (Fatal& e) {
    cerr << e.message() << endl;
    return (-1);
  }

  return (0);
}

/* -------------------------------------------------------- */
