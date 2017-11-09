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
// Fx Effect DX10 policy
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-08-21
// ------------------------------------------------------

#include <d3d10_1.h>
#include <d3dx10.h>

#include <LibSL/StlHelpers/StlHelpers.h>

class T_DX10ShaderPolicy
{
public:

  class t_Shader {
  public:
    t_Shader()    { effect = NULL; activeTechnique = NULL; }
    t_Shader(int) { effect = NULL; activeTechnique = NULL; } // enables t_Shader(0)syntax
    ID3D10Effect*          effect;
    ID3D10EffectTechnique* activeTechnique;
  };
  typedef ID3D10EffectTechnique*              t_Technique;
  typedef ID3D10EffectVariable*               t_Parameter;
  typedef D3D10_SHADER_MACRO                  t_Defines;
  typedef ID3D10ShaderResourceView*           t_Texture;

  class t_InitData 
  {
  public:
    t_InitData() { }
  };

  class FxIncludeManager : public ID3D10Include
  {
  protected:
    LibSL::Memory::Array::Array<uchar> m_Data;
  public:
    FxIncludeManager() { }
    STDMETHOD(Open)(D3D10_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
    {
      std::string content = LibSL::StlHelpers::loadFileIntoString(pFileName);
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
    ID3D10Blob *errors;
    D3D10CreateBlob(1024,&errors);
    FxIncludeManager inclmanager;
    t_Shader shader;
    LIBSL_D3D_CHECK_ERROR_DISPLAY( D3DX10CreateEffectFromMemory(
      code,(UINT)strlen(code),
      "[dynamic]",
      defines,
      &inclmanager,
      "fx_4_0",
      D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY | flags,0,
      LIBSL_D3D_DEVICE,
      NULL,
      NULL,
      &shader.effect,
      &errors,
      NULL) );
    LIBSL_D3D_SAFE_RELEASE(errors);
    if (!shader.effect->IsValid()) {
      throw FxException("T_DX10ShaderPolicy - effect is not valid!");
    }
    shader.activeTechnique = shader.effect->GetTechniqueByIndex(0);
    return (shader);
  }

  // Destroy a shader
  void destroy(t_Shader e) const
  {
    LIBSL_D3D_SAFE_RELEASE(e.effect);
  }

  // Commit any changes
  void commit(t_Shader e) const
  {

  }

  int  begin(t_Shader e) const
  {
    checkShaderReady(e);
    D3D10_TECHNIQUE_DESC desc;
    e.activeTechnique->GetDesc(&desc);
    e.activeTechnique->GetPassByIndex(0)->Apply(0); 
    return (desc.Passes);
  }

  void nextPass(t_Shader e,int p) const
  {
    checkShaderReady(e);
    e.activeTechnique->GetPassByIndex(p)->Apply(0);
  }

  void end(t_Shader e) const
  {

  }

  t_Technique currentTechnique(t_Shader e) const
  {
    checkShaderReady(e);
    return (e.activeTechnique);
  }

  // Determines if the shader is ready to use
  void checkShaderReady(const t_Shader e) const 
  {
    if (e.effect == NULL || e.activeTechnique == NULL) {
      throw FxException("FxShader - shader used before initialization!");
    }
  }

  // Load a technique
  t_Technique loadTechnique(t_Shader e,const char *name) const
  {
    checkShaderReady(e);
    t_Technique h = e.effect->GetTechniqueByName(name);
    if (h == NULL) {
      throw FxException("FxTechnique::init - technique '%s' does not exist !",name);
    }
    if (!h->IsValid()) {
      throw FxException("FxTechnique::init - technique '%s' is not valid !",name);
    }
    e.activeTechnique = h;
    return (h);
  }

  // Activate a technique
  void activateTechnique(t_Shader e,t_Technique t) const
  {
    checkShaderReady(e);
    e.activeTechnique = t;
  }


  // Load a parameter
  t_Parameter loadParameter(t_Shader e,const char *name) const
  {
    checkShaderReady(e);
    t_Parameter h = e.effect->GetVariableByName(name);
    if (h == NULL) {
      throw FxException("FxParameter::init - parameter '%s' does not exist !",name);
    }
    return (h);
  }

  // Load an array parameter
  t_Parameter loadParameter(t_Shader e,const char *name,uint posinarray) const
  {
    checkShaderReady(e);
    t_Parameter a = e.effect->GetVariableByName(name);
    if (a == NULL)
      throw FxException("FxParameter::init - array parameter '%s' does not exist !",name,posinarray);
    t_Parameter h = a->GetElement(posinarray);
    if (h == NULL) {
      throw FxException("FxParameter::init - parameter '%s' does not exist !",name);
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
    throw FxException("FxParameter::setAPIMatrix - No API matrices in DX10!");
  }

  void set(t_Shader e,t_Parameter p,float f) const             
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(p->AsScalar()->SetFloat(f)); }

  void set(t_Shader e,t_Parameter p,float a,float b) const           
  { checkParamReady(p); float v[4]={a,b,0,0}; LIBSL_D3D_CHECK_ERROR(p->AsVector()->SetFloatVector(v)); }

  void set(t_Shader e,t_Parameter p,float a,float b,float c) const
  { checkParamReady(p); float v[4]={a,b,c,0}; LIBSL_D3D_CHECK_ERROR(p->AsVector()->SetFloatVector(v)); }

  void set(t_Shader e,t_Parameter p,float a,float b,float c,float d) const
  { checkParamReady(p); float v[4]={a,b,c,d}; LIBSL_D3D_CHECK_ERROR(p->AsVector()->SetFloatVector(v)); }

  void set(t_Shader e,t_Parameter p,int i) const
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(p->AsScalar()->SetInt(i)); }

  void set(t_Shader e,t_Parameter p,const D3DXVECTOR4& v) const
  { checkParamReady(p); float f[4]={v[0],v[1],v[2],v[3]}; LIBSL_D3D_CHECK_ERROR(p->AsVector()->SetFloatVector(f)); }

  void set(t_Shader e,t_Parameter p,const D3DXVECTOR4 *pv) const
  { checkParamReady(p); float f[4]={*pv[0],*pv[1],*pv[2],*pv[3]}; LIBSL_D3D_CHECK_ERROR(p->AsVector()->SetFloatVector(f)); }

  void set(t_Shader e,t_Parameter p,const D3DXVECTOR3& v) const
  { checkParamReady(p); float f[4]={v[0],v[1],v[2],0}; LIBSL_D3D_CHECK_ERROR(p->AsVector()->SetFloatVector(f)); }

  void set(t_Shader e,t_Parameter p,const D3DXVECTOR2& v) const
  { checkParamReady(p); float f[4]={v[0],v[1],0,0}; LIBSL_D3D_CHECK_ERROR(p->AsVector()->SetFloatVector(f)); }

  void setArray(t_Shader e,t_Parameter p,const D3DXVECTOR4 *pv,int size) const
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(p->AsVector()->SetFloatVectorArray(const_cast<float*>(&pv[0][0]),0,size)); }

  void set(t_Shader e,t_Parameter p,const LibSL::Math::v2f& v) const
  { checkParamReady(p); float f[4]={v[0],v[1],0,0}; LIBSL_D3D_CHECK_ERROR(p->AsVector()->SetFloatVector(f)); }

  void set(t_Shader e,t_Parameter p,const LibSL::Math::v3f& v) const
  { checkParamReady(p); float f[4]={v[0],v[1],v[2],0}; LIBSL_D3D_CHECK_ERROR(p->AsVector()->SetFloatVector(f)); }

  void set(t_Shader e,t_Parameter p,const LibSL::Math::v4f& v) const
  { checkParamReady(p); float f[4]={v[0],v[1],v[2],v[3]}; LIBSL_D3D_CHECK_ERROR(p->AsVector()->SetFloatVector(f)); }

  void set(t_Shader e,t_Parameter p,t_Texture tex) const
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(p->AsShaderResource()->SetResource(tex)); }

  void set(t_Shader e,t_Parameter p,const D3DXMATRIX& matr) const
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(p->AsMatrix()->SetMatrix(const_cast<float*>(&matr[0]))); }

  void set(t_Shader e,t_Parameter p,const D3DXMATRIX *pmatr) const
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(p->AsMatrix()->SetMatrix(const_cast<float*>(&(*pmatr[0])))); }

  void set(t_Shader e,t_Parameter p,const LibSL::Math::m4x4f& matr) const
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(p->AsMatrix()->SetMatrix(const_cast<float*>(&(matr.at(0,0))))); }

  void set(t_Shader e,t_Parameter p,bool b) const
  { checkParamReady(p); LIBSL_D3D_CHECK_ERROR(p->AsScalar()->SetBool(b)); }

};

