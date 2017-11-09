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
//   FxException
//   FxShader
//   FxParameter
//   FxTechnique
// ------------------------------------------------------
//
// Fx Effect file helpers
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-07
// ------------------------------------------------------
//
// Fx Shader helper tools
// 
// Contains:
// 
// 
// Usage:
// 
// FxShader    sh;
// FxParameter pr;
// FxTechnique tc;
// 
// sh.init(d3d,"sh.fx");             // can be safely called multiple times
// pr.init(sh,"param0");
// tc.init(sh,"solid");
// 
// pr.set(0.2f,0.3f);
// sh.commit();
// tc.activate();
// 
// ... render something ...
// 
// sh.terminate();
// ------------------------------------------------------

#pragma once

// -----------------------------------------------------

#include <list>
#include <string>

#include <windows.h>

#ifdef DIRECT3D
#include <d3d9.h>
#include <d3dx9.h>
#endif

#ifdef DIRECTX10
#include <d3d10_1.h>
#include <d3dx10.h>
#endif

#include "D3DHelpers.h"

#include <LibSL/GPUHelpers/GPUHelpers.h>
#include <LibSL/Math/Matrix4x4.h>

// -----------------------------------------------------

namespace LibSL {
  namespace D3DHelpers {

    LIBSL_NEW_EXCEPTION_TYPE(FxException)

    template <class T_ShaderPolicy>
    class FxShader_generic : public T_ShaderPolicy
    {
    private:

      std::list<typename T_ShaderPolicy::t_Technique>  m_TechniqueStack;
      typename T_ShaderPolicy::t_Shader                m_Shader;
	    bool                                             m_Owner;

    public:

      FxShader_generic() : m_Shader(0) { m_Owner = true; }

      void init(
        const char                         *code,
        DWORD                               flags   = 0,
        typename T_ShaderPolicy::t_Defines *defines = 0)
      {
        LIBSL_BEGIN;
        destroy(m_Shader);
        m_Shader = create(code,flags,defines);
        m_Owner  = true;
        LIBSL_END;
      }

      void init(typename T_ShaderPolicy::t_Shader shader)
      {
        m_Shader = shader;
        m_Owner  = false;
      }

      void terminate()
      {
        if (m_Owner) {
          destroy(m_Shader);
        }
      }
      
      void commit()
      {
        T_ShaderPolicy::commit(m_Shader);
      }

      int  begin() 
      {
        return (T_ShaderPolicy::begin(m_Shader));
      }

      void nextPass(int p)
      {
        T_ShaderPolicy::nextPass(m_Shader,p);      
      }

      void end()
      {
        T_ShaderPolicy::end(m_Shader);
      }

      void pushTechnique() 
      {
        m_TechniqueStack.push_back(currentTechnique(m_Shader));
      }
      
      void popTechnique()  
      {
        activateTechnique(m_Shader,m_TechniqueStack.back()); 
        m_TechniqueStack.pop_back();
      }

      typename T_ShaderPolicy::t_Shader handle()     const {return (m_Shader);}

      bool  isReady() const { return (m_Shader!=NULL); }

    };

    template <class T_ShaderPolicy>
    class FxParameter_generic : public T_ShaderPolicy
    {
    protected:

      typename T_ShaderPolicy::t_Shader    m_Shader; // handle to shader to which the parameter is bound
      typename T_ShaderPolicy::t_Parameter m_Handle; // handle to parameter
      std::string                          m_Name;   // parameter name

    public:

      FxParameter_generic() { m_Name = "[null]"; m_Shader = NULL; m_Handle = NULL; }

      // init
      void init(FxShader_generic<T_ShaderPolicy>& shader,const char *name)
      {
        m_Shader = shader.handle();
        m_Handle = T_ShaderPolicy::loadParameter(shader.handle(),name);
        m_Name   = std::string(name);
      }
      
      void init(FxShader_generic<T_ShaderPolicy>& shader,const char *name,uint posinarray)
      {
        m_Shader = shader.handle();
        m_Handle = T_ShaderPolicy::loadParameter(shader.handle(),name,posinarray);
        m_Name   = std::string(name);
      }

      void set(float f)                         { T_ShaderPolicy::set(m_Shader,m_Handle, f); }
      void set(float a,float b)                 { T_ShaderPolicy::set(m_Shader,m_Handle, a,b); }
      void set(float a,float b,float c)         { T_ShaderPolicy::set(m_Shader,m_Handle, a,b,c); }
      void set(float a,float b,float c,float d) { T_ShaderPolicy::set(m_Shader,m_Handle, a,b,c,d); } 
      void set(int i)                           { T_ShaderPolicy::set(m_Shader,m_Handle, i); } 
      void set(const D3DXVECTOR4& v)            { T_ShaderPolicy::set(m_Shader,m_Handle, v); } 
      void set(const D3DXVECTOR4 *pv)           { T_ShaderPolicy::set(m_Shader,m_Handle, pv);} 
      void set(const D3DXVECTOR3& v)            { T_ShaderPolicy::set(m_Shader,m_Handle, v); } 
      void set(const D3DXVECTOR2& v)            { T_ShaderPolicy::set(m_Shader,m_Handle, v); } 
      void set(const LibSL::Math::v2f& v)       { T_ShaderPolicy::set(m_Shader,m_Handle, v); } 
      void set(const LibSL::Math::v3f& v)       { T_ShaderPolicy::set(m_Shader,m_Handle, v); } 
      void set(const LibSL::Math::v4f& v)       { T_ShaderPolicy::set(m_Shader,m_Handle, v); } 
      void set(const D3DXMATRIX& m)             { T_ShaderPolicy::set(m_Shader,m_Handle, m); } 
      void set(const D3DXMATRIX *pm)            { T_ShaderPolicy::set(m_Shader,m_Handle, pm);}
      void set(const LibSL::Math::m4x4f& m)     { T_ShaderPolicy::set(m_Shader,m_Handle, m); }
      void set(bool b)                          { T_ShaderPolicy::set(m_Shader,m_Handle, b); } 
      void set(typename T_ShaderPolicy::t_Texture t) { T_ShaderPolicy::set(m_Shader,m_Handle, t); } 
      void setArray(const D3DXVECTOR4 *pv,int size)  { T_ShaderPolicy::setArray(m_Shader,m_Handle,pv,size); } 
      void setAPIMatrix(uint matrixid,uint flags=0)  { T_ShaderPolicy::setAPIMatrix(m_Shader,m_Handle,matrixid,flags); }

    };

    template <class T_ShaderPolicy>
    class FxTechnique_generic : public T_ShaderPolicy
    {
    protected:

      typename T_ShaderPolicy::t_Shader    m_Shader; // handle to shader to which the technique belongs
      typename T_ShaderPolicy::t_Technique m_Handle; // handle to technique
      std::string                          m_Name;   // technique name

    public:

      FxTechnique_generic() { m_Name = "[null]"; m_Shader = NULL; m_Handle = NULL; }

      // init
      void init(FxShader_generic<T_ShaderPolicy>& shader,const char *name)
      {
        m_Shader = shader.handle();
        m_Handle = T_ShaderPolicy::loadTechnique(shader.handle(),name);
        m_Name   = std::string(name);
      }
      
      // activate this technique (de-activate any other)
      void activate()
      {
        T_ShaderPolicy::activateTechnique(m_Shader,m_Handle);
      }

    };

    // -----------------------------------------------------

#ifdef DIRECT3D
#   include "FxHelpers_dx9_policy.h"
    typedef FxShader_generic   <T_DX9ShaderPolicy> FxShader;
    typedef FxParameter_generic<T_DX9ShaderPolicy> FxParameter;
    typedef FxTechnique_generic<T_DX9ShaderPolicy> FxTechnique;
#endif

#ifdef DIRECTX10
#   include "FxHelpers_dx10_policy.h"
    typedef FxShader_generic   <T_DX10ShaderPolicy> FxShader;
    typedef FxParameter_generic<T_DX10ShaderPolicy> FxParameter;
    typedef FxTechnique_generic<T_DX10ShaderPolicy> FxTechnique;
#endif

    // -----------------------------------------------------

  } //namespace LibSL::D3DHelpers
} //namespace LibSL

// -----------------------------------------------------

