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
//------------------------------------------------------------------------
#include "LibSL.precompiled.h"
// -----------------------------------------------------------------------
#include "GPUHelpers.h"
// -----------------------------------------------------------------------

#include <iostream>
#include <sstream>

#include <LibSL/CppHelpers/CppHelpers.h>

using namespace std;
using namespace LibSL::CppHelpers;
using namespace LibSL::Image;
using namespace LibSL::Math;

// -----------------------------------------------------------------------

#define NAMESPACE LibSL::GPUHelpers

// -----------------------------------------------------------------------

#ifdef DIRECT3D

// -----------------------------------------------------------------------

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include <LibSL/D3DHelpers/D3DHelpers.h>

// -----------------------------------------------------------------------

void NAMESPACE::clearScreen(
                            uint flags,
                            float r,float g,float b,float a,
                            float depth,
                            int   stencil)
{
  DWORD clr = D3DCOLOR_ARGB(int(a*255.0f),int(r*255.0f),int(g*255.0f),int(b*255.0f));
  DWORD d3dflags=0;
  if (flags & LIBSL_COLOR_BUFFER) {
    d3dflags |= D3DCLEAR_TARGET;
  }
  if (flags & LIBSL_DEPTH_BUFFER) {
    d3dflags |= D3DCLEAR_ZBUFFER;
  }
  if (flags & LIBSL_STENCIL_BUFFER) {
    d3dflags |= D3DCLEAR_STENCIL;
  }
  LIBSL_DIRECT3D_DEVICE->Clear(0L, NULL, d3dflags, clr, depth, stencil);
}

void NAMESPACE::Transform::ortho2D(uint matrixid,float l,float r,float t,float b)
{
  D3DXMATRIX m;
  D3DXMatrixOrthoOffCenterRH(&m,l,r,t,b,-1,1);
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    LIBSL_DIRECT3D_DEVICE->SetTransform(D3DTS_WORLD,&m);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    LIBSL_DIRECT3D_DEVICE->SetTransform(D3DTS_PROJECTION,&m);
  } else sl_assert(false);
}

void NAMESPACE::Transform::ortho2D(uint matrixid,float l,float r,float t,float b,float znear,float zfar)
{
  D3DXMATRIX m;
  D3DXMatrixOrthoOffCenterRH(&m,l,r,t,b,znear,zfar);
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    LIBSL_DIRECT3D_DEVICE->SetTransform(D3DTS_WORLD,&m);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    LIBSL_DIRECT3D_DEVICE->SetTransform(D3DTS_PROJECTION,&m);
  } else sl_assert(false);
}

void NAMESPACE::Transform::identity(uint matrixid)
{
  D3DXMATRIX m;
  D3DXMatrixIdentity(&m);
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    LIBSL_DIRECT3D_DEVICE->SetTransform(D3DTS_WORLD,&m);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    LIBSL_DIRECT3D_DEVICE->SetTransform(D3DTS_PROJECTION,&m);
  } else sl_assert(false);
}

void NAMESPACE::Transform::perspective(uint matrixid,float fov_rad,float aspect,float znear,float zfar)
{
  D3DXMATRIX m;
  D3DXMatrixPerspectiveFovRH(&m,fov_rad,aspect,znear,zfar);
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    LIBSL_DIRECT3D_DEVICE->SetTransform(D3DTS_WORLD,&m);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    LIBSL_DIRECT3D_DEVICE->SetTransform(D3DTS_PROJECTION,&m);
  } else sl_assert(false);
}

void NAMESPACE::Transform::lookat(uint matrixid,
                                  const LibSL::Math::v3f& eye,
                                  const LibSL::Math::v3f& at,
                                  const LibSL::Math::v3f& up)
{
  D3DXMATRIX  m;
  D3DXVECTOR3 veye(eye[0],eye[1],eye[2]);
  D3DXVECTOR3 vat(at[0],at[1],at[2]);
  D3DXVECTOR3 vup(up[0],up[1],up[2]);
  D3DXMatrixLookAtRH(&m,&veye,&vat,&vup);
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    LIBSL_DIRECT3D_DEVICE->SetTransform(D3DTS_WORLD,&m);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    LIBSL_DIRECT3D_DEVICE->SetTransform(D3DTS_PROJECTION,&m);
  } else sl_assert(false);
}

void NAMESPACE::Transform::set(uint matrixid,const m4x4f& m)
{
  m4x4f mt=m.transpose();
  D3DXMATRIX md3d(&(mt[0]));
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    LIBSL_DIRECT3D_DEVICE->SetTransform(D3DTS_WORLD,&md3d);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    LIBSL_DIRECT3D_DEVICE->SetTransform(D3DTS_PROJECTION,&md3d);
  } else if (matrixid == LIBSL_TEXTURE_MATRIX) {
    LIBSL_DIRECT3D_DEVICE->SetTransform(D3DTS_TEXTURE0,&md3d);
  } else sl_assert(false);
}

// -----------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------

#ifdef OPENGL

// -----------------------------------------------------------------------

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#ifdef USE_GLUX
#include <glux.h>
#endif

#ifdef __APPLE__
#include "apple_gl.h"
#include <OpenGL/glu.h>
#else
#ifdef ANDROID
#include <GLES2/gl2.h>
#else
#include <GL/gl.h>
#ifndef EMSCRIPTEN
#include <GL/glu.h>
#endif
#endif
#endif

#include <LibSL/GLHelpers/GLHelpers.h>

// -----------------------------------------------------------------------

void NAMESPACE::clearScreen(
                            uint flags,
                            float r,float g,float b,float a,
                            float depth,
                            int   stencil)
{
  glClearColor(r,g,b,a);
#ifdef ANDROID
  glClearDepthf(depth);
#else
  glClearDepth(depth);
#endif
  glClearStencil(stencil);

  GLbitfield glflags=0;
  if (flags & LIBSL_COLOR_BUFFER) {
    glflags |= GL_COLOR_BUFFER_BIT;
  }
  if (flags & LIBSL_DEPTH_BUFFER) {
    glflags |= GL_DEPTH_BUFFER_BIT;
  }
  if (flags & LIBSL_STENCIL_BUFFER) {
    glflags |= GL_STENCIL_BUFFER_BIT;
  }

  glClear(glflags);
}


ImageRGBA_Ptr NAMESPACE::captureScreen()
{
  GLenum format     = GL_RGBA;
  GLint  v[4];
  glGetIntegerv(GL_VIEWPORT,v);
  ImageRGBA_Ptr img = ImageRGBA_Ptr(new ImageRGBA(v[2], v[3]));
  sl_assert(sizeof(ImageRGBA::t_Pixel) == sizeof(ImageRGBA::t_Pixel::t_Element)*ImageRGBA::t_Pixel::e_Size);
  glReadPixels(v[0],v[1],v[2],v[3],format,GL_UNSIGNED_BYTE,img->pixels().raw());
  img->flipH();
  return (img);
}


void NAMESPACE::Transform::ortho2D(uint matrixid,float l,float r,float t,float b)
{
#if defined(EMSCRIPTEN) | defined(ANDROID)
  m4x4f m = LibSL::Math::orthoMatrixGL<float>(l,r,t,b,-1,1);
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->setModelview(m);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->setProjection(m);
  } else sl_assert(false);
#else
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    glMatrixMode(GL_MODELVIEW);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    glMatrixMode(GL_PROJECTION);
  } else sl_assert(false);
  glLoadIdentity();
  gluOrtho2D(l,r,t,b);
#endif
}

void NAMESPACE::Transform::ortho2D(uint matrixid,float l,float r,float t,float b,float znear,float zfar)
{
#if defined(EMSCRIPTEN) | defined(ANDROID)
  m4x4f m = LibSL::Math::orthoMatrixGL<float>(l, r, t, b, znear, zfar);
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->setModelview(m);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->setProjection(m);
  } else sl_assert(false);
#else
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    glMatrixMode(GL_MODELVIEW);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    glMatrixMode(GL_PROJECTION);
  } else sl_assert(false);
  glLoadIdentity();
  glOrtho(l,r,t,b,znear,zfar);
#endif
}

void NAMESPACE::Transform::identity(uint matrixid)
{
#if defined(EMSCRIPTEN) | defined(ANDROID)
  m4x4f m = m4x4f::identity();
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->setModelview(m);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->setProjection(m);
  } else sl_assert(false);
#else
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    glMatrixMode(GL_MODELVIEW);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    glMatrixMode(GL_PROJECTION);
  } else sl_assert(false);
  glLoadIdentity();
#endif
}

void NAMESPACE::Transform::perspective(uint matrixid,float fov_rad,float aspect,float znear,float zfar)
{
#if defined(EMSCRIPTEN) | defined(ANDROID)
  m4x4f m = LibSL::Math::perspectiveMatrixGL<float>(fov_rad,aspect,znear,zfar);
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->setModelview(m);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->setProjection(m);
  } else sl_assert(false);
#else
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    glMatrixMode(GL_MODELVIEW);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    glMatrixMode(GL_PROJECTION);
  } else sl_assert(false);
  glLoadIdentity();
  gluPerspective(fov_rad*180.0/M_PI,aspect,znear,zfar);
#endif
}

void NAMESPACE::Transform::lookat(uint matrixid,
                                  const LibSL::Math::v3f& eye,
                                  const LibSL::Math::v3f& at,
                                  const LibSL::Math::v3f& up)
{
#if defined(EMSCRIPTEN) | defined(ANDROID)
  m4x4f m = LibSL::Math::lookatMatrix(eye,at,up);
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->setModelview(m);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->setProjection(m);
  } else sl_assert(false);
#else
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    glMatrixMode(GL_MODELVIEW);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    glMatrixMode(GL_PROJECTION);
  } else sl_assert(false);
  glLoadIdentity();
  gluLookAt(
    eye[0],eye[1],eye[2],
    at[0],at[1],at[2],
    up[0],up[1],up[2]
  );
#endif
}

void NAMESPACE::Transform::set(uint matrixid,const m4x4f& m)
{
#if defined(EMSCRIPTEN) | defined(ANDROID)
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->setModelview(m);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->setProjection(m);
  } else if (matrixid == LIBSL_TEXTURE_MATRIX) {
    LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->setTextureMatrix(m);
} else sl_assert(false);
#else
  if (matrixid == LIBSL_MODELVIEW_MATRIX) {
    glMatrixMode(GL_MODELVIEW);
  } else if (matrixid == LIBSL_PROJECTION_MATRIX) {
    glMatrixMode(GL_PROJECTION);
  } else if (matrixid == LIBSL_TEXTURE_MATRIX) {
    glMatrixMode(GL_TEXTURE);
  } else sl_assert(false);
  glLoadIdentity();
  m4x4f mt = m.transpose();
  glMultMatrixf(&(mt[0]));
  glMatrixMode(GL_MODELVIEW);
#endif
}

// -----------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------

#ifdef DIRECTX10

// -----------------------------------------------------------------------

#include <windows.h>
#include <d3d10_1.h>
#include <d3dx10.h>

#include <LibSL/D3DHelpers/D3DHelpers.h>

// -----------------------------------------------------------------------

void NAMESPACE::clearScreen(
                            uint flags,
                            float r,float g,float b,float a,
                            float depth,
                            int   stencil)
{
  ID3D10RenderTargetView *rts[D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT];
  ID3D10DepthStencilView *rtdepth;
  LIBSL_D3D_DEVICE->OMGetRenderTargets(D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT,rts,&rtdepth);

  if (flags & LIBSL_COLOR_BUFFER) {
    float clr[4] = {r,g,b,a};
    ForIndex(n,D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT) {
      if (rts[n]) LIBSL_D3D_DEVICE->ClearRenderTargetView( rts[n], clr );
      LIBSL_D3D_SAFE_RELEASE(rts[n]);
    }
  }

  D3D10_CLEAR_FLAG dxflags = (D3D10_CLEAR_FLAG)0;
  if (flags & LIBSL_DEPTH_BUFFER) {
    dxflags = (D3D10_CLEAR_FLAG)(int(dxflags) | D3D10_CLEAR_DEPTH);
  }
  if (flags & LIBSL_STENCIL_BUFFER) {
    dxflags = (D3D10_CLEAR_FLAG)(int(dxflags) | D3D10_CLEAR_STENCIL);
  }
  if (dxflags) {
    if (rtdepth) LIBSL_D3D_DEVICE->ClearDepthStencilView( rtdepth, dxflags, depth, stencil );
    LIBSL_D3D_SAFE_RELEASE( rtdepth );
  }
}

void NAMESPACE::Transform::ortho2D(uint matrixid,float l,float r,float t,float b)
{
  sl_assert(false);
}

void NAMESPACE::Transform::identity(uint matrixid)
{
  sl_assert(false);
}

void NAMESPACE::Transform::perspective(uint matrixid,float fov_rad,float aspect,float znear,float zfar)
{
  sl_assert(false);
}

void NAMESPACE::Transform::lookat(uint matrixid,
                                  const LibSL::Math::v3f& eye,
                                  const LibSL::Math::v3f& at,
                                  const LibSL::Math::v3f& up)
{
  sl_assert(false);
}

void NAMESPACE::Transform::set(uint matrixid,const m4x4f& m)
{
  sl_assert(false);
}

#endif

// -----------------------------------------------------------------------

void NAMESPACE::Renderer::setViewport(uint x,uint y,uint w,uint h)
{
#ifdef DIRECT3D
  D3DVIEWPORT9 vp;
  vp.X      = x;
  vp.Y      = y;
  vp.Width  = w;
  vp.Height = h;
  vp.MinZ   = 0;
  vp.MaxZ   = 1;
  LIBSL_D3D_DEVICE->SetViewport(&vp);
#endif

#ifdef DIRECTX10
  D3D10_VIEWPORT vp;
  vp.Width    = w;
  vp.Height   = h;
  vp.MinDepth = 0;
  vp.MaxDepth = 1;
  vp.TopLeftX = x;
  vp.TopLeftY = y;
  LIBSL_D3D_DEVICE->RSSetViewports( 1, &vp );
#endif

#ifdef OPENGL
  glViewport(x,y,w,h);
#endif
}

// -----------------------------------------------------------------------

void NAMESPACE::Renderer::getViewport(uint& _x,uint& _y,uint& _w,uint& _h)
{
#ifdef DIRECT3D
  D3DVIEWPORT9 vp;
  LIBSL_D3D_DEVICE->GetViewport(&vp);
  _x = vp.X;
  _y = vp.Y;
  _w = vp.Width;
  _h = vp.Height;
#endif

#ifdef DIRECTX10
  D3D10_VIEWPORT vp;
  UINT n = 1;
  LIBSL_D3D_DEVICE->RSGetViewports( &n, &vp );
  _x = vp.TopLeftX;
  _y = vp.TopLeftY;
  _w = vp.Width;
  _h = vp.Height;
#endif

#ifdef OPENGL
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT,vp);
  _x = vp[0];
  _y = vp[1];
  _w = vp[2];
  _h = vp[3];
#endif
}

// -----------------------------------------------------------------------
