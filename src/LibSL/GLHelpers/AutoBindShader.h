#pragma once

#include <LibSL/LibSL.h>
#ifdef OPENGL4
#include <LibSL/LibSL_gl4.h>
#else
#include <LibSL/LibSL_gl.h>
#endif
#include <string>
#include <fstream>

namespace AutoBindShader {

  // ---------------------

  template<class T_Precompiled>
  class Shader : public T_Precompiled
  {
  protected:

    LibSL::GLHelpers::GLShader m_Shader;
    bool	   m_FirstInit;
    bool	   m_WriteShaderFiles;

  public:

    void init(GLuint typeIn = 0, GLuint typeOut = 0, uint maxVerticesOut = 0)
    {
      if (m_FirstInit) {
        T_Precompiled::m_TypeIn = typeIn;
        T_Precompiled::m_TypeOut = typeOut;
        T_Precompiled::m_MaxVerticesOut = maxVerticesOut;
      } else {
        typeIn = T_Precompiled::m_TypeIn;
        typeOut = T_Precompiled::m_TypeOut;
        maxVerticesOut = T_Precompiled::m_MaxVerticesOut;
      }
      std::string vp_code = T_Precompiled::vpCode();
      std::string fp_code = T_Precompiled::fpCode();
      if (m_WriteShaderFiles) {
        std::ofstream file_vp((T_Precompiled::name() + std::string("_auto.vp")).c_str());
        file_vp << vp_code.c_str();
        file_vp.close();
      }
      if (m_WriteShaderFiles) {
        std::ofstream file_fp((T_Precompiled::name() + std::string("_auto.fp")).c_str());
        file_fp << fp_code.c_str();
        file_fp.close();
      }
      LibSL::GLHelpers::GLShader::t_GeometryShaderNfo gs = T_Precompiled::gsCode();
      if (!m_FirstInit) {
        m_Shader.terminate();
      }
      m_Shader.init(
        vp_code.length() > 0 ? vp_code.c_str() : NULL,
        fp_code.length() > 0 ? fp_code.c_str() : NULL,
        gs.code.length() > 0 ? &gs : NULL);
      m_Shader.setStrict(false);
      if (m_FirstInit) {
        T_Precompiled::initTweaks();
      }
      T_Precompiled::initParameters(m_Shader, m_FirstInit);
      if (m_FirstInit) {
        m_Shader.begin();
        m_Shader.end();
      }
      m_FirstInit = false;
    }
    void terminate()
    {
      m_Shader.terminate();
    }
    Shader()
    {
      m_FirstInit = true;
      m_WriteShaderFiles = false;
      T_Precompiled::m_TypeIn = 0;
      T_Precompiled::m_TypeOut = 0;
      T_Precompiled::m_MaxVerticesOut = 0;
    }
    LibSL::GLHelpers::GLShader& shader()   { return m_Shader; }
    void    setWriteShaderFiles(bool b)    { m_WriteShaderFiles = b; }
    void	  begin()                        { m_Shader.begin(); T_Precompiled::commitTweaks(); }
    void	  end()	                         { m_Shader.end(); }

#ifdef TW_INCLUDED
    TwBar *makeTwBar() { return T_Precompiled::makeTwBar(this); }
#endif

  };

  // ---------------------

#ifdef OPENGL4

  template<class T_Precompiled>
  class Compute : public T_Precompiled
  {
  protected:

    LibSL::GLHelpers::GLCompute m_Shader;
    bool	    m_FirstInit;
    bool	    m_WriteShaderFiles;

  public:

    void init()
    {
      std::string cs_code = T_Precompiled::csCode();
      if (!m_FirstInit) {
        m_Shader.terminate();
      }
      if (m_WriteShaderFiles) {
        std::ofstream file_cs((T_Precompiled::name() + std::string("_auto.cs")).c_str());
        file_cs << cs_code.c_str();
        file_cs.close();
      }
      m_Shader.init(this->csCode().c_str());
      m_Shader.setStrict( false );
      T_Precompiled::initParameters(m_Shader, m_FirstInit);
      if ( m_FirstInit ) {
        m_Shader.begin();
        m_Shader.end();
      }
      m_FirstInit = false;
    }
    void terminate()
    {
      m_Shader.terminate();
    }
    Compute()
    {
      m_FirstInit = true;
      m_WriteShaderFiles = false;
    }
    LibSL::GLHelpers::GLCompute& shader()                  { return m_Shader;  }
    void    setWriteShaderFiles(bool b) { m_WriteShaderFiles = b; }

    void	  begin() { m_Shader.begin(); T_Precompiled::commitTweaks(); }
    void	  end()	  { m_Shader.end();   }

    void	  run(const LibSL::Math::v3i& numGroups)	{ m_Shader.run(numGroups); }
    void	  run(const LibSL::Math::v3i& numGroups, const LibSL::Math::v3i& groupSize)	{ m_Shader.run(numGroups, groupSize); }

#ifdef TW_INCLUDED
    TwBar *makeTwBar() { return T_Precompiled::makeTwBar(this); }
#endif
  };
  
  template<class T_Precompiled>
  class MeshShader : public T_Precompiled
  {

  public:
    MeshShader()
    {
      m_FirstInit = true;
      m_WriteShaderFiles = false;
    }

    void init()
    {
      std::string ms_code = T_Precompiled::msCode();
      std::string fp_code = T_Precompiled::fpCode();
      std::string ts_code = T_Precompiled::tsCode();
      if (m_WriteShaderFiles) {
        std::ofstream file_ms((T_Precompiled::name() + std::string("_auto.ms")).c_str());
        file_ms << ms_code.c_str();
        file_ms.close();
      }
      if (m_WriteShaderFiles) {
        std::ofstream file_fp((T_Precompiled::name() + std::string("_auto.fp")).c_str());
        file_fp << fp_code.c_str();
        file_fp.close();
      }

      if (!m_FirstInit) {
        m_Shader.terminate();
      }
      m_Shader.init(
            ms_code.length() > 0 ? ms_code.c_str() : nullptr,
            fp_code.length() > 0 ? fp_code.c_str() : nullptr,
            ts_code.length() > 0 ? ts_code.c_str() : nullptr);
      m_Shader.setStrict(false);
      T_Precompiled::initParameters(m_Shader, m_FirstInit);
      if (m_FirstInit) {
        m_Shader.begin();
        m_Shader.end();
      }
      m_FirstInit = false;
    }

    void terminate(){ m_Shader.terminate(); }
    LibSL::GLHelpers::GLMeshShader& shader()   { return m_Shader; }
    void    setWriteShaderFiles(bool b)    { m_WriteShaderFiles = b; }
    void	  begin()                        { m_Shader.begin(); }
    void	  end()	                         { m_Shader.end(); }

    void	  run(const uint first, const uint count)	{ m_Shader.run(first, count); }

  #ifdef TW_INCLUDED
    TwBar *makeTwBar() { return T_Precompiled::makeTwBar(this); }
  #endif

  protected:

    LibSL::GLHelpers::GLMeshShader m_Shader;
    bool	   m_FirstInit;
    bool	   m_WriteShaderFiles;

  };

#endif // OPENGL4

  // ---------------------

  std::string decode(const std::string& str);

  // ---------------------

  template<class T_Precompiled1, class T_Precompiled2>
  class Join : public T_Precompiled1, public T_Precompiled2
  {
  protected:
    std::string vpCode()
    {
      return T_Precompiled1::vpCode() + T_Precompiled2::vpCode();
    }
    std::string fpCode()
    {
      return T_Precompiled1::fpCode() + T_Precompiled2::fpCode();
    }
    void initTweaks()
    {
      T_Precompiled1::initTweaks();
      T_Precompiled2::initTweaks();
    }
    void initParameters()
    {
      T_Precompiled1::initParameters();
      T_Precompiled2::initParameters();
    }
  };

}
