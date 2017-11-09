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
// --------------------------------------------------------------
//
// DirectX tutorial 1 of the GPUmesh class (indexed meshes)
//
// Dependencies
//
//
// (c) Sylvain Lefebvre 2003
//
//     Sylvain.Lefebvre@laposte.net
// --------------------------------------------------------------
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
//-----------------------------------------------------------------------------
#include "D3DGPUmesh.h"
//-----------------------------------------------------------------------------
using namespace GPUmesh;
//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device
/* -------------------------------------------------------- */

// define our vertex format

typedef GPUMESH_MVF2(mvf_vertex_3f,mvf_color0_rgba) mvf_mesh;

// associate render mesh and choosen vertex format

typedef GPUmesh_Indexed_D3D<mvf_mesh,unsigned int> IndexedMesh;

IndexedMesh         *g_Object=NULL;

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

  return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Create the textures and vertex buffers
//-----------------------------------------------------------------------------

HRESULT InitGeometry()
{
  // Create a mesh
  g_Object=new IndexedMesh(g_pd3dDevice);

  // Create some simple indexed geometry

  g_Object->begin(GPUMESH_TRIANGLELIST);

  g_Object->color0_4(0,0,255,0);
  g_Object->vertex_3(0.1f,0.1f,0.0f);

  g_Object->color0_4(255,0,0,0);
  g_Object->vertex_3(0.1f,0.9f,0.0f);

  g_Object->color0_4(0,255,0,0);
  g_Object->vertex_3(0.9f,0.9f,0.0f);

  g_Object->color0_4(64,0,255,0);
  g_Object->vertex_3(0.9f,0.1f,0.0f);

  g_Object->index(0);
  g_Object->index(1);
  g_Object->index(2);

  g_Object->index(0);
  g_Object->index(2);
  g_Object->index(3);

  g_Object->end();

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
  g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

  D3DXMATRIXA16 matView;
  D3DXMatrixIdentity( &matView );
  g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

  D3DXMATRIXA16 matProj;
  D3DXMatrixOrthoOffCenterRH(&matProj,0.0,1.0,0.0,1.0,-1.0,1.0);
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
  HWND hWnd = CreateWindow( "D3D Tutorial", "GPUmesh D3D tutorial 1 (indexed mesh)",
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
