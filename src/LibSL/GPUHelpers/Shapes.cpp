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
// Sylvain Lefebvre - 2007-02-16
//-------------------------------------------------------
#include "LibSL.precompiled.h"
// ------------------------------------------------------

#include "Shapes.h"

// ------------------------------------------------------

#define NAMESPACE LibSL::GPUHelpers::Shapes

// ------------------------------------------------------

uint   NAMESPACE::Box   ::s_UsageCounter = 0;
uint   NAMESPACE::Square::s_UsageCounter = 0;

static NAMESPACE::TexturedShape *s_Box        = NULL;
static NAMESPACE::TexturedShape *s_Square     = NULL;

// ------------------------------------------------------
// Box
// ------------------------------------------------------

static void createBox()
{
  sl_assert(s_Box == NULL);
  s_Box = new NAMESPACE::TexturedShape();
  s_Box->begin(GPUMESH_TRIANGLELIST);

  s_Box->normal(0,-1,0);

  s_Box->texcoord0_4(1,1, 0,1);
  s_Box->vertex_3   (1,0,1); //2
  s_Box->texcoord0_4(0,0, 0,1);
  s_Box->vertex_3   (0,0,0); //0
  s_Box->texcoord0_4(1,0, 0,1);
  s_Box->vertex_3   (1,0,0); //1

  s_Box->texcoord0_4(0,1, 0,1);
  s_Box->vertex_3   (0,0,1); //3
  s_Box->texcoord0_4(0,0, 0,1);
  s_Box->vertex_3   (0,0,0); //0
  s_Box->texcoord0_4(1,1, 0,1);
  s_Box->vertex_3   (1,0,1); //2

  ////

  s_Box->normal(0, 1,0);

  s_Box->texcoord0_4(1,0, 0,1);
  s_Box->vertex_3   (1,1,0); //5
  s_Box->texcoord0_4(0,0, 0,1);
  s_Box->vertex_3   (0,1,0); //4
  s_Box->texcoord0_4(1,1, 0,1);
  s_Box->vertex_3   (1,1,1); //6

  s_Box->texcoord0_4(1,1, 0,1);
  s_Box->vertex_3   (1,1,1); //6
  s_Box->texcoord0_4(0,0, 0,1);
  s_Box->vertex_3   (0,1,0); //4
  s_Box->texcoord0_4(0,1, 0,1);
  s_Box->vertex_3   (0,1,1); //7

  ////

  s_Box->normal( 1,0,0);

  s_Box->texcoord0_4(0,1, 0,1);
  s_Box->vertex_3   (1,0,1); //2
  s_Box->texcoord0_4(0,0, 0,1);
  s_Box->vertex_3   (1,0,0); //1
  s_Box->texcoord0_4(1,1, 0,1);
  s_Box->vertex_3   (1,1,1); //6

  s_Box->texcoord0_4(1,1, 0,1);
  s_Box->vertex_3   (1,1,1); //6
  s_Box->texcoord0_4(0,0, 0,1);
  s_Box->vertex_3   (1,0,0); //1
  s_Box->texcoord0_4(1,0, 0,1);
  s_Box->vertex_3   (1,1,0); //5

  ////

  s_Box->normal(-1,0,0);

  s_Box->texcoord0_4(1,0, 0,1);
  s_Box->vertex_3   (0,1,0); //4
  s_Box->texcoord0_4(0,0, 0,1);
  s_Box->vertex_3   (0,0,0); //0
  s_Box->texcoord0_4(0,1, 0,1);
  s_Box->vertex_3   (0,0,1); //3

  s_Box->texcoord0_4(1,0, 0,1);
  s_Box->vertex_3   (0,1,0); //4
  s_Box->texcoord0_4(0,1, 0,1);
  s_Box->vertex_3   (0,0,1); //3
  s_Box->texcoord0_4(1,1, 0,1);
  s_Box->vertex_3   (0,1,1); //7

  ////

  s_Box->normal(0,0,-1);

  s_Box->texcoord0_4(1,0, 0,1);
  s_Box->vertex_3   (1,0,0); //1
  s_Box->texcoord0_4(0,0, 0,1);
  s_Box->vertex_3   (0,0,0); //0
  s_Box->texcoord0_4(1,1, 0,1);
  s_Box->vertex_3   (1,1,0); //5

  s_Box->texcoord0_4(1,1, 0,1);
  s_Box->vertex_3   (1,1,0); //5
  s_Box->texcoord0_4(0,0, 0,1);
  s_Box->vertex_3   (0,0,0); //0
  s_Box->texcoord0_4(0,1, 0,1);
  s_Box->vertex_3   (0,1,0); //4

  ////

  s_Box->normal(0,0, 1);

  s_Box->texcoord0_4(1,1, 0,1);
  s_Box->vertex_3   (1,1,1); //6
  s_Box->texcoord0_4(0,0, 0,1);
  s_Box->vertex_3   (0,0,1); //3
  s_Box->texcoord0_4(1,0, 0,1);
  s_Box->vertex_3   (1,0,1); //2

  s_Box->texcoord0_4(0,1, 0,1);
  s_Box->vertex_3   (0,1,1); //7
  s_Box->texcoord0_4(0,0, 0,1);
  s_Box->vertex_3   (0,0,1); //3
  s_Box->texcoord0_4(1,1, 0,1);
  s_Box->vertex_3   (1,1,1); //6

  s_Box->end();
}

// ------------------------------------------------------

void        NAMESPACE::Box::lock()
{
  if (s_UsageCounter == 0) {
    createBox();
  }
  sl_assert(s_Box != NULL);
  s_UsageCounter ++;
}

// ------------------------------------------------------

void        NAMESPACE::Box::unlock()
{
  sl_assert(s_UsageCounter > 0);
  s_UsageCounter --;
  if (s_UsageCounter == 0) {
    delete (s_Box); s_Box = NULL;
  }
}

// ------------------------------------------------------

void   NAMESPACE::Box::render()
{
  s_Box->render();
}

// ------------------------------------------------------
// Square
// ------------------------------------------------------

static void createSquare()
{
  sl_assert(s_Square == NULL);
  s_Square = new NAMESPACE::TexturedShape();
  s_Square->begin(GPUMESH_TRIANGLELIST);

  s_Square->normal     ( 0, 0, 1);
  
  s_Square->texcoord0_4( 0, 1, 0, 1);
  s_Square->vertex_3   (-1,-1, 0);
  s_Square->texcoord0_4( 1, 0, 0, 1);
  s_Square->vertex_3   ( 1, 1, 0);
  s_Square->texcoord0_4( 1, 1, 0, 1);
  s_Square->vertex_3   ( 1,-1, 0);
  
  s_Square->texcoord0_4( 0, 1, 0, 1);
  s_Square->vertex_3   (-1,-1, 0);
  s_Square->texcoord0_4( 0, 0, 0, 1);
  s_Square->vertex_3   (-1, 1, 0);
  s_Square->texcoord0_4( 1, 0, 0, 1);
  s_Square->vertex_3   ( 1, 1, 0);

  s_Square->end();
}

// ------------------------------------------------------

void        NAMESPACE::Square::lock()
{
  if (s_UsageCounter == 0) {
    createSquare();
  }
  sl_assert(s_Square != NULL);
  s_UsageCounter ++;
}

// ------------------------------------------------------

void        NAMESPACE::Square::unlock()
{
  sl_assert(s_UsageCounter > 0);
  s_UsageCounter --;
  if (s_UsageCounter == 0) {
    delete (s_Square); s_Square = NULL;
  }
}

// ------------------------------------------------------

void   NAMESPACE::Square::render()
{
  s_Square->render();
}

// ------------------------------------------------------
// Polygon
// ------------------------------------------------------

NAMESPACE::Polygon::Polygon(const Geometry::Polygon<3,v3f>& poly)
{
  m_Shape = AutoPtr<NAMESPACE::Shape>(new NAMESPACE::Shape());

  m_Shape->begin(GPUMESH_TRIANGLELIST);
  v3f n   = poly.normal();
  m_Shape->normal(n[0],n[1],n[2]);
  v3f p0  = poly.vertexAt(0);
  for (uint i=1;i<poly.size();i++) {
    v3f pi   = poly.vertexAt(i);
    v3f pip1 = poly.vertexAt((i+1)%poly.size());
    m_Shape->vertex_3(p0  [0],p0  [1],p0  [2]);
    m_Shape->vertex_3(pi  [0],pi  [1],pi  [2]);
    m_Shape->vertex_3(pip1[0],pip1[1],pip1[2]);
  }
  m_Shape->end();
}
  
// ------------------------------------------------------

void  NAMESPACE::Polygon::render()
{
  m_Shape->render();
}

// ------------------------------------------------------
// Grid
// ------------------------------------------------------

NAMESPACE::Grid::Grid(const LibSL::Memory::Array::Array2D<std::pair<v3f,v3f> >& grid)
{
  m_Shape = AutoPtr<NAMESPACE::IndexedShape>(new NAMESPACE::IndexedShape());

  m_Shape->begin(GPUMESH_TRIANGLELIST);

  ForArray2D(grid,i,j) {
    v3f p = grid.at(i  ,j  ).first;
    v3f n = grid.at(i  ,j  ).second;
    m_Shape->normal  (n[0],n[1],n[2]);
    m_Shape->vertex_3(p[0],p[1],p[2]);
  }

  for (uint j=0;j<grid.ysize()-1;j++) {
    for (uint i=0;i<grid.xsize()-1;i++) {
      m_Shape->index((i  )+(j  )*grid.xsize());
      m_Shape->index((i+1)+(j  )*grid.xsize());
      m_Shape->index((i+1)+(j+1)*grid.xsize());

      m_Shape->index((i  )+(j  )*grid.xsize());
      m_Shape->index((i+1)+(j+1)*grid.xsize());
      m_Shape->index((i  )+(j+1)*grid.xsize());
    }
  }

  m_Shape->end();
}
  
// ------------------------------------------------------

void  NAMESPACE::Grid::render()
{
  m_Shape->render();
}

// ------------------------------------------------------
