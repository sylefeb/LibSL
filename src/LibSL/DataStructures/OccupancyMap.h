// Sylvain Lefebvre - 2008-04-11
#pragma once

// ------------------------------------------------------

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/Memory/Pointer.h>
#include <LibSL/Memory/TraceLeaks.h>
#include <LibSL/Memory/Array.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Math.h>

// ------------------------------------------------------

namespace LibSL {
namespace DataStructures {

template <typename T_Element>
class OccupancyMap
{
protected:

  Array3D<std::set<T_Element> >      m_Map;
  v3f                                m_MapScale;

public:

  typedef T_Element t_Element;

  v3i           minCorner(const v3f& pos)
  {
    v3i cell = 0;
    ForIndex(c,3) {
      cell[c] = int( floor(pos[c]*m_MapScale[c]) );
    }
    uint sx  = m_Map.xsize()-1;
    uint sy  = m_Map.ysize()-1;
    uint sz  = m_Map.zsize()-1;
    return tupleMax(tupleMin(cell,V3I(sx,sy,sz)),V3I(0,0,0));
  }

  v3i           maxCorner(const v3f& pos)
  {
    v3i cell = 0;
    ForIndex(c,3) {
      cell[c] = int( ceil(pos[c]*m_MapScale[c]) );
    }
    uint sx  = m_Map.xsize()-1;
    uint sy  = m_Map.ysize()-1;
    uint sz  = m_Map.zsize()-1;
    return tupleMax(tupleMin(cell,V3I(sx,sy,sz)),V3I(0,0,0));
  }

  v3i           pos2Map(const v3f& pos)
  {
    v3i cell = 0;
    ForIndex(c,3) {
      cell[c] = int( round(pos[c]*m_MapScale[c]) );
    }
    uint sx  = m_Map.xsize()-1;
    uint sy  = m_Map.ysize()-1;
    uint sz  = m_Map.zsize()-1;
    return tupleMax(tupleMin(cell,V3I(sx,sy,sz)),V3I(0,0,0));
  }

  v3f           map2Pos(const v3i& m)
  {
    return ( v3f(m) + v3f(0.5f) ) / m_MapScale;
  }

  Tuple<v3i,2>  occupancyBox(const v3f& pos,float radius)
  {
    v3i bmin = minCorner(pos - v3f(1.0f)*radius);
    v3i bmax = maxCorner(pos + v3f(1.0f)*radius);
    return Pair(bmin,bmax);
  }

public:

  OccupancyMap(const v3i& size,const v3f& scale)
  {
    m_MapScale = scale;
    m_Map.allocate(size[0],size[1],size[2]);
  }

  void          insert(const v3f& pos,float radius,const T_Element& key)
  {
    Tuple<v3i,2> minmax = occupancyBox(pos,radius);
    ForRange(x,minmax[0][0],minmax[1][0]) {
      ForRange(y,minmax[0][1],minmax[1][1]) {
        ForRange(z,minmax[0][2],minmax[1][2]) {
          m_Map.at(x,y,z).insert(key);
        }
      }
    }
  }

  void          remove(const v3f& pos,float radius,const T_Element& key)
  {
    Tuple<v3i,2> minmax = occupancyBox(pos,radius);
    ForRange(x,minmax[0][0],minmax[1][0]) {
      ForRange(y,minmax[0][1],minmax[1][1]) {
        ForRange(z,minmax[0][2],minmax[1][2]) {
          m_Map.at(x,y,z).erase(key);
        }
      }
    }
  }

  //! Get all items within radius around pos.
  //! Does *not* clear set
  void          getProximitySet(
    const v3f&           pos,
    float                radius,
    std::set<T_Element>& _set)
  {
    Tuple<v3i,2> minmax = occupancyBox(pos,radius);
    //std::cerr << "[PROXIM] " << minmax << ' ';
    ForRange(x,minmax[0][0],minmax[1][0]) {
      ForRange(y,minmax[0][1],minmax[1][1]) {
        ForRange(z,minmax[0][2],minmax[1][2]) {
          //std::cerr << sprint(" [%d,%d,%d] ",x,y,z);
          ForIterator_typename(std::set<T_Element>,m_Map.at(x,y,z),I) {
            //std::cerr << (*I) << ' ';
            _set.insert((*I));
          }
        }
      }
    }
    //std::cerr << std::endl;
  }

  v3f                                  scale() const { return m_MapScale; }
  const Array3D<std::set<T_Element> >& map()   const { return (m_Map); }
};


// ------------------------------------------------------

} // DataStructures
} // LibSL

// ------------------------------------------------------
