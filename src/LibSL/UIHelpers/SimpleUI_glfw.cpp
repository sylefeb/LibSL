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
#include <emscripten/emscripten.h>
#endif
#include <GLFW/glfw3.h>

//---------------------------------------------------------------------------
// GLFW implementation
//---------------------------------------------------------------------------

GLFWwindow* glfw_window = nullptr; // CZ 2018-05-02 : ugly but required in loop and shutdown

void glfwError(int error, const char* description)
{
    fprintf(stderr, "GLFW error: %s\n", description);
}

uint glfw_to_LibSL_scancode(uint sc)
{
  switch (sc)
  {
  case GLFW_KEY_F1:             return LIBSL_KEY_F1;
  case GLFW_KEY_F2:             return LIBSL_KEY_F2;
  case GLFW_KEY_F3:             return LIBSL_KEY_F3;
  case GLFW_KEY_F4:             return LIBSL_KEY_F4;
  case GLFW_KEY_F5:             return LIBSL_KEY_F5;
  case GLFW_KEY_F6:             return LIBSL_KEY_F6;
  case GLFW_KEY_F7:             return LIBSL_KEY_F7;
  case GLFW_KEY_F8:             return LIBSL_KEY_F8;
  case GLFW_KEY_F9:             return LIBSL_KEY_F9;
  case GLFW_KEY_F10:            return LIBSL_KEY_F10;
  case GLFW_KEY_F11:            return LIBSL_KEY_F11;
  case GLFW_KEY_F12:            return LIBSL_KEY_F12;
  case GLFW_KEY_ENTER:          return LIBSL_KEY_ENTER;
  case GLFW_KEY_KP_ENTER:       return LIBSL_KEY_ENTER;
  case GLFW_KEY_UP:             return LIBSL_KEY_UP;
  case GLFW_KEY_RIGHT:          return LIBSL_KEY_RIGHT;
  case GLFW_KEY_LEFT:           return LIBSL_KEY_LEFT;
  case GLFW_KEY_DOWN:           return LIBSL_KEY_DOWN;
  case GLFW_KEY_PAGE_UP:        return LIBSL_KEY_PAGE_UP;
  case GLFW_KEY_PAGE_DOWN:      return LIBSL_KEY_PAGE_DOWN;
  case GLFW_KEY_HOME:           return LIBSL_KEY_HOME;
  case GLFW_KEY_END:            return LIBSL_KEY_END;
  case GLFW_KEY_INSERT:         return LIBSL_KEY_INSERT;
  case GLFW_KEY_DELETE:         return LIBSL_KEY_DELETE;
  case GLFW_KEY_LEFT_SHIFT:     return LIBSL_KEY_SHIFT;
  case GLFW_KEY_RIGHT_SHIFT:    return LIBSL_KEY_SHIFT;
  case GLFW_KEY_LEFT_CONTROL:   return LIBSL_KEY_CTRL;
  case GLFW_KEY_RIGHT_CONTROL:  return LIBSL_KEY_CTRL;
  case GLFW_KEY_LEFT_ALT:       return LIBSL_KEY_ALT;
  case GLFW_KEY_RIGHT_ALT:      return LIBSL_KEY_ALT;
  case GLFW_KEY_BACKSPACE:      return LIBSL_KEY_BK_SPACE;
  }

  return 0;
}

static void glfwMouseButton(GLFWwindow* window, int glfw_button, int glfw_action, int /*mods*/)
{
  // mod is a bitfield :
  //GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER

  double x, y;

  uint button=0, flags=0;
  // button
  if      (glfw_button == GLFW_MOUSE_BUTTON_LEFT)   button=LIBSL_LEFT_BUTTON;
  else if (glfw_button == GLFW_MOUSE_BUTTON_MIDDLE) button=LIBSL_MIDDLE_BUTTON;
  else if (glfw_button == GLFW_MOUSE_BUTTON_RIGHT)  button=LIBSL_RIGHT_BUTTON;
  // action
  if (glfw_action == GLFW_PRESS) {
      flags |= LIBSL_BUTTON_DOWN;
  } else if (glfw_action == GLFW_RELEASE) {
      flags |= LIBSL_BUTTON_UP;
  }
  glfwGetCursorPos(window, &x, &y); //CZ 2018-05-02 : warning, might have changed inbetween click and callback
  NAMESPACE::onMouseButtonPressed(std::floor(x),std::floor(y),button,flags);
}

static void glfwMouseWheel(GLFWwindow* window, double x, double y)
{
#ifdef EMSCRIPTEN
  NAMESPACE::onMouseWheel(-40 * y);
#else
  NAMESPACE::onMouseWheel(40 * y);
#endif
}

static void glfwMouseMove(GLFWwindow* window, double x, double y)
{
  NAMESPACE::onMouseMotion(std::floor(x),std::floor(y));
}

void glfwKeyboardText(GLFWwindow* window, unsigned int codepoint)
{
  NAMESPACE::onKeyPressed(codepoint);
}

static void glfwKeyboardSpecial(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  // mod is a bitfield :
  //GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER

  uint sc = glfw_to_LibSL_scancode(key);
  if (sc) {
    if (action == GLFW_PRESS){
      NAMESPACE::onScanCodePressed(sc);
    } else if (action == GLFW_RELEASE) {
      NAMESPACE::onScanCodeUnpressed(sc);
    } else if (action == GLFW_REPEAT) {
      NAMESPACE::onScanCodePressed(sc);
    }
  }  // SP 2019-02-06: This is already handled by glfwKeyboardText which accepts unicode chars
  /*else {
    const char* keyName = glfwGetKeyName(key, 0); // might be more than 1 char !
    if (keyName) {
      if (action == GLFW_PRESS){
        NAMESPACE::onKeyPressed(*keyName);
      } else if (action == GLFW_RELEASE) {
        NAMESPACE::onKeyUnpressed(*keyName);
      } else if (action == GLFW_REPEAT) {
        NAMESPACE::onKeyPressed(*keyName);
      }
    }
  }*/
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

  glfwSwapBuffers(glfw_window);
}

static void glfwReshape(GLFWwindow *window, int w, int h)
{
  NAMESPACE::onReshape(w,h);
}

static void glfwDragDrop(GLFWwindow *window, int nfiles, const char** files)
{
  NAMESPACE::onDragDrop(nfiles, files);
}

#ifndef EMSCRIPTEN
static void glfwClose(GLFWwindow* window)
{
  glfwSetWindowShouldClose(window, GLFW_TRUE);
}
#endif

static void glfwRefresh(GLFWwindow* window)
{
  glfwRender();
}

static void glfwMainLoop()
{
  if (s_AlwaysRefresh) {
    glfwPollEvents();
  } else {
    glfwWaitEvents();
  }
  glfwRender();
}

void NAMESPACE::init(uint width,uint height, const char *title,char **argv, int argc, bool frameLess, bool hidden, bool fullscreen)
{
  glfwInit();

#ifdef OPENGLCORE
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, LIBSL_OPENGL_MAJOR_VERSION);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, LIBSL_OPENGL_MINOR_VERSION);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
  // In case core profile fails.
  // From: https://www.glfw.org/docs/latest/window_guide.html#window_hints_ctx
  // While there is no way to ask the driver for a context of the highest
  // supported version, GLFW will attempt to provide this when you ask for
  // a version 1.0 context, which is the default for these hints.
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
#endif

  // set a default title
  if (title == NULL) {
    title = "LibSL::SimpleUI (GLFW)";
  }

  // window creation
  if (fullscreen) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfw_window = glfwCreateWindow(mode->width, mode->height, title, monitor, NULL);
  } else {
    if (frameLess) {
      glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    }
    glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);
  }
  if (!glfw_window) {
#ifdef OPENGLCORE
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!glfw_window) {
      return;
    }
#else
    return;
#endif
  }
  // window position
  glfwSetWindowPos(glfw_window, 0, 1);
  // opengl context
  glfwMakeContextCurrent(glfw_window);

  glfwSetErrorCallback(glfwError);
  glfwSetMouseButtonCallback(glfw_window, glfwMouseButton);
  glfwSetScrollCallback(glfw_window, glfwMouseWheel);
  glfwSetCursorPosCallback(glfw_window, glfwMouseMove);
  glfwSetCharCallback(glfw_window, glfwKeyboardText);
  glfwSetKeyCallback(glfw_window, glfwKeyboardSpecial);
  glfwSetWindowSizeCallback(glfw_window, glfwReshape);
  glfwSetDropCallback(glfw_window, glfwDragDrop);
#ifndef EMSCRIPTEN
  glfwSetWindowCloseCallback(glfw_window, glfwClose);
#endif
  glfwSetWindowRefreshCallback(glfw_window,glfwRefresh);
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

  if (hidden) {
    glfwHideWindow(glfw_window);
  } else {
    glfwShowWindow(glfw_window);
  }
}

void NAMESPACE::loop()
{
#ifdef EMSCRIPTEN
  emscripten_set_main_loop(glfwMainLoop,0,1);
#else
  while (!glfwWindowShouldClose(glfw_window))
  {
    glfwMainLoop();
  }
#endif
}

void NAMESPACE::shutdown()
{
  LibSL::GLHelpers::GLBasicPipeline::terminate();
  glfwDestroyWindow(glfw_window);
  glfwTerminate();
}

void NAMESPACE::exit()
{
  LibSL::GLHelpers::GLBasicPipeline::terminate();
  glfwDestroyWindow(glfw_window);
  glfwTerminate();
  ::exit (0);
}

void NAMESPACE::showCursor(bool show) {
  if (show) {
    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  } else {
    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  }
}

void NAMESPACE::glSwapBuffers()
{
  glfwSwapBuffers(glfw_window);
}

void NAMESPACE::glShowWindow(bool hide)
{
  if (hide) {
    glfwHideWindow(glfw_window);
  } else {
    glfwShowWindow(glfw_window);
  }
}
