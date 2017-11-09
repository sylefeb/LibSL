#include <iostream>
#include <ctime>
#include <cmath>
#include <cstring>

/* -------------------------------------------------------- */

#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>
#include <GL/gl.h>
#include <GL/glut.h>

using namespace LibSL;

LIBSL_WIN32_FIX;

/* -------------------------------------------------------- */
using namespace std;
/* -------------------------------------------------------- */

int       g_Width = 1024;
int       g_Height = 1024;

// Mesh definition for a simple textured quad

typedef GPUMESH_MVF2(mvf_vertex_3f, mvf_texcoord0_2f) mvf_quad;
typedef GPUMesh_VertexBuffer<mvf_quad>               MeshQuad;

AutoPtr<MeshQuad> g_Quad;
Tex2DRGBA_Ptr     g_Tex;
RenderTarget2DRGBA_Ptr g_RT;

/* -------------------------------------------------------- */

void mainKeyboard(unsigned char key)
{

}

/* -------------------------------------------------------- */

void mainRender()
{
  m4x4f rot = quatf(v3f(0, 0, 1), milliseconds()/1000.0f).toMatrix();

  // render to texture
  g_RT->bind();
  glViewport(0, 0, g_RT->w(), g_RT->h());
  GPUHelpers::clearScreen(
    LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER,
    0.0, 1.0, 0.0);

  LibSL::GPUHelpers::Transform::ortho2D(LIBSL_PROJECTION_MATRIX, -2, 2, -2, 2);

  LibSL::GPUHelpers::Transform::set(LIBSL_MODELVIEW_MATRIX, rot);

  glBindTexture(GL_TEXTURE_2D, g_Tex->handle());

  g_Quad->render();

  g_RT->unbind();

  // render to main

  GPUHelpers::clearScreen(
    LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER,
    0.0, 0.0, 1.0);

  LibSL::GPUHelpers::Transform::ortho2D(LIBSL_PROJECTION_MATRIX, -2, 2, -2, 2);

  LibSL::GPUHelpers::Transform::set(LIBSL_MODELVIEW_MATRIX, rot);

  glBindTexture(GL_TEXTURE_2D, g_RT->texture());

  g_Quad->render();

}

/* -------------------------------------------------------- */

int main(int argc, char **argv)
{
  try {

    SimpleUI::init(256, 256);
    SimpleUI::onRender = mainRender;

    cerr << "******************************************************************\n";

    g_RT = RenderTarget2DRGBA_Ptr(new RenderTarget2DRGBA(256, 256));

    cerr << "******************************************************************\n";

    ImageRGBA_Ptr img(new ImageRGBA(256, 256));
    ForImage(img, i, j) {
      img->pixel(i, j) = v4b(i, j, 0, 255);
    }
    g_Tex = Tex2DRGBA_Ptr(new Tex2DRGBA(img->pixels()));

    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->begin();
    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->bindTextureUnit(0);
    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->enableTexture();
    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->end();

    cerr << "******************************************************************\n";

    // create a quad
    g_Quad = AutoPtr<MeshQuad>(new MeshQuad());
    g_Quad->begin(GPUMESH_TRIANGLELIST);
    g_Quad->texcoord0_2(1.0f,0.0f);
    g_Quad->vertex_3(1.0f, -1.0f, 0);
    g_Quad->texcoord0_2(0.0f, 0.0f);
    g_Quad->vertex_3(-1.0f, -1.0f, 0);
    g_Quad->texcoord0_2(1.0f, 1.0f);
    g_Quad->vertex_3(1.0f, 1.0f, 0);
    g_Quad->texcoord0_2(1.0f, 1.0f);
    g_Quad->vertex_3(1.0f, 1.0f, 0);
    g_Quad->texcoord0_2(0.0f, 0.0f);
    g_Quad->vertex_3(-1.0f, -1.0f, 0);
    g_Quad->texcoord0_2(0.0f, 1.0f);
    g_Quad->vertex_3(-1.0f, 1.0f, 0);
    g_Quad->end();

    cerr << "******************************************************************\n";

    SimpleUI::loop();

    SimpleUI::shutdown();

  } catch (Fatal& f) {
    cerr << f.message() << endl;
  }

  return (0);
}

/* -------------------------------------------------------- */
