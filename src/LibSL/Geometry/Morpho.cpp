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

#include "Morpho.h"

using namespace std;

// ------------------------------------------------------

#define NAMESPACE LibSL::Geometry::Morpho

// ------------------------------------------------------

void NAMESPACE::Morpho::prepare(const Array2D<bool>& _array)
{
  if (m_Dist.xsize() != _array.xsize() || m_Dist.ysize() != _array.ysize()) {
    m_Dist.erase();
    m_Dist.allocate(_array.xsize(), _array.ysize());
  }
}

// ------------------------------------------------------

void NAMESPACE::Morpho::dilate(Array2D<bool>& _array, float radius)
{
  prepare(_array);
  ForArray2D(_array, i, j) {
    m_Dist.at(i, j) = _array.at(i,j) ? v2i(0) : v2i(1 << 15, 0);
  }
  computeEuclidianDistanceField(m_Dist);
  ForArray2D(_array, i, j) {
    if (length(v2f(m_Dist.at(i, j))) < radius) {
      _array.at(i, j) = true;
    }
  }
}

// ------------------------------------------------------

void NAMESPACE::Morpho::erode(Array2D<bool>& _array, float radius)
{
  prepare(_array);
  ForArray2D(_array, i, j) {
    m_Dist.at(i, j) = _array.at(i, j) ? v2i(1 << 15, 0) : v2i(0);
  }
  computeEuclidianDistanceField(m_Dist);
  ForArray2D(_array, i, j) {
    if (length(v2f(m_Dist.at(i, j))) < radius) {
      _array.at(i, j) = false;
    }
  }
}

// ------------------------------------------------------

void NAMESPACE::Morpho::close(Array2D<bool>& _array, float radius)
{
  dilate(_array, radius);
  erode(_array, radius);
}

// ------------------------------------------------------

void NAMESPACE::Morpho::open(Array2D<bool>& _array, float radius)
{
  erode(_array, radius);
  dilate(_array, radius);
}

// ---------------------------------------------------

void NAMESPACE::Morpho::skeleton(Array2D<bool>& _array,float thres)
{
  prepare(_array);
  ForArray2D(_array, i, j) {
    m_Dist.at(i, j) = _array.at(i, j) ? v2i(1 << 15, 0) : v2i(0);
  }
  computeEuclidianDistanceField(m_Dist);
  ForArray2D(_array, i, j) {
    // find out local maxima
    _array.at(i, j) = false;
    v2i p = m_Dist.at(i, j);
    ForRange(nj, -1, 1) {
      ForRange(ni, -1, 1) {
        v2i q = m_Dist.at<Clamp>(i+ni, j+nj);
        if (length(v2f(p - q)) > thres) {
          _array.at(i, j) = true;
          break;
        }
      }
      if (_array.at(i, j)) break;
    }
  }
}

// ---------------------------------------------------

// TODO: boundary conditions
void NAMESPACE::Morpho::hit_and_miss(Array2D<bool>& _array, int8_t kernel[3][3])
{
  Array2D<bool> tmp = _array;
  ForArray2D(_array, i, j) {
    bool accept = true;
    ForRange(nj, -1, 1) {
      ForRange(ni, -1, 1) {
        int ki = ni + 1;
        int kj = nj + 1;
        int8_t k = kernel[ki][kj];
        if (k == 0) {
          if (tmp.at<Wrap>(i + ni, j + nj)) {
            accept = false;
          }
        } else if (k == 1) {
          if ( ! tmp.at<Wrap>(i + ni, j + nj)) {
            accept = false;
          }
        } // else ignore
        if (!accept) break;
      }
      if (!accept) break;
    }
    _array.at(i, j) = accept;
  }
#if 0
  ///// DEBUG
  static int cnt = 0;
  ImageRGB img(_array.xsize(), _array.ysize());
  ForImage((&img), i, j) {
    img.pixel(i, j) = _array.at(i, j) ? v3b(255) : v3b(0);
  }
  saveImage(sprint("dump\\%04d.hit.tga",cnt++), &img);
#endif
}

// ---------------------------------------------------

void NAMESPACE::Morpho::thinning(Array2D<bool>& _array)
{
  int8_t k1_b[3][3] =
  {{ 0, 0, 0},
   {-1, 1,-1},
   { 1, 1, 1}};
  int8_t k1_c[3][3] =
  { { 1,-1, 0 },
    { 1, 1, 0 },
    { 1,-1, 0 } };
  int8_t k1_a[3][3] =
  { { 1, 1, 1 },
    {-1, 1,-1 },
    { 0, 0, 0 } };
  int8_t k1_d[3][3] =
  { { 0,-1, 1 },
    { 0, 1, 1 },
    { 0,-1, 1 } };
  int8_t k2_d[3][3] =
  { {-1, 0, 0 },
    { 1, 1, 0 },
    {-1, 1,-1 } };
  int8_t k2_a[3][3] =
  { {-1, 1,-1 },
    { 1, 1, 0 },
    {-1, 0, 0 } };
  int8_t k2_b[3][3] =
  { {-1, 1,-1 },
    { 0, 1, 1 },
    { 0, 0,-1 } };
  int8_t k2_c[3][3] =
  { { 0, 0,-1 },
    { 0, 1, 1 },
    {-1, 1,-1 } };
  bool changed = true;
  Array2D<bool> map = _array;
  Array2D<bool> tmp;
  while (changed) {
    changed = false;
    tmp = map;
    hit_and_miss(tmp, k1_a);
    ForArray2D(tmp, i, j) { bool r = (map.at(i, j) && !tmp.at(i, j)); changed |= r ^ map.at(i, j); map.at(i, j) = r; }
    tmp = map;
    hit_and_miss(tmp, k1_b);
    ForArray2D(tmp, i, j) { bool r = (map.at(i, j) && !tmp.at(i, j)); changed |= r ^ map.at(i, j); map.at(i, j) = r; }
    tmp = map;
    hit_and_miss(tmp, k1_c);
    ForArray2D(tmp, i, j) { bool r = (map.at(i, j) && !tmp.at(i, j)); changed |= r ^ map.at(i, j); map.at(i, j) = r; }
    tmp = map;
    hit_and_miss(tmp, k1_d);
    ForArray2D(tmp, i, j) { bool r = (map.at(i, j) && !tmp.at(i, j)); changed |= r ^ map.at(i, j); map.at(i, j) = r; }
    tmp = map;
    hit_and_miss(tmp, k2_a);
    ForArray2D(tmp, i, j) { bool r = (map.at(i, j) && !tmp.at(i, j)); changed |= r ^ map.at(i, j); map.at(i, j) = r; }
    tmp = map;
    hit_and_miss(tmp, k2_b);
    ForArray2D(tmp, i, j) { bool r = (map.at(i, j) && !tmp.at(i, j)); changed |= r ^ map.at(i, j); map.at(i, j) = r; }
    tmp = map;
    hit_and_miss(tmp, k2_c);
    ForArray2D(tmp, i, j) { bool r = (map.at(i, j) && !tmp.at(i, j)); changed |= r ^ map.at(i, j); map.at(i, j) = r; }
    tmp = map;
    hit_and_miss(tmp, k2_d);
    ForArray2D(tmp, i, j) { bool r = (map.at(i, j) && !tmp.at(i, j)); changed |= r ^ map.at(i, j); map.at(i, j) = r; }
#if 0
    static int cnt = 0;
    ImageRGB img(_array.xsize(), _array.ysize());
    ForImage((&img), i, j) {
      img.pixel(i, j) = map.at(i, j) ? v3b(255) : v3b(0);
    }
    saveImage(sprint("dump\\%04d.thinmap.tga", cnt++), &img);
#endif
  }
  _array = map;
}

// ---------------------------------------------------

void NAMESPACE::Morpho::negate(Array2D<bool>& a)
{
  ForArray2D(a, i, j) {
    a.at(i, j) = !a.at(i, j);
  }
}

// ---------------------------------------------------

void NAMESPACE::Morpho::Union(const Array2D<bool>& a, const Array2D<bool>& b, Array2D<bool>& _result)
{
  ForArray2D(a, i, j) {
    _result.at(i, j) = a.at(i, j) || b.at(i, j);
  }
}

// ---------------------------------------------------

void NAMESPACE::Morpho::Difference(const Array2D<bool>& a, const Array2D<bool>& b, Array2D<bool>& _result)
{
  ForArray2D(a, i, j) {
    _result.at(i, j) = a.at(i, j) && ! b.at(i, j);
  }
}

// ---------------------------------------------------

void NAMESPACE::Morpho::Intersection(const Array2D<bool>& a, const Array2D<bool>& b, Array2D<bool>& _result)
{
  ForArray2D(a, i, j) {
    _result.at(i, j) = a.at(i, j) && b.at(i, j);
  }
}

// ---------------------------------------------------
