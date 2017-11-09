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
// LibSL::Math::Stats
// ------------------------------------------------------
//
// Stats class
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-10-09
// ------------------------------------------------------

#pragma once

#include <LibSL/CppHelpers/CppHelpers.h>

namespace LibSL {
  namespace Math {

    template<typename T_Type>
    class Stats
    {
    protected:

      T_Type      m_Min;
      T_Type      m_Max;
      T_Type      m_Avg;
      T_Type      m_AvgSq;
      uint        m_Num;
      const char *m_Name;
      bool        m_Silent;

    public:

      typedef T_Type t_Element;

      Stats(const char *name="",bool silent=false)  { 
        m_Min    = T_Type(0);
        m_Max    = T_Type(0);
        m_Avg    = T_Type(0);
        m_AvgSq  = T_Type(0);
        m_Num    = 0;
        m_Name   = name;
        m_Silent = silent;
      }

      ~Stats() { if (!m_Silent) { print(); } }

      void reset()
      {
        m_Min    = T_Type(0);
        m_Max    = T_Type(0);
        m_Avg    = T_Type(0);
        m_AvgSq  = T_Type(0);
        m_Num    = 0;
      }

      void print()
      {
        if (m_Num > 0) {
          std::cout << "[" << m_Name 
            << "] min = " << m_Min
            << ", max = " << m_Max
            << ", avg = " << avg()
            << ", std = " << std()
            << ", num = " << m_Num
            << "\n";
        }
      }

      T_Type min() const { return (m_Min); }
      T_Type max() const { return (m_Max); }
      T_Type avg() const { if (m_Num == 0) { return T_Type(0); } else { return (m_Avg / T_Type(m_Num)); } }
      T_Type std() const { if (m_Num == 0 || m_Num == 1) { return T_Type(0); } else { T_Type a = avg(); return sqrt( m_AvgSq / T_Type(m_Num) - a*a ); } }

      void operator <<(const T_Type& t)
      {
        if (m_Num == 0) {
          m_Min = t;
          m_Max = t;
        } else {
          m_Min = Math::min(t,m_Min);
          m_Max = Math::max(t,m_Max);
        }
        m_Avg   = m_Avg + t;
        m_AvgSq = m_AvgSq + (t*t);
        m_Num++;
      }

    };

  } //namespace LibSL::Math
} //namespace LibSL

// ------------------------------------------------------
