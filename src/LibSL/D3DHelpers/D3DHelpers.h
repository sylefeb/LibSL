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
// ------------------------------------------------------
// LibSL::D3DHelpers
// ------------------------------------------------------
//
// Direct3D helpers
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-07
// ------------------------------------------------------

#pragma once

#include <windows.h>
#include <d3d9.h>

#include <dxut.h>

#include <cstdarg>
#include <cstdio>

#include <LibSL/Errors/Errors.h>

namespace LibSL {
  namespace D3DHelpers {

    LIBSL_NEW_EXCEPTION_TYPE(D3DException)

    class D3DProtectRenderTarget
    {
    private:

      LPDIRECT3DDEVICE9        m_d3dDevice;
      LPDIRECT3DSURFACE9       m_d3dTarget;
      D3DVIEWPORT9             m_d3dVP;
      bool                     m_bEnabled;

    public:

      D3DProtectRenderTarget(LPDIRECT3DDEVICE9 d3d,bool enabled=true)
      {
        m_bEnabled=enabled;
        if (m_bEnabled)
        {
          d3d->GetViewport(&m_d3dVP);
          d3d->GetRenderTarget(0,&m_d3dTarget);
          m_d3dDevice=d3d;
        }
      }

      ~D3DProtectRenderTarget()
      {
        if (m_bEnabled)
        {
          m_d3dDevice->SetRenderTarget(0,m_d3dTarget);
          m_d3dTarget->Release();
          m_d3dDevice->SetViewport(&m_d3dVP);
        }
      }

    };

#ifdef DIRECT3D
    LIBSL_DLL LPDIRECT3DDEVICE9& d3dDevice();
#endif

#ifdef DIRECTX10
    LIBSL_DLL ID3D10Device*&     d3dDevice();
#endif

  } //namespace LibSL::D3DHelpers
} //namespace LibSL

// ------------------------------------------------------

#define LIBSL_D3D_CHECK_ERROR(x)         {if ((x) != D3D_OK) throw LibSL::D3DHelpers::D3DException("Error %s (line %d, file %s)",#x,__LINE__,__FILE__);}
#define LIBSL_D3D_CHECK_ERROR_DISPLAY(x) {if ((x) != D3D_OK) throw LibSL::D3DHelpers::D3DException("Error %s '%s' (line %d, file %s)",#x,(char *)errors->GetBufferPointer(),__LINE__,__FILE__);}
#ifndef LIBSL_D3D_SAFE_RELEASE
#  define LIBSL_D3D_SAFE_RELEASE(p) {if (p != NULL) {p->Release(); p = NULL;} }
#endif
#ifndef LIBSL_D3D_SAFE_DELETE
#  define LIBSL_D3D_SAFE_DELETE(p) {if (p != NULL) {delete (p); p = NULL;} }
#endif

#ifdef DXUT_OLD_VERSION
# define LIBSL_DIRECT3D_DEVICE       LibSL::D3DHelpers::d3dDevice()
# define LIBSL_D3D_DEVICE            LibSL::D3DHelpers::d3dDevice()
#else
# ifdef DIRECT3D
#  define LIBSL_DIRECT3D_DEVICE       LibSL::D3DHelpers::d3dDevice()
#  define LIBSL_D3D_DEVICE            LibSL::D3DHelpers::d3dDevice()
#  define LPDIRECT3DDEVICE            LPDIRECT3DDEVICE9
# endif
# ifdef DIRECTX10
#  define LIBSL_DIRECT3D_DEVICE       LibSL::D3DHelpers::d3dDevice()
#  define LIBSL_D3D_DEVICE            LibSL::D3DHelpers::d3dDevice()
#  define LPDIRECT3DDEVICE            ID3D10Device*
# endif
#endif

#if defined(DEBUG) || defined(_DEBUG)
#ifndef V
#define V(x)           { hr = x; if( FAILED(hr) ) { DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
#endif
#else
#ifndef V
#define V(x)           { hr = x; }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return hr; } }
#endif
#endif

// ------------------------------------------------------
