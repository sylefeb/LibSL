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
// --------------------------------------------------------------
// Manipulator class
//
// Sylvain Lefebvre - 2008-02-03
// --------------------------------------------------------------

#pragma once

#include <LibSL/LibSL.common.h>
#include <LibSL/Memory/Pointer.h>

#include <LibSL/GPUTex/GPUTex.h>
#include <LibSL/GPUMesh/GPUMesh.h>
#include <LibSL/GPUHelpers/GPUHelpers.h>
#include <LibSL/GPUHelpers/Shapes.h>

// --------------------------------------------------------------

class Manipulator
{
protected:

  enum e_Mode { 
    Translate, Rotate };
  enum e_TranslateMode {
    Translate_X,Translate_Y,Translate_Z,
    Translate_XY,Translate_XZ,Translate_YZ};
  enum e_RotateMode {
    Rotate_X,Rotate_Y,Rotate_Z};

  typedef MVF1(LibSL::GPUMesh::mvf_position_3f)               mvf_vertices;
  typedef LibSL::GPUMesh::GPUMesh_VertexBuffer<mvf_vertices>  t_Handles;

  LibSL::Math::v3f              m_Translation;
  LibSL::Math::quatf            m_Rotation;

  LibSL::Math::m4x4f            m_Proj;
  LibSL::Math::m4x4f            m_View;
  uint                          m_ScrW;
  uint                          m_ScrH;
  float                         m_ZNear;
  float                         m_ZFar;
  float                         m_Size;

  bool                          m_Drag;

  e_Mode                        m_Mode;
  e_TranslateMode               m_TranslateMode;
  e_RotateMode                  m_RotateMode;

  static LibSL::GPUTex::RenderTarget2DRGBA_Ptr                            s_RTPick;
  static LibSL::Memory::Pointer::AutoPtr<LibSL::GPUHelpers::Shapes::Box>  s_Box;
  static LibSL::Memory::Pointer::AutoPtr<t_Handles>                       s_Triangles[3];
  static LibSL::Memory::Pointer::AutoPtr<t_Handles>                       s_Circles  [3];
  static LibSL::Memory::Pointer::AutoPtr<t_Handles>                       s_Lines    [3];
  static uint                                                             s_RefCnt;

  static void  createHandles();


  LibSL::Math::v4b pick(const LibSL::Math::Tuple<int,2>& pos);
  void             selectMode(const LibSL::Math::v4b& pix);
  void             drawTranslationHandles();
  void             drawRotationHandles();

  void             doTranslate(uint x,uint y);
  void             doRotate   (uint x,uint y);

public:

  Manipulator();
  ~Manipulator();

  void setTransform (const LibSL::Math::v3f& tr,const LibSL::Math::quatf& q);
  void setViewParams(uint w,uint h,float znear,float zfar,const LibSL::Math::m4x4f& proj,const LibSL::Math::m4x4f& view);
  
  void draw();

  bool mouseButton(uint x,uint y,uint button,uint flags);
  bool mouseMotion(uint x,uint y);
  
  LibSL::Math::m4x4f  toMatrix(const LibSL::Math::v3f& ctr=LibSL::Math::v3f(0)) const;
  LibSL::Math::v3f&   translation()  { return m_Translation; }
  LibSL::Math::quatf& rotation()     { return m_Rotation;    }
  const LibSL::Math::v3f&   translation() const { return m_Translation; }
  const LibSL::Math::quatf& rotation()    const { return m_Rotation;    }
  
  void  setSize(float s) { m_Size = s; }

  bool  selected()    const { return (m_Drag); }
};

// --------------------------------------------------------------
