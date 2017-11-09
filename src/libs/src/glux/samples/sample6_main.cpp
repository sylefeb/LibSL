#include <iostream>
using namespace std;

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glut.h>
#include <glux.h>

// declare extensions that you want to load
GLUX_LOAD(GL_NV_vertex_program);
GLUX_LOAD(GL_SUN_mesh_array);

void aux();

int main(int argc,char **argv)
{
  // init openGL (with glut)
  glutInit(&argc, argv);
  glutInitWindowSize(640, 480);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
  glutCreateWindow("Sample - window for gl init");
  
  cerr << "gluX support extension testing and loading in multiple files." << endl;
  cerr << "  A single GLUX_REQUIRE overrides all GLUX_LOAD applied to a same extension" << endl;

  // init gluX
  gluxInit();
  
  aux();

  cerr << endl << "Done." << endl;
  return (0);
}
