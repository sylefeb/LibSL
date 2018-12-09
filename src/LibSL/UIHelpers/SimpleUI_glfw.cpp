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

//CZ 2018-05-02 : include opengl header before glfw if really needed

//#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> // includes appropriate opengl headers
//CZ 2018-05-02 : beware of exposed OpenGL version

//---------------------------------------------------------------------------
// GLFW implementation
//---------------------------------------------------------------------------

GLFWwindow* window = nullptr; //CZ 2018-05-02 : ugly but required in loop and shutdown

//CZ 2018-05-02 : basic temporary callbacks
void glfwError(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

uint glfw_to_LibSL_scancode(uint sc)
{
  switch (sc)
  {
  case GLFW_KEY_F1: return LIBSL_KEY_F1;
  case GLFW_KEY_F2: return LIBSL_KEY_F2;
  case GLFW_KEY_F3: return LIBSL_KEY_F3;
  case GLFW_KEY_F4: return LIBSL_KEY_F4;
  case GLFW_KEY_F5: return LIBSL_KEY_F5;
  case GLFW_KEY_F6: return LIBSL_KEY_F6;
  case GLFW_KEY_F7: return LIBSL_KEY_F7;
  case GLFW_KEY_F8: return LIBSL_KEY_F8;
  case GLFW_KEY_F9: return LIBSL_KEY_F9;
  case GLFW_KEY_F10: return LIBSL_KEY_F10;
  case GLFW_KEY_F11: return LIBSL_KEY_F11;
  case GLFW_KEY_F12: return LIBSL_KEY_F12;
      // case GLUT_RETURN: return LIBSL_KEY_ENTER;
  case GLFW_KEY_UP: return LIBSL_KEY_UP;
  case GLFW_KEY_RIGHT: return LIBSL_KEY_RIGHT;
  case GLFW_KEY_LEFT: return LIBSL_KEY_LEFT;
  case GLFW_KEY_DOWN: return LIBSL_KEY_DOWN;
  case GLFW_KEY_PAGE_UP: return LIBSL_KEY_PAGE_UP;
  case GLFW_KEY_PAGE_DOWN: return LIBSL_KEY_PAGE_DOWN;
  case GLFW_KEY_HOME: return LIBSL_KEY_HOME;
  case GLFW_KEY_END: return LIBSL_KEY_END;
  case GLFW_KEY_INSERT: return LIBSL_KEY_INSERT;
  case GLFW_KEY_DELETE: return LIBSL_KEY_DELETE;
  case GLFW_KEY_LEFT_SHIFT: return LIBSL_KEY_SHIFT;
  case GLFW_KEY_RIGHT_SHIFT: return LIBSL_KEY_SHIFT;
  case GLFW_KEY_LEFT_CONTROL: return LIBSL_KEY_CTRL;
  case GLFW_KEY_RIGHT_CONTROL: return LIBSL_KEY_CTRL;
  case GLFW_KEY_LEFT_ALT: return LIBSL_KEY_ALT;
  case GLFW_KEY_RIGHT_ALT: return LIBSL_KEY_ALT;
  case GLFW_KEY_BACKSPACE: return LIBSL_KEY_BK_SPACE;
  }

  return 0;
}

static void glfwMouseButton(GLFWwindow* window, int glfw_button, int glfw_action, int /*mods*/)
{
    // mod is a bitfield :
    //GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER

////CZ 2018-05-02 : TODO ?
//    if (  (glut_button & GLUT_LEFT_BUTTON)
//        &&  (glut_btn & GLUT_RIGHT_BUTTON) ) {
//        glut_btn = GLUT_MIDDLE_BUTTON;
//    }

    double x, y;
    glfwGetCursorPos(window, &x, &y); //CZ 2018-05-02 : warning, might have changed inbetween click and callback

    uint button=0, flags=0;

    if      (glfw_button == GLFW_MOUSE_BUTTON_LEFT)   button=LIBSL_LEFT_BUTTON;
    else if (glfw_button == GLFW_MOUSE_BUTTON_MIDDLE) button=LIBSL_MIDDLE_BUTTON;
    else if (glfw_button == GLFW_MOUSE_BUTTON_RIGHT)  button=LIBSL_RIGHT_BUTTON;
#ifdef EMSCRIPTEN
//CZ 2018-05-02 : TODO
    else if (glut_button == 3)  button = LIBSL_WHEEL_UP;
    else if (glut_button == 4)  button = LIBSL_WHEEL_DOWN;
#endif
    if (glfw_action == GLFW_PRESS) {
        flags |= LIBSL_BUTTON_DOWN;
    } else if (glfw_action == GLFW_RELEASE) {
        flags |= LIBSL_BUTTON_UP;
    }

    NAMESPACE::onMouseButtonPressed(std::floor(x),std::floor(y),button,flags);
}

static void glfwMouseMove(GLFWwindow* window, double x, double y)
{
    NAMESPACE::onMouseMotion(std::floor(x),std::floor(y));
}

static void glfwKeyboardSpecial(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // mod is a bitfield :
    //GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER

    uint sc = glfw_to_LibSL_scancode(key);
    if(sc>0) {
        if(action==GLFW_PRESS){
            NAMESPACE::onScanCodePressed(sc);
        } else if(action==GLFW_RELEASE) {
            NAMESPACE::onScanCodeUnpressed(sc);
        }
        //else{ what to do when we have GLFW_REPEAT }
    }
//    else {
//        const char* keyName = glfwGetKeyName(key, 0); // might be more than 1 char !
//        if(keyName) {
//            if(action==GLFW_PRESS){
//                NAMESPACE::onKeyPressed(*keyName);
//            } else if(action==GLFW_RELEASE) {
//                NAMESPACE::onKeyUnpressed(*keyName);
//            }
//            //else{ what to do when we have GLFW_REPEAT }
//        }
//    }
}

void glfwKeyboardText(GLFWwindow* window, unsigned int codepoint)
{
    //CZ 2018-05-03 : cannot distinguish between press and release
    //note that both callback for char and key are done when a key is pressed
    NAMESPACE::onKeyPressed(codepoint);
}

static void glfwRender()
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

  glfwSwapBuffers(window);
}

static void glfwReshape(GLFWwindow *window, int w, int h)
{
    NAMESPACE::onReshape(w,h);
}

#ifndef EMSCRIPTEN
static void glfwClose(GLFWwindow* window)
{
    //CZ 2018-05-02 : function not needed, could be useful to prevent direct close under some conditions
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}
#endif

static void glfwRefresh(GLFWwindow* window)
{
    glfwRender();
    glfwSwapBuffers(window);
}

//CZ 2018-05-02 : TODO
//void NAMESPACE::refresh()
//{
//  glutPostRedisplay();
//}

//#ifdef EMSCRIPTEN
//static void glutMouseWheel(int button, int dir, int x, int y)
//{
//  NAMESPACE::onMouseWheel(dir);
//}
//#endif

void NAMESPACE::showCursor(bool show) {
  if (show) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
  } else {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

  }
}

void NAMESPACE::init(uint width,uint height,const char *title,char **argv,int argc,bool frameLess,bool hidden,bool fullscreen)
{
  sl_assert(!hidden);    // not yet supported
  sl_assert(!frameLess); // not yet supported
  sl_assert(!fullscreen); // not yet supported

  glfwSetErrorCallback(glfwError);
  //CZ 2018-05-02 : basic temporary error callback

  glfwInit(); //CZ 2018-05-02 : cannot take additional parameters (&argc, argv) as was done in glutInit
  //CZ 2018-05-02 : hanlde init error ?

#ifndef OPENGLCORE
#error "we currently only allow core context with GLFW"
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, LIBSL_OPENGL_MAJOR_VERSION);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, LIBSL_OPENGL_MINOR_VERSION);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif

  glfwWindowHint(GLFW_DOUBLEBUFFER,GL_TRUE);
  //GLFW_RED_BITS, GLFW_GREEN_BITS, GLFW_BLUE_BITS, GLFW_ALPHA_BITS, GLFW_DEPTH_BITS and GLFW_STENCIL_BITS specify the desired bit depths of the various components of the default framebuffer. GLFW_DONT_CARE means the application has no preference.
  //http://www.glfw.org/docs/latest/window_guide.html

  /*GLFWwindow* */window = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!window)
  {
      // Window or OpenGL context creation failed
      return;
  }

  glfwSetWindowPos(window, 0, 0);

  glfwMakeContextCurrent(window);
  //CZ 2018-05-02 : load required GL extension here if needed

  // setting callbacks :

  glfwSetCursorPosCallback(window, glfwMouseMove);
  glfwSetMouseButtonCallback(window, glfwMouseButton);
  glfwSetKeyCallback(window, glfwKeyboardSpecial);
  glfwSetCharCallback(window, glfwKeyboardText);
  glfwSetWindowSizeCallback(window, glfwReshape);
#ifndef EMSCRIPTEN
  glfwSetWindowCloseCallback(window, glfwClose);
  //glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
#endif
  glfwSetWindowRefreshCallback(window,glfwRefresh);
//#ifdef EMSCRIPTEN
//  glutMouseWheelFunc    (glutMouseWheel);
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

  glEnable(GL_DEPTH_TEST);
}

void NAMESPACE::loop()
{
  while (!glfwWindowShouldClose(window))
  {
      glfwRender();

      if (s_AlwaysRefresh) {
          glfwPollEvents();
      } else {
          glfwWaitEvents();
//          glfwWaitEventsTimeout(0.7);
          //NB : wake up from other thread possible with glfwPostEmptyEvent();
      }
  }

}

void NAMESPACE::shutdown()
{
  LibSL::GLHelpers::GLBasicPipeline::terminate();

  glfwDestroyWindow(window);

  glfwTerminate();
}

void NAMESPACE::exit()
{
  NAMESPACE::shutdown();

  ::exit (0);
}

//void NAMESPACE::glSwapBuffers()
//{
//    assert(false);
//    //TODO
////  glutSwapBuffers();
//}

//void NAMESPACE::glShowWindow(bool hide)
//{
//    assert(false);
//    //TODO
////  if (hide)
////    glutHideWindow();
////  else
////    glutShowWindow();
//}
