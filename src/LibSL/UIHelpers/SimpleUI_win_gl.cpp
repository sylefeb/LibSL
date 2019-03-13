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

//---------------------------------------------------------------------------
// Windows OpenGL implementation - native
//---------------------------------------------------------------------------

static HWND  s_hWnd = NULL;
static HDC   s_hDC  = NULL;
static HGLRC s_hRC  = NULL;

static void SimpleUI_gl_Render()
{
  static time_t last = LibSL::System::Time::milliseconds();
  time_t now         = LibSL::System::Time::milliseconds();
  time_t elapsed     = (now - last);

  if (elapsed > 0) {
    NAMESPACE::onAnimate( (double)now, (float)elapsed );
    last = now;
  }

  NAMESPACE::onRender();

}

uint win32_VirtualKey_to_LibSL_scancode(uint vk)
{
  switch (vk)
  {
  case VK_F1:       return LIBSL_KEY_F1;
  case VK_F2:       return LIBSL_KEY_F2;
  case VK_F3:       return LIBSL_KEY_F3;
  case VK_F4:       return LIBSL_KEY_F4;
  case VK_F5:       return LIBSL_KEY_F5;
  case VK_F6:       return LIBSL_KEY_F6;
  case VK_F7:       return LIBSL_KEY_F7;
  case VK_F8:       return LIBSL_KEY_F8;
  case VK_F9:       return LIBSL_KEY_F9;
  case VK_F10:      return LIBSL_KEY_F10;
  case VK_F11:      return LIBSL_KEY_F11;
  case VK_F12:      return LIBSL_KEY_F12;
  case VK_RETURN:   return LIBSL_KEY_ENTER;
  case VK_UP:       return LIBSL_KEY_UP;
  case VK_RIGHT:    return LIBSL_KEY_RIGHT;
  case VK_LEFT:     return LIBSL_KEY_LEFT;
  case VK_DOWN:     return LIBSL_KEY_DOWN;
  case VK_PRIOR:    return LIBSL_KEY_PAGE_UP;
  case VK_NEXT:     return LIBSL_KEY_PAGE_DOWN;
  case VK_HOME:     return LIBSL_KEY_HOME;
  case VK_END:      return LIBSL_KEY_END;
  case VK_INSERT:   return LIBSL_KEY_INSERT;
  case VK_CONTROL:  return LIBSL_KEY_CTRL;
  case VK_RMENU:    return LIBSL_KEY_ALT;
  case VK_LMENU:    return LIBSL_KEY_ALT;
  case VK_SHIFT:    return LIBSL_KEY_SHIFT;
  case VK_TAB:      return LIBSL_KEY_TAB;
  case VK_BACK:     return LIBSL_KEY_BK_SPACE;
  case VK_ESCAPE:   return LIBSL_KEY_ESC;
  case VK_DELETE:   return LIBSL_KEY_DELETE;
  }
  return 0;
}


static LRESULT CALLBACK SimpleUI_gl_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  if (SimpleUI_Custom_WndProc != NULL) {
    if (SimpleUI_Custom_WndProc(hWnd,message,wParam,lParam)) {
      return 0;
    }
  }
  switch (message)
  {
  case WM_ERASEBKGND:
  {
    // Do nothing = do not erase the background
  } return 0;

  case WM_PAINT:
  {
    // FC: adding missing BeginPaint/EndPaint
    // Windows continuously sends WM_PAINT messages if the WM_PAINT handler doesn't call BeginPaint/EndPaint,
    // eg. WM_PAINT messages are continuously transiting through the message loop and PeekMessage always returns TRUE.
    // This causes the code supposed to refresh the window ( if (s_AlwaysRefresh) ) to _never_ be called, but the window
    // is still continuously being repainted as WM_PAINT messages are processed.
    // Not calling BeginPaint/EndPaint causes some issues on my system on startup.
    // Calling GetUpdateRect is required as per strictly mentionned in the WM_PAINT handling spec.

    if (GetUpdateRect(hWnd, NULL, FALSE)) /// SL: test
    {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps);
      SimpleUI_gl_Render();
      SwapBuffers(s_hDC);
      EndPaint(hWnd, &ps);
    }
  } // else {
    //  return DefWindowProc(hWnd, message, wParam, lParam);
    // }
  return 0;
  case WM_CREATE:
  {
  } return 0;

  case WM_CLOSE:
  {
    PostQuitMessage(0);
  } return 0;

  case WM_DESTROY:
  {
  } return 0;

  case WM_KEYDOWN:
  {
    uint scancode = (lParam >> 16) & 0xFF;

    uint vk = MapVirtualKey(scancode, MAPVK_VSC_TO_VK);
    uint lsc = win32_VirtualKey_to_LibSL_scancode(vk);

    BYTE keyboardState[256];
    GetKeyboardState(keyboardState);
    char charvalue[2];
    int toasc = ToAsciiEx((uint)wParam, scancode, keyboardState, (LPWORD)&charvalue[0], 0, GetKeyboardLayout(0));
    if (toasc == 1 && charvalue[0] != 8 /*backspace*/) {
      //std::cerr << "charvalue = " << (int)charvalue[0] << std::endl;
      NAMESPACE::onKeyPressed(charvalue[0]);
    } else if (lsc > 0) {
      //std::cerr << "scancode = " << lsc << std::endl;
      NAMESPACE::onScanCodePressed(lsc);
    }
  } return 0;

  case WM_KEYUP:
  {
    uint scancode = (lParam >> 16) & 0xFF;
    uint vk = MapVirtualKey(scancode, MAPVK_VSC_TO_VK);
    uint lsc = win32_VirtualKey_to_LibSL_scancode(vk);
    BYTE keyboardState[256];
    GetKeyboardState(keyboardState);
    char charvalue[2];
    int toasc = ToAsciiEx((uint)wParam, scancode, keyboardState, (LPWORD)&charvalue[0], 0, GetKeyboardLayout(0));
    if (toasc == 1 && charvalue[0] != 8 /*backspace*/) {
      //std::cerr << "charvalue = " << (int)charvalue[0] << std::endl;
      NAMESPACE::onKeyUnpressed(charvalue[0]);
    } else if (lsc > 0) {
      //std::cerr << "scancode = " << lsc << std::endl;
      NAMESPACE::onScanCodeUnpressed(lsc);
    }
  } return 0;

  case WM_MOUSEMOVE:
  {
    int xPos = LOWORD(lParam);
    int yPos = HIWORD(lParam);
    NAMESPACE::onMouseMotion(xPos,yPos);
  } return 0;

  case WM_LBUTTONDOWN:
  {
    int xPos = LOWORD(lParam);
    int yPos = HIWORD(lParam);
    NAMESPACE::onMouseButtonPressed(xPos,yPos,LIBSL_LEFT_BUTTON,LIBSL_BUTTON_DOWN);
  } return 0;

  case WM_LBUTTONUP:
  {
    int xPos = LOWORD(lParam);
    int yPos = HIWORD(lParam);
    NAMESPACE::onMouseButtonPressed(xPos,yPos,LIBSL_LEFT_BUTTON,LIBSL_BUTTON_UP);
  } return 0;

  case WM_MBUTTONDOWN:
  {
    int xPos = LOWORD(lParam);
    int yPos = HIWORD(lParam);
    NAMESPACE::onMouseButtonPressed(xPos, yPos, LIBSL_MIDDLE_BUTTON, LIBSL_BUTTON_DOWN);
  } return 0;

  case WM_MBUTTONUP:
  {
    int xPos = LOWORD(lParam);
    int yPos = HIWORD(lParam);
    NAMESPACE::onMouseButtonPressed(xPos,yPos,LIBSL_MIDDLE_BUTTON,LIBSL_BUTTON_UP);
  } return 0;

  case WM_RBUTTONDOWN:
  {
    int xPos = LOWORD(lParam);
    int yPos = HIWORD(lParam);
    NAMESPACE::onMouseButtonPressed(xPos,yPos,LIBSL_RIGHT_BUTTON,LIBSL_BUTTON_DOWN);
  } return 0;

  case WM_RBUTTONUP:
  {
    int xPos = LOWORD(lParam);
    int yPos = HIWORD(lParam);
    NAMESPACE::onMouseButtonPressed(xPos,yPos,LIBSL_RIGHT_BUTTON,LIBSL_BUTTON_UP);
  } return 0;

  case WM_MOUSEWHEEL:
  {
    NAMESPACE::onMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
  } return 0;

  case WM_SIZE:
  {
    s_ScreenW = LOWORD(lParam);
    s_ScreenH = HIWORD(lParam);
    NAMESPACE::onReshape(s_ScreenW, s_ScreenH);
  } return 0;

  case WM_DROPFILES:
  {
    HDROP drop = HDROP(wParam);
    uint nfiles = DragQueryFile(drop, 0xFFFFFFFF, NULL, 0);
    char** files = new char*[nfiles];
    // fill in the file names
    for (uint i = 0; i < nfiles; i++) {
      WCHAR file[MAX_PATH];
      DragQueryFile(drop, i, file, MAX_PATH);
      files[i] = new char[MAX_PATH];
      size_t nchar = 0;
      wcstombs_s(&nchar, files[i], MAX_PATH, file, _TRUNCATE);
    }
    // pass on to simpleui
    NAMESPACE::onDragDrop(nfiles, (const char**)files);
    // release file names and everything else
    for (uint i = 0; i < nfiles; i++) { delete files[i]; }
    DragFinish(drop);
    delete files;
  } return 0;

  default:
    return DefWindowProc( hWnd, message, wParam, lParam );
  }
}

// Enable OpenGL
static bool enableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC)
{
  PIXELFORMATDESCRIPTOR pfd;
  int format;

  // get the device context (DC)
  *hDC = GetDC( hWnd );
  if (!hDC) {
    return false;
  }

  // set the pixel format for the DC
  ZeroMemory( &pfd, sizeof( pfd ) );
  pfd.nSize        = sizeof( pfd );
  pfd.nVersion     = 1;
  pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType   = PFD_TYPE_RGBA;
  pfd.cColorBits   = 24;
  pfd.cDepthBits   = 24;
  pfd.cStencilBits = 8;
  pfd.cAlphaBits   = 8;
  pfd.iLayerType = PFD_MAIN_PLANE;
  format = ChoosePixelFormat( *hDC, &pfd );
  if (!format) {
    return false;
  }
  SetPixelFormat( *hDC, format, &pfd );

  // create and enable the render context (RC)
  *hRC = wglCreateContext( *hDC );
  if (!*hRC) {
    return false;
  }

  wglMakeCurrent( *hDC, *hRC );

  return true;
}

#ifdef OPENGLCORE

// see https://mariuszbartosik.com/opengl-4-x-initialization-in-windows-without-a-framework/
static bool enabelCoreProfile(HWND * hWnd, HDC * hDC, HGLRC * hRC, HWND hWndCP)
{
  PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));

  const int pixelAttribs[] = {
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
    WGL_COLOR_BITS_ARB, 24,
    WGL_ALPHA_BITS_ARB, 8,
    WGL_DEPTH_BITS_ARB, 24,
    WGL_STENCIL_BITS_ARB, 8,
    0
  };
  int contextAttributes[] = {
    WGL_CONTEXT_MAJOR_VERSION_ARB, LIBSL_OPENGL_MAJOR_VERSION,
    WGL_CONTEXT_MINOR_VERSION_ARB, LIBSL_OPENGL_MINOR_VERSION,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
    0 };

  HDC hDCCP = GetDC(hWndCP);
  PIXELFORMATDESCRIPTOR PFD;
  int pixelFormatID; UINT numFormats;
  BOOL status = wglChoosePixelFormatARB(hDCCP, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats);
  DescribePixelFormat(hDCCP, pixelFormatID, sizeof(PFD), &PFD);
  SetPixelFormat(hDCCP, pixelFormatID, &PFD);
  HGLRC hRCCP = wglCreateContextAttribsARB(hDCCP, NULL, contextAttributes);

  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(*hRC);
  ReleaseDC(*hWnd, *hDC);
  DestroyWindow(*hWnd);

  if (!wglMakeCurrent(hDCCP, hRCCP) || hRCCP == NULL) {
    wglDeleteContext(hRCCP);
    ReleaseDC(hWndCP, hDCCP);
    *hWnd = hWndCP;
    return false;
  }

  *hWnd = hWndCP;
  *hDC = hDCCP;
  *hRC = hRCCP;
  return true;
}

#endif

// Disable OpenGL
static void disableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent( NULL, NULL );
    wglDeleteContext( hRC );
    ReleaseDC( hWnd, hDC );
}

HMONITOR      g_HMonitors[16];
MONITORINFOEX g_Monitors[16];
uint          g_NumMonitors = 0;

BOOL CALLBACK monitorEnumProc(
   HMONITOR hMonitor,
   HDC hdcMonitor,
   LPRECT lprcMonitor,
   LPARAM dwData
)
{
  sl_assert( g_NumMonitors < 16 );
  g_HMonitors[ g_NumMonitors ]        = hMonitor;
  g_Monitors [ g_NumMonitors ].cbSize = sizeof(MONITORINFOEX);
  GetMonitorInfo( hMonitor, &g_Monitors[ g_NumMonitors ] );
  g_NumMonitors ++;
  return TRUE;
}

void NAMESPACE::init(
  uint width, uint height,
  const char *title,
  char **argv, int argc,
  bool frameLess,
  bool hidden,
  bool fullscreen)
{
  WNDCLASS wc;

  // register window class
  HINSTANCE hInstance = GetModuleHandle(NULL);
  wc.style = CS_OWNDC;
  wc.lpfnWndProc = SimpleUI_gl_WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = frameLess ? NULL : LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = L"SimpleUI::GL";
  RegisterClass(&wc);

  RECT rc;
  rc.top = 0; rc.left = 0;
  rc.right = width; rc.bottom = height;

  if (fullscreen) { // Full screen from NeHe tutorial

    g_NumMonitors = 0;
    EnumDisplayMonitors(NULL, NULL, monitorEnumProc, NULL);
    ForIndex(i, g_NumMonitors) {
      if (!(g_Monitors[i].dwFlags & MONITORINFOF_PRIMARY)) {
        // prefer secondary monitor
        rc = g_Monitors[i].rcMonitor;
        std::cerr << sprint("[SimpleUI] selected monitor %ls\n", g_Monitors[i].szDevice);
        std::cerr << sprint("[SimpleUI] %d %d %d %d\n", g_Monitors[i].rcMonitor.left, g_Monitors[i].rcMonitor.right, g_Monitors[i].rcMonitor.top, g_Monitors[i].rcMonitor.bottom);
        break;
      }
    }

    /*
    DEVMODE dmScreenSettings;								// Device Mode
    memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
    dmScreenSettings.dmSize=sizeof(dmScreenSettings);		  // Size Of The Devmode Structure
    dmScreenSettings.dmPelsWidth	= width;				        // Selected Screen Width
    dmScreenSettings.dmPelsHeight	= height;				        // Selected Screen Height
    dmScreenSettings.dmBitsPerPel	= 32;	          				// Selected Bits Per Pixel
    dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
    if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
      // failed
      throw Fatal("Cannot change display settings to fullscreen.");
    }
    */
  }

  DWORD style = 0;
  DWORD exStyle = 0;
  if (fullscreen) {
    style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;	// Windows Style
    exStyle = WS_EX_APPWINDOW | WS_EX_ACCEPTFILES;				// Window Extended Style
    // ShowCursor(FALSE);	// Hide Mouse Pointer
  } else {
    // create main window
    style = frameLess ? (WS_POPUP) : (WS_CAPTION | WS_POPUPWINDOW | /*WS_BORDER |*/ WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
    exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES;
  }
  style |= CS_OWNDC;

  RECT before = rc;
  AdjustWindowRectEx(&rc, style, false, exStyle);
  rc.bottom += (before.top - rc.top);
  rc.right += (before.left - rc.left);
  rc.top = before.top;
  rc.left = before.left;

  s_hWnd = CreateWindowEx(
    exStyle,
    L"SimpleUI::GL", toUnicode(title),
    style,
    rc.left, rc.top,
    rc.right - rc.left, rc.bottom - rc.top,
    NULL, NULL, hInstance, NULL);

  if (!s_hWnd) {
    // failed
    ChangeDisplaySettings(NULL, 0);
    throw Fatal("Cannot change display settings to fullscreen.");
  }

  // enable OpenGL for the window
  bool success = enableOpenGL(s_hWnd, &s_hDC, &s_hRC);
  if (!success) {
    // failed
    ChangeDisplaySettings(NULL, 0);
    throw Fatal("Cannot enable opengl.");
  }

#ifdef OPENGLCORE
  HWND hWnd = CreateWindowEx(
    exStyle,
    L"SimpleUI::GL", toUnicode(title),
    style, rc.left, rc.top,
    rc.right - rc.left, rc.bottom - rc.top,
    NULL, NULL, hInstance, NULL);
  success = enabelCoreProfile(&s_hWnd, &s_hDC, &s_hRC, hWnd);
  if (!success) {
    // failed core-profile intialization, retry with compatibility mode
    success = enableOpenGL(s_hWnd, &s_hDC, &s_hRC);
    if (!success) {
      ChangeDisplaySettings(NULL, 0);
      throw Fatal("Cannot enable opengl.");
    }
  }
#endif

  s_FullScreen = fullscreen;

#ifndef EMSCRIPTEN
#ifdef USE_GLUX
  gluxInit();
#endif
#endif

  NAMESPACE::onReshape(width,height);

  LibSL::GLHelpers::GLBasicPipeline::init();

  glEnable(GL_DEPTH_TEST);

}

void NAMESPACE::refresh()
{
  InvalidateRect(s_hWnd, NULL, FALSE);
  UpdateWindow(s_hWnd);
}

void NAMESPACE::loop()
{
  // show window
  ShowWindow(s_hWnd,SW_SHOW);
  // program main loop
  bool quit = false;
    while ( !quit ) {
        // check for messages
      MSG msg;
    if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )  ) {
            // handle or dispatch messages
      if ( msg.message == WM_QUIT ) {
                quit = true;
            } else {
                TranslateMessage( &msg );
                DispatchMessage ( &msg );
            }
        } else {
      if (s_AlwaysRefresh) { // this code is currently never executed (see the way WM_PAINT is handled in this file). Change WM_PAINT handler to fix (and match Linux/Glut implementation)
        NAMESPACE::refresh();
      }
        }
    }

}

void NAMESPACE::shutdown()
{
  LibSL::GLHelpers::GLBasicPipeline::terminate();

  // shutdown OpenGL
    disableOpenGL( s_hWnd, s_hDC, s_hRC );
    // destroy the window explicitly
    DestroyWindow( s_hWnd );

  if (s_FullScreen) {
    // restore display
    ChangeDisplaySettings(NULL,0);
    s_FullScreen = false;
  }
}

void NAMESPACE::exit()
{
  PostQuitMessage(0);
}

void *NAMESPACE::glContext()
{
  return (s_hRC);
}

void NAMESPACE::glMakeCurrent()
{
  wglMakeCurrent( s_hDC, s_hRC );
}

void  NAMESPACE::glShowWindow(bool hide)
{
  ShowWindow(s_hWnd,hide ? SW_HIDE : SW_SHOW);
}

void  NAMESPACE::glSwapBuffers()
{
  wglMakeCurrent( s_hDC, s_hRC );
  SwapBuffers( s_hDC );
}

HWND NAMESPACE::getHWND()
{
  return s_hWnd;
}

void NAMESPACE::showCursor(bool show)
{
  if (show) {
    while (ShowCursor(true) < 0) {}
  } else {
    while (ShowCursor(false) >= 0) {}
  }
}

