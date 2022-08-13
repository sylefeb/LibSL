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
// LibSL::Trackball
// ------------------------------------------------------
//
// Simple trackball
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2007-05-07
// ------------------------------------------------------

#pragma once

#include <LibSL/Math/Quaternion.h>
#include <LibSL/Math/Vertex.h>

namespace LibSL {
  namespace UIHelpers {

    class Trackball
    {
    public:

      enum e_Direction {X_pos, X_neg, Y_pos, Y_neg, Z_pos, Z_neg};
      
    protected:

      typedef LibSL::Math::quatf quatf;
      typedef LibSL::Math::v3f   v3f;

      quatf m_Rotation;
      v3f   m_Translation;
      v3f   m_Center;
      uint  m_Width;
      uint  m_Height;
      int   m_PrevX;
      int   m_PrevY;
      float m_Elapsed;
      uint  m_Status;
      float m_Radius;
      bool  m_Walkthrough;
			bool  m_AllowRoll;
      float m_WalkDir;
      float m_WalkSide;
      float m_WalkSpeed;
      float m_BallSpeed;
      bool  m_Locked;
      bool  m_ForceZoom;
      e_Direction m_Up;      

      void  initRotation(uint x,uint y);
      void  initTranslation(uint x,uint y);
      void  initZoom(uint x,uint y);
      void  updateRotation(uint x,uint y);
      void  updateTranslation(uint x,uint y);
      void  updateZoom(uint x,uint y);
      quatf deltaRotation(float dx,float dy);

    public:

      Trackball();

      void  buttonPressed (uint button,uint x,uint y);
      void  buttonReleased(uint button);
      void  update        (uint x,uint y);
      void  init          (uint x,uint y);
      void  animate       (float elapsed);
      void  reset         ();
      void  lock          ()  {m_Locked = true;}
      void  unlock        ()  {m_Locked = false;}

      void  setRadius(float r);
      void  stopMotion();
      void  setWindowSize(uint w,uint h);
      void  setWalkthrough(bool b);
			void  setAllowRoll(bool b);
      void  setForceZoom(bool b);

      // Set the up direction for walktrough. 
      // Possible values are X_pos, X_neg, Y_pos, Y_neg, Z_pos, Z_neg.
      void  setUp(e_Direction dir)  { m_Up = dir; }

      LibSL::Math::quatf&       rotation();
      const LibSL::Math::quatf& rotation() const;
      LibSL::Math::v3f&         translation();
      const LibSL::Math::v3f&   translation() const;

      const LibSL::Math::v3f&   getCenter() const;
      void                      setCenter(const LibSL::Math::v3f& center);

      void                      set(v3f translation, v3f center, quatf rot);

      const LibSL::Math::m4x4f  matrix()  const;

      v3f                       up()      const;
      v3f                       forward() const;
      v3f                       eye()     const;

      void  goForward();
      void  goBackward();
      void  goLeft();
      void  goRight();
      void  stopFB();
      void  stopLR();

      /// set walk speed (radius per second)
      void  setWalkSpeed(float s);
      /// set ball speed (radius per second)
      void  setBallSpeed(float s);
    };

  } //namespace LibSL::UIHelpers
} //namespace LibSL

// ------------------------------------------------------
