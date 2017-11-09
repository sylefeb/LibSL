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
// OpenGL tranform class - Sylvain Lefebvre - 21/08/2003

// TODO: template this - vc7 & gcc only ...

#ifndef __GLTRANSFORM__
#define __GLTRANSFORM__

#define GLTRANSFORM_MATRIX    0
#define GLTRANSFORM_TRANSLATE 1
#define GLTRANSFORM_SCALE     2
#define GLTRANSFORM_ROTATE    3

#include <list>

class GLTransform
{
private:
  float m_fError;
  int   m_iType;
  union
  {
    float m_Mtx[16];
    float m_Trl[3];
    float m_Scl[3];
    float m_Rot[4];
  };
public:
  GLTransform() : m_iType(-1) {memset(m_Mtx,0,sizeof(float)*16);m_fError=0.0;}

  GLTransform(int t) : m_iType(t) {memset(m_Mtx,0,sizeof(float)*16);m_fError=0.0;}

  GLTransform toMatrix() const
  {
    GLTransform t;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    gl();
    t.m_iType=GLTRANSFORM_MATRIX;
    glGetFloatv(GL_MODELVIEW_MATRIX,t.m_Mtx);
    glPopMatrix();
    return (t);
  }

  GLTransform(const std::list<GLTransform>& trs)
  {
    m_iType=GLTRANSFORM_MATRIX;
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    for (std::list<GLTransform>::const_iterator T=trs.begin();T!=trs.end();T++)
      (*T).gl();
    glGetFloatv(GL_MODELVIEW_MATRIX,m_Mtx);
    glPopMatrix();
  }

  float& operator [](int i)
  {
    switch (m_iType)
    {
    case GLTRANSFORM_ROTATE:
      return (m_Rot[i]);
      break;
    case GLTRANSFORM_TRANSLATE:
      return (m_Trl[i]);
      break;
    case GLTRANSFORM_SCALE:
      return (m_Scl[i]);
      break;
    case GLTRANSFORM_MATRIX:
      return (m_Mtx[i]);
      break;
    }
    return (m_fError);
  }

  const float& operator [](int i) const
  {
    switch (m_iType)
    {
    case GLTRANSFORM_ROTATE:
      return (m_Rot[i]);
      break;
    case GLTRANSFORM_TRANSLATE:
      return (m_Trl[i]);
      break;
    case GLTRANSFORM_SCALE:
      return (m_Scl[i]);
      break;
    case GLTRANSFORM_MATRIX:
      return (m_Mtx[i]);
      break;
    }
    return (m_fError);
  }

  void gl() const
  {
    switch (m_iType)
    {
    case GLTRANSFORM_ROTATE:
      glRotatef(m_Rot[0],m_Rot[1],m_Rot[2],m_Rot[3]);
      break;
    case GLTRANSFORM_TRANSLATE:
      glTranslatef(m_Trl[0],m_Trl[1],m_Trl[2]);
      break;
    case GLTRANSFORM_SCALE:
      glScalef(m_Scl[0],m_Scl[1],m_Scl[2]);
      break;
    case GLTRANSFORM_MATRIX:
      glMultMatrixf(m_Mtx);
      break;
    }    
  }

  void apply(float x,float y,float z,float w,
             float& _x,float& _y,float& _z,float& _w) const
  {
    switch (m_iType)
    {
    case GLTRANSFORM_MATRIX:
     /*
     _x=x*m_Mtx[0+0*4]+y*m_Mtx[1+0*4]+z*m_Mtx[2+0*4]+w*m_Mtx[3+0*4];
     _y=x*m_Mtx[0+1*4]+y*m_Mtx[1+1*4]+z*m_Mtx[2+1*4]+w*m_Mtx[3+1*4];
     _z=x*m_Mtx[0+2*4]+y*m_Mtx[1+2*4]+z*m_Mtx[2+2*4]+w*m_Mtx[3+2*4];
     _w=x*m_Mtx[0+3*4]+y*m_Mtx[1+3*4]+z*m_Mtx[2+3*4]+w*m_Mtx[3+3*4];
     */   
     _x=x*m_Mtx[0+0*4]+y*m_Mtx[0+1*4]+z*m_Mtx[0+2*4]+w*m_Mtx[0+3*4];
     _y=x*m_Mtx[1+0*4]+y*m_Mtx[1+1*4]+z*m_Mtx[1+2*4]+w*m_Mtx[1+3*4];
     _z=x*m_Mtx[2+0*4]+y*m_Mtx[2+1*4]+z*m_Mtx[2+2*4]+w*m_Mtx[2+3*4];
     _w=x*m_Mtx[3+0*4]+y*m_Mtx[3+1*4]+z*m_Mtx[3+2*4]+w*m_Mtx[3+3*4];
     
     break;
    default:
      GLTransform tr=(*this).toMatrix();
      tr.apply(x,y,z,w,_x,_y,_z,_w);
      break;
    }
  }

};

#endif
