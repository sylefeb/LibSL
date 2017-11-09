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
// LibSL::Image::PushPull
// ------------------------------------------------------
//
// PushPull algorithm - CPU version
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2010-06-24
// ------------------------------------------------------

#pragma once

#include <LibSL/System/Types.h>
#include <LibSL/Memory/Pointer.h>
#include <LibSL/Memory/Array2D.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Math.h>
#include <LibSL/Memory/TraceLeaks.h>
#include <LibSL/Memory/Pointer.h>
#include <LibSL/Image/Image.h>
#include <LibSL/Geometry/Plane.h>

// ------------------------------------------------------

namespace LibSL {
  namespace Image {

    using namespace Math;    

    template<class T_Image> T_Image *
    gradientPaste(typename T_Image::t_AutoPtr dest,int x,int y,typename T_Image::t_AutoPtr src)
    {
      // gradient paste with linear system
      T_Image *pp = new T_Image(dest->w(),dest->h());
      ForIndex(c,3) {
        // build system for this channel
        LinearSolver ls;
        cerr << "building system ... ";
        int numEqns = dest->w()*dest->h()*2;
        int numVars = dest->w()*dest->h();
        ls.allocate( numEqns , numVars );
        int eqn = 0;
        LIBSL_TRACE;
        ForIndex(j,dest->h()) {
          ForIndex(i,dest->w()) {
            // horiz
            {
              int pid_0 = ((i+1  +dest->w())%dest->w()) + j*dest->w();
              int pid_1 = i      +   j * dest->w();
              ls.coeff( eqn , pid_0 ) +=   1;
              ls.coeff( eqn , pid_1 ) += - 1;
              ls.b()[eqn]             +=   0;
              eqn ++;
            }
            // vert
            {
              int pid_0 = i      + ((j+1  +dest->h())%dest->h())*dest->w();
              int pid_1 = i      +   j * dest->w();
              ls.coeff( eqn , pid_0 ) +=   1;
              ls.coeff( eqn , pid_1 ) += - 1;
              ls.b()[eqn]             +=   0;
              eqn ++;
            }
            if ( dest->pixel(i,j)[0] ) {
              ls.lock( i + j * dest->w() , dest->pixel(i,j)[c] );
            }
          }
        }
        sl_assert(eqn == numEqns);
        // solve
        // cerr << "solving ... ";
        ls.prepareSolver();
        Array<double> x( numVars );
        ls.solve(x);
        // cerr << "done.\n"; 
        // read result in image
        ForRange(v,0,numVars-1) {
          pp->pixel( v%dest->w(),v/dest->w() )[c] = float(x[v]);
        }
      }
      return pp;
    }

  } //namespace LibSL::Image
} //namespace LibSL

// ------------------------------------------------------
