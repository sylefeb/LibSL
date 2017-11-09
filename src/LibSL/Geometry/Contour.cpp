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
// ------------------------------------------------------

#include "Contour.h"

#include <functional>
using namespace std;

// ------------------------------------------------------

#define NAMESPACE LibSL::Geometry::Contour

// ------------------------------------------------------

int NAMESPACE::numEdges(int edge_flag)
{
  int cnt = 0;
  if (edge_flag & e_LeftEdge)   cnt ++;
  if (edge_flag & e_RightEdge)  cnt ++;
  if (edge_flag & e_TopEdge)    cnt ++;
  if (edge_flag & e_BottomEdge) cnt ++;
  return cnt;
}

// ------------------------------------------------------

// SL: TODO: use templates
inline static bool edge_test(Contour::e_EdgeTest test, uchar cur, uchar neigh, int refval)
{
  if   (test == Contour::e_EdgeAny)    { return (cur == refval) && (neigh != refval); }
  else                 /*e_EdgeBelow*/ { return (cur == refval) && (neigh < refval);  }
}

int NAMESPACE::getEdges(const Array2DRemap& a,e_EdgeTest etest,int refval,v2i p)
{
  int edges  = 0;
  bool left  = edge_test(etest, *a.at(p[0], p[1]), *a.at(p[0] - 1, p[1]), refval);
  bool right = edge_test(etest, *a.at(p[0], p[1]), *a.at(p[0] + 1, p[1]), refval);
  bool top   = edge_test(etest, *a.at(p[0], p[1]), *a.at(p[0], p[1] - 1), refval);
  bool btm   = edge_test(etest, *a.at(p[0], p[1]), *a.at(p[0], p[1] + 1), refval);
  if ( left  ) edges |= e_LeftEdge;
  if ( right ) edges |= e_RightEdge;
  if ( top   ) edges |= e_TopEdge;
  if ( btm   ) edges |= e_BottomEdge;
  return edges;
}

// ---------------------------------------------------

int NAMESPACE::edgeIndex(e_Edge e)
{
  switch (e)
  {
  case e_LeftEdge:   return 0; break;
  case e_TopEdge:    return 1; break;
  case e_RightEdge:  return 2; break;
  case e_BottomEdge: return 3; break;
  }
  return -1;
}

// ---------------------------------------------------

v2f  NAMESPACE::edgeMidPoint(v3i e)
{
  switch (e[2])
  {
  case e_LeftEdge:   return v2f(e)+V2F(0.0f,0.5f); break;
  case e_TopEdge:    return v2f(e)+V2F(0.5f,0.0f); break;
  case e_RightEdge:  return v2f(e)+V2F(1.0f,0.5f); break;
  case e_BottomEdge: return v2f(e)+V2F(0.5f,1.0f); break;
  }
  sl_assert(false);
  return v2f(0);
}

// ---------------------------------------------------

v2i  NAMESPACE::edgeDirection(v3i e)
{
  switch (e[2])
  {
  case e_LeftEdge:   return v2i( 0,-1); break;
  case e_TopEdge:    return v2i( 1, 0); break;
  case e_RightEdge:  return v2i( 0, 1); break;
  case e_BottomEdge: return v2i(-1, 0); break;
  }
  sl_assert(false);
  return v2i(0);
}

// ---------------------------------------------------

v2i  NAMESPACE::edgeStartCorner(v3i e)
{
  switch (e[2])
  {
  case e_LeftEdge:   return v2i(e) + v2i(0, 1); break;
  case e_TopEdge:    return v2i(e) + v2i(0, 0); break;
  case e_RightEdge:  return v2i(e) + v2i(1, 0); break;
  case e_BottomEdge: return v2i(e) + v2i(1, 1); break;
  }
  sl_assert(false);
  return v2i(0);
}

// ---------------------------------------------------

v2i  NAMESPACE::edgeInnerPixelNeighbor(v3i e)
{
  switch (e[2])
  {
  case e_LeftEdge:   return v2i(e) + v2i(0, 0); break;
  case e_TopEdge:    return v2i(e) + v2i(0, 0); break;
  case e_RightEdge:  return v2i(e) + v2i(0, 0); break;
  case e_BottomEdge: return v2i(e) + v2i(0, 0); break;
  }
  sl_assert(false);
  return v2i(0);
}

// ---------------------------------------------------

v3i NAMESPACE::nextEdge(const Array2DRemap& a,e_EdgeTest etest,int refval,v3i e)
{
  v3i testTbl[4][3] = {
    {V3I(-1,-1,e_BottomEdge),V3I( 0,-1,e_LeftEdge)  ,V3I(0,0,e_TopEdge)},    // left
    {V3I( 1,-1,e_LeftEdge)  ,V3I( 1, 0,e_TopEdge)   ,V3I(0,0,e_RightEdge)},  // top
    {V3I( 1, 1,e_TopEdge)   ,V3I( 0, 1,e_RightEdge) ,V3I(0,0,e_BottomEdge)}, // right
    {V3I(-1, 1,e_RightEdge) ,V3I(-1, 0,e_BottomEdge),V3I(0,0,e_LeftEdge)},   // bottom
  };
  int t = edgeIndex((e_Edge)e[2]);
  ForIndex(i,3) {
    v3i next = V3I(e[0],e[1],0) + testTbl[t][i];
    if ((getEdges(a,etest,refval,v2i(next)) & next[2]) != 0) {
      return next;
    }
  }
  return v3i(-1);
}

// ---------------------------------------------------

void NAMESPACE::extract(
  const Array2DRemap& a,
  e_EdgeTest seedtest,
  e_EdgeTest etest,
  int refval,
  std::vector< std::vector<v3i> >& _contours)
{
  vector<v3i> startEdges;
  Array<int>  startEdgesPerLine(a.ysize() + 1);
  {
    // Timer tm("step1");
    // enumerate all possible start edges
    int vwidth       = a.vwidth();
    int vstride      = a.vstride();
    const uchar *pix = a.at(0, 0) - vstride;
    ForIndex(j, a.ysize()) {
      startEdgesPerLine[j] = (int)startEdges.size();
      uchar prev = *a.borderValue();
      ForIndex(i, a.xsize()) {
        // all contours have at least one left edge, so we test one these which is sufficient to discover all
        uchar cur = *(pix + vstride);
        if (edge_test(seedtest,cur,prev,refval)) {
          v3i e = V3I(i, j, e_LeftEdge);
          startEdges.push_back(e);
        }
        prev = cur;
        pix += vwidth;
      }
    }
    startEdgesPerLine[a.ysize()] = (int)startEdges.size();
  }

  {
    // Timer tm("step2");
    int nextStartEdge = -1;
    while (1) {

      // find a first edge
      do {
        nextStartEdge++;
        if (nextStartEdge >= (int)startEdges.size()) return;
      } while (startEdges[nextStartEdge] == v3i(-1)); // skip deleted

      v3i start = startEdges[nextStartEdge];

      // follow path - guaranteed to be a cycle
      std::vector<v3i> contour;
      v3i cur = start;
      do {

        // push this edge on the contour
        contour.push_back(cur);
        // remove from possible start edges, if this is a left edge
        if (cur[2] & e_LeftEdge) {
          if (cur[1] < start[1]) {
            // nothing to do, this is before the current start edge
          } else {
            // search and mark
            bool sanity = false;
            int left  = startEdgesPerLine[cur[1]];
            int right = startEdgesPerLine[cur[1] + 1] - 1;
#if 1
            ForRange(i, left, right) {
              if (startEdges[i] == cur) {
                startEdges[i] = v3i(-1);
                sanity = true;
                break;
              }
            }
#else
            while (1) {
              if (left >= right) {
                if (left == right && startEdges[left] == cur) {
                  startEdges[left] = v3i(-1);
                  sanity = true;
                }
                break;
              } else {
                int mid = (left + right) >> 1;
                if (cur[0] > startEdges[mid][0]) {
                  left = mid+1;
                } else {
                  right = mid;
                }
              }
            }
#endif
#if 0
            if (!sanity) {
              if (getEdges(a, seedtest, refval, v2i(cur)) & e_LeftEdge) {
                ImageRGB img(a.xsize(), a.ysize());
                ForArray2D(img.pixels(), i, j) {
                  img.pixel(i, j) = v3b(*a.at(i, j) * 32);
                }
                ForIndex(c, contour.size()) {
                  img.pixel(contour[c][0], contour[c][1])[1] = 0;
                }
                ForIndex(c, contour.size()) {
                  img.pixel(contour[c][0], contour[c][1])[1] += 64;
                  img.pixel(contour[c][0], contour[c][1])[2] = contour[c][2] * 32;
                }
                saveImage("dump\\countour.png", &img);
              }
            }
#endif
          }
        }
        // go to next edge
        cur = nextEdge(a, etest, refval, cur);

        sl_assert(cur != v3i(-1)); // inconsistent contour, should never happen

      } while (cur != start);

      _contours.push_back(contour);

    }
  }

}

// ---------------------------------------------------

void NAMESPACE::convertContourIntoEdgeMidPointPath(
  const std::vector<v3i>&          incontour,
  std::vector<v2f>&               _outpaths)
{
  ForIndex(i,incontour.size()) {
    _outpaths.push_back( edgeMidPoint(incontour[i]) );
  }
}

// ---------------------------------------------------

void NAMESPACE::convertContourIntoPixelCornerPath(
  const std::vector<v3i>&          incontour,
  std::vector<v2i>&               _outpaths)
{
  sl_assert(incontour.size() >= 1);
  v2i cur = edgeStartCorner(incontour[0]);
  ForIndex(i, incontour.size()) {
    _outpaths.push_back(cur);
    // next
    cur = cur + edgeDirection(incontour[i]);
  }
  _outpaths.push_back(cur);
  sl_assert(_outpaths.back() == _outpaths.front());
}

// ---------------------------------------------------

int NAMESPACE::simplify(
  const std::vector<v2i>& cnt, std::vector<v2i>& _simpler,
  int start, int end, float threshold, int idx)
{
  float maxDistance = 0.0f;
  bool cut = false;
  int added = 0;
  sl_assert(start >= 0);
  sl_assert(end >= 0);
  sl_assert(start< (int)cnt.size());
  sl_assert(end<(int)cnt.size());
  v2i first = cnt[start];
  v2i last = cnt[end];
  int pos = 0;
  if (start+1 < end) {
    if (first == last) {
      pos = (start + end) / 2;
      cut = true;
    } else {
      ForRange(i, start+1, end-1){
        v3f current = V3F((float)cnt[i][0], (float)cnt[i][1], 0.0f);
        float d = Distances::Point_Segment(current, V3F((float)first[0], (float)first[1], 0), V3F((float)last[0], (float)last[1], 0));
        if (d > threshold && d > maxDistance){
          cut = true;
          pos = i;
          maxDistance = d;
        }
      }
    }
  }
  int numberOfaddedLeft = 0;
  int numberOfaddedRight = 0;
  if (cut) {
    numberOfaddedLeft = NAMESPACE::simplify(cnt, _simpler, start, pos, threshold, idx);
    _simpler.insert(_simpler.begin() + idx + numberOfaddedLeft, cnt[pos]);
    added = 1;
    numberOfaddedRight = NAMESPACE::simplify(cnt, _simpler, pos, end, threshold, idx + numberOfaddedLeft + 1);
  }
  return numberOfaddedLeft + numberOfaddedRight + added;
}

// ---------------------------------------------------

void NAMESPACE::simplify(
  const std::vector<v2i>& cnt, std::vector<v2i>& _simpler,
  float threshold)
{
  sl_assert(cnt.size() >= 2);
  _simpler.push_back(cnt[0]);
  _simpler.push_back(cnt.back());
  simplify(cnt, _simpler, 0, (int)cnt.size() - 1, threshold, 1);
}

// ---------------------------------------------------

namespace {

  double pointSegmentSqDistance(v2d p, v2d a, v2d b) {
    double l2 = sqDistance(a, b);
    double t = dot(p - a, b - a);
    double lambda0, lambda1;
    v2d closest_point;
    if (t <= 0.0 || l2 == 0.0) {
      closest_point = a;
      lambda0 = 1.0;
      lambda1 = 0.0;
      return sqDistance(p, a);
    } else if (t >= l2) {
      closest_point = b;
      lambda0 = 0.0;
      lambda1 = 1.0;
      return sqDistance(p, b);
    }
    lambda1 = t / l2;
    lambda0 = 1.0 - lambda1;
    closest_point = lambda0 * a + lambda1 * b;
    return sqDistance(p, closest_point);
  }

  void simplifyDouglasPeuckerRecursive(
    const std::vector<v2i>& poly, std::vector<v2i>& result,
    int first, int last, double sqEpsilon)
  {
    // Find the point with the maximum distance
    double dmax2 = 0;
    int index = 0;
    const int n = (int)poly.size();
    if (last < first) {
      last += n;
    }
    for (int i = first+1; i < last; ++i) {
      double d2 = pointSegmentSqDistance(v2d(poly[i%n]), v2d(poly[first]), v2d(poly[last%n]));
      if (d2 > dmax2) {
        index = i % n;
        dmax2 = d2;
      }
    }

    // If max distance is greater than sqEpsilon, recursively simplify
    if (dmax2 > sqEpsilon) {
      // Recursive call
      simplifyDouglasPeuckerRecursive(poly, result, first, index, sqEpsilon);
      simplifyDouglasPeuckerRecursive(poly, result, index, last%n, sqEpsilon);
    } else {
      result.push_back(poly[first]); // poly[last] is implicit
    }
  }

}

/*
 * Douglas-Peucker algorithm
 * Worst-case: O(n^2). In practice: O(n.log(n)) most of the time.
 *
 * For further exploration of other simplifications algorithm, check out:
 * http://www.codeproject.com/Articles/114797/Polyline-Simplification
 *
 * As we do not test for self-intersections in the simplification process, one
 * might also consider using Clipper's CleanPolygons function as a post-process:
 * http://www.angusj.com/delphi/clipper/documentation/Docs/Overview/Rounding.htm
 */
void NAMESPACE::simplifyDP(const std::vector<v2i>& poly, std::vector<v2i>& result, float threshold) {
  // Use leftmost and rightmost points as initial endpoints
  int leftMost = 0;
  int rightMost = 0;
  for (int i = 1; i < (int) poly.size(); ++i) {
    if (poly[i] < poly[leftMost]) {
      leftMost = i;
    }
    if (poly[rightMost] < poly[i]) {
      rightMost = i;
    }
  }

  // Recursive call
  const double sqEpsilon = double(threshold) * double(threshold);
  result.clear();
  simplifyDouglasPeuckerRecursive(poly, result, leftMost, rightMost, sqEpsilon);
  simplifyDouglasPeuckerRecursive(poly, result, rightMost, leftMost, sqEpsilon);
}

// ---------------------------------------------------

namespace {

  double triangleArea(v2d p1, v2d p2, v2d p3) {
    double a = LibSL::Math::distance(p1, p2);
    double b = LibSL::Math::distance(p2, p3);
    double c = LibSL::Math::distance(p3, p1);
    double s = double(0.5) * (a + b + c);
    double A2 = s * (s - a) * (s - b) * (s - c);
    // the max is there to avoid some numerical problems.
    return std::sqrt(std::max(A2, 0.0));
  }

  double cosine(v2d p1, v2d p2, v2d p3) {
    double na2 = sqLength(p2 - p1);
    double nb2 = sqLength(p2 - p3);
    return dot(p2 - p1, p2 - p3) / std::sqrt(na2 * nb2);
  }

  // Different weighting are possible, e.g. pondering by the angle of by the current point, etc.
  double weightFunc(v2d p1, v2d p2, v2d p3, NAMESPACE::e_Weighting w) {
    double coeff = 1.0;
    if (w == NAMESPACE::e_WeightAngleArea) {
      coeff = 1.0 - 0.7 * cosine(p1, p2, p3);
    }
    return coeff * triangleArea(p1, p2, p3);
  }

}

/*
 * Visvalingam's algorithm
 * Complexity: O(n.log(n))
 *
 * Check out explanation here:
 * http://bost.ocks.org/mike/simplify/
 */
void NAMESPACE::simplifyVA(
  const std::vector<v2i>& poly, std::vector<v2i>& result,
  float triangleAreaThres, NAMESPACE::e_Weighting weighting)
{
  const int n = (int)poly.size();
  std::vector<int> prev(n);
  std::vector<int> next(n);
  std::vector<double> score(n);

  typedef std::pair<double, int> KeyVal;
  typedef std::priority_queue<KeyVal, std::vector<KeyVal>, std::greater<KeyVal> > PriorityQueue;

  PriorityQueue heap;

  // Init heap
  for (int x = 0, xl = n-1, xr = 1%n; x < n; ++x) {
    score[x] = weightFunc(v2d(poly[xl]), v2d(poly[x]), v2d(poly[xr]), weighting);
    prev[x] = xl;
    next[x] = xr;
    xl = x;
    xr = (xr + 1) % n;
    heap.push(std::make_pair(score[x], x));
  }

  int nkilled = 0;
  while (!heap.empty()) {
    double area = heap.top().first;
    int x = heap.top().second;
    heap.pop();

    // Skip invalid entries
    if (score[x] != area) { continue; }

    if (area >= triangleAreaThres || prev[x] == next[x]) {
      break;
    }

    // Assign score -1 to removed vertices
    score[x] = -1;
    ++nkilled;

    // Update linked list
    int xl = prev[x];
    int xr = next[x];
    sl_assert(xr != x || xl != x);
    next[xl] = xr;
    prev[xr] = xl;

    // Update scores of adjacent vertices in the heap
    score[xl] = weightFunc(v2d(poly[prev[xl]]), v2d(poly[xl]), v2d(poly[xr]), weighting);
    score[xr] = weightFunc(v2d(poly[xl]), v2d(poly[xr]), v2d(poly[next[xr]]), weighting);
    heap.push(std::make_pair(score[xl], xl));
    heap.push(std::make_pair(score[xr], xr));
  }

  result.clear();
  result.reserve(poly.size() - nkilled);
  for (int x = 0; x < n; ++x) {
    if (score[x] >= 0) {
      result.push_back(poly[x]);
    }
  }
}
