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

// --------------------------------------------------------------

#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>

// --------------------------------------------------------------

using namespace std;
using namespace LibSL;

// --------------------------------------------------------------

LIBSL_WIN32_FIX

// --------------------------------------------------------------

#define SCREEN_W 1024
#define SCREEN_H 1024
#define FOV      float(M_PI/2.0)

typedef struct
{
  LibSL::Math::v3f pos;
  LibSL::Math::v3f nrm;
  LibSL::Math::v2f uv;
} t_VertexData;

typedef MVF3(mvf_position_3f,mvf_normal_3f,mvf_texcoord0_2f) t_VertexFormat;

TriangleMesh_Ptr                               g_Mesh;
AutoPtr<MeshRenderer        <t_VertexFormat> > g_Renderer;
AutoPtr<TexturedMeshRenderer<t_VertexFormat> > g_TexturedRenderer;

GLShader                  g_glShader;
GLParameter               g_glTex;
GLParameter               g_glDiffuse;
GLParameter               g_glShowTexCoord;
GLParameter               g_glShowNormal;

RenderTarget2DRGBA32F_Ptr g_RT;
Tex2DRGBA_Ptr             g_Tex;
// AutoPtr<GPUHelpers::Font> g_Font;

bool                      g_Wireframe = false;

// --------------------------------------------------------------

void mainKeyboard(unsigned char key)
{
  static float speed=1.0f;
  static char last=' ';

  if (key == 'q') {
    SimpleUI::exit();
  } else if (key == 'l') {
    g_Wireframe = !g_Wireframe;
  } else if (key == 'z') {
    TrackballUI::trackball().setUp(Trackball::Z_neg);
  } else if (key == 'x') {
    TrackballUI::trackball().setUp(Trackball::Z_pos);
  } else if (key == '*') {
    static uint n = 0;
    g_RT->bind();
    glViewport(0,0,g_RT->w(),g_RT->h());
    glEnable(GL_DEPTH_TEST);
    GPUHelpers::clearScreen(LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER, 0,0,0);
    GPUHelpers::Transform::set(LIBSL_MODELVIEW_MATRIX,TrackballUI::matrix());
    g_glShader.begin();
    glActiveTextureARB(GL_TEXTURE0);
	  glBindTexture(GL_TEXTURE_2D,g_Tex->handle());
    g_glTex   .set(0);
	  if (g_Renderer.isNull ())  {
		  g_TexturedRenderer->render ();
	  } else {
		  g_Renderer->render();
	  }
    g_glShader.end();
    g_RT->unbind();
    ImageFloat4_Ptr img = ImageFloat4_Ptr(new ImageFloat4());
    g_RT->readBack(img->pixels());
    saveImage(sprint("shot%03d.float", n),img.raw());
    saveImage(sprint("shot%03d.png"  ,n),img->cast<ImageRGB>());
    n ++;
  } else if (key == 't') {
    g_glShader.begin();
    g_glShowNormal  .set(0);
    g_glShowTexCoord.set(1);
    g_glShader.end();
  } else if (key == 'n') {
    g_glShader.begin();
    g_glShowNormal  .set(1);
    g_glShowTexCoord.set(0);
    g_glShader.end();
  } else if (key == ' ') {
    g_glShader.begin();
    g_glShowNormal  .set(0);
    g_glShowTexCoord.set(0);
    g_glShader.end();
  } else if (key == '\\') {
    static bool swap = true;
    TrackballUI::trackball().setWalkthrough(swap);
    swap = !swap;
  } else if (key == 'v') {
    cerr << TrackballUI::trackball().rotation()    << endl;
    cerr << TrackballUI::trackball().translation() << endl;
  }
}

// --------------------------------------------------------------

void mainAnimate(double time,float elapsed)
{

}

// --------------------------------------------------------------

void   trackballUI_debug();

void mainRender()
{
  /// render on screen
  glViewport(0,0,SCREEN_W,SCREEN_H);
  GPUHelpers::clearScreen(LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER, 0.2f,0.2f,0.2f);

  // setup view
  float vdist = max(g_Mesh->bbox().extent()[0],g_Mesh->bbox().extent()[1]);
  Transform::perspective(LIBSL_PROJECTION_MATRIX,FOV,SCREEN_W/float(SCREEN_H),0.001f*vdist,100.0f*vdist);
  Transform::set        (LIBSL_MODELVIEW_MATRIX,TrackballUI::matrix());

  glEnable(GL_CULL_FACE);

  if (g_Wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  }

  if ( ! g_Renderer.isNull()) {
    g_glShader.begin();
    g_Renderer->render();
    g_glShader.end();
  } else {
    glDisable   (GL_ALPHA_TEST);
    glDisable   (GL_LIGHTING);
    glEnable    (GL_TEXTURE_2D);
    g_glShader.begin();
    ForIndex(s,g_Mesh->numSurfaces()) {
      g_glDiffuse.set( g_Mesh->surfaceAt(s).diffuse );
      g_TexturedRenderer->renderSurface(s);
    }
    g_glShader.end();
  }

  trackballUI_debug();

  static Every ev(1000);
  static int   cnt = 0;
  static float fps = 0;
  cnt ++;
  System::Time::t_time diff = 0;
  if (ev.expired(diff)) {
    fps = cnt * 1000.0f / diff;
    cnt = 0;
  }
/*
  if (fps > 0) {
    char str[1024];
    sprintf_s(str,1024,"%.2f FPS",fps);
    float w,h;
    glPushAttrib(GL_ENABLE_BIT);
    glDisable   (GL_LIGHTING);
    glDisable   (GL_ALPHA_TEST);
    glDisable   (GL_DEPTH_TEST);
    glEnable    (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f   (1,1,1,1);
    GPUHelpers::Transform::ortho2D (LIBSL_PROJECTION_MATRIX,0,1,0,1);
    GPUHelpers::Transform::identity(LIBSL_MODELVIEW_MATRIX);
    g_Font->printStringNeed(0.01f,str,&w,&h);
    g_Font->printString    (0.0f,0.0f,0.01f,str);
    glPopAttrib();
  }
*/

}

/* -------------------------------------------------------- */

int main(int argc, char **argv)
{
  try {

    if (argc < 2) {
      cerr << "Usage: %s <mesh file> (<texture file>)\n" << endl;
      cerr << "       if 'viewmesh.fp' is present in current directory, it will be used" << endl;
      return (0);
    }

    /// init simple UI (glut clone for both GL and D3D)
    cerr << "Init TrackballUI   ";
    TrackballUI::onRender     = mainRender;
    TrackballUI::onKeyPressed = mainKeyboard;
    TrackballUI::onAnimate    = mainAnimate;
    TrackballUI::init(SCREEN_W,SCREEN_H);

    cerr << "[OK]" << endl;

    /// help
    printf("[q]     - quit\n");

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    // render target
    g_RT = RenderTarget2DRGBA32F_Ptr(new RenderTarget2DRGBA32F(SCREEN_W, SCREEN_H));

    // shader
    cerr << "Loading shader ";
    string vp_code = loadFileIntoString(adaptPath(sprint("%s\\viewmesh.vp",System::Application::executablePath())));
    string fp_code;
    if (System::File::exists("viewmesh.fp")) {
      cerr << "(using local shader) " << endl;
      fp_code = loadFileIntoString("viewmesh.fp");
    } else {
      fp_code = loadFileIntoString(adaptPath(sprint("%s\\viewmesh.fp",System::Application::executablePath())));
    }
    g_glShader.init(vp_code.c_str(),fp_code.c_str(),NULL,"[viewmesh]");
    g_glShader.setStrict(false);

    if (argc < 3) {
      ImageRGBA_Ptr chk = ImageRGBA_Ptr(new ImageRGBA(16, 16));
      ForImage(chk,i,j) {
        chk->pixel(i,j)  = (i+j)&1 ? 255 : 0;
      }
      g_Tex = Tex2DRGBA_Ptr(new Tex2DRGBA(chk->pixels(), GPUTEX_AUTOGEN_MIPMAP));
    } else {
      ImageRGBA_Ptr img = ImageRGBA_Ptr(loadImage<ImageRGBA>(argv[2]));
      g_Tex = Tex2DRGBA_Ptr(new Tex2DRGBA(img->pixels(), GPUTEX_AUTOGEN_MIPMAP));
    }
    g_glShader      .begin();
    g_glTex         .init(g_glShader,"u_Tex");
    g_glShowTexCoord.init(g_glShader,"u_ShowTexCoord");
    g_glShowTexCoord.set(0);
    g_glShowNormal  .init(g_glShader,"u_ShowNormal");
    g_glShowNormal  .set(1);
    g_glDiffuse     .init(g_glShader,"u_Diffuse");
    g_glDiffuse     .set(V3F(1,1,1));
    glActiveTextureARB(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,g_Tex->handle());
    g_glTex         .set(0);
    g_glShader.end();
    cerr << "[OK]" << endl;

    // load font
    // cerr << "Loading font      ";
    // g_Font = new GPUHelpers::Font(adaptPath(sprint("%s\\media\\basefont.png",System::Application::executablePath())));
    // cerr << "[OK]" << endl;

    // load mesh
    cerr << "Loading mesh      ";
    g_Mesh     = TriangleMesh_Ptr(loadTriangleMesh<t_VertexData,t_VertexFormat>(argv[1]));

    cerr << "[OK]" << endl;
    if (g_Mesh->mvf().isNull()) {
      cerr << "  mesh has no vertex format information" << endl;
    } else {
      cerr << "  mesh MVF: \n" << *g_Mesh->mvf() << endl;
    }
    cerr << "  mesh bbox : " << g_Mesh->bbox().minCorner() << " - " << g_Mesh->bbox().maxCorner() << endl;

	g_Mesh->scaleToUnitCube();
    g_Mesh->centerOn(0);

    // TEST
    // saveTriangleMesh("tmp.obj",g_Mesh);

    TrackballUI::trackball().translation() = g_Mesh->bbox().center()[0];
    cerr << sprint("  mesh contains %d vertices, %d triangles, %d surfaces\n",g_Mesh->numVertices(),g_Mesh->numTriangles(),g_Mesh->numSurfaces());

    cerr << "Creating renderer ";
    if (g_Mesh->numSurfaces() == 0) {
      cerr << " (no texture) ";
      g_Renderer = AutoPtr<MeshRenderer<t_VertexFormat> >(new MeshRenderer<t_VertexFormat>(g_Mesh.raw()));
    } else {
      cerr << " ( textured ) ";
      if (strstr(argv[1],".proc") != NULL) {
        std::vector<std::string> suffixes;
        suffixes.push_back ("_d");
        g_TexturedRenderer = AutoPtr<TexturedMeshRenderer<t_VertexFormat> >(new TexturedMeshRenderer<t_VertexFormat>(
          g_Mesh,
          AutoPtr<TextureProvider>(new DefaultTextureProvider("", Tex2DRGBA_Ptr(), ".tga", suffixes)) ));
      } else {
        g_TexturedRenderer = AutoPtr<TexturedMeshRenderer<t_VertexFormat> >(new TexturedMeshRenderer<t_VertexFormat>(
          g_Mesh,
          AutoPtr<TextureProvider>(new DefaultTextureProvider("", Tex2DRGBA_Ptr(), ".png", vector<string>()))));
      }
    }
    cerr << "[OK]" << endl;

    /// setup trackball
    float vdist = max(g_Mesh->bbox().extent()[0],g_Mesh->bbox().extent()[1]);
    TrackballUI::trackball().setWalkSpeed(vdist/2.0f);
    TrackballUI::trackball().translation() = V3F(0,0,-vdist*2);

    /// main loop
    TrackballUI::loop();

    /// clean exit
    g_glShader.terminate();
    g_Mesh     = AutoPtr<TriangleMesh>();
    g_Renderer = AutoPtr<MeshRenderer<t_VertexFormat> >();
    g_TexturedRenderer = AutoPtr<TexturedMeshRenderer<t_VertexFormat> >();
    g_RT       = RenderTarget2DRGBA32F_Ptr();
    g_Tex      = Tex2DRGBA_Ptr();

    // shutdown SimpleUI
    TrackballUI::shutdown();

  } catch (Fatal& e) {
#ifdef WIN32
    MessageBoxA(NULL,e.message(),"",MB_OK);
#endif
    cerr << Console::red << e.message() << Console::gray << endl;
    return (-1);
  }
  return (0);
}

/* -------------------------------------------------------- */
