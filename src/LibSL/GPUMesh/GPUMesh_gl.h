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
// GPUmesh_gl.h
//
//   Header for using the GPUMesh class with OpenGL.
//
//                                                    (c) Sylvain Lefebvre 2003
// ----------------------------------------------------------------------------

#pragma once

// ----------------------------------------------

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#ifdef EMSCRIPTEN
#define GL_GLEXT_PROTOTYPES
#endif
#ifdef ANDROID
#include <GLES2/gl2.h>
#else
#include <GL/gl.h>
#endif
#endif

//#ifndef LIBSL_NO_GRAPHICS_API
//#define LIBSL_NO_GRAPHICS_API
//#include <LibSL/GPUMesh/GPUMesh.h>
//#undef LIBSL_NO_GRAPHICS_API
//#else
//#include <LibSL/GPUMesh/GPUMesh.h>
//#endif

#define GPUMESH_TRIANGLESTRIP GL_TRIANGLE_STRIP
#define GPUMESH_TRIANGLELIST  GL_TRIANGLES
#define GPUMESH_TRIANGLEFAN   GL_TRIANGLE_FAN
#ifndef EMSCRIPTEN
#define GPUMESH_QUADS         GL_QUADS
#endif
#define GPUMESH_LINELIST      GL_LINES
#define GPUMESH_LINESTRIP     GL_LINE_STRIP
#define GPUMESH_LINELOOP      GL_LINE_LOOP
#define GPUMESH_POINTLIST     GL_POINTS

#include <LibSL/GPUMesh/GPUMesh.h>

#if !defined(EMSCRIPTEN) && !defined(ANDROID)
#include <LibSL/GPUMesh/GPUMesh_CompilePolicy_GL_Calls.h>
#include <LibSL/GPUMesh/GPUMesh_CompilePolicy_GL_DisplayList.h>
#include <LibSL/GPUMesh/GPUMesh_CompilePolicy_GL_VBO.h>
#else
#include <LibSL/GPUMesh/GPUMesh_CompilePolicy_GLES_VBO.h>
#endif

namespace LibSL {
namespace GPUMesh {

  /// GL Mesh with vertex buffer
  template <class T_VertexFormat> 
    class GPUMesh_VertexBuffer
    : public GPUMesh_GL_VBO< T_VertexFormat > 
    {
    public:
      GPUMesh_VertexBuffer(int flags = 0) : GPUMesh_GL_VBO< T_VertexFormat >(flags) {}
    };
  
  /// GL Mesh with indexed vertex buffer
  template <class T_VertexFormat, typename T_IndexType> 
    class GPUMesh_Indexed_VertexBuffer
    : public GPUMesh_Indexed_GL_VBO< T_VertexFormat, T_IndexType > 
    {
    public:
      GPUMesh_Indexed_VertexBuffer(int flags = 0) : GPUMesh_Indexed_GL_VBO< T_VertexFormat, T_IndexType >(flags) {}
    };
  
} // namespace GPUMesh
} // namespace LibSL

// ----------------------------------------------
