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
LIBSL_WIN32_FIX // required due to VisualC++ 2005 compiler issue

#include <LibSL/LibSL_gl.h>

#include "GenSpiral.h"

using namespace std;

// --------------------------------------------------------------

// define our vertex format

typedef GPUMESH_MVF3(mvf_vertex_3f,mvf_normal_3f,mvf_color0_rgba) mvf_simple;

// associate render meshes and choosen vertex format

typedef GPUMesh_GL_VBO<mvf_simple>         SimpleMesh_VBO;

typedef GPUMesh_GL_DisplayList<mvf_simple> SimpleMesh_DisplayList;

typedef GPUMesh_GL_Calls<mvf_simple>       SimpleMesh_Calls;

// --------------------------------------------------------------

// pointers for the three different types of objects

SimpleMesh_VBO         *g_Object_VBO         = NULL;
SimpleMesh_DisplayList *g_Object_DisplayList = NULL;
SimpleMesh_Calls       *g_Object_Calls       = NULL;

// font

Font                   *g_Font               = NULL;

// -------------------------------------------------------- 

int     g_Width      = 640;
int     g_Height     = 480;
int     g_RenderMode = 0;

// -------------------------------------------------------- 

void mainKeyboard(uchar key) 
{
  if (key == 'q') {
    TrackballUI::exit();
  } else if (key == ' ') {
    g_RenderMode = (g_RenderMode+1) % 3;
  }
}

// -------------------------------------------------------- 

void mainRender()
{
  glViewport(0,0,g_Width,g_Height);

  // clear screen
  glClearColor(0.5,0.5,0.5,0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // transform
  GPUHelpers::Transform::perspective(LIBSL_PROJECTION_MATRIX,M_PI/4.0,g_Width/float(g_Height),0.1f,100.0f);
  GPUHelpers::Transform::set        (LIBSL_MODELVIEW_MATRIX,TrackballUI::matrix());

  // draw selected object
  switch (g_RenderMode)
  {
  case 0:
    g_Object_VBO        ->render();
    break;
  case 1:
    g_Object_DisplayList->render();
    break;
  case 2:
    g_Object_Calls      ->render();
    break;
  }

  // fps
  static float fps   = 0;
  static uint  count = 0;
  static uint last   = milliseconds();
  uint        now    = milliseconds();

  if (now - last > 1000) {
    fps   = count * 1000.0f / float(now - last);
    last  = now;
    count = 0;
  }
  GPUHelpers::Transform::ortho2D (LIBSL_PROJECTION_MATRIX,0,1,0,1);
  GPUHelpers::Transform::identity(LIBSL_MODELVIEW_MATRIX);
  
  glColor4f(1,1,1,1);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  
  g_Font->printString(0,0,0.03f,sprint("%.1f FPS",fps));
  switch (g_RenderMode)
  {
  case 0:
    g_Font->printString(0,1-0.04f,0.03f,"VBO");
    break;
  case 1:
    g_Font->printString(0,1-0.04f,0.03f,"Display list");
    break;
  case 2:
    g_Font->printString(0,1-0.04f,0.03f,"GL calls");
    break;
  }

  glDisable(GL_BLEND);

  count ++;
}

// -------------------------------------------------------- 

int main(int argc, char **argv) 
{

  try {

    // create viewport

    TrackballUI::onRender     = mainRender;
    TrackballUI::onKeyPressed = mainKeyboard;
    TrackballUI::init(g_Width,g_Height,"GPUMesh tutorial");

    // create objects

    g_Object_VBO         = new SimpleMesh_VBO();
    g_Object_DisplayList = new SimpleMesh_DisplayList();
    g_Object_Calls       = new SimpleMesh_Calls();

    // create geometry
    //   the geomtry producer (CGenSpiral) can be used with the three 
    //   types of meshes (and is also compatible with DirectX - see 
    //   rendermesh_d3d).
    GenSpiral<SimpleMesh_VBO>        ::genSpiral(*g_Object_VBO        ,50);
    GenSpiral<SimpleMesh_DisplayList>::genSpiral(*g_Object_DisplayList,50);
    GenSpiral<SimpleMesh_Calls>      ::genSpiral(*g_Object_Calls      ,50);

    // load font
    g_Font = new Font("media\\basefont.png");

    // help
    printf("[q]     - quit\n");
    printf("[SPACE] - next rendering method\n");

    // gl init
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    // setup light
    float spec[] = {1.0f,1.0f,1.0f,1.0f};
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);
    float shin[] = {20.0f,20.0f,20.0f,20.0f};
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shin);
    float f      = 1.0f;
    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER,&f);

    // render loop
    SimpleUI::loop();

    // clean up
    delete (g_Object_VBO);
    delete (g_Object_DisplayList);
    delete (g_Object_Calls);

    // close window
    SimpleUI::shutdown();

  } catch (Fatal& f) {
    std::cerr << f.message() << std::endl;
  }

  return (0);
}
/* -------------------------------------------------------- */
