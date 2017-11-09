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
// Tutorial for Nature::Sky
// --------------------------------------------------------------

#include <iostream>
#include <ctime>
#include <cmath>

#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>

using namespace std;

/* -------------------------------------------------------- */

int     g_iMainWindow;
int     g_iWinWidth =640;
int     g_iWinHeight=480;

float   g_Theta = -1.079922f;
float   g_Phy   =  0.196350f;

/* -------------------------------------------------------- */

CgShader    g_cgSimple;
Sky         g_Sky;

/* -------------------------------------------------------- */

void mainKeyboard(unsigned char key) 
{
  if (key == 'q')
    SimpleUI::exit();
  else if (key == 'g')
    g_Theta+=M_PI/32.0;
  else if (key == 'j')
    g_Theta-=M_PI/32.0;
  else if (key == 'y')
    g_Phy+=M_PI/32.0;
  else if (key == 'h')
    g_Phy-=M_PI/32.0;
}

/* -------------------------------------------------------- */

void mainRender()
{
  glViewport(0,0,g_iWinWidth,g_iWinHeight);
	
  // clear screen
  glClearColor(0.5,0.5,0.5,0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
  // transform
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(70.0,g_iWinWidth/(double)g_iWinHeight,0.1,1000.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0,0,0,
	    0,1,0,
	    0,0,1);

  v3f camera=V3F(0,0,0);
  v3f sundir=V3F(cos(g_Theta)*cos(g_Phy),sin(g_Theta)*cos(g_Phy),-sin(g_Phy));

  g_Sky.drawSky(camera,sundir);
 
}

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  try {

    SimpleUI::onRender     = mainRender;
    SimpleUI::onKeyPressed = mainKeyboard;

    SimpleUI::init(g_iWinWidth,g_iWinHeight);

    // gluX
    gluxInit();
    
    // help
    printf("[q]     - quit\n");
    printf("[y,h,g,j] - move sun\n");
    
    // shader
    string code;
    // add sky code to shader code
    g_Sky.addTo(code);
    // init shader
    g_cgSimple.init(code.c_str());
    // bind sky to this shader
    g_Sky.bindTo(g_cgSimple);
    
    SimpleUI::loop();

    SimpleUI::shutdown();

  } catch (CgException& e) {
    cerr << e.message() << endl;
    return (-1);
  } catch (Fatal& e) {
    cerr << e.message() << endl;
    return (-1);
  }

  return (0);
}

/* -------------------------------------------------------- */
