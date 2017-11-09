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
// LibSL::DataStructures::Hierarchy
// ------------------------------------------------------
//
// Hierarchy classes
//  - Hierarchy, most generic hierarchy class
//  - StaticHierarchy, build from the generic Hierarchy
//    and optimized either for depth-first or breadth-first
//    traversal.
//
// TODO
//  - StaticHierarchy
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-06-14
// ------------------------------------------------------

#pragma once

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/Memory/Pointer.h>
#include <LibSL/Memory/TraceLeaks.h>

#include <list>

namespace LibSL {
  namespace DataStructures {

    /*!

    \class Hierarchy
    \brief Use this class to build a hierarchy or maintain a dynamic hierarchy

    */
    template <typename T_Data>
    class Hierarchy
      : public LibSL::Memory::TraceLeaks::LeakProbe<Hierarchy<T_Data> >
    {
    public:

      typedef LibSL::Memory::Pointer::AutoPtr<Hierarchy> t_AutoPtr;
      typedef LibSL::Memory::Pointer::AutoPtr<Hierarchy> t_Pointer;

      // Children iterator class
      class ChildrenIterator
      {
      private:
        Hierarchy                               *m_Owner;
        typename std::list<t_AutoPtr>::iterator  m_Iterator;
      public:

        ChildrenIterator(Hierarchy *h)
        {
          m_Owner    = h;
          m_Iterator = m_Owner->m_Children.begin();
        }

        LIBSL_DISABLE_COPY(ChildrenIterator);

        //! Returns true if there are no more children in the list
        bool end()
        {
          if (m_Iterator == m_Owner->m_Children.end()) {
            return (true);
          } else {
            return (false);
          }
        }
        //! Go to the next child in the list
        //!   end() must be false
        //!   current() will point to the next children after next() returns
        void next()
        {
          sl_assert(m_Iterator != m_Owner->m_Children.end());
          m_Iterator++;
        }
        //! Retrieve current child
        t_AutoPtr current()
        {
          return (*m_Iterator);
        }
        //! Remove the current child from the hierarchy
        //!    as children are handled by autopointers, the
        //!    child hierarchy will be destroyed if it is no
        //!    longer referenced
        void remove()
        {
          sl_assert(m_Iterator != m_Owner->m_Children.end());
          m_Iterator=m_Owner->m_Children.erase(m_Iterator);
        }
      };

    private:

      T_Data                m_Data;
      std::list<t_AutoPtr>  m_Children;

    public:

      //! Build a leaf hierarchy around some data
      Hierarchy(const T_Data& d)
      {
        m_Data=d;
      }

      ~Hierarchy()
      {

      }

      LIBSL_DISABLE_COPY(Hierarchy);

      //! Retrieve data at root (const access)
      const T_Data& data() const
      {
        return (m_Data);
      }

      //! Retrieve data at root
      T_Data& data()
      {
        return (m_Data);
      }

      //! Add a child to the hierarchy, build around some data
      t_AutoPtr addChild(const T_Data& d)
      {
        t_AutoPtr child=new Hierarchy(d);
        m_Children.push_back(child);
        return (child);
      }

      //! Add a child hierarchy to the hierarchy
      void addChild(t_AutoPtr child)
      {
        m_Children.push_back(child);
      }

      //! Returns the number of children
      uint numChildren() const
      {
        return uint(m_Children.size());
      }

      //! Returns an iterator on children (see also Hierarchy::ChildrenIterator)
      ChildrenIterator children()
      {
        return (ChildrenIterator(this));
      }
    };

    /*!

    \class StaticHierarchy
    \brief Use this class to store a static hierarchy and efficiently traverse it.
    Traversal can be optimized for depth-first or breadth-first.
    The hierarchy cannot be changed once backed into a StaticHierarchy
    */
    /*
    template <typename T_Data>
    class StaticDepthFirstHierarchy
    {
    private:

    class Node
    {
    public:
    int    m_ParentPos;
    T_Data m_Data;
    Node(const T_Data& d,int p) : m_Data(d), m_ParentPos(p) {}
    }

    std::vector<Node> m_Nodes;

    void unfoldHierarchyDepthFirst(Hierarchy<T_Data>::t_AutoPtr h,int parent)
    {
    Node n(h->data(),parent);
    m_Nodes.push_back(n);
    int nodepos = m_Nodes.size();
    for (Hierarchy<uint>::ChildrenIterator I=h->children();
    !I.end();I.next()) {
    unfoldHierarchyDepthFirst(I.current(),nodepos);
    }
    }

    public:

    StaticHierarchy(Hierarchy<T_Data>::t_AutoPtr h)
    {
    unfoldHierarchyDepthFirst(h,-1);
    }



    }
    */
  } // namespace DataStructures
} //namespace LibSL

// ------------------------------------------------------



