
#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>
LIBSL_WIN32_FIX

#include "test_sh.h"
AutoBindShader::test_sh g_Shader;
Tex2DRGBA_Ptr           g_Tex;
AutoPtr<Shapes::Square> g_Sq;

// 'mainRender' is called everytime the screen is drawn
void mainRender()
{
  clearScreen( );

  g_Tex->bind();
  g_Shader.begin();
  g_Shader.u_Image.set((GLuint)0);
  g_Sq->render();
  g_Shader.end(); 
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
    // set callbacks
    SimpleUI::onRender             = mainRender;
    SimpleUI::onMouseButtonPressed = mainMousePressed;
    SimpleUI::onMouseMotion        = mouseMotion;

    // load an image into a texture
    ImageRGBA_Ptr img(new ImageRGBA(128,128));
    ForImage(img, i, j) {
      img->pixel(i, j) = v4b(i, j, 0, 255);
    }
    g_Tex = Tex2DRGBA_Ptr(new Tex2DRGBA( img->pixels() ));

    // create a square
    g_Sq  = AutoPtr<Shapes::Square>(new Shapes::Square());

    // shader init
    g_Shader.init();

    // enter the main loop
    SimpleUI::loop();

    // -> free ressources
    g_Tex = Tex2DRGBA_Ptr();
    g_Sq = AutoPtr<Shapes::Square>();
    g_Shader.terminate();

    // -> close the window
    SimpleUI::shutdown();

  } catch (Fatal& f) { // error handling
    std::cerr << Console::red << f.message() << Console::gray << std::endl;
  }

  return 0;
}


