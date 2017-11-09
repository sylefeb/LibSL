#pragma once

#ifdef GLPANELBINDER_EXPORTS
#define GLPANELBINDER_API __declspec(dllexport)
#else
#define GLPANELBINDER_API __declspec(dllimport)
#endif

#include <LibSL/System/Types.h>
#include <map>

extern "C" GLPANELBINDER_API int nGLPanelBinder;

namespace GLPanelBinder
{

  typedef void (*f_onRender)(HWND,uint w,uint h);
  typedef void (*f_onReshape)(HWND);
  typedef void (*f_onMouseMotion)(HWND,uint x,uint y,uint w,uint h);
  typedef void (*f_onMouseButtonPressed)(HWND,uint x,uint y,uint w,uint h,uint button,uint flags);

  extern "C" GLPANELBINDER_API std::map<HWND,f_onRender>             onRender;
  extern "C" GLPANELBINDER_API std::map<HWND,f_onReshape>            onReshape;
  extern "C" GLPANELBINDER_API std::map<HWND,f_onMouseMotion>        onMouseMotion;
  extern "C" GLPANELBINDER_API std::map<HWND,f_onMouseButtonPressed> onMouseButtonPressed;

  extern "C" GLPANELBINDER_API std::map<HWND,uint>                   ScreenW;
  extern "C" GLPANELBINDER_API std::map<HWND,uint>                   ScreenH;

#define Left   1
#define Middle 2
#define Right  3

};

extern "C" GLPANELBINDER_API void __stdcall GLPanelInit(HWND hwnd);
extern "C" GLPANELBINDER_API void __stdcall GLPaint(HWND hwnd,int w,int h);
extern "C" GLPANELBINDER_API void __stdcall GLReshape(HWND hwnd,int w,int h);
extern "C" GLPANELBINDER_API void __stdcall GLMouseMotion(HWND hwnd,int x,int y,int w,int h);
extern "C" GLPANELBINDER_API void __stdcall GLMouseButtonPressed(HWND hwnd,int x,int y,int w,int h,int btn,int flags);

