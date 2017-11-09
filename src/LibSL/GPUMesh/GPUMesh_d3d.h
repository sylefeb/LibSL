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
// GPUmesh_d3d.h
//
//   Header for using the GPUmesh class with Direct3D.
//
//                                                    (c) Sylvain Lefebvre 2003
// ----------------------------------------------------------------------------
#pragma once
// ----------------------------------------------


#include <Windows.h>
#include <mmsystem.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "LibSL/D3DHelpers/D3DHelpers.h"

#define GPUMESH_TRIANGLESTRIP D3DPT_TRIANGLESTRIP
#define GPUMESH_TRIANGLELIST  D3DPT_TRIANGLELIST
#define GPUMESH_TRIANGLEFAN   D3DPT_TRIANGLEFAN
#define GPUMESH_QUADS         -2
#define GPUMESH_LINELIST      D3DPT_LINELIST
#define GPUMESH_LINESTRIP     D3DPT_LINESTRIP

#include "GPUmesh.h"
#include "GPUmesh_CompilePolicy_D3D.h"

namespace LibSL {
namespace GPUMesh {
  
  /// D3D Mesh with vertex buffer
  template <class VertexFormat> 
  class GPUMesh_VertexBuffer
    : public GPUMesh_generic<CompilePolicy_D3D,VertexFormat> 
  {
  public:
    GPUMesh_VertexBuffer()
    {
#ifdef DXUT_OLD_VERSION
      m_Mesh.pDevice = LIBSL_D3D_DEVICE;
#else
      m_Mesh.pDevice = LIBSL_D3D_DEVICE;
#endif
    }
  };

  /// D3D Mesh with indexed vertex buffer
  template <class T_VertexFormat,class T_IndexType> 
  class GPUMesh_Indexed_VertexBuffer
    : public GPUMesh_generic<CompilePolicy_Indexed_D3D,T_VertexFormat,T_IndexType> 
  {
  public:
    GPUMesh_Indexed_VertexBuffer()
    {
#ifdef DXUT_OLD_VERSION
      m_Mesh.pDevice = LIBSL_D3D_DEVICE;
#else
      m_Mesh.pDevice = LIBSL_D3D_DEVICE;
#endif
    }
  };

} // namespace GPUMesh
} // namespace LibSL

// ----------------------------------------------
