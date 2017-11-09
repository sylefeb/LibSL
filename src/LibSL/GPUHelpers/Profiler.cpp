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
#include "LibSL.precompiled.h"

// --------------------------------------------------------------

#include <LibSL/LibSL.h>

#include "Profiler.h"
#include "GPUHelpers.h"

// ---------------------------------------------------------------

#define NAMESPACE LibSL::GPUHelpers

// ---------------------------------------------------------------

NAMESPACE::ProfilerVarData::ProfilerVarData(std::string name,e_Color clr,int period,bool frequency) 
{
  m_Next = 0;
  m_Name = name;
  ForIndex(i,PROFILER_HISTORY_MAX_SIZE) {
    history(i) = 0.0;
  }
  m_Color            = clr;
  m_Init             = false;
  m_LastUpdate       = 0;
  m_UpdatePeriod     = period;
  m_Group            = 0;
  m_RecordInProgress = false;
  m_MeasureFrequency = frequency;
  sl_assert(!frequency || m_UpdatePeriod > 0);
}

// ---------------------------------------------------------------

void NAMESPACE::ProfilerVarData::step()
{
  time_t tm = System::Time::milliseconds();
  
  if (!m_Init || m_UpdatePeriod < 0 || (int(tm - m_LastUpdate) > m_UpdatePeriod)) {
    double v = getValue();
    if (m_MeasureFrequency) {
      v = v * m_UpdatePeriod / double(tm - m_LastUpdate);
      setValue(0);
    }
    if (v > m_Max || !m_Init) {
      m_Max = v;
    }
    if (v < m_Min || !m_Init) {
      m_Min = v;
    }
    history((m_Next++) % PROFILER_HISTORY_MAX_SIZE) = v;
    m_LastUpdate = tm;
    m_Init       = true;
  }

  // record
  if (m_RecordInProgress) {
    fprintf(m_RecordFile,"%d %f\n",m_RecordFrame,getValue());
    m_RecordFrame ++;
  }

}

// ---------------------------------------------------------------

void NAMESPACE::ProfilerVarData::normalize()
{
  std::cerr << "=====> " << name() << " <===== " << std::endl;
  std::cerr << "  min = " << m_Min << ' ' << " max = " << m_Max << std::endl;
  m_Max = history(0);
  m_Min = history(0);
  ForIndex(i,PROFILER_HISTORY_MAX_SIZE) {
    double v = history(i);
    if (v > m_Max) {
      m_Max = v;
    }
    if (v < m_Min) {
      m_Min = v;
    }
  }
  std::cerr << "  over last " << PROFILER_HISTORY_MAX_SIZE << " frames: " << std::endl;
  std::cerr << "  min = " << m_Min << ' ' << " max = " << m_Max << std::endl;
  std::cerr << std::endl;
}

// ---------------------------------------------------------------

double& NAMESPACE::ProfilerVarData::history(int i)
{
  sl_assert(i >= 0 && i < PROFILER_HISTORY_MAX_SIZE);
  return (m_History[i]);
}

// ---------------------------------------------------------------

const double& NAMESPACE::ProfilerVarData::history(int i) const
{
  sl_assert(i >= 0 && i < PROFILER_HISTORY_MAX_SIZE);
  return (m_History[i]);
}

// ---------------------------------------------------------------

void NAMESPACE::ProfilerVarData::startRecord(const char *dir)
{
  m_RecordInProgress=true;
  m_RecordFrame=0;
  std::string fname;
  if (dir) {
    fname = std::string(dir) + m_Name + ".plot";
  } else {
    fname = m_Name + ".plot";
  }
  m_RecordFile = NULL;
	fopen_s(&m_RecordFile, fname.c_str(), "w");
  if (m_RecordFile == NULL) {
    m_RecordInProgress = false;
  }
}

// ---------------------------------------------------------------

void NAMESPACE::ProfilerVarData::stopRecord()
{
  if (m_RecordInProgress) {
    m_RecordInProgress = false;
    fclose(m_RecordFile);
  }
}

// ---------------------------------------------------------------
// ---------------------------------------------------------------
// ---------------------------------------------------------------

void   NAMESPACE::ProfilerTimer::start() 
{
  m_Start = System::Time::milliseconds();
}

// ---------------------------------------------------------------

void   NAMESPACE::ProfilerTimer::stop()  
{
  m_Accum += (System::Time::milliseconds() - m_Start);
}

// ---------------------------------------------------------------
// ---------------------------------------------------------------
// ---------------------------------------------------------------

NAMESPACE::Profiler *NAMESPACE::Profiler::getUniqueInstance(bool destroy)
{
  static Profiler *profiler=NULL;
  if (destroy) {
    if (NULL != profiler)
      delete (profiler);
  } else {
    if (NULL == profiler) {
      profiler = new Profiler();
    }
  }
  return (profiler);
}

// ---------------------------------------------------------------

NAMESPACE::Profiler::Profiler()
{
  m_DisplayGroup = 0;
}

// ---------------------------------------------------------------

NAMESPACE::Profiler::~Profiler()
{
  m_Font = AutoPtr<Font>();
}

// ---------------------------------------------------------------

void NAMESPACE::Profiler::init(AutoPtr<Font> f)
{ 
  m_Font = f;
}

// ---------------------------------------------------------------

void NAMESPACE::Profiler::step()
{
  // step vars
  ForIndex(i,m_Vars.size()) {
    m_Vars[i]->step();
  }
  // update group's min/max
  computeGroupsMinMax();
}

// ---------------------------------------------------------------

void NAMESPACE::Profiler::computeGroupsMinMax()
{
  double gmin=0,gmax=0;
  // global min/max
  ForIndex(i,m_Vars.size()) {
    if (i == 0) {
      gmin = m_Vars[i]->min();
      gmax = m_Vars[i]->max();
    } else {
      gmin = Math::min(gmin,m_Vars[i]->min());
      gmax = Math::max(gmax,m_Vars[i]->max());
    }
  }
  // update group's min/max values
  for (std::map<int,std::pair<double,double> >::iterator G=m_GroupsMinMax.begin();
    G != m_GroupsMinMax.end();G++)
  {
    (*G).second.first  = gmax; // init min with global max
    (*G).second.second = gmin; // init max with global min
  }
  ForIndex(i,m_Vars.size()) {
    m_GroupsMinMax[m_Vars[i]->getGroup()].first  = Math::min(m_GroupsMinMax[m_Vars[i]->getGroup()].first ,m_Vars[i]->min());
    m_GroupsMinMax[m_Vars[i]->getGroup()].second = Math::max(m_GroupsMinMax[m_Vars[i]->getGroup()].second,m_Vars[i]->max());
  }
}

// ---------------------------------------------------------------

void NAMESPACE::Profiler::draw()
{
  //glPushAttrib(GL_ENABLE_BIT);
  //glDisable(GL_LIGHTING);
  //glDisable(GL_TEXTURE_2D);
  ForIndex(i,m_Vars.size()) {
    if (m_Vars[i]->isValid() && (m_DisplayGroup == 0 || m_DisplayGroup == m_Vars[i]->getGroup())) {
      uint  clr = m_Vars[i]->color();
      uchar r   = (clr >> 16) & 255;
      uchar g   = (clr >>  8) & 255;
      uchar b   = (clr      ) & 255;
      Curve curve;
      curve.begin(GPUMESH_LINESTRIP);
      curve.color0_4(r,g,b,255);

	  ForIndex(j,PROFILER_HISTORY_MAX_SIZE-1) {
		  if (m_Vars[i]->getGroup() != 0) {
			  curve.vertex_3(
				  (float) (j/(double)(PROFILER_HISTORY_MAX_SIZE-1)),
				  (float) ((m_Vars[i]->previousValue(PROFILER_HISTORY_MAX_SIZE-1-j)-m_GroupsMinMax[m_Vars[i]->getGroup()].first)
				  /(m_GroupsMinMax[m_Vars[i]->getGroup()].second-m_GroupsMinMax[m_Vars[i]->getGroup()].first)),
				  0);
        } else {
			curve.vertex_3(
				(float) (j/(double)(PROFILER_HISTORY_MAX_SIZE-1)),
				(float) ((m_Vars[i]->previousValue(PROFILER_HISTORY_MAX_SIZE-1-j)-m_Vars[i]->min())
				/(m_Vars[i]->max()-m_Vars[i]->min())),
				0);
		  }
      }
      curve.end();
      
      LibSL::GPUHelpers::Transform::identity(LIBSL_MODELVIEW_MATRIX);
      curve.render();
      
      if (!m_Font.isNull()) {
        static char str[64];
				if (m_Vars[i]->isTime()) {
					sprintf_s(str, 64, "%s %.2f ms", m_Vars[i]->name().c_str(), m_Vars[i]->previousValue(1));
				} else {
					sprintf_s(str, 64, "%s %.2f", m_Vars[i]->name().c_str(), m_Vars[i]->previousValue(1));
				}
        // std::cerr << str << std::endl;
        float w,h;
        m_Font->printStringNeed(0.04f,str,&w,&h);
        float y;
        if (m_Vars[i]->getGroup() != 0) {
          y = (float)((m_Vars[i]->previousValue(1)-m_GroupsMinMax[m_Vars[i]->getGroup()].first)/(m_GroupsMinMax[m_Vars[i]->getGroup()].second-m_GroupsMinMax[m_Vars[i]->getGroup()].first));
        } else {
          y = (float)((m_Vars[i]->previousValue(1)-m_Vars[i]->min())/(m_Vars[i]->max()-m_Vars[i]->min()));
        }
        m_Font->printString(1.0f-w,Math::min(y,1.0f - 0.04f),0.04f,str);
      }
    }
  }
  // glPopAttrib();
}

// ---------------------------------------------------------------

void  NAMESPACE::Profiler::displayNextGroup()
{
  static std::map<int,std::pair<double,double> >::iterator I=m_GroupsMinMax.begin();

  if (m_DisplayGroup == 0) {
    I = m_GroupsMinMax.begin();
  }
  I++;
  if (I == m_GroupsMinMax.end()) {
    m_DisplayGroup = 0;
  } else {
    m_DisplayGroup = (*I).first;
  }
}

// ---------------------------------------------------------------

void NAMESPACE::Profiler::normalize()
{
  ForIndex(i,m_Vars.size()) {
    m_Vars[i]->normalize();
  }
  // update group's min/max
  computeGroupsMinMax();
}

// ---------------------------------------------------------------

int NAMESPACE::Profiler::createTimer(std::string name,e_Color clr,int group)
{
  ProfilerTimer *tm = new ProfilerTimer(name,clr,-1);
  tm->setGroup(group);
  m_Vars.push_back(tm);
  return (int(m_Vars.size())-1);
}

// ---------------------------------------------------------------

void NAMESPACE::Profiler::startTimer(int tm)
{
  static_cast<ProfilerTimer *>(m_Vars[tm])->start();
}

// ---------------------------------------------------------------

void NAMESPACE::Profiler::stopTimer(int tm)
{
  static_cast<ProfilerTimer *>(m_Vars[tm])->stop();
}

// ---------------------------------------------------------------

void NAMESPACE::Profiler::startRecord(const char *dir)
{
  ForIndex(i,m_Vars.size()) {
    m_Vars[i]->startRecord(dir);
  }
}

// ---------------------------------------------------------------

void NAMESPACE::Profiler::stopRecord()
{
  ForIndex(i,m_Vars.size()) {
    m_Vars[i]->stopRecord();
  }
}

// ---------------------------------------------------------------

#define ADDVAR(T) void NAMESPACE::Profiler::addVar(T *ptr,std::string name,e_Color clr,int group,int period,bool freq) \
{ \
  NAMESPACE::ProfilerVar<T> *var = new NAMESPACE::ProfilerVar<T>(name,clr,ptr,period,freq); \
  var->setGroup(group); \
	m_Vars.push_back(var); \
}

// ---------------------------------------------------------------

ADDVAR(double)
ADDVAR(float)
ADDVAR(int)
ADDVAR(uint)

// ---------------------------------------------------------------
