#include <iostream>
using namespace std;

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glut.h>
#include <glux.h>

// declare extensions that you want to load
// same as in sample5_main.cpp to show that there is no redefine problem
GLUX_LOAD(GL_NV_vertex_program);
GLUX_LOAD(GL_SUN_mesh_array);
GLUX_LOAD(GL_SGIX_depth_texture);

void aux()
{
  cerr << endl << "*-*-*-* sample5_aux.cpp *-*-*-*" << endl;

  if (GLUX_IS_AVAILABLE(GL_SUN_mesh_array))
    cerr << endl << "GL_SUN_mesh_array is available" << endl;
  else
    cerr << endl << "GL_SUN_mesh_array is *not* available" << endl;

  if (GLUX_IS_AVAILABLE(GL_NV_vertex_program))
    cerr << endl << "GL_NV_vertex_program is available" << endl;
  else
    cerr << endl << "GL_NV_vertex_program is *not* available" << endl;

  if (GLUX_IS_AVAILABLE(GL_SGIX_depth_texture))
    cerr << endl << "GL_SGIX_depth_texture is available" << endl;
  else
    cerr << endl << "GL_SGIX_depth_texture is *not* available" << endl;
  
}
