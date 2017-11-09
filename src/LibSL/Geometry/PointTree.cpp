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

#include <algorithm>
#include <LibSL/Geometry/PointTree.h>

#define NAMESPACE LibSL::Geometry

int NAMESPACE::PointTree::allocateChildren(int node_id)
{
	int recs = (int)m_Nodes.size();
  m_Nodes.resize(m_Nodes.size() + 8);
  ForIndex(c, 8) {
    m_Nodes[recs + c] = 0;
  }
  m_Nodes[node_id] = (recs << 1) | 1;
  return recs;
}

static inline NAMESPACE::PointTree::t_bx childBox(const NAMESPACE::PointTree::t_bx& box, int ci, int cj, int ck)
{
  NAMESPACE::PointTree::t_bx cbox;
  cbox.minCorner() = box.minCorner() + box.extent() * NAMESPACE::PointTree::t_fp((float)ci, (float)cj, (float)ck) * 0.5f;
  cbox.maxCorner() = box.minCorner() + box.extent() * NAMESPACE::PointTree::t_fp((float)(ci + 1), (float)(cj + 1), (float)(ck + 1)) * 0.5f;
  return cbox;
}

void NAMESPACE::PointTree::insertPoint_recurse(t_fp p, int p_id, int node_id, const t_bx& box, int depth)
{
  if (depth >= c_max_depth) {
    return; // ignore this point (duplicate?)
  }
  // get children
  if (m_Nodes[node_id] == 0) {
    // nobody here, insert point!
    m_Nodes[node_id] = (p_id << 1) + 0;
  } else if ((m_Nodes[node_id] & 1) == 0) {
    // already has a point
    // => get point out
    int q_id = m_Nodes[node_id] >> 1;
    t_fp q = m_Points[q_id];
    // => subdivide and restart from this node
    int child_recs = allocateChildren(node_id);
    insertPoint_recurse(p, p_id, node_id, box, depth);
    insertPoint_recurse(q, q_id, node_id, box, depth);
  } else {
    int child_recs = m_Nodes[node_id] >> 1;
    t_fp rel = (p - box.minCorner()) * 2.0f / box.extent();
    //rel = clamp(rel, t_fp(0.0f), t_fp(1.0f - FLT_MIN));
    t_ip coord = t_ip(rel);
    coord[0] = std::min(1, std::max(0, coord[0]));
    coord[1] = std::min(1, std::max(0, coord[1]));
    coord[2] = std::min(1, std::max(0, coord[2]));
    int child_id = child_recs + (coord[0] + coord[1] * 2 + coord[2] * 2 * 2);
    insertPoint_recurse(p, p_id, child_id, childBox(box, coord[0], coord[1], coord[2]), depth + 1);
  }
}

void NAMESPACE::PointTree::findClosest_recurse(t_fp q, t_bx& _bxq, int node_id, t_bx box, int depth, std::pair<int, t_fp>& _best_so_far,const std::set<int>& exclude)
{
  if (m_Nodes[node_id] == 0) {
    // nobody here!
  } else if ((m_Nodes[node_id] & 1) == 0) {
    // there is a point here, check
    int p_id = m_Nodes[node_id] >> 1;
    if (exclude.find(p_id - 1) == exclude.end()) {
      t_fp p = m_Points[p_id];
      if (_best_so_far.first < 0 || sqLength(q - p) < sqLength(q - _best_so_far.second)) {
        _best_so_far.first = p_id;
        _best_so_far.second = p;
        float l = length(q - p);
        _bxq.minCorner() = q - t_fp(l);
        _bxq.maxCorner() = q + t_fp(l);
      }
    }
  } else {
    // recurse
    int child_recs = m_Nodes[node_id] >> 1;
    ForIndex(ck, 2) {
      ForIndex(cj, 2) {
        ForIndex(ci, 2) {
          int child_id = child_recs + (ci + cj * 2 + ck * 2 * 2);
          t_bx cbox = childBox(box, ci, cj, ck);
          if (cbox.intersect(_bxq)) {
            std::pair<int, t_fp> best = std::make_pair(0, t_fp(0));
            findClosest_recurse(q, _bxq, child_id, cbox, depth + 1, best, exclude);
            if (_best_so_far.first == 0) {
              _best_so_far = best;
            } else if (best.first > 0) {
              if (sqLength(q - best.second) < sqLength(q - _best_so_far.second)) {
                _best_so_far = best;
                float l = length(q - best.second);
                _bxq.minCorner() = q - t_fp(l);
                _bxq.maxCorner() = q + t_fp(l);
              }
            }
          }
        }
      }
    }
  }
}

NAMESPACE::PointTree::PointTree(t_bx bbox)
{
  m_BBox = bbox.enlarge(bbox.extent()*0.01f); // slightly enlarge to not reach faces
  m_Nodes.push_back(0); // root, no children, no point
  m_Points.push_back(t_fp(0)); // no points have id 0
}

void NAMESPACE::PointTree::insertPoint(t_fp p)
{
	int p_id = (int)m_Points.size();
  m_Points.push_back(p);
  insertPoint_recurse(p, p_id, 0, m_BBox, 0);
}

std::pair<int, NAMESPACE::PointTree::t_fp> NAMESPACE::PointTree::findClosest(t_fp q, float radius, const std::set<int>& exclude)
{
  t_bx bxq;
  bxq.minCorner() = q - t_fp(radius);
  bxq.maxCorner() = q + t_fp(radius);
  std::pair<int, t_fp> best = std::make_pair(0, t_fp(0));
  findClosest_recurse(q, bxq, 0, m_BBox, 0, best, exclude);
  sl_assert(best.first < (int)m_Points.size());
  best.first--; // indices start at 1 in PointTree
  return best;
}

std::pair<int, NAMESPACE::PointTree::t_fp> NAMESPACE::PointTree::findClosest(t_fp q, float radius)
{
  t_bx bxq;
  bxq.minCorner() = q - t_fp(radius);
  bxq.maxCorner() = q + t_fp(radius);
  std::pair<int, t_fp> best = std::make_pair(0, t_fp(0));
  std::set<int> exclude;
  findClosest_recurse(q, bxq, 0, m_BBox, 0, best, exclude);
  sl_assert(best.first < (int)m_Points.size());
  best.first--; // indices start at 1 in PointTree
  return best;
}

// -------------------------------------------------------
