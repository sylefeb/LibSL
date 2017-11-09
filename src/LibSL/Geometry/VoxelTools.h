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
// LibSL::Geometry::VoxelTools
// ------------------------------------------------------
//
// Tools to manipulate voxels:
//   - find neighboring voxels
//   - create a graph from voxels
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-11-15
// ------------------------------------------------------

#pragma once

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/StlHelpers/StlHelpers.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Math/Math.h>

#include <vector>
#include <map>
#include <set>

namespace LibSL {
  namespace Geometry {
    namespace VoxelTools {

      typedef Math::v3s v3s;
      typedef Math::v3i v3i;

      template<class T_Voxelizer>
      void gatherNeighborsBelow_rec(
        typename T_Voxelizer::t_Voxelization& voxels, v3s pos, uint d, v3s face,
        std::set<std::pair<uint,v3s> >&      _neighs)
      {
        // find out which coord in face is 1 or -1
        uint face_id;
        int  face_dir;
        ForIndex(i,3) {
          if (face[i] != 0) {
            face_id  = i;
            face_dir = face[i];
            break;
          }
        }
        // for all levels below
        ForRange(current_d,d+1,voxels.size()-1) {
          // generate all coordinates of children along face at next depth
          if (voxels.empty()) {
            continue;
          }
          ForTuple(2,cloc2d,v3s(1 << (current_d - d))) { // FIXME enumerates way too many
            v3s  cloc = 0;
            uint r    = 0;
            ForIndex(i,3) {
              if (i == (int) face_id) {
                cloc[i] = 0;
              } else {
                cloc[i] = cloc2d[r++];
              }
            }
            sl_assert(r == 2);
            // generate child coord
            v3s cpos = (pos + face) * short(1 << (current_d - d)) + v3s(cloc);
            if (face_dir < 0) {
              cpos[face_id] += short(1 << (current_d - d)) - 1;
            }
            // check presence
            typename T_Voxelizer::t_VoxelMap::const_iterator F = voxels[current_d].find(cpos);
            if (F != voxels[current_d].end() )
              _neighs.insert(std::make_pair(current_d,cpos));
          }
        }
      }

      // --------

      template<class T_Voxelizer>
      void gatherNeighborsBelow(
        typename T_Voxelizer::t_Voxelization& voxels,v3s pos, uint d,
        std::set<std::pair<uint,v3s> >&    _neighs)
      {
        _neighs.clear();
        gatherNeighborsBelow_rec<T_Voxelizer>(voxels,pos,d,Math::V3S( 1,0,0),_neighs);
        gatherNeighborsBelow_rec<T_Voxelizer>(voxels,pos,d,Math::V3S(-1,0,0),_neighs);
        gatherNeighborsBelow_rec<T_Voxelizer>(voxels,pos,d,Math::V3S(0, 1,0),_neighs);
        gatherNeighborsBelow_rec<T_Voxelizer>(voxels,pos,d,Math::V3S(0,-1,0),_neighs);
        gatherNeighborsBelow_rec<T_Voxelizer>(voxels,pos,d,Math::V3S(0,0, 1),_neighs);
        gatherNeighborsBelow_rec<T_Voxelizer>(voxels,pos,d,Math::V3S(0,0,-1),_neighs);
      }

      // -------

      template<class T_Voxelizer>
      void gatherAllNeighbors(
        typename T_Voxelizer::t_Voxelization& voxels,v3s pos, uint d,
        std::set<std::pair<uint,v3s> >&      _neighs)
      {
        _neighs.clear();
        // neighbors at same level or above
        ForTupleRange(3,offs,v3i(-1),v3i(1)) {
          // check for neighbors presence
          v3s npos = pos + v3s(offs);
          ForRangeReverse(r,d,0) {
            if (voxels[r].empty()) {
              break;
            }
            v3s rpos = npos / short(1<<(d-r));
            typename T_Voxelizer::t_VoxelMap::const_iterator F = voxels[r].find(rpos);
            if (F != voxels[r].end()) {
              // present - add
              _neighs.insert(std::make_pair(r,(*F).first));
            } else {
              // not present, do nothing
            }
          }
        } // tuple neighbors

        // neighbors below
        gatherNeighborsBelow_rec<T_Voxelizer>(voxels,pos,d,Math::V3S( 1,0,0),_neighs);
        gatherNeighborsBelow_rec<T_Voxelizer>(voxels,pos,d,Math::V3S(-1,0,0),_neighs);
        gatherNeighborsBelow_rec<T_Voxelizer>(voxels,pos,d,Math::V3S(0, 1,0),_neighs);
        gatherNeighborsBelow_rec<T_Voxelizer>(voxels,pos,d,Math::V3S(0,-1,0),_neighs);
        gatherNeighborsBelow_rec<T_Voxelizer>(voxels,pos,d,Math::V3S(0,0, 1),_neighs);
        gatherNeighborsBelow_rec<T_Voxelizer>(voxels,pos,d,Math::V3S(0,0,-1),_neighs);
      }

      // -------

      template<typename T_Voxelizer>
      class AcceptLink
      {
      public:
        bool operator()(typename T_Voxelizer::t_Voxelization& voxels,const std::pair<uint,v3s>& v0,const std::pair<uint,v3s>& v1) const { return (true); }
      };

      // -------

      template<typename T_Voxelizer,typename T_Graph>
      void linkVoxels(
        typename T_Voxelizer::t_Voxelization& voxels,
        std::vector<std::map<v3s,uint> >&     voxelIds,
        const std::vector<v3i>&               offsets,
        T_Graph&                             _graph,
        const AcceptLink<T_Voxelizer>&        linkTester)
      {
        ForIndex( d, voxels.size() ) { // depth
          ForConstIterator_typename(T_Voxelizer::t_VoxelMap,voxels[d],V) { // voxel
            // link nodes to neighbors at this level
            ForArray(offsets,o) {
              v3i offs = offsets[o];
              // check for neighbors presence
              v3s npos = (*V).first + v3s(offs);
              typename T_Voxelizer::t_VoxelMap::const_iterator F = voxels[d].find(npos);
              if (F != voxels[d].end()) {
                // present - add edge
                if (linkTester(voxels, std::make_pair(d,(*V).first) , std::make_pair(d,(*F).first) )) {
                  typename T_Graph::t_EdgeData enfo;
                  _graph.addEdge(voxelIds[d][(*V).first],voxelIds[d][F->first],enfo);
                }
              } else {
                // not present, do nothing
              }
            } // tuple neighbors

            // gather neighbors for this voxel
            typedef std::set<std::pair<uint,v3s> > t_NeighborSet;
            t_NeighborSet neighbors;
            gatherNeighborsBelow<T_Voxelizer>(voxels, (*V).first, d,  neighbors);
            // add edge to neighbors
            ForIterator_typename(t_NeighborSet,neighbors,N){
              if (linkTester(voxels, std::make_pair(d,(*V).first) , (*N) )) {
                typename T_Graph::t_EdgeData enfo;
                _graph.addEdge(voxelIds[d][(*V).first],voxelIds[(*N).first][(*N).second],enfo);
              }
            }
          } // voxel
        } // depth
      }

      // -------

      template<typename T_Voxelizer,typename T_Graph>
      void createGraphFromVoxels(
        typename T_Voxelizer::t_Voxelization&  voxels,
        T_Graph&                              _graph,
        std::vector<std::map<v3s,uint> >&     _voxelIds,
        bool                                   diagonalLink = false,
        const AcceptLink<T_Voxelizer>&         linkTester   = AcceptLink<T_Voxelizer>())
      {
        _graph.clear();

        // for each depth
        _voxelIds.resize(voxels.size());
        ForIndex(d, voxels.size()) {
          // add nodes to graph
          ForIterator_typename(T_Voxelizer::t_VoxelMap,voxels[d],V) {
            typename T_Graph::t_NodeData nnfo(V);
            v3s vpos           = (*V).first;
            uint id            = _graph.addNode(nnfo);
            _voxelIds[d][vpos] = id;
          }

        } //for depth

        std::vector<v3i> offsets;
        if (diagonalLink) {
          ForTupleRange(3,offs,v3i(-1),v3i(1)) {
            if (dot(offs,v3i(1)) < 0 || offs == v3i(0)) {
              continue;
            }
            offsets.push_back(v3i(offs));
          }
        } else {
          ForTuple(3,offs,v3i(2)) {
            // use 6-connectivity
            if ( abs( dot(offs,offs) ) != 1 ) {
              continue;
            }
            offsets.push_back(v3i(offs));
          }
        }

        LIBSL_BEGIN;
        linkVoxels(voxels,_voxelIds,offsets,_graph,linkTester);
        LIBSL_END;

      }

    } //namespace LibSL::Geometry::VoxelTools
  } //namespace LibSL::Geometry
} //namespace LibSL
