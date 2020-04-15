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
#include "LibSL.precompiled.h"
//---------------------------------------------------------------------------

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

#include <LibSL/Errors/Errors.h>
#include <LibSL/UIHelpers/SimpleUI.h>
#include <LibSL/UIHelpers/BindAntTweakBar.h>

#include <AntTweakBar.h>

#define NAMESPACE LibSL::UIHelpers::SimpleUI

#if defined(_WIN32) || defined(_WIN64) // ---------------------------------------

static LRESULT CALLBACK bindWin32Event(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  LRESULT res = TwEventWin32(hWnd, uMsg, wParam, lParam); // send event message to AntTweakBar
  return res;
}

void NAMESPACE::TwBind()
{
  SimpleUI::setCustomCallbackMsgProc(bindWin32Event);
}

#else // ! WIN32

#ifdef USE_GLUT // ---------------------------------------

static NAMESPACE::f_onKeyPressed         prevKeyPressed         = NULL;
static NAMESPACE::f_onKeyUnpressed       prevKeyUnpressed       = NULL;
static NAMESPACE::f_onScanCodePressed    prevScanCodePressed    = NULL;
static NAMESPACE::f_onScanCodeUnpressed  prevScanCodeUnpressed  = NULL;
static NAMESPACE::f_onMouseButtonPressed prevMouseButtonPressed = NULL;
static NAMESPACE::f_onMouseMotion        prevMouseMotion        = NULL;
static NAMESPACE::f_onReshape            prevReshape            = NULL;

static void bindMouseButtonPressed(uint x,uint y,uint button,uint flags)
{
  TwMouseAction   twa  = (flags == LIBSL_BUTTON_UP ? TW_MOUSE_RELEASED : TW_MOUSE_PRESSED);
  TwMouseButtonID twid = (button == LIBSL_LEFT_BUTTON ? TW_MOUSE_LEFT : ( button == LIBSL_RIGHT_BUTTON ? TW_MOUSE_RIGHT : TW_MOUSE_MIDDLE ) );
  if ( ! TwMouseButton(twa,twid) ) {
    prevMouseButtonPressed(x,y,button,flags);
  }
}

void bindMouseMotion(uint x,uint y)
{
  if ( ! TwMouseMotion(x,y) ) {
    prevMouseMotion(x,y);
  }
}

void bindKeypressed(uchar key)
{
  if ( ! TwKeyPressed((int)key,0) ) {
    prevKeyPressed( key );
  }
}

void bindReshape(uint w,uint h)
{
  TwWindowSize(w,h);
  prevReshape(w,h);
}

void NAMESPACE::TwBind()
{
  // chain with SimpleUI
  prevKeyPressed                 = SimpleUI::onKeyPressed;
  prevKeyUnpressed               = SimpleUI::onKeyUnpressed;
  prevScanCodePressed            = SimpleUI::onScanCodePressed;
  prevScanCodeUnpressed          = SimpleUI::onScanCodeUnpressed;
  prevMouseButtonPressed         = SimpleUI::onMouseButtonPressed;
  prevMouseMotion                = SimpleUI::onMouseMotion;
  prevReshape                    = SimpleUI::onReshape;
  // redirect
  SimpleUI::onMouseButtonPressed = bindMouseButtonPressed;
  SimpleUI::onMouseMotion        = bindMouseMotion;
  SimpleUI::onKeyPressed         = bindKeypressed;
  SimpleUI::onReshape            = bindReshape;
}
  
#else // ! USE_GLUT

void NAMESPACE::TwBind()
{
  throw LibSL::Errors::Fatal("[LibSL::BindAntTweakBar::bind()] not implemented");
}

#endif // USE_GLUT
#endif // WIN32

//---------------------------------------------------------------------------


