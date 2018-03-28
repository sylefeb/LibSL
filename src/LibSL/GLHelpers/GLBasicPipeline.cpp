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

#include <string>
#include "GLBasicPipeline.h"

#define NAMESPACE LibSL::GLHelpers

// ------------------------------------------------------

NAMESPACE::GLBasicPipeline *NAMESPACE::GLBasicPipeline::s_Instance = NULL;

// ------------------------------------------------------

static std::string vp_string =
#ifdef LIBSL_OPENGL_CORE_PROFILE
"#version " + std::to_string(LIBSL_OPENGL_MAJOR_VERSION) + std::to_string(LIBSL_OPENGL_MINOR_VERSION) + "0 core\n" +
"in vec4 mvf_vertex;\n"
"in vec4 mvf_texcoord0;\n"
"uniform mat4 u_mdlview;\n"
"uniform mat4 u_proj;\n"
"uniform mat4 u_texmat;\n"
"out vec4 v_tex;\n"
"void main()\n"
"{\n"
"  v_tex = u_texmat * mvf_texcoord0;\n"
"  gl_Position = u_proj * u_mdlview * mvf_vertex;\n"
"}";
#else
"attribute vec4 mvf_vertex;\n"
"attribute vec4 mvf_texcoord0;\n"
"uniform mat4 u_mdlview;\n"
"uniform mat4 u_proj;\n"
"uniform mat4 u_texmat;\n"
"varying vec4 v_tex;\n"
"void main()\n"
"{\n"
"  v_tex = u_texmat * mvf_texcoord0;\n"
"  gl_Position = u_proj * u_mdlview * mvf_vertex;\n"
"}";
#endif

static std::string fp_string =
#ifdef LIBSL_OPENGL_CORE_PROFILE
"#version " + std::to_string(LIBSL_OPENGL_MAJOR_VERSION) + std::to_string(LIBSL_OPENGL_MINOR_VERSION) + "0 core\n" +
"uniform vec4 u_color;\n"
"uniform sampler2D u_tex;\n"
"uniform float u_tex_enabled;\n"
"in vec4 v_tex;\n"
"out vec4 o_color;\n"
"void main()\n"
"{\n"
"  vec4 clr = u_color;"
"  if (u_tex_enabled > 0.0) {\n"
"    clr = clr * texture(u_tex, v_tex.xy);\n"
"  }\n"
"  o_color = clr;\n"
"}\n";
#else
"uniform vec4 u_color;\n"
"uniform sampler2D u_tex;\n"
"uniform float u_tex_enabled;\n"
"varying vec4  v_tex;\n"
"void main()\n"
"{\n"
"  vec4 clr = u_color;"
"  if (u_tex_enabled > 0.0) {\n"
"    clr = clr * texture2D(u_tex, v_tex.xy);\n"
"  }\n"
"  gl_FragColor = clr;\n"
"}\n";
#endif

// ------------------------------------------------------

NAMESPACE::GLBasicPipeline::GLBasicPipeline()
{
  m_Shader.init(vp_string.c_str(), fp_string.c_str());
  m_Projection.init(m_Shader, "u_proj");
  m_Modelview .init(m_Shader, "u_mdlview");
  m_TextureMatrix.init(m_Shader, "u_texmat");
  m_Color.init(m_Shader, "u_color");
  m_TextureEnabled.init(m_Shader, "u_tex_enabled");
  m_Texture   .init(m_Shader, "u_tex");

  m_Shader.begin();
  setProjection(m4x4f::identity());
  setModelview (m4x4f::identity());
  setTextureMatrix(m4x4f::identity());
  m_Modelview.set(m4x4f::identity());
  m_Projection.set(m4x4f::identity());
  m_TextureMatrix.set(m4x4f::identity());
  m_Texture.set(0);
  m_TextureEnabled.set(0.0f);
  m_Color.set(v4f(1,1,1,1));
  m_Shader.end();
}

// ------------------------------------------------------
