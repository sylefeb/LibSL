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
// --------------------------------------------------------------

#include <iostream>
#include <ctime>
#include <cmath>

// --------------------------------------------------------------

#include <LibSL/LibSL.h>

// --------------------------------------------------------------

using namespace std;

// --------------------------------------------------------------

LIBSL_WIN32_FIX

// --------------------------------------------------------------

typedef struct
{
  LibSL::Math::v3f pos;
  LibSL::Math::v3f nrm;
  LibSL::Math::v2f uv;
} t_VertexData;

typedef MVF3(mvf_position_3f,mvf_normal_3f,mvf_texcoord0_2f) t_VertexFormat;

TriangleMesh_Ptr                               g_Mesh;

/* -------------------------------------------------------- */

int main(int argc, char **argv)
{
  try {

    if (argc < 2) {
      cerr << "Usage: %s <mesh_file>\n" << endl;
      cerr << "       output will be smooth.<mesh_file>.mesh" << endl;
      return (0);
    }

    // load mesh
    cerr << "Loading mesh      ";
    g_Mesh     = TriangleMesh_Ptr(loadTriangleMesh<t_VertexData,t_VertexFormat>(argv[1]));
    cerr << "[OK]" << endl;

    cerr << "Smoothing         ";
	  v3f ex = g_Mesh->bbox().extent();
	  g_Mesh->mergeVertices( 0.001f * tupleMax(ex) );
    Array<v3f> nrms;
    nrms.allocate( g_Mesh->numVertices() );
    nrms.fill( 0 );
    ForIndex(t, g_Mesh->numTriangles() ) {
      v3u tri = g_Mesh->triangleAt(t);
      v3f pts[3];
      ForIndex(i,3) {
        pts[i] = g_Mesh->posAt(tri[i]);
      }
      v3f n = cross( pts[1]-pts[0] , pts[2]-pts[0] );
      ForIndex(i,3) {
        nrms[ tri[i] ] += n;
      }
    }
    ForIndex(v,nrms.size()) {
      ((t_VertexData*)g_Mesh->vertexDataAt(v))->nrm = normalize_safe( nrms[v] );
    }

    cerr << "[OK]" << endl;

    cerr << "Saving mesh       ";
    string name = extractFileName( argv[1] );
    saveTriangleMesh(("smooth."+name+".mesh").c_str(),g_Mesh.raw());
    cerr << "[OK]" << endl;

  } catch (Fatal& e) {
#ifdef WIN32
    MessageBoxA(NULL,e.message(),"",MB_OK);
#endif
    cerr << Console::red << e.message() << Console::gray << endl;
    return (-1);
  }
  return (0);
}

/* -------------------------------------------------------- */
