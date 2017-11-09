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

// -----------

#include <iostream>
using namespace std;

// -----------

using namespace LibSL::Memory::Array;

void test_array()
{
  Array<int,InitZero,CheckAll> array0;

  cerr << "=== Array<> ===" << endl;

  // test zeroed
  array0.allocate(16);
  ForArray(array0,i) {
    cerr << i << '=' << array0[i] << ' ';
  }
  cerr << endl;

  try {
    // test out of bound
    array0[-1]=0;
  } catch (LibSL::Errors::Fatal& err) {
    cerr << err.message() << endl;
  }

  try {
    // test out of bound
    array0[345345]=0;
  } catch (LibSL::Errors::Fatal& err) {
    cerr << err.message() << endl;
  }

  // create an unsafe array
  Array<int,InitNop,LibSL::Memory::Array::CheckNop> unsafe0;
  unsafe0.allocate(16);
  ForArray(unsafe0,i) {
    cerr << i << '=' << unsafe0[i] << ' ';
  }
  cerr << endl;

  // copy
  ForArray(array0,i) {
    array0[i]=i;
  }

  Array<int,InitZero,CheckAll> array1=array0;
  Array<int,InitZero,CheckAll> array2;
  array2.allocate(16);
  array2=array0;
  ForArray(array0,i) {
    sl_assert(array0[i]==i);
    sl_assert(array0[i]==array1[i]);
    sl_assert(array0[i]==array2[i]);
  }

  // 2D array

  cerr << "=== Array2D<> ===" << endl;

  Array2D<int,InitZero,CheckAll> a2d(4,6);
  ForArray2D(a2d,i,j) {
    a2d.set(i,j) = i+j*a2d.xsize();
  }
  for (uint j=0;j<a2d.ysize();j++) {
    for (uint i=0;i<a2d.xsize();i++) {
      cerr << a2d.get(i,j) << ' ';
    }
    cerr << endl;
  }

  // copy
  cerr << endl;
  Array2D<int,InitZero,CheckAll> b2d;
  b2d=a2d;
  for (uint j=0;j<b2d.ysize();j++) {
    for (uint i=0;i<b2d.xsize();i++) {
      cerr << b2d.get(i,j) << ' ';
    }
    cerr << endl;
  }

  // 3D array

  cerr << "=== Array3D<> ===" << endl;

  Array3D<int,InitZero,CheckAll> a3d(2,4,8);
  ForArray3D(a3d,i,j,k) {
    a3d.set(i,j,k) = i+(j+k*a3d.ysize())*a3d.xsize();
  }
  for (uint k=0;k<a3d.zsize();k++) {
    for (uint j=0;j<a3d.ysize();j++) {
      for (uint i=0;i<a3d.xsize();i++) {
        cerr << a3d.get(i,j,k) << ' ';
      }
      cerr << endl;
    }
    cerr << '-' << endl;
  }

  // copy
  cerr << endl;
  Array3D<int,InitZero,CheckAll> b3d;
  b3d=a3d;
  for (uint k=0;k<a3d.zsize();k++) {
    for (uint j=0;j<a3d.ysize();j++) {
      for (uint i=0;i<a3d.xsize();i++) {
        cerr << a3d.get(i,j,k) << ' ';
      }
      cerr << endl;
    }
    cerr << '-' << endl;
  }

  
  // Tuple for arrays

  {
    cerr << "=== Tuples for arrays ===" << endl;

    {
      cerr << "adressing:\n";
      Tuple<uint,3> sz = Triple(uint(2),uint(3),uint(4));
      cerr << "  array size:" << sz << endl;
      ForTuple(3,i,sz) {
        cerr << "    -> idx = " << i << " addr = " << addressOf(i,sz) << " indexOf(addressOf) = " << indexOf(addressOf(i,sz),sz) << endl;
      }
    }

    {
      cerr << "method 1:\n";
      Array2D<uint> a(Pair(uint(8),uint(10)));
      Tuple<uint,2> S=a.sizeTuple();
      for (Tuple<uint,2> I=0 ; !done(I,S) ; inc(I,S)) {
        a[I] = I[0]+I[1]*a.xsize();
      }
      for (uint j=0;j<a.ysize();j++) {
        for (uint i=0;i<a.xsize();i++) {
          cerr << a.get(i,j) << ' ';
        }
        cerr << endl;
      }
    }

    {
      cerr << "method 2:\n";
      Array2D<uint> b2d(Pair(uint(8),uint(10)));
      ForArrayND(2,b2d,t) {
        b2d[t] = t[0]+t[1]*b2d.xsize();
      }
      for (uint j=0;j<b2d.ysize();j++) {
        for (uint i=0;i<b2d.xsize();i++) {
          cerr << b2d.get(i,j) << ' ';
        }
        cerr << endl;
      }
    }
  }

  cerr << "=== ArrayND<> ===" << endl;
  {
    Tuple<uint,3> sz=Triple(uint(2),uint(3),uint(4));
    ArrayND<3,int,InitZero,CheckAll> aND(sz);
    uint n = 0;
    ForArrayND(3,aND,T) {
      aND.at(T) = n++;
    }

    for (uint k=0;k<aND.sizeTuple()[2];k++) {
      for (uint j=0;j<aND.sizeTuple()[1];j++) {
        for (uint i=0;i<aND.sizeTuple()[0];i++) {
          cerr << aND.at(Triple(i,j,k)) << ' ';
        }
        cerr << endl;
      }
      cerr << '-' << endl;
    }
  }

}

// -----------

// ------------------------------------------------------

class AnObject : public LibSL::Memory::TraceLeaks::LeakProbe<AnObject>
{
protected:
  int m_Int;
public:
  AnObject()            {m_Int=0;}
  AnObject(const AnObject& o) {}
  void set(int v)       {m_Int=v;}
  int  get()      const {return (m_Int);}
};

// ------------------------------------------------------

using namespace LibSL::Memory::Pointer;

typedef AutoPtr<AnObject> t_pointer;

// ------------------------------------------------------

class A
{
public:

  static int s_Cnt;

  A() 
  {
    cerr << "A()" << endl;
    s_Cnt ++;
  }

  virtual ~A() 
  {
    cerr << "~A()" << endl;
    s_Cnt --;
  }
};

int A::s_Cnt = 0;

// -----------------------

class B : public A
{
public:

  static int s_CntB;

  B() 
  {
    cerr << "B()" << endl;
    s_CntB ++;
  }

  ~B() 
  {
    cerr << "~B()" << endl;
    s_CntB --;
  }

  int cnt() { return s_CntB; }
  int cnt_const() const { return s_CntB; }

};

int B::s_CntB = 0;

// ------------------------------------------------------

void test_pointer()
{
  cerr << "=== Pointer<> ===" << endl;

  // test deletion on assignment
  if (1) {
    AutoPtr<A> a( new A() );
    a = AutoPtr<A>(new A());
    a = AutoPtr<A>(new A());
    a = AutoPtr<A>();
    sl_assert(A::s_Cnt == 0);
  }
cerr << endl;
  if (1) {
    AutoPtr<B> b( new B() );
    b = AutoPtr<B>( new B() );
    b = AutoPtr<B>(new B());
    b = AutoPtr<B>();
    sl_assert(B::s_CntB == 0);
    sl_assert(A::s_Cnt  == 0);
  }
cerr << endl;
  if (1) {
    AutoPtr<A> a( new B() );
    a = AutoPtr<A>(new B());
    a = AutoPtr<A>(NULL);
    sl_assert(B::s_CntB == 0);
    sl_assert(A::s_Cnt  == 0);
  }
cerr << endl;
  if (1) {
    {
      AutoPtr<A> a;
      ForIndex(n,4) {
        a = AutoPtr<A>(new B());
      }
    }
    // a = NULL;
    sl_assert(A::s_Cnt  == 0);
  }
  cerr << endl;

  if (1) {
    AutoPtr<A> a;
    AutoPtr<B> b;
    b = AutoPtr<B>(new B());
    a = AutoPtr<A>(b);
    cerr << "refCnt a = " << a.refCount() << endl;
    cerr << "refCnt b = " << b.refCount() << endl;
    a = AutoPtr<A>();
    cerr << "refCnt a = " << a.refCount() << endl;
    cerr << "refCnt b = " << b.refCount() << endl;
    b = AutoPtr<B>();
    cerr << "refCnt a = " << a.refCount() << endl;
    cerr << "refCnt b = " << b.refCount() << endl;
    sl_assert(B::s_CntB == 0);
    sl_assert(A::s_Cnt  == 0);
  }
  ////

  t_pointer ptr0(new AnObject());
  cerr << sprint("ptr0->set(2)\n");
  ptr0->set(2);
  cerr << sprint("ptr0->get() = %d\n",ptr0->get());

  AutoPtr<AnObject> ptr_null;
  cerr << sprint("ptr_null->set(2)\n");
  try {
    // test null pointer check
    ptr_null->set(2);
  } catch (LibSL::Errors::Fatal& err) {
    cerr << err.message() << endl;
  }

  t_pointer ptr_other( new AnObject() );

  cerr << sprint("ptr0->refCount() = %d\n",ptr0.refCount());
  {
    t_pointer ptr1=ptr0;

    cerr << sprint("  ptr1->set(99)\n");
    ptr1->set(99);
    cerr << sprint("  ptr0->get() = %d\n",ptr0->get());
    cerr << sprint("  ptr1->get() = %d\n",ptr1->get());

    cerr << sprint("  ptr0->refCount() = %d\n",ptr0.refCount());
    cerr << sprint("  ptr1->refCount() = %d\n",ptr1.refCount());
    {
      t_pointer ptr2=ptr1;
      cerr << sprint("    ptr0->refCount() = %d\n",ptr0.refCount());
      cerr << sprint("    ptr1->refCount() = %d\n",ptr1.refCount());
      cerr << sprint("    ptr2->refCount() = %d\n",ptr2.refCount());
    }
    cerr << sprint("  ptr0->refCount() = %d\n",ptr0.refCount());
    cerr << sprint("  ptr1->refCount() = %d\n",ptr1.refCount());
    cerr << sprint("ptr1=ptr_other\n",ptr1.refCount());
    ptr1=ptr_other;
    cerr << sprint("  ptr0->refCount() = %d\n",ptr0.refCount());
    cerr << sprint("  ptr1->refCount() = %d\n",ptr1.refCount());
    cerr << sprint("  ptr_other->refCount() = %d\n",ptr_other.refCount());
    cerr << sprint("ptr0=ptr_other\n",ptr1.refCount());
    ptr0=ptr_other;
  }
  cerr << sprint("ptr_other->refCount() = %d\n",ptr_other.refCount());
  cerr << sprint("ptr0->refCount() = %d\n", ptr0.refCount());

  ///////
  // AutoPtr stress tests

  {
    // This is fine
    AutoPtr<B> ptrB( new B() );
    AutoPtr<A> ptrA = AutoPtr<A>(ptrB);
    cerr << "ptrA ref count is " << ptrA.refCount() << endl;
    cerr << "ptrB ref count is " << ptrB.refCount() << endl;
  }
#if 0
    ///// Does nto compile
    AutoPtr<B> ptrB = new B();
    AutoPtr<A> ptrA = ptrB;
    cerr << "ptrA ref count is " << ptrA.refCount() << endl;
    cerr << "ptrB ref count is " << ptrB.refCount() << endl;
#endif
}

// -----------

void test_memory()
{
  cerr << sprint("\n\n-=< Testing Arrays >=-\n\n");
  test_array();
  cerr << sprint("\n\n-=< Testing Pointers >=-\n\n");
  test_pointer();
}

// -----------
