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
// LibSL::GLBasicPipeline
// ------------------------------------------------------
//
// OpenGL basic pipeline for platforms that do not support
// the fixed pipeline anymore
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2016-05-22
// ------------------------------------------------------

#pragma once

// ------------------------------------------------------

#include <LibSL/Errors/Errors.h>
#include <LibSL/System/Types.h>
#include <LibSL/Memory/Array.h>
#include <LibSL/Memory/Pointer.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Math/Matrix4x4.h>

using namespace LibSL::System::Types;

#include <LibSL/GLHelpers/GLHelpers.h>

// ------------------------------------------------------

namespace LibSL {
  namespace GLHelpers {

    class GLBasicPipeline
    {
    private:

      static GLBasicPipeline *s_Instance;

      GLShader    m_Shader;
      GLParameter m_Projection;
      GLParameter m_Modelview;
      GLParameter m_TextureMatrix;
      GLParameter m_Color;
      GLParameter m_TextureEnabled;
      GLParameter m_Texture;

      GLBasicPipeline();

      m4x4f m_mat_Projection;
      m4x4f m_mat_Modelview;
      m4x4f m_mat_Texture;

    public:

      static void init()
      {
        s_Instance = new GLBasicPipeline();
      }

      static void terminate()
      {
        if (s_Instance != NULL) {
          delete (s_Instance);
          s_Instance = NULL;
        }
      }

      static GLBasicPipeline *getUniqueInstance()
      {
        sl_assert(s_Instance != NULL);
        return s_Instance;
      }

      void setModelview(const m4x4f& m)
      {
        m_mat_Modelview = m;
        m_Modelview.set(m_mat_Modelview);
      }

      void setProjection(const m4x4f& m)
      {
        m_mat_Projection = m;
        m_Projection.set(m_mat_Projection);
      }

      void setTextureMatrix(const m4x4f& m)
      {
        m_mat_Texture = m;
        m_TextureMatrix.set(m_mat_Texture);
      }

      void begin()
      {
        m_Shader.begin();
        m_Modelview.set(m_mat_Modelview);
        m_Projection.set(m_mat_Projection);
        m_TextureMatrix.set(m_mat_Texture);
      }

      void end()
      {
        m_Shader.end();
      }

      void bindTextureUnit(GLint id)
      {
        m_Texture.set(id);
      }
      
      void enableTexture()
      {
        m_TextureEnabled.set(1.0f);
      }
      void disableTexture()
      {
        m_TextureEnabled.set(0.0f);
      }

      void setColor(const v4f& clr) 
      {
        m_Color.set(clr);
      }

      const m4x4f& getModelview()     const { return m_mat_Modelview;  }
      const m4x4f& getProjection()    const { return m_mat_Projection; }
      const m4x4f& getTextureMatrix() const { return m_mat_Texture; }

    };

  } //namespace LibSL::GLHelpers
} //namespace LibSL

// ------------------------------------------------------
