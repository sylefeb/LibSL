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
// Classes to emulate GPU vector arithmetic
//
// - NOT efficient!! use for debug purposes
// - Does not support assignments such as v.yx = ...
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-09-19
// ------------------------------------------------------

#pragma once

#include <LibSL/Math/Tuple.h>
#include <vector>

namespace LibSL {
  namespace Math {

    class float3 : public Tuple<float,3>
    {
    public:

      float          x;
      float          y;
      float          z;

#include "floatN_swizzle3_declare.h"

      float3()
      {
        fill(0.0f);
      }
      float3(const Tuple<float,3>& t)
      {
        (*this)[0]=t[0]; (*this)[1]=t[1]; (*this)[2]=t[2];
        updateSwizzles();
      }
      float3(float x,float y,float z)
      {
        (*this)[0]=x; (*this)[1]=y; (*this)[2]=z;
        updateSwizzles();
      }
      float3(float2 xy,float z)
      {
        (*this)[0]=xy[0]; (*this)[1]=xy[1]; (*this)[2]=z;
        updateSwizzles();
      }
      float3(float x,float2 yz)
      {
        (*this)[0]=x; (*this)[1]=yz[0]; (*this)[2]=yz[1];
        updateSwizzles();
      }
      void updateSwizzles()
      {
        float _x=(*this)[0];
        float _y=(*this)[1];
        float _z=(*this)[2];
        x=_x;
        y=_y;
        z=_z;
#include "floatN_swizzle3_update.h"
      }

    };

    class float4 : public Tuple<float,4>
    {
    public:

      float          x;
      float          y;
      float          z;

#include "floatN_swizzle4_declare.h"

      float4()
      {
        fill(0.0f);
      }
      float4(const Tuple<float,4>& t)
      {
        (*this)[0]=t[0]; (*this)[1]=t[1]; (*this)[2]=t[2]; (*this)[3]=t[3];
        updateSwizzles();
      }
      float4(float x,float y,float z,float w)
      {
        (*this)[0]=x; (*this)[1]=y; (*this)[2]=z; (*this)[2]=w;
        updateSwizzles();
      }
      float4(float3 xyz,float w)
      {
        (*this)[0]=xyz[0]; (*this)[1]=xyz[1]; (*this)[2]=z;
        updateSwizzles();
      }
      float4(float x,float2 yz)
      {
        (*this)[0]=x; (*this)[1]=yz[0]; (*this)[2]=yz[1];
        updateSwizzles();
      }
      void updateSwizzles()
      {
        float _x=(*this)[0];
        float _y=(*this)[1];
        float _z=(*this)[2];
        x=_x;
        y=_y;
        z=_z;
#include "floatN_swizzle4_update.h"
      }

    };

  } // namespace LibSL::Math
} // namespace LibSL
