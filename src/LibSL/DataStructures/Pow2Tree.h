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
// LibSL::DataStructures::Pow2Tree
// ------------------------------------------------------
//
// Pow2Tree class
//  - Power of 2 trees: split along all dimensions by 2
//    at every node
//  - Typical specializations are BinaryTree, QuadTree and OcTree
//
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-11-14
// ------------------------------------------------------

#pragma once

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/Memory/Pointer.h>
#include <LibSL/Memory/TraceLeaks.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Math.h>

#include <loki/SmallObj.h>

#include <list>

namespace LibSL {
  namespace DataStructures {

    /*!

    \class Pow2Tree
    \brief Generic class for binary trees, quadtrees and octrees

    */
    template <uint T_NumDim,typename T_Data>
    class Pow2Tree
      : public Loki::SmallObject<> // use small object allocator
//      , public LibSL::Memory::TraceLeaks::LeakProbe<Pow2Tree<T_NumDim,T_Data> >
    {
    public:

      /// Pointer
      /// NOTE: Why not using an AutoPtr as in Hierarchy?
      //        Mainly for efficiency concerns. Pow2Tree are meant to be used
      //        on large datasets. The internal overhead of AutoPtr becomes
      //        unacceptable in this situation.
      typedef LibSL::Memory::Pointer::Pointer<
        Pow2Tree,
#ifdef LIBSL_RELEASE
        LibSL::Memory::Pointer::CheckNop,
#else
        LibSL::Memory::Pointer::CheckValid,
#endif
        LibSL::Memory::Pointer::TransferAddress>   t_Pointer;

      // Children iterator class
      class ChildrenIterator 
      {
      private:

        Pow2Tree                          *m_Owner;
        LibSL::Math::Tuple<int,T_NumDim>   m_Iterator;

      public:

        ChildrenIterator(Pow2Tree *h)
        {
          m_Owner    = h;
          m_Iterator = 0;
          if (m_Owner->m_Children[Pow2Tree::access(m_Iterator)].isNull()) {
            next();
          }
        }

        LIBSL_DISABLE_COPY(ChildrenIterator);

        //! Returns true if there are no more children in the list
        bool end()
        {
          LibSL::Math::Tuple<int,e_NumDim> size;
          size = 2;
          return (LibSL::Math::done(m_Iterator,size));
        }

        //! Go to the next child in the list
        //!   end() must be false
        //!   current() will point to the next children after next() returns
        void next()
        {
          sl_assert(!end());
          LibSL::Math::Tuple<int,e_NumDim> size;
          size = 2;
          while (1) {
            LibSL::Math::inc(m_Iterator,size);
            if (LibSL::Math::done(m_Iterator,size)) {
              break;
            }
            if (!m_Owner->m_Children[Pow2Tree::access(m_Iterator)].isNull()) {
              break;
            }
          }
        }

        //! Retrieve current child
        t_Pointer current() 
        {
          return (m_Owner->m_Children[Pow2Tree::access(m_Iterator)]);
        }

        //! Delete the current child from the hierarchy
        void deleteCurrent()
        {
          sl_assert(!end());
          m_Owner->deleteChild(Pow2Tree::access(m_Iterator));
          next();
        }
      };

    public:

      enum {e_NumDim      = T_NumDim};
      enum {e_NumChildren = (1 << T_NumDim)};
      
      typedef LibSL::Math::Tuple<int,T_NumDim> t_Access;

    private:

      T_Data    m_Data;
      t_Pointer m_Children[e_NumChildren];
      bool      m_Owner[e_NumChildren];

      static uint access(const t_Access& pos)
      {
        uint offset = 0;
        uint stride = 1;
        ForIndex(n,T_NumDim) {
          sl_assert(pos[n] >= 0);
          sl_assert(pos[n]  < 2);
          offset  += pos[n]*stride;
          stride <<= 1;
        }
        return (offset);
      }

    public:

      //! Build a leaf around some data
      Pow2Tree(const T_Data& d)
      {
        m_Data = d;
        ForIndex(n,e_NumChildren) {
          m_Children[n] = NULL;
          m_Owner[n]    = false;
        }
      }

      ~Pow2Tree()
      {
        ForTuple_int(T_NumDim,cpos,2) {
          deleteChild(cpos);
        }
      }

      LIBSL_DISABLE_COPY(Pow2Tree);

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
      void addChild(const t_Access& pos,const T_Data& d)
      {
        t_Pointer child         = new Pow2Tree(d);
        m_Owner   [access(pos)] = true;
        m_Children[access(pos)] = (child);
      }

      //! Add an existing child hierarchy to the hierarchy
      void addChild(const t_Access& pos,t_Pointer child)
      {
        deleteChild(pos);
        m_Owner   [access(pos)] = false;
        m_Children[access(pos)] = (child);
      }

      //! Delete a child from the hierarchy
      void deleteChild(const t_Access& pos)
      {
        if (m_Owner[access(pos)]) {
          delete (m_Children[access(pos)]);
        }
        m_Children[access(pos)] = NULL;
      }

      //! Returns the number of children
      LibSL::Math::Tuple<uint,T_NumDim> numChildren() const
      {
        LibSL::Math::Tuple<uint,T_NumDim> sz = 2;
        return (sz);
      }

      //! Returns an iterator on children (see also Pow2Tree::ChildrenIterator)
      ChildrenIterator children()
      {
        return (ChildrenIterator(this));
      }

      //! Returns a child
      const t_Pointer& childAt(const t_Access& pos)
      {
        return (m_Children[access(pos)]);
      }
      const t_Pointer& childAt(const LibSL::Math::Tuple<uint,T_NumDim>& pos)
      {
        return (childAt(t_Access(pos)));
      }

      //! Returns number of nodes
      uint numNodes() const
      {
        uint n = 1;
        ForIndex(c,e_NumChildren) {
          if (!m_Children[c].isNull()) {
            n += m_Children[c]->numNodes();
          }
        }
        return (n);
      }

      //! Returns true if tree is a leaf
      bool isLeaf() const
      {
        ForIndex(c,e_NumChildren) {
          if (!m_Children[c].isNull()) {
            return (false);
          }
        }
        return (true);
      }

      //! Returns number of leaves
      uint numLeaves() const
      {
        if (isLeaf()) {
          return (1);
        } else {
          uint n = 0;
          ForIndex(c,e_NumChildren) {
            if (!m_Children[c].isNull()) {
              n += m_Children[c]->numLeaves();
            }
          }
          return (n);
        }
      }

      //! Returns tree height
      uint height() const
      {
        uint d = 0;
        ForIndex(c,e_NumChildren) {
          if (!m_Children[c].isNull()) {
            d = LibSL::Math::max(d,1+m_Children[c]->height());
          }
        }
        return (d);
      }

    };

    //! BinaryTree
    template <class T_Data>
    class BinaryTree : public Pow2Tree<1,T_Data>
    {
    public:

      BinaryTree(const T_Data& d) : Pow2Tree<1,T_Data>(d)
      {
      }

      LIBSL_DISABLE_COPY(BinaryTree);

    };

    //! QuadTree
    template <class T_Data>
    class QuadTree : public Pow2Tree<2,T_Data>
    {
    public:

      QuadTree(const T_Data& d) : Pow2Tree<2,T_Data>(d)
      {
      }

      LIBSL_DISABLE_COPY(QuadTree);

    };

    //! OcTree
    template <class T_Data>
    class OcTree : public Pow2Tree<3,T_Data>
    {
    public:

      OcTree(const T_Data& d) : Pow2Tree<3,T_Data>(d)
      {
      }

      LIBSL_DISABLE_COPY(OcTree);

    };

  } //namespace LibSL::DataStructures
} //namespace LibSL

// ------------------------------------------------------



