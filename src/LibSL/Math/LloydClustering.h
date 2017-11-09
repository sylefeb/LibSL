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
// LibSL::Math
// ------------------------------------------------------
//
// class LloydClustering
//  Simple Lloyd clustering implementation
//
// NOTE:
//
//    Assumes samples are unique
//
// USAGE:
//
//    See tests/test_lloyd.cpp
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-11-28
// ------------------------------------------------------

#pragma once

#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/Memory/Array.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Matrix4x4.h>

#include <list>
#include <vector>
#include <algorithm>

namespace LibSL {
  namespace Math {

    template <class T_Sample>
    class TupleDistance
    {
    public:
      float sqDistanceBetween(const T_Sample& sample,const T_Sample& center) const
      {
        return (sqDistance(sample,center));
      }
    };

    template <class T_Sample>
    class TupleAverager
    {
    protected:
      T_Sample m_RunningSum;
      uint     m_Num;
    public:
      void begin()
      {
        m_Num        = 0;
        m_RunningSum = 0.0f;
      }
      void add(const T_Sample& s)
      {
        m_Num ++;
        m_RunningSum = m_RunningSum + s;
      }
      bool     valid() const {return (m_Num > 0);}
      T_Sample end() const
      {
        if (m_Num > 0) {
          return (m_RunningSum / float(m_Num));
        } else {
          return (m_RunningSum);
        }
      }
    };

    template <class T_Sample>
    class TupleMedianSelect
    {
    protected:
      T_Sample              m_RunningSum;
      uint                  m_Num;
      std::vector<T_Sample> m_Smpls;
    public:
      void begin()
      {
        m_Num        = 0;
        m_RunningSum = 0.0f;
        m_Smpls.clear();
      }
      void add(const T_Sample& s)
      {
        m_Num ++;
        m_RunningSum = m_RunningSum + s;
        m_Smpls.push_back(s);
      }
      bool     valid() const {return (m_Num > 0);}
      T_Sample end() const
      {
        T_Sample avg = 0;
        if (m_Num > 0) {
          avg = (m_RunningSum / float(m_Num));
        } else {
          avg = (m_RunningSum);
        }
        // search closest to average
        if (m_Smpls.size() > 1) {
          float mind = 1e20f;
          int   smpl = -1;
          ForIndex(s,m_Smpls.size()) {
            float  d = sqLength( m_Smpls[s] - avg );
            if (d < mind) {
              mind = d;
              smpl = s;
            }
          }
          sl_assert( smpl > -1 );
          return m_Smpls[ smpl ];
        } else {
          return avg;
        }
      }
    };


    template <
      class T_Sample,
      class T_Distance = TupleDistance<T_Sample>,
      class T_Averager = TupleAverager<T_Sample> > 
    class LloydClustering
    {
    protected:

      T_Distance                             m_Distance;
      /// number of clusters
      uint                                   m_NumClusters;
      /// sample references
      std::vector<T_Sample>                  m_Samples;
      /// cluster centers
      LibSL::Memory::Array::Array<T_Sample,
        LibSL::Memory::Array::InitNop,
        LibSL::Memory::Array::CheckNop>      m_Centers;
      /// cluster ownership
      LibSL::Memory::Array::Array<int,
        LibSL::Memory::Array::InitNop,
        LibSL::Memory::Array::CheckNop>      m_SampleOwner;
      /// averager for cluster centers
      LibSL::Memory::Array::Array<T_Averager,
        LibSL::Memory::Array::InitNop,
        LibSL::Memory::Array::CheckNop>      m_Averager;

    public:

      /// constructor
      LloydClustering()
      {	
        m_NumClusters            = 1;
        m_Samples.clear();
      }

      /// add a sample
      void addSample(const T_Sample& s)
      {
        m_Samples.push_back(s);
      }

      /// find which cluster center is the closest to a sample
      uint findClosestCenter(const T_Sample& s) const
      {
        int   argmin  = -1;
        float distmin = 1e30f;
        ForIndex(c,m_NumClusters) {
          float dist = m_Distance.sqDistanceBetween(s,m_Centers[c]);
          argmin     = dist < distmin ? c : argmin;
          distmin    = LibSL::Math::min(dist,distmin);
        }
        //sl_assert(argmin >= 0);
        return uint(argmin);
      }

      /// distribute samples in clusters
      uint distributeSamples()
      {
        LibSL::System::Time::Timer tm("distributeSamples");
        uint num_changes = 0;
        uint num_samples = uint(m_Samples.size());
        // foreach sample, find closest center
        LibSL::CppHelpers::Console::progressBarInit(uint(m_Samples.size()));
        ForIndex(s,num_samples) {
          LibSL::CppHelpers::Console::progressBarUpdate();
          uint closest     = findClosestCenter(m_Samples[s]); // expensive...
          // update sample owner table, count changes
          if (m_SampleOwner[s] != closest) {
            num_changes ++;
          }
          m_SampleOwner[s] = closest;
        }
        LibSL::CppHelpers::Console::progressBarEnd();
        return (num_changes);
      }

      /// update cluster centers
      void updateCenters()
      {                                 
         LibSL::System::Time::Timer tm("updateCenters    ");
        // -> compute averages
        ForIndex(c,m_NumClusters) {
          m_Averager[c].begin();
        }
        ForIndex(s,m_Samples.size()) {
          int o        = m_SampleOwner[s];
          sl_assert(o >= 0);
          m_Averager[o].add(m_Samples[s]);
        }
        ForIndex(c,m_NumClusters) {
          if (m_Averager[c].valid()) {
            m_Centers[c]  = m_Averager[c].end();
          } else {
            // reassign to a sample
            reassign(c);
          }
        }
      }

      /// teleport
      void teleport()
      {
        std::cerr << "teleporting ... ";
        // find pair of clusters that would result in a cluster with minimal variance
        float minvar            = 0.0f;
        std::pair<int,int> argminvar = std::make_pair(-1,-1);
        ForIndex(c0,m_NumClusters) {
          for (uint c1=c0+1 ; c1<m_NumClusters ; c1++) {
            // compute what would the center be if clusters are merged
            T_Averager avg;
            avg.begin();
            ForIndex(s,m_Samples.size()) {
              if (m_SampleOwner[s] != c0 || m_SampleOwner[s] != c1) {
                continue;
              }
              avg.add(m_Samples[s]);
            }
            //sl_assert(avg.valid());
            T_Sample   newcenter = avg.end();
            // compute variance
            float var      = 0.0f;
            uint  num      = 0;
            ForIndex(s,m_Samples.size()) {
              if (m_SampleOwner[s] != c0 || m_SampleOwner[s] != c1) {
                continue;
              }
              float d = m_Distance.sqDistanceBetween(m_Samples[s],newcenter);
              var += d;
              num ++;
            }
            if (num > 0) {
              var /= float(num);
              // track min variance
              if (argminvar.first < 0 || var < minvar) {
                argminvar = std::make_pair(c0,c1);
                var       = minvar;
              }
            }
          }
        }
        // sl_assert(argminvar.first > -1 && argminvar.second > -1);
        if (argminvar.first < 0 || argminvar.second < 0) {
          return;
        }
        // merge into a single cluster
        ForIndex(s,m_Samples.size()) {
          if (m_SampleOwner[s] == argminvar.second) {
            m_SampleOwner[s] = argminvar.first;
          }
        }
        // reassign other
        reassign(argminvar.second);
        // done
        std::cerr << "done." << std::endl;
      }

      /// reassign an empty cluster
      //  -> find cluster with highest variance
      //  -> find sample farthest from its center
      //  -> allocate empty cluster to it
      void reassign(uint emptyc)
      {
        // find cluster with highest variance
        float maxvar      = 0.0f;
        uint  maxc        = emptyc;
        int   maxfarthest = -1;
        ForIndex(c,m_NumClusters) {
          if (c == emptyc) {
            continue;
          }
          // compute var and sample farthest from center
          int   farthest = -1;
          float maxd     = 0.0f;
          float var      = 0.0f;
          uint  num      = 0;
          ForIndex(s,m_Samples.size()) {
            if (m_SampleOwner[s] != c) {
              continue;
            }
            float d = m_Distance.sqDistanceBetween(m_Samples[s],m_Centers[c]);
            if (d >= maxd) {
              maxd     = d;
              farthest = s;
            }
            var += d;
            num ++;
          }
          if (num > 0) {
            var /= float(num);
            // select as current max?
            if (var >= maxvar) {
              maxvar      = var;
              maxc        = c;
              maxfarthest = farthest;
            }
          }
        }
        sl_assert(maxfarthest >= 0);
        sl_assert(maxc != emptyc);
        sl_assert(m_SampleOwner[maxfarthest] == maxc);
        // allocate empty cluster to sample farthest from center
        m_Centers[emptyc]          = m_Samples[maxfarthest];
        m_SampleOwner[maxfarthest] = emptyc;
      }

      /// compute k-mean clustering from samples 
      bool computeForNumClusters(
        uint     num_clusters,
        float    stop_threshold   = 0.0f,
				float*   averageDistortion = NULL)
      {
      	using namespace LibSL::CppHelpers;

        LibSL::System::Time::Timer tm("Lloyd clustering");
        
        // allocate
        m_NumClusters = num_clusters;
        m_Centers    .allocate(m_NumClusters);
        m_Averager   .allocate(m_NumClusters);
        m_SampleOwner.allocate(uint(m_Samples.size()));

        m_SampleOwner.fill(-1);

        std::cerr << sprint("[LloydClustering] starting - target num clusters = %d, num samples = %d\n",num_clusters,m_Samples.size());

        // need more samples than clusters
        // (if not, caller does not need clustering!)
        sl_assert(m_Samples.size() >= m_NumClusters);

        // randomly chose samples to init clusters
        LibSL::Memory::Array::Array<int> chosen;
        chosen.allocate(m_NumClusters);
        chosen.fill(-1);
        ForIndex(c,m_NumClusters) {
          // init with a random point
          int s     = rand() % int(m_Samples.size());
          while (1) {
            // make sure 's' has not been already chosen
            // search if chosen before
            int i=0;
            for (;i<c;i++) {
              if (chosen[i] == s) {
                break;
              }
            }
            if (i == c) { // never chosen before
              chosen[c]=s;
              break;
            }
            // otherwise go to next
            s = (s+1) % int(m_Samples.size());
          } // while (1)
          // copy into center
          m_Centers[c] = m_Samples[s];
        }

        // iterate
        int niter        = 0;
        float prev_disto = 0.0f;
        CppHelpers::Console::pushCursor();
        while (1) {
          CppHelpers::Console::popCursor();
          CppHelpers::Console::pushCursor();
          // update clusters
          uint num_changes = distributeSamples();
          // teleport every 10 iterations
          if ((niter > 0) && (niter % 10 == 0)) {
            teleport();
          }
          // update centers
          updateCenters();
          // compute average distortion
          float avg_distortion = 0.0f;
          ForIndex(s,m_Samples.size()) {
            avg_distortion += m_Distance.sqDistanceBetween(m_Samples[s],m_Centers[m_SampleOwner[s]]);
          }
          avg_distortion = avg_distortion / float(m_Samples.size()*T_Sample::e_Size);
          // message
          std::cerr << sprint("[LloydClustering] iteration %d: %d changes, avg distortion = %.3f\n",niter++,num_changes,avg_distortion);
          // continue?
          if (abs(prev_disto-avg_distortion) <= stop_threshold && num_changes <= max(1,m_Samples.size()/100)) {
            break;
          }
          prev_disto = avg_distortion;
          if (num_changes == 0) {
            break;
          }
        }
        std::cerr << "done.\n";

				if (averageDistortion)
				{
					*averageDistortion = prev_disto;
				}

        // clean up
        m_Samples     .clear();
        
        // done
        return (true);
      }

      uint            numClusters()                const {return (m_NumClusters);}
      const T_Sample& clusterCenter(uint c)        const {sl_assert(c>=0 && c<m_Centers.size());     return (m_Centers[c]);}
      uint            sampleOwner(uint s)          const {sl_assert(s>=0 && s<m_SampleOwner.size()); return (m_SampleOwner[s]);}
      uint            numSamples()                 const {return m_SampleOwner.size();}

    };

  } // namespace LibSL::Math
} // namespace LibSL
