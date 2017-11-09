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

#include "ArrayRemap.h"

using namespace std;

// ------------------------------------------------------

#define NAMESPACE LibSL::Memory::Array

// ------------------------------------------------------

NAMESPACE::ArrayRemap::ArrayRemap(const uchar *ptr,int size,int vwidth,uchar brdrval)
{
  m_Ptr        = ptr;
  m_Size       = size;
  m_VWidth     = vwidth;
  m_BorderValue.allocate( vwidth );
  m_BorderValue.fill( brdrval );
}

// ------------------------------------------------------

const uchar *NAMESPACE::ArrayRemap::at(int i) const
{
  if (i < 0 || i >= m_Size) return m_BorderValue.raw();
  return ( m_Ptr + ((i) * m_VWidth) );
}

// ------------------------------------------------------

const uchar *NAMESPACE::ArrayRemap::operator[](int i) const
{
  return at(i);
}

// ------------------------------------------------------

NAMESPACE::Array2DRemap::Array2DRemap(const uchar *ptr,int xsize,int ysize,int vwidth,int vstride,uchar brdrval,bool transpose)
{
  m_Ptr         = ptr;
  m_XSize       = xsize;
  m_YSize       = ysize;
  m_VWidth      = vwidth;
  m_VStride     = vstride;
  m_BorderValue.allocate( vwidth );
  m_BorderValue.fill( brdrval );
  m_Transpose = transpose;
}

// ------------------------------------------------------

const uchar *NAMESPACE::Array2DRemap::at(int i,int j) const
{
  if (m_Transpose) {
    std::swap(i, j);
  }
  if (i < 0 || i >= m_XSize) return m_BorderValue.raw();
  if (j < 0 || j >= m_YSize) return m_BorderValue.raw();
  return ( m_Ptr + ((i+j*m_XSize) * m_VWidth + m_VStride) );
}

// ------------------------------------------------------
