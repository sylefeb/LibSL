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
// Tutorial for GPUTex
// --------------------------------------------------------------

#include <iostream>
#include <ctime>
#include <cmath>
#include <cstring>

/* -------------------------------------------------------- */

#include <LibSL/LibSL.h>

#ifdef OPENGL
#include <LibSL/LibSL_gl.h>
#endif
#ifdef OPENGL4
#include <LibSL/LibSL_gl.h>
#endif
#ifdef DIRECT3D
#include <LibSL/LibSL_d3d.h>
#endif

#include <imgui.h>

/* -------------------------------------------------------- */

LIBSL_WIN32_FIX

using namespace std;
using namespace LibSL;
using namespace LibSL::GPUHelpers;
using namespace LibSL::GLHelpers;

/* -------------------------------------------------------- */

int           g_Width  = 512;
int           g_Height = 512;

/* -------------------------------------------------------- */

void mainKeyboard(unsigned char key) 
{
  if (key == 'q')
    exit (0);
}

/* -------------------------------------------------------- */

ImVec4 clear_color = ImColor(114, 144, 154);

void mainRender()
{
  // clear screen
  GPUHelpers::clearScreen(LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER,
    clear_color.x, clear_color.y, clear_color.z);

  Transform::ortho2D (LIBSL_PROJECTION_MATRIX,0,1,0,1);
  Transform::identity(LIBSL_MODELVIEW_MATRIX);

  static float f = 0.0f;
  ImGui::Text("Hello, world!");
  ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

  ImGui::ColorEdit3("clear color", (float*)&clear_color);

  ImGui::Render();
}

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  try {

    /// init simple UI (glut clone for both GL and D3D)
    SimpleUI::onRender     = mainRender;
    SimpleUI::onKeyPressed = mainKeyboard;
    
    SimpleUI::bindImGui();
    SimpleUI::init(g_Width, g_Height);
    SimpleUI::initImGui();

#ifdef OPENGL
#ifndef EMSCRIPTEN
    /// gl init
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
#endif
    //glEnable(GL_DEPTH_TEST);
#endif

    Transform::ortho2D (LIBSL_PROJECTION_MATRIX,-1,1,-1,1);
    Transform::identity(LIBSL_MODELVIEW_MATRIX);

    /// main loop
    SimpleUI::loop();

    // shutdown SimpleUI
    SimpleUI::terminateImGui();
    SimpleUI::shutdown();

    
  } catch (Fatal& e) {
    cerr << e.message() << endl;
    return (-1);
  }
  
  return (0);
}

/* -------------------------------------------------------- */
