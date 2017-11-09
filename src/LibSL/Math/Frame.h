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
// LibSL::Math
// ------------------------------------------------------
//
// class Frame
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-06-17
// ------------------------------------------------------

#pragma once

#include <LibSL/Math/Vertex.h>
#include <LibSL/Math/Quaternion.h>
#include <LibSL/Math/Matrix4x4.h>

namespace LibSL {
  namespace Math {

    //! Frame class
    template <typename T_Type>
    class Frame
    {
    private:

      typedef Quaternion<T_Type> quat;
      typedef Tuple<T_Type,3>    vec;
    
      quat m_Rotation;
      vec  m_Translation;

    public:

        //! Default frame
        Frame()
        {
        }

        //! Frame from rotation and translation
        Frame(const quat& q,const vec& t)
        {
          m_Rotation=quat;
          m_Translation=t;
        }

        const quat& rotation()    const {return (m_Rotation);}
        const vec&  translation() const {return (m_Translation);}

        //! Transform a point
        vec transform(const vec& p)
        {
          return (q*m_Rotation + m_Translation);
        }

        //! Express as a 4x4 matrix
        Matrix4x4<T_Type> toMatrix()
        {
          Matrix4x4 m=m_Rotation.toMatrix();
          ForIndex(n,3) {
            m.at(n,3)=m_Translation[n];
          }
          return (m);
        }

        //! Change reference frame
        //  \input newReferenceframeA is the new frame of reference A
        //  \input oldReferenceInA is the old frame of reference expressed in frame A
        //  This frame will be expressed in A after calling changeReferenceFrame
        void changeReferenceFrame(
          const Frame& oldReferenceInA,
          const Frame& newReferenceframeA)
        {
          m_Rotation    = oldReferenceInA.rotation()*m_Rotation;
          m_Translation = oldReferenceInA.rotation()*m_Translation
                        + oldReferenceInA.translation();
        }
    };

  } //namespace LibSL::Math
} //namespace LibSL
