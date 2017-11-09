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
// #include "precompiled.h"

#include <LibSL/DataStructures/Hierarchy.h>
#include <LibSL/DataStructures/Pow2Tree.h>
using namespace LibSL::DataStructures;
#include <LibSL/CppHelpers/CppHelpers.h>
using namespace LibSL::CppHelpers;
#include <LibSL/Math/Tuple.h>
using namespace LibSL::Math;
#include <LibSL/DataStructures/Pod.h>

// -----------

#include <iostream>
using namespace std;

// -----------

template <class T_Hierarchy>
void outputHierarchy(typename T_Hierarchy::t_Pointer h,uint lvl=0)
{
  ForIndex(n,1+lvl) {
    cerr << "  ";
  }
  cerr << "|-" << h->data() << endl;

  for (typename T_Hierarchy::ChildrenIterator I=h->children();!I.end();I.next()) {
    outputHierarchy<T_Hierarchy>(I.current(),lvl+1);
  }

}

// -----------

template <class T_QTree>
void outputQuadtree(typename T_QTree::t_Pointer h,uint lvl=0)
{
  if (h->isLeaf())
    return;

  ForIndex(n,1+lvl) {
    cerr << "  ";
  }
  cerr << "|-" << h->data() << endl;

  ForIndex(j,2) {
    ForIndex(n,1+lvl) {
      cerr << "  ";
    } 
    cerr << "  ";
    cerr << '|';
    ForIndex(i,2) {
      if (h->childAt(Pair(i,j)).isNull()) {
        cerr << "  ";
      } else {
        cerr << sprint("%2d",h->childAt(Pair(i,j))->data());
      }
      cerr << '|';
    }
    cerr << endl;
  }

  for (typename T_QTree::ChildrenIterator I=h->children();!I.end();I.next()) {
    outputQuadtree<T_QTree>(I.current(),lvl+1);
  }

}

// -----------

  POD_MEMBER(char ,Member0);
  POD_MEMBER(int  ,Member1);
  POD_MEMBER(float,Member2);

  POD_MEMBER(float,Member3);
  POD_MEMBER(double,Member4);

  typedef POD3(Member0,Member1,Member2) t_PodA;

  typedef char t_Text[32];
  POD_MEMBER(t_Text,Member5);

  typedef POD3(Member3,Member4,Member5) t_PodB;

  typedef POD::PodAppend< t_PodA,t_PodB >::Result t_Pod;

  POD_MEMBER(t_Text,SomeText);
  typedef POD1(SomeText) t_PodText;

// -----------

void test_datastructures()
{
  cerr << "---------------------------" << endl;
  cerr << " LibSL::DataStructures " << endl;
  cerr << "---------------------------" << endl;

  cerr << "\nHierarchy\n" << endl;
  Hierarchy<uint>::t_AutoPtr h=new Hierarchy<uint>(0);
  h->addChild(1);
  h->addChild(2);
  h->addChild(3);
  for (Hierarchy<uint>::ChildrenIterator I=h->children();
    !I.end();I.next()) {
      ForIndex(n,I.current()->data()) {
        I.current()->addChild(I.current()->data()*10+n);
      }
      if (I.current()->data() > 2) {
        for (Hierarchy<uint>::ChildrenIterator Ic=I.current()->children();
          !Ic.end();Ic.next()) {
            Ic.current()->addChild(Ic.current()->data()*10);
        }
      }
  }

  outputHierarchy<Hierarchy<uint> >(h);

  cerr << "Erasing all level 1 children" << endl;
  for (Hierarchy<uint>::ChildrenIterator I=h->children();
    !I.end();I.next()) {
      Hierarchy<uint>::ChildrenIterator Ic=I.current()->children();
      while (!Ic.end()) {
        Ic.remove();
      }
  }

  outputHierarchy<Hierarchy<uint> >(h);


  cerr << "Building another hierarchy" << endl;
  Hierarchy<uint>::t_AutoPtr t0=new Hierarchy<uint>(0);
  t0->addChild(1);
  t0->addChild(2);
  Hierarchy<uint>::ChildrenIterator It0=t0->children();
  It0.current()->addChild(3);
  Hierarchy<uint>::ChildrenIterator It1=It0.current()->children();
  It1.current()->addChild(4);
  It1.current()->addChild(5);
  It0.next();
  It0.current()->addChild(6);
  It0.current()->addChild(7);

  outputHierarchy<Hierarchy<uint> >(t0);

  cerr << "Attaching it to previous, child 2" << endl;

  Hierarchy<uint>::ChildrenIterator Ih0=h->children();
  Ih0.next();
  Ih0.current()->addChild(t0);

  outputHierarchy<Hierarchy<uint> >(h); 

  t0=NULL;

  //////////////////////////////////

  cerr << "---------------------------" << endl;
  cerr << "\nPow2Tree\n" << endl;
  cerr << "---------------------------" << endl;
  cerr << "* BinaryTree\n" << endl;

  cerr << "Building a binary tree ";
  BinaryTree<uint>::t_Pointer btree = new BinaryTree<uint>(0);
  btree->addChild(Single(0),1);
  btree->addChild(Single(1),2);
  btree->childAt(Single(0))->addChild(Single(0),3);
  btree->childAt(Single(0))->addChild(Single(1),4);
  btree->childAt(Single(1))->addChild(Single(1),5);
  btree->childAt(Single(1))->childAt(Single(1))->addChild(Single(0),6);
  cerr << '.' << endl;

  outputHierarchy<BinaryTree<uint> >(btree);
  cerr << "number of nodes  = " << btree->numNodes() << endl;
  cerr << "number of leaves = " << btree->numLeaves() << endl;
  cerr << "tree height      = " << btree->height() << endl;

  cerr << "Building another bintree";
  BinaryTree<uint>::t_Pointer btree2=new BinaryTree<uint>(99);
  btree2->addChild(Single(0),10);
  btree2->addChild(Single(1),20);
  btree2->childAt(Single(0))->addChild(Single(0),30);
  btree2->childAt(Single(0))->childAt(Single(0))->addChild(Single(1),60);
  btree2->childAt(Single(0))->addChild(Single(1),40);
  btree2->childAt(Single(1))->addChild(Single(1),50);
  cerr << '.' << endl;
  cerr << "Connecting it to the first tree in place of child '2'";
  btree->addChild(Single(1),btree2);
  cerr << '.' << endl;
  outputHierarchy<BinaryTree<uint> >(btree);
  cerr << "number of nodes  = " << btree->numNodes() << endl;
  cerr << "number of leaves = " << btree->numLeaves() << endl;
  cerr << "tree height      = " << btree->height() << endl;

  cerr << "Deleting a subtree" << endl;
  btree->childAt(Single(1))->deleteChild(Single(0));
  outputHierarchy<BinaryTree<uint> >(btree);
  cerr << "number of nodes  = " << btree->numNodes() << endl;
  cerr << "number of leaves = " << btree->numLeaves() << endl;
  cerr << "tree height      = " << btree->height() << endl;

  delete (btree2);
  delete (btree);

  cerr << "---------------------------" << endl;
  cerr << "* QuadTree\n" << endl;
  cerr << "Building a quadtree ";
  QuadTree<uint>::t_Pointer qtree=new QuadTree<uint>(0);
  qtree->addChild(Pair(0,0),1);
  qtree->addChild(Pair(1,0),2);
  qtree->addChild(Pair(0,1),3);
  qtree->addChild(Pair(1,1),4);
  qtree->childAt(Pair(0,1))->addChild(Pair(0,0),41);
  qtree->childAt(Pair(0,1))->addChild(Pair(1,1),44);
  qtree->childAt(Pair(1,1))->addChild(Pair(1,0),32);
  qtree->childAt(Pair(1,1))->addChild(Pair(0,1),33);
  qtree->childAt(Pair(1,1))->childAt(Pair(1,0))->addChild(Pair(1,1),94);
  cerr << '.' << endl;

  outputQuadtree<QuadTree<uint> >(qtree);
  cerr << "number of nodes  = " << qtree->numNodes() << endl;
  cerr << "number of leaves = " << qtree->numLeaves() << endl;
  cerr << "tree height      = " << qtree->height() << endl;

  delete (qtree);

  //////////////////////////////////

  cerr << "---------------------------" << endl;
  cerr << "\nPod\n" << endl;
  cerr << "---------------------------" << endl;

  t_Pod pod;
  
  POD::PodStringInterface<t_Pod> str_interface( pod );
  str_interface.set("Member3","1.234f");
  str_interface.set("Member4","5.67891");
  str_interface.set("Member1","1234");
  str_interface.set("Member0","Z");
  str_interface.set("Member2","314.01234e-2f");
  str_interface.set("Member5","Testing_text");
  ForIndex(i,str_interface.numMembers()) {
    cerr << sprint("Pod member %d is named '%s' and has type '%s'; value = '%s'\n",i,
      str_interface.memberNfo(i).name,
      str_interface.memberNfo(i).type,
      str_interface.get(str_interface.memberNfo(i).name).c_str() 
      );
  }

#define TEST_POD_STRING(T) { t_PodText p; POD::PodStringInterface<t_PodText> sp( p ); sp.set("SomeText",T); sl_assert(sp.get("SomeText") == T); }
  TEST_POD_STRING("1212");
  TEST_POD_STRING("&&&");
  TEST_POD_STRING("11&1&&2&&22");
  TEST_POD_STRING("11&&1& &1 &2&  &2 1&2");
  TEST_POD_STRING("&1&2&1");

  //  cerr << "---------------------------" << endl;
  //  cerr << "* Octree\n" << endl;

}

// -----------
