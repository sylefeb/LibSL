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

#include <LibSL.h>

#include <iostream>
using namespace std;

// -----------

void extract_paths(const uchar *tbl,int refval)
{
  ForIndex(j,5) {
    ForIndex(i,8) {
      cerr << (char)('0'+tbl[j*8+i]) << ' ';
    }
    cerr << endl;
  }

  LibSL::Memory::Array::ArrayRemap a(tbl, 8,5, 1,0);

  vector<vector<v3i> > cnts;
  Contour::extract(a,Contour::EdgeBelow(),Contour::EdgeAny(),refval, cnts);

  ForIndex(c,cnts.size()) {
    std::cerr << "contour " << c << " " << endl;
    ForIndex(i,cnts[c].size()) {
      std::cerr << cnts[c][i] << " ";
    }
    std::cerr << endl;
    vector<vector<v2i> > paths;
    Contour::splitContourIntoPixelPaths(cnts[c],paths);
    ForIndex(p,paths.size()) {
      std::cerr << "  path " << p << " " << endl;
      ForIndex(i,paths[p].size()) {
        std::cerr << paths[p][i] << " ";
      }
      std::cerr << endl;
    }

    vector<v2f> edgepath;
    Contour::convertContourIntoEdgeMidPointPath(cnts[c],edgepath);
    std::cerr << "edgepath " << endl;
    ForIndex(p,edgepath.size()) {
      std::cerr << edgepath[p] << " ";
    }
    std::cerr << endl;
  }
}

// -----------

void test_contour()
{

  const uchar test0[] = {
    0,0,0,0,0,0,0,0,
    0,0,1,0,0,0,0,0,
    0,1,1,1,0,0,0,0,
    0,1,1,1,1,1,1,1,
    0,0,1,0,0,0,0,0,
  };

  const uchar test1[] = {
    0,0,0,0,0,0,0,0,
    1,1,1,1,1,0,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,1,1,1,
    0,0,1,0,0,0,0,0,
  };

  const uchar test2[] = {
    0,0,0,0,0,0,0,0,
    0,0,1,0,1,0,0,0,
    0,1,1,1,1,1,0,0,
    0,0,1,0,1,0,0,0,
    0,0,0,0,0,0,0,0,
  };

  const uchar test3[] = {
    0,0,0,0,0,0,0,0,
    0,0,1,0,0,1,0,0,
    0,1,1,1,1,1,1,0,
    0,0,1,0,0,1,0,0,
    0,0,0,0,0,0,0,0,
  };

  const uchar test4[] = {
    3,3,3,3,3,3,3,3,
    3,3,3,2,2,2,3,3,
    3,3,3,2,1,2,3,3,
    3,3,3,2,2,2,3,3,
    3,3,3,3,3,3,3,3,
  };

  cerr << "----- Test 0 ------\n";
  extract_paths( test0,1 );
  cerr << "----- Test 1 ------\n";
  extract_paths( test1,1 );
  cerr << "----- Test 2 ------\n";
  extract_paths( test2,1 );
  cerr << "----- Test 3 ------\n";
  extract_paths( test3,1 );
  cerr << "----- Test 4 ------\n";
  extract_paths( test4,3 );

}

// -----------
