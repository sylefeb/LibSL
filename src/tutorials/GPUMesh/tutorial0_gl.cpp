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
// LibSL GPUMesh tutorial
// --------------------------------------------------------------

#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>

// -------------------------------------------------------- 

// define our vertex format

typedef GPUMESH_MVF2(mvf_vertex_2f,mvf_color0_rgba) mvf_mesh;

// associate render mesh and choosen vertex format

typedef GPUMesh_GL_DisplayList<mvf_mesh> SimpleMesh;

SimpleMesh         *g_Object = NULL;

// -------------------------------------------------------- 

int     g_Width  = 640;
int     g_Height = 480;

// -------------------------------------------------------- 

void mainKeyboard(uchar key) 
{
  if (key == 'q') {
    SimpleUI::exit();
  }
}

// -------------------------------------------------------- 

void mainReshape(uint w,uint h)
{
  g_Width  = w;
  g_Height = h;
}

// -------------------------------------------------------- 

void mainRender()
{
  glViewport(0,0,g_Width,g_Height);
	
  // clear screen
  glClearColor(0.5,0.5,0.5,0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0,1.0,1.0,0.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // render object
  g_Object->render();

}

// -------------------------------------------------------- 

int main(int argc, char **argv) 
{
  // create viewport

  SimpleUI::onRender     = mainRender;
  SimpleUI::onKeyPressed = mainKeyboard;
  SimpleUI::init(g_Width,g_Height,"GPUMesh tutorial");

  // create objects

  g_Object = new SimpleMesh();

  g_Object->begin(GPUMESH_TRIANGLELIST);

  g_Object->color0_4(0,0,255,0);
  g_Object->vertex_2(0.1,0.1);

  g_Object->color0_4(255,0,0,0);
  g_Object->vertex_2(0.1,0.9);

  g_Object->color0_4(0,255,0,0);
  g_Object->vertex_2(0.9,0.9);

  g_Object->color0_4(0,0,255,0);
  g_Object->vertex_2(0.1,0.1);

  g_Object->color0_4(0,255,0,0);
  g_Object->vertex_2(0.9,0.9);

  g_Object->color0_4(64,0,255,0);
  g_Object->vertex_2(0.9,0.1);

  g_Object->end();

  // help
  printf("[q]     - quit\n");
	
  // render loop
  SimpleUI::loop();

  // clean up
  delete (g_Object);

  // close window
  SimpleUI::shutdown();

  return (0);
}

// -------------------------------------------------------- 

