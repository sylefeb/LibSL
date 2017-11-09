
#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>
LIBSL_WIN32_FIX
#include <AntTweakBar.h>

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

  TwDraw();
}

void mainMousePressed(uint x,uint y,uint button,uint flags)
{
  TwMouseAction   twa  = (flags == LIBSL_BUTTON_UP ? TW_MOUSE_RELEASED : TW_MOUSE_PRESSED);
  TwMouseButtonID twid = (button == LIBSL_LEFT_BUTTON ? TW_MOUSE_LEFT : ( button == LIBSL_RIGHT_BUTTON ? TW_MOUSE_RIGHT : TW_MOUSE_MIDDLE ) );
  TwMouseButton(twa,twid);
}

void mouseMotion(uint x,uint y)
{
  TwMouseMotion(x,y);
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
    ImageRGBA_Ptr img = loadImage<ImageRGBA>("media/image_test_rgba.png");
    g_Tex = new Tex2DRGBA( img->pixels() );

    // create a square
    g_Sq  = new Shapes::Square();

    // init AntTweakBar
    if( ! TwInit(TW_OPENGL,NULL) ) {
      throw Fatal("Cannot initialize AntTweakBar!");
    }
    TwWindowSize(600,600);

    // shader init
    g_Shader.init();
    g_Shader.makeTwBar();

    // enter the main loop
    SimpleUI::loop();

    // -> free the loaded texture
    g_Tex = NULL;
    // -> free shader
    g_Shader.terminate();
    // -> close the window
    SimpleUI::shutdown();

  } catch (Fatal& f) { // error handling
    std::cerr << Console::red << f.message() << Console::gray << std::endl;
  }

  return 0;
}


