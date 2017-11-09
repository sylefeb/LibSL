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
// LibSL::CgHelpers
// ------------------------------------------------------
//
// Cg effects helpers (multiplatform)
// 
// OPENGL or DIRECT3D must be define at compile time
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-23
// ------------------------------------------------------

#pragma once

// ------------------------------------------------------

#ifndef OPENGL
#ifndef DIRECT3D
#ifndef DIRECTX10
ERROR_____OPENGL_or_DIRECT3D_or_DIRECTX10_must_be_defined _;
#endif
#endif
#endif

#ifdef OPENGL
#ifdef DIRECT3D
ERROR_____either_OPENGL_or_DIRECT3D_must_be_defined__not_both _;
#endif
#ifdef DIRECTX10
ERROR_____either_OPENGL_or_DIRECTX10_must_be_defined__not_both _;
#endif
#endif

#ifdef DIRECTX10
ERROR_____no_Cg_support_for_DIRECTX10 _;
#endif

// ------------------------------------------------------

#include <LibSL/LibSL.common.h>

#include <LibSL/Errors/Errors.h>
#include <LibSL/System/Types.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Matrix4x4.h>
using namespace LibSL::System::Types;

#include <LibSL/GPUHelpers/GPUHelpers.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <Cg/cg.h>

#ifdef OPENGL
#include <GL/gl.h>
#endif

#ifdef DIRECT3D
#include <d3d9.h>
#endif

#include <iostream>
#include <string>
#include <list>

// ------------------------------------------------------

namespace LibSL {
  namespace CgHelpers {

    LIBSL_NEW_EXCEPTION_TYPE(CgException)

    class CgParameter;
    class CgTechnique;

    class LIBSL_DLL CgShader
    {
    private:

      static char          *s_CgDirs[16];

      std::string            m_Name;
      std::list<CGtechnique> m_TechniqueStack;
      CGeffect               m_cgShader;
      CGpass                 m_ActivePass;
      CGtechnique            m_ActiveTechnique;

      static int             s_CgInitCounter; // Counter for Cg init

      // check everything is properly loaded
      void authorize() const;

    public:

      CgShader() : m_cgShader(NULL), m_ActivePass(NULL), m_ActiveTechnique(NULL)
      {	m_Name="[null]"; }

      void init(const char *code,const char **args=NULL);

      void init(CGeffect shader);

      void terminate();
      void commit();

      void begin();
      bool nextPass();
      void end();

      void pushTechnique();
      void popTechnique();

      void setActiveTechnique(CgTechnique& tech);

      CGeffect                      effect()     const {return (m_cgShader);}
      const char *                  name()       const {return (m_Name.c_str());}

      bool  isReady() const {return (m_cgShader!=NULL);}

      static const char **cgDirectories();

    };


    class LIBSL_DLL CgParameter
    {
    protected:

      // handle to shader to which the parameter is bound
      CGeffect         m_cgShader;

      // handle to parameter
      CGparameter      m_cgHandle;

      // parameter name
      std::string      m_Name;

      // check everything is properly loaded
      void authorize() const;

    public:

      CgParameter() : m_cgShader(NULL), m_cgHandle(NULL) { m_Name = "[null]";}

      // init
      void init(CgShader& shader,const char *name);
      void init(CgShader& shader,const char *name,uint posinarray);

      // set methods

      void set(float f);
      void set(float a,float b);
      void set(float a,float b,float c);
      void set(float a,float b,float c,float d);
      void set(const LibSL::Math::Tuple<float,2>& );
      void set(const LibSL::Math::Tuple<float,3>& );
      void set(const LibSL::Math::Tuple<float,4>& );
      void set(const float *matrix);
      void set(const LibSL::Math::m4x4f& matrix);
      void set(uint num,const LibSL::Math::m4x4f *matrices);
      //void set(const GPUTex&);
      //void set(const GPUTex*);
#ifdef OPENGL
      void set(GLuint tex);
#endif
#ifdef DIRECT3D
      void set(LPDIRECT3DBASETEXTURE9 tex);
#endif
      void setAPIMatrix(uint matrixid,uint flags=0);

      CGparameter handle() const {return m_cgHandle;}
    };

    class LIBSL_DLL CgTechnique
    {
    protected:

      // shader to which the parameter is bound
      CgShader   *m_Shader;

      // handle to technique
      CGtechnique m_cgHandle;

      // parameter name
      std::string      m_Name;

    public:

      CgTechnique() : m_Shader(NULL), m_cgHandle(NULL) {}

      // init
      void init(CgShader& shader,const char *name);

      // activate this technique (de-activate any other)
      void activate();

      // return handle
      CGtechnique handle() {return (m_cgHandle);}

      // check everything is properly loaded
      void authorize() const;
    };

    // -----------------------------------------------------

  } //namespace LibSL::CgHelpers
} //namespace LibSL

// ------------------------------------------------------
