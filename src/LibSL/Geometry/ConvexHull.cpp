// -----------------------------------------------------------------------------
// Wrapper around the qhull library
// Written by Qingnan Zhou for PyMesh, adapted for LibSL by Jérémie Dumas
// -----------------------------------------------------------------------------
#include "ConvexHull.h"
#include "LibSL.precompiled.h"

#include <LibSL/Geometry/AAB.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Vertex.h>

#include <LibSL/System/System.h>

#include <sstream>
#include <string>

#ifdef _MSC_VER
#include <iterator>
#endif

#if defined(USE_CXX11) || defined(USE_CXX14) || defined(USE_CXX17)
#include <mutex>
#else
#pragma message ("Convex hull compiled without multithread support")
#endif

extern "C" {
#include <qhull_a.h>
}

////////////////////////////////////////////////////////////////////////////////

//#define NAMESPACE LibSL::Geometry
#define NAMESPACE

namespace LibSL {
namespace Geometry {

////////////////////////////////////////////////////////////////////////////////
namespace {
  typedef LibSL::Math::v2d Vector2d;
  typedef LibSL::Math::v3d Vector3d;

  bool is_positively_oriented(
      const Vector3d& v0, const Vector3d& v1, const Vector3d& v2) {
    return dot(cross(v0, v1), v2) > 0.0;
  }

  bool is_positively_oriented(const Vector2d& v0, const Vector2d& v1) {
    return v0[0]*v1[1] - v0[1]*v1[0] > 0.0;
  }
}

// -----------------------------------------------------------------------------

#ifdef USE_CXX11
std::mutex g_QhullLock;
#endif

////////////////////////////////////////////////////////////////////////////////
template<int Dim>
void ConvexHullEngine<Dim>::run(
    const std::vector<VectorXd>& points)
{
  const size_t dim = Dim;
  const size_t num_points = points.size();
  char flags[64];
	sprintf_s(flags, 64, "qhull Qt");
  coordT* data = new coordT[num_points * dim];
  if (num_points != 0) {
    const double* rawPtr = &points[0][0];
#ifdef _MSC_VER
		std::copy(rawPtr, rawPtr + num_points * dim, stdext::checked_array_iterator<double*>(data, num_points * dim));
#else
    std::copy(rawPtr, rawPtr + num_points * dim, data);
#endif
  }

#ifdef USE_CXX11
  g_QhullLock.lock();
#endif
  int err = qh_new_qhull((int)dim, (int)num_points, data, false, flags, NULL, stderr);

  if (!err) {
      extractHull(points);
  } else {
      throw LibSL::Errors::Fatal("Qhull error: ");
  }

  qh_freeqhull(!qh_ALL);
#ifdef USE_CXX11
  g_QhullLock.unlock();
#endif
  delete [] data;
  reorientFaces();
}

////////////////////////////////////////////////////////////////////////////////
template<int Dim>
void ConvexHullEngine<Dim>::extractHull(
    const std::vector<VectorXd>& points)
{
  const size_t dim = Dim;
  const size_t num_input_points = points.size();
  const size_t num_faces = qh num_facets;
  const size_t num_vertices = qh num_vertices;

  size_t index = 0;
  m_Vertices.resize(num_vertices);
  m_IndexMap.resize(num_vertices);
  std::vector<int> inverse_map(num_input_points, -1);
  vertexT* vertex, **vertexp;
  FORALLvertices {
      size_t i = qh_pointid(vertex->point);
#ifdef _MSC_VER
	std::copy(vertex->point, vertex->point + dim, stdext::checked_array_iterator<pointT*>(&m_Vertices[index][0], dim));
#else
      std::copy(vertex->point, vertex->point + dim, &m_Vertices[index][0]);
#endif
      m_IndexMap[index] = (int)i;
      inverse_map[i] = (int)index;
      index++;
  }

  index = 0;
  m_Faces.resize(num_faces);
  facetT *facet;
  FORALLfacets {
		size_t i = 0;
    FOREACHvertex_( facet->vertices ) {
      sl_assert(inverse_map[qh_pointid(vertex->point)] != -1);
      m_Faces[index][(int)i] = inverse_map[qh_pointid(vertex->point)];
      i++;
    }
    index++;
  }
}

////////////////////////////////////////////////////////////////////////////////
template<>
void ConvexHullEngine<2>::reorientFaces() {
  const size_t dim = 2;
  const size_t num_faces = m_Faces.size();
  if (m_Vertices.empty()) { return; }
  VectorXd ctr = 0;
  for (size_t i = 0; i<m_Vertices.size(); i++) {
    ctr = ctr + m_Vertices[i];
  }
  VectorXd center = ctr / (double)m_Vertices.size();
  for (size_t i=0; i<num_faces; i++) {
    VectorXd v0 = m_Vertices[m_Faces[i][0]] - center;
    VectorXd v1 = m_Vertices[m_Faces[i][1]] - center;
    if (!is_positively_oriented(v0, v1)) {
      std::swap(m_Faces[i][0], m_Faces[i][1]);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
template<>
void ConvexHullEngine<3>::reorientFaces() {
  const size_t dim = 3;
  const size_t num_faces = m_Faces.size();
  if (m_Vertices.empty()) { return; }
  VectorXd ctr = 0;
  for (size_t i=0; i<m_Vertices.size(); i++) {
    ctr = ctr + m_Vertices[i];
  }
  VectorXd center = ctr / (double)m_Vertices.size();
  for (size_t i=0; i<num_faces; i++) {
    VectorXd v0 = m_Vertices[m_Faces[i][0]] - center;
    VectorXd v1 = m_Vertices[m_Faces[i][1]] - center;
    VectorXd v2 = m_Vertices[m_Faces[i][2]] - center;
    if (!is_positively_oriented(v0, v1, v2)) {
      std::swap(m_Faces[i][0], m_Faces[i][1]);
    }
  }
}

// -----------------------------------------------------------------------------

} // namespace LibSL::Geometry
} // namespace LibSL

////////////////////////////////////////////////////////////////////////////////
// Explicit template specializations for 2D and 3D cases
////////////////////////////////////////////////////////////////////////////////

template class LibSL::Geometry::ConvexHullEngine<2>;
template class LibSL::Geometry::ConvexHullEngine<3>;
