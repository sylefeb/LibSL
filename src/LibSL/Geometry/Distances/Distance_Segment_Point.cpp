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
#include "LibSL.precompiled.h"
// ------------------------------------------------------

#include <LibSL/Geometry/Distances/Distance_Segment_Point.h>
#include <limits>
using namespace LibSL::Geometry;
using namespace LibSL::Math;

// ------------------------------------------------------

#define NAMESPACE LibSL::Geometry::Distances

// ------------------------------------------------------

// distance point / segment
float NAMESPACE::Point_Segment(const LibSL::Math::v3f& p,const LibSL::Math::v3f& s0,const LibSL::Math::v3f& s1)
{
  // TODO optimize 
  // compute line vector
  float l2 = dot( s1 - s0 , s1 - s0 );
  if (l2 < std::numeric_limits<float>::epsilon()) return length(p - s0);
  float t  = dot(  p - s0 , s1 - s0 );
  if (t < 0.0) {
    return length(p - s0);
  } else if (t > l2) {
    return length(p - s1);   
  } else {
    return length(p - (s0 + t * (s1-s0) / l2));
  }
} 

// ------------------------------------------------------

float NAMESPACE::Segment_Point(const LibSL::Math::v3f& s0,const LibSL::Math::v3f& s1,const LibSL::Math::v3f& p)
{
  return Point_Segment(p, s0,s1);
}

// ------------------------------------------------------
