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
// Tutorial for GLHelpers
// --------------------------------------------------------------
#include <iostream>
#include <ctime>
#include <cmath>
/* -------------------------------------------------------- */

#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>

#include "GenSpiral.h"

#ifdef WIN32
LIBSL_WIN32_FIX; // necessary due to a VC 2005 compiler issue
#endif

using namespace std;

// associate render meshes and choosen vertex format

typedef GPUMesh_GL_VBO<GenSpiral_mvf>         SimpleMesh;

/* -------------------------------------------------------- */

SimpleMesh *g_Object = NULL;

/* -------------------------------------------------------- */

int          g_Width     = 640;
int          g_Height    = 480;
bool         g_Wireframe = false;

/* -------------------------------------------------------- */

Tex2DRGB_Ptr g_Tex;

/* -------------------------------------------------------- */

GLShader     g_glSimple;
GLParameter  g_glColor;
GLParameter  g_glTex;

/* -------------------------------------------------------- */

void mainKeyboard(uchar key) 
{
  if (key == 'q') {
    TrackballUI::exit();
  }
}

/* -------------------------------------------------------- */

void mainReshape(uint w,uint h)
{
  g_Width  = w;
  g_Height = h;
}

/* -------------------------------------------------------- */

void mainRender()
{
  // viewport
  GPUHelpers::Renderer::setViewport(0,0,g_Width,g_Height);

  // clear screen
  clearScreen(LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER,0.5,0.5,0.5);
	
  // transform
  Transform  ::perspective(LIBSL_PROJECTION_MATRIX,float(M_PI/4.0),g_Width/float(g_Height),0.1f,1000.0f);
  Transform  ::set(LIBSL_MODELVIEW_MATRIX,TrackballUI::matrix());
	
  g_glSimple.begin();
  g_Object->render();
  g_glSimple.end();
 
}

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  try {

    // init trackball
    TrackballUI::onKeyPressed = mainKeyboard;
    TrackballUI::onReshape    = mainReshape;
    TrackballUI::onRender     = mainRender;
    TrackballUI::init(g_Width,g_Height,"GLHelpers tutorial");
    
    // create objects
    g_Object = new SimpleMesh();
    
    // create geometry
    GenSpiral<SimpleMesh>::genSpiral(*g_Object,50);
    
    // help
    printf("[q]     - quit\n");
    
    // gl init
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    
    // setup light
    float spec[]={1.0,1.0,1.0,1.0};
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);
    float shin[]={20.0,20.0,20.0,20.0};
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shin);
    float f=1.0;
    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER,&f);
    
    // load a texture
    { // this block makes sure the image will be released
      ImageRGB_Ptr image=loadImage<ImageRGB>("media/image_test_rgb.png");
      g_Tex = new Tex2DRGB(image->pixels(),GPUTEX_AUTOGEN_MIPMAP);
    }

    // shader
    string vp_code=loadFileIntoString("tutorial0.vp");
    string fp_code=loadFileIntoString("tutorial0.fp");
    g_glSimple.init(vp_code.c_str(),fp_code.c_str(),"tutorial0"); 
    g_glSimple.setStrict(false);

    LIBSL_GL_CHECK_ERROR;

    g_glColor.init(g_glSimple,"color");
    g_glColor.set(1.0f,0.0f,0.5f,0.0f);
    g_glTex.init(g_glSimple,"tex");

    glActiveTextureARB(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,g_Tex->handle());  
    g_glTex.set(GLuint(0));

    LIBSL_GL_CHECK_ERROR;

    // let's go
    TrackballUI::loop();

    // shutdown
    TrackballUI::shutdown();

  } catch (GLException& e) {
    cerr << e.message() << endl;
    return (-1);
  } catch (Fatal& e) {
    cerr << e.message() << endl;
    return (-1);
  }

  return (0);
}

/* -------------------------------------------------------- */
