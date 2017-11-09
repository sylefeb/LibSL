#include <iostream>
using namespace std;

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glut.h>
#include <glux.h>

// declare GL_NV_vertex_program extension as required
GLUX_REQUIRE(GL_NV_vertex_program);
GLUX_REQUIRE(GL_ATI_fragment_shader);

int main(int argc,char **argv)
{
  GLuint vprog;
  
  // init openGL (with glut)
  glutInit(&argc, argv);
  glutInitWindowSize(640, 480);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
  glutCreateWindow("Sample - window for gl init");
  
  // init gluX
  gluxInit();
  
  // if we are here, GL_NV_vertex_program is available !
  // gluX has already stopped the execution if one of the required extensions
  // is not available.

  // let's call some vertex program stuff (just for example)
  cerr << "Calling glGenProgramsNV ..." << endl;
  glGenProgramsNV(1,&vprog);
  cerr << "Calling glBindProgramNV ..." << endl;
  glBindProgramNV(GL_VERTEX_PROGRAM_NV, vprog);
  cerr << "Done." << endl;
  return (0);
}
