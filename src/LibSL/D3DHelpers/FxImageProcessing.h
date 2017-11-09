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
/*
Fx Shader helper tools

Sylvain Lefebvre - 2005-03-15
2005-11-28 modified to fit GPUFillTable class

Helper class to write Image processing shader

Implements rendering operation (full, window)
Provides default vertex program and variables under the defines
FX_IMAGE_PROCESSING_SHADER_HEADER and FX_IMAGE_PROCESSING_SHADER_FOOTER (see below)
These defines are provided at run-time compile time by the application, OR by including
the file FxImageProcessing

Variables:
float4 Viewport          (left,top,width,height) (in pixels)
float2 DestRegionCoord   (left,top) coordinates of rendered window (in pixels)
float2 DestRegionSize    (width,height) of rendered region (in pixels)
float2 InvDestRegionSize (1.0 / width, 1.0 / height) of rendered region

The vertex program setup texture coordinates so that you can safely use
floor(In.Tex) to obtain an integer ( In.Tex ~= (i,j)+0.5 where i,j are integers ).
This integer is the absolute coordinate of the pixel computed from DestRegionCoord
and DestRegionSize by the vertex program.

Exemple: (test.fx)

FX_IMAGE_PROCESSING_SHADER_HEADER

float4 ps_main(VS_OUTPUT In) : COLOR
{
return float4(floor(In.Tex)*InvDestRegionSize,0,0);   
}

FX_IMAGE_PROCESSING_SHADER_FOOTER

*/
// ------------------------------------------------------------

#pragma once

#include <LibSL/LibSL.common.h>

#include "FxHelpers.h"
#include "D3DQuad.h"

#include "FxImageProcessing.fx"

// ------------------------------------------------------------

namespace LibSL {
  namespace D3DHelpers {

    class LIBSL_DLL FxImageProcessing : public FxShader
    {

    protected:

      static D3DQuad *s_Quad;
      static int      s_iRefCount;

      FxTechnique       m_fxTechMain;
      FxParameter       m_fxViewport;
      FxParameter       m_fxDestRegionCoord;
      FxParameter       m_fxDestRegionSize;
      FxParameter       m_fxInvDestRegionSize;

      LPDIRECT3DDEVICE9 m_pd3dDevice;

    public:

      void init(
        LPDIRECT3DDEVICE9 d3d,
        const char  *shader,
        DWORD        flags=0,
        D3DXMACRO   *defines=NULL);

      void terminate();

      void renderFull(int targetw,int targeth);
      void renderRegion(int x,int y,int w,int h);
      void renderRegion(int srcx,int srcy,int dstx,int dsty,int w,int h);

    };

  } //namespace LibSL::D3DHelpers
} //namespace LibSL
