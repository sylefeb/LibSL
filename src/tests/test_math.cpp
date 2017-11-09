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
#include "precompiled.h"

#include <LibSL/Math/Math.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Math/Quaternion.h>
#include <LibSL/Math/Matrix4x4.h>

#include <LibSL/CppHelpers/CppHelpers.h>
using namespace LibSL::CppHelpers;

// -----------

#include <iostream>
using namespace std;

// -----------

using namespace LibSL::Math;

typedef Tuple<float,1> tuple1;
typedef Tuple<float,2> tuple2;
typedef Tuple<float,3> tuple3;

void test_math()
{
  cerr << "---------------------------" << endl;
  cerr << " Tuple " << endl;
  cerr << "---------------------------" << endl;
 
  tuple1 a;
  a[0]=1;
  cerr << sprint("tuple1 a; a[0]=1;\n");
  tuple2 b(a,2.0f);
  cerr << sprint("tuple2 b(a,2.0f);\n");
  ForIndex(i,2) {
    cerr << sprint("b[i]=%f\n",b[i]);
  }
  tuple3 c(b,3.0f);
  cerr << sprint("tuple3 c(b,3.0f);\n");
  ForIndex(i,3) {
    cerr << sprint("c[i]=%f\n",c[i]);
  }
  tuple3 d(4.0f,b);
  cerr << sprint("tuple3 d(4.0f,b);\n");
  ForIndex(i,3) {
    cerr << sprint("d[i]=%f\n",d[i]);
  }

  c=(tuple3)a;
  cerr << sprint("c=a;\n");
  ForIndex(i,3) {
    cerr << sprint(" -> c[i]=%f\n",c[i]);
  }

  c=tuple3(b);
  cerr << sprint("c=b;\n");
  ForIndex(i,3) {
    cerr << sprint(" -> c[i]=%f\n",c[i]);
  }

{
  cerr << "ForTupleRange(2,i,Pair(3,5),Pair(6,7)):\n";
  v2i low  = V2I(3,5);
  v2i high = V2I(6,7);
  ForTupleRange(2,i,low,high) {
    cerr << i << endl;
  }
}

{
  cerr << "ForTupleRange(2,i,Pair(10,5),Pair(12,5)):\n";
  v2i low  = V2I(10,5);
  v2i high = V2I(12,5);
  ForTupleRange(2,i,low,high) {
    cerr << i << endl;
  }
}

  // vectors

  cerr << "---------------------------" << endl;
  cerr << " vectors " << endl;
  cerr << "---------------------------" << endl;

  cerr << "Test stream operator: v(0.1,0.23,0.45)=" << V3F(0.1f,0.23f,0.45f) << endl;

  {
    v3f v0=V3F(0.0,1.0,2.0);
    v3f v1=V3F(1.0,2.0,3.0);
    cerr << sprint("v0 = (%f,%f,%f)\n",v0[0],v0[1],v0[2]);
    cerr << sprint("v1 = (%f,%f,%f)\n",v1[0],v1[1],v1[2]);
    float d=dot(v0,v1);
    cerr << sprint("dot(v0,v1) = %f\n",d);
    v3f v2=v0+v1;
    cerr << sprint("v0+v1 = (%f,%f,%f)\n",v2[0],v2[1],v2[2]);
    v3f v3=add(v0,v1);
    cerr << sprint("add(v0,v1)   = (%f,%f,%f)\n",v3[0],v3[1],v3[2]);
    addEq(v0,v1);
    cerr << sprint("addEq(v0,v1) = (%f,%f,%f)\n",v0[0],v0[1],v0[2]);

    cerr << sprint("v0 = (%f,%f,%f)\n",v0[0],v0[1],v0[2]);
    cerr << sprint("v1 = (%f,%f,%f)\n",v1[0],v1[1],v1[2]);
    v3f v4=v0-v1;
    cerr << sprint("v0-v1 = (%f,%f,%f)\n",v4[0],v4[1],v4[2]);
    v3f v5=sub(v0,v1);
    cerr << sprint("sub(v0,v1)   = (%f,%f,%f)\n",v5[0],v5[1],v5[2]);
    subEq(v0,v1);
    cerr << sprint("subEq(v0,v1) = (%f,%f,%f)\n",v0[0],v0[1],v0[2]);
    cerr << sprint("v0 = (%f,%f,%f)\n",v0[0],v0[1],v0[2]);
    cerr << sprint("v1 = (%f,%f,%f)\n",v1[0],v1[1],v1[2]);
  }

  {
    v3f a=V3F(1.0,0.0,0.0);
    v3f b=V3F(0.0,1.0,0.0);
    cerr << sprint("a = (%f,%f,%f)\n",a[0],a[1],a[2]);
    cerr << sprint("b = (%f,%f,%f)\n",b[0],b[1],b[2]);
    v3f c=cross(a,b);
    cerr << sprint("c = (%f,%f,%f)\n",c[0],c[1],c[2]);
    
  }

  // quaternion
  {
    cerr << "---------------------------" << endl;
    cerr << " quaternion " << endl;
    cerr << "---------------------------" << endl;

    quatf qx(V3F(1.0,0.0,0.0),float(M_PI/2.0));
    quatf qy(V3F(0.0,1.0,0.0),float(M_PI/2.0));
    quatf qz(V3F(0.0,0.0,1.0),float(M_PI/2.0));

    quatf qg=qx*qy*qz;
    v3f   p;

    p=V3F(1.0f,0.5f,0.2f);
    cerr << "p at " << p << endl;

    cerr << "p rotation qz around z, 90 d" << endl;
    p=qz*p;
    cerr << "p at " << p << endl;

    cerr << "p rotation qy around y, 90 d" << endl;
    p=qy*p;
    cerr << "p at " << p << endl;

    cerr << "p rotation qx around x, 90 d" << endl;
    p=qx*p;
    cerr << "p at " << p << endl;

    cerr << "---------------------------" << endl;

    p=V3F(1.0f,0.5f,0.2f);
    cerr << "p at " << p << endl;

    cerr << "p rotation around qg = qx*qy*qz" << endl;
    p=qg*p;
    cerr << "p at " << p << endl;

    cerr << "p rotation by invert of qg" << endl;
    p=qg.inverse()*p;
    cerr << "p at " << p << endl;

    cerr << "p rotation by (qg*qg.invert()) " << endl;
    p=(qg*qg.inverse())*p;
    cerr << "p at " << p << endl;

    cerr << "---------------------------" << endl;

    p=V3F(1.0f,0.5f,0.2f);
    cerr << "p at " << p << endl;

    cerr << "qz*p             : " << (qz*p) << endl;
    //  cerr << "image  of p by qz: " << (qz.image(p)) << endl;
    //  cerr << "source of p by qz: " << (qz.source(p)) << endl;

    cerr << "---------------------------" << endl;

    quatf q;
    q=quatf(V3F(0.0,0.0,1.0),float(M_PI/4.0));
    p=V3F(1.0f,0.5f,0.2f);
    cerr << "p at " << p << endl;
    cerr << "rotation q around z, 45 degrees" << endl;
    cerr << "q*p = " << q*p << endl;
    cerr << "m=q.toMatrix()    : " << endl;
    m4x4f m=q.toMatrix();
    cerr << m << endl;
    cerr << "  m as Tuple :" << static_cast<Tuple<float,16> >(m) << endl;
    cerr << "m*p = " << m.mulPoint(p) << endl;
    
    cerr << "---------------------------" << endl;
  }

  // matrix 4x4
  {
    cerr << "---------------------------" << endl;
    cerr << " matrix 4x4 " << endl;
    cerr << "---------------------------" << endl;

    
    cerr << "a = (0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15)\n";
    m4x4f a(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
    cerr << "   as Tuple = " << static_cast<Tuple<float,16> >(a) << endl;
    cerr << "   as matrix= " << endl << a << endl;

    m4x4f m;
    cerr << "m = identity\n";
    m.eqIdentity();
    cerr << m << endl;

    cerr << "m = inverse(m)\n";
    m=m.inverse();
    cerr << m << endl;

    m=m4x4f(
      1,0,0,0,
      0,2,0,0,
      0,0,3,0,
      0,0,0,4);
    cerr << "m = " << endl << m << endl;
    cerr << "m = inverse(m)\n";
    m=m.inverse();
    cerr << m << endl;

    m=m4x4f(
      1,2,3,4,
      5,2,0,0,
      6,2,3,0,
      7,3,4,4);
    cerr << "m = " << endl << m << endl;

    cerr << "m = transpose(m)\n";
    m=m.transpose();
    cerr << m << endl;

    m4x4f m0(
      0,1,1,1,
      1,0,1,1,
      1,1,0,1,
      1,1,1,0);
    cerr << "m0 = \n";
    cerr << m0 << endl;

    cerr << "m1 = inverse(m0)\n";
    m4x4f m1=m0.inverse();
    cerr << m1 << endl;

    cerr << "m2 = m0 * m1\n";
    m4x4f m2=m0*m1;
    cerr << m2 << endl;
  }

  // Histogram
  {
    Histogram h;

    ForIndex(n,100) {
      h << (rand()&15);
    }

    h.print();
  }
}

// -----------
