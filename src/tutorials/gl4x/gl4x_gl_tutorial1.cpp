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
// --------------------------------------------------------------
// Tutorial for GLHelpers
// --------------------------------------------------------------
#include <iostream>
#include <ctime>
#include <cmath>
/* -------------------------------------------------------- */

#include <LibSL/LibSL_gl4.h>

#ifdef WIN32
LIBSL_WIN32_FIX; // necessary due to a VC compiler issue
#endif

/* -------------------------------------------------------- */

#include "gl4x_gl_tutorial1.h"

AutoBindShader::gl4x_gl_tutorial1 shader;

GLBuffer                g_Buffer;

/* -------------------------------------------------------- */

using namespace std;

/* -------------------------------------------------------- */

int main(int argc, char **argv) 
{
  try {

    // init trackball
    TrackballUI::init(256,256,"OpenGL 4.x tutorial");
    
    // init shader
    shader  .init();
    g_Buffer.init( 256*sizeof(char) );

    // help
    printf("[q]     - quit\n");
    
	shader.begin();
	shader.u_Buffer.set( g_Buffer );
	shader.run( V3I(256/16,1,1) );
	shader.end();

    // read back buffer
    uchar tbl[256];
    g_Buffer.readBack(tbl,256);
    ForIndex(i,256) {
      cerr << (int)tbl[i] << ' ';
    }
    cerr << endl;

	shader  .terminate();
    g_Buffer.terminate();

    // shutdown
    TrackballUI::shutdown();

  } catch (GLException& e) {
    cerr << e.message() << endl;
    return (-1);
  } catch (Fatal& e) {
    cerr << e.message() << endl;
    return (-1);
  }

  return (0);
}

/* -------------------------------------------------------- */
