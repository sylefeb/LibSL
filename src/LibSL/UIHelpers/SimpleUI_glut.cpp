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

#ifdef EMSCRIPTEN
#include <GL/glut.h>
#else
#include <GL/freeglut.h>
#endif

//---------------------------------------------------------------------------
// GLUT implementation
//---------------------------------------------------------------------------

uint glut_to_LibSL_scancode(uchar key,uint sc)
{
  //std::cout << "glut_to_LibSL_scancode k=" << (int)key << " sc=" << sc << std::endl;
  switch (sc)
  {
  case GLUT_KEY_F1: return LIBSL_KEY_F1;
  case GLUT_KEY_F2: return LIBSL_KEY_F2;
  case GLUT_KEY_F3: return LIBSL_KEY_F3;
  case GLUT_KEY_F4: return LIBSL_KEY_F4;
  case GLUT_KEY_F5: return LIBSL_KEY_F5;
  case GLUT_KEY_F6: return LIBSL_KEY_F6;
  case GLUT_KEY_F7: return LIBSL_KEY_F7;
  case GLUT_KEY_F8: return LIBSL_KEY_F8;
  case GLUT_KEY_F9: return LIBSL_KEY_F9;
  case GLUT_KEY_F10: return LIBSL_KEY_F10;
  case GLUT_KEY_F11: return LIBSL_KEY_F11;
  case GLUT_KEY_F12: return LIBSL_KEY_F12;
  // case GLUT_RETURN: return LIBSL_KEY_ENTER;
  case GLUT_KEY_UP: return LIBSL_KEY_UP;
  case GLUT_KEY_RIGHT: return LIBSL_KEY_RIGHT;
  case GLUT_KEY_LEFT: return LIBSL_KEY_LEFT;
  case GLUT_KEY_DOWN: return LIBSL_KEY_DOWN;
  case GLUT_KEY_PAGE_UP: return LIBSL_KEY_PAGE_UP;
  case GLUT_KEY_PAGE_DOWN: return LIBSL_KEY_PAGE_DOWN;
  case GLUT_KEY_HOME: return LIBSL_KEY_HOME;
  case GLUT_KEY_END: return LIBSL_KEY_END;
  case GLUT_KEY_INSERT: return LIBSL_KEY_INSERT;
#ifndef EMSCRIPTEN
  case GLUT_KEY_DELETE: return LIBSL_KEY_DELETE;
#endif
  case 112: return LIBSL_KEY_SHIFT;
  case 114: return LIBSL_KEY_CTRL;
  case 116: return LIBSL_KEY_ALT;
  case 120: return LIBSL_KEY_BK_SPACE;
  }
  switch (key)
  {
  case '\t': return LIBSL_KEY_TAB;
  case 27: return LIBSL_KEY_ESC;
  case 8: return LIBSL_KEY_BK_SPACE;
  }
  return 0;
}

static void glutMouse(int glut_btn, int glut_state, int x, int y)
{
  if (  (glut_btn & GLUT_LEFT_BUTTON)
    &&  (glut_btn & GLUT_RIGHT_BUTTON) ) {
    glut_btn = GLUT_MIDDLE_BUTTON;
  }

  uint button=0,flags=0;
  if      (glut_btn == GLUT_LEFT_BUTTON)   button=LIBSL_LEFT_BUTTON;
  else if (glut_btn == GLUT_MIDDLE_BUTTON) button=LIBSL_MIDDLE_BUTTON;
  else if (glut_btn == GLUT_RIGHT_BUTTON)  button=LIBSL_RIGHT_BUTTON;
  else if (glut_btn == 3)  button = LIBSL_WHEEL_UP;
  else if (glut_btn == 4)  button = LIBSL_WHEEL_DOWN;
  if (glut_state == GLUT_DOWN) {
    flags |= LIBSL_BUTTON_DOWN;
  } else if (glut_state == GLUT_UP) {
    flags |= LIBSL_BUTTON_UP;
  }

  if (button == LIBSL_WHEEL_UP) {
    NAMESPACE::onMouseWheel(1);
  } else if (button == LIBSL_WHEEL_DOWN) {
    NAMESPACE::onMouseWheel(-1);
  } else {
    NAMESPACE::onMouseButtonPressed(x,y,button,flags);
  }
}


static void glutMotion(int x,int y)
{
  NAMESPACE::onMouseMotion(x,y);
}

static void glutKeyboard(uchar k, int x, int y)
{
//  std::cout << "glutKeyboard '" << k << "'" << std::endl;
  uint sc = glut_to_LibSL_scancode(k,0);
  if (sc) {
    NAMESPACE::onScanCodePressed(sc);
  } else {
    NAMESPACE::onKeyPressed(k);
  }
}

static void glutKeyboardUp (uchar k, int x, int y)
{
  uint sc = glut_to_LibSL_scancode(k,0);
  if (sc) {
    NAMESPACE::onScanCodeUnpressed(sc);
  } else {
    NAMESPACE::onKeyUnpressed(k);
  }
}

static void glutKeyboardSpecial(int k, int x, int y)
{
//  std::cout << "glutKeyboardSpecial '" << k << "'" << std::endl;
  uint code = 0;
  code = glut_to_LibSL_scancode(0,k);
  NAMESPACE::onScanCodePressed(code);
}

static void glutKeyboardSpecialUp(int k, int x, int y)
{
  int code = 0;
  code = glut_to_LibSL_scancode(0,k);
  NAMESPACE::onScanCodeUnpressed(code = glut_to_LibSL_scancode(0,k));
}

static void glutRender()
{
  static double last = LibSL::System::Time::milliseconds();
  double now         = LibSL::System::Time::milliseconds();
  float  elapsed     = float(now - last);

  if (elapsed > 0) {
    NAMESPACE::onAnimate( now, elapsed );
    last = now;
  }

#ifdef EMSCRIPTEN
  LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->begin();
#endif

  NAMESPACE::onRender();

#ifdef EMSCRIPTEN
  LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->end();
#endif

  glutSwapBuffers();
}

static void glutReshape(int w,int h)
{
  NAMESPACE::onReshape(w,h);
}

static void glutIdle()
{
  if (s_AlwaysRefresh) {
    glutPostRedisplay();
  }
}

#ifndef EMSCRIPTEN
#ifdef FREEGLUT
static void glutClose()
{
  glutLeaveMainLoop();
}
#endif
#endif

void NAMESPACE::refresh()
{
  glutPostRedisplay();
}

void NAMESPACE::init(uint width,uint height,const char *title,char **argv,int argc,bool frameLess,bool hidden,bool fullscreen)
{
  sl_assert(!hidden);     // not supported
  sl_assert(!frameLess);  // not supported
  sl_assert(!fullscreen); // not supported

  glutInit              (&argc, argv);

#ifdef OPENGLCORE
  glutInitContextVersion(LIBSL_OPENGL_MAJOR_VERSION,LIBSL_OPENGL_MINOR_VERSION);
  glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
  glutInitContextProfile(GLUT_CORE_PROFILE);
#endif

  glutInitDisplayMode   (GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_ALPHA);
  glutInitWindowPosition(0,0);
  glutInitWindowSize    (width,height);
  glutCreateWindow      (title);
#ifndef EMSCRIPTEN
  std::cout << "\033]0;" << title << "\007";
  glutSetIconTitle      (title);
#endif
  glutDisplayFunc       (glutRender);
  glutIdleFunc          (glutIdle);
  glutMotionFunc        (glutMotion);
  glutPassiveMotionFunc (glutMotion);
  glutMouseFunc         (glutMouse);
  glutKeyboardFunc      (glutKeyboard);
  glutKeyboardUpFunc	(glutKeyboardUp);
  glutSpecialFunc       (glutKeyboardSpecial);
  glutSpecialUpFunc     (glutKeyboardSpecialUp);
  glutReshapeFunc       (glutReshape);
#ifndef EMSCRIPTEN
#ifdef FREEGLUT
  glutCloseFunc         (glutClose);
  glutSetOption         (GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
#endif
#endif
//#ifdef EMSCRIPTEN
//  glutMouseWheelFunc  (glutMouseWheel);
//#endif
#ifndef EMSCRIPTEN
#ifdef USE_GLUX
  gluxInit();
#endif
#endif

  LibSL::GLHelpers::GLBasicPipeline::init();
#ifdef EMSCRIPTEN
  LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->begin();
#endif

  glutReshape(width,height);

  glEnable(GL_DEPTH_TEST);
}

void NAMESPACE::loop()
{
  glutMainLoop();
}

void NAMESPACE::shutdown()
{
  LibSL::GLHelpers::GLBasicPipeline::terminate();
}

void NAMESPACE::exit()
{
  LibSL::GLHelpers::GLBasicPipeline::terminate();
  ::exit (0);
}

void NAMESPACE::showCursor(bool show) {
  if (show) {
    glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
  } else {
    glutSetCursor(GLUT_CURSOR_NONE);
  }
}

void NAMESPACE::glSwapBuffers()
{
  glutSwapBuffers();
}

void NAMESPACE::glShowWindow(bool hide)
{
  if (hide)
    glutHideWindow();
  else
    glutShowWindow();
}
