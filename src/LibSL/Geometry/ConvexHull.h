// -----------------------------------------------------------------------------
// Wrapper around the qhull library
// Written by Qingnan Zhou for PyMesh, adapted for LibSL by Jérémie Dumas
// -----------------------------------------------------------------------------

#pragma once

#include <LibSL/Math/Math.h>
#include <LibSL/Math/Tuple.h>

#include <vector>

namespace LibSL {
  namespace Geometry {
    /**
     * ConvexHullEngine defines the minimalistic interface that compute the convex
     * hull of a set of input points.  The output is the vertices and faces of the
     * triangulated convex hull.  Each face is correctly orienated with normal
     * pointing away from the center.  In addition, the algorithm also returns an
     * index for each convex hull vertex that specifies the corresponding input
     * point.
     */
    template<int Dim>
    class ConvexHullEngine {
      public:
        
        typedef LibSL::Math::Tuple<double, Dim> VectorXd;
        typedef LibSL::Math::Tuple<int   , Dim> VectorXi;

        ConvexHullEngine() { m_Context = NULL; }

      public:
        void run(const std::vector<VectorXd>& points);

        const std::vector<VectorXd>& getVertices() const { return m_Vertices; }
        const std::vector<VectorXi>& getFaces()    const { return m_Faces; }
        const std::vector<int>&      getIndexMap() const { return m_IndexMap; }

      protected:
        void extractHull(const std::vector<VectorXd>& points);
        void reorientFaces();

      protected:
        std::vector<VectorXd> m_Vertices;
        std::vector<VectorXi> m_Faces;
        std::vector<int>      m_IndexMap;
        void                 *m_Context;
    };

    // Explicit specializations for 2D and 3D cases
    class ConvexHull2d : public ConvexHullEngine<2> {};
    class ConvexHull3d : public ConvexHullEngine<3> {};

  } // namespace LibSL::Geometry
} // namespace LibSL
