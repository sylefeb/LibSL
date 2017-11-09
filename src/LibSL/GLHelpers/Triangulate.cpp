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
// LibSL::GLHelpers
// ------------------------------------------------------
//
// Wrapper to OpenGL tesselation
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2015-06-15
// ------------------------------------------------------

#ifndef EMSCRIPTEN

#include "Triangulate.h"

#include <vector>

using namespace LibSL::GLHelpers;
using namespace std;

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#ifndef _GLUfuncptr
#ifdef WIN32
typedef GLvoid(CALLBACK *_GLUfuncptr)(void);
#else
typedef GLvoid(*_GLUfuncptr)(void);
#endif
#endif

#ifndef CALLBACK
#define CALLBACK
#endif

static void CALLBACK vertexCallback(GLvoid *dta,void *userData)
{
  t_triangulate_nfo *nfo = (t_triangulate_nfo*)userData;
  GLdouble   *v   = (GLdouble*)dta;
  // cerr << "vertexCallback: " << v[0] << ',' << v[1] << ',' << v[2] << endl;
  if (nfo->tris.back().size() == 3) {
    // next triangle
    nfo->tris.push_back( std::vector<v4d>() );
  }
  nfo->tris.back().push_back( v4d(v[0],v[1],0,0) );
}

static void CALLBACK beginCallBack(GLenum which,void *userData)
{
  t_triangulate_nfo *nfo = (t_triangulate_nfo*)userData;
  // cerr << "beginCallBack: " << which << endl;
  sl_assert(which == GL_TRIANGLES);
  nfo->tris.push_back( std::vector<v4d>() );
}

static void CALLBACK endCallBack(void *userData)
{
  // cerr << "endCallBack" << endl;
}

static void CALLBACK edgeFlagCallBack(void *userData,GLboolean)
{
}

static void CALLBACK errorCallback(void *userData,GLenum err)
{
  // cerr << "errorCallback" << endl;
  t_triangulate_nfo *nfo = (t_triangulate_nfo*)userData;
  nfo->pts.clear();
  nfo->tris.clear();
}

static void CALLBACK combineCallBack(
  GLdouble   coords[3],
  double    *vertex_data[4],
  GLfloat    weight[4],
  GLdouble **outData,
  void      *userData)
{
  sl_assert(false);
  // cerr << "combineCallBack" << endl;
  //t_triangulate_nfo *nfo = (t_triangulate_nfo*)userData;
  //nfo->pts.push_back( v4d(coords[0],coords[1],0,0) );
  //*outData = &(nfo->pts.back()[0]);
}

// ------------------------------------------------

bool LibSL::GLHelpers::triangulate(
  const std::vector<std::vector<v2i> >& paths, 
  t_triangulate_nfo& _triangulation, int widing)
{
  ForIndex(p, paths.size()) {
    ForIndex(i, paths[p].size()) {
      _triangulation.pts.push_back(v4d(paths[p][i][0], paths[p][i][1], 0, 0 ));
    }
  }
  // tesselate polygon
  GLUtesselator *tess = gluNewTess();
  gluTessNormal(tess, 0, 0, 1);
  gluTessProperty(tess, GLU_TESS_WINDING_RULE, widing > 0 ? GLU_TESS_WINDING_POSITIVE : GLU_TESS_WINDING_NEGATIVE);
  gluTessProperty(tess, GLU_TESS_TOLERANCE, 0.0);
  gluTessCallback(tess, GLU_TESS_VERTEX_DATA, (_GLUfuncptr)vertexCallback);
  gluTessCallback(tess,GLU_TESS_BEGIN_DATA,    (_GLUfuncptr)beginCallBack);
  gluTessCallback(tess,GLU_TESS_END_DATA,      (_GLUfuncptr)endCallBack);
  gluTessCallback(tess,GLU_TESS_EDGE_FLAG_DATA,(_GLUfuncptr)edgeFlagCallBack);
  // gluTessCallback(tess,GLU_TESS_COMBINE_DATA,  (_GLUfuncptr)combineCallBack);
  gluTessCallback(tess, GLU_TESS_ERROR_DATA,   (_GLUfuncptr)errorCallback);
  gluTessBeginPolygon(tess, &_triangulation);
  int i = 0;
  ForIndex(p,paths.size()) {
    gluTessBeginContour(tess);
    ForIndex(j,paths[p].size()) {
      gluTessVertex(tess, &_triangulation.pts[i][0], &_triangulation.pts[i][0]);
      i ++;
    }
    gluTessEndContour(tess);
  }
  gluTessEndPolygon(tess);
  gluDeleteTess(tess);
  return _triangulation.pts.size() > 0;
}

// ------------------------------------------------

#endif