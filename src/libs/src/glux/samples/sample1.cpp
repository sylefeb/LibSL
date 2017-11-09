#include <iostream>
using namespace std;

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glut.h>
#include <glux.h>

// declare required extensions
GLUX_REQUIRE(GL_EXT_vertex_array);
GLUX_REQUIRE(GL_SUN_mesh_array);
GLUX_REQUIRE(GL_HP_occlusion_test);
GLUX_REQUIRE(GL_IBM_multimode_draw_arrays);
GLUX_REQUIRE(GL_SGI_color_matrix);
GLUX_REQUIRE(GL_NV_texture_shader);
GLUX_REQUIRE(WGL_NV_render_texture_rectangle);

int main(int argc,char **argv)
{
  // init openGL (with glut)
  glutInit(&argc, argv);
  glutInitWindowSize(640, 480);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
  glutCreateWindow("Sample - window for gl init");
  
  cerr << "gluX displays a list of required extensions with their status:" << endl;

  // init gluX
  gluxInit();
  
  cerr << "Done." << endl;
  return (0);
}
