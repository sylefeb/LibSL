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
// LibSL::Geometry::Contour
// ------------------------------------------------------
//
// Extract contours from regular grids
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2013-10-01
// ------------------------------------------------------

#pragma once

#include <vector>
#include <LibSL/Errors/Errors.h>
#include <LibSL/Memory/Array.h>
#include <LibSL/Math/Math.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/Memory/ArrayRemap.h>

// ------------------------------------------------------

namespace LibSL {
  namespace Geometry {

    namespace Contour
    {
      using LibSL::Math::Tuple;
      using LibSL::Memory::Array::Array;

      enum e_Edge {e_LeftEdge=1,e_RightEdge=2,e_TopEdge=4,e_BottomEdge=8};
      enum e_EdgeTest {e_EdgeAny,e_EdgeBelow};

      int  edgeIndex   (e_Edge e);
      int  numEdges    (int edge_flag);
      v2f  edgeMidPoint(v3i e);
      v2i  edgeDirection(v3i e);
      v2i  edgeStartCorner(v3i e);
      v2i  edgeInnerPixelNeighbor(v3i e);
      int  getEdges(const Array2DRemap& a, e_EdgeTest etest, int refval, v2i p);
      v3i  nextEdge(const Array2DRemap& a, e_EdgeTest etest, int refval, v3i e);
      void extract(const Array2DRemap& a, e_EdgeTest seedtest, e_EdgeTest edgetest, int refval, std::vector<std::vector<v3i> >& _contours);

      void convertContourIntoEdgeMidPointPath(
        const std::vector<v3i>&          incontour,
        std::vector<v2f>&               _outpaths);

      void convertContourIntoPixelCornerPath(
        const std::vector<v3i>&          incontour,
        std::vector<v2i>&               _outpaths);

      int  simplify(const std::vector<v2i>& cnt, std::vector<v2i>& _simpler, int start, int end, float threshold,int idx);
      void simplify(const std::vector<v2i>& cnt, std::vector<v2i>& _simpler, float threshold);

      // Douglas-Peucker algorithm (use a distance-to-edge threshold)
      void simplifyDP(const std::vector<v2i>& cnt, std::vector<v2i>& _simpler, float threshold);

      // Two different weighting strategies for vertices elimination:
      // - e_WeightAreaOnly:  use triangle areas only
      // - e_WeightAngleArea: product of triangle area and cosine of the candidate vertex angle
      enum e_Weighting { e_WeightAreaOnly, e_WeightAngleArea };

      // Visvalingam's algorithm (use a triangle area threshold)
      void simplifyVA(const std::vector<v2i>& cnt, std::vector<v2i>& _simpler,
      	float triangleAreaThres, e_Weighting weighting = e_WeightAngleArea);

    }

  }
}

// ------------------------------------------------------
