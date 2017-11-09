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

#include <LibSL/Geometry/Intersections/Intersection_Plane_AABox.h>
using namespace LibSL::Geometry;
using namespace LibSL::Math;

// ------------------------------------------------------

#define NAMESPACE LibSL::Geometry::Intersections

// ------------------------------------------------------

/*
 * Ray-box intersection using IEEE numerical properties to ensure that the
 * test is both robust and efficient, as described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */
static bool ray_bbox(const v3f * const bbox,
                     const v3f& org,const v3f& dir,const v3f& inv_dir,
                     const int * const sign,
                     float t0, float t1)
{

  float tmin = (bbox[sign[0]][0] - org[0]) * inv_dir[0];
  float tmax = (bbox[1-sign[0]][0] - org[0]) * inv_dir[0];

  float tymin = (bbox[sign[1]][1] - org[1]) * inv_dir[1];
  float tymax = (bbox[1-sign[1]][1] - org[1]) * inv_dir[1];

  if ( (tmin > tymax) || (tymin > tmax) )
    return false;
  if (tymin > tmin)
    tmin = tymin;
  if (tymax < tmax)
    tmax = tymax;

  float tzmin = (bbox[sign[2]][2] - org[2]) * inv_dir[2];
  float tzmax = (bbox[1-sign[2]][2] - org[2]) * inv_dir[2];

  if ( (tmin > tzmax) || (tzmin > tmax) )
    return false;
  if (tzmin > tmin)
    tmin = tzmin;
  if (tzmax < tmax)
    tmax = tzmax;
  return ( (tmin < t1) && (tmax > t0) );
  // NOTE: tmin provides intersection location
}

// ------------------------------------------------------
      
bool NAMESPACE::Ray_AABox(const LibSL::Math::v3f& o,const LibSL::Math::v3f& d,const LibSL::Geometry::AABox& b,float t0,float t1)
{
  v3f origin    = o;
  v3f direction = d;
  v3f inv_dir   = V3F(1/d[0], 1/d[1], 1/d[2]);
  int sign[3];
  sign[0] = (inv_dir[0] < 0) ? 1 : 0;
  sign[1] = (inv_dir[1] < 0) ? 1 : 0;
  sign[2] = (inv_dir[2] < 0) ? 1 : 0;
  v3f bbox[2];
  bbox[0] = b.minCorner();
  bbox[1] = b.maxCorner();
  return (ray_bbox(bbox,origin,direction,inv_dir,sign, t0,t1));
}

// ------------------------------------------------------

bool NAMESPACE::AABox_Ray(const LibSL::Geometry::AABox& b,const LibSL::Math::v3f& o,const LibSL::Math::v3f& d,float t0,float t1)
{
  return Ray_AABox(o,d, b, t0,t1);
}

// ------------------------------------------------------
