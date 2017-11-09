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

#ifdef OPENGL
#include <LibSL/LibSL_gl.h>
#define CG
#endif

#ifdef DIRECT3D
#include <LibSL/LibSL_d3d.h>
//#define CG // comment to use Fx render (Direct3D version)
#endif

// --------------------------------------------------------------

#include <LibSL/Mesh/AnimatedMesh.h>
#include <LibSL/Mesh/AnimatedMeshController.h>
using namespace LibSL::Mesh;

// --------------------------------------------------------------

using namespace std;

// --------------------------------------------------------------

#ifdef WIN32 // FIXME this is due to a VC 2005 compiler issue
#include <LibSL/Image/ImageFormat_PNG.h>
LibSL::Image::ImageFormat_PNG s_PNG;
#include <LibSL/Image/ImageFormat_JPG.h>
LibSL::Image::ImageFormat_JPG s_JPG;
#endif

// --------------------------------------------------------------

#define SCREEN_W 1024
#define SCREEN_H 768

AnimatedMesh_Ptr              g_AnimatedMesh;
AnimatedMeshController_Ptr    g_Controller;


#ifdef CG
#include <LibSL/Mesh/AnimatedMeshCgRenderer.h>
typedef AnimatedMeshCgRenderer MyRenderer;
#else
#include <LibSL/Mesh/AnimatedMeshFxRenderer.h>
typedef AnimatedMeshFxRenderer MyRenderer;
#endif

AutoPtr<MyRenderer> g_Renderer;

v3f    m_Center;
float  m_Radius;

// --------------------------------------------------------------

class TestController : public AnimatedMeshController
{

public:

  TestController(LibSL::Mesh::AnimatedMesh_Ptr mesh) : AnimatedMeshController(mesh)
  {

  }

  void computeAnimatedFrame(
    LibSL::DataStructures::Hierarchy<LibSL::Mesh::AnimatedMesh::t_frame>::t_AutoPtr node,
    quatf& _q,v3f& _t,v3f& _s)
  {
    _t=V3F(0,0,0);
    _s=V3F(1,1,1);
    if (node->data().name == "Bip01_Spine1") {
      static float agl=0.0f;
      agl+=M_PI/200.0f;
      _q=quatf(V3F(1,0,0),cos(agl)*M_PI/4.0f);
    } else if (node->data().name == "Bip01_Head") {
      static float agl=0.0f;
      agl+=M_PI/100.0f;
      _q=quatf(V3F(0,1,0),cos(agl)*M_PI/5.0f);
    }
  }

};

// --------------------------------------------------------------

void mainKeyboard(unsigned char key) 
{
  if (key == 'q') {
    SimpleUI::exit();
  }
}

// --------------------------------------------------------------

void mainAnimate(float time,float elapsed)
{
  FLOAT a=time/2000.0;
  Transform::lookat(LIBSL_MODELVIEW_MATRIX,
    V3F(m_Radius*2.0*cos(a),0,m_Radius*2.0*sin(a))+m_Center,
    V3F(1,0.7f,1)*m_Center,
    V3F(0,1,0));
}

// --------------------------------------------------------------

void mainRender()
{
  /// render on screen

  GPUHelpers::clearScreen(LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER,
    0,0,1);

  g_Controller->animate();
  
  g_Renderer->render(*g_Controller);
  
}

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  try {

    /// init simple UI (glut clone for both GL and D3D)
    cerr << "Init SimpleUI   ";
    SimpleUI::onRender=mainRender;
    SimpleUI::onKeyPressed=mainKeyboard;
    SimpleUI::onAnimate=mainAnimate;
    SimpleUI::init(SCREEN_W,SCREEN_H);
    cerr << "[OK]" << endl;

    /// help
    printf("[q]     - quit\n");

#ifdef DIRECT3D
    /// d3d init
    LPDIRECT3DDEVICE9 d3d=LIBSL_DIRECT3D_DEVICE;
    d3d->SetRenderState(D3DRS_LIGHTING,FALSE);
    d3d->SetRenderState(D3DRS_ZENABLE,TRUE);
    d3d->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
#endif

#ifdef OPENGL
    glEnable(GL_DEPTH_TEST);
#endif

    cerr << "Loading texture  ";
    ImageRGB_Ptr img=loadImage<ImageRGB>("C:\\WORK\\GFX\\MODELS\\LibSL\\female_soldier.jpg");
    ImageRGBA_Ptr imga=img->cast<ImageRGBA>();
    img=NULL;
    Tex2DRGBA_Ptr tex=new Tex2DRGBA(imga->pixels(),GPUTEX_AUTOGEN_MIPMAP);
    imga=NULL;
    cerr << "[OK]" << endl;

    // open animated mesh
    cerr << "Loading mesh     ";
    g_AnimatedMesh=new AnimatedMesh();
    g_AnimatedMesh->readFromFile("C:\\WORK\\GFX\\MODELS\\LibSL\\female.animesh");
    cerr << "[OK]" << endl;

#ifdef CG
    cerr << "Init Cg renderer ";
    const char *custom_code="\
\n#ifdef DIRECT3D\n\
               sampler2D Tex = sampler_state { \
                 MinFilter = LinearMipmapLinear; \
                 MagFilter = Linear; \
               }; \
\n#else\n\
               sampler2D Tex = sampler_state { \
                 MinFilter = LinearMipmapLinear; \
                 MagFilter = Linear; \
               }; \
\n#endif\n\
       float4 fp_animatedmesh(VS_OUTPUT In) : COLOR0 \
       { \
       return (tex2D(Tex,In.UV)); \
       } \
";
#else
    cerr << "Init Fx renderer ";
    const char *custom_code="\
               Texture Tex; \
               sampler2D S_Tex = sampler_state { \
                 Texture   = (Tex); \
                 MinFilter = Linear; \
                 MipFilter = Linear; \
                 MagFilter = Linear; \
               }; \
\
       float4 fp_animatedmesh(VS_OUTPUT In) : COLOR0 \
       { \
         return (tex2D(S_Tex,In.UV)); \
       } \
";
#endif

    g_Renderer=new MyRenderer(g_AnimatedMesh,custom_code);

#ifdef CG
    CgParameter cgTex;
    cgTex.init(g_Renderer->shader(),"Tex");
    cgTex.set(tex->texture());
#else
    FxParameter fxTex;
    fxTex.init(g_Renderer->shader(),"Tex");
    fxTex.set(tex->texture());
#endif

    cerr << "[OK]" << endl;

    g_Controller=new CombineController(g_AnimatedMesh,
      new KeyframeController(g_AnimatedMesh,1.0f,0,59,true),
      new TestController(g_AnimatedMesh));

    // setup view
    m_Center=g_AnimatedMesh->center();
    m_Radius=g_AnimatedMesh->radius();
    Transform::perspective(LIBSL_PROJECTION_MATRIX,float(M_PI/4.0),SCREEN_W/float(SCREEN_H),0.001f,100.0f);
    Transform::lookat(LIBSL_MODELVIEW_MATRIX,
      V3F(m_Radius*2.0,0,0)+m_Center,
      V3F(1,0.7f,1)*m_Center,
      V3F(0,1,0));

    /// main loop
    SimpleUI::loop();

    /// clean exit
    g_AnimatedMesh=NULL;
    g_Renderer=NULL;
    g_Controller=NULL;
    tex=NULL;

    // shutdown SimpleUI
    SimpleUI::shutdown();

  } catch (Fatal& e) {
    cerr << e.message() << endl;
    return (-1);
  }

  return (0);
}

/* -------------------------------------------------------- */
