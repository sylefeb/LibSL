#include <iostream>
using namespace std;

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glut.h>
#include <glux.h>

// declare extensions that you want to load
// in this sample we REQUIRE extensions that are only loaded in main -> REQUIRE override LOAD
GLUX_REQUIRE(GL_NV_vertex_program);
GLUX_REQUIRE(GL_SUN_mesh_array);

void aux()
{
  cerr << "GLUX_REQUIRE override GLUX_LOAD." << endl;
  if (GLUX_IS_AVAILABLE(GL_SUN_mesh_array))
    cerr << endl << "GL_SUN_mesh_array is available" << endl;
  else
    cerr << endl << "GL_SUN_mesh_array is *not* available" << endl;
  if (GLUX_IS_AVAILABLE(GL_NV_vertex_program))
    cerr << endl << "GL_NV_vertex_program is available" << endl;
  else
    cerr << endl << "GL_NV_vertex_program is *not* available" << endl;
  
  cerr << endl << "Done." << endl;
}
