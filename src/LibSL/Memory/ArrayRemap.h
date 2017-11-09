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
// LibSL::Memory::Array::ArrayRemap
// ------------------------------------------------------
//
// Utility class to remap raw buffer into an array-like interface
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2014-??-??
// ------------------------------------------------------

#pragma once

#include <LibSL/Errors/Errors.h>
#include <LibSL/Memory/Array.h>

// ------------------------------------------------------

namespace LibSL  {
  namespace Memory {
    namespace Array {

      class ArrayRemap
      {
      private:
        const  uchar   *m_Ptr;
        int             m_Size;
        int             m_VWidth;
        Array<uchar>    m_BorderValue;
      public:
        int              size() const { return m_Size; }
        const uchar     *at(int i) const;
        const uchar     *operator[](int i) const;
        ArrayRemap(const uchar *ptr, int size, int vwidth = 1, uchar brdrval = 255u);
      };

      class Array2DRemap
      {
      private:
        const  uchar *m_Ptr;
        int           m_XSize;
        int           m_YSize;
        int           m_VWidth;
        int           m_VStride;
        Array<uchar>  m_BorderValue;
        bool          m_Transpose;
      public:
        int    xsize()       const  { return m_XSize;  }
        int    ysize()       const  { return m_YSize;  }
        int    vwidth()      const  { return m_VWidth; }
        int    vstride()     const  { return m_VStride; }
        const uchar *borderValue() const  { return m_BorderValue.raw(); }
        const uchar *at(int i, int j) const;
        Array2DRemap(const uchar *ptr, int xsize, int ysize, int vwidth = 1, int vstride = 0, uchar brdrval = 255u,bool transpose = false);
      };

    }
  }
}

// ------------------------------------------------------
