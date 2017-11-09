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
#include "LibSL.precompiled.h"
// --------------------------------------------------------------

#include <math.h>
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <commctrl.h>

// --------------------------------------------------------------

#include "D3DHelpers.h"
#include "D3DQuad.h"

// --------------------------------------------------------------

#define NAMESPACE LibSL::D3DHelpers

// --------------------------------------------------------------

DWORD NAMESPACE::D3DQuad::FVF = 
D3DFVF_XYZ              |
D3DFVF_TEX2             |
D3DFVF_TEXCOORDSIZE2(0) |
D3DFVF_TEXCOORDSIZE2(1);

// --------------------------------------------------------------

LPDIRECT3DVERTEXBUFFER9 NAMESPACE::D3DQuad::s_QuadVB        =NULL;
int                     NAMESPACE::D3DQuad::s_iInstanceCount=0;

// --------------------------------------------------------------

NAMESPACE::D3DQuad::D3DQuad(LPDIRECT3DDEVICE9 p)
{
  m_pd3dDevice=p;
  s_iInstanceCount++;
  if (s_QuadVB == NULL) {

    D3DQuad::VERTEX vertices[] =
    {
      {  -1.0,-1.0,0.0, 0.0,0.0, 0.0,0.0 },
      {   1.0,-1.0,0.0, 1.0,0.0, 1.0,0.0 },
      {   1.0, 1.0,0.0, 1.0,1.0, 1.0,1.0 },

      {  -1.0,-1.0,0.0, 0.0,0.0, 0.0,0.0 },
      {   1.0, 1.0,0.0, 1.0,1.0, 1.0,1.0 },
      {  -1.0, 1.0,0.0, 0.0,1.0, 0.0,1.0 },
    };

    m_pd3dDevice->CreateVertexBuffer(6*sizeof(D3DQuad::VERTEX),
      0, D3DQuad::FVF,
      D3DPOOL_DEFAULT, &s_QuadVB, NULL);

    VOID* pVertices;
    s_QuadVB->Lock( 0, 0, (void**)&pVertices, 0 );
    memcpy( pVertices, vertices, 6*sizeof(D3DQuad::VERTEX) );
    s_QuadVB->Unlock();
  }
}

// --------------------------------------------------------------

NAMESPACE::D3DQuad::~D3DQuad()
{
  s_iInstanceCount--;
  if (s_iInstanceCount == 0) {
    LIBSL_D3D_SAFE_RELEASE(s_QuadVB);
  }
}

// --------------------------------------------------------------

void NAMESPACE::D3DQuad::render()
{
  m_pd3dDevice->SetStreamSource( 0, s_QuadVB, 0, sizeof(D3DQuad::VERTEX) );
  m_pd3dDevice->SetFVF( D3DQuad::FVF );
  m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );	
}

// --------------------------------------------------------------
