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
//-----------------------------------------------------------------------------
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
//-----------------------------------------------------------------------------
#include "D3DGPUmesh.h"
#include "CGenSpiral.h"
//-----------------------------------------------------------------------------
using namespace GPUmesh;
//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device
/* -------------------------------------------------------- */

/* -------------------------------------------------------- */
// creates our own GPUmesh class from the generic render mesh

// define our vertex format

typedef TYPELIST_3(mvf_vertex_3f,mvf_normal_3f,mvf_color0_rgba) mvf_simple;

// associate render meshes and choosen vertex format

typedef GPUmesh_D3D<mvf_simple>         SimpleMesh;

/* -------------------------------------------------------- */

SimpleMesh *g_Object=NULL;

/* -------------------------------------------------------- */

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
  // Create the D3D object.
  if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
    return E_FAIL;

  // Set up the structure used to create the D3DDevice. Since we are now
  // using more complex geometry, we will create a device with a zbuffer.
  D3DPRESENT_PARAMETERS d3dpp;
  ZeroMemory( &d3dpp, sizeof(d3dpp) );
  d3dpp.Windowed = TRUE;
  d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
  d3dpp.EnableAutoDepthStencil = TRUE;
  d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

  // Create the D3DDevice
  if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
    D3DCREATE_SOFTWARE_VERTEXPROCESSING,
    &d3dpp, &g_pd3dDevice ) ) )
  {
    return E_FAIL;
  }

  // Turn off culling
  g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

  // Turn off D3D lighting
  g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

  // Turn on the zbuffer
  g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

  // Lighting
  // -> light
  D3DXVECTOR3 vecDir;
  D3DLIGHT9 light;
  ZeroMemory( &light, sizeof(D3DLIGHT9) );
  light.Type       = D3DLIGHT_DIRECTIONAL;
  light.Diffuse.r  = 1.0f;
  light.Diffuse.g  = 1.0f;
  light.Diffuse.b  = 1.0f;
  light.Specular.r  = 1.0f;
  light.Specular.g  = 1.0f;
  light.Specular.b  = 1.0f;
  vecDir           = D3DXVECTOR3(0.0,1.0,1.0);
  D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
  light.Range      = 1000.0f;
  g_pd3dDevice->SetLight(0,&light );
  g_pd3dDevice->LightEnable(0,TRUE );
  g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE );
  g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0x00202020 );
  // -> material
  D3DMATERIAL9 mtrl;
  ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
  mtrl.Diffuse.r = mtrl.Specular.r = 1.0f;
  mtrl.Diffuse.g = mtrl.Specular.g = 1.0f;
  mtrl.Diffuse.b = mtrl.Specular.b = 1.0f;
  mtrl.Diffuse.a = mtrl.Specular.a = 1.0f;
  mtrl.Power = 20.0;
  g_pd3dDevice->SetMaterial( &mtrl );

  g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS,TRUE);
  g_pd3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
  g_pd3dDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL);

  return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Create the textures and vertex buffers
//-----------------------------------------------------------------------------

HRESULT InitGeometry()
{
  // Create a mesh
  g_Object=new SimpleMesh(g_pd3dDevice);
  // Use the geometry producer to create the mesh geometry (see CGenSpiral.h)
  GenSpiral<SimpleMesh>::genSpiral(*g_Object,50);

  return (D3D_OK);
}
//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
  if( g_pd3dDevice != NULL )
    g_pd3dDevice->Release();

  if( g_pD3D != NULL )
    g_pD3D->Release();

  delete (g_Object);
}
//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
  D3DXMATRIXA16 matWorld;
  D3DXMatrixIdentity( &matWorld );
  D3DXMATRIXA16 mat;
  D3DXMatrixRotationY(&mat,timeGetTime()/1000.0f );
  D3DXMatrixMultiply(&matWorld,&mat,&matWorld);
  D3DXMatrixRotationZ(&mat,3.14f*timeGetTime()/1000.0f );
  D3DXMatrixMultiply(&matWorld,&mat,&matWorld);
  g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

  D3DXVECTOR3 vEyePt( 0.0, -5.0, -5.0 );
  D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
  D3DXVECTOR3 vUpVec( 0.0f, 0.0f, 1.0f );
  D3DXMATRIXA16 matView;
  D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
  g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

  D3DXMATRIXA16 matProj;
  D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
  g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
  // Clear the backbuffer and the zbuffer
  g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
    D3DCOLOR_XRGB(128,128,128), 1.0f, 0 );

  // Begin the scene
  if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
  {
    // Setup the world, view, and projection matrices
    SetupMatrices();

    g_Object->bind();
    g_Object->draw();
    g_Object->unbind();

    // End the scene
    g_pd3dDevice->EndScene();
  }

  // Present the backbuffer contents to the display
  g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch( msg )
  {
  case WM_DESTROY:
    Cleanup();
    PostQuitMessage( 0 );
    return 0;
  }

  return DefWindowProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
  // Register the window class
  WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
    GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
    "D3D Tutorial", NULL };
  RegisterClassEx( &wc );

  // Create the application's window
  HWND hWnd = CreateWindow( "D3D Tutorial", "Render mesh demo",
    WS_OVERLAPPEDWINDOW, 100, 100, 640, 480,
    GetDesktopWindow(), NULL, wc.hInstance, NULL );

  // Initialize Direct3D
  if( SUCCEEDED( InitD3D( hWnd ) ) )
  {
    // Create the scene geometry
    if( SUCCEEDED( InitGeometry() ) )
    {
      // Show the window
      ShowWindow( hWnd, SW_SHOWDEFAULT );
      UpdateWindow( hWnd );

      // Enter the message loop
      MSG msg;
      ZeroMemory( &msg, sizeof(msg) );
      while( msg.message!=WM_QUIT )
      {
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
          TranslateMessage( &msg );
          DispatchMessage( &msg );
        }
        else
          Render();
      }
    }
  }

  UnregisterClass( "D3D Tutorial", wc.hInstance );
  return 0;
}
