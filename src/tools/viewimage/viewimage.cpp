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
#include <cstring>

/* -------------------------------------------------------- */

#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>

using namespace LibSL;

LIBSL_WIN32_FIX;

#ifdef WIN32
#include <windows.h>
#endif

/* -------------------------------------------------------- */
using namespace std;
/* -------------------------------------------------------- */

int       g_Width  = 1024;
int       g_Height = 1024;

// Mesh definition for a simple textured quad

typedef GPUMESH_MVF2(mvf_vertex_3f,mvf_texcoord0_2f) mvf_quad;
typedef GPUMesh_VertexBuffer<mvf_quad>               MeshQuad;

AutoPtr<MeshQuad> g_Quad;
Tex2DRGBA_Ptr     g_Tex;

/* -------------------------------------------------------- */

void mainKeyboard(unsigned char key) 
{
  if (key == 'q') {
    SimpleUI::exit();
  }
}

/* -------------------------------------------------------- */
 
void mainRender()
{
  // clear screen
  GPUHelpers::clearScreen(
    LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER,
    1.0,0.0,1.0);

  GPUHelpers::Transform::ortho2D (LIBSL_PROJECTION_MATRIX,-1,1,-1,1);
  GPUHelpers::Transform::identity(LIBSL_MODELVIEW_MATRIX);

  glEnable (GL_TEXTURE_2D);
  glColor4f(1,1,1,1);

  g_Tex ->bind();
  g_Quad->render();

}

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  bool batch_mode    = false;
  bool tonemap       = false;
  float compensation = 0;

  try {

    sl_assert(argc > 1);

    // load image
    ImageRGBA_Ptr img;
    try {
      img = ImageRGBA_Ptr(loadImage<ImageRGBA>(argv[1]));
    } catch (Fatal& e) {
      cerr << "[1] " << e.message() << endl;
      try {
        img = ImageRGBA_Ptr(loadImage<ImageRGB>(argv[1])->cast<ImageRGBA>());
      } catch (Fatal& e) {
        cerr << "[2] " << e.message() << endl;
        throw Fatal("Cannot load image '%s'",argv[1]);
      }
    }

    // init UI
    SimpleUI::onRender     = mainRender;
    SimpleUI::onKeyPressed = mainKeyboard;
    SimpleUI::init(img->w(),img->h(),argv[1]);

    // create texture
    g_Tex = Tex2DRGBA_Ptr(new Tex2DRGBA(img->pixels()));

    // help
    cerr << "[q]     quit\n";
    
    // create a quad
    g_Quad = AutoPtr<MeshQuad>(new MeshQuad());
    g_Quad->begin(GPUMESH_TRIANGLELIST);
    g_Quad->texcoord0_2(1.0f,1.0f);
    g_Quad->vertex_3(1.0f,-1.0f,0);
    g_Quad->texcoord0_2(0.0f,1.0f);
    g_Quad->vertex_3(-1.0f,-1.0f,0);
    g_Quad->texcoord0_2(1.0f,0.0f);
    g_Quad->vertex_3(1.0f,1.0f,0);    
    g_Quad->texcoord0_2(1.0f,0.0f);
    g_Quad->vertex_3(1.0f,1.0f,0);
    g_Quad->texcoord0_2(0.0f,1.0f);
    g_Quad->vertex_3(-1.0f,-1.0f,0);
    g_Quad->texcoord0_2(0.0f,0.0f);
    g_Quad->vertex_3(-1.0f,1.0f,0);
    g_Quad->end();

    // inner loop
    SimpleUI::loop();

    // free ressources
    g_Quad = AutoPtr<MeshQuad>();
    g_Tex  = Tex2DRGBA_Ptr();
    SimpleUI::shutdown();

  } catch (Fatal& e) {
#ifdef WIN32
    MessageBoxA(NULL,e.message(),"Error",MB_OK | MB_ICONERROR);
#endif
    cerr << e.message() << endl;
    return (-1);
  }

  return (0);
}

/* -------------------------------------------------------- */
