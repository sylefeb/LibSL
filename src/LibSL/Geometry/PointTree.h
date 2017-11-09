/* --------------------------------------------------------------------
Author: Sylvain Lefebvre    sylvain.lefebvre@inria.fr

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
// LibSL::Geometry::PointTee
// ------------------------------------------------------
//
//  Simple octree for closest points queries
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2016-02-19
// ------------------------------------------------------

#pragma once

#include <LibSL/LibSL.common.h>

#include <LibSL/Memory/Pointer.h>
#include <LibSL/Memory/Array.h>
#include <LibSL/Math/Matrix4x4.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Math/Quaternion.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Mesh/Mesh.h>
#include <LibSL/Mesh/MeshFormat_mesh.h>
#include <LibSL/Geometry/AAB.h>

#include <set>

namespace LibSL {
  namespace Geometry {

    class PointTree
    {
    public: 

      typedef LibSL::Math::v3f        t_fp;
      typedef LibSL::Math::v3i        t_ip;
      typedef LibSL::Geometry::AAB<3> t_bx;

    private:

      static const int c_max_depth = 16; // after this depth, points are considered equal

      t_bx              m_BBox;
      std::vector<int>  m_Nodes;
      std::vector<t_fp> m_Points;

      int allocateChildren(int node_id);

      void insertPoint_recurse(t_fp p, int p_id, int node_id, const t_bx& box, int depth);
      void findClosest_recurse(t_fp q, t_bx& _bxq, int node_id, t_bx box, int depth, std::pair<int, t_fp>& _best_so_far, const std::set<int>& exclude);

    public:

      PointTree(t_bx bbox);
      void insertPoint(t_fp p);

      std::pair<int, t_fp> findClosest(t_fp q, float radius);
      std::pair<int, t_fp> findClosest(t_fp q, float radius, const std::set<int>& exclude);

    };

  } //namespace LibSL::Geometry
} //namespace LibSL

// -------------------------------------------------------
