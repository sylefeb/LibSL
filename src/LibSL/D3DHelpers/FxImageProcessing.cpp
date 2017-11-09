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
// ------------------------------------------------------------
#include "LibSL.precompiled.h"
// ------------------------------------------------------------

#include "FxImageProcessing.h"

// ------------------------------------------------------------

#define NAMESPACE LibSL::D3DHelpers

// ------------------------------------------------------------

NAMESPACE::D3DQuad *NAMESPACE::FxImageProcessing::s_Quad      = NULL;
int                 NAMESPACE::FxImageProcessing::s_iRefCount = 0;

// ------------------------------------------------------------

void NAMESPACE::FxImageProcessing::init(
    LPDIRECT3DDEVICE9 d3d,
    const char  *shader_code,
    DWORD        flags,
    D3DXMACRO   *defines)
{
  m_pd3dDevice = d3d;
  if (s_Quad == NULL) {
    s_Quad = new D3DQuad(d3d);
  }
  s_iRefCount++;

  // create new defines 
  int nb=0;
  if (defines != NULL) {
    // cound previous defines
    for (nb=0; ;nb++)
      if (defines[nb].Name == NULL)
        break;
  }

  FxShader::t_Defines *new_defines = new FxShader::t_Defines[nb+3];
  // copy old defines
  int i=0;
  for (i=0;i<nb;i++) {
    new_defines[i].Name       = defines[i].Name;
    new_defines[i].Definition = defines[i].Definition;
  }
  new_defines[i].Name      ="FX_IMAGE_PROCESSING_SHADER_HEADER";
  new_defines[i].Definition= FX_IMAGE_PROCESSING_SHADER_HEADER_STRING;
  i++;
  new_defines[i].Name      ="FX_IMAGE_PROCESSING_SHADER_FOOTER";
  new_defines[i].Definition= FX_IMAGE_PROCESSING_SHADER_FOOTER_STRING;
  i++;
  new_defines[i].Name      = NULL;
  new_defines[i].Definition= NULL;

  // call FxShader constructor
  FxShader::init(shader_code,flags,new_defines);

  // init default parameters
  m_fxViewport         .init(*this,"Viewport");
  m_fxDestRegionCoord  .init(*this,"DestRegionCoord");
  m_fxDestRegionSize   .init(*this,"DestRegionSize");
  m_fxInvDestRegionSize.init(*this,"InvDestRegionSize");
  m_fxTechMain         .init(*this,"t_main");

  m_fxTechMain.activate();
}

// ------------------------------------------------------------

void NAMESPACE::FxImageProcessing::terminate()
{
  s_iRefCount--;
  if (s_iRefCount <= 0)
    LIBSL_SAFE_DELETE(s_Quad);
  FxShader::terminate();
}

// ------------------------------------------------------------

void NAMESPACE::FxImageProcessing::renderFull(int w,int h)
{
  m_fxViewport         .set(0,0,(float)w,(float)h);
  m_fxDestRegionCoord  .set(0,0);
  m_fxDestRegionSize   .set((float)w,(float)h);
  m_fxInvDestRegionSize.set(1.0f/(float)w,1.0f/(float)h);

  D3DVIEWPORT9 vp;
  vp.X     = 0;
  vp.Y     = 0;
  vp.Width = w;
  vp.Height= h;
  vp.MinZ  =-1;
  vp.MaxZ  = 1;
  LIBSL_D3D_CHECK_ERROR(m_pd3dDevice->SetViewport(&vp));

  LIBSL_D3D_CHECK_ERROR(m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
  LIBSL_D3D_CHECK_ERROR(m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));

  commit();

  begin();
  s_Quad->render();
  end();
}

// ------------------------------------------------------------

void NAMESPACE::FxImageProcessing::renderRegion(int x,int y,int w,int h)
{
  renderRegion(x,y,x,y,w,h);
}

// ------------------------------------------------------------

void NAMESPACE::FxImageProcessing::renderRegion(int srcx,int srcy,int dstx,int dsty,int w,int h)
{
  m_fxViewport         .set((float)dstx,(float)dsty,(float)w,(float)h);
  m_fxDestRegionCoord  .set((float)srcx,(float)srcy);
  m_fxDestRegionSize   .set((float)w,(float)h);
  m_fxInvDestRegionSize.set(1.0f/(float)w,1.0f/(float)h);

  D3DVIEWPORT9 vp;
  vp.X     = dstx;
  vp.Y     = dsty;
  vp.Width = w;
  vp.Height= h;
  vp.MinZ  =-1;
  vp.MaxZ  = 1;
  LIBSL_D3D_CHECK_ERROR(m_pd3dDevice->SetViewport(&vp));

  LIBSL_D3D_CHECK_ERROR(m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
  LIBSL_D3D_CHECK_ERROR(m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));

  commit();

  begin();
  s_Quad->render();
  end(); 
}

// ------------------------------------------------------------
