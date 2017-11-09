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
#include "LibSL.precompiled.h"
// -------------------------------------------------------

using namespace std;

// -------------------------------------------------------

#include "MeshEditing.h"

#define NAMESPACE LibSL::Mesh::MeshEditing

// -------------------------------------------------------

typedef LibSL::Math::Tuple<float,6>   v6f;
typedef struct
{
  LibSL::Math::v3f pos;
  LibSL::Math::v3f nrm;
  LibSL::Math::v2f uv;
} t_VertexData;

// -------------------------------------------------------

void NAMESPACE::cutMesh(const TriangleMesh_Ptr  mesh,
                        const Plane<3>&         pl,
                        TriangleMesh_Ptr&      _front,
                        TriangleMesh_Ptr&      _back)
{
  sl_assert(_front.isNull());
  sl_assert(_back .isNull());
  vector<Tuple<v6f,3> > ftris;
  vector<Tuple<v6f,3> > btris;

  /*
  // early rejection
  Geometry::AAB<3> mbox = mesh->bbox();
  if (!Intersections::Plane_AABox(pl,mbox)) {
    if (pl.distance(mbox.center()) > 0) {
      _front = mesh->clone();
    } else {
      _back  = mesh->clone();
    }
  }
  */
  // for each mesh triangle
  ForIndex(t,mesh->numTriangles()) {
    // box and polygon for this triangle
    Geometry::AAB<3>         box;
    Geometry::Polygon<3,v6f> poly;
    ForIndex(pt,3) {
      t_VertexData *dta = (t_VertexData *)mesh->vertexDataAt(mesh->triangleAt(t)[pt]);
      box.addPoint(dta->pos);
      v6f p;
      ForIndex(i,3) {
        p[i  ] = dta->pos[i];
        p[i+3] = dta->nrm[i];
      }
      poly.add( p );
    }
    // check for intersection
    if (!Intersections::Plane_AABox(pl,box)) {
      // none => add to correct side
      if (pl.distance(box.center()) > 0) {
        ftris.push_back(Tuple<v6f,3>());
        ftris.back()[0] = poly.vertexAt(0);
        ftris.back()[1] = poly.vertexAt(1);
        ftris.back()[2] = poly.vertexAt(2);
      } else {
        btris.push_back(Tuple<v6f,3>());
        btris.back()[0] = poly.vertexAt(0);
        btris.back()[1] = poly.vertexAt(1);
        btris.back()[2] = poly.vertexAt(2);
      }
    } else {
      if (poly.size() < 3) continue;
      // => split
      Geometry::Polygon<3,v6f> fpoly,bpoly;
      switch (poly.cut(pl,fpoly,bpoly)) 
      {
      case LIBSL_POLYGON_FRONT:
        // add to front
        sl_assert(fpoly.size() > 2);
        ForRange(i,2,fpoly.size()-1) {
          ftris.push_back(Tuple<v6f,3>());
          ftris.back()[0] = fpoly.vertexAt(0);
          ftris.back()[1] = fpoly.vertexAt(i-1);
          ftris.back()[2] = fpoly.vertexAt(i);
        }
        break;
      case LIBSL_POLYGON_BACK:
        // add to back
        sl_assert(bpoly.size() > 2);
        ForRange(i,2,bpoly.size()-1) {
          btris.push_back(Tuple<v6f,3>());
          btris.back()[0] = bpoly.vertexAt(0);
          btris.back()[1] = bpoly.vertexAt(i-1);
          btris.back()[2] = bpoly.vertexAt(i);
        }
        break;
      case LIBSL_POLYGON_ON:
      case LIBSL_POLYGON_CUT:
        sl_assert(fpoly.size() > 2);
        sl_assert(bpoly.size() > 2);
        // add to front
        ForRange(i,2,fpoly.size()-1) {
          ftris.push_back(Tuple<v6f,3>());
          ftris.back()[0] = fpoly.vertexAt(0);
          ftris.back()[1] = fpoly.vertexAt(i-1);
          ftris.back()[2] = fpoly.vertexAt(i);
        }
        // add to back
        ForRange(i,2,bpoly.size()-1) {
          btris.push_back(Tuple<v6f,3>());
          btris.back()[0] = bpoly.vertexAt(0);
          btris.back()[1] = bpoly.vertexAt(i-1);
          btris.back()[2] = bpoly.vertexAt(i);
        }
        break;
      default:
        sl_assert(false);
        break;
      }
    }
  } // t
  // store in meshes
  if (!ftris.empty()) {
    TriangleMesh_Ptr mfront = TriangleMesh_Ptr(mesh->newInstance());
    mfront->allocate(uint(ftris.size()*3),uint(ftris.size()));
    uint v = 0;
    ForIndex(t,ftris.size()) {
      ForIndex(pt,3) {
        mfront->triangleAt(t)[pt] = v ++;
        t_VertexData *dta = (t_VertexData *)mfront->vertexDataAt(mfront->triangleAt(t)[pt]);
        ForIndex(i,3) {
          dta->pos[i] = ftris[t][pt][i  ];
          dta->nrm[i] = ftris[t][pt][i+3];
        }
      }
    }
    _front = mfront;
  }
  if (!btris.empty()) {
    TriangleMesh_Ptr mback = TriangleMesh_Ptr(mesh->newInstance());
    mback->allocate(uint(btris.size()*3),uint(btris.size()));
    uint v = 0;
    ForIndex(t,btris.size()) {
      ForIndex(pt,3) {
        mback->triangleAt(t)[pt] = v ++;
        t_VertexData *dta = (t_VertexData *)mback->vertexDataAt(mback->triangleAt(t)[pt]);
        ForIndex(i,3) {
          dta->pos[i] = btris[t][pt][i  ];
          dta->nrm[i] = btris[t][pt][i+3];
        }
      }
    }
    _back = mback;
  }
}

// -------------------------------------------------------
