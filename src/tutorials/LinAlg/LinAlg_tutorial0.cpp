/* -------------------------------------------------------- */

#include <iostream>
#include <ctime>
#include <cmath>
#include <cstring>

using namespace std;

/* -------------------------------------------------------- */

#include <LibSL/LibSL.h>
#include <LibSL/LibSL_linalg.h>

LIBSL_WIN32_FIX;

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  try {

    // build system
    LinearSystem ls;
    cerr << "building system ... ";

    int numEqns = 3;
    int numVars = 2;

    ls.allocate( numEqns , numVars );

    int eqn = 0;
    
    ls.coeff( eqn , 0 ) =  1.0f;
    ls.coeff( eqn , 1 ) =  2.0f;
    ls.b()[eqn]         =  0.5f;
    eqn ++;
    ls.coeff( eqn , 0 ) =  4.0f;
    ls.coeff( eqn , 1 ) =  1.0f;
    ls.b()[eqn]         =  2.0f;
    eqn ++;
    ls.coeff( eqn , 0 ) =  3.0f;
    ls.coeff( eqn , 1 ) =  2.0f;
    ls.b()[eqn]         = 10.0f;
    eqn ++;

    cerr << "done.\n"; 

    // print system
    ForIndex(e,numEqns) {
      cerr << "Eqn. " << e << " : ";
      ForIndex(v,numVars) {
        cerr << sprint(" var%d * %f ",v,ls.coeff( e , v ));
        if (v < numVars-1) {
          cerr << " + ";
        } else {
          cerr << " = ";
        }
      }
      cerr << ls.b()[e] << endl;
    }

    // solve
    cerr << endl;
    cerr << "solving ... ";
    ls.prepareSolver();
    Array<double> x( numVars );
    ls.solve(x);
    cerr << "done.\n"; 

    // solution
    cerr << endl;
    cerr << "Best solution is: " << endl;
    ForIndex(i,x.size()) {
      cerr << "  var" << i << " = " << x[i] << endl;
    }

  } catch (Fatal& e) {
    cerr << Console::red << e.message() << Console::gray << endl;
    return (-1);
  }

  return (0);
}

/* -------------------------------------------------------- */
