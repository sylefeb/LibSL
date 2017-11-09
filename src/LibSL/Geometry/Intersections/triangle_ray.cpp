/*************************************************************/
/* Ray-Triangle Intersection Test Routines                   */
/* Different optimizations of my and Ben Trumbore's          */
/* code from journals of graphics tools (JGT)                */
/* http://www.acm.org/jgt/                                   */
/* by Tomas Moller, May 2000                                 */
/*                                                           */
/* code rewritten to do tests on the sign of the determinant */
/* the division is before the test of the sign of the det    */
/* and one CROSS has been moved out from the if-else if-else */
/**************************************************************/
#include "LibSL.precompiled.h"
// ------------------------------------------------------

#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Vertex.h>
using namespace LibSL::Math;

int triangle_ray(const v3f& orig,  const v3f& dir, 
                 const v3f& vert0, const v3f& vert1, const v3f& vert2, 
                 float *t, float *u, float *v )
{
 const   float EPSILON = 1e-16f;
 v3f     edge1, edge2, tvec, pvec, qvec;
 float   det,inv_det;
 
 /* find vectors for two edges sharing vert0 */
 edge1   = vert1 - vert0;
 edge2   = vert2 - vert0;
 
 /* begin calculating determinant - also used to calculate U parameter */
 pvec    = cross(dir,edge2);
 
 /* if determinant is near zero, ray lies in plane of triangle */
 det     = dot(edge1,pvec);
 
 /* calculate distance from vert0 to ray origin */
 tvec    = orig - vert0;
 inv_det = 1.0f / det;
 
 qvec = cross(tvec,edge1);
    
 if ( det > EPSILON ) 
 {
  *u =  dot(tvec,pvec);
  if ( *u < 0.0 || *u > det )
    return 0;
        
  /* calculate V parameter and test bounds */
  *v = dot(dir,qvec);
  if ( *v < 0.0 || *u + *v > det )
    return 0;
 } 
 else if( det < -EPSILON )
 {
  /* calculate U parameter and test bounds */
  *u =  dot(tvec,pvec);
  if ( *u > 0.0 || *u < det )
    return 0;
  /* calculate V parameter and test bounds */
  *v =  dot(dir,qvec);
  if ( *v > 0.0 || *u + *v < det )
    return 0;
 } else {
   return 0;  /* ray is parallel to the plane of the triangle */
 }

 *t =  dot(edge2,qvec) * inv_det;
 (*u) *= inv_det;
 (*v) *= inv_det;
 
 return 1;
}
