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
// LibSL::Geometry::Morpho
// ------------------------------------------------------
//
// Morphology operators based on (2D) distance fields
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2014-06-12
// ------------------------------------------------------

#pragma once

#include <LibSL/Errors/Errors.h>
#include <LibSL/Memory/Array2D.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Geometry/AAB.h>
#include <LibSL/Mesh/Mesh.h>
#include <climits>

// ------------------------------------------------------

namespace LibSL {
  namespace Geometry {

    namespace Morpho
    {
      using LibSL::Math::Tuple;
      using LibSL::Memory::Array::Array2D;

      class Morpho
      {
      private:
        Array2D<Tuple<int,2> > m_Dist;
        void prepare(const Array2D<bool>& _array);
      public:

        void hit_and_miss(Array2D<bool>& _array, signed char kernel[3][3]);

        void dilate(Array2D<bool>& _array, float radius);
        void erode(Array2D<bool>& _array, float radius);
        void close(Array2D<bool>& _array, float radius);
        void open(Array2D<bool>& _array, float radius);
        void skeleton(Array2D<bool>& _array, float thres);
        void thinning(Array2D<bool>& _array);
        void negate(Array2D<bool>& _array);

        void Union(const Array2D<bool>& a, const Array2D<bool>& b, Array2D<bool>& _result);
        void Difference(const Array2D<bool>& a, const Array2D<bool>& b, Array2D<bool>& _result);
        void Intersection(const Array2D<bool>& a, const Array2D<bool>& b, Array2D<bool>& _result);
      };

    }

  }
}

// ------------------------------------------------------
