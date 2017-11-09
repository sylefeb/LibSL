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
// class LBGClustering
//  LBG clustering implementation
//
// USAGE:
//
//    See tests/test_lloyd.cpp
//
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-12-06
// ------------------------------------------------------

#pragma once

#include <LibSL/Memory/Array.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Matrix4x4.h>

#include <LibSL/Math/LloydClustering.h>

#include <list>
#include <vector>
#include <algorithm>

namespace LibSL {
  namespace Math {

    template <
      class T_Sample,
      class T_Distance = TupleDistance<T_Sample>,
      class T_Averager = TupleAverager<T_Sample> > 
    class LBGClustering
    {
    protected:

      /// for distance computations
      T_Distance                             m_Distance;
      /// target number of clusters
      uint                                   m_MaxNumClusters;
      /// current number of clusters
      uint                                   m_CurrentNumClusters;
      /// samples
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
      /// iteration average distortion
      float                                  m_AvgVar;

    public:

      /// constructor
      LBGClustering()
      {	
        m_MaxNumClusters  = 0;
        m_CurrentNumClusters = 0;
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
        ForIndex(c,m_CurrentNumClusters) {
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
        //LibSL::System::Time::Timer tm("distributeSamples");
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
        // -> compute averages
        ForIndex(c,m_CurrentNumClusters) {
          m_Averager[c].begin();
        }
        ForIndex(s,m_Samples.size()) {
          int o        = m_SampleOwner[s];
          sl_assert(o >= 0);
          m_Averager[o].add(m_Samples[s]);
        }
        ForIndex(c,m_CurrentNumClusters) {
          if (m_Averager[c].valid()) {
            m_Centers[c]  = m_Averager[c].end();
          } else {
            // randomly re-assign a sample
            m_Centers[c]  = m_Samples[rand() % m_Samples.size()];
          }
        }
      }

      /// find cluster of max var
      float maxVarCluster(int& _cluster)
      {
        // -> compute all variances
        Array<float> vars;
        Array<int>   nums;
        vars.allocate( m_CurrentNumClusters );
        nums.allocate( m_CurrentNumClusters );
        vars.fill( 0.0f );
        nums.fill( 0.0f );
        ForIndex(s,m_Samples.size()) {
          sl_assert( m_SampleOwner[s] < m_CurrentNumClusters );
          vars[ m_SampleOwner[s] ] += m_Distance.sqDistanceBetween( m_Samples[s] , m_Centers[ m_SampleOwner[s] ] );
          nums[ m_SampleOwner[s] ] ++;
        }
        ForIndex(c,m_CurrentNumClusters) {
          if (nums[c] > 0) {
            vars[c] = vars[c] / float( nums[c] * T_Sample::e_Size );
          }
        }
        // search argmax
        _cluster     = -1;
        float maxVar = 0.0f;
        ForIndex(c,m_CurrentNumClusters) {
          if (vars[c] > maxVar) {
            maxVar      = vars[c];
            _cluster    = c;
          }
        }
        sl_assert( _cluster > -1 );
        sl_assert( nums[ _cluster ] > 1 );
        return maxVar;
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
        ForIndex(c,m_CurrentNumClusters) {
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

      /// teleport
      void teleport()
      {
        // find pair of clusters that would result in a cluster with minimal variance
        float minvar = 0.0f;
        std::pair<int,int> argminvar = std::make_pair(-1,-1);
        ForIndex(c0,m_CurrentNumClusters) {
          for (uint c1=c0+1 ; c1<m_CurrentNumClusters ; c1++) {
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
      }

      /// split
      void split()
      {
        /*
        ForIndex(c,m_CurrentNumClusters) {
        m_Centers[c+m_CurrentNumClusters] = (1.0f+epsilon) * m_Centers[ c ];
        m_Centers[c                     ] = (1.0f-epsilon) * m_Centers[ c ];
        }
        m_CurrentNumClusters *= 2;
        */
        // split
        m_CurrentNumClusters ++;
        reassign( m_CurrentNumClusters-1 );
      }

      /// compute k-mean clustering from samples 
      bool computeForNumClusters(
        uint  min_num_clusters,
        uint  max_num_clusters,
        float stop_avg_var,
        float stop_threshold   = 0.0f
        )
      {
        LibSL::System::Time::Timer tm("LBG clustering");

        // allocate
        m_MaxNumClusters  = max_num_clusters;
        m_Centers    .allocate(m_MaxNumClusters);
        m_Averager   .allocate(m_MaxNumClusters);
        m_SampleOwner.allocate(uint(m_Samples.size()));
        m_SampleOwner.fill( 0 );

        cerr << sprint("[LBGClustering] starting - max num clusters = %d, num samples = %d     \n",
          max_num_clusters,m_Samples.size());

        // initial number of clusters
        m_CurrentNumClusters = min_num_clusters;

        // randomly chose samples to init clusters
        LibSL::Memory::Array::Array<int> chosen;
        chosen.allocate(m_CurrentNumClusters);
        chosen.fill(-1);
        ForIndex(c,m_CurrentNumClusters) {
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

        // average distortion
        m_AvgVar = 0.0f;
        ForIndex(s,m_Samples.size()) {
          m_AvgVar += m_Distance.sqDistanceBetween(m_Samples[s],m_Centers[0]);
        }
        m_AvgVar = m_AvgVar / float(m_Samples.size()*T_Sample::e_Size);

        // iterate
        Console::pushCursor();
        int niter = 0;
        while (1) {
          Console::popCursor();
          Console::pushCursor();
          // test variance
          int   c      = -1;
          cerr << sprint("[LBGClustering] split,  %d clusters                         \n",
            m_CurrentNumClusters);
          // stabalize
          int nsubiter = 0;
          while (1) {
            nsubiter ++;
            Console::popCursor();
            Console::pushCursor();
            // distribute samples
            uint num_changes = distributeSamples(); 
            // update centers
            updateCenters();
            // compute average variance
            float avgvar = 0.0f;
            ForIndex(s,m_Samples.size()) {
              avgvar += m_Distance.sqDistanceBetween(m_Samples[s],m_Centers[m_SampleOwner[s]]);
            }
            avgvar = avgvar / float(m_Samples.size()*T_Sample::e_Size);
            // message
            cout << sprint("[LBGClustering] iteration %d: %d clusters, %d changes, avg var = %.3f   \n",
              niter++,m_CurrentNumClusters,num_changes,avgvar);
            // done with this number of clusters?
            if ( num_changes == 0 || abs(m_AvgVar - avgvar) <= stop_threshold /* && num_changes <= m_Samples.size()/100 */ ) {
              m_AvgVar = avgvar;
              break;
            }
            m_AvgVar = avgvar;
          }
          if (m_CurrentNumClusters == m_MaxNumClusters) {
            break;
          }
          if (m_AvgVar < stop_avg_var) {
            break;
          }
          // teleport
          teleport();
          // split
          split();
        }
        cerr << "done.\n";

        // clean up
        m_Samples     .clear();

        // done
        return (true);
      }

      uint            numClusters()                const {return (m_CurrentNumClusters);}
      uint            numSamples()                 const {return (m_SampleOwner.size());}
      const T_Sample& clusterCenter(uint c)        const {sl_assert(c>=0 && c<m_Centers.size());     return (m_Centers[c]);}
      uint            sampleOwner(uint s)          const {sl_assert(s>=0 && s<m_SampleOwner.size()); return (m_SampleOwner[s]);}

    };

  } // namespace LibSL::Math
} // namespace LibSL
