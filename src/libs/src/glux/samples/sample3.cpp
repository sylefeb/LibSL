#include <iostream>
using namespace std;

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glut.h>
#include <glux.h>

int main(int argc,char **argv)
{
  // init openGL (with glut)
  glutInit(&argc, argv);
  glutInitWindowSize(640, 480);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
  glutCreateWindow("Sample - window for gl init");
  
  cerr << "Dummy test: calling an extension function without any initialisation ..." 
       << endl << endl;

  // an extension which is likely here
  glTexImage3DEXT(GL_TEXTURE_3D,0,0,0,0,0,0,0,0,0);

  cerr << "Extension has been automatically loaded" << endl;

  // an extension which is likely not here
  glDrawMeshArraysSUN(0,0,0,0);
  
  cerr << "This message is never displayed" << endl;
  
  return (0);
}
