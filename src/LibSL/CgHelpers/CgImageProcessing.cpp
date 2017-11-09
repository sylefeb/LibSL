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

#include "CgImageProcessing.h"
#include "CgImageProcessing.cg"

// ------------------------------------------------------------

#ifdef OPENGL
#include <LibSL/GPUMesh/GPUMesh_gl.h>
#endif
#ifdef DIRECT3D
#include <LibSL/GPUMesh/GPUMesh_d3d.h>
#include <LibSL/GPUHelpers/GPUHelpers.h>
#include <LibSL/D3DHelpers/D3DHelpers.h>
#endif

using namespace GPUMesh;

#include <LibSL/Memory/Pointer.h>
using namespace LibSL::Memory::Pointer;

// ------------------------------------------------------------

#include <string>
using namespace std;

// ------------------------------------------------------------

#define NAMESPACE LibSL::CgHelpers

// ------------------------------------------------------------

typedef MVF2(mvf_position_3f,mvf_texcoord0_2f)       mvf_quad;
typedef GPUMesh_VertexBuffer<mvf_quad>               QuadMesh;

static  QuadMesh *s_Quad                         = NULL;

int NAMESPACE::CgImageProcessing::s_QuadCounter  = 0;

static const char *CgImageProcessing_code_string =
#include "CgImageProcessing.string"

// ------------------------------------------------------------

void NAMESPACE::CgImageProcessing::init(const char *code,const char **args)
{
  sl_dbg_assert(s_QuadCounter >= 0);

  if (s_Quad == NULL) {

    s_Quad = new QuadMesh();
    s_Quad->begin(GPUMESH_TRIANGLELIST);

    s_Quad->texcoord0_2(0.0f,0.0f);
    s_Quad->vertex_3(-1.0f,-1.0f,0);

    s_Quad->texcoord0_2(1.0f,0.0f);
    s_Quad->vertex_3(1.0f,-1.0f,0);

    s_Quad->texcoord0_2(1.0f,1.0f);
    s_Quad->vertex_3(1.0f,1.0f,0);

    s_Quad->texcoord0_2(0.0f,0.0f);
    s_Quad->vertex_3(-1.0f,-1.0f,0);

    s_Quad->texcoord0_2(1.0f,1.0f);
    s_Quad->vertex_3(1.0f,1.0f,0);

    s_Quad->texcoord0_2(0.0f,1.0f);
    s_Quad->vertex_3(-1.0f,1.0f,0);

    s_Quad->end();
  }

  s_QuadCounter++;

  // build arguments
  static const char* compiler_args[128];
  uint nargs=0;
  if (args != NULL) {
    for (uint n=0;args[n]!=NULL;n++) {
      sl_assert(nargs < 128);
      compiler_args[nargs++]=args[n];
    }
  }
  sl_assert(nargs < 128-2);
  compiler_args[nargs++]="-DRUNTIME";
  compiler_args[nargs++]=NULL;
  
  // add header/footer defines to code
  string shader_code=string(CgImageProcessing_code_string)+"\n\n"+string(code);

  try {
    // call CgShader init
    CgShader::init(shader_code.c_str(),compiler_args);
  } catch (CgException& e) {
    cerr << "\n\n\n                    -= ERROR - Cg Compilation =-" << endl;
    cerr << "[dump in 'CgImageProcessing_lasterror.cg']\n\n" << endl;
    cerr << shader_code << endl;
    cerr << e.message();
    ofstream o("CgImageProcessing_lasterror.cg");
    o << shader_code;
    o.close();
    cerr << "\n\n\n";
    LIBSL_BOUNCE_ERROR(e);
  } 
  
  // init default parameters
  m_cgDestRegionCoord  .init(*this,"DestRegionCoord");
  m_cgViewport         .init(*this,"Viewport");
  m_cgDestRegionSize   .init(*this,"DestRegionSize");
  m_cgInvDestRegionSize.init(*this,"InvDestRegionSize");
  m_cgTechMain         .init(*this,"t_main");

  m_cgTechMain.activate();
}

// ------------------------------------------------------------

void NAMESPACE::CgImageProcessing::terminate()
{
  if (effect() != NULL) {  
    s_QuadCounter--;
    if (s_QuadCounter == 0) {
      delete (s_Quad);
      s_Quad=NULL;
    }
  }
  CgShader::terminate();
}

// ------------------------------------------------------------

void NAMESPACE::CgImageProcessing::renderFull(int w,int h)
{
  m_cgViewport         .set(0.0f,0.0f,float(w),float(h));
  m_cgDestRegionCoord  .set(0.0f,0.0f);
  m_cgDestRegionSize   .set(float(w),float(h));
  m_cgInvDestRegionSize.set(1.0f/float(w),1.0f/float(h));

#ifdef DIRECT3D
  LPDIRECT3DDEVICE9 d3d=LIBSL_DIRECT3D_DEVICE;
  D3DVIEWPORT9 vp;
  vp.X     = 0;
  vp.Y     = 0;
  vp.Width = w;
  vp.Height= h;
  vp.MinZ  =-1;
  vp.MaxZ  = 1;
  LIBSL_D3D_CHECK_ERROR(d3d->SetViewport(&vp));
  LIBSL_D3D_CHECK_ERROR(d3d->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
  LIBSL_D3D_CHECK_ERROR(d3d->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
  LIBSL_D3D_CHECK_ERROR(d3d->SetRenderState(D3DRS_ZENABLE,  FALSE));
#endif

#ifdef OPENGL
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT,vp);
  glViewport(0,0,w,h);
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
#endif

  commit();

  begin();
  s_Quad->render();
  end();

#ifdef DIRECT3D
  LIBSL_D3D_CHECK_ERROR(d3d->SetRenderState(D3DRS_ZENABLE,  TRUE));
#endif

#ifdef OPENGL
  glPopAttrib();
  glViewport(vp[0],vp[1],vp[2],vp[3]);
#endif
}

// ------------------------------------------------------------

void NAMESPACE::CgImageProcessing::renderRegion(int x,int y,int w,int h)
{
  renderRegion(x,y,x,y,w,h);
}

// ------------------------------------------------------------

void NAMESPACE::CgImageProcessing::renderRegion(int srcx,int srcy,int dstx,int dsty,int w,int h)
{
  m_cgViewport         .set(float(dstx),float(dsty),float(w),float(h));
  m_cgDestRegionCoord  .set(float(srcx),float(srcy));
  m_cgDestRegionSize   .set(float(w),float(h));
  m_cgInvDestRegionSize.set(1.0f/float(w),1.0f/float(h));
  
#ifdef DIRECT3D
  D3DVIEWPORT9 vp;
  vp.X     = dstx;
  vp.Y     = dsty;
  vp.Width = w;
  vp.Height= h;
  vp.MinZ  =-1;
  vp.MaxZ  = 1;
  LPDIRECT3DDEVICE9 d3d=LIBSL_DIRECT3D_DEVICE;
  LIBSL_D3D_CHECK_ERROR(d3d->SetViewport(&vp));
  LIBSL_D3D_CHECK_ERROR(d3d->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
  LIBSL_D3D_CHECK_ERROR(d3d->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
#endif

#ifdef OPENGL
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_CULL_FACE);
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT,vp);
  glViewport(dstx,dsty,w,h);
#endif

  commit();

  begin();
  s_Quad->render();
  end(); 

#ifdef OPENGL
  glPopAttrib();
  glViewport(vp[0],vp[1],vp[2],vp[3]);
#endif

}

// ------------------------------------------------------------
