/* -------------------------------------------------------- */

#include <iostream>
#include <ctime>
#include <cmath>
#include <cstring>

using namespace std;

/* -------------------------------------------------------- */

#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>
#include <LibSL/LibSL_linalg.h>

LIBSL_WIN32_FIX;

/* -------------------------------------------------------- */

uint         g_W     = 0;
uint         g_H     = 0;
uint         g_InitW = 0;
uint         g_InitH = 0;

ImageRGB_Ptr  g_Img;
Tex2DRGBA_Ptr g_Tex;
Array2D<v2f>  g_Grid;

/* -------------------------------------------------------- */

void solve()
{

    // build system
    LinearSystem ls;
    cerr << "building system ... ";

    const double w_soft   = 1e-6; // weight for soft constraints
    const double w_rigid  = 1.0;  // weight for rigid constraints
    const double w_strict = 1e4; // weight for positional constraints

    int numEqns = 2 * g_Grid.xsize() * g_Grid.ysize() // eqns for each point
                + 2 * 4                               // corners
                + 2 * (g_Grid.xsize() - 2)            // horiz. border
                + 2 * (g_Grid.ysize() - 2);           // vert. border
    int numVars = 2 * g_Grid.xsize() * g_Grid.ysize(); // x,y for each point
    ls.allocate( numEqns , numVars );

    // interval between two grid nodes at rest
    float xstep = float(g_InitW) / float(g_Grid.xsize()-1);
    float ystep = float(g_InitH) / float(g_Grid.ysize()-1);

    int eqn = 0;
    LIBSL_BEGIN;
    // equations for inside
    ForArray2D(g_Grid,i,j) {
      int id  = i + j * g_Grid.xsize();
      ls.b()[ eqn   ]            = 0.0;
      ls.b()[ eqn+1 ]            = 0.0;
      float numNeigh = 0.0;
      ForRange(nj,-1,1) {
        ForRange(ni,-1,1) {
          // cross neighborhood excl. center
          if (ni == 0 && nj == 0)    continue;
          if (abs(ni) + abs(nj) > 1) continue;
          // check if outside
          if (i+ni < 0 || i+ni >= g_Grid.xsize()
            ||j+nj < 0 || j+nj >= g_Grid.ysize() ) {
              continue;
          }
          // rigidity
          float r = g_Img->bilinear(
            (i+ni*0.5f+0.5f)/(float)g_Img->w(),
            (j+nj*0.5f+0.5f)/(float)g_Img->h())[0] / 255.0f;
          float w = w_soft * (1.0f-r) + w_rigid * r;
          // write equation
          int nid  = (i + ni) + (j + nj) * g_Grid.xsize();
          // x
          ls.coeff( eqn   , id *2   ) -= w;
          ls.coeff( eqn   , nid*2   ) += 1.0 * w;
          ls.b()  [ eqn   ]           += ni * xstep * w;
          // y
          ls.coeff( eqn+1 , id *2+1 ) -= w;
          ls.coeff( eqn+1 , nid*2+1 ) += 1.0 * w;
          ls.b()  [ eqn+1 ]           += nj * ystep * w;
          sl_assert(nid*2+1 < numVars);
        }
      }
      eqn += 2;
    }
    LIBSL_END;

    LIBSL_BEGIN;
    // postional constraints
    // -> corners
    int id00  =   0 +   0 * g_Grid.xsize();
    ls.coeff( eqn   , id00*2   ) =   1.0 * w_strict; // x
    ls.coeff( eqn+1 , id00*2+1 ) =   1.0 * w_strict; // y
    ls.b()  [ eqn   ]            =   0.0 * w_strict;
    ls.b()  [ eqn+1 ]            =   0.0 * w_strict;
    eqn += 2;

    int id10  = (g_Grid.xsize()-1) +   0 * g_Grid.xsize();
    ls.coeff( eqn   , id10*2   ) =   1.0 * w_strict; // x
    ls.coeff( eqn+1 , id10*2+1 ) =   1.0 * w_strict; // y
    ls.b()  [ eqn   ]            =   g_W * w_strict;
    ls.b()  [ eqn+1 ]            =   0.0 * w_strict;
    eqn += 2;

    int id11  = (g_Grid.xsize()-1) + (g_Grid.ysize()-1) * g_Grid.xsize();
    ls.coeff( eqn   , id11*2   ) =   1.0 * w_strict; // x
    ls.coeff( eqn+1 , id11*2+1 ) =   1.0 * w_strict; // y
    ls.b()  [ eqn   ]            =   g_W * w_strict;
    ls.b()  [ eqn+1 ]            =   g_H * w_strict;
    sl_assert(id11*2+1 < numVars);
    eqn += 2;

    int id01  = 0 + (g_Grid.ysize()-1) * g_Grid.xsize();
    ls.coeff( eqn   , id01*2   ) =   1.0 * w_strict; // x
    ls.coeff( eqn+1 , id01*2+1 ) =   1.0 * w_strict; // y
    ls.b()  [ eqn   ]            =   0.0 * w_strict;
    ls.b()  [ eqn+1 ]            =   g_H * w_strict;
    eqn += 2;

    // -> horiz. border
    ForRange(i,1,g_Grid.xsize()-2) {
      int t = i +                 0  * g_Grid.xsize();
      int b = i + (g_Grid.ysize()-1) * g_Grid.xsize();
      ls.coeff( eqn   , t*2+1 ) =   1.0 * w_strict; // y == 0
      ls.b()  [ eqn   ]         =   0.0 * w_strict;
      eqn ++;
      ls.coeff( eqn   , b*2+1 ) =   1.0 * w_strict; // y == g_H
      ls.b()  [ eqn   ]         =   g_H * w_strict;
      eqn ++;
    }
    
    // -> vert. border
    ForRange(j,1,g_Grid.ysize()-2) {
      int l =                  0 + j * g_Grid.xsize();
      int r = (g_Grid.xsize()-1) + j * g_Grid.xsize();
      ls.coeff( eqn   , l*2   ) =   1.0 * w_strict; // x == 0
      ls.b()  [ eqn   ]         =   0.0 * w_strict;
      eqn ++;
      ls.coeff( eqn   , r*2   ) =   1.0 * w_strict; // x == g_W
      ls.b()  [ eqn   ]         =   g_W * w_strict;
      eqn ++;
    }

    LIBSL_END;

    sl_assert(eqn == numEqns);
    
    cerr << "done.\n"; 

    // solve
    Array<double> vars( numVars );
    cerr << endl;
    cerr << "solving ... ";
    ls.prepareSolver();   
    ls.solve(vars);
    cerr << "done.\n";

    // store solution
    LIBSL_BEGIN;
    ForIndex(v,vars.size()) {
      int i = (v/2) % g_Grid.xsize();
      int j = (v/2) / g_Grid.xsize();
      if (v & 1) {
        g_Grid.at(i,j)[1] = vars[v]; // y 
      } else {
        g_Grid.at(i,j)[0] = vars[v]; // x
      }
    }
    LIBSL_END;

}

/* -------------------------------------------------------- */

void mainRender()
{
  glViewport(0,0,g_W,g_H);
  glClear   (GL_COLOR_BUFFER_BIT);

  Transform::ortho2D (LIBSL_PROJECTION_MATRIX,0,g_W,0,g_H);
  Transform::identity(LIBSL_MODELVIEW_MATRIX);

  // draw grid
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glColor3f    (1,1,1);
  glEnable     (GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,g_Tex->texture());
  glBegin      (GL_QUADS);
  ForRange(j,0,g_Grid.ysize()-2) {
    ForRange(i,0,g_Grid.xsize()-2) {
      float fi0 =  i    / float(g_Grid.xsize()-1);
      float fi1 = (i+1) / float(g_Grid.xsize()-1);
      float fj0 =  j    / float(g_Grid.ysize()-1);
      float fj1 = (j+1) / float(g_Grid.ysize()-1);
      glTexCoord2f(fi0,fj0); glVertex2fv(&g_Grid.at(i  ,j  )[0]);
      glTexCoord2f(fi1,fj0); glVertex2fv(&g_Grid.at(i+1,j  )[0]);
      glTexCoord2f(fi1,fj1); glVertex2fv(&g_Grid.at(i+1,j+1)[0]);
      glTexCoord2f(fi0,fj1); glVertex2fv(&g_Grid.at(i  ,j+1)[0]);
    }
  }
  glEnd();

}

/* -------------------------------------------------------- */

void mainReshape(uint w,uint h)
{
  g_W = w;
  g_H = h;
  if (g_InitW == 0) {
    g_InitW = g_W;
    g_InitH = g_H;
  }
}

/* -------------------------------------------------------- */

void mainKeypressed(uchar k)
{
  if (k == 'q') {
    SimpleUI::exit();
  } else if (k == ' ') {
    if (!g_Grid.empty()) {
      solve();
    }
  }
}

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  try {

    SimpleUI::onRender     = mainRender;
    SimpleUI::onReshape    = mainReshape;
    SimpleUI::onKeyPressed = mainKeypressed;
    SimpleUI::init(256,256);

    glDisable   (GL_DEPTH_TEST);
    glDisable   (GL_LIGHTING);
    glDisable   (GL_CULL_FACE);
    glClearColor(0,0,1,0);

    // load image and init grid
    g_Img = loadImage<ImageRGB>("grid.png");
    g_Tex = new Tex2DRGBA(g_Img->cast<ImageRGBA>()->pixels(),GPUTEX_AUTOGEN_MIPMAP);

    g_Grid.allocate(g_Img->w(),g_Img->h());
    solve();

    SimpleUI::loop();

    // clean-up GL objects
    g_Tex = NULL;

    SimpleUI::shutdown();

  } catch (Fatal& e) {
    cerr << Console::red << e.message() << Console::gray << endl;
    return (-1);
  }

  return (0);
}

/* -------------------------------------------------------- */
