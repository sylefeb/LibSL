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
//
// OpenGL tutorial 2 of the GPUmesh class (indexed meshes)
//
// Dependencies
//
//    * glut
//    * gluX
//    * libTexture
//
// (c) Sylvain Lefebvre 2004
//
//     Sylvain.Lefebvre@laposte.net
// --------------------------------------------------------------
// --------------------------------------------------------------
// LibSL GPUMesh tutorial
// --------------------------------------------------------------

#include <LibSL/LibSL.h>
LIBSL_WIN32_FIX // required due to VisualC++ 2005 compiler issue

#include <LibSL/LibSL_gl.h>

// --------------------------------------------------------------

// define our vertex format

typedef GPUMESH_MVF10(mvf_vertex_2d,
                      mvf_color0_rgba,
                      mvf_texcoord0_2d,
                      mvf_texcoord1_2d,
                      mvf_texcoord2_2d,
                      mvf_texcoord3_2d,
                      mvf_texcoord4_2d,
                      mvf_texcoord5_2d,
                      mvf_texcoord6_2d,
                      mvf_texcoord7_2d     ) mvf_mesh;

// associate mesh and choosen vertex format

typedef GPUMesh_Indexed_GL_DisplayList<mvf_mesh,unsigned int> IndexedMesh;

IndexedMesh         *g_Object=NULL;

// --------------------------------------------------------------

int     g_Width  = 640;
int     g_Height = 480;

Tex2DRGBA_Ptr g_Texs[8];
int           g_NumTex;

// --------------------------------------------------------------

void mainKeyboard(uchar key)
{
  if (key == 'q') {
    SimpleUI::exit();
  }
}

// --------------------------------------------------------------

void mainMouse(int btn, int state, int x, int y) 
{

}

// --------------------------------------------------------------

void mainReshape(uint w,uint h)
{
  g_Width  = w;
  g_Height = h;
}

// --------------------------------------------------------------

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

  // setup textures
  ForIndex(i,g_NumTex) {
    glActiveTextureARB(GL_TEXTURE0_ARB + i);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,g_Texs[i]->handle());
  }
  glActiveTextureARB(GL_TEXTURE0_ARB);

  // render object
  g_Object->render();

}

// --------------------------------------------------------------

int main(int argc, char **argv) 
{
  try {

    // create viewport

    SimpleUI::onRender     = mainRender;
    SimpleUI::onKeyPressed = mainKeyboard;
    SimpleUI::init(g_Width,g_Height,"GPUMesh tutorial");

    // create objects

    g_Object = new IndexedMesh();

    g_Object->begin(GPUMESH_TRIANGLELIST);

    g_Object->color0_4(255,255,255,0);
    g_Object->texcoord0_2(0.0,0.0);
    g_Object->texcoord1_2(0.0,0.0);
    g_Object->texcoord2_2(0.0,0.0);
    g_Object->texcoord3_2(0.0,0.0);
    g_Object->texcoord4_2(0.0,0.0);
    g_Object->texcoord5_2(0.0,0.0);
    g_Object->texcoord6_2(0.0,0.0);
    g_Object->texcoord7_2(0.0,0.0);
    g_Object->vertex_2(0.1,0.1);

    g_Object->color0_4(255,255,255,0);
    g_Object->texcoord0_2(0.0,1.0);
    g_Object->texcoord1_2(0.0,1.0);
    g_Object->texcoord2_2(0.0,1.0);
    g_Object->texcoord3_2(0.0,1.0);
    g_Object->texcoord4_2(0.0,1.0);
    g_Object->texcoord5_2(0.0,1.0);
    g_Object->texcoord6_2(0.0,1.0);
    g_Object->texcoord7_2(0.0,1.0);
    g_Object->vertex_2(0.1,0.9);

    g_Object->color0_4(255,255,255,0);
    g_Object->texcoord0_2(1.0,1.0);
    g_Object->texcoord1_2(1.0,1.0);
    g_Object->texcoord2_2(1.0,1.0);
    g_Object->texcoord3_2(1.0,1.0);
    g_Object->texcoord4_2(1.0,1.0);
    g_Object->texcoord5_2(1.0,1.0);
    g_Object->texcoord6_2(1.0,1.0);
    g_Object->texcoord7_2(1.0,1.0);
    g_Object->vertex_2(0.9,0.9);

    g_Object->color0_4(255,255,255,0);
    g_Object->texcoord0_2(1.0,0.0);
    g_Object->texcoord1_2(1.0,0.0);
    g_Object->texcoord2_2(1.0,0.0);
    g_Object->texcoord3_2(1.0,0.0);
    g_Object->texcoord4_2(1.0,0.0);
    g_Object->texcoord5_2(1.0,0.0);
    g_Object->texcoord6_2(1.0,0.0);
    g_Object->texcoord7_2(1.0,0.0);
    g_Object->vertex_2(0.9,0.1);

    g_Object->index(0);
    g_Object->index(1);
    g_Object->index(2);

    g_Object->index(0);
    g_Object->index(2);
    g_Object->index(3);

    g_Object->end();

    GLint v;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&v);
    std::cerr << "Number of available texture units " 
      << v << std::endl << std::endl;
    g_NumTex = min(v,8);

    // load textures
    ForIndex(i,8) {
      ImageRGBA_Ptr img = loadImage<ImageRGB>(sprint("media\\%d.png",i+1))->cast<ImageRGBA>();
      g_Texs[i]         = new Tex2DRGBA(img->pixels());
    }

    // help
    printf("[q]     - quit\n");

    // render loop
    SimpleUI::loop();

    // clean up
    delete (g_Object);
    ForIndex(i,8) { 
      g_Texs[i] = NULL; 
    }

    // close window
    SimpleUI::shutdown();

  } catch (Fatal& f) {
    std::cerr << f.message() << std::endl;
  }

  return (0);

}

// --------------------------------------------------------------
