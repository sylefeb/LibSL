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
// LibSL::GPUHelpers::Profiler
// ------------------------------------------------------
//
//   Usefull to profile graphics app (D3D and GL)
//   Define D3D to enable DirectX support
//
// Author : Sylvain Lefebvre
// History: Created somewhere during 2002
//          2004-03-18 merge D3D and OpenGL versions
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-21
// ------------------------------------------------------

#pragma once

#include <LibSL/LibSL.common.h>

// ------------------------------------------------------

#include <LibSL/GPUHelpers/Font.h>

#include <LibSL/Errors/Errors.h>
#include <LibSL/System/Types.h>
#include <LibSL/Memory/Pointer.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Matrix4x4.h>
#include <LibSL/Geometry/Polygon.h>

#include <vector>
#include <string>
#include <iostream>
#include <map>

// ------------------------------------------------------

#define PROFILER         (*LibSL::GPUHelpers::Profiler::getUniqueInstance())

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

// ------------------------------------------------------

#define PROFILER_HISTORY_MAX_SIZE 100

// ------------------------------------------------------

namespace LibSL {
  namespace GPUHelpers {

    using namespace LibSL::Mesh;

    // ---------------

    enum e_Color {blue  = 0x000000FF,red   = 0x00FF0000,green = 0x0000FF00,yellow= 0x00FFFF00,purple= 0x00FF00FF};

    // ---------------

    class LIBSL_DLL ProfilerVarData
    {
    protected:
      double      m_History[PROFILER_HISTORY_MAX_SIZE];
      double      m_Max;
      double      m_Min;
      int         m_Next;
      std::string m_Name;
      uint        m_Color;
      bool        m_Init;
      bool        m_MeasureFrequency;
      int         m_UpdatePeriod;
      time_t      m_LastUpdate;
      int         m_Group;
      FILE       *m_RecordFile;
      bool        m_RecordInProgress;
      int         m_RecordFrame;

      double&         history(int i);
      const double&   history(int i) const;

    public:
      ProfilerVarData(std::string name,e_Color clr,int period,bool frequency);
      virtual ~ProfilerVarData() {}

      virtual double getValue()      const =0;
      virtual void   setValue(double)      =0;
      virtual void   step();
      virtual bool   isTime()    const {return (false);}

      int            getGroup()      const {return (m_Group);}
      void           setGroup(int g)       {m_Group=g;}

      double         previousValue(int age) const {return (history(((m_Next+PROFILER_HISTORY_MAX_SIZE-age))%PROFILER_HISTORY_MAX_SIZE));}

      std::string    name()      const {return (m_Name);}
      uint           color()     const {return (m_Color);}
      double         min()       const {return (m_Min);}
      double         max()       const {return (m_Max);}
      bool           isValid()   const {return (m_Init);}
      void           normalize();

      void           startRecord(const char *dir);
      void           stopRecord();
    };

    // ---------------

    template <typename T> class ProfilerVar : public ProfilerVarData
    {
    protected:
      T *m_Ptr;
    public:
      ProfilerVar(std::string name,e_Color clr,T *ptr,int period,bool freq)
        : ProfilerVarData(name,clr,period,freq) { m_Ptr = ptr; }
      virtual ~ProfilerVar() {}
      virtual double getValue()         const {return (double)(*m_Ptr);}
      virtual void   setValue(double v)       {(*m_Ptr)=(T)(v);}
    };

    // ---------------

    class LIBSL_DLL ProfilerTimer : public ProfilerVarData
    {
    protected:
      time_t m_Accum;
      time_t m_Start;
    public:
      ProfilerTimer(std::string name,e_Color clr,int period)
        : ProfilerVarData(name,clr,period,false) { m_Accum = 0 ; m_Start = 0; }
      virtual ~ProfilerTimer() {}

      void   start();
      void   stop();

      virtual double getValue()      const {return (double)(m_Accum);}
      virtual void   setValue(double)      {}

      virtual void   step() { ProfilerVarData::step(); m_Accum = 0;}
      virtual bool   isTime() const {return (true);}
    };

    // ---------------

#define ADDVAR_DEF(T) void addVar(T *ptr,std::string name,e_Color clr,int group=0,int period=-1,bool frequency=false);

    // ---------------

    class LIBSL_DLL Profiler
    {
    protected:
      std::vector<ProfilerVarData *>          m_Vars;
      std::map<int,std::pair<double,double> > m_GroupsMinMax;
      int                                     m_DisplayGroup;

      LibSL::Memory::Pointer::AutoPtr<Font>   m_Font;

      typedef MVF2(mvf_position_3f,mvf_color0_rgba)          mvf_curve;
      typedef GPUMesh::GPUMesh_VertexBuffer<mvf_curve>     Curve;

      Profiler();
      ~Profiler();

      void computeGroupsMinMax();

    public:

      void init(LibSL::Memory::Pointer::AutoPtr<Font> f);

      void                       step();
      void                       draw();
      void                       normalize();
      int                        createTimer(std::string name,e_Color clr,int group=0);
      void                       startTimer(int tm);
      void                       stopTimer(int tm);
      std::pair<double,double>   getGroupMinMax(int g) {return (m_GroupsMinMax[g]);}
      void                       displayAllGroups() {m_DisplayGroup=0;}
      void                       displayNextGroup();
      void                       startRecord(const char *dir=NULL);
      void                       stopRecord();

      ADDVAR_DEF(double)
      ADDVAR_DEF(float)
      ADDVAR_DEF(int)
      ADDVAR_DEF(uint)

      static Profiler *getUniqueInstance(bool destroy = false);

      static void terminate() { getUniqueInstance(true); }

    };

  } //namespace LibSL::GPUHelpers
} //namespace LibSL

//------------------------------------------------------------------------
