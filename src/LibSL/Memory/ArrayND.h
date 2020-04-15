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
// LibSL::Memory::ArrayND
// ------------------------------------------------------
//
// ND Array built upon Array
// 
// Access is done by get(T)/set(T) or array[T] or at(T)
//
// => arrays are stored in row major order
//
// TODO replace Array2D and Array3D by typedefs on ArrayND
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-12-16
// ------------------------------------------------------

#pragma once

// ------------------------------------------------------

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/System/Types.h>
#include <LibSL/Math/Tuple.h>
using namespace LibSL::System::Types;

#include "LibSL/Memory/Array.h"

// ------------------------------------------------------

namespace LibSL  {
  namespace Memory {
    namespace Array {

      /// ArrayND class
      ///   Configurable through Array policies

      template <
        int      T_Dim,
        typename T_Type,
        template <typename> class P_Init=InitNop,
#ifdef LIBSL_RELEASE
        class P_Check=CheckNop
#else
        class P_Check=CheckAll
#endif
      >
      class ArrayND 
        // : public LibSL::Memory::TraceLeaks::LeakProbe<ArrayND<T_Type,P_Init,P_Check> >
      {
      private:

        Array<T_Type,P_Init,P_Check>   m_Array;
        LibSL::Math::Tuple<uint,T_Dim> m_Size;

      public:

        typedef T_Type t_Element;

        enum {e_NumDim = T_Dim};

      public:

        ArrayND(void) 
        {
          m_Size = 0;
        }

        ArrayND(const LibSL::Math::Tuple<uint,T_Dim>& size) 
        {
          m_Size = size;
          m_Array.allocate(LibSL::Math::productOfAll(m_Size));
        }

        ~ArrayND(void)
        {
        }

        void allocate(const LibSL::Math::Tuple<uint,T_Dim>& size) 
        {
          m_Size = size;
          m_Array.allocate(LibSL::Math::productOfAll(m_Size));
        }

        /// Copy (constructor)
        ArrayND(const ArrayND& a)
        {
          m_Size  = a.m_Size;
          m_Array = a.m_Array;
        }

        /// Copy (affectation)
        const ArrayND& operator = (const ArrayND& a)
        {
          m_Size  = a.m_Size;
          m_Array = a.m_Array;
          return (*this);
        }

        /// Fill array with a given value
        void fill(T_Type value_to_fill_with)
        {
          m_Array.fill(value_to_fill_with);
        }

        /// Read only access
        const T_Type& get(const LibSL::Math::Tuple<uint,T_Dim>& idx) const
        {
          return (at(idx));
        }

        /// Read only access
        const T_Type& at(const LibSL::Math::Tuple<uint,T_Dim>& idx) const
        {
          ForIndex(n,T_Dim) {
            if (P_Check::PerformCheck) P_Check::checkAccess(idx[n],m_Size[n]);
          }
          return (m_Array[LibSL::Math::addressOf(idx,m_Size)]);
        }

        /// Read/write access
        T_Type& set(const LibSL::Math::Tuple<uint,T_Dim>& idx)
        {
          return (at(idx));
        }

        /// Read/write access
        T_Type& at(const LibSL::Math::Tuple<uint,T_Dim>& idx)
        {
          ForIndex(n,T_Dim) {
            if (P_Check::PerformCheck) P_Check::checkAccess(idx[n],m_Size[n]);
          }
          return (m_Array[LibSL::Math::addressOf(idx,m_Size)]);
        }

        /// [] operator with Tuple uint
        const T_Type& operator[](const LibSL::Math::Tuple<uint,T_Dim>& idx) const
        {
          return at(idx);
        }

        T_Type& operator[](const LibSL::Math::Tuple<uint,T_Dim>& idx)
        {
          return at(idx);
        }

        /// [] operator with Tuple int
        const T_Type& operator[](const LibSL::Math::Tuple<int,T_Dim>& idx) const
        {
          return at(LibSL::Math::Tuple<uint,T_Dim>(idx));
        }

        T_Type& operator[](const LibSL::Math::Tuple<int,T_Dim>& idx)
        {
          return at(LibSL::Math::Tuple<uint,T_Dim>(idx));
        }

        /// Array size as a tuple
        LibSL::Math::Tuple<uint,T_Dim> sizeTuple() const 
        {
          return (m_Size);
        }

        /// empty?
        bool empty() const
        {
          return (m_Array.empty());
        }

        /// Erase
        void erase()
        {
          m_Array.erase();
          m_Size = 0;
        }

        /// Raw pointer
        const T_Type *raw() const {return (m_Array.raw());}
        T_Type       *raw()       {return (m_Array.raw());}

        /// Array size of data
        uint sizeOfData() const 
        {
          return (m_Array.sizeOfData());
        }

      };

#ifdef _MSC_VER // This is due to an incompatibility btw Visual C and g++ // TODO FIXME
             // assumes WIN32 means Visual C++

      /*!

      FastArray build from Array with InitNop and CheckNop policies

      */
      template <int T_Dim,class T_Type> 
      class FastArrayND : public ArrayND<T_Dim,T_Type,InitNop,CheckNop>
      {
      public:
        FastArrayND()                                           : ArrayND()     { }
        FastArrayND(const LibSL::Math::Tuple<uint,T_Dim>& size) : ArrayND(size) { }
      };

#endif

    } // namespace LibSL::Memory::Array
  } // namespace LibSL::Memory
} // namespace LibSL

// ----------------------------------------------------

#define ForArrayND(N,A,T) for (Tuple<uint,N> T=0;!done(T,A.sizeTuple());inc(T,A.sizeTuple()))

// ----------------------------------------------------
