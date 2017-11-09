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
// Tutorial for GPUTex - 3D textures
// --------------------------------------------------------------
#include <iostream>
#include <ctime>
#include <cmath>
/* -------------------------------------------------------- */
#include <glux.h>
/* -------------------------------------------------------- */
#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>
/* -------------------------------------------------------- */
#include <GL/glut.h>
/* -------------------------------------------------------- */
using namespace std;
/* -------------------------------------------------------- */

int     g_iMainWindow;        // window glut id
int     g_iWinWidth=512;
int     g_iWinHeight=512;
double  g_dDist=8.0;
int     g_iOldX=-1;
int     g_iOldY=-1;
double  g_dRotX=90.0;
double  g_dRotZ=0.0;
bool    g_bStop=false;
bool    g_bWireframe=false;

Tex3DRGB_Ptr          g_Tex;

/* -------------------------------------------------------- */

void mainKeyboard(unsigned char key, int x, int y) 
{
  if (key == 'q')
    exit (0);
}

/* -------------------------------------------------------- */

void mainMouse(int btn, int state, int x, int y) 
{
  if (state == GLUT_DOWN) {
    g_iOldX=x;
    g_iOldY=y;
  }
}

/* -------------------------------------------------------- */

void mainMotion(int x,int y)
{
  g_dRotZ-=(g_iOldX-x)*360.0/800.0;
  g_dRotX-=(g_iOldY-y)*360.0/800.0;
  g_iOldX=x;
  g_iOldY=y;
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

void glQuad()
{
  glBegin(GL_QUADS);

  glTexCoord3f(0,1,0);
  glVertex2f(0,0);

  glTexCoord3f(0,0,0);
  glVertex2f(0,1);

  glTexCoord3f(1,0,0);
  glVertex2f(1,1);

  glTexCoord3f(1,1,0);
  glVertex2f(1,0);
  
  glEnd();
}

/* -------------------------------------------------------- */

void mainRender()
{
  // clear screen
  glViewport(0,0,g_iWinWidth,g_iWinHeight);
  
  glClearColor(0.5,0.5,0.5,0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0,1.0,0.001,10.0);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(3,3,3,
	    0.5,0.5,-0.5,
	    0,1,0);
  
  glBindTexture(GL_TEXTURE_3D,g_Tex->texture());

  static float agl=0;
  agl+=0.0001;
  agl=agl-floor(agl);

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glTranslatef(0,0,agl);
  glMatrixMode(GL_MODELVIEW);
  glTranslatef(0,0,-agl);

  glQuad();

  // swap buffers
  glutSwapBuffers();
}

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  try {

    srand(time(NULL));
    // GLUT
    glutInit(&argc, argv);
    glutInitWindowSize(g_iWinWidth,g_iWinHeight);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
    // main window
    g_iMainWindow=glutCreateWindow("GPUTex tutorial - 3D texture");
    glutMouseFunc(mainMouse);
    glutMotionFunc(mainMotion);
    glutKeyboardFunc(mainKeyboard);
    glutDisplayFunc(mainRender);
    glutReshapeFunc(mainReshape);
    glutSetWindow(g_iMainWindow);
    glutIdleFunc(mainIdle);
    
    // help
    printf("[q]     - quit\n");
    
    // glux
    gluxInit();

    // gl init
    glDisable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_3D);
    
    // create a 3D sphere texture
    Tex3DRGB::t_PixelArray sphere3d;
    sphere3d.allocate(16,16,16);
    ForArray3D(sphere3d,i,j,k) {
      v3f ctr=V3F(7.5f,7.5f,7.5f);
      v3f pt =V3F(i,j,k);
      if (length(ctr-pt) < 7.0f) {
        sphere3d.set(i,j,k)=uchar(length(ctr-pt)/7.0f*255);
      } else {
        sphere3d.set(i,j,k)=0;
      }
    }
    g_Tex=new Tex3DRGB(sphere3d,GPUTEX_AUTOGEN_MIPMAP);
    glBindTexture(GL_TEXTURE_3D,g_Tex->handle());
    
    // let's go
    glutMainLoop();
    
  } catch (Fatal& e) {
    cerr << e.message() << endl;
    return (-1);
  }
  
  return (0);
}

/* -------------------------------------------------------- */
