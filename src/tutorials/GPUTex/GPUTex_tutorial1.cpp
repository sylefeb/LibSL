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

#include <LibSL/LibSL_gl.h>

/* -------------------------------------------------------- */

using namespace std;

/* -------------------------------------------------------- */

int                   g_Width  = 512;
int                   g_Height = 512;

ImageRGBA_Ptr         g_Image;
Tex2DRGBA_Ptr         g_Tex;
RenderTarget2DRGB_Ptr g_RenderTarget;

/* -------------------------------------------------------- */

void mainKeyboard(uchar key) 
{
  if (key == 'q') {
    SimpleUI::exit();
  } else if (key == ' ') {
    ImageRGB_Ptr img = new ImageRGB();
    g_RenderTarget->readBack(img->pixels());
    saveImage("rt.png",img);
  }
}

/* -------------------------------------------------------- */

void glQuad()
{
  glBegin(GL_QUADS);

  glTexCoord2f(0,1);
  glVertex2f(0,0);

  glTexCoord2f(0,0);
  glVertex2f(0,1);

  glTexCoord2f(1,0);
  glVertex2f(1,1);

  glTexCoord2f(1,1);
  glVertex2f(1,0);
  
  glEnd();
}

/* -------------------------------------------------------- */

void mainRender()
{
  // draw into render target

  g_RenderTarget->bind();
 
  glViewport(0,0,g_RenderTarget->w(),g_RenderTarget->h());
  
  glClearColor(0.2,0.2,0.5,0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0,1,0,1);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glColor3f(1,1,1);
  static float agl = 0.0f;
  agl += 0.1f;
  glTranslatef(0.5f,0.5f,0.0f);
  glRotatef(agl,0,0,1);
  glTranslatef(-0.25f,-0.25f,0.0f);
  glScalef(0.5f,0.5f,1.0f);
  glBindTexture(GL_TEXTURE_2D,g_Tex->texture());
  glQuad();
  
  g_RenderTarget->unbind();
  
  // draw to screen

  // clear screen
  
  glViewport(0,0,g_Width,g_Height);
  
  glClearColor(0.5,0.5,0.5,0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glLoadIdentity();
  glBindTexture(GL_TEXTURE_2D,g_RenderTarget->texture());
  glTranslatef(0.05f,0.05f,0.0f);
  glScalef(0.9f,0.9f,1.0f);
  glQuad();
  
}

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  try {

    /// init simple UI (glut clone for both GL and D3D)
    SimpleUI::onRender     = mainRender;
    SimpleUI::onKeyPressed = mainKeyboard;
    SimpleUI::init(g_Width,g_Height,"GPUTex tutorial");

    // help
    printf("[q]     - quit\n");
    printf("[SPACE] - save render target as 'rt.png'\n");
    
    // gl init
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_NORMALIZE);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);
    
    // render target
    g_RenderTarget = new RenderTarget2DRGB(128,128);

    // load a texture
    g_Image = loadImage<ImageRGBA>("media/image_test_rgba.png");
    g_Tex   = new Tex2DRGBA(g_Image->pixels(),GPUTEX_AUTOGEN_MIPMAP);
    glBindTexture(GL_TEXTURE_2D,g_Tex->handle());
    
    /// main loop
    SimpleUI::loop();

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
