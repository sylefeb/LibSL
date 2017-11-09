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
// ----------------------------------------------
#pragma once
// ----------------------------------------------

#include <loki/TypeManip.h>
#include <LibSL/Mesh/VertexFormat.h>

// ----------------------------------------------

namespace LibSL {
namespace GPUMesh {

// ----------------------------------------------
// Associates a value to mvf item for D3D
// ----------------------------------------------

template <class T> struct MVF_D3D_item_keys;
template <> struct MVF_D3D_item_keys<Loki::NullType>                  { enum {value = 999}; };
template <> struct MVF_D3D_item_keys<LibSL::Mesh::MVF_BASE_POSITION>  { enum {value =   0}; };
template <> struct MVF_D3D_item_keys<LibSL::Mesh::MVF_BASE_NORMAL>    { enum {value =   1}; };
template <> struct MVF_D3D_item_keys<LibSL::Mesh::MVF_BASE_COLOR0>    { enum {value =   2}; };
template <> struct MVF_D3D_item_keys<LibSL::Mesh::MVF_BASE_COLOR1>    { enum {value =   3}; };
template <> struct MVF_D3D_item_keys<LibSL::Mesh::MVF_BASE_TEXCOORD0> { enum {value =   4}; };
template <> struct MVF_D3D_item_keys<LibSL::Mesh::MVF_BASE_TEXCOORD1> { enum {value =   5}; };
template <> struct MVF_D3D_item_keys<LibSL::Mesh::MVF_BASE_TEXCOORD2> { enum {value =   6}; };
template <> struct MVF_D3D_item_keys<LibSL::Mesh::MVF_BASE_TEXCOORD3> { enum {value =   7}; };
template <> struct MVF_D3D_item_keys<LibSL::Mesh::MVF_BASE_TEXCOORD4> { enum {value =   8}; };
template <> struct MVF_D3D_item_keys<LibSL::Mesh::MVF_BASE_TEXCOORD5> { enum {value =   9}; };
template <> struct MVF_D3D_item_keys<LibSL::Mesh::MVF_BASE_TEXCOORD6> { enum {value =  10}; };
template <> struct MVF_D3D_item_keys<LibSL::Mesh::MVF_BASE_TEXCOORD7> { enum {value =  11}; };
template <class T> struct MVF_D3D_item_keys          
{ 
private:
  enum {temp = MVF_D3D_item_keys<T::base_attribute>::value};
public:
  enum {value = temp};
};

// ----------------------------------------------
// Find min item
// ----------------------------------------------

template <class VertexFormat> class MVF_D3D_find_min;

template <> class MVF_D3D_find_min<Loki::NullType>
{
public:
  typedef Loki::NullType Result;
};

template <class VertexFormat> class MVF_D3D_find_min
{
private:

  ASSERT_TYPELIST(VertexFormat);

  typedef typename VertexFormat::Tail Tail;
  typedef typename VertexFormat::Head Head;

  typedef typename MVF_D3D_find_min<Tail>::Result next_min;

public:
  enum {current = MVF_D3D_item_keys<Head>::value};
  enum {next    = MVF_D3D_item_keys<next_min>::value};
  typedef typename Loki::Select<
                                (current < next),
                                Head,
                                next_min>::Result
                   Result;
};

// ----------------------------------------------
// Sorts vertex format to correspond to D3D FVF ordering
// ----------------------------------------------

template <class VertexFormat> class MVF_D3D_sort_items;

template <> class MVF_D3D_sort_items<Loki::NullType>
{
public:
  typedef Loki::NullType Result;
};

template <class VertexFormat> class MVF_D3D_sort_items
{
private:

  ASSERT_TYPELIST(VertexFormat);

  typedef typename VertexFormat::Head Head;
  typedef typename VertexFormat::Tail Tail;

  typedef typename MVF_D3D_find_min<VertexFormat>::Result        min;
  typedef typename Loki::TL::Replace<Tail,min,Head>::Result      next;

public:

  typedef typename Loki::Typelist< min , typename MVF_D3D_sort_items<next>::Result > Result;

};

// ----------------------------------------------
} // namespace GPUMesh
} // namespace LibSL

// ----------------------------------------------
