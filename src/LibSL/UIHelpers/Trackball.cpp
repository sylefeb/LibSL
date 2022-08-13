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
//---------------------------------------------------------------------------
#include "LibSL.precompiled.h"
//---------------------------------------------------------------------------

#include "Trackball.h"

#include <LibSL/Math/Quaternion.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Math/Math.h>

//---------------------------------------------------------------------------

#define NAMESPACE LibSL::UIHelpers

//---------------------------------------------------------------------------
// Trackball implementation
//---------------------------------------------------------------------------

#define TRANSLATING 1
#define ROTATING    2
#define ZOOMING     4

//---------------------------------------------------------------------------

static v3f dir2v3f(NAMESPACE::Trackball::e_Direction dir)
{
  switch (dir)
  {
  case NAMESPACE::Trackball::X_pos: return V3F(1, 0, 0); break;
  case NAMESPACE::Trackball::X_neg: return V3F(-1, 0, 0); break;
  case NAMESPACE::Trackball::Y_pos: return V3F(0, 1, 0); break;
  case NAMESPACE::Trackball::Y_neg: return V3F(0, -1, 0); break;
  case NAMESPACE::Trackball::Z_pos: return V3F(0, 0, 1); break;
  case NAMESPACE::Trackball::Z_neg: return V3F(0, 0, -1); break;
  };
  sl_assert(false);
  return v3f(0);
}

static float select(NAMESPACE::Trackball::e_Direction dir, v3f v)
{
  switch (dir)
  {
  case NAMESPACE::Trackball::X_pos: return  v[0]; break;
  case NAMESPACE::Trackball::X_neg: return  v[0]; break;
  case NAMESPACE::Trackball::Y_pos: return  v[1]; break;
  case NAMESPACE::Trackball::Y_neg: return  v[1]; break;
  case NAMESPACE::Trackball::Z_pos: return  v[2]; break;
  case NAMESPACE::Trackball::Z_neg: return  v[2]; break;
  };
  sl_assert(false);
  return 0;
}

//---------------------------------------------------------------------------

NAMESPACE::Trackball::Trackball()
{
  m_Rotation = quatf(LibSL::Math::V3F(0, 0, 1), 0);
  m_Translation = 0;
  m_Center      = 0;
  m_Width       = 0;
  m_Height      = 0;
  m_PrevX       = 0;
  m_PrevY       = 0;
  m_Elapsed     = 0;
  m_Status      = 0;
  m_Radius      = 1.0f;
  m_Walkthrough = false;
  m_AllowRoll   = true;
  m_WalkDir     = 0.0f;
  m_WalkSide    = 0.0f;
  m_Up          = Z_pos;
  m_WalkSpeed   = 1.0f;
  m_BallSpeed   = 1.0f;
  m_Locked      = false;
  m_ForceZoom   = false;
}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::set(v3f translation, v3f center, quatf rot)
{
  m_Translation = translation;
  m_Center = center;
  m_Rotation = rot;
}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::setRadius(float r)
{
  m_Radius = r;
}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::setCenter(const v3f& center)
{
  v3f pos = matrix().mulPoint(v3f(0));
  m_Center = center;
  if (!m_Walkthrough) {
    // update translation to make sure the viewpoint does not jump
    v3f newPos = matrix().mulPoint(v3f(0));
    m_Translation += pos - newPos;
  }
}

//---------------------------------------------------------------------------

const LibSL::Math::v3f& NAMESPACE::Trackball::getCenter() const
{
  return (m_Center);
}

//---------------------------------------------------------------------------

quatf& NAMESPACE::Trackball::rotation()
{
  return (m_Rotation);
}

const quatf& NAMESPACE::Trackball::rotation() const
{
  return (m_Rotation);
}

//---------------------------------------------------------------------------

v3f& NAMESPACE::Trackball::translation()
{
  return (m_Translation);
}

const v3f& NAMESPACE::Trackball::translation() const
{
  return (m_Translation);
}

//---------------------------------------------------------------------------

const m4x4f NAMESPACE::Trackball::matrix() const
{
  if (m_Walkthrough) {
    return (m_Rotation.toMatrix()*translationMatrix(-m_Translation));
  }
  else {
    return (
        translationMatrix(m_Translation)
      * translationMatrix(m_Center)
      * m_Rotation.toMatrix()
      * translationMatrix(-m_Center)
      );
  }
}

//---------------------------------------------------------------------------

v3f NAMESPACE::Trackball::forward() const
{
  v3f front = V3F(0, 0, -1);
  return matrix().inverse().mulVector(front);
}

//---------------------------------------------------------------------------

v3f NAMESPACE::Trackball::up() const
{
  switch (m_Up) {
  case X_pos: return v3f( 1, 0, 0);
  case X_neg: return v3f(-1, 0, 0);
  case Y_pos: return v3f( 0, 1, 0);
  case Y_neg: return v3f( 0,-1, 0);
  case Z_pos: return v3f( 0, 0, 1);
  case Z_neg: return v3f( 0, 0,-1);
  }
  return v3f(0, 0, 0);
}

//---------------------------------------------------------------------------

v3f NAMESPACE::Trackball::eye() const
{
  return matrix().inverse().mulPoint(v3f(0));
}

//---------------------------------------------------------------------------

void  NAMESPACE::Trackball::stopMotion()
{
  m_Status = 0;
}

//---------------------------------------------------------------------------

void  NAMESPACE::Trackball::animate(float elapsed)
{
  m_Elapsed = elapsed;
  if (m_Walkthrough) {
    float speed = m_WalkSpeed * m_Radius * elapsed / 1000.0f;
    // v3f   up      = dir2v3f(m_Up);
    // v3f   left    = normalize_safe(cross(up,forward));
    m_Translation = m_Translation + m_Rotation.inverse() * (speed * ((V3F(0, 0, -1)*m_WalkDir) + (V3F(1, 0, 0)*m_WalkSide)));
  }
}

//---------------------------------------------------------------------------

void  NAMESPACE::Trackball::goForward()
{
  m_WalkDir = 1.0f;
}

//---------------------------------------------------------------------------

void  NAMESPACE::Trackball::goBackward()
{
  m_WalkDir = -1.0f;
}

//---------------------------------------------------------------------------

void  NAMESPACE::Trackball::stopFB()
{
  m_WalkDir = 0.0f;
}

//---------------------------------------------------------------------------

void  NAMESPACE::Trackball::goLeft()
{
  m_WalkSide = -1.0f;
}

//---------------------------------------------------------------------------

void  NAMESPACE::Trackball::goRight()
{
  m_WalkSide = 1.0f;
}

//---------------------------------------------------------------------------

void  NAMESPACE::Trackball::stopLR()
{
  m_WalkSide = 0.0f;
}

//---------------------------------------------------------------------------

void  NAMESPACE::Trackball::setWalkSpeed(float s)
{
  m_WalkSpeed = s;
}

//---------------------------------------------------------------------------

void  NAMESPACE::Trackball::setBallSpeed(float s)
{
  m_BallSpeed = s;
}

//---------------------------------------------------------------------------

void  NAMESPACE::Trackball::setWalkthrough(bool b)
{
  if (m_Walkthrough && !b) {
    m_Translation = -(m_Rotation * m_Translation) - m_Center + (m_Rotation * m_Center);
  }
  else if (b && !m_Walkthrough) {
    m_Translation = matrix().inverse().mulPoint(V3F(0, 0, 0));
  }
  m_Walkthrough = b;
}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::setAllowRoll(bool b)
{
	m_AllowRoll = b;
}

//---------------------------------------------------------------------------

quatf NAMESPACE::Trackball::deltaRotation(float dx, float dy)
{
  //quatf qx=quatf(V3F(1,0,0),dy);
  //quatf qy=quatf(qx.inverse()*V3F(0,1,0),dx);

  if (dx*dx + dy*dy > 1e-10f) {
    quatf qd = quatf(V3F(dy, dx, 0), sqrt(dx*dx + dy*dy));
    return (qd);
  }
  else {
    return quatf();
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::initRotation(uint x, uint y)
{
  m_PrevX = x;
  m_PrevY = y;
}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::initTranslation(uint x, uint y)
{
  m_PrevX = x;
  m_PrevY = y;
}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::initZoom(uint x, uint y)
{
  m_PrevX = x;
  m_PrevY = y;
}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::updateRotation(uint x, uint y)
{
  float speed = 3.0f; // *m_Elapsed;
  float dx = (float((int) x) - float(m_PrevX)) * speed / float(m_Width);
  float dy = (float((int) y) - float(m_PrevY)) * speed / float(m_Height);
  m_PrevX = x;
  m_PrevY = y;
  quatf dr = deltaRotation(dx, dy);
  m_Rotation = dr * m_Rotation;

  if (!m_AllowRoll || m_Walkthrough) {
    // remove 'roll'
    int d = -1;
    if (m_Up == X_neg || m_Up == Y_neg || m_Up == Z_neg) {
      d = 1;
    }
    quatf rinv = m_Rotation.inverse();
    v3f up = dir2v3f(m_Up);
    v3f realleft = rinv * V3F(1, 0, 0);
    v3f flat = normalize_safe(realleft - dot(realleft, up)*up);
    float cs = dot(realleft, flat);
    if (cs > -1.0f && cs < 1.0f) {
      float val  = 1.0f - abs(dot(up, realleft));
      val = pow(val, 3.0f);
      float sign = dot(up, realleft) > 0 ? -1.0f : 1.0f;
      float target_agl = sign * val * acos(cs);
      float agl = target_agl;
      m_Rotation = quatf(V3F(0, 0, 1), agl) * m_Rotation;
    }
  }

}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::updateTranslation(uint x, uint y)
{
  if (!m_Walkthrough) {
    float speed = m_BallSpeed * m_Radius * (1.0f + length(m_Translation) / m_Radius);
    if (m_BallSpeed == 0.0f) speed = 1.0f * m_Radius;
    float dx =  (float((int) x) - float(m_PrevX)) * speed / float(m_Width);
    float dy = -(float((int) y) - float(m_PrevY)) * speed / float(m_Height);
    //std::cerr << abs(dx) << std::endl;
    //std::cerr << abs(dy) << std::endl;
    const float limit = 100.0f;
    dx = min(abs(dx), limit)*sign(dx);
    dy = min(abs(dy), limit)*sign(dy);
    m_PrevX = x;
    m_PrevY = y;
    v3f   tx = dx * V3F(1, 0, 0);
    v3f   ty = dy * V3F(0, 1, 0);
    m_Translation = m_Translation + tx + ty;
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::updateZoom(uint x, uint y)
{
  float speed = m_BallSpeed * m_Radius * (1.0f + length(m_Translation) / m_Radius);
  if (m_BallSpeed == 0.0f) speed = 1.0f * m_Radius;
  float d = -(float((int)y) - float((int)m_PrevY)) * speed / float(m_Width);
  const float limit = 100.0f;
  d = min(abs(d), limit)*sign(d);
  m_PrevX = x;
  m_PrevY = y;
  v3f tz;
  if (!m_Walkthrough) {
    tz = d * V3F(0, 0, 1);
  }
  else {
    tz = d * dir2v3f(m_Up);
  }
  m_Translation = m_Translation + tz;
}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::init(uint w, uint h)
{
  m_Width = w;
  m_Height = h;
  m_Translation = V3F(0, 0, -m_Radius);
  m_Rotation = quatf(V3F(0, 0, 1), 0.0f);
  m_Center = V3F(0, 0, 0);
}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::reset()
{
  m_Translation = V3F(0, 0, -m_Radius);
  m_Rotation = quatf(V3F(0, 0, 1), 0.0f);
}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::setWindowSize(uint w, uint h)
{
  m_Width = w;
  m_Height = h;
}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::setForceZoom(bool b)
{
  if (b) {
    m_ForceZoom = b;
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::buttonPressed(uint button, uint x, uint y)
{
  if (m_ForceZoom) {
    m_Status = m_Status | ZOOMING;
    initZoom(x, y);
  } else if (button == LIBSL_LEFT_BUTTON) {
    m_Status = m_Status | ROTATING;
    initRotation(x, y);
  } else if (button == LIBSL_MIDDLE_BUTTON) {
    m_Status = m_Status | ZOOMING;
    initZoom(x, y);
  } else if (button == LIBSL_RIGHT_BUTTON) {
    m_Status = m_Status | TRANSLATING;
    initTranslation(x, y);
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::buttonReleased(uint button)
{
  if (m_ForceZoom) {
    m_Status    = m_Status & (~uint(ZOOMING));
    m_ForceZoom = false;
  } else if (button == LIBSL_LEFT_BUTTON) {
    m_Status = m_Status & (~uint(ROTATING));
  } else if (button == LIBSL_MIDDLE_BUTTON) {
    m_Status = m_Status & (~uint(ZOOMING));
  } else if (button == LIBSL_RIGHT_BUTTON) {
    m_Status = m_Status & (~uint(TRANSLATING));
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::Trackball::update(uint x, uint y)
{
  if (m_Locked) return;
  if (m_Status & TRANSLATING) updateTranslation(x, y);
  if (m_Status & ROTATING)    updateRotation(x, y);
  if (m_Status & ZOOMING)     updateZoom(x, y);
}

//---------------------------------------------------------------------------
