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
// LibSL::DataStructures::Graph
// ------------------------------------------------------
//
// Basic graph class
//
// TODO: separate algorithms from data structure
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2007-06-15
// ------------------------------------------------------

#pragma once

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/Memory/Pointer.h>
#include <LibSL/Memory/TraceLeaks.h>
#include <LibSL/Memory/Array.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Math.h>

#include <loki/SmallObj.h>

#include <list>
#include <vector>
#include <set>
#include <queue>

namespace LibSL {
  namespace DataStructures {

    typedef int   t_EdgeId;
    typedef int   t_NodeId;
    const   float Infinity = 1e20f;


    /*!

    \class Graph
    \brief Generic class for graphs. See namespace GraphAlgorithms for simple graph manipulations.

    */
    template <typename T_NodeData,typename T_EdgeData,bool T_Oriented>
    class Graph
    {
    public:

      typedef LibSL::Memory::Pointer::AutoPtr<Graph> t_Pointer;
      typedef LibSL::Math::v2i                       v2i;

      //! Edge container class
      class EdgeContainer
      {
      private:
        T_EdgeData m_Data;
        t_NodeId   m_From;
        t_NodeId   m_To;
        bool       m_Valid;
      public:

        EdgeContainer()
        { m_From = m_To = -1; m_Valid = false; }
        EdgeContainer(uint from,uint to,const T_EdgeData& d)
        { m_Data = d; m_From = from; m_To = to; m_Valid = true; }
        EdgeContainer(const EdgeContainer& ec)
        {
          m_Data  = ec.m_Data;
          m_From  = ec.m_From;
          m_To    = ec.m_To;
          m_Valid = ec.m_Valid;
        }

        const T_EdgeData& data() const {sl_assert(m_Valid); return (m_Data);}
        T_EdgeData&       data()       {sl_assert(m_Valid); return (m_Data);}

        t_NodeId          from()  const {sl_assert(m_Valid); return (m_From);}
        t_NodeId          to()    const {sl_assert(m_Valid); return (m_To);  }
        t_NodeId          nodeA() const {sl_assert(m_Valid); return (m_From);}
        t_NodeId          nodeB() const {sl_assert(m_Valid); return (m_To);  }

        t_NodeId          other(t_NodeId node) const
        {
          sl_assert(m_Valid);
          if      (node == m_To)   { return (m_From); }
          else if (node == m_From) { return (m_To);   }
          else                     { sl_assert(false); return -1; }
          // ... ignore warning of non returning path
        }

        bool isValid() const {return (m_Valid);}
      };

      //! Node container class
      class NodeContainer
      {
      private:
        T_NodeData            m_Data;
        std::vector<t_EdgeId> m_EdgeIds;
        bool                  m_Valid;
      public:

        NodeContainer()
        { m_Valid = false; }
        NodeContainer(const T_NodeData& d)
        { m_Data = d; m_Valid = true; }
        NodeContainer(const NodeContainer& nc)
        {
          m_Data    = nc.m_Data;
          m_EdgeIds = nc.m_EdgeIds;
          m_Valid   = nc.m_Valid;
        }

        void removeEdge(t_EdgeId e)
        {
          sl_assert(m_Valid);
          uint r = 0;
          for (;r<m_EdgeIds.size();r++) {
            if (m_EdgeIds[r] == e) {
              break;
            }
          }
          sl_assert(r < m_EdgeIds.size()); // edge not found: error!
          m_EdgeIds[r] = m_EdgeIds[m_EdgeIds.size()-1];
          m_EdgeIds.pop_back();
        }

        const T_NodeData& data() const {sl_assert(m_Valid); return (m_Data);}
        T_NodeData&       data()       {sl_assert(m_Valid); return (m_Data);}

        const std::vector<t_EdgeId>& edgeIds() const {sl_assert(m_Valid); return (m_EdgeIds);}
        std::vector<t_EdgeId>&       edgeIds()       {sl_assert(m_Valid); return (m_EdgeIds);}

        bool isValid() const {return (m_Valid);}
      };

    public:

      typedef T_NodeData t_NodeData;
      typedef T_EdgeData t_EdgeData;
      enum { e_IsOriented = T_Oriented ? 1 : 0 };

    private:

      std::vector<NodeContainer> m_Nodes;
      std::vector<EdgeContainer> m_Edges;

    public:

      Graph()
      {  }

      ~Graph()
      {  }

      //! Clear the graph
      void clear()
      {
        m_Nodes.clear();
        m_Edges.clear();
      }

      LIBSL_DISABLE_COPY(Graph);

      //! Add a node, return new node id
      t_NodeId addNode(const T_NodeData& d)
      {
        m_Nodes.push_back(NodeContainer(d));
        return t_NodeId(m_Nodes.size()-1);
      }

      //! Add an (oriented) edge, return new edge id
      t_EdgeId addEdge(t_NodeId from,t_NodeId to,const T_EdgeData& d)
      {
        sl_assert(size_t(from) < (int)m_Nodes.size());
        sl_assert(size_t(to  ) < (int)m_Nodes.size());

        m_Edges                .push_back(EdgeContainer(from,to,d));
        m_Nodes[from].edgeIds().push_back(int(m_Edges.size())-1);
        if ( ! T_Oriented ) {
          m_Nodes[to].edgeIds().push_back(int(m_Edges.size())-1);
        }
        return t_EdgeId(m_Edges.size()-1);
      }

      //! Remove the edge: all reference to the edge are removed from the graph
      //! the edge table is not changed to avoid global renumbering
      //! In the special case the edge is the last one, the table is resized
      void removeEdge(uint e)
      {
        if (T_Oriented) {
          t_NodeId from = m_Edges[e].from();
          m_Nodes[from].removeEdge(e);
        } else {
          t_NodeId a = m_Edges[e].nodeA();
          t_NodeId b = m_Edges[e].nodeB();
          m_Nodes[a].removeEdge(e);
          m_Nodes[b].removeEdge(e);
        }
        if (size_t(e) == m_Edges.size()-1) {
          m_Edges.pop_back();
        } else {
          m_Edges[e] = EdgeContainer();
        }
      }

      //! Remove the node: all reference to the node are removed from the graph
      //! the node/edge tables are not changed to avoid global renumbering
      //! all edges to/from the node are removed as well
      //! In the special case the node is the last one, the table is resized
      void removeNode(t_NodeId node)
      {
        sl_assert(size_t(node) < m_Nodes.size());
        // go through all edges and flag for removal
        std::vector<t_EdgeId> to_remove;
        ForIndex(e,m_Edges.size()) {
          if (!m_Edges[e].isValid()) {
            continue;
          }
          if (m_Edges[e].to() == node || m_Edges[e].from() == node) {
            to_remove.push_back(e);
          }
        }
        // remove edges
        ForIndex(r,to_remove.size()) {
          removeEdge(to_remove[r]);
        }
        // erase node
        if (node == m_Nodes.size()-1) {
          m_Nodes.pop_back();
        } else {
          m_Nodes[node] = NodeContainer();
        }
        // try to clean up tables (triming the end is easy)
        trim();
      }

      //! Remove all invalid nodes and edges at the end of tables
      void trim()
      {
        if (!m_Edges.empty()) {
          while (!m_Edges.back().isValid()) {
            m_Edges.pop_back();
            if (m_Edges.empty()) break;
          }
        }
        if (!m_Nodes.empty()) {
          while (!m_Nodes.back().isValid()) {
            m_Nodes.pop_back();
            if (m_Nodes.empty()) break;
          }
        }
      }

      //! Search for a specific edge between two nodes
      EdgeContainer *findEdge(t_NodeId from,t_NodeId to)
      {
        sl_assert(from < (int)m_Nodes.size());
        sl_assert(to   < (int)m_Nodes.size());

        NodeContainer& nc = m_Nodes[from];
        ForIndex(e,nc.edgeIds().size()) {
          EdgeContainer *ec = &(m_Edges[nc.edgeIds()[e]]);
          if (m_Edges[nc.edgeIds()[e]].to() == to) {
            return (ec);
          }
          if (!T_Oriented) {
            if (m_Edges[nc.edgeIds()[e]].from() == to) {
              return (ec);
            }
          }
        }
        return (NULL);
      }

      //! Search for a specific edge between two nodes
      t_EdgeId findEdgeId(t_NodeId from,t_NodeId to)
      {
        sl_assert(from < (int)m_Nodes.size());
        sl_assert(to   < (int)m_Nodes.size());

        NodeContainer& nc = m_Nodes[from];
        ForIndex(e,nc.edgeIds().size()) {
          EdgeContainer *ec = &(m_Edges[nc.edgeIds()[e]]);
          if (m_Edges[nc.edgeIds()[e]].to() == to) {
            return (nc.edgeIds()[e]);
          }
          if (!T_Oriented) {
            if (m_Edges[nc.edgeIds()[e]].from() == to) {
              return (nc.edgeIds()[e]);
            }
          }
        }
        return (-1);
      }

      const std::vector<NodeContainer>& nodes() const {return (m_Nodes);}
      std::vector<NodeContainer>&       nodes()       {return (m_Nodes);}
      const std::vector<EdgeContainer>& edges() const {return (m_Edges);}
      std::vector<EdgeContainer>&       edges()       {return (m_Edges);}

    };

  } //namespace LibSL::DataStructures
} //namespace LibSL

// ------------------------------------------------------



