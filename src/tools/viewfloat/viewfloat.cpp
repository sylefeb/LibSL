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
#include <LibSL/LibSL_d3d.h>

LIBSL_WIN32_FIX;

#include <AntTweakBar.h>
#include <psapi.h>

/* -------------------------------------------------------- */
using namespace std;
/* -------------------------------------------------------- */

FxShader    g_fxShader;
FxParameter g_fxTex;
FxParameter g_fxMinValue;
FxParameter g_fxMaxValue;
FxParameter g_fxGamma;
FxParameter g_fxExpCompensation;
FxTechnique g_fxLinear;
FxTechnique g_fxToneMap;

int       g_Width  = 1024;
int       g_Height = 1024;

// Mesh definition for a simple textured quad

typedef GPUMESH_MVF2(mvf_vertex_3f,mvf_texcoord0_2f) mvf_quad;
typedef GPUMesh_VertexBuffer<mvf_quad>               MeshQuad;

AutoPtr<MeshQuad> g_Quad;

// Floating point texture
Tex2DRGBA32F_Ptr  g_Tex;

float g_MinValue        =  1e30f;
float g_MaxValue        = -1e30f;
float g_Gamma           = 2.5f;
float g_ExpCompensation = 0;
uint  g_Mode            = 1;

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

  if (g_MinValue >= g_MaxValue) {
    g_MinValue = g_MaxValue - 0.001f*(g_MaxValue-g_MinValue);
  }
  g_fxMinValue       .set(g_MinValue);
  g_fxMaxValue       .set(g_MaxValue);
  g_fxGamma          .set(g_Gamma);
  g_fxExpCompensation.set(g_ExpCompensation);

  if (g_Mode == 0) {
    g_fxLinear.activate();
  } else {
    g_fxToneMap.activate();
  }

  g_fxShader.begin();
  g_Quad->render();
  g_fxShader.end();

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
  bool batch_mode    = false;
  bool tonemap       = false;
  float compensation = 0;

  try {

    sl_assert(argc > 1);

    // load float image
    Image_generic<float,4>::t_AutoPtr fimg4;
    try {
      Image_generic<float,3>::t_AutoPtr fimg3( loadImage<Image_generic<float,3> >(argv[1]) );
      cerr << sprint("Loaded floating point image '%s', %dx%d pixels, %d components\n",argv[1],fimg3->w(),fimg3->h(),fimg3->numComp());
      fimg4 = Image_generic<float,4>::t_AutoPtr(fimg3->cast<Image_generic<float,4> >());
      fimg3 = Image_generic<float,3>::t_AutoPtr(NULL);
    } catch (...) {
      try {
        Image_generic<float,1>::t_AutoPtr fimg1( loadImage<Image_generic<float,1> >(argv[1]) );
        cerr << sprint("Loaded floating point image '%s', %dx%d pixels, %d components\n",argv[1],fimg1->w(),fimg1->h(),fimg1->numComp());
        fimg4 = Image_generic<float,4>::t_AutoPtr(fimg1->cast<Image_generic<float,4> >());
        fimg1 = Image_generic<float,1>::t_AutoPtr(NULL);
      } catch (...) {
        fimg4 = Image_generic<float,4>::t_AutoPtr(loadImage<Image_generic<float,4> >(argv[1]));
      }
    }

    if (0) {
      uint res   = 1025;
      if (fimg4->w() > res || fimg4->h() > res) {
        uint cropx = (fimg4->w()-res)/2;
        uint cropy = (fimg4->h()-res)/2;
        fimg4 = Image_generic<float,4>::t_AutoPtr(fimg4->extract(cropx,cropy,res,res));
      }
    }

    // search min/max over image
    g_MinValue =  1e30f;
    g_MaxValue = -1e30f;
    ForImage(fimg4,x,y) {
      g_MinValue = Math::min( tupleMin(fimg4->pixel(x,y)) , g_MinValue);
      g_MaxValue = Math::max( tupleMax(fimg4->pixel(x,y)) , g_MaxValue);
    }

    /// command line
    int arg = 1;
    while (arg < argc) {
      if (!strcmp(argv[arg],"-tonemap")) {
        if (arg < argc-1) {
          compensation = (float)atof(argv[arg+1]);
          cout << "* batch mode: tone mapping with compensation " << compensation << endl;
          tonemap    = true;
          batch_mode = true;
          arg ++;
        }
      } else {
        // argv[arg] is file name
      }
      arg ++;
    }

    // init UI
    SimpleUI::onRender     = mainRender;
    SimpleUI::onKeyPressed = mainKeyboard;
    SimpleUI::init(fimg4->w(),fimg4->h(),argv[1]);

    // Initialize AntTweakBar
    if( !TwInit(TW_DIRECT3D9, LIBSL_DIRECT3D_DEVICE) ) {
	    throw Fatal("Cannot initialize AntTweakBar!");
  	}
    SimpleUI::setCustomCallbackMsgProc(mainWin32Event);

    // Create a tweak bar
    TwBar *bar = TwNewBar("main");
    TwDefine("main label='Linear mapping'");
    TwDefine("main size='150 150'");
    TwWindowSize(fimg4->w(),fimg4->h());
    TwEnumVal mode[] = 
    {
      { 0,	"Linear"		}, 
      { 1,	"ToneMapping"	}, 
    };
	  TwType modeType = TwDefineEnum("Mode", mode, 2);	// create a new TwType associated to the enum defined by the modeEV array
    TwAddVarRW(bar, "Mode", modeType, &g_Mode,"label='Mode'");
    TwAddVarRW(bar, "Gamma"          , TW_TYPE_FLOAT, &g_Gamma          ,"label='Gamma' group='tonemap' min=0 max=4 step=0.01 precision=3");
    TwAddVarRW(bar, "ExpCompensation", TW_TYPE_FLOAT, &g_ExpCompensation,"label='Compensation' group='tonemap' min=-8 max=8 step=0.01 precision=3");
//    TwAddVarRW(bar, "Min"            , TW_TYPE_FLOAT, &g_MinValue       ,sprint("label='Min' group='minmax' logstep min=%f max=%f step=%f keyIncr=s keyDecr=a precision=3",g_MinValue,g_MaxValue,(g_MaxValue-g_MinValue)*0.01f));
    TwAddVarRW(bar, "Max"            , TW_TYPE_FLOAT, &g_MaxValue       ,sprint("label='Max' group='minmax' min=%f max=%f step=%f keyIncr=f keyDecr=d precision=3",g_MinValue,g_MaxValue,(g_MaxValue-g_MinValue)*0.01f));

    TwAddVarRW(bar, "Min"            , TW_TYPE_FLOAT, &g_MinValue       ,sprint("label='Min' group='minmax' min=0 max=256 step=0.5 keyIncr=s keyDecr=a"));

    // create texture
    g_Tex = Tex2DRGBA32F_Ptr(new Tex2DRGBA32F(fimg4->pixels()));
    fimg4 = Image_generic<float,4>::t_AutoPtr(NULL);

    // help
    cerr << "floatting point images viewever\n";
    cerr << "[q]     quit\n";
    cerr << "Click on values, hold mouse button and circle around to increase / decrease\n";
    cerr << sprint("min/max: [%f,%f]\n",g_MinValue,g_MaxValue);
    
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

    // load shader
    string code = loadFileIntoString(sprint("%s\\viewfloat.fx",System::Application::executablePath()));
    g_fxShader  .init(code.c_str());
    g_fxTex     .init(g_fxShader,"Tex");
    g_fxTex     .set(g_Tex->handle());
    g_fxMinValue.init(g_fxShader,"MinValue");
    g_fxMaxValue.init(g_fxShader,"MaxValue");
    g_fxGamma   .init(g_fxShader,"Gamma");
    g_fxExpCompensation.init(g_fxShader,"ExpCompensation");

    g_fxLinear  .init(g_fxShader,"t_linear");
    g_fxToneMap .init(g_fxShader,"t_tonemap");

    g_fxToneMap .activate();

    if (!batch_mode) {

      // inner loop
      SimpleUI::loop();

    } else {

      // create a RT
      RenderTarget2DRGBA_Ptr rt(new RenderTarget2DRGBA(g_Tex->w(),g_Tex->h()));
      LIBSL_DIRECT3D_DEVICE->BeginScene();
      rt->bind();
      GPUHelpers::clearScreen(
        LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER,
        1.0,0.0,1.0);
      
      g_fxMinValue       .set(g_MinValue);
      g_fxMaxValue       .set(g_MaxValue);
      g_fxGamma          .set(g_Gamma);
      g_fxExpCompensation.set(compensation);
      if (!tonemap) {
        g_fxLinear.activate();
      } else {
        g_fxToneMap.activate();
      }
      
      g_fxShader.begin();
      g_Quad->render();
      g_fxShader.end();

      rt->unbind();
      LIBSL_DIRECT3D_DEVICE->EndScene();
      // read back and save
      ImageRGBA_Ptr img( new ImageRGBA() );
      rt->readBack(img->pixels());
      ImageRGB_Ptr  rgb( img->cast<ImageRGB>() );
      ForImage(rgb,i,j) {
        // for some reason R and B channels are reverted
        uchar tmp          = rgb->pixel(i,j)[0];
        rgb->pixel(i,j)[0] = rgb->pixel(i,j)[2];
        rgb->pixel(i,j)[2] = tmp;
      }
      saveImage(sprint("%s.tonemap.%f.png",argv[1],compensation),rgb);

    }

    // free ressources
    g_Quad = AutoPtr<MeshQuad>(NULL);
    g_Tex  = Tex2DRGBA32F_Ptr(NULL);
    g_fxShader.terminate();
    TwTerminate();
    SimpleUI::shutdown();

  } catch (Fatal& e) {
    cerr << e.message() << endl;
    return (-1);
  }

  return (0);
}

/* -------------------------------------------------------- */
