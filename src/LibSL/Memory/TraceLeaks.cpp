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
#include "LibSL.precompiled.h"
// ------------------------------------------------------

#include "TraceLeaks.h"
using namespace LibSL::Memory::TraceLeaks;

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/StlHelpers/StlHelpers.h>
#include <LibSL/System/Types.h>

using namespace LibSL::System::Types;
using namespace LibSL::CppHelpers;

#include <iostream>
#include <sstream>
using namespace std;

// ------------------------------------------------------
#define NAMESPACE LibSL::Memory::TraceLeaks
// ------------------------------------------------------

// leak tracker unique instance
NAMESPACE::LeakTracker *NAMESPACE::LeakTracker::s_Tracker=NULL;

// ----------------------------------------------------

// Class to destroy the tracker upon exit
// Only one global instance is used to track application
// exit and destroy the tracker unique instance.
class TrackerDestroyer
{
public:
  TrackerDestroyer()
  {
  }
  ~TrackerDestroyer()
  {
    delete (NAMESPACE::LeakTracker::getUniqueInstance());
  }
};

static TrackerDestroyer g_Destroyer;

// ----------------------------------------------------

LeakTracker *NAMESPACE::LeakTracker::getUniqueInstance()
{
  if (s_Tracker==NULL) {
    s_Tracker=new LeakTracker();
  }
  return (s_Tracker);
}

// ----------------------------------------------------

NAMESPACE::LeakTracker::~LeakTracker()
{
  ostringstream sstr;
  bool non_0_found=false;

  for (std::map<std::string,int>::iterator I=m_InstanceCounters.begin();I!=m_InstanceCounters.end();I++) {
    if ((*I).second > 0) {
      sstr << sprint("[%s] %d instance(s)\n",(*I).first.c_str(),(*I).second); 
      non_0_found=true;
    }
    if ((*I).second < 0) {
      cerr << "\n===== LibSL memory leak tracker =====\n";
      cerr << sprint(
        "   WARNING: one instance counter is negative:\n"
        "            Make sure the copy operator is explicitely defined"
        "            for class '%s'\n\n",(*I).first.c_str());
      cerr << "=====================================\n\n";
    }
  }
  if (non_0_found) {
    cerr << "\n============== LibSL memory leak tracker ===============\n";
    cerr << "WARNING: non deleted instances found for:\n\n";
    cerr << sstr.str();
    cerr << "\n(NOTE: Only classes inheriting from LeakProbe are traced.)\n";
    cerr << "(      After an exception, objects within the block are  )\n";
    cerr << "(      not destroyed.                                    )\n";
    cerr << "==========================================================\n\n";
  }
}

// ----------------------------------------------------

void NAMESPACE::LeakTracker::report() const
{
  ostringstream sstr;

  for (std::map<std::string,int>::const_iterator I=m_InstanceCounters.begin();I!=m_InstanceCounters.end();I++) {
    if ((*I).second > 0) {
      sstr << sprint("[%s] %d instance(s)\n",(*I).first.c_str(),(*I).second); 
    }
    if ((*I).second < 0) {
      cerr << "\n===== LibSL memory leak tracker =====\n";
      cerr << sprint(
        "   WARNING: one instance counter is negative:\n"
        "            Make sure the copy operator is explicitely defined"
        "            for class '%s'\n\n",(*I).first.c_str());
      cerr << "=====================================\n\n";
    }
  }

  cerr << "\n============== LibSL memory leak tracker ===============\n";
  cerr << "                    Memory status report\n\n";
  cerr << sstr.str();
  cerr << "\n(NOTE: Only classes inheriting from LeakProbe are traced.)\n";
  cerr << "==========================================================\n\n";

}

// ----------------------------------------------------
