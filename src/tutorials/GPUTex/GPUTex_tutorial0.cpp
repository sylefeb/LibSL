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

/* -------------------------------------------------------- */

#include <LibSL/LibSL.h>
LIBSL_WIN32_FIX // required due to VisualC++ 2005 compiler issue

#ifdef OPENGL
#  include <LibSL/LibSL_gl.h>
#endif
#ifdef DIRECT3D
#  include <LibSL/LibSL_d3d.h>
#endif

/* -------------------------------------------------------- */

using namespace std;

/* -------------------------------------------------------- */

// Mesh definition for a simple textured quad

typedef GPUMESH_MVF2(mvf_vertex_3f,mvf_texcoord0_2f) mvf_quad;
typedef GPUMesh_VertexBuffer<mvf_quad>               QuadMesh;

QuadMesh *g_Quad = NULL;

/* -------------------------------------------------------- */

int     g_Width  = 512;
int     g_Height = 512;

/* -------------------------------------------------------- */

Tex2DRGBA_Ptr g_Tex;

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
  GPUHelpers::clearScreen(LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER,
    1,0,0);

  // draw quad (transforms have been setup at init)
  g_Quad->render();

}

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  try {
    
    /// init simple UI (glut clone for both GL and D3D)
    SimpleUI::onRender     = mainRender;
    SimpleUI::onKeyPressed = mainKeyboard;
    SimpleUI::init(g_Width,g_Height,"GPUTex tutorial");
    
    /// create a quad
    g_Quad = new QuadMesh();
    g_Quad->begin(GPUMESH_TRIANGLELIST);
    g_Quad->texcoord0_2(0.0f,1.0f);
    g_Quad->vertex_3(-1.0f,-1.0f,0);
    g_Quad->texcoord0_2(1.0f,1.0f);
    g_Quad->vertex_3(1.0f,-1.0f,0);
    g_Quad->texcoord0_2(1.0f,0.0f);
    g_Quad->vertex_3(1.0f,1.0f,0);
    g_Quad->texcoord0_2(0.0f,1.0f);
    g_Quad->vertex_3(-1.0f,-1.0f,0);
    g_Quad->texcoord0_2(1.0f,0.0f);
    g_Quad->vertex_3(1.0f,1.0f,0);
    g_Quad->texcoord0_2(0.0f,0.0f);
    g_Quad->vertex_3(-1.0f,1.0f,0);
    g_Quad->end();

    /// help
    printf("[q]     - quit\n");
    
    /// load a texture
    {
      ImageRGBA_Ptr image = loadImage<ImageRGBA>("media/image_test_rgba.png");
      g_Tex               = new Tex2DRGBA(image->pixels(),GPUTEX_AUTOGEN_MIPMAP);
    }

#ifdef OPENGL
    /// gl init
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,g_Tex->handle());
#endif

#ifdef DIRECT3D
    /// d3d init
    LPDIRECT3DDEVICE9 d3d=LIBSL_DIRECT3D_DEVICE;
    d3d->SetTexture(0,g_Tex->handle());
    d3d->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
    d3d->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
    d3d->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
    d3d->SetRenderState(D3DRS_LIGHTING,FALSE);
    d3d->SetRenderState(D3DRS_ZENABLE,FALSE);
    d3d->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
#endif

    Transform::ortho2D(LIBSL_PROJECTION_MATRIX,-1,1,-1,1);
    Transform::identity(LIBSL_MODELVIEW_MATRIX);

    /// main loop
    SimpleUI::loop();

    /// clean exit
#ifdef DIRECT3D
    // unbind texture from d3d
    d3d->SetTexture(0,NULL); 
#endif
    
    // erase autopointer (will free ressources)
    g_Tex = NULL; 
    
    // shutdown SimpleUI
    SimpleUI::shutdown();

  } catch (Fatal& e) {
    cerr << e.message() << endl;
    return (-1);
  }
  
  return (0);
}

/* -------------------------------------------------------- */
