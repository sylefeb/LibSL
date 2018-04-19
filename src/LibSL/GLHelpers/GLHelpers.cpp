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

#include "LibSL/GLHelpers/GLHelpers.h"
#include "LibSL/CppHelpers/CppHelpers.h"
#include "LibSL/Memory/Array.h"
#include "LibSL/Mesh/VertexFormat.h"
#include "LibSL/GPUMesh/GPUMesh.h"

using namespace LibSL::Memory::Array;
using namespace LibSL::CppHelpers;

#ifndef EMSCRIPTEN
#ifdef USE_GLUX
GLUX_LOAD(GL_ARB_shader_objects);
GLUX_LOAD(GL_ARB_vertex_shader);
GLUX_LOAD(GL_ARB_fragment_shader);
GLUX_LOAD(GL_EXT_geometry_shader4);
GLUX_LOAD(GL_EXT_gpu_shader4);
GLUX_LOAD(GL_ARB_timer_query);
GLUX_LOAD(GL_ARB_texture_buffer_object);

#ifdef OPENGL4
#include "GL_NV_shader_buffer_load.h"

GLUX_LOAD( GL_ARB_shader_image_load_store );
GLUX_LOAD( GL_ARB_map_buffer_range );
GLUX_LOAD( GL_ARB_shader_atomic_counters );
GLUX_LOAD( GL_NV_shader_buffer_load );
GLUX_LOAD( GL_ARB_compute_shader );
#endif
#endif
#endif

// ------------------------------------------------------

#define NAMESPACE LibSL::GLHelpers

// ------------------------------------------------------

using namespace std;
using namespace LibSL::Mesh;

// ------------------------------------------------------

GLhandleARB NAMESPACE::loadGLSLProgram(const char *prg,GLuint type)
{
  GLhandleARB id;

  id = glCreateShaderObjectARB(type);
  glShaderSourceARB(id,1,&prg,NULL);
  glCompileShaderARB(id);

  GLint compiled;
  #ifdef EMSCRIPTEN
  glGetShaderiv(id,GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
  #else
  glGetObjectParameterivARB(id,GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
  #endif
  if (!compiled) {
    cout << "**** GLSL shader failed to compile (" << (type == GL_VERTEX_SHADER ? "vertex" : (type == GL_FRAGMENT_SHADER ? "fragment" : "geometry")) << ") ****" << endl;
    GLint maxLength;
    #ifdef EMSCRIPTEN
    glGetShaderiv(id,GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
    #else
    glGetObjectParameterivARB(id,GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
    #endif
    Array<GLcharARB> infoLog(maxLength+1);
    GLint len = 0;
    glGetInfoLogARB(id, maxLength, &len, infoLog.raw());
    cout << Console::yellow << infoLog.raw() << Console::gray << endl;
    if (0) {
      ofstream out("lasterror.glsl.txt");
      out << prg;
      out << endl;
      out << "************************************" << endl;
      out << infoLog.raw();
      out << endl;
      out.close();
    }
    glDeleteObjectARB(id);
    throw GLException("\n\n**** GLSL shader failed to compile (%s) ****\n%s\n", (type == GL_VERTEX_SHADER ? "vertex" : (type == GL_FRAGMENT_SHADER ? "fragment" : "geometry")), infoLog.raw() != nullptr ? infoLog.raw() : "<unknown error>");
  }

  return (id);
}

// ------------------------------------------------------

void NAMESPACE::GLShader::init(
	const char				  *vp_code,
	const char			    *fp_code,
	const t_GeometryShaderNfo *gs_code,
	const char				  *name)
{
  m_Name   = string(name);
  m_Shader = glCreateProgramObjectARB();

  GLhandleARB vp,fp,gs;

  if (vp_code) {
    vp = loadGLSLProgram(vp_code,GL_VERTEX_SHADER);
    glAttachObjectARB(m_Shader,vp);
  }

  if (fp_code) {
    fp = loadGLSLProgram(fp_code,GL_FRAGMENT_SHADER);
    glAttachObjectARB(m_Shader,fp);
  }

#ifndef EMSCRIPTEN
  if (gs_code) {
    //sl_assert(GLUX_IS_AVAILABLE(GL_EXT_geometry_shader4) == GLUX_AVAILABLE);
#ifdef __APPLE__
    gs = loadGLSLProgram(gs_code->code.c_str(),GL_GEOMETRY_SHADER_EXT);
#else
    gs = loadGLSLProgram(gs_code->code.c_str(),GL_GEOMETRY_SHADER_ARB);
#endif
    glAttachObjectARB(m_Shader,gs);
    LIBSL_GL_CHECK_ERROR;
    /*
    // set default values as linking may fail otherwise
    glProgramParameteriEXT( m_Shader, GL_GEOMETRY_INPUT_TYPE_EXT,   gs_code->typeIn );
    LIBSL_GL_CHECK_ERROR;
    glProgramParameteriEXT(m_Shader, GL_GEOMETRY_OUTPUT_TYPE_EXT, gs_code->typeOut);
    LIBSL_GL_CHECK_ERROR;
    glProgramParameteriEXT(m_Shader, GL_GEOMETRY_VERTICES_OUT_EXT, gs_code->maxVerticesOut);
    LIBSL_GL_CHECK_ERROR;
    */
  }
#endif

#ifdef OPENGL4
  LIBSL_GL_CHECK_ERROR;
  // set default bindings
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl4::mvf_attrib_location<MVF_BASE_POSITION >::value, "mvf_position");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl4::mvf_attrib_location<MVF_BASE_POSITION >::value, "mvf_vertex"); // aliasing
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl4::mvf_attrib_location<MVF_BASE_NORMAL   >::value, "mvf_normal");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl4::mvf_attrib_location<MVF_BASE_COLOR0   >::value, "mvf_color0");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl4::mvf_attrib_location<MVF_BASE_COLOR1   >::value, "mvf_color1");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl4::mvf_attrib_location<MVF_BASE_TEXCOORD0>::value, "mvf_texcoord0");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl4::mvf_attrib_location<MVF_BASE_TEXCOORD1>::value, "mvf_texcoord1");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl4::mvf_attrib_location<MVF_BASE_TEXCOORD2>::value, "mvf_texcoord2");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl4::mvf_attrib_location<MVF_BASE_TEXCOORD3>::value, "mvf_texcoord3");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl4::mvf_attrib_location<MVF_BASE_TEXCOORD4>::value, "mvf_texcoord4");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl4::mvf_attrib_location<MVF_BASE_TEXCOORD5>::value, "mvf_texcoord5");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl4::mvf_attrib_location<MVF_BASE_TEXCOORD6>::value, "mvf_texcoord6");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl4::mvf_attrib_location<MVF_BASE_TEXCOORD7>::value, "mvf_texcoord7");
  LIBSL_GL_CHECK_ERROR;
#else
#ifdef EMSCRIPTEN
  LIBSL_GL_CHECK_ERROR;
  // set default bindings
  glBindAttribLocation(m_Shader, LibSL::GPUMesh::gles::mvf_attrib_location<MVF_BASE_POSITION >::value, "mvf_position");
  glBindAttribLocation(m_Shader, LibSL::GPUMesh::gles::mvf_attrib_location<MVF_BASE_POSITION >::value, "mvf_vertex"); // aliasing
  glBindAttribLocation(m_Shader, LibSL::GPUMesh::gles::mvf_attrib_location<MVF_BASE_NORMAL   >::value, "mvf_normal");
  glBindAttribLocation(m_Shader, LibSL::GPUMesh::gles::mvf_attrib_location<MVF_BASE_COLOR0   >::value, "mvf_color0");
  glBindAttribLocation(m_Shader, LibSL::GPUMesh::gles::mvf_attrib_location<MVF_BASE_COLOR1   >::value, "mvf_color1");
  glBindAttribLocation(m_Shader, LibSL::GPUMesh::gles::mvf_attrib_location<MVF_BASE_TEXCOORD0>::value, "mvf_texcoord0");
  glBindAttribLocation(m_Shader, LibSL::GPUMesh::gles::mvf_attrib_location<MVF_BASE_TEXCOORD1>::value, "mvf_texcoord1");
  glBindAttribLocation(m_Shader, LibSL::GPUMesh::gles::mvf_attrib_location<MVF_BASE_TEXCOORD2>::value, "mvf_texcoord2");
  glBindAttribLocation(m_Shader, LibSL::GPUMesh::gles::mvf_attrib_location<MVF_BASE_TEXCOORD3>::value, "mvf_texcoord3");
  glBindAttribLocation(m_Shader, LibSL::GPUMesh::gles::mvf_attrib_location<MVF_BASE_TEXCOORD4>::value, "mvf_texcoord4");
  glBindAttribLocation(m_Shader, LibSL::GPUMesh::gles::mvf_attrib_location<MVF_BASE_TEXCOORD5>::value, "mvf_texcoord5");
  glBindAttribLocation(m_Shader, LibSL::GPUMesh::gles::mvf_attrib_location<MVF_BASE_TEXCOORD6>::value, "mvf_texcoord6");
  glBindAttribLocation(m_Shader, LibSL::GPUMesh::gles::mvf_attrib_location<MVF_BASE_TEXCOORD7>::value, "mvf_texcoord7");
  LIBSL_GL_CHECK_ERROR;
#else
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl::mvf_attrib_location<MVF_BASE_POSITION >::value, "mvf_position");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl::mvf_attrib_location<MVF_BASE_POSITION >::value, "mvf_vertex"); // aliasing
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl::mvf_attrib_location<MVF_BASE_NORMAL   >::value, "mvf_normal");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl::mvf_attrib_location<MVF_BASE_COLOR0   >::value, "mvf_color0");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl::mvf_attrib_location<MVF_BASE_COLOR1   >::value, "mvf_color1");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl::mvf_attrib_location<MVF_BASE_TEXCOORD0>::value, "mvf_texcoord0");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl::mvf_attrib_location<MVF_BASE_TEXCOORD1>::value, "mvf_texcoord1");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl::mvf_attrib_location<MVF_BASE_TEXCOORD2>::value, "mvf_texcoord2");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl::mvf_attrib_location<MVF_BASE_TEXCOORD3>::value, "mvf_texcoord3");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl::mvf_attrib_location<MVF_BASE_TEXCOORD4>::value, "mvf_texcoord4");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl::mvf_attrib_location<MVF_BASE_TEXCOORD5>::value, "mvf_texcoord5");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl::mvf_attrib_location<MVF_BASE_TEXCOORD6>::value, "mvf_texcoord6");
  glBindAttribLocationARB(m_Shader, LibSL::GPUMesh::gl::mvf_attrib_location<MVF_BASE_TEXCOORD7>::value, "mvf_texcoord7");
#endif
#endif

  glLinkProgramARB(m_Shader);

  GLint linked;
  #ifdef EMSCRIPTEN
  glGetProgramiv(m_Shader,GL_OBJECT_LINK_STATUS_ARB, &linked);
  #else
  glGetObjectParameterivARB(m_Shader,GL_OBJECT_LINK_STATUS_ARB, &linked);
  #endif
  if (!linked) {
    cout << "**** GLSL program failed to link ****" << endl;
    GLint maxLength;
    #ifdef EMSCRIPTEN
    glGetProgramiv(m_Shader,GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
    #else
    glGetObjectParameterivARB(m_Shader,GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
    #endif
    Array<GLcharARB> infoLog(maxLength);
    glGetInfoLogARB(m_Shader, maxLength, NULL, infoLog.raw());
    if (0) {
      ofstream out("lasterror.glsl.txt");
      out << "// ========== VP ===========\n";
      if (vp_code) out << vp_code;
      out << "// ========== FP ===========\n";
      if (fp_code) out << fp_code;
      out.close();
    }
    throw GLException("\n\n**** GLSL program failed to link (%s) ****\n%s",m_Name.c_str(),infoLog.raw());
  }

  LIBSL_GL_CHECK_ERROR;

  if (vp_code) {
    #ifdef EMSCRIPTEN
    glDeleteShader(vp);
    #else
    glDeleteObjectARB(vp);
    #endif
  }
  LIBSL_GL_CHECK_ERROR;
  if (fp_code) {
    #ifdef EMSCRIPTEN
    glDeleteShader(fp);
    #else
    glDeleteObjectARB(fp);
    #endif
  }
  LIBSL_GL_CHECK_ERROR;
  if (gs_code) {
    // default values for geometry shader inputs / outputs
    #ifdef EMSCRIPTEN
    glDeleteShader(gs);
    #else
    glDeleteObjectARB(gs);
    #endif
  }
  LIBSL_GL_CHECK_ERROR;
  glUseProgramObjectARB(0);
  LIBSL_GL_CHECK_ERROR;
}

// -----------------------------------------------------

void NAMESPACE::GLShader::init(GLhandleARB shader)
{
  m_Shader = shader;
  m_Name   = string("[runtime]");
}

// -----------------------------------------------------

void NAMESPACE::GLShader::terminate()
{
  if (m_Shader != 0) {
    glUseProgramObjectARB(0);
    #ifdef EMSCRIPTEN
    glDeleteProgram(m_Shader);
    #else
    glDeleteObjectARB(m_Shader);
    #endif
    LIBSL_GL_CHECK_ERROR;
    m_Shader = 0;
  }
}

// -----------------------------------------------------

void NAMESPACE::GLShader::authorize() const
{
  if (m_Shader == 0) {
#ifdef EMSCRIPTEN
    cout << "GLShader::authorize - shader used without having been initialized !" << endl;
#endif
    throw GLException("GLShader::authorize - shader used without having been initialized !");
  }
}

// -----------------------------------------------------

void NAMESPACE::GLShader::begin()
{
  authorize();
  glUseProgramObjectARB(m_Shader);
  m_Active = true;
}

// -----------------------------------------------------

void NAMESPACE::GLShader::end()
{
  glUseProgramObjectARB(0);
  m_Active = false;
}

// -----------------------------------------------------
// -----------------------------------------------------
// -----------------------------------------------------

void NAMESPACE::GLParameter::init(GLBaseShader& shader,const char *name)
{
  if (!shader.isReady()) {
    throw GLException("GLParameter::init - shader '%s' - Shader is not ready ! (call init before load parameters)",shader.name());
  }
  m_Shader = &shader;
  m_Handle = glGetUniformLocationARB(m_Shader->handle(),name);
  m_Strict = m_Shader->isStrict();
  if (m_Handle == -1) {
    if (m_Strict) {
      throw GLException("GLParameter::init - shader '%s' - parameter '%s' does not exist !",shader.name(),name);
    } else {
      // cerr << sprint("GLParameter - WARNING: shader '%s' - parameter '%s' was not found\n",shader.name(),name);
    }
  }
  m_Name = string(name);
}

// -----------------------------------------------------

void NAMESPACE::GLParameter::authorize() const
{
/*  GLenum err; err = glGetError(); if (err) {
    std::cerr << "GLParameter::authorize() GL error detected" << std::endl;
#ifdef EMSCRIPTEN
    cout << "GLParameter::authorize() GL error detected" << endl;
#endif
  }
*/
  if (m_Shader == NULL) {
    throw GLException("GLParameter::authorize - parameter used without having been initialized !");
  }
  if (m_Strict && m_Handle == -1) {
    throw GLException("GLParameter::authorize - parameter used without having been initialized !");
  }
  if (!m_Shader->isActive()) {
    throw GLException("GLParameter::authorize - parameter used while shader not active (parameters can only be set between begin/end) !");
  }
}


// -----------------------------------------------------

void NAMESPACE::GLParameter::set(float f)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform1fARB(m_Handle,f);
}
void NAMESPACE::GLParameter::set(float a,float b)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform2fARB(m_Handle,a,b);
}
void NAMESPACE::GLParameter::set(float a,float b,float c)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform3fARB(m_Handle,a,b,c);
}
void NAMESPACE::GLParameter::set(float a,float b,float c,float d)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform4fARB(m_Handle,a,b,c,d);
}
void NAMESPACE::GLParameter::set(const float *matrix)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniformMatrix4fvARB(m_Handle,1,GL_TRUE,matrix); // row major
}
void NAMESPACE::GLParameter::set(const m4x4f& m)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  #ifdef EMSCRIPTEN
  glUniformMatrix4fvARB(m_Handle,1,GL_FALSE,&m.transpose()[0]); // row major
  #else
  glUniformMatrix4fvARB(m_Handle, 1, GL_TRUE, &m[0]); // row major
  #endif
}
void NAMESPACE::GLParameter::set(const LibSL::Memory::Array::Array<m4x4f>& m)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
#ifdef EMSCRIPTEN
  sl_assert(false); // glUniformMatrix4fvARB cannot transpose on call with EMSCRIPTEN
#else
  glUniformMatrix4fvARB(m_Handle, m.size(), GL_TRUE, &(m[0][0])); // row major
#endif
}
#ifndef EMSCRIPTEN
void NAMESPACE::GLParameter::set(uint v)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform1uiEXT(m_Handle,v);
}
#endif
void NAMESPACE::GLParameter::set(int v)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform1iARB(m_Handle,v);
}
// SP 13-03-2017: Added support for setting booleans
void NAMESPACE::GLParameter::set(bool v)
{
	authorize();
	if (!m_Strict && m_Handle == -1) return;
	// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glUniform.xhtml
	// "..Either the i, ui or f variants may be used to provide values for uniform variables of type bool, bvec2, bvec3, bvec4, or arrays of these.."
	glUniform1iARB(m_Handle, (int)v);
}
void NAMESPACE::GLParameter::set(const v2f& v)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform2fARB(m_Handle,v[0],v[1]);
}
void NAMESPACE::GLParameter::set(const v3f& v)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform3fARB(m_Handle,v[0],v[1],v[2]);
}
void NAMESPACE::GLParameter::set(const v3i& v)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform3iARB(m_Handle,v[0],v[1],v[2]);
}
void NAMESPACE::GLParameter::set(const v4i& v)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform4iARB(m_Handle,v[0],v[1],v[2],v[3]);
}
void NAMESPACE::GLParameter::set(const v4f& v)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform4fARB(m_Handle,v[0],v[1],v[2],v[3]);
}
void NAMESPACE::GLParameter::setArray(const float *pv,int size)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform1fvARB(m_Handle,size,pv);
}
void NAMESPACE::GLParameter::setArray(const int *pv,int size)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform1ivARB(m_Handle,size,pv);
}
#ifndef EMSCRIPTEN
void NAMESPACE::GLParameter::setArray(const uint *pv,int size)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform1uivEXT(m_Handle,size,pv);
}
#endif
void NAMESPACE::GLParameter::setArray(const v2f *pv,int size)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform2fvARB(m_Handle,size,(float*)pv);
}
void NAMESPACE::GLParameter::setArray(const v2i *pv,int size)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform2ivARB(m_Handle,size,(int*)pv);
}
#ifndef EMSCRIPTEN
void NAMESPACE::GLParameter::setArray(const v2u *pv,int size)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform2uivEXT(m_Handle,size,(uint*)pv);
}
#endif
void NAMESPACE::GLParameter::setArray(const v3f *pv,int size)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform3fvARB(m_Handle,size,(float*)pv);
}
void NAMESPACE::GLParameter::setArray(const v3i *pv,int size)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform3ivARB(m_Handle,size,(int*)pv);
}
#ifndef EMSCRIPTEN
void NAMESPACE::GLParameter::setArray(const v3u *pv,int size)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform3uivEXT(m_Handle,size,(uint*)pv);
}
#endif
void NAMESPACE::GLParameter::setArray(const v4f *pv,int size)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform4fvARB(m_Handle,size,(float*)pv);
}
void NAMESPACE::GLParameter::setArray(const v4i *pv,int size)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform4ivARB(m_Handle,size,(int*)pv);
}
#ifndef EMSCRIPTEN
void NAMESPACE::GLParameter::setArray(const v4u *pv,int size)
{
  authorize();
  if (!m_Strict && m_Handle == -1) return;
  glUniform4uivEXT(m_Handle,size,(uint*)pv);
}
#endif

// -----------------------------------------------------

#ifdef OPENGL4

NAMESPACE::GLBuffer::GLBuffer()
{
  m_glId   = 0;
  m_Sz     = 0;
}

NAMESPACE::GLBuffer::GLBuffer(uint sz)
{
  m_glId   = 0;
  m_Sz     = 0;
  init( sz );
}

NAMESPACE::GLBuffer::GLBuffer(GLBuffer const& buffer)
{
	init(buffer.size());
	copy(buffer);
}

void NAMESPACE::GLBuffer::adopt(GLBuffer const& buffer)
{
  m_glId   = buffer.m_glId;
  m_Sz     = buffer.m_Sz;
}

void NAMESPACE::GLBuffer::forget()
{
  m_glId   = 0;
  m_Sz     = 0;
}

void NAMESPACE::GLBuffer::copy(GLBuffer const& buffer)
{
	if( buffer.size() != size() )
	{
		terminate();
		init(buffer.size());
	}
	if( size() != 0 )
	{
		glBindBufferARB(GL_COPY_READ_BUFFER,  buffer.m_glId);
		glBindBufferARB(GL_COPY_WRITE_BUFFER, m_glId);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, buffer.size());
		glBindBufferARB(GL_COPY_READ_BUFFER,  0);
		glBindBufferARB(GL_COPY_WRITE_BUFFER, 0);
	}
}

void NAMESPACE::GLBuffer::init(uint sz)
{
  LIBSL_GL_CHECK_ERROR;
  sl_assert(m_glId == 0);
  m_Sz     = sz;
	glGenBuffersARB(1, &m_glId);
  glBindBufferARB(c_buf_type, m_glId);
  glBufferDataARB(c_buf_type, sz, NULL, GL_DYNAMIC_DRAW);
  glBindBufferARB(c_buf_type, 0);
  LIBSL_GL_CHECK_ERROR;
}

void NAMESPACE::GLBuffer::resize(uint sz)
{
  LIBSL_GL_CHECK_ERROR;
  m_Sz     = sz;
  glBindBufferARB(c_buf_type, m_glId);
  glBufferDataARB(c_buf_type, sz, NULL, GL_DYNAMIC_DRAW);
  glBindBufferARB(c_buf_type, 0);
  LIBSL_GL_CHECK_ERROR;
}

void NAMESPACE::GLBuffer::terminate()
{
  if (m_glId != 0) {
    LIBSL_GL_CHECK_ERROR;
    glBindBufferARB(c_buf_type, 0);
    glDeleteBuffersARB(1,&m_glId);
    LIBSL_GL_CHECK_ERROR;
    m_glId   = 0;
    m_Sz     = 0;
  }
}

NAMESPACE::GLBuffer::~GLBuffer()
{
  // NOTE: do not call check error from here as GL may no longer be initialized
  terminate();
}

// -----------------------------------------------------
// -----------------------------------------------------
// -----------------------------------------------------

NAMESPACE::GLTexBuffer::GLTexBuffer() : GLBuffer()
{
  m_glTexId  = 0;
  m_glFormat = 0;
}

NAMESPACE::GLTexBuffer::GLTexBuffer(uint sz) : GLBuffer(sz)
{
  m_glTexId  = 0;
  m_glFormat = GL_R32UI;
  createTexture();
}

NAMESPACE::GLTexBuffer::GLTexBuffer(uint sz, GLuint format) : GLBuffer(sz)
{
  m_glTexId  = 0;
  m_glFormat = format;
  createTexture();
}

NAMESPACE::GLTexBuffer::GLTexBuffer(GLTexBuffer const& buffer) : GLBuffer(buffer)
{
	init(buffer.size());
	copy(buffer);
  createTexture();
}

void NAMESPACE::GLTexBuffer::adopt(GLTexBuffer const& buffer)
{
  GLBuffer::adopt(buffer);
  m_glTexId = buffer.m_glTexId;
}

void NAMESPACE::GLTexBuffer::forget()
{
  GLBuffer::forget();
  m_glTexId = 0;
}

void NAMESPACE::GLTexBuffer::init(uint sz)
{
  GLBuffer::init(sz);
  m_glFormat = GL_R32UI;
  createTexture();
}

void NAMESPACE::GLTexBuffer::init(uint sz, uint format)
{
  GLBuffer::init(sz);
  m_glFormat = format;
  createTexture();
}

void NAMESPACE::GLTexBuffer::resize(uint sz)
{
  GLBuffer::resize(sz);
  glBindTexture     (GL_TEXTURE_BUFFER, m_glTexId);
  glTexBufferARB    (GL_TEXTURE_BUFFER, m_glFormat, glId());
  glBindTexture     (GL_TEXTURE_BUFFER, 0);
}

void NAMESPACE::GLTexBuffer::terminate()
{
  deleteTexture();
  GLBuffer::terminate();
}

void NAMESPACE::GLTexBuffer::createTexture()
{
  glGenTextures     (1, &m_glTexId);
  glBindTexture     (GL_TEXTURE_BUFFER, m_glTexId);
  glTexBufferARB    (GL_TEXTURE_BUFFER, m_glFormat, glId());
  glBindTexture     (GL_TEXTURE_BUFFER, 0);
}

void NAMESPACE::GLTexBuffer::deleteTexture()
{
  if ( m_glTexId != 0 ) {
    LIBSL_GL_CHECK_ERROR;
    glDeleteTextures  (1, &m_glTexId);
    m_glTexId = 0;
    LIBSL_GL_CHECK_ERROR;
  }
}

// -----------------------------------------------------
// -----------------------------------------------------
// -----------------------------------------------------

void NAMESPACE::GLCompute::init(
	const char				  *cs_code,
	const char				  *name)
{
  m_Name   = string(name);
  m_Shader = glCreateProgramObjectARB();

  GLuint cs;

  cs = loadGLSLProgram(cs_code,GL_COMPUTE_SHADER);

  glAttachObjectARB(m_Shader,cs);

  glLinkProgramARB(m_Shader);

  GLint linked;
  glGetObjectParameterivARB(m_Shader,
			    GL_OBJECT_LINK_STATUS_ARB, &linked);
  if (!linked) {
    GLint maxLength;
    glGetObjectParameterivARB(m_Shader,
			     GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
    Array<GLcharARB> infoLog(maxLength);
    glGetInfoLogARB(m_Shader, maxLength, NULL, infoLog.raw());
    throw GLException("\n\n**** GLSL compute shader failed to link (%s) ****\n%s",m_Name.c_str(),infoLog.raw());
  }

  glDeleteObjectARB(cs);

  glUseProgramObjectARB(0);

  LIBSL_GL_CHECK_ERROR;
}

// -----------------------------------------------------

void NAMESPACE::GLCompute::init(GLuint shader)
{
  m_Shader = shader;
  m_Name   = string("[runtime]");
}

// -----------------------------------------------------

void NAMESPACE::GLCompute::terminate()
{
  if (m_Shader != 0) {
    glUseProgramObjectARB(0);
    glDeleteObjectARB(m_Shader);
    m_Shader = 0;
  }
  LIBSL_GL_CHECK_ERROR;
}

// -----------------------------------------------------

void NAMESPACE::GLCompute::authorize() const
{
  if (m_Shader==0)
    throw GLException("GLCompute::authorize - shader used without having been initialized !");
}

// -----------------------------------------------------

void NAMESPACE::GLCompute::begin()
{
  authorize();
  glUseProgramObjectARB(m_Shader);
  m_Active = true;
}

// -----------------------------------------------------

void NAMESPACE::GLCompute::end()
{
  glUseProgramObjectARB(0);
  m_Active = false;
}

// -----------------------------------------------------

void NAMESPACE::GLCompute::run(const v3i& numGroups)
{
    authorize();
	if (!m_Active)
		throw GLException("GLCompute::run - must be enclosed in-between begin/end calls");
	glDispatchCompute(numGroups[0],numGroups[1],numGroups[2]);
}

// -----------------------------------------------------

#endif

// -----------------------------------------------------

NAMESPACE::GLProtectMatrices::GLProtectMatrices()
{
#ifndef EMSCRIPTEN
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
#else
  m_Proj    = GLBasicPipeline::getUniqueInstance()->getProjection();
  m_MdlView = GLBasicPipeline::getUniqueInstance()->getModelview();
  m_Texture = GLBasicPipeline::getUniqueInstance()->getTextureMatrix();
#endif
}

NAMESPACE::GLProtectMatrices::~GLProtectMatrices()
{
#ifndef EMSCRIPTEN
  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
#else
  GLBasicPipeline::getUniqueInstance()->setProjection(m_Proj);
  GLBasicPipeline::getUniqueInstance()->setModelview (m_MdlView);
  GLBasicPipeline::getUniqueInstance()->setTextureMatrix(m_Texture);
#endif
}

// -----------------------------------------------------
