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
#include "LibSL.precompiled.h"
// --------------------------------------------------------------
// Sylvain Lefebvre - 2008-02-03
// --------------------------------------------------------------

#include "Manipulator.h"

using namespace LibSL;
using namespace LibSL::GPUTex;
using namespace LibSL::GPUHelpers;

// --------------------------------------------------------------

RenderTarget2DRGBA_Ptr              Manipulator::s_RTPick;
AutoPtr<Shapes::Box>                Manipulator::s_Box;
AutoPtr<Manipulator::t_Handles>     Manipulator::s_Triangles[3];
AutoPtr<Manipulator::t_Handles>     Manipulator::s_Circles  [3];
AutoPtr<Manipulator::t_Handles>     Manipulator::s_Lines    [3];
uint                                Manipulator::s_RefCnt = 0;

// --------------------------------------------------------------

Manipulator::Manipulator()
{
  if (s_RefCnt == 0) {
    s_RTPick = RenderTarget2DRGBA_Ptr(new RenderTarget2DRGBA(1, 1));
    createHandles();
  }
  s_RefCnt ++;

  m_Size        = 1.0;
  m_Translation = 0;
  m_Rotation    = quatf(V3F(0,0,1),0);
  m_Drag  = false;
  m_ScrW  = 1;
  m_ScrH  = 1;
  m_ZNear = 0;
  m_ZFar  = 1;
  m_Proj  .eqIdentity();
  m_View  .eqIdentity();
  m_Mode          = Translate;
  m_TranslateMode = Translate_X;
  m_RotateMode    = Rotate_X;

}

// --------------------------------------------------------------

Manipulator::~Manipulator()
{
  s_RefCnt --;
  if (s_RefCnt == 0) {
    s_RTPick = RenderTarget2DRGBA_Ptr();
    s_Box    = AutoPtr<Shapes::Box>();
    ForIndex(i,3) { s_Triangles[i] = AutoPtr<t_Handles>(); }
  }
}

// --------------------------------------------------------------

void Manipulator::createHandles()
{
  // box
  s_Box = AutoPtr<Shapes::Box>(new Shapes::Box());

  // triangles
  s_Triangles[0] = AutoPtr<t_Handles>(new t_Handles());
  s_Triangles[0]->begin(GPUMESH_TRIANGLELIST);
  s_Triangles[0]->vertex_3(0,0,0);
  s_Triangles[0]->vertex_3(0,1,0);
  s_Triangles[0]->vertex_3(0,0,1);
  s_Triangles[0]->end();
  s_Triangles[1] = AutoPtr<t_Handles>(new t_Handles());
  s_Triangles[1]->begin(GPUMESH_TRIANGLELIST);
  s_Triangles[1]->vertex_3(0,0,0);
  s_Triangles[1]->vertex_3(0,0,1);
  s_Triangles[1]->vertex_3(1,0,0);
  s_Triangles[1]->end();
  s_Triangles[2] = AutoPtr<t_Handles>(new t_Handles());
  s_Triangles[2]->begin(GPUMESH_TRIANGLELIST);
  s_Triangles[2]->vertex_3(0,0,0);
  s_Triangles[2]->vertex_3(1,0,0);
  s_Triangles[2]->vertex_3(0,1,0);
  s_Triangles[2]->end();

  // circles
  ForIndex(i,3) {
    s_Circles[i] = AutoPtr<t_Handles>(new t_Handles());
    s_Circles[i]->begin(GPUMESH_LINESTRIP);
  }
  const int res = 32;
  ForIndex(c,res) {
    float a = (float)(c*M_PI*2.0f)/float(res-1);
    s_Circles[0]->vertex_3(0,cos(a),sin(a));
    s_Circles[1]->vertex_3(sin(a),0,cos(a));
    s_Circles[2]->vertex_3(cos(a),sin(a),0);
  }
  ForIndex(i,3) {
    s_Circles[i]->end();
  }

  // lines
  ForIndex(i,3) {
    s_Lines[i] = AutoPtr<t_Handles>(new t_Handles());
    s_Lines[i]->begin(GPUMESH_LINESTRIP);
  }
  s_Lines[0]->vertex_3(0,0,0);
  s_Lines[0]->vertex_3(1,0,0);
  s_Lines[1]->vertex_3(0,0,0);
  s_Lines[1]->vertex_3(0,1,0);
  s_Lines[2]->vertex_3(0,0,0);
  s_Lines[2]->vertex_3(0,0,1);
  ForIndex(i,3) {
    s_Lines[i]->end();
  }
}

// --------------------------------------------------------------

void Manipulator::setTransform (const v3f& tr,const quatf& q)
{
  m_Translation = tr;
  m_Rotation    = q;
}

// --------------------------------------------------------------

void Manipulator::setViewParams(uint w,uint h,float znear,float zfar,const m4x4f& proj,const m4x4f& view)
{
  m_ScrW  = w;
  m_ScrH  = h;
  m_ZNear = znear;
  m_ZFar  = zfar;
  m_Proj  = proj;
  m_View  = view;
}

// --------------------------------------------------------------

m4x4f Manipulator::toMatrix(const v3f& ctr) const
{
  return (translationMatrix(m_Translation) * translationMatrix(ctr) * m_Rotation.toMatrix() * translationMatrix(-ctr) );
}

// --------------------------------------------------------------

v4b Manipulator::pick(const Tuple<int,2>& pos)
{
  GPUHelpers::Renderer::ProtectViewport vp;

  // bind RT
  s_RTPick->bind();

  GPUHelpers::Renderer::setViewport(0,0,s_RTPick->w(),s_RTPick->h());
  GPUHelpers::clearScreen(LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER, 0,0,0,0);

  // setup pick matrix
  int x           = pos[0];
  int y           = (int(m_ScrH) - 1 - pos[1]);

  m4x4f mpick = translationMatrix(V3F(-float(x)+m_ScrW/2.0f,-float(y)+m_ScrH/2.0f,0)) * scaleMatrix(V3F(m_ScrW/2.0f,m_ScrH/2.0f,1.0f));
  Transform::set(LIBSL_PROJECTION_MATRIX,mpick * m_Proj);
  Transform::set(LIBSL_MODELVIEW_MATRIX ,m_View);
  // draw manipulator
  draw();
  // unbind RT
  s_RTPick->unbind();
  // restore proj. matrix
  Transform::set(LIBSL_PROJECTION_MATRIX,m_Proj);
  // readback
  Array2D<v4b> pixels;
  s_RTPick->readBack(pixels);

  //  std::cerr << v4i(pixels.at(0,0)) << std::endl;

  return (pixels.at(0,0));

}

// --------------------------------------------------------------

void Manipulator::selectMode(const v4b& pix)
{
  if (pix[0] == 0 && pix[1] == 0 && pix[2] == 0) {
    m_Drag = false;
    return;
  }

  if (pix[0] == 255 && pix[1] == 255 && pix[2] == 255) {
    if      (m_Mode == Rotate)    { m_Mode = Translate; }
    else if (m_Mode == Translate) { m_Mode = Rotate;    }
    m_Drag = false;
    return;
  }

  m_Drag = true;

  if (m_Mode == Translate) {
    if        (pix[0] == 255 && pix[1] == 0   && pix[2] == 0) {
      m_TranslateMode = Translate_X;
    } else if (pix[0] == 0   && pix[1] == 255 && pix[2] == 0) {
      m_TranslateMode = Translate_Y;
    } else if (pix[0] == 0   && pix[1] == 0   && pix[2] == 255) {
      m_TranslateMode = Translate_Z;
    } else if (pix[0] == 1   && pix[1] == 1 && pix[2] == 255) {
      m_TranslateMode = Translate_XY;
    } else if (pix[0] == 1   && pix[1] == 255 && pix[2] == 1) {
      m_TranslateMode = Translate_XZ;
    } else if (pix[0] == 255   && pix[1] == 1 && pix[2] == 1) {
      m_TranslateMode = Translate_YZ;
    }

  } else if (m_Mode == Rotate) {
    if        (pix[0] == 255 && pix[1] == 0   && pix[2] == 0) {
      m_RotateMode = Rotate_X;
    } else if (pix[0] == 0   && pix[1] == 255 && pix[2] == 0) {
      m_RotateMode = Rotate_Y;
    } else if (pix[0] == 0   && pix[1] == 0   && pix[2] == 255) {
      m_RotateMode = Rotate_Z;
    }
  }

}

// --------------------------------------------------------------

void Manipulator::draw()
{
  if (m_Mode == Rotate) {
    drawRotationHandles();
  } else {
    drawTranslationHandles();
  }
}

// --------------------------------------------------------------

void Manipulator::drawRotationHandles()
{

#ifdef DIRECT3D
  D3DMATERIAL9 mtrl;
  ZeroMemory( &mtrl, sizeof( mtrl ) );
  mtrl.Ambient.r = mtrl.Ambient.g = mtrl.Ambient.b = mtrl.Ambient.a = 1.0;
  mtrl.Diffuse.r = mtrl.Diffuse.g = mtrl.Diffuse.b = mtrl.Diffuse.a = 0.0;
  LIBSL_D3D_DEVICE->SetMaterial( &mtrl );
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_LIGHTING, TRUE  );
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_ZENABLE,  FALSE );
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_CULLMODE , D3DCULL_NONE );
#endif

#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glPushAttrib(GL_ENABLE_BIT);
  glDisable   (GL_TEXTURE_2D);
  glDisable   (GL_LIGHTING);
  glDisable   (GL_ALPHA_TEST);
  glDisable   (GL_BLEND);
  glDisable   (GL_DEPTH_TEST);
  glDisable   (GL_CULL_FACE);
#endif
#endif

  // rotation around x handle
#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_AMBIENT , 0xFF0000FF );
#endif
#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glColor3f(1,0,0);
#endif
#endif
  Transform::set(LIBSL_MODELVIEW_MATRIX  , m_View * toMatrix() * scaleMatrix(v3f(m_Size)) * translationMatrix(V3F(0,1,0)) * scaleMatrix(v3f(0.1f)) * translationMatrix(v3f(-0.5f)));
  s_Box->render();

  // rotation around y handle
#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_AMBIENT , 0xFF00FF00 );
#endif
#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glColor3f(0,1,0);
#endif
#endif
  Transform::set(LIBSL_MODELVIEW_MATRIX  , m_View * toMatrix() * scaleMatrix(v3f(m_Size)) * translationMatrix(V3F(0,0,1)) * scaleMatrix(v3f(0.1f)) * translationMatrix(v3f(-0.5f)));
  s_Box->render();

  // rotation around z handle
#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_AMBIENT , 0xFFFF0000 );
#endif
#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glColor3f(0,0,1);
#endif
#endif
  Transform::set(LIBSL_MODELVIEW_MATRIX  , m_View * toMatrix() * scaleMatrix(v3f(m_Size)) * translationMatrix(V3F(1,0,0)) * scaleMatrix(v3f(0.1f)) * translationMatrix(v3f(-0.5f)));
  s_Box->render();

  // yz handle
#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_AMBIENT , 0xFF0101FF );
#endif
#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  float _1_255 = 1.5f/255.0f;
  glColor3f(1,_1_255,_1_255);
#endif
#endif
  Transform::set(LIBSL_MODELVIEW_MATRIX  , m_View * toMatrix() * scaleMatrix(v3f(m_Size)));
  s_Circles[0]->render();

  // xz handle
#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_AMBIENT , 0xFF01FF01 );
#endif
#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glColor3f(_1_255,1,_1_255);
#endif
#endif
  Transform::set(LIBSL_MODELVIEW_MATRIX  , m_View * toMatrix() * scaleMatrix(v3f(m_Size)));
  s_Circles[1]->render();

  // xy handle
#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_AMBIENT , 0xFFFF0101 );
#endif
#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glColor3f(_1_255,_1_255,1);
#endif
#endif
  Transform::set(LIBSL_MODELVIEW_MATRIX  , m_View * toMatrix() * scaleMatrix(v3f(m_Size)));
  s_Circles[2]->render();

  // center handle
#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_AMBIENT , 0xFFFFFFFF );
#endif
#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glColor3f(1,1,1);
#endif
#endif
  Transform::set(LIBSL_MODELVIEW_MATRIX  , m_View * toMatrix() * scaleMatrix(v3f(m_Size)) * scaleMatrix(v3f(0.1f)) * translationMatrix(v3f(-0.5f)));
  s_Box->render();

  Transform::set(LIBSL_MODELVIEW_MATRIX  , m_View);
#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_LIGHTING, FALSE );
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_ZENABLE,  TRUE  );
#endif
#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glPopAttrib();
#endif
#endif
}

// --------------------------------------------------------------

void Manipulator::drawTranslationHandles()
{

#ifdef DIRECT3D
  D3DMATERIAL9 mtrl;
  ZeroMemory( &mtrl, sizeof( mtrl ) );
  mtrl.Ambient.r = mtrl.Ambient.g = mtrl.Ambient.b = mtrl.Ambient.a = 1.0;
  mtrl.Diffuse.r = mtrl.Diffuse.g = mtrl.Diffuse.b = mtrl.Diffuse.a = 0.0;
  LIBSL_D3D_DEVICE->SetMaterial( &mtrl );
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_LIGHTING, TRUE  );
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_ZENABLE,  FALSE );
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_CULLMODE , D3DCULL_NONE );
#endif

#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glPushAttrib(GL_ENABLE_BIT);
  glDisable   (GL_TEXTURE_2D);
  glDisable   (GL_LIGHTING);
  glDisable   (GL_ALPHA_TEST);
  glDisable   (GL_BLEND);
  glDisable   (GL_DEPTH_TEST);
  glDisable   (GL_CULL_FACE);
#endif
#endif

  // x handle
#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_AMBIENT , 0xFF0000FF );
#endif
#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glColor3f(1,0,0);
#endif
#endif
  Transform::set(LIBSL_MODELVIEW_MATRIX  , 
    m_View * translationMatrix(m_Translation) * scaleMatrix(v3f(m_Size)) * translationMatrix(V3F(1,0,0)) * scaleMatrix(v3f(0.1f)) * translationMatrix(v3f(-0.5f)));
  s_Box->render();

  Transform::set(LIBSL_MODELVIEW_MATRIX  , m_View * translationMatrix(m_Translation) * scaleMatrix(v3f(m_Size)) );
  s_Lines[0]->render();

  // y handle
#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_AMBIENT , 0xFF00FF00 );
#endif
#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glColor3f(0,1,0);
#endif
#endif
  Transform::set(LIBSL_MODELVIEW_MATRIX  , 
    m_View * translationMatrix(m_Translation) * scaleMatrix(v3f(m_Size)) * translationMatrix(V3F(0,1,0)) * scaleMatrix(v3f(0.1f)) * translationMatrix(v3f(-0.5f)));
  s_Box->render();

  Transform::set(LIBSL_MODELVIEW_MATRIX  , m_View * translationMatrix(m_Translation) * scaleMatrix(v3f(m_Size)) );
  s_Lines[1]->render();

  // z handle
#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_AMBIENT , 0xFFFF0000 );
#endif
#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glColor3f(0,0,1);
#endif
#endif
  Transform::set(LIBSL_MODELVIEW_MATRIX  , 
    m_View * translationMatrix(m_Translation) * scaleMatrix(v3f(m_Size)) * translationMatrix(V3F(0,0,1)) * scaleMatrix(v3f(0.1f)) * translationMatrix(v3f(-0.5f)));
  s_Box->render();

  Transform::set(LIBSL_MODELVIEW_MATRIX  , m_View * translationMatrix(m_Translation) * scaleMatrix(v3f(m_Size)) );
  s_Lines[2]->render();

  // yz handle
#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_AMBIENT , 0xFF0101FF );
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );
#endif
#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  float _1_255 = 1.5f/255.0f;
  glColor3f(1,_1_255,_1_255);
  glEnable   (GL_CULL_FACE);
  glCullFace (GL_BACK);
  glFrontFace(GL_CCW);
#endif
#endif
  Transform::set(LIBSL_MODELVIEW_MATRIX  , 
    m_View * translationMatrix(m_Translation) * scaleMatrix(v3f(m_Size)) * scaleMatrix(v3f(0.25f)));
  s_Triangles[0]->render();

  // xz handle
#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_AMBIENT , 0xFF01FF01 );
#endif
#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glColor3f(_1_255,1,_1_255);
#endif
#endif
  Transform::set(LIBSL_MODELVIEW_MATRIX  , 
    m_View * translationMatrix(m_Translation) * scaleMatrix(v3f(m_Size)) * scaleMatrix(v3f(0.25f)));
  s_Triangles[1]->render();

  // xy handle
#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_AMBIENT , 0xFFFF0101 );
#endif
#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glColor3f(_1_255,_1_255,1);
#endif
#endif

  Transform::set(LIBSL_MODELVIEW_MATRIX  , 
    m_View * translationMatrix(m_Translation) * scaleMatrix(v3f(m_Size)) * scaleMatrix(v3f(0.25f)));
  s_Triangles[2]->render();

  // center handle
#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_AMBIENT , 0xFFFFFFFF );
#endif
#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glColor3f(1,1,1);
#endif
#endif
  Transform::set(LIBSL_MODELVIEW_MATRIX  , 
    m_View * translationMatrix(m_Translation) * scaleMatrix(v3f(m_Size)) * scaleMatrix(v3f(0.1f)) * translationMatrix(v3f(-0.5f)));
  s_Box->render();

  Transform::set(LIBSL_MODELVIEW_MATRIX  , m_View );
#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_LIGHTING, FALSE );
  LIBSL_D3D_DEVICE->SetRenderState(D3DRS_ZENABLE,  TRUE  );
#endif
#ifdef OPENGL
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glPopAttrib();
#endif
#endif
}

// --------------------------------------------------------------

bool Manipulator::mouseButton(uint x,uint y,uint button,uint flags)
{
  if (button == LIBSL_LEFT_BUTTON) {
    if (flags & LIBSL_BUTTON_DOWN) {
      selectMode(pick(V2I(x,y)));
    } else {
      m_Drag = false;
    }
  }
  return m_Drag;
}

// --------------------------------------------------------------

void Manipulator::doTranslate(uint x,uint y)
{
  switch (m_TranslateMode)
  {

  case Translate_XY:
  case Translate_XZ:
  case Translate_YZ:
    {
      m4x4f mcam     = (m_Proj * m_View);
      m4x4f inv_mcam = mcam.inverse();
      float px       = -1.0f + 2.0f * (       x / float(m_ScrW));
      float py       = -1.0f + 2.0f * (1.0f - y / float(m_ScrH));
      v3f   o        = v3f( inv_mcam.mul(V4F(px*m_ZNear,py*m_ZNear,-m_ZNear, m_ZNear)) );
      v3f   f        = v3f( inv_mcam.mul(V4F(px*m_ZFar ,py*m_ZFar , m_ZFar, m_ZFar )) );
      v3f   d        = normalize( f - o );
      v3f   n;
      if      (m_TranslateMode == Translate_XY) { n = V3F(0,0,1); }
      else if (m_TranslateMode == Translate_XZ) { n = V3F(0,1,0); }
      else                                      { n = V3F(1,0,0); }
      v3f   ctr      = toMatrix().mulPoint(v3f(0));
      Plane<3> pl(ctr,n);
      v3f   i        = o + d * pl.intersect(o,d);
      m_Translation  = i;
    }
    break;

  case Translate_X:
  case Translate_Y:
  case Translate_Z:
    {
      m4x4f mcam     = (m_Proj * m_View);
      m4x4f inv_mcam = mcam.inverse();
      v3f   v;
      if      (m_TranslateMode == Translate_X) { v = V3F(1,0,0); }
      else if (m_TranslateMode == Translate_Y) { v = V3F(0,1,0); }
      else                                     { v = V3F(0,0,1); }
      v3f   ctr      = toMatrix().mulPoint( v3f(0) );
      v4f   p_ctr    = mcam  .mul(v4f(ctr  ,1));
      v4f   p_vec    = mcam  .mul(v4f(ctr+v,1));
      // go into screen space
      v2f   s_ctr    = V2F(p_ctr[0] / p_ctr[3],p_ctr[1] / p_ctr[3]);
      v2f   s_vec    = V2F(p_vec[0] / p_vec[3],p_vec[1] / p_vec[3]);
      v2f   s_dir    = normalize( s_vec - s_ctr );
      float px       = -1.0f + 2.0f * (       x / float(m_ScrW));
      float py       = -1.0f + 2.0f * (1.0f - y / float(m_ScrH));
      float t        = dot( (V2F(px,py) - s_ctr) , s_dir );
      // find closest point along line
      v2f   s_pt     = s_ctr + t * s_dir;
      // project onto near/far plane
      v3f   o        = v3f( inv_mcam.mul(V4F(s_pt[0]*m_ZNear,s_pt[1]*m_ZNear,-m_ZNear, m_ZNear)) );
      v3f   f        = v3f( inv_mcam.mul(V4F(s_pt[0]*m_ZFar ,s_pt[1]*m_ZFar ,  m_ZFar, m_ZFar )) );
      v3f   d        = normalize( f - o );
      v3f   n;
      if      (m_TranslateMode == Translate_X) { n = V3F(0,0,1); }
      else if (m_TranslateMode == Translate_Y) { n = V3F(0,0,1); }
      else                                     { n = V3F(0,1,0); }
      Plane<3> pl(ctr,n);
      v3f   i        = o + d * pl.intersect(o,d);
      m_Translation  = i - v*m_Size;
    } 
    break;
  }
}

// --------------------------------------------------------------

void Manipulator::doRotate(uint x,uint y)
{
  m4x4f mcam     = (m_Proj * m_View);
  m4x4f inv_mcam = mcam.inverse();
  float px       = -1.0f + 2.0f * (       x / float(m_ScrW));
  float py       = -1.0f + 2.0f * (1.0f - y / float(m_ScrH));
  v3f   o        = v3f( inv_mcam.mul(V4F(px*m_ZNear,py*m_ZNear,-m_ZNear, m_ZNear)) );
  v3f   f        = v3f( inv_mcam.mul(V4F(px*m_ZFar ,py*m_ZFar , m_ZFar, m_ZFar )) );
  v3f   d        = normalize( f - o );
  v3f   n,h;
  if      (m_RotateMode == Rotate_X) { n = V3F(1,0,0); h = V3F(0,1,0); }
  else if (m_RotateMode == Rotate_Y) { n = V3F(0,1,0); h = V3F(0,0,1); }
  else                               { n = V3F(0,0,1); h = V3F(1,0,0); }
  n              = m_Rotation.transform(n);
  v3f   ctr      = toMatrix().mulPoint(v3f(0));
  Plane<3> pl(ctr,n);
  v3f   i        = o + d * pl.intersect(o,d);
  v3f   prev_d   = m_Rotation.transform(h);
  v3f   new_d    = normalize(i - ctr);
  m_Rotation     = quatf(n,asin(dot(cross(prev_d,new_d),n))) * m_Rotation;
}

// --------------------------------------------------------------

bool Manipulator::mouseMotion(uint x,uint y)
{
  if (m_Drag) {
    if (m_Mode == Translate) {
      doTranslate(x,y);
    } else if (m_Mode == Rotate) {
      doRotate(x,y);
    }
  }
  return m_Drag;
}

// --------------------------------------------------------------
