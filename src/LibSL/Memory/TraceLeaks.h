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
// LibSL::Memory::TraceLeaks
// ------------------------------------------------------
//
// Tools to track memory leaks.
// To debug leaks, make your class inherit from LeakProbe.
//
// Note: if an object inherits LeakProbe, it must reimplement
//       the copy operator (or instance count could be wrong)
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-02-21
// ------------------------------------------------------

#pragma once

#include <LibSL/LibSL.common.h>

// ------------------------------------------------------

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/System/Types.h>
using namespace LibSL::System::Types;

// ----------------------------------------------------

#include <map>
#include <string>
#include <iostream>
#include <typeinfo>

// ----------------------------------------------------

#define LEAK_TRACKER (*LibSL::Memory::TraceLeaks::LeakTracker::getUniqueInstance())

// ------------------------------------------------------

namespace LibSL  {
  namespace Memory {
    namespace TraceLeaks {

      class LIBSL_DLL LeakTracker // singleton
      {
      private:

        std::map<std::string,int> m_InstanceCounters;

        static LeakTracker *s_Tracker;
        LeakTracker() {}

      public:

        ~LeakTracker();

        int&  classCounter(const std::string& classname)
        {
          std::map<std::string,int>::iterator F=m_InstanceCounters.find(classname);
          if (F==m_InstanceCounters.end())
            m_InstanceCounters[classname]=0;
          return (m_InstanceCounters[classname]);
        }

        static LeakTracker *getUniqueInstance();

        void report() const;
      };

      template <class T_Tracked> class LeakProbe
      {
      public:
        LeakProbe()
        {
          LEAK_TRACKER.classCounter(typeid(T_Tracked).name())++;
        }
        virtual ~LeakProbe()
        {
          LEAK_TRACKER.classCounter(typeid(T_Tracked).name())--;
        }
        LeakProbe(const LeakProbe& o) {}
      };

    } // namespace LibSL::TraceLeaks
  } // namespace LibSL::Memory
} // namespace LibSL

// ----------------------------------------------------
