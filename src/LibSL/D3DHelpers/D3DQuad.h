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
// LibSL::D3DHelpers::D3DQuad
// ------------------------------------------------------
//
// Simple quad class
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-07
// ------------------------------------------------------

#pragma once

#include <LibSL/LibSL.common.h>

namespace LibSL {
  namespace D3DHelpers {

    class LIBSL_DLL D3DQuad
    {
      struct VERTEX
      {
        FLOAT x, y, z;
        FLOAT u0, v0;
        FLOAT u1, v1;
      };

      static DWORD FVF;

    protected:

      LPDIRECT3DDEVICE9              m_pd3dDevice;
      static LPDIRECT3DVERTEXBUFFER9 s_QuadVB;
      static int                     s_iInstanceCount;

    public:

      D3DQuad(LPDIRECT3DDEVICE9 p);
      ~D3DQuad();

      void render();
    };

  } //namespace LibSL::D3DHelpers
} //namespace LibSL
