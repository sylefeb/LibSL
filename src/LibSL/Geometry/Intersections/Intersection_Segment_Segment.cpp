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

#include <LibSL/Geometry/Intersections/Intersection_Segment_Segment.h>
using namespace LibSL::Geometry;
using namespace LibSL::Math;

// ------------------------------------------------------

#define NAMESPACE LibSL::Geometry::Intersections

// ------------------------------------------------------

// http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
bool NAMESPACE::Segment_Segment(
  const LibSL::Math::v2f& a0, const LibSL::Math::v2f& a1,
  const LibSL::Math::v2f& b0, const LibSL::Math::v2f& b1, 
  float &_at, float &_bt)
{
  v2f  ad = a1 - a0; // r
  v2f  bd = b1 - b0; // s
  float o = cross(v3f(ad), v3f(bd))[2];
  if (fabs(o) < 1e-6f) {
    return false;
  }
  float t = cross(v3f(b0 - a0), v3f(bd))[2] / o;
  float u = cross(v3f(b0 - a0), v3f(ad))[2] / o;
  _at = t;
  _bt = u;
  return (t >= 0 && t <= 1 && u >= 0 && u <= 1);
}

// ------------------------------------------------------
