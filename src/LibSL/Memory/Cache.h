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
// LibSL::Memory::LRUCache
// ------------------------------------------------------
//
// Generic LRU Cache
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2007-04-05
// ------------------------------------------------------

#pragma once

// ------------------------------------------------------

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/System/Types.h>
#include <LibSL/Math/Tuple.h>
using namespace LibSL::System::Types;

#include "LibSL/Memory/Array.h"

#include <list>
#include <map>

// ------------------------------------------------------

namespace LibSL  {
  namespace Memory {

    /// LRUCache class
    template<class T_AccessKey,class T_CachedData>
    class LRUCache
    {
    protected:

      typedef std::list<T_AccessKey>        t_LRU;

      class Cached {
      public:
        T_CachedData              m_Value;
        typename t_LRU::iterator  m_LRUPos;
      };

      typedef std::map<T_AccessKey, Cached> t_CacheData;

      t_CacheData m_CacheData;
      t_LRU       m_LRU;

      uint        m_NumReq;
      uint        m_NumMiss;
      uint        m_NumDel;

      uint        m_Size;
      bool        m_Enabled;

    public:

      LRUCache(uint sz) 
      { 
        m_Size    = sz;
        m_NumReq  = 0;
        m_NumMiss = 0;
        m_NumDel  = 0;
        m_Enabled = true;
      }

      /// Test whether a value is available in the cache.
      /// If yes: returns true, the value is copied in "_value"
      bool contains(const T_AccessKey& a, T_CachedData* &_value)
      {
        m_NumReq ++;
        if (!m_Enabled) { 
          m_NumMiss ++; 
          return (false); 
        }
        t_CacheData::iterator C = m_CacheData.find(a);
        if (C == m_CacheData.end()) {
          // miss
          m_NumMiss ++;
          return (false);
        } else {
          // get value
          _value                = &((*C).second.m_Value);
          // update pos in LRU
          t_LRU::iterator li    = (*C).second.m_LRUPos;
          m_LRU.splice(m_LRU.begin(), m_LRU, li);
          return (true);
        }
      }

      /// Add a new value to the cache
      void add(const T_AccessKey& a,const T_CachedData& d)
      {
        if (!m_Enabled) {
          return;
        }
        m_LRU          .push_front(a);
        Cached c;
        c.m_Value      = d;
        c.m_LRUPos     = m_LRU.begin();
        m_CacheData    .insert(make_pair(a,c));
        if (m_CacheData.size() > m_Size) {
          // supress oldest one
          m_NumDel ++;
          const T_AccessKey& key = m_LRU.back();
          m_CacheData  .erase(key);
          m_LRU        .pop_back();
        }
      }

      /// Disable the cache. Call 'reset' to re-enable (will thrash the cache)
      void disable() 
      {
        m_Enabled = false;
      }

      /// Reset the cache
      void reset(uint new_sz) 
      {
        m_CacheData.clear();
        m_LRU      .clear();
        m_NumReq   = 0;
        m_NumMiss  = 0;
        m_NumDel   = 0;
        m_Enabled  = true;
        m_Size     = new_sz;
        m_Size    = new_sz;
      }

      void printStats() 
      {
        if (m_Enabled) {
          cout << sprint("(size: %d nodes): %d requests, %d misses, %d deletions \n",m_Size,m_NumReq,m_NumMiss,m_NumDel);
        } else {
          cout << sprint("!!DISABLED!! Cache stats (size: %d nodes): (%d requests)\n",m_Size,m_NumReq);
        }
      }

      uint numMiss() const {return (m_NumMiss);}
      uint numReq()  const {return (m_NumReq);}
      uint size()    const {return (m_Size);}

      typedef T_AccessKey  t_AccessKey;
      typedef T_CachedData t_CachedData;

    };


  } // namespace LibSL::Memory
} // namespace LibSL


// ------------------------------------------------------
