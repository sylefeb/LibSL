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
// Tutorial for Nature::Sky
//
// Demonstrate how to add an object with aerial perspective
// --------------------------------------------------------------

#include <iostream>
#include <ctime>
#include <cmath>

#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>

#include <Cg/CgGL.h>

using namespace std;

/* -------------------------------------------------------- */

int     g_Width  = 640;
int     g_Height = 480;

float   g_Theta  = -1.079922f;
float   g_Phy    =  0.196350f;

/* -------------------------------------------------------- */

CgShader    g_cgSimple;
CgTechnique g_cgTechGround;
CgParameter g_cgModelviewProjection;
Sky         g_Sky;

/* -------------------------------------------------------- */

void mainKeyboard(uchar key) 
{
  if (key == 'q')
    exit (0);
  else if (key == 'g')
    g_Theta+=M_PI/128.0;
  else if (key == 'j')
    g_Theta-=M_PI/128.0;
  else if (key == 'y')
    g_Phy+=M_PI/128.0;
  else if (key == 'h')
    g_Phy-=M_PI/128.0;
  cerr << sprint("theta = %f phy = %f\n",g_Theta,g_Phy);
}

/* -------------------------------------------------------- */

void mainReshape(uint w,uint h)
{
  g_Width  = w;
  g_Height = h;
}

/* -------------------------------------------------------- */

void mainRender()
{
  glViewport(0,0,g_Width,g_Height);
	
  // clear screen
  glClearColor(0.5,0.5,0.5,0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
  // transform
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(70.0f,g_Width/(float)g_Height,10.0f,6300000.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  v3f camera = V3F(0,0,10.0);
  v3f sundir = V3F(cos(g_Theta)*cos(g_Phy),sin(g_Theta)*cos(g_Phy),-sin(g_Phy));

  gluLookAt(camera[0],camera[1],camera[2],
	    0,1000.0,0,
	    0,0,1);

  // draw sky
  g_Sky.drawSky(camera,sundir);

  // draw ground
  cgGLSetStateMatrixParameter(g_cgModelviewProjection.handle(),
			      CG_GL_MODELVIEW_PROJECTION_MATRIX,
			      CG_GL_MATRIX_IDENTITY);

  g_cgTechGround.activate();
  g_cgSimple.begin();
  float sz = 5000.0f;        // 5 km
  static float y  = 10000.0f; // dist from camera goes from 10 km to 1000 km
  y += y*0.005f;
  if (y > 1000000.0) {
    y=10000.0f;
  }
  //  if (int(y/1000.0f) % 10 == 0) {
  //    cerr << (y/1000.0f) << "km\n";
  //  }
  glBegin(GL_QUADS);
  glVertex3d(-sz, y,-sz);
  glVertex3d( sz, y,-sz);
  glVertex3d( sz, y, sz);
  glVertex3d(-sz, y, sz);
  glEnd();
  g_cgSimple.end();
  
}

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  try {

    SimpleUI::onRender     = mainRender;
    SimpleUI::onKeyPressed = mainKeyboard;
    SimpleUI::onReshape    = mainReshape;
    SimpleUI::init(g_Width,g_Height,"Nature::Sky tutorial");
    
    // help
    printf("[q]       - quit\n");
    printf("[y,h,g,j] - move sun\n");
    
    // shader code
    //
    // Uses the Nature::Sky library to take into account
    // the atmosphere between the eye and the object
    //
    // Note that Sky_CameraPos and Sky_SunDir directly
    // correspond to the parameters of Sky::drawSky(...)
    string code= " \
\
struct VS_INPUT \
{ \
  float4 Pos   : POSITION; \
}; \
\
struct VS_OUTPUT \
{ \
  float4 Pos   : POSITION; \
  float3 Pos3D : TEXCOORD0;\
}; \
\
uniform float4x4 ModelviewProjection_GL_MVP; \
\
VS_OUTPUT vp_ground(VS_INPUT In) \
{ \
  VS_OUTPUT Out; \
\
  Out.Pos   = mul(ModelviewProjection_GL_MVP,In.Pos); \
  Out.Pos3D = In.Pos.xyz; \
  return Out; \
} \
\
float4 fp_ground(VS_OUTPUT In) : COLOR0 \
{ \
  return (float4( \
     groundFromAtmosphere(In.Pos3D,Sky_CameraPos,Sky_SunDir,float3(0.3,0.8,0.1)) \
  ,1)); \
} \
\
technique t_ground { \
  pass P0 { \
    VertexProgram = compile vp40 vp_ground(); \
    FragmentProgram = compile fp40 fp_ground(); \
  } \
} \
";
    // add sky code to shader code
    g_Sky.addTo(code);
    // init shader
    g_cgSimple.init(code.c_str());
    // bind sky to this shader
    g_Sky.bindTo(g_cgSimple);

    g_cgTechGround.init(g_cgSimple,"t_ground");
    g_cgModelviewProjection.init(g_cgSimple,"ModelviewProjection_GL_MVP");

    SimpleUI::loop();

    SimpleUI::shutdown();

  } catch (CgException& e) {
    cerr << e.message() << endl;
    return (-1);
  } catch (Fatal& e) {
    cerr << e.message() << endl;
    return (-1);
  }

  return (0);
}

/* -------------------------------------------------------- */
