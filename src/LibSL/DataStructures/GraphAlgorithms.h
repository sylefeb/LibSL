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
// LibSL::DataStructures::GraphAlgorithms
// ------------------------------------------------------
//
// Basic graph algorithms
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2009-02-10
// ------------------------------------------------------

#pragma once

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/Memory/Pointer.h>
#include <LibSL/Memory/TraceLeaks.h>
#include <LibSL/Memory/Array.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Math.h>

#include <LibSL/DataStructures/Graph.h>

namespace LibSL {
  namespace DataStructures {
    namespace GraphAlgorithms {

      using namespace Math;
      using namespace Memory::Array;

      ///// Default functors

      //! Default edge cost evaluator
      template < class T_Graph >
      class DefaultEdgeCost
      {
      public:
        float operator()(const T_Graph& graph,t_NodeId current,t_EdgeId edge) const { return 1.0f; }
      };

      //! Default edge tester
      template < class T_Graph >
      class DefaultEdgeTester
      {
      public:
        bool operator()(const T_Graph& graph,t_NodeId current,t_EdgeId edge) const { return (true); }
      };

      //! Default node tester
      template < class T_Graph >
      class DefaultNodeTester
      {
      public:
        bool operator()(const T_Graph& graph,t_NodeId node) const { return (true); }
      };

      //! A node record for shortest path computation
      struct ShortestPathNodeRecord
      {
        t_NodeId  m_Node;
        float     m_Dist;
        ShortestPathNodeRecord(t_NodeId node, float dist) : m_Node(node), m_Dist(dist) { }
        bool operator < (const ShortestPathNodeRecord& o) const { return (m_Dist > o.m_Dist); }
      };

      //! Default stop condition
      template <class T_Graph>
      class DefaultStopCondition
      {
      public:
        DefaultStopCondition() {}
        bool forceFirstStep() const { return false; }
        bool operator()(const T_Graph& graph,const ShortestPathNodeRecord& node) const { return ( false ); }
      };

      //! Stop condition to reach a given node
      template <class T_Graph>
      class TargetStopCondition
      {
      private:
        t_NodeId m_Target;
      public:
        TargetStopCondition(t_NodeId target) : m_Target(target) {}
        bool forceFirstStep() const { return false; }
        bool operator()(const T_Graph& graph,const ShortestPathNodeRecord& node) const { return ( node.m_Node == m_Target ); }
      };


      ///// Algorithms

      template< class T_Graph,class T_EdgeTester = DefaultEdgeTester<T_Graph>,class T_NodeTester = DefaultNodeTester<T_Graph> >
      class ConnectedComponents
      {
      public:
        //! Find the connected component including a given node - with edge and node tester
        void findConnectedComponent(
          const T_Graph&                                 g,
          t_NodeId                                       node,
          std::vector<t_NodeId>&                        _component,
          const T_EdgeTester&                            follow = T_EdgeTester(),
          const T_NodeTester&                            accept = T_NodeTester())
        {
          LibSL::Memory::Array::Array<bool> visited(g.nodes().size());
          visited   .fill(false);
          _component.clear();
          findConnectedComponentContaining(g,node,visited,_component,follow,accept);
        }

        //! Enumerate all connected components - with edge and node tester.
        //! Returns false when no more components
        bool enumerateConnectedComponents(
          const T_Graph&                                 g,
          LibSL::Memory::Array::Array<bool>&            _visited,
          std::vector<t_NodeId>&                        _component,
          const T_EdgeTester&                            follow = T_EdgeTester(),
          const T_NodeTester&                            accept = T_NodeTester())
        {
          if (g.nodes().size() == 0) {
            return (false);
          }
          if (_visited.empty()) {
            _visited.erase();
            _visited.allocate((int)g.nodes().size());
            _visited.fill(false);
          }
          _component.clear();
          // find first non visited node
          uint next = 0;
          while (_visited[next] || !accept(g,next)) {
            next ++;
            if (next == _visited.size()) {
              return (false);
            }
          };
          findConnectedComponentContaining(g,next,_visited,_component,follow,accept);
          return (true);
        }

        //! Find the connected component including a given node - with edge and node tester
        void findConnectedComponentContaining(
          const T_Graph&                                 g,
          t_NodeId                                       start,
          LibSL::Memory::Array::Array<bool>&            _visited,
          std::vector<t_NodeId>&                        _component,
          const T_EdgeTester&                            follow = T_EdgeTester(),
          const T_NodeTester&                            accept = T_NodeTester())
        {
          if (_visited[start]) {
            return;
          }
          // init queue
          std::queue<t_NodeId> Q;
          Q.push(start);
          // flag as visited
          _visited[start] = true;
          while (!Q.empty()) {
            // take from queue
            t_NodeId current = Q.front();
            Q.pop();
            // add to component
            _component.push_back(current);
            // add neighbors
            const typename T_Graph::NodeContainer& nc   = g.nodes()[current];
            ForIndex(n,nc.edgeIds().size()) {
              const typename T_Graph::EdgeContainer& ec = g.edges()[nc.edgeIds()[n]];
              t_NodeId other    = ec.to();
              if (!T_Graph::e_IsOriented) {
                other = ec.other(current);
              }
              if (!_visited[other]
              &&   follow(g,current,nc.edgeIds()[n])
                &&   accept(g,other)) {
                  // add to queue
                  Q.push(other);
                  // flag as visited
                  _visited[other] = true;
              }
            } // edges
          } // Q
        }

      };

      // -------------------------------------
      /// Shortest path

      template <class T_Graph,
      class T_EdgeCost      = DefaultEdgeCost<T_Graph>,
      class T_EdgeTester    = DefaultEdgeTester<T_Graph>,
      class T_NodeTester    = DefaultNodeTester<T_Graph>
      >
      class ShortestPaths
      {
      public:

        //! Find shortest path from one node to all others
        //!   fills _dist and _prev with path min cost and path information (prev[i][0] is the node from which to reach i)
        //!   In prev[i], [0] contains the node id; [1] the edge to the node, -1 for the first
        //!   Returns -1 if the stop condition was not reached, the node id otherwise
        template< class T_StopCondition >
        t_NodeId dijkstra(
          const T_Graph&                                 g,
          t_NodeId                                       source,
          Array<float>&                                 _dist,
          Array<v2i  >&                                 _prev,
          const T_StopCondition&                         stop   = T_StopCondition(),
          const T_EdgeCost&                              cost   = T_EdgeCost(),
          const T_EdgeTester&                            follow = T_EdgeTester(),
          const T_NodeTester&                            accept = T_NodeTester()
          )
        {
          _dist.allocate( (uint)g.nodes().size() );
          _prev.allocate( (uint)g.nodes().size() );
          _dist.fill(Infinity);
          _prev.fill(V2I(-1,-1));
          return dijkstraUpdate(g,source,_dist,_prev,stop,cost,follow,accept);
        }

        //! Find shortest path from one node to all others
        //!   fills _dist and _prev with path min cost and path information (prev[i][0] is the node from which to reach i)
        //!   In prev[i], [0] contains the node id; [1] the edge to the node, -1 for the first
        //!   Returns -1 if the stop condition was not reached, the node id otherwise
        template< class T_StopCondition >
        t_NodeId dijkstraUpdate(
          const T_Graph&                                 g,
          t_NodeId                                       source,
          Array<float>&                                 _dist,
          Array<v2i  >&                                 _prev,
          const T_StopCondition&                         stop   = T_StopCondition(),
          const T_EdgeCost&                              cost   = T_EdgeCost(),
          const T_EdgeTester&                            follow = T_EdgeTester(),
          const T_NodeTester&                            accept = T_NodeTester()
          )
        {
          std::priority_queue<ShortestPathNodeRecord> heap;
          // push source on heap
          if ( ! stop.forceFirstStep() ) {
            _dist[source] = 0;
          }
          heap.push(ShortestPathNodeRecord(source,0));
          // dijkstra
          LIBSL_BEGIN;
          bool firstStep = true;
          while ( ! heap.empty() ) {
            // extract min
            ShortestPathNodeRecord current = heap.top();
            heap.pop();
            // consider nodes only once
            if (current.m_Dist <= _dist[current.m_Node]) {
              if ( ! stop.forceFirstStep() || ! firstStep ) {
                // check if destination reached
                if ( stop( g, current ) ) {
                  return current.m_Node;
                }
                // update dist
                _dist[current.m_Node] = current.m_Dist;
              }
              firstStep = false;
              // for each neighbor of current
              t_NodeId                     u         = current.m_Node;
              const std::vector<t_EdgeId>& edges_out = g.nodes()[u].edgeIds();
              ForIndex(e,edges_out.size()) {
                sl_assert( ! T_Graph::e_IsOriented || g.edges()[edges_out[e]].from() == u);
                t_NodeId to  = g.edges()[edges_out[e]].other(u);
                if (follow(g,u,edges_out[e]) && accept(g,to)) {
                  // new distance
                  float newd = current.m_Dist + cost(g,u,edges_out[e]);
                  if (newd < _dist[to]) {
                    _dist[to]    = newd;
                    _prev[to][0] = u;
                    _prev[to][1] = edges_out[e];
                    heap.push(ShortestPathNodeRecord(to,newd));
                  }
                }
              }
            }
          }
          return -1;
          LIBSL_END;
          return -1;
        }

        // Dijkstra without stop condition
        void dijkstraToAll(
          const T_Graph&                                 g,
          t_NodeId                                       source,
          Array<float>&                                 _dist,
          Array<v2i  >&                                 _prev,
          const T_EdgeCost&                              cost   = T_EdgeCost(),
          const T_EdgeTester&                            follow = T_EdgeTester(),
          const T_NodeTester&                            accept = T_NodeTester()
          )
        {
          DefaultStopCondition<T_Graph> stop;
          dijkstra(g,source,_dist,_prev,stop,cost,follow,accept);
        }

        //! Find shortest path between two nodes
        //!   returns path cost
        //!   _path is filled with the path. [0] contains the node id; [1] the edge to the node, -1 for the first
        template< class T_StopCondition >
        float dijkstraToTarget(
          const T_Graph&                                 g,
          t_NodeId                                       source,
          t_NodeId                                       target,
          std::vector<LibSL::Math::v2i>&                _path,
          const T_EdgeCost&                              cost   = T_EdgeCost(),
          const T_EdgeTester&                            follow = T_EdgeTester(),
          const T_NodeTester&                            accept = T_NodeTester()
          )
        {
          LIBSL_BEGIN;
          Array<float>     dist;
          Array<v2i  >     prev;
          T_StopCondition stop( target );
          int check = dijkstra< T_StopCondition >(g,source,dist,prev,stop,cost,follow,accept);
          _path.clear();
          // does a path exist?
          if (prev[target][0] == -1) {
            if ( target == source ) {
              _path.push_back(V2I(source,-1));
              return (dist[target]);
            } else {
              // no path!
              sl_assert( check == -1 );
              return -1;
            }
          }
          //sl_assert( found );
          // backtrack path
          sl_assert( check == target );
          t_NodeId u = target;
          do {
            _path.push_back(V2I(u,prev[u][1]));
            u = prev[u][0];
          } while (u != source);
          _path.push_back(V2I(source,-1));
          std::reverse(_path.begin(),_path.end());
          return (dist[target]);
          LIBSL_END;
        }

      }; // ShortestPaths

    } //namespace LibSL::DataStructures::GraphAlgorithms
  } //namespace LibSL::DataStructures
} //namespace LibSL

// ------------------------------------------------------
