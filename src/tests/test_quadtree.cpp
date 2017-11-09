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
#include <LibSL/LibSL_gl.h>

#include <iostream>
#include <ctime>

using namespace std;

static int g_WinWidth =512;
static int g_WinHeight=512;

typedef struct
{
  AASquare m_Square;
} t_Node;

AutoPtr<QuadTree<t_Node> > g_QTree;

Tex2DLum_Ptr g_Tex;

// --------------------------------------------------------

static void mainKeyboard(unsigned char key) 
{
  if (key == 'q') {
    g_Tex   = NULL;
    g_QTree = NULL;
    SimpleUI::exit();
  }
}

// --------------------------------------------------------

static void mainOnMouseButtonPressed(uint x,uint y,uint btn,uint flags)
{

}

// --------------------------------------------------------

static void mainOnMouseMotion(uint x,uint y)
{

}

// --------------------------------------------------------

void drawQuadtree(QuadTree<t_Node>::t_Pointer tree);

static void mainRender()
{
  // clear screen
  GPUHelpers::clearScreen(LIBSL_COLOR_BUFFER | LIBSL_DEPTH_BUFFER,
    0,0,0);
  
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glColor3f(1,1,1);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,g_Tex->texture());
  glBegin(GL_QUADS);
  glTexCoord2f(0,0);
  glVertex2f(0,0);
  glTexCoord2f(1,0);
  glVertex2f(1,0);
  glTexCoord2f(1,1);
  glVertex2f(1,1);
  glTexCoord2f(0,1);
  glVertex2f(0,1);
  glEnd();

  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glDisable(GL_TEXTURE_2D);
  glColor3f(0,0,1);
  drawQuadtree(g_QTree.raw());
}

// --------------------------------------------------------

void subdivide(QuadTree<t_Node>::t_Pointer node,const Tex2DLum::t_PixelArray& img)
{
  v2f   m = node->data().m_Square.minCorner();
  v2f   M = node->data().m_Square.maxCorner();
  uint sx = LibSL::Math::min(uint(64),uint(m[0]*64.0));
  uint ex = LibSL::Math::min(uint(64),uint(M[0]*64.0));
  uint sy = LibSL::Math::min(uint(64),uint(m[1]*64.0));
  uint ey = LibSL::Math::min(uint(64),uint(M[1]*64.0));
  uint w  = ex-sx;
  uint h  = ey-sy;
  ForIndex(cj,2) {
    ForIndex(ci,2) {
      bool whiteFound = false;
      for (uint j = (cj==0? sy : sy+h/2 ) ; j < (cj==0 ? sy+h/2 : sy+h) ; j++) {
        for (uint i = (ci==0? sx : sx+w/2 ) ; i < (ci==0 ? sx+w/2 : sx+w) ; i++) {
          if (img.at(i,j)[0] == 255) {
            whiteFound = true;
          }
        }
      }
      if (whiteFound && w > 1 && h > 1) {
        t_Node child;
        child.m_Square = AASquare(
          m+V2F(ci,cj)*0.5f*(M-m),
          m+V2F(ci,cj)*0.5f*(M-m)+0.5f*(M-m));
        node->addChild(Pair(ci,cj),child);
        subdivide(node->childAt(Pair(ci,cj)),img);
      }
    }
  }

}

// --------------------------------------------------------

void drawQuadtree(QuadTree<t_Node>::t_Pointer node)
{
  v2f   m = node->data().m_Square.minCorner();
  v2f   M = node->data().m_Square.maxCorner();

  glBegin(GL_QUADS);
  glVertex2f(m[0],m[1]);
  glVertex2f(M[0],m[1]);
  glVertex2f(M[0],M[1]);
  glVertex2f(m[0],M[1]);
  glEnd();

  ForIndex(j,2) {
    ForIndex(i,2) {
      if (!node->childAt(Pair(i,j)).isNull()) {
        drawQuadtree(node->childAt(Pair(i,j)));
      }
    }
  }
}

// --------------------------------------------------------

void test_quadtree()
{
  try {
    
    /// init simple UI (glut clone for both GL and D3D)
    SimpleUI::onRender             = mainRender;
    SimpleUI::onKeyPressed         = mainKeyboard;
    SimpleUI::onMouseButtonPressed = mainOnMouseButtonPressed;
    SimpleUI::onMouseMotion        = mainOnMouseMotion;
    SimpleUI::init(g_WinWidth,g_WinHeight);
    
    /// help
    printf("[q]     - quit\n");
    
    /// gl init
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    Transform::ortho2D(LIBSL_PROJECTION_MATRIX,0,1,0,1);
    Transform::identity(LIBSL_MODELVIEW_MATRIX);

    Tex2DLum::t_PixelArray img(64,64);
    img.fill(0);
    ForIndex(n,32) {
      uint x = rand() & 63;
      uint y = rand() & 63;
      img.at(x,y) = 255;
    }

    t_Node root;
    root.m_Square.addPoint(V2F(0,0));
    root.m_Square.addPoint(V2F(1,1));
    g_QTree = new QuadTree<t_Node>(root);

    subdivide(g_QTree.raw(),img);
    cerr << "tree number of nodes  = " << g_QTree->numNodes() << endl;
    cerr << "tree number of leaves = " << g_QTree->numLeaves() << endl;

    g_Tex   = new Tex2DLum(img);

    /// main loop
    SimpleUI::loop();

    // shutdown SimpleUI
    SimpleUI::shutdown();
  
  } catch (Fatal& e) {
    cerr << e.message() << endl;
  }

}

/* -------------------------------------------------------- */
