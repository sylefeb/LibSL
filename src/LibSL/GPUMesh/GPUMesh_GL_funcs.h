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
// ----------------------------------------------------------------------------
// GPUMesh_GL_funcs.h
//
// The goal of this template class is to build a template mechanism
// above the OpenGL function pointers.
// The run time overhead is null as everything is resolved at compile time.
//
// Exemple: GLFuncs<float>::getFct(0)(4,ptr);
// => this calls glVertexAttrib1fvARB(4,ptr)
//
//                                                    (c) Sylvain Lefebvre 2003
// ----------------------------------------------------------------------------
#pragma once

#ifdef __APPLE__
#include <OpenGL/gl.h>
#define APIENTRY
#else
#ifndef EMSCRIPTEN
#include <GL/gl.h>
#endif
#endif

#ifdef USE_GLUX
#  include <glux.h>
#include "GL_ARB_multitexture.h"
#include "GL_ARB_vertex_program.h"
GLUX_LOAD(GL_ARB_multitexture)
GLUX_LOAD(GL_ARB_vertex_program)
#endif

// -----------------------------------------
namespace LibSL {
namespace GPUMesh {
// -----------------------------------------

template <class T> struct GLFuncs;

template <> struct GLFuncs<LibSL::Mesh::MVF_null>
{
  typedef void (APIENTRY *func)   (const LibSL::Mesh::MVF_null *);
  typedef void (APIENTRY *attfunc)(GLuint,const LibSL::Mesh::MVF_null *);
  typedef void (APIENTRY *texfunc)(GLenum,const LibSL::Mesh::MVF_null *);

  static GLFuncs<LibSL::Mesh::MVF_null>::func    getColor0Fct(int) {return (NULL);}
  static GLFuncs<LibSL::Mesh::MVF_null>::attfunc getAttributeFct(int) {return (NULL);}
  static GLFuncs<LibSL::Mesh::MVF_null>::func    getVertexFct(int) {return (NULL);}
  static GLFuncs<LibSL::Mesh::MVF_null>::func    getNormalFct(int) {return (NULL);}
  static GLFuncs<LibSL::Mesh::MVF_null>::texfunc getTexCoordnFct(int) {return (NULL);}
};

// -----------------------------------------

template <> struct GLFuncs<float>
{
public:

  typedef void (APIENTRY *func)(const GLfloat *);
  typedef void (APIENTRY *attfunc)(GLuint,const GLfloat *);
  typedef void (APIENTRY *texfunc)(GLenum,const GLfloat *);

  static func getColor0Fct(int i)
  {
    static const func funcs[4]={NULL, NULL, glColor3fv, glColor4fv}; 
    return (funcs[i]);
  }
  static attfunc getAttributeFct(int i)
  {
    static const attfunc funcs[4]={glVertexAttrib1fvARB, glVertexAttrib2fvARB, glVertexAttrib3fvARB, glVertexAttrib4fvARB}; 
    return (funcs[i]);
  }
  static func getVertexFct(int i)
  {
    static const func funcs[4]={NULL, glVertex2fv, glVertex3fv, glVertex4fv}; 
    return (funcs[i]);
  }
  static func getNormalFct(int i)
  {
    static const func funcs[4]={NULL, NULL, glNormal3fv, NULL}; 
    return (funcs[i]);
  }
  static texfunc getTexCoordnFct(int i)
  {
		static const texfunc funcs[4]={glMultiTexCoord1fvARB, glMultiTexCoord2fvARB, glMultiTexCoord3fvARB, glMultiTexCoord4fvARB}; 
    return (funcs[i]);
  }
  
};

// -----------------------------------------

template <> struct GLFuncs<double>
{
public:

  typedef void (APIENTRY *func)(const GLdouble *);
  typedef void (APIENTRY *attfunc)(GLuint,const GLdouble *);
  typedef void (APIENTRY *texfunc)(GLenum,const GLdouble *);

  static func getColor0Fct(int i)
  {
    static const func funcs[4]={NULL, NULL, glColor3dv, glColor4dv}; 
    return (funcs[i]);
  }
  static attfunc getAttributeFct(int i)
  {
    static const attfunc funcs[4]={glVertexAttrib1dvARB, glVertexAttrib2dvARB, glVertexAttrib3dvARB, glVertexAttrib4dvARB}; 
    return (funcs[i]);
  }
  static func getVertexFct(int i)
  {
    static const func funcs[4]={NULL, glVertex2dv, glVertex3dv, glVertex4dv}; 
    return (funcs[i]);
  }
  static func getNormalFct(int i)
  {
    static const func funcs[4]={NULL, NULL, glNormal3dv, NULL}; 
    return (funcs[i]);
  }
  static texfunc getTexCoordnFct(int i)
  {
    static const texfunc funcs[4]={glMultiTexCoord1dvARB, glMultiTexCoord2dvARB, glMultiTexCoord3dvARB, glMultiTexCoord4dvARB}; 
    return (funcs[i]);
  }
  
};

// -----------------------------------------

template <> struct GLFuncs<unsigned char>
{
public:

  typedef void (APIENTRY *func)(const GLubyte *);

  static func getColor0Fct(int i)
  {
    static const func funcs[4]={NULL, NULL, glColor3ubv, glColor4ubv}; 
    return (funcs[i]);
  }
  
};

// -----------------------------------------
} // namespace GPUMesh
} // namespace LibSL
// -----------------------------------------
