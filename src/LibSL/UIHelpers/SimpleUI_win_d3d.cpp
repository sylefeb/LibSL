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
// DirectX common implementation - uses DXUT
//---------------------------------------------------------------------------

static VOID APIENTRY dxutOnDestroyDevice(void* pUserContext)
{

}

static VOID APIENTRY dxutOnLostDevice(void* pUserContext)
{

}

static bool s_MouseButtonPreviousState[3]={false,false,false};

static VOID APIENTRY dxutOnMouse(
                          bool bLeftButtonDown,
                          bool bRightButtonDown,
                          bool bMiddleButtonDown,
                          bool bSideButton1Down,
                          bool bSideButton2Down,
                          INT nMouseWheelDelta,
                          INT xPos,
                          INT yPos,
                          void* pUserContext)
{
  uint button=0,flags=0;

  if (bLeftButtonDown && bRightButtonDown) {
    bLeftButtonDown=bRightButtonDown=false;
    bMiddleButtonDown=true;
  }

  if (bLeftButtonDown  )       button=LIBSL_LEFT_BUTTON;
  else if (bMiddleButtonDown)  button=LIBSL_MIDDLE_BUTTON;
  else if (bRightButtonDown )  button=LIBSL_RIGHT_BUTTON;

  // cerr << sprint(" %d %d %d \n",bLeftButtonDown,bMiddleButtonDown,bRightButtonDown);

  if ( (bLeftButtonDown   && !s_MouseButtonPreviousState[0])
    || (bMiddleButtonDown && !s_MouseButtonPreviousState[1])
    || (bRightButtonDown  && !s_MouseButtonPreviousState[2])
       ) {
    flags |= LIBSL_BUTTON_DOWN;
  }

  if (s_MouseButtonPreviousState[0] && !bLeftButtonDown) {
    button = LIBSL_LEFT_BUTTON;
    flags |= LIBSL_BUTTON_UP;
  }
  if (s_MouseButtonPreviousState[1] && !bMiddleButtonDown) {
    button = LIBSL_MIDDLE_BUTTON;
    flags |= LIBSL_BUTTON_UP;
  }
  if (s_MouseButtonPreviousState[2] && !bRightButtonDown) {
    button = LIBSL_RIGHT_BUTTON;
    flags |= LIBSL_BUTTON_UP;
  }

  if ((flags & LIBSL_BUTTON_DOWN) || (flags & LIBSL_BUTTON_UP)) {
    NAMESPACE::onMouseButtonPressed(xPos,yPos,button,flags);
  }

  s_MouseButtonPreviousState[0] = bLeftButtonDown;
  s_MouseButtonPreviousState[1] = bMiddleButtonDown;
  s_MouseButtonPreviousState[2] = bRightButtonDown;

  NAMESPACE::onMouseMotion(xPos,yPos);

  if (nMouseWheelDelta != 0) {
    NAMESPACE::onMouseWheel( nMouseWheelDelta );
  }
}


static VOID APIENTRY dxutOnKeyboard(
                             UINT nChar,
                             bool bKeyDown,
                             bool bAltDown,
                             void* pUserContext)
{
  if (bKeyDown) {
    NAMESPACE::onKeyPressed  (nChar);
  } else {
    NAMESPACE::onKeyUnpressed(nChar);
  }
}

LRESULT CALLBACK dxutMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
  *pbNoFurtherProcessing = false;

  if (SimpleUI_Custom_WndProc != NULL) {
    if (SimpleUI_Custom_WndProc(hWnd,uMsg,wParam,lParam)) {
      return 0;
    }
  }

  switch (uMsg)
  {
  case WM_KEYDOWN:
    NAMESPACE::onScanCodePressed((lParam >> 16) & 0xFF);
    break;

  case WM_KEYUP:
    NAMESPACE::onScanCodeUnpressed((lParam >> 16) & 0xFF);
    break;
  }
  return S_OK;
}

void NAMESPACE::loop()
{
  DXUTMainLoop(NULL);
}

void NAMESPACE::shutdown()
{
  DXUTShutdown();
}

void NAMESPACE::exit()
{
  PostMessage(DXUTGetHWND(),WM_QUIT,0,0);
}

HWND NAMESPACE::getHWND()
{
  return DXUTGetHWND();
}

#endif

#ifdef DIRECT3D

//---------------------------------------------------------------------------
// DirectX 9 implementation - uses DXUT
//---------------------------------------------------------------------------

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxut.h>

#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/D3DHelpers/D3DHelpers.h>

static VOID APIENTRY dxutOnFrameRender(
                                IDirect3DDevice9 * pd3dDevice,
                                DOUBLE fTime,
                                FLOAT fElapsedTime,
                                void* pUserContext)
{
  static double last = (double)LibSL::System::Time::milliseconds();
  double now         = (double)LibSL::System::Time::milliseconds();
  float  elapsed     = float(now - last);

  if (elapsed > 0) {
    NAMESPACE::onAnimate( now, elapsed );
    last = now;
  }

  LIBSL_D3D_CHECK_ERROR(LIBSL_DIRECT3D_DEVICE->BeginScene());
  NAMESPACE::onRender();
  LIBSL_D3D_CHECK_ERROR(LIBSL_DIRECT3D_DEVICE->EndScene());

}

static HRESULT APIENTRY dxutOnResetDevice(
                                   IDirect3DDevice9 * pd3dDevice,
                                   CONST D3DSURFACE_DESC * pBackBufferSurfaceDesc,
                                   void* pUserContext)
{
  return S_OK;
}

static void dxutGetSupportedTextureFormat( IDirect3D9 *pD3D, D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
                    D3DFORMAT* pfmtTexture, D3DFORMAT* pfmtCubeMap )
{
  D3DFORMAT fmtTexture = D3DFMT_UNKNOWN;
  D3DFORMAT fmtCubeMap = D3DFMT_UNKNOWN;

  // check for linear filtering support of signed formats
  fmtTexture = D3DFMT_UNKNOWN;
  if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,AdapterFormat,
    D3DUSAGE_QUERY_FILTER,D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F))) {
    fmtTexture = D3DFMT_A16B16G16R16F;
  }
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,AdapterFormat,
    D3DUSAGE_QUERY_FILTER,D3DRTYPE_TEXTURE, D3DFMT_Q16W16V16U16))) {
    fmtTexture = D3DFMT_Q16W16V16U16;
  }
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType, AdapterFormat,
    D3DUSAGE_QUERY_FILTER,D3DRTYPE_TEXTURE, D3DFMT_Q8W8V8U8))) {
    fmtTexture = D3DFMT_Q8W8V8U8;
  }
  // no support for linear filtering of signed, just checking for format support now
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType, AdapterFormat,
    0, D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F))) {
    fmtTexture = D3DFMT_A16B16G16R16F;
  }
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType, AdapterFormat,
    0, D3DRTYPE_TEXTURE, D3DFMT_Q16W16V16U16))) {
    fmtTexture = D3DFMT_Q16W16V16U16;
  }
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType, AdapterFormat,
    0, D3DRTYPE_TEXTURE, D3DFMT_Q8W8V8U8))) {
    fmtTexture = D3DFMT_Q8W8V8U8;
  }

  // check for support linear filtering of signed format cubemaps
  fmtCubeMap = D3DFMT_UNKNOWN;
  if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,AdapterFormat,
    D3DUSAGE_QUERY_FILTER, D3DRTYPE_CUBETEXTURE, D3DFMT_A16B16G16R16F))) {
    fmtCubeMap = D3DFMT_A16B16G16R16F;
  }
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,AdapterFormat,
    D3DUSAGE_QUERY_FILTER,D3DRTYPE_CUBETEXTURE, D3DFMT_Q16W16V16U16))) {
    fmtCubeMap = D3DFMT_Q16W16V16U16;
  }
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,AdapterFormat,
    D3DUSAGE_QUERY_FILTER,D3DRTYPE_CUBETEXTURE, D3DFMT_Q8W8V8U8))) {
    fmtCubeMap = D3DFMT_Q8W8V8U8;
  }
  // no support for linear filtering of signed formats, just checking for format support now
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,AdapterFormat,
    0,D3DRTYPE_CUBETEXTURE, D3DFMT_A16B16G16R16F))) {
    fmtCubeMap = D3DFMT_A16B16G16R16F;
  }
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,AdapterFormat,
    0,D3DRTYPE_CUBETEXTURE, D3DFMT_Q16W16V16U16))) {
    fmtCubeMap = D3DFMT_Q16W16V16U16;
  }
  else if (SUCCEEDED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,AdapterFormat,
    0,D3DRTYPE_CUBETEXTURE, D3DFMT_Q8W8V8U8))) {
    fmtCubeMap = D3DFMT_Q8W8V8U8;
  }

  if( pfmtTexture ) *pfmtTexture = fmtTexture;
  if( pfmtCubeMap ) *pfmtCubeMap = fmtCubeMap;
}



static bool CALLBACK dxutIsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
                                 D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
  // Skip backbuffer formats that don't support alpha blending
#ifdef DXUT_OLD_VERSION
  IDirect3D9* pD3D = DXUTGetD3DObject();
#else
  IDirect3D9* pD3D = DXUTGetD3D9Object();
#endif
  if( FAILED( pD3D->CheckDeviceFormat(
    pCaps->AdapterOrdinal, pCaps->DeviceType,
    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
    return false;

  // Determine texture support.  Fail if not good enough
  D3DFORMAT fmtTexture, fmtCubeMap;
  dxutGetSupportedTextureFormat( pD3D, pCaps, AdapterFormat, &fmtTexture, &fmtCubeMap );
  if (D3DFMT_UNKNOWN == fmtTexture || D3DFMT_UNKNOWN == fmtCubeMap)
    return false;

  // Requires pixel shader 3.0
  if(pCaps->PixelShaderVersion < D3DPS_VERSION(3,0))
    return false;

  return true;
}



static bool CALLBACK dxutModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
#ifdef DXUT_OLD_VERSION
  pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D24S8;
#else
  pDeviceSettings->d3d9.pp.AutoDepthStencilFormat = D3DFMT_D24S8;
#endif
  return true;
}


static HRESULT APIENTRY dxutOnCreateDevice(
                                    IDirect3DDevice9 * pd3dDevice,
                                    CONST D3DSURFACE_DESC * pBackBufferSurfaceDesc,
                                    void* pUserContext)
{
  s_ScreenW = pBackBufferSurfaceDesc->Width;
  s_ScreenH = pBackBufferSurfaceDesc->Height;
  return S_OK;
}


void NAMESPACE::init(uint width,uint height,const char *title,char **argv,int argc,bool frameLess,bool hidden,bool fullScreen)
{
  sl_assert( ! fullScreen );

  DXUTInit(false/*parse cmd line*/,true/*show msg on error*/,L"foo -forceapi:9");
#ifdef DXUT_OLD_VERSION
  DXUTSetCallbackDeviceCreated      ( dxutOnCreateDevice );
  DXUTSetCallbackDeviceReset        ( dxutOnResetDevice );
  DXUTSetCallbackDeviceLost         ( dxutOnLostDevice );
  DXUTSetCallbackDeviceDestroyed    ( dxutOnDestroyDevice );
  DXUTSetCallbackFrameRender        ( dxutOnFrameRender );
#else
  DXUTSetCallbackD3D9DeviceCreated  ( dxutOnCreateDevice );
  DXUTSetCallbackD3D9DeviceReset    ( dxutOnResetDevice );
  DXUTSetCallbackD3D9DeviceLost     ( dxutOnLostDevice );
  DXUTSetCallbackD3D9DeviceDestroyed( dxutOnDestroyDevice );
  DXUTSetCallbackD3D9FrameRender    ( dxutOnFrameRender );
#endif
  if (title == NULL) {
    DXUTCreateWindow(L"LibSL::SimpleUI (DX9)");
  } else {
    DXUTCreateWindow(CppHelpers::toUnicode(title));
  }
#ifdef DXUT_OLD_VERSION
  DXUTCreateDevice( D3DADAPTER_DEFAULT, true /*windowed*/, width, height);
  LIBSL_D3D_DEVICE = DXUTGetD3DDevice();
#else
  DXUTCreateDevice( true /*windowed*/, width, height);
  LIBSL_D3D_DEVICE = DXUTGetD3D9Device();
#endif
  DXUTSetCallbackKeyboard( dxutOnKeyboard );
  DXUTSetCallbackMouse( dxutOnMouse , true );
  DXUTSetCallbackMsgProc( dxutMsgProc );
  NAMESPACE::onReshape(width,height);
}

void NAMESPACE::init(HWND hwnd,LPDIRECT3DDEVICE9 device)
{
  // DXUTSetWindow(hwnd,hwnd,hwnd,true);
  // DXUTSetD3D9Device(device);
  LIBSL_D3D_DEVICE = device;

/*
#ifdef DXUT_OLD_VERSION
  DXUTSetCallbackDeviceCreated      ( dxutOnCreateDevice );
  DXUTSetCallbackDeviceReset        ( dxutOnResetDevice );
  DXUTSetCallbackDeviceLost         ( dxutOnLostDevice );
  DXUTSetCallbackDeviceDestroyed    ( dxutOnDestroyDevice );
  DXUTSetCallbackFrameRender        ( dxutOnFrameRender );
#else
  DXUTSetCallbackD3D9DeviceCreated  ( dxutOnCreateDevice );
  DXUTSetCallbackD3D9DeviceReset    ( dxutOnResetDevice );
  DXUTSetCallbackD3D9DeviceLost     ( dxutOnLostDevice );
  DXUTSetCallbackD3D9DeviceDestroyed( dxutOnDestroyDevice );
  DXUTSetCallbackD3D9FrameRender    ( dxutOnFrameRender );
#endif

  DXUTSetCallbackKeyboard( dxutOnKeyboard );
  DXUTSetCallbackMsgProc( dxutMsgProc );
  DXUTSetCallbackMouse( dxutOnMouse , true );
  // NAMESPACE::onReshape(width,height);
*/
}

#endif // DIRECT3D

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifdef DIRECTX10

//---------------------------------------------------------------------------
// DirectX 10 implementation - uses DXUT
//---------------------------------------------------------------------------

#include <windows.h>
#include <dxut.h>

#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/D3DHelpers/D3DHelpers.h>

static VOID APIENTRY dxutOnFrameRender(
                                ID3D10Device* pd3dDevice,
                                DOUBLE fTime,
                                FLOAT fElapsedTime,
                                void* pUserContext)
{
  static double last = (double)LibSL::System::Time::milliseconds();
  double now         = (double)LibSL::System::Time::milliseconds();
  float  elapsed     = float(now - last);

  if (elapsed > 0) {
    NAMESPACE::onAnimate( now, elapsed );
    last = now;
  }

  NAMESPACE::onRender();

}

bool CALLBACK IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
  return true;
}

HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC *pBackBufferSurfaceDesc, void* pUserContext )
{
  return S_OK;
}

HRESULT CALLBACK OnD3D10ResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain *pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
  NAMESPACE::onReshape(pBackBufferSurfaceDesc->Width,pBackBufferSurfaceDesc->Height);
  return S_OK;
}

void CALLBACK OnD3D10ReleasingSwapChain( void* pUserContext )
{

}

void CALLBACK OnD3D10DestroyDevice( void* pUserContext )
{

}

void NAMESPACE::init(uint width,uint height,const char *title,char **argv,int argc,bool frameLess,bool hidden,bool fulscreen)
{
  // Set DXUT callbacks
  DXUTInit( true, true, NULL );
  DXUTSetCursorSettings( true, true );

  DXUTSetCallbackKeyboard ( dxutOnKeyboard );
  DXUTSetCallbackMsgProc( dxutMsgProc );
  DXUTSetCallbackMouse    ( dxutOnMouse , true );

  DXUTSetCallbackD3D10DeviceAcceptable  ( IsD3D10DeviceAcceptable );
  DXUTSetCallbackD3D10SwapChainResized  ( OnD3D10ResizedSwapChain );
  DXUTSetCallbackD3D10FrameRender       ( dxutOnFrameRender );
  DXUTSetCallbackD3D10SwapChainReleasing( OnD3D10ReleasingSwapChain );
  DXUTSetCallbackD3D10DeviceDestroyed   ( OnD3D10DestroyDevice );
  DXUTSetCallbackD3D10DeviceCreated     ( OnD3D10CreateDevice );

  if (title == NULL) {
    DXUTCreateWindow(L"LibSL::SimpleUI (DX10)");
  } else {
    DXUTCreateWindow(CppHelpers::toUnicode(title));
  }

  DXUTCreateDevice( true, width, height );
  LIBSL_D3D_DEVICE = DXUTGetD3D10Device();

  NAMESPACE::onReshape(width,height);
}

#endif // DIRECTX10
