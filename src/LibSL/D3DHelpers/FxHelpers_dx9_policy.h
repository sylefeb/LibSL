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
//
// Fx Effect DX9 policy
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-08-21
// ------------------------------------------------------

#include <d3d9.h>
#include <d3dx9.h>

class T_DX9ShaderPolicy
{
public:

  typedef LPD3DXEFFECT            t_Shader;
  typedef D3DXHANDLE              t_Technique;
  typedef D3DXHANDLE              t_Parameter;
  typedef D3DXMACRO               t_Defines;
  typedef LPDIRECT3DBASETEXTURE9  t_Texture;

  class t_InitData 
  {
  public:
    t_InitData() { }
  };

  class FxIncludeManager : public ID3DXInclude
  {
  protected:
    LibSL::Memory::Array::Array<uchar> m_Data;
  public:
    FxIncludeManager() { }
    STDMETHOD(Open)(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
    {
      std::string content = LibSL::StlHelpers::loadFileIntoString(pFileName);
      m_Data.erase();
      m_Data.allocate(uint(content.length()));
      memcpy(m_Data.raw(),content.c_str(),content.length());
      *ppData = m_Data.raw();
      *pBytes = m_Data.size();
      return S_OK;
    }
    STDMETHOD(Close)(LPCVOID pData)
    {
      return S_OK;
    }
  };


  // Create a shader from a code string
  t_Shader create(
    const char  *code,
    DWORD        flags,
    t_Defines   *defines,
    const t_InitData& d = t_InitData()) const
  {
    LIBSL_BEGIN;
    ID3DXBuffer *errors;
    D3DXCreateBuffer(1024,&errors);
    FxIncludeManager inclmanager;
    t_Shader shader;
    LIBSL_D3D_CHECK_ERROR_DISPLAY( D3DXCreateEffect(
      LIBSL_DIRECT3D_DEVICE, 
      code,(UINT)strlen(code),
      defines, 
      &inclmanager, 
      flags,
      NULL,
      &shader,
      &errors) );
    LIBSL_D3D_SAFE_RELEASE(errors);
    return (shader);
    LIBSL_END;
  }

  // Destroy a shader
  void destroy(t_Shader e) const
  {
    LIBSL_D3D_SAFE_RELEASE(e);
  }

  // Commit any changes
  void commit(t_Shader e) const
  {
    checkShaderReady(e);
    e->CommitChanges();
  }

  int  begin(t_Shader e) const
  {
    checkShaderReady(e);
    UINT numPasses; 
    e->Begin(&numPasses,0); 
    e->BeginPass(0);
    return (numPasses);
  }

  void nextPass(t_Shader e,int p) const
  {
    checkShaderReady(e);
    e->EndPass(); 
    e->BeginPass(p); 
  }

  void end(t_Shader e) const
  {
    checkShaderReady(e);
    e->EndPass(); 
    e->End(); 
  }

  t_Technique currentTechnique(t_Shader e) const
  {
    checkShaderReady(e);
    return (e->GetCurrentTechnique());
  }

  // Determines if the shader is ready to use
  void checkShaderReady(const t_Shader e) const 
  {
    if (e == NULL) {
      throw FxException("FxShader - shader used before initialization!");
    }
  }

  // Load a technique
  t_Technique loadTechnique(t_Shader e,const char *name) const
  {
    checkShaderReady(e);
    t_Technique h = e->GetTechniqueByName(name);
    if (h == NULL) {
      throw FxException("FxTechnique::init - technique '%s' does not exist !",name);
    }
    if (S_OK != e->ValidateTechnique(h)) {
      throw FxException("FxTechnique::init - technique '%s' is not valid !",name);
    }
    e->SetTechnique(h);
    return (h);
  }

  // Activate a technique
  void activateTechnique(t_Shader e,t_Technique t) const
  {
    checkShaderReady(e);
    LIBSL_D3D_CHECK_ERROR(e->SetTechnique(t));
  }


  // Load a parameter
  t_Parameter loadParameter(t_Shader e,const char *name) const
  {
    checkShaderReady(e);
    t_Parameter h = e->GetParameterByName(NULL,name);
    if (h == NULL) {
      throw FxException("FxParameter::init - parameter '%s' does not exist !",name);
    }
    return (h);
  }

  // Load an array parameter
  t_Parameter loadParameter(t_Shader e,const char *name,uint posinarray) const
  {
    checkShaderReady(e);
    t_Parameter ha = e->GetParameterByName(NULL,name);
    if (ha == NULL)
      throw FxException("FxParameter::init - array parameter '%s'[%d] does not exist !",name,posinarray);
    t_Parameter h = e->GetParameterElement(ha,posinarray);
    if (h == NULL) {
      throw FxException("FxParameter::init - array '%s' does not exist (entry %d) !",name,posinarray);
    }
    return (h);
  }

  // Determines if a parameter is ready
  void checkParamReady(const t_Parameter p) const 
  {
    if (p == NULL) {
      throw FxException("FxParameter - parameter used before initialization!");
    }
  }

  /// Set methods

  // Set a matrix parameter to a matrix API
  void setAPIMatrix(t_Shader e,t_Parameter p,uint matrixid,uint flags) const
  {
    checkShaderReady(e);
    checkParamReady(p);
    D3DXMATRIX m;
    LPDIRECT3DDEVICE d3d = LIBSL_DIRECT3D_DEVICE;
    switch (matrixid) 
    {
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
    } else if (flags & LIBSL_INVERSE) {
      FLOAT det;
      D3DXMatrixInverse(&m,&det,&m);
      D3DXMatrixTranspose(&m,&m);
    } else if (flags & LIBSL_TRANSPOSE) {
    } else {
      D3DXMatrixTranspose(&m,&m);
    }
    set(e,p,m);
  }

  void set(t_Shader e,t_Parameter p,float f) const             
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(e->SetFloat(p,f)); }

  void set(t_Shader e,t_Parameter p,float a,float b) const           
  { checkParamReady(p); D3DXVECTOR4 v(a,b,0,0); LIBSL_D3D_CHECK_ERROR(e->SetVector(p,&v)); }

  void set(t_Shader e,t_Parameter p,float a,float b,float c) const
  { checkParamReady(p); D3DXVECTOR4 v(a,b,c,0); LIBSL_D3D_CHECK_ERROR(e->SetVector(p,&v)); }

  void set(t_Shader e,t_Parameter p,float a,float b,float c,float d) const
  { checkParamReady(p); D3DXVECTOR4 v(a,b,c,d); LIBSL_D3D_CHECK_ERROR(e->SetVector(p,&v)); }

  void set(t_Shader e,t_Parameter p,int i) const
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(e->SetInt(p,i)); }

  void set(t_Shader e,t_Parameter p,const D3DXVECTOR4& v) const
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(e->SetVector(p,&v)); }

  void set(t_Shader e,t_Parameter p,const D3DXVECTOR4 *pv) const
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(e->SetVector(p,pv)); }

  void set(t_Shader e,t_Parameter p,const D3DXVECTOR3& v) const
  { checkParamReady(p); D3DXVECTOR4 v4(v.x,v.y,v.z,0); LIBSL_D3D_CHECK_ERROR(e->SetVector(p,&v4)); }

  void set(t_Shader e,t_Parameter p,const D3DXVECTOR2& v) const
  { checkParamReady(p); D3DXVECTOR4 v4(v.x,v.y,0,0); LIBSL_D3D_CHECK_ERROR(e->SetVector(p,&v4)); }

  void setArray(t_Shader e,t_Parameter p,const D3DXVECTOR4 *pv,int size) const
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(e->SetVectorArray(p,pv,size)); }

  void set(t_Shader e,t_Parameter p,const LibSL::Math::v2f& v) const
  { checkParamReady(p); D3DXVECTOR4 v4(v[0],v[1],0,0); LIBSL_D3D_CHECK_ERROR(e->SetVector(p,&v4)); }

  void set(t_Shader e,t_Parameter p,const LibSL::Math::v3f& v) const
  { checkParamReady(p); D3DXVECTOR4 v4(v[0],v[1],v[2],0); LIBSL_D3D_CHECK_ERROR(e->SetVector(p,&v4)); }

  void set(t_Shader e,t_Parameter p,const LibSL::Math::v4f& v) const
  { checkParamReady(p); D3DXVECTOR4 v4(v[0],v[1],v[2],v[3]); LIBSL_D3D_CHECK_ERROR(e->SetVector(p,&v4)); }

  void set(t_Shader e,t_Parameter p,t_Texture tex) const
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(e->SetTexture(p,tex)); }

  void set(t_Shader e,t_Parameter p,const D3DXMATRIX& matr) const
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(e->SetMatrix(p,&matr)); }

  void set(t_Shader e,t_Parameter p,const D3DXMATRIX *pmatr) const
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(e->SetMatrix(p,pmatr)); }

  void set(t_Shader e,t_Parameter p,const LibSL::Math::m4x4f& matr) const
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(e->SetMatrix(p,(D3DXMATRIX *)&(matr.at(0,0)))); }

  void set(t_Shader e,t_Parameter p,bool b) const
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(e->SetBool(p,b)); }

};
