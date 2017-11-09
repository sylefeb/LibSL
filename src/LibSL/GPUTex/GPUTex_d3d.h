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
// LibSL::GPUTex  - Direct3D configuration
// ------------------------------------------------------
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-06-05
// ------------------------------------------------------

#pragma once

// ------------------------------------------------------

#ifdef WIN32
#include <windows.h>
#endif

// ------------------------------------------------------

#include <LibSL/GPUTex/GPUTex.h>
#include <LibSL/GPUTex/GPUTex_CompilePolicy_D3D.h>
#include <LibSL/Math/Tuple.h>

// ------------------------------------------------------

namespace LibSL  {
  namespace GPUTex {

    // Tex2D

    typedef Tex2D_interface<D3D_APIPolicy> Tex2D_api;
    typedef Tex2D_api::t_AutoPtr           Tex2D_Ptr;

    template <class T_PixelFormat> 
    class Tex2D
      : public Tex2D_generic<D3D_APIPolicy,D3D_CompilePolicy,T_PixelFormat> 
    {
    public:
      typedef typename D3D_CompilePolicy<D3D_APIPolicy,T_PixelFormat>::t_PixelArray2D t_PixelArray;
    public:
      Tex2D(const t_PixelArray& array,uint flags=0) :
          Tex2D_generic<D3D_APIPolicy,D3D_CompilePolicy,T_PixelFormat>(array,flags)
          {}
          Tex2D(const LibSL::Memory::Array::Array<t_PixelArray>& miparray,uint flags=0) :
          Tex2D_generic<D3D_APIPolicy,D3D_CompilePolicy,T_PixelFormat>(miparray,flags)
          {}
    };

    // RenderTarget 2D

    typedef RenderTarget2D_interface<D3D_APIPolicy> RenderTarget2D_api;
    typedef RenderTarget2D_api::t_AutoPtr           RenderTarget2D_Ptr;

    template <class T_PixelFormat> 
    class RenderTarget2D
      : public RenderTarget2D_generic<D3D_APIPolicy,D3D_CompilePolicy,T_PixelFormat> 
    {
    public:
      typedef typename D3D_CompilePolicy<D3D_APIPolicy,T_PixelFormat>::t_PixelArray2D t_PixelArray;
    public:
      RenderTarget2D(uint w,uint h,uint flags=0,uint num=1) :
          RenderTarget2D_generic<D3D_APIPolicy,D3D_CompilePolicy,T_PixelFormat>(w,h,flags,num)
          {}
    };

    // Tex3D

    typedef Tex3D_interface<D3D_APIPolicy> Tex3D_api;
    typedef Tex3D_api::t_AutoPtr           Tex3D_Ptr;

    template <class T_PixelFormat> 
    class Tex3D
      : public Tex3D_generic<D3D_APIPolicy,D3D_CompilePolicy,T_PixelFormat> 
    {
    public:
      typedef typename D3D_CompilePolicy<D3D_APIPolicy,T_PixelFormat>::t_PixelArray3D t_PixelArray;
    public:
      Tex3D(const t_PixelArray& array,uint flags=0) :
          Tex3D_generic<D3D_APIPolicy,D3D_CompilePolicy,T_PixelFormat>(array,flags)
          {}
    };  

    // typedef standard formats
#      include "GPUTex_stdfmt.h"

  } // namespace LibSL::GPUTex
} // namespace LibSL

// ------------------------------------------------------
