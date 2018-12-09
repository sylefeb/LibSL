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
// ----------------------------------------------------------------------------
// GPUMesh_primitives.h
//

//
//                                                    (c) Sylvain Lefebvre 2003
// ----------------------------------------------------------------------------

#pragma once

namespace LibSL {
namespace GPUMesh {

class PrimitiveNfo
{
private:
  int  m_iNbPrim;
  bool m_bValid;
public:
  PrimitiveNfo(int primtype,int nbidx)
  {
    switch (primtype)
    {
    case GPUMESH_TRIANGLELIST:
      m_iNbPrim = nbidx/3;
      m_bValid  = (nbidx % 3 == 0);
      break;
    case GPUMESH_TRIANGLEFAN:
    case GPUMESH_TRIANGLESTRIP:
      m_iNbPrim = nbidx-2;
      m_bValid  = (nbidx > 2);
      break;
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
    case GPUMESH_QUADS:
      m_iNbPrim = nbidx/4;
      m_bValid  = (nbidx % 4 == 0);
      break;
#endif
    case GPUMESH_LINELIST:
      m_iNbPrim = nbidx/2;
      m_bValid  = (nbidx % 2 == 0);
      break;
    case GPUMESH_LINESTRIP:
      m_iNbPrim = nbidx-1;
      m_bValid  = nbidx > 1;
      break;
    default:
      m_iNbPrim=0;
      m_bValid=false;
      break;
    }
    if (primtype < 0)
    {
      m_iNbPrim=0;
      m_bValid=false;
    }
  }

  const int&  nbPrim() const {return (m_iNbPrim);}
  const bool& valid() const {return (m_bValid);}
};

} // namespace GPUMesh
} // namespace LibSL
