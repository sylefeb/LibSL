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
//----------------------------------------------------------
//
// GPUMesh - CGenSpiral.h - 2004-05-04
//
// Generates an helicoidal object. Illustrates how GPUMesh
// can be used to create object generators common to OpenGL
// and Direct3D. Vertex format compatibility is also ensured.
//
//
// (c) Sylvain Lefebvre 2004 
//
//     Sylvain.Lefebvre@laposte.net
//----------------------------------------------------------
#ifndef __CGenSpiral__
#define __CGenSpiral__
//----------------------------------------------------------
#include "GPUMesh.h"
//----------------------------------------------------------
#include <list>
#include <loki/static_check.h>
//----------------------------------------------------------
#ifndef M_PI
#  define M_PI	       3.14159265359
#endif
//----------------------------------------------------------
typedef GPUMESH_MVF4(mvf_vertex_3f,mvf_normal_3f,mvf_color0_rgba,mvf_texcoord0_2f) GenSpiral_mvf;
//----------------------------------------------------------
using namespace GPUMesh;
//----------------------------------------------------------
template <class T_Mesh> class GenSpiral
{
 public:


 private:

  struct v3d
  {
    float x,y,z;

    v3d() : x(0), y(0),z(0) {}
    v3d(float px,float py,float pz) : x(px), y(py), z(pz) {}
    float dot(const v3d& v)      const {return (v.x*x+v.y*y+v.z*z);}
    v3d cross(const v3d& p)      const {return (v3d(y * p.z - p.y * z, z * p.x - p.z * x, x * p.y - p.x * y)); }
    v3d operator -(const v3d& p) const {return (v3d(x-p.x,y-p.y,z-p.z));}
    v3d operator +(const v3d& p) const {return (v3d(x+p.x,y+p.y,z+p.z));}
    float length()               const {return ((float)sqrt(x*x+y*y+z*z));}
    v3d operator *(float l)      const {return (v3d(x*l,y*l,z*l));}
    v3d   unit()                 const {return (v3d(x,y,z)*(1.0f/length()));}
  };

public:

  static void genSpiral(T_Mesh& mesh,int lod)
  {
    CHECK_COMPATIBLE_FORMAT(typename T_Mesh::vertex_format,GenSpiral_mvf);

    mesh.begin(GPUMESH_TRIANGLELIST);

    int nbTris=lod*2;

    v3d u0,u1;

    u0=v3d(-1.0,0.0,0.0);
    for (int l=1;l<lod*4;l++)
    {
      v3d p0(
        (float)cos(M_PI*2.0f*l/(float)(lod)),
        (float)sin(M_PI*2.0f*l/(float)(lod)),
        l/(float)(lod) - 2.0f);
      v3d p1(
        (float)cos(M_PI*2.0*(l+1)/(float)(lod)),
        (float)sin(M_PI*2.0*(l+1)/(float)(lod)),
        (l+1)/(float)(lod) - 2.0f);
      v3d p2(
        (float)cos(M_PI*2.0*(l+2)/(float)(lod)),
        (float)sin(M_PI*2.0*(l+2)/(float)(lod)),
        (l+2)/(float)(lod) - 2.0f);
      v3d n0(
       -(float)sin(M_PI/2.0f+M_PI*2.0f*l/(float)(lod)),
        (float)cos(M_PI/2.0f+M_PI*2.0f*l/(float)(lod)),
        0.0);
      v3d n1(
       -(float)sin(M_PI/2.0f+M_PI*2.0f*(l+1)/(float)(lod)),
        (float)cos(M_PI/2.0f+M_PI*2.0f*(l+1)/(float)(lod)),
        0.0);

      u1=(p2-p0).unit();

      n0=n0.unit();
      v3d t0=n0.cross(u0);
      t0=t0.unit();
      u1=u1.unit();
      v3d t1=n1.cross(u1);
      t1=t1.unit();

      for (int i=0;i<nbTris;i++)
      {
        v3d nl0,nl1,n00,n01,n11,n10;
        v3d pl0,pl1,p00,p01,p11,p10;
        int r=i/2;

        nl0=v3d(
          (float)-sin(M_PI/2.0+M_PI*2.0*r/(float)(lod)),
          (float)cos(M_PI/2.0+M_PI*2.0*r/(float)(lod)),
          0.0);
        pl0=v3d(
          (float)cos(M_PI*2.0*r/(float)(lod)),
          (float)sin(M_PI*2.0*r/(float)(lod)),
          0.0)
          *0.4f;

        nl1=v3d(
          (float)-sin(M_PI/2.0+M_PI*2.0*(r+1)/(float)(lod)),
          (float)cos(M_PI/2.0+M_PI*2.0*(r+1)/(float)(lod)),
          0.0);
        pl1=v3d(
          (float)cos(M_PI*2.0*(r+1)/(float)(lod)),
          (float)sin(M_PI*2.0*(r+1)/(float)(lod)),
          0.0)
          *0.4f;

        n00=   n0*nl0.x+t0*nl0.y+u0*nl0.z;
        p00=p0+n0*pl0.x+t0*pl0.y+u0*pl0.z;

        n01=   n0*nl1.x+t0*nl1.y+u0*nl1.z;
        p01=p0+n0*pl1.x+t0*pl1.y+u0*pl1.z;

        n11=   n1*nl1.x+t1*nl1.y+u1*nl1.z;
        p11=p1+n1  *pl1.x+t1*pl1.y+u1*pl1.z;

        n10=   n1*nl0.x+t1*nl0.y+u1*nl0.z;
        p10=p1+n1*pl0.x+t1*pl0.y+u1*pl0.z;

        if (i & 1)
        {
          mesh.color0_4((unsigned char)(255.0f*l/(float)(lod*4)),
			(unsigned char)(255.0f*r/(float)(lod)),
			255,
			0);
          mesh.normal(-n00.x,-n00.y,-n00.z);
	  mesh.texcoord0_2(l/float(lod),r/float(lod));
          mesh.vertex_3(p00.x,p00.y,p00.z);

          mesh.color0_4((unsigned char)(255.0f*l/(float)(lod*4)),
			(unsigned char)(255.0f*(r+1)/(float)(lod)),
			255,
			0);
          mesh.normal(-n01.x,-n01.y,-n01.z);
	  mesh.texcoord0_2(l/float(lod),(r+1)/float(lod));
          mesh.vertex_3(p01.x,p01.y,p01.z);

          mesh.color0_4((unsigned char)(255.0f*(l+1)/(float)(lod*4)),
			(unsigned char)(255.0f*(r+1)/(float)(lod)),
			255,
			0);
          mesh.normal(-n11.x,-n11.y,-n11.z);
	  mesh.texcoord0_2((l+1)/float(lod),(r+1)/float(lod));
          mesh.vertex_3(p11.x,p11.y,p11.z);
        }
        else
        {
          mesh.color0_4((unsigned char)(255.0f*l/(float)(lod*4)),
			(unsigned char)(255.0f*r/(float)(lod)),
			255,
			0);
          mesh.normal(-n00.x,-n00.y,-n00.z);
	  mesh.texcoord0_2(l/float(lod),r/float(lod));
          mesh.vertex_3(p00.x,p00.y,p00.z);

          mesh.color0_4((unsigned char)(255.0f*(l+1)/(float)(lod*4)),
			(unsigned char)(255.0f*r/(float)(lod)),
			255,
			0);
          mesh.normal(-n10.x,-n10.y,-n10.z);
	  mesh.texcoord0_2((l+1)/float(lod),r/float(lod));
          mesh.vertex_3(p10.x,p10.y,p10.z);

          mesh.color0_4((unsigned char)(255.0f*(l+1)/(float)(lod*4)),
			(unsigned char)(255.0f*(r+1)/(float)(lod)),
			255,
			0);
          mesh.normal(-n11.x,-n11.y,-n11.z);
	  mesh.texcoord0_2((l+1)/float(lod),(r+1)/float(lod));
          mesh.vertex_3(p11.x,p11.y,p11.z);
        }
      }
      u0=u1;
    }

    mesh.end();
  }

};
//----------------------------------------------------------
#endif
//----------------------------------------------------------
