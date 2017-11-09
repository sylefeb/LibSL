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
#ifdef DIRECT3D
#  include <LibSL/LibSL_d3d.h>
#endif
#ifdef DIRECTX10
#  include <LibSL/LibSL_dx10.h>
#endif

using namespace std;

LIBSL_WIN32_FIX;

/* -------------------------------------------------------- */

// Mesh definition for a simple textured quad

typedef GPUMESH_MVF2(mvf_vertex_3f,mvf_texcoord0_2f) mvf_quad;
typedef GPUMesh_VertexBuffer<mvf_quad>               QuadMesh;

QuadMesh *g_Quad = NULL;

FxShader    g_fxShader;
FxParameter g_fxTex;
FxParameter g_fxLevel;

RenderTarget2DRGBA_Ptr g_RT;
Tex2DRGBA_Ptr          g_Tex;

/* -------------------------------------------------------- */

int     g_Width  = 512;
int     g_Height = 512;

/* -------------------------------------------------------- */

void mainKeyboard(unsigned char key) 
{
  if (key == 'q') {
    SimpleUI::exit(); 
  } else if (key == ' ') {
    ImageRGBA_Ptr img = new ImageRGBA();
    g_RT->readBack(img->pixels());
    saveImage("readback.png",img);
  }
}

/* -------------------------------------------------------- */

void mainRender()
{
  /// draw in render target

  g_RT->bind();
  GPUHelpers::Renderer::setViewport(0,0,g_RT->w(),g_RT->h());

  // clear
  GPUHelpers::clearScreen(LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER,  1,0,0);
  // draw textured quad
  g_fxLevel .set (0.0f);
  g_fxTex   .set(g_Tex->texture());
  g_fxShader.begin();
  g_Quad   ->render();
  g_fxShader.end();
  
  g_RT->unbind();
  GPUHelpers::Renderer::setViewport(0,0,g_Width,g_Height);

  /// draw on screen

  // clear
  GPUHelpers::clearScreen(LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER,  0,0,1);

  g_fxLevel .set (4.0f);
  g_fxTex   .set(g_RT->texture());
  g_fxShader.begin();
  g_Quad   ->render();
  g_fxShader.end();
  
}

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  try {
    
    /// init simple UI (glut clone for both GL and D3D)
    SimpleUI::onRender     = mainRender;
    SimpleUI::onKeyPressed = mainKeyboard;
    SimpleUI::init(g_Width,g_Height);
    
    /// create a quad
    g_Quad = new QuadMesh();
    g_Quad->begin(GPUMESH_TRIANGLELIST);
    g_Quad->texcoord0_2(0.0f,0.0f);
    g_Quad->vertex_3(-1.0f,-1.0f,0);
    g_Quad->texcoord0_2(1.0f,0.0f);
    g_Quad->vertex_3(1.0f,-1.0f,0);
    g_Quad->texcoord0_2(1.0f,1.0f);
    g_Quad->vertex_3(1.0f,1.0f,0);
    g_Quad->texcoord0_2(0.0f,0.0f);
    g_Quad->vertex_3(-1.0f,-1.0f,0);
    g_Quad->texcoord0_2(1.0f,1.0f);
    g_Quad->vertex_3(1.0f,1.0f,0);
    g_Quad->texcoord0_2(0.0f,1.0f);
    g_Quad->vertex_3(-1.0f,1.0f,0);
    g_Quad->end();

    /// help
    printf("[q]     - quit\n");
    
    /// load a texture
    {
      ImageRGBA_Ptr image = loadImage<ImageRGBA>("media/image_test_rgba.png");
      g_Tex               = new Tex2DRGBA(image->pixels(),GPUTEX_AUTOGEN_MIPMAP);
    }

    /// create a render target
    g_RT = new RenderTarget2DRGBA(128,128,GPUTEX_AUTOGEN_MIPMAP);

    string code = " \
struct VS_OUTPUT \
{\
    float4 Pos   : POSITION; \
    float2 Tex0  : TEXCOORD0;\
};\
\
VS_OUTPUT RenderSceneVS( float4 vPos       : POSITION, \
                         float2 vTexCoord0 : TEXCOORD0)\
{\
    VS_OUTPUT Output;\
    Output.Pos  = float4(0.8*vPos.x,-0.8*vPos.y,0,1);\
    Output.Tex0 = vTexCoord0;\
    return Output;    \
}\
\
texture Tex;  \
sampler S_Tex = sampler_state { Texture = <Tex>; Filter = MIN_MAG_MIP_LINEAR; }; \
\
struct PS_OUTPUT\
{\
    float4 RGBColor : COLOR0;  \
};\
\
uniform float Level; \
\
PS_OUTPUT RenderScenePS( VS_OUTPUT In ) \
{ \
    PS_OUTPUT Output;\
    Output.RGBColor = tex2Dlod(S_Tex,float4(In.Tex0,0,Level));\
    return Output;\
}\
technique t_main\
{\
    pass P0\
    {          \
        VertexShader = compile vs_3_0 RenderSceneVS();\
        PixelShader  = compile ps_3_0 RenderScenePS();\
    }\
}\
technique10 t_main10\
{\
    pass P0\
    {\
        SetVertexShader  ( CompileShader( vs_4_0, RenderSceneVS( ) ) );\
        SetGeometryShader( NULL );\
        SetPixelShader   ( CompileShader( ps_4_0, RenderScenePS( ) ) );\
    }\
}\
";
    g_fxShader.init(code.c_str());
    
    g_fxTex   .init(g_fxShader,"Tex");
    g_fxLevel .init(g_fxShader,"Level");

    g_fxTex   .set (g_Tex->texture());
    g_fxLevel .set (0.0f);

    /// main loop
    SimpleUI::loop();

    // terminate shader
    g_fxShader.terminate();
    // erase autopointer (will free ressources)
    g_Tex = NULL; 
    g_RT  = NULL;
    // shutdown SimpleUI
    SimpleUI::shutdown();

  } catch (Fatal& e) {
    cerr << e.message() << endl;
    return (-1);
  }
  
  return (0);
}

/* -------------------------------------------------------- */
