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
#include <AntTweakBar.h>

LIBSL_WIN32_FIX; // necessary due to a VC 2005 compiler issue

/* -------------------------------------------------------- */

#include "shader_tutorial2.h"

AutoBindShader::shader_tutorial2 shader;

/* -------------------------------------------------------- */

using namespace std;

/* -------------------------------------------------------- */

int          g_Width     = 640;
int          g_Height    = 480;

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
  Transform  ::ortho2D (LIBSL_PROJECTION_MATRIX,0,1,0,1);
  Transform  ::identity(LIBSL_MODELVIEW_MATRIX);
  
  // draw
  // -> activate shader
  shader.begin();
  // -> set uniform color
  shader.u_Color.set( V4F(1,0.5,0,0) );
  // -> make a quad
  glBegin(GL_QUADS);
  glVertex2i(0,0);
  glVertex2i(1,0);
  glVertex2i(1,1);
  glVertex2i(0,1);
  glEnd(); 
  // -> deactivate shader
  shader.end();

  TwDraw();
}

/* -------------------------------------------------------- */

LRESULT CALLBACK mainWin32Event(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  return TwEventWin32(hWnd, uMsg, wParam, lParam); // send event message to AntTweakBar
}

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  try {

    // init Trackball
    TrackballUI::onKeyPressed = mainKeyboard;
    TrackballUI::onReshape    = mainReshape;
    TrackballUI::onRender     = mainRender;
    TrackballUI::init(g_Width,g_Height,"GLHelpers tutorial");
    
    // init shader
    shader.init();

    // init AntTweakBar
    if( ! TwInit(TW_OPENGL,NULL) ) {
      throw Fatal("Cannot initialize AntTweakBar!");
    }
    TrackballUI::setCustomCallbackMsgProc(mainWin32Event);
    TwWindowSize(g_Width,g_Height);

    shader.makeTwBar();
    
    // help
    printf("[q]     - quit\n");

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
