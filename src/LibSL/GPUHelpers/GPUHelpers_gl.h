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
// LibSL::GPUHelpers_gl
// ------------------------------------------------------
//
// GPU helpers OpenGL policies
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-29
// ------------------------------------------------------

#pragma once

#ifndef EMSCRIPTEN

#include "LibSL/GPUHelpers/GPUHelpers.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#ifndef EMSCRIPTEN
#include <GL/gl.h>
#endif
#endif

// ------------------------------------------------------

#ifdef USE_GLUX
# include <glux.h>
#include "GL_ARB_occlusion_query.h"
GLUX_LOAD(GL_ARB_occlusion_query)
#endif

// ------------------------------------------------------

namespace LibSL {
  namespace GPUHelpers {

    class OcclusionQuery_GL_Policy
    {
    public:

      typedef GLuint t_Handle;

      static t_Handle create()
      {
        // check to make sure functionality is supported
        GLint bitsSupported;
        glGetQueryivARB(GL_SAMPLES_PASSED_ARB,GL_QUERY_COUNTER_BITS_ARB,&bitsSupported);
        if (bitsSupported == 0) {
          throw LibSL::Errors::Fatal("LibSL::GPUHelpers::OcclusionQuery - hardware does not support queries");
        }
        GLuint q;
        glGenQueriesARB(1,&q);
        return (q);
      }

      static void destroy(t_Handle& q)
      {
        glDeleteQueriesARB(1,&q);
      }

      static void start(const t_Handle& q)
      {
        glBeginQueryARB(GL_SAMPLES_PASSED_ARB,q);  
      }

      static void stop(const t_Handle& q)
      {
        glEndQueryARB(GL_SAMPLES_PASSED_ARB);
      }

      static int done(const t_Handle& q)
      {
        int available=0;
        glGetQueryObjectivARB(q,
          GL_QUERY_RESULT_AVAILABLE_ARB,
          &available);
        if (available) {
          GLuint sampleCount=0;
          glGetQueryObjectuivARB(q, GL_QUERY_RESULT_ARB,
            &sampleCount);
          return int(sampleCount);
        } else {
          return (-1);
        }
      }

    };

    typedef OcclusionQuery_generic<OcclusionQuery_GL_Policy> OcclusionQuery;
    typedef OcclusionQuery::t_AutoPtr                        OcclusionQuery_Ptr;

  } //namespace LibSL::GPUHelpers
} //namespace LibSL

#endif

// ------------------------------------------------------
