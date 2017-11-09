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
// ------------------------------------------------------

#include "ImplicitShape.h"

#define NAMESPACE LibSL::Geometry

// ------------------------------------------------------

NAMESPACE::ImplicitShape::~ImplicitShape()
{
  if (!m_MC.isNull()) {
    m_MC->clean_temps();
    m_MC->clean_all();
  }
}

// ------------------------------------------------------

void NAMESPACE::ImplicitShape::initGrid()
{
  float x,y,z;
  float sx,sy,sz;
//  float tx,ty,tz;

  sx     = (float)m_MC->size_x();
  sy     = (float)m_MC->size_y();
  sz     = (float)m_MC->size_z();
  std::cerr << "Sampling function ... ";
  Console::progressTextInit(m_MC->size_z());
  ForIndex(k,m_MC->size_z()) {
    Console::progressTextUpdate();
    z = ( (float) k ) / (sz-1) ;
    z = m_Box.minCorner()[2] + z * (m_Box.maxCorner()[2] - m_Box.minCorner()[2]);
    ForIndex(j,m_MC->size_y()) {
      y = ( (float) j ) / (sy-1) ;
      y = m_Box.minCorner()[1] + y * (m_Box.maxCorner()[1] - m_Box.minCorner()[1]);
      ForIndex(i,m_MC->size_x()) {
        x = ( (float) i ) / (sx-1) ;
        x = m_Box.minCorner()[0] + x * (m_Box.maxCorner()[0] - m_Box.minCorner()[0]);
        m_MC->set_data( m_Implicit(V3F(x,y,z)), i,j,k );
      }
    }
  }
  Console::progressTextEnd();
  std::cerr << std::endl;
}

// ------------------------------------------------------

void NAMESPACE::ImplicitShape::init()
{
  m_MC = AutoPtr<NAMESPACE::MarchingCubes::MarchingCubes>(new NAMESPACE::MarchingCubes::MarchingCubes());
  m_MC->set_resolution( m_Resolution,m_Resolution,m_Resolution );
  m_MC->init_all();
  initGrid();
}

// ------------------------------------------------------

NAMESPACE::ImplicitShape::t_Mesh *NAMESPACE::ImplicitShape::generateShape(float iso)
{
  if (m_MC.isNull()) {
    init();
  }

  m_MC->restart();

  m_MC->run(iso);

  if (m_MC->nverts() == 0 || m_MC->ntrigs() == 0) {
    return NULL;
  }

  t_Mesh *mesh = new t_Mesh( m_MC->nverts(),m_MC->ntrigs() );
  
  float sx    = (float)m_MC->size_x();
  float sy    = (float)m_MC->size_y();
  float sz    = (float)m_MC->size_z();
  ForIndex(i,m_MC->nverts()) {
		float x = m_MC->vertices()[i].x;
		float y = m_MC->vertices()[i].y;
		float z = m_MC->vertices()[i].z;
    mesh->vertexAt(i).pos = V3F(x,y,z) * (m_Box.maxCorner() - m_Box.minCorner()) / V3F(sx-1,sy-1,sz-1) + m_Box.minCorner();
    mesh->vertexAt(i).nrm = 0;
    mesh->vertexAt(i).uv  = 0;
	}

  ForIndex(i,m_MC->ntrigs()) {
		v3u tri;
    tri[0] = m_MC->triangles()[i].v1;
		tri[1] = m_MC->triangles()[i].v2;
		tri[2] = m_MC->triangles()[i].v3;
	  mesh->triangleAt(i) = tri;
  }

  // compute normals
  FastArray<v3f> nrms(mesh->numVertices());
  nrms.fill(0);
  ForIndex(t,mesh->numTriangles()) {
    int vids[3];
    v3f pts[3];
    ForIndex(i,3) {
      vids[i] = mesh->triangleAt(t)[i];
      pts[i]  = mesh->vertexAt(vids[i]).pos;
    }
    v3f n = normalize_safe( cross(pts[1]-pts[0],pts[2]-pts[0]) );
    ForIndex(i,3) {
      nrms[vids[i]] += n;
    }
  }
  ForIndex(v,mesh->numVertices()) {
    mesh->vertexAt(v).nrm = normalize_safe(nrms[v]);
  }

  return mesh;
}

// ------------------------------------------------------
