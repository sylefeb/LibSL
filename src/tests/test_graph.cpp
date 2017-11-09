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
#include "precompiled.h"

#include <LibSL/LibSL.h>
#include <LibSL/DataStructures/GraphRndCut.h>

// -----------

#include <iostream>
using namespace std;

// -----------

class EdgeData
{
public:
  float m_Cost;
  EdgeData()        { m_Cost = 1.0f; }
  EdgeData(float c) { m_Cost = c; }
};

class NodeData
{
};

typedef Graph<NodeData,EdgeData,true> t_Graph;

class MyEdgeCost
{
public:
  float operator()(const t_Graph& graph,t_NodeId current,t_EdgeId edge) const 
  { 
    return graph.edges()[edge].data().m_Cost;
  }
};

// -----------

void test_graph()
{
  cerr << "---------------------------" << endl;
  cerr << " LibSL::DataStructures::Graph " << endl;
  cerr << "---------------------------" << endl;

  ImageRGB_Ptr img = loadImage<ImageRGB>("media/map.png");

  t_Graph g;
  ForImage(img,i,j) {
    g.addNode(NodeData());
  }

  ForImage(img,i,j) {
    if (i < img->w()-1 && j < img->h()-1) {
      if (img->pixel(i,j)[0] == 0 && img->pixel(i+1,j)[0] == 0) {
        g.addEdge(i+j*img->w()    , (i+1)+j*img->w() , EdgeData(1.0f));
        g.addEdge((i+1)+j*img->w(), i+j*img->w()     , EdgeData(1.0f));
      }
      if (img->pixel(i,j)[0] == 0 && img->pixel(i,j+1)[0] == 0) {
        g.addEdge(i+j*img->w()    , i+(j+1)*img->w() , EdgeData(1.0f));
        g.addEdge(i+(j+1)*img->w(), i+j*img->w()     , EdgeData(1.0f));
      }
    }
  }

  vector<v2i> path;
  uint x = img->w()-2;
  uint y = img->h()-2;

  GraphAlgorithms::ShortestPaths<t_Graph,MyEdgeCost> sp;
  float cost = sp.dijkstraToTarget( g,0,x+y*img->w(), path );

  cerr << "path is " << path.size() << " node(s) long, cost = " << cost << endl;

  ForIndex(n,path.size()) {
    uint i = path[n][0] % img->w();
    uint j = path[n][0] / img->w();
    img->pixel(i,j)[0] = 0;
    img->pixel(i,j)[1] = 255;
    img->pixel(i,j)[2] = 0;
  }
  saveImage("path.png",img);

  //// cut

  GraphAlgorithms::RndCut<t_Graph,MyEdgeCost> ct;
  Histogram h;
  int N = 200;
  cerr << "Running " << N << " randomized cuts ";
  { 
    Timer tm("[all cuts]"); 
    Console::progressBarInit(N);
    double best     = (1<<30);
    ForIndex(n,N) {
      Console::progressBarUpdate();
      time_t seed = milliseconds();
      srand( (uint)seed );
      Array<int> sides;
      ct.cut( g, 0,x+y*img->w(), sides);
      double ccost = ct.cutCost();
      h << (int)ccost;
      if (ccost < best) {
        best = ccost;
        // produce cut image
        img  = loadImage<ImageRGB>("media/map.png");
        ForImage(img,i,j) {
          t_NodeId n = i+j*img->w();
          if (sides[n] == GraphAlgorithms::Source) {
            img->pixel(i,j)[0] = 0;
            img->pixel(i,j)[1] = 255;
            img->pixel(i,j)[2] = 0;
          } else if (sides[n] == GraphAlgorithms::Disconnected) {
            img->pixel(i,j)[0] = 255;
            img->pixel(i,j)[1] = 0;
            img->pixel(i,j)[2] = 0;
          }
        }
        saveImage("cut.png",img);
      }
    }
    Console::progressBarEnd();
  }
  cerr << endl;
  h.print();

  cerr << "---------------------------" << endl;

}

// -----------
