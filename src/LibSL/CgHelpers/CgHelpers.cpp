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
#include "LibSL.precompiled.h"
// ------------------------------------------------------

// TODO: 
// - authorize should be a 'check' policy
// - test setArray in the Direct3D implementation

#include <LibSL/CgHelpers/CgHelpers.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/GPUHelpers/GPUHelpers.h>

#include <Cg/cg.h>

// ------------------------------------------------------

#define NAMESPACE LibSL::CgHelpers

// ------------------------------------------------------

#ifdef DIRECT3D
#include <Cg/cgD3D9.h>
#include <LibSL/D3DHelpers/D3DHelpers.h>
#endif

#ifdef OPENGL
#include <Cg/cgGL.h>
#endif

#include <fstream>

// ------------------------------------------------------

static CGprofile g_cgVertexProfile   = CG_PROFILE_VP40;
static CGprofile g_cgFragmentProfile = CG_PROFILE_FP40;
static CGcontext g_cgContext         = NULL;

// ------------------------------------------------------

char *NAMESPACE::CgShader::s_CgDirs[16]={".",".\\cg",""};
int   NAMESPACE::CgShader::s_CgInitCounter=0;

// -----------------------------------------------------

using namespace std;

// ------------------------------------------------------

void cgHandleError(CGcontext ctx, CGerror err, void *appdata)
{
  cerr << LibSL::CppHelpers::sprint("Cg error: %s\n",cgGetErrorString(err));
#ifdef DIRECT3D
  cerr << LibSL::CppHelpers::sprint("CgD3D9 error: %s\n",cgD3D9TranslateCGerror(err));
#endif
  
  // exit (-1); // DEBUG

  const char *listing = cgGetLastListing(ctx);
  if (listing != NULL)
    cerr << LibSL::CppHelpers::sprint(" last listing: %s\n", listing);
  throw NAMESPACE::CgException("Cg Error (see std::err)");
}

// ------------------------------------------------------

static void cgMainInit()
{
  sl_assert(g_cgContext == NULL);

  g_cgContext=cgCreateContext();
  cgSetErrorHandler(cgHandleError,NULL);

#ifdef DIRECT3D
  cgD3D9SetDevice(LIBSL_DIRECT3D_DEVICE);
#endif

#ifdef DIRECTX10
  cgD3D10SetDevice(LIBSL_DIRECT3D_DEVICE);
#endif

#ifdef OPENGL
  cgGLSetManageTextureParameters(g_cgContext,GL_TRUE);

  g_cgVertexProfile=cgGLGetLatestProfile(CG_GL_VERTEX);
  cgGLSetOptimalOptions(g_cgVertexProfile);  

  g_cgFragmentProfile=cgGLGetLatestProfile(CG_GL_FRAGMENT);
  cgGLSetOptimalOptions(g_cgFragmentProfile);

  cgGLRegisterStates(g_cgContext);
#endif

#ifdef DIRECT3D

  g_cgVertexProfile=cgD3D9GetLatestVertexProfile();
  g_cgFragmentProfile=cgD3D9GetLatestPixelProfile();

  cgD3D9SetManageTextureParameters(g_cgContext,CG_TRUE);
  cgD3D9RegisterStates(g_cgContext);
//  cgD3D9EnableDebugTracing(CG_TRUE);
#endif
}

// ------------------------------------------------------

static void cgMainTerminate()
{
  sl_assert(g_cgContext != NULL);

#ifdef DIRECT3D
  cgD3D9SetDevice(0);
#endif

  cgDestroyContext(g_cgContext);
  g_cgContext=NULL;
}

// ------------------------------------------------------

static void checkError(const char *msg)
{
#ifdef OPENGL
  CGerror err;
  if (err=cgGetError()) {
    cerr << cgGetErrorString(err) << ' ' << msg << endl;
    throw LibSL::CgHelpers::CgException("%s %s",cgGetErrorString(err),msg);
  }
  GLenum glerr;
  if (glerr=glGetError()) {
    cerr << "Cg::checkError - gl error - " << msg << endl;
    throw LibSL::CgHelpers::CgException("Cg::checkError - gl error - %s ",msg);
  }
#endif
}

// ------------------------------------------------------
// ------------------------------------------------------

void NAMESPACE::CgShader::init(const char  *code,const char **args)
{
  sl_dbg_assert(s_CgInitCounter >= 0);

  static const char* compiler_args[128];
  uint nargs=0;
#ifdef OPENGL
  compiler_args[nargs++]="-DOPENGL";
  compiler_args[nargs++]="-DCG_PROFILE_VP=vp40";
  compiler_args[nargs++]="-DCG_PROFILE_FP=fp40";
  //compiler_args[nargs++]="-DCG_PROFILE_VP=glslv";
  //compiler_args[nargs++]="-DCG_PROFILE_FP=glslf";
#endif
#ifdef DIRECT3D
  compiler_args[nargs++]="-DDIRECT3D";
  compiler_args[nargs++]="-DCG_PROFILE_VP=vs_3_0";
  compiler_args[nargs++]="-DCG_PROFILE_FP=ps_3_0";
#endif
  if (args != NULL) {
    for (uint n=0;args[n]!=NULL;n++) {
      sl_assert(nargs < 128);
      compiler_args[nargs++]=args[n];
    }
  }
  sl_assert(nargs < 128);
  compiler_args[nargs++]=NULL;
/*
  // DEBUG
    cerr << code;
    for (uint n=0;compiler_args[n]!=NULL;n++) {
    cerr << LibSL::CppHelpers::sprint("[%02d] %s\n",n,compiler_args[n]);
    }
*/
  if (s_CgInitCounter == 0) {
    cgMainInit();
  }
  s_CgInitCounter++;

  if (m_cgShader != NULL) {
    cgDestroyEffect(m_cgShader);
  }
  
  m_Name = string("[dynamic]");
  m_cgShader = cgCreateEffect(g_cgContext,code,compiler_args);
  if (m_cgShader == NULL) {
    /*
    // DEBUG
    ofstream o("error.cg");
    o << code;
    o.close();
    */
    throw CgException("CgShader::init - cgCreateEffect failed !");
  }
  m_ActiveTechnique = cgGetFirstTechnique(m_cgShader);
}

// -----------------------------------------------------

void NAMESPACE::CgShader::init(CGeffect effect)
{
  if (m_cgShader != NULL) {
    cgDestroyEffect(m_cgShader);
  } 
  m_cgShader=effect;
  m_Name=string("[runtime]");
}

// -----------------------------------------------------

void NAMESPACE::CgShader::setActiveTechnique(CgTechnique& tech)
{
  m_ActiveTechnique=tech.handle();
}

// -----------------------------------------------------

void NAMESPACE::CgShader::pushTechnique() 
{
  m_TechniqueStack.push_back(m_ActiveTechnique);
}

// -----------------------------------------------------

void NAMESPACE::CgShader::popTechnique()  
{
  m_ActiveTechnique=m_TechniqueStack.back();
  m_TechniqueStack.pop_back();
}

// -----------------------------------------------------

void NAMESPACE::CgShader::terminate()
{
  if (m_cgShader != NULL) {
    cgDestroyEffect(m_cgShader);
    m_cgShader=NULL;
    s_CgInitCounter--;
    if (s_CgInitCounter==0) {
      cgMainTerminate();
    }
  }
}

// -----------------------------------------------------

void NAMESPACE::CgShader::commit()
{
  // commit changes...
}

// -----------------------------------------------------

void NAMESPACE::CgShader::authorize() const
{
  if (m_cgShader == NULL)
    throw CgException("CgShader::authorize - shader used without having been initialized !");
}

// -----------------------------------------------------

void NAMESPACE::CgShader::begin()
{
  authorize(); 
  sl_assert(m_ActiveTechnique != NULL);
  sl_assert(m_ActivePass      == NULL);
  m_ActivePass = cgGetFirstPass(m_ActiveTechnique);
  cgSetPassState(m_ActivePass);
  checkError("(begin)");
}

// -----------------------------------------------------

bool NAMESPACE::CgShader::nextPass()
{
  authorize(); 
  cgResetPassState(m_ActivePass);
  m_ActivePass = cgGetNextPass(m_ActivePass);
  if (m_ActivePass == NULL) {
    return (false);
  }
  cgSetPassState(m_ActivePass);
  checkError("(nextPass)");
  return (true);
}

// -----------------------------------------------------

void NAMESPACE::CgShader::end()
{
  authorize();
  if (m_ActivePass != NULL) {
    cgResetPassState(m_ActivePass);
    m_ActivePass = NULL;
  }
  checkError("(end)");
}

// -----------------------------------------------------

const char **NAMESPACE::CgShader::cgDirectories() 
{
  return ((const char **)s_CgDirs);
}

// -----------------------------------------------------
// -----------------------------------------------------
// -----------------------------------------------------

void NAMESPACE::CgParameter::init(CgShader& shader,const char *name)
{
  if (!shader.isReady())
    throw CgException("CgParameter::init - shader '%s' - parameter '%s' - Shader is not ready ! (call init before load parameters)",shader.name(),name);
  m_cgShader = shader.effect();
  m_Name     = string(name);
  m_cgHandle = cgGetNamedEffectParameter(m_cgShader,name);
  if (m_cgHandle == NULL)
    throw CgException("CgParameter::init - shader '%s' - parameter '%s' does not exist !",shader.name(),name);
}

// -----------------------------------------------------

void NAMESPACE::CgParameter::init(CgShader& shader,const char *name,uint posinarray)
{
  if (!shader.isReady())
    throw CgException("CgParameter::init - shader '%s' - array parameter '%s' - Shader is not ready ! (call init before load parameters)",shader.name(),name);
  m_cgShader = shader.effect();
  m_Name     = string(name);
  CGparameter cgarray = cgGetNamedEffectParameter(m_cgShader,name);
  if (cgarray == NULL)
    throw CgException("CgParameter::init - shader '%s' - array parameter '%s' does not exist !",shader.name(),name);
  m_cgHandle = cgGetArrayParameter(cgarray,posinarray);
  if (m_cgHandle == NULL)
    throw CgException("CgParameter::init - shader '%s' - array parameter '%s[%d]' does not exist !",shader.name(),name,posinarray);
}

// -----------------------------------------------------

void NAMESPACE::CgParameter::authorize() const
{
  if (m_cgShader == NULL || m_cgHandle == NULL)
    throw CgException("CgParameter::authorize - parameter used without having been initialized !");
}

// -----------------------------------------------------

void NAMESPACE::CgParameter::set(float f) 
{
  authorize(); cgSetParameter1f(m_cgHandle,f);
}

void NAMESPACE::CgParameter::set(float a,float b)
{
  authorize(); cgSetParameter2f(m_cgHandle,a,b);
}

void NAMESPACE::CgParameter::set(float a,float b,float c)
{
  authorize(); cgSetParameter3f(m_cgHandle,a,b,c);
}

void NAMESPACE::CgParameter::set(float a,float b,float c,float d)
{
  authorize(); cgSetParameter4f(m_cgHandle,a,b,c,d);
}

void NAMESPACE::CgParameter::set(const LibSL::Math::Tuple<float,2>& t)
{
  authorize(); cgSetParameter2f(m_cgHandle,t[0],t[1]);
}

void NAMESPACE::CgParameter::set(const LibSL::Math::Tuple<float,3>& t)
{
  authorize(); cgSetParameter3f(m_cgHandle,t[0],t[1],t[2]);
}

void NAMESPACE::CgParameter::set(const LibSL::Math::Tuple<float,4>& t)
{
  authorize(); cgSetParameter4f(m_cgHandle,t[0],t[1],t[2],t[3]);
}

void NAMESPACE::CgParameter::set(const float *matrix)
{
  authorize(); cgSetMatrixParameterfc(m_cgHandle,matrix);
}

void NAMESPACE::CgParameter::set(const LibSL::Math::m4x4f& matrix)
{
  authorize(); cgSetMatrixParameterfr(m_cgHandle,&(matrix.at(0,0)));
}

/*void NAMESPACE::CgParameter::setArray(const float *pv,int size)
{
  authorize(); cgSetParameterArray1f(m_cgHandle,0,size,pv);
}*/

// -----------------------------------------------------

#ifdef OPENGL

// -----------------------------------------------------

void NAMESPACE::CgParameter::set(GLuint tex)
{
  authorize(); 
  glBindTexture(GL_TEXTURE_2D,tex);
  cgGLSetTextureParameter(m_cgHandle,tex); 
  cgSetSamplerState(m_cgHandle);
}

void NAMESPACE::CgParameter::setAPIMatrix(uint matrixid,uint flags)
{
  authorize();
  CGGLenum cgmatr=CGGLenum(0);
  CGGLenum cgflags=CG_GL_MATRIX_IDENTITY;
  switch (matrixid) {
  case LIBSL_PROJECTION_MATRIX:           cgmatr=CG_GL_PROJECTION_MATRIX; break;
  case LIBSL_MODELVIEW_MATRIX:            cgmatr=CG_GL_MODELVIEW_MATRIX; break;
  case LIBSL_MODELVIEW_PROJECTION_MATRIX: cgmatr=CG_GL_MODELVIEW_PROJECTION_MATRIX; break;
  default: sl_assert(false);
  }
  if ((flags & LIBSL_INVERSE) && (flags & LIBSL_TRANSPOSE)) {
    cgflags=CG_GL_MATRIX_INVERSE_TRANSPOSE;
  } else if (flags & LIBSL_INVERSE) {
    cgflags=CG_GL_MATRIX_INVERSE;
  } else if (flags & LIBSL_TRANSPOSE) {
    cgflags=CG_GL_MATRIX_TRANSPOSE;
  }
  cgGLSetStateMatrixParameter(m_cgHandle,cgmatr,cgflags);
}

void NAMESPACE::CgParameter::set(uint num,const LibSL::Math::m4x4f *matrices)
{
  cgGLSetMatrixParameterArrayfr(m_cgHandle,0,num,(const float *)matrices);
}

#endif

// -----------------------------------------------------

#ifdef DIRECT3D

// -----------------------------------------------------

void NAMESPACE::CgParameter::set(LPDIRECT3DBASETEXTURE9 tex)
{
  authorize();
  //cgD3D9SetupSampler(m_cgHandle,tex); 
  cgD3D9SetTextureParameter(m_cgHandle,tex); 
  cgSetSamplerState(m_cgHandle);
}

void NAMESPACE::CgParameter::setAPIMatrix(uint matrixid,uint flags)
{
  authorize();
  D3DXMATRIX m;
  LPDIRECT3DDEVICE9 d3d=LIBSL_DIRECT3D_DEVICE;
  switch (matrixid) {
  case LIBSL_PROJECTION_MATRIX:
    d3d->GetTransform(D3DTS_PROJECTION,&m);
    break;
  case LIBSL_MODELVIEW_MATRIX:
    d3d->GetTransform(D3DTS_WORLD,&m);
    break;
  case LIBSL_MODELVIEW_PROJECTION_MATRIX:
    {
      D3DXMATRIX p,v;
      d3d->GetTransform(D3DTS_PROJECTION,&p);
      d3d->GetTransform(D3DTS_WORLD,&v);
      D3DXMatrixMultiply(&m,&v,&p);
      break;
    }
  default: sl_assert(false);
  }
  if ((flags & LIBSL_INVERSE) && (flags & LIBSL_TRANSPOSE)) {
    FLOAT det;
    D3DXMatrixInverse(&m,&det,&m);
    D3DXMatrixTranspose(&m,&m);
  } else if (flags & LIBSL_INVERSE) {
    FLOAT det;
    D3DXMatrixInverse(&m,&det,&m);
  } else if (flags & LIBSL_TRANSPOSE) {
    D3DXMatrixTranspose(&m,&m);
  }
  set((float *)&m);
}

void NAMESPACE::CgParameter::set(uint num,const LibSL::Math::m4x4f *matrices)
{
  cgD3D9SetUniformMatrixArray(m_cgHandle,0,num,(const D3DXMATRIX *)matrices);
}

#endif



// -----------------------------------------------------
// -----------------------------------------------------

void NAMESPACE::CgTechnique::init(CgShader& shader,const char *name)
{
  m_Shader = &shader;
  if (!shader.isReady())
    throw CgException("CgTechnique::init - shader '%s' - technique '%s' - Shader is not ready ! (call init before load parameters)",shader.name(),name);
  m_cgHandle = cgGetNamedTechnique(shader.effect(),name);
  if (m_cgHandle == NULL)
    throw CgException("CgTechnique::init - shader '%s' - technique '%s' does not exist !",shader.name(),name);
  if (!cgValidateTechnique(m_cgHandle))
    throw CgException("CgTechnique::init - shader '%s' - technique '%s' is not valid !",shader.name(),name);

  shader.setActiveTechnique(*this);

  m_Name=string(name);
}

// -----------------------------------------------------

void NAMESPACE::CgTechnique::authorize() const
{
  if (m_Shader == NULL || m_cgHandle == NULL)
    throw CgException("CgTechnique::authorize - technique used without having been initialized !");
}

// -----------------------------------------------------

void NAMESPACE::CgTechnique::activate()
{
  authorize();
  m_Shader->setActiveTechnique(*this);
}

// -----------------------------------------------------
