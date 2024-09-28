
#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>

LIBSL_FILE_FORMATS

#include <imgui.h>
#include <LibSL/UIHelpers/BindImGui.h>

#include "test_sh_gl_ems.h"
AutoBindShader::test_sh_gl_ems g_Shader;
Tex2DRGBA_Ptr                  g_Tex;
AutoPtr<Shapes::Square>        g_Sq;

// 'mainRender' is called everytime the screen is drawn
void mainRender()
{
  clearScreen();

  g_Tex->bind();
  g_Shader.begin();
  g_Shader.u_Scale.set(1.0f);
  g_Shader.u_Image.set(0);
  g_Sq->render();
  g_Shader.end();

  // ImGUI
  ImGui::SetNextWindowSize(ImVec2(500, 180), ImGuiCond_Once);
  ImGui::Begin("Status");
  ImGui::End();
  SimpleUI::renderImGui();
}

void mainMousePressed(uint x,uint y,uint button,uint flags)
{
}

void mouseMotion(uint x,uint y)
{
}

// 'main' is the starting point of the application
int main(int argc,const char **argv)
{
  try { // error handling

    // opens a window
    SimpleUI::init(600,600,"autobindshader");
#ifdef __APPLE__
    glGetError();
#endif

    LIBSL_TRACE; LIBSL_GL_CHECK_ERROR;

    // set callbacks
    SimpleUI::onRender             = mainRender;
    SimpleUI::onMouseButtonPressed = mainMousePressed;
    SimpleUI::onMouseMotion        = mouseMotion;

    LIBSL_TRACE; LIBSL_GL_CHECK_ERROR;

    /// bind imgui
    SimpleUI::bindImGui();
    SimpleUI::initImGui();
    SimpleUI::onReshape(600, 600);

    LIBSL_TRACE; LIBSL_GL_CHECK_ERROR;

    // load an image into a texture
    ImageRGBA_Ptr img(new ImageRGBA(128,128));
    ForImage(img, i, j) {
      img->pixel(i, j) = v4b(i, j, 0, 255);
    }
    g_Tex = Tex2DRGBA_Ptr(new Tex2DRGBA( img->pixels() ));

    LIBSL_TRACE; LIBSL_GL_CHECK_ERROR;

    // create a square
    g_Sq  = AutoPtr<Shapes::Square>(new Shapes::Square());

    LIBSL_TRACE; LIBSL_GL_CHECK_ERROR;

    // shader init
    g_Shader.init();

    LIBSL_TRACE; LIBSL_GL_CHECK_ERROR;

    // enter the main loop
    SimpleUI::loop();

    // -> free ressources
    g_Tex = Tex2DRGBA_Ptr();
    g_Sq = AutoPtr<Shapes::Square>();
    g_Shader.terminate();

    // -> close the window
    SimpleUI::shutdown();

    return 0;

  } catch (Fatal& f) { // error handling
    std::cerr << Console::red << f.message() << Console::gray << std::endl;
    return -1;
  }
}
