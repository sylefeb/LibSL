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
#include <GL/glut.h>

#include <glux.h>

#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>

#include <Cg/cgGL.h>

using namespace std;

/* -------------------------------------------------------- */

int     g_iMainWindow;
int     g_iWinWidth =640;
int     g_iWinHeight=480;

float   g_Theta=-1.079922f;
float   g_Phy  = 0.196350f;

v3f     g_Camera,g_VDir;
float   g_VAngle=M_PI/2.0f;

/* -------------------------------------------------------- */

CgShader    g_cgSimple;
CgTechnique g_cgTechGround;
CgParameter g_cgModelviewProjection;
CgParameter g_cgBetaM,g_cgBetaR,g_cgG,g_cgESun;
Sky         g_Sky;

/* -------------------------------------------------------- */

typedef GPUMESH_MVF2(mvf_vertex_3f,mvf_normal_3f)   mvf_mesh;
typedef GPUMesh_Indexed_GL_VBO<mvf_mesh,uint> Mesh;

AutoPtr<Mesh> g_Mesh;

/* -------------------------------------------------------- */

AutoPtr<Mesh> meshFromHeightField(ImageRGB_Ptr img,float extent,float height)
{

  // vertices
  Array2D<v3f> pts(img->w(),img->h());
  ForIndex(j,img->h()) {
    ForIndex(i,img->w()) {
      float h  = height*float(img->pixel(i,j)[0])/255.0f;
      float fi = i/float(img->w() -1);
      float fj = j/float(img->h()-1);
      pts.at(i,j)=V3F((fi*2.0f-1.0f)*extent/2, 
		      (fj*2.0f-1.0f)*extent/2,
		      h);
    }
  }
  // normals
  Array2D<v3f> nrms(img->w(),img->h());
  ForIndex(j,img->h()-1) {
    ForIndex(i,img->w()-1) {
      v3f p00=pts.at(i,j);
      v3f p01=pts.at(i,j+1);
      v3f p11=pts.at(i+1,j+1);
      v3f p10=pts.at(i+1,j);
      v3f nA =cross(p01-p00,p11-p00);
      nA=normalize(nA);
      v3f nB =cross(p11-p00,p10-p00);
      nB=normalize(nB);
      nrms.at(i,j)=-(normalize(nA+nB));
    }
  }
  // send to mesh
  AutoPtr<Mesh> mesh=new Mesh();
  mesh->begin(GPUMESH_TRIANGLELIST);
  ForIndex(j,img->h()) {
    ForIndex(i,img->w()) {
      const v3f& n=nrms.at(i,j);
      const v3f& v=pts.at(i,j);
      mesh->normal(n[0],n[1],n[2]);
      mesh->vertex_3(v[0],v[1],v[2]);
    }
  }
  // triangles
  ForIndex(j,img->h()-1) {
    ForIndex(i,img->w()-1) {
      mesh->index( j   *img->w() + i  );
      mesh->index( j   *img->w() + i+1);
      mesh->index((j+1)*img->w() + i  );
      mesh->index( j   *img->w() + i+1);
      mesh->index((j+1)*img->w() + i+1);
      mesh->index((j+1)*img->w() + i  );
    }
  }
  mesh->end();
  // done
  return (mesh);
}

/* -------------------------------------------------------- */

void mainKeyboard(unsigned char key, int x, int y) 
{
  if (key == 'q')
    exit (0);
  else if (key == 'g')
    g_Theta+=M_PI/32.0;
  else if (key == 'j')
    g_Theta-=M_PI/32.0;
  else if (key == 'y')
    g_Phy+=M_PI/64.0;
  else if (key == 'h')
    g_Phy-=M_PI/64.0;
  else if (key == '+')
    g_Camera[2]+=100.0f;
  else if (key == '-')
    g_Camera[2]-=100.0f;
  else if (key == '8')
    g_Camera=g_Camera+100.0f*g_VDir;
  else if (key == '5')
    g_Camera=g_Camera-100.0f*g_VDir;
  else if (key == '4')
    g_VAngle+=M_PI/16.0f;
  else if (key == '6')
    g_VAngle-=M_PI/16.0f;
  else if (key == '0') {
    g_cgBetaR.set(4.1e-06,6.93327e-06,1.43768e-05);
    g_cgBetaM.set(4.3e-06,4.3e-06,4.3e-06);
    g_cgG.set(-0.94f);
  } else if (key == '1') {
    // light haze
    g_cgBetaR.set(6.95e-06f,1.18e-05f,2.44e-05f);
    g_cgBetaM.set(2e-05f,3e-05f,4e-05f);
    g_cgG.set(0.07f);
  } else if (key == '2') {
    // heavy haze
    g_cgBetaR.set(6.95e-06f,1.18e-05f,2.44e-05f);
    g_cgBetaM.set(8e-05,1e-04,1.2e-04);
    g_cgG.set(0.2f);
  } else if (key == '3') {
    // light fog
    g_cgBetaR.set(6.95e-06f,1.18e-05f,2.44e-05f);
    g_cgBetaM.set(4e-04,4e-04,4e-04);
    g_cgG.set(0.4f);
  } else if (key == '.') {
    // heavy fog
    g_cgBetaR.set(6.95e-06f,1.18e-05f,2.44e-05f);
    g_cgBetaM.set(1e-03,1e-03,1e-03);
    g_cgG.set(0.5f);
  } else if (key == ' ') {
    cerr << "camera = " << g_Camera << endl;
    cerr << "vangle = " << g_VAngle<< endl;
    cerr << sprint("sun theta = %f, phy = %f\n",g_Theta,g_Phy);
    ImageRGBA_Ptr scr=captureScreen();
    static uint n=0;
    saveImage(sprint("screen%04d.png",n++),scr);
  }
}

/* -------------------------------------------------------- */

void mainReshape(int w,int h)
{
  g_iWinWidth=w;
  g_iWinHeight=h;
  glutPostRedisplay();
}

/* -------------------------------------------------------- */

void mainIdle()
{
  glutPostRedisplay();
}
/* -------------------------------------------------------- */

void mainRender()
{
  glViewport(0,0,g_iWinWidth,g_iWinHeight);
	
  // clear screen
  glClearColor(0.5,0.5,0.5,0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
  // transform
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(70.0,g_iWinWidth/(double)g_iWinHeight,10.0,100000.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  v3f sundir=V3F(cos(g_Theta)*cos(g_Phy),sin(g_Theta)*cos(g_Phy),-sin(g_Phy));
  g_VDir = V3F(cos(g_VAngle),sin(g_VAngle),0);
  v3f at=g_Camera+1000.0f*g_VDir;
  gluLookAt(g_Camera[0],g_Camera[1],g_Camera[2],
	    at[0],at[1],at[2]-100.0,
	    0,0,1);

  // draw sky
  g_Sky.drawSky(g_Camera,sundir);

  // draw ground
  cgGLSetStateMatrixParameter(g_cgModelviewProjection.handle(),
			      CG_GL_MODELVIEW_PROJECTION_MATRIX,
			      CG_GL_MATRIX_IDENTITY);

  g_cgTechGround.activate();
  g_cgSimple.begin();
  g_Mesh->render();
  g_cgSimple.end();
  
 
  // swap buffers
  glutSwapBuffers();
}

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  try {

    // GLUT
    glutInit(&argc, argv);
    glutInitWindowSize(g_iWinWidth,g_iWinHeight);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    g_iMainWindow=glutCreateWindow("Nature::Sky tutorial");
    glutKeyboardFunc(mainKeyboard);
    glutDisplayFunc(mainRender);
    glutReshapeFunc(mainReshape);
    glutSetWindow(g_iMainWindow);
    glutIdleFunc(mainIdle);

    // gluX
    gluxInit();
    
    // GL
    glEnable(GL_DEPTH_TEST);
    
    // help
    printf("[q]           - quit\n");
    printf("[y,h,g,j]     - move sun\n");
    printf("[4,5,6,8,+,-] - move view\n");
    
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
  float3 Nrm   : NORMAL;   \
}; \
\
struct VS_OUTPUT \
{ \
  float4 Pos   : POSITION; \
  float3 Pos3D : TEXCOORD0;\
  float3 Nrm   : TEXCOORD1;\
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
  Out.Nrm   = In.Nrm; \
  return Out; \
} \
\
float4 fp_ground(VS_OUTPUT In) : COLOR0 \
{ \
  return (float4( \
     groundFromAtmosphere(In.Pos3D,Sky_CameraPos,Sky_SunDir,float3(0.5,1,0.6),In.Nrm) \
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

    // load terrain mesh
    {
      ImageRGB_Ptr img=loadImage<ImageRGB>("media/terrain.png");
      g_Mesh=meshFromHeightField(img,40000.0,3000.0);
    }

    g_cgBetaM.init(g_cgSimple,"Sky_BetaM");
    g_cgBetaR.init(g_cgSimple,"Sky_BetaR");
    g_cgG.init(g_cgSimple,"Sky_G");
    g_cgESun.init(g_cgSimple,"Sky_ESun");

    g_cgESun.set(20.0f,20.0f,20.0f);

    //g_Camera=V3F(0,-20000.0,1000.0);
    g_Camera=V3F(-17841.2f,-18698.9f,1000.0f);
    g_VAngle=0.785398f;
    g_VDir = V3F(cos(g_VAngle),sin(g_VAngle),0);
    g_Theta = -2.159845f;
    g_Phy = 0.392700f;

    // let's go
    glutMainLoop();

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
