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

int main(int argc,char **argv)
{
  // init openGL (with glut)
  glutInit(&argc, argv);
  glutInitWindowSize(640, 480);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
  glutCreateWindow("Sample - window for gl init");
  
  // init gluX
  gluxInit();
  
  cerr << "gluX does not stop if a loaded extension is not available." << endl;
  cerr << "  In this sample, GLUX_IS_AVAILABLE macro is used for fast testing " << endl
       << "  (extension availability is tested only once at startup)" << endl;
  if (GLUX_IS_AVAILABLE(GL_SUN_mesh_array))
    cerr << endl << "GL_SUN_mesh_array is available" << endl;
  else
    cerr << endl << "GL_SUN_mesh_array is *not* available" << endl;
  if (GLUX_IS_AVAILABLE(GL_NV_vertex_program))
    cerr << endl << "GL_NV_vertex_program is available" << endl;
  else
    cerr << endl << "GL_NV_vertex_program is *not* available" << endl;
  
  cerr << endl << "Done." << endl;
  return (0);
}
