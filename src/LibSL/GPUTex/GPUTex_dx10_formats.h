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
// LibSL::GPUTex::DX10_format
// ------------------------------------------------------
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-06-05
// ------------------------------------------------------

#pragma once

// ------------------------------------------------------

#include <LibSL/System/Types.h>
using namespace LibSL::System::Types;

// ------------------------------------------------------

#include <d3d10_1.h>

// ------------------------------------------------------

namespace LibSL  {
  namespace GPUTex {

    template <typename T_Type,int T_Num> class DX10_format;

    template <> class DX10_format<unsigned char,1>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_R8_UNORM,
                      dx10fmt_int  = DXGI_FORMAT_R8_UINT};
      enum {isdepth = 0};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class DX10_format<unsigned char,2>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_R8G8_UNORM,
                      dx10fmt_int  = DXGI_FORMAT_R8G8_UINT};
      enum {isdepth = 0};
      static uint ordering(uint r)  {return (r);}
    };

    /* template <> class DX10_format<unsigned char,3>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_R8G8B8_UNORM,
                      dx10fmt_int  = DXGI_FORMAT_R8G8B8_UINT};
      enum {isdepth = 0};
      static uint ordering(uint r)  {return (2-r);}
    }; */

    template <> class DX10_format<unsigned char,4>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_R8G8B8A8_UNORM,
                      dx10fmt_int  = DXGI_FORMAT_R8G8B8A8_UINT};
      enum {isdepth = 0};
      static uint ordering(uint r)  {return 3-((r+1)&3);}
    };

    template <> class DX10_format<unsigned short,1>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_R16_UNORM,
                      dx10fmt_int  = DXGI_FORMAT_R16_UINT};
      enum {isdepth       = 0};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class DX10_format<unsigned short,2>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_R16G16_UNORM,
                      dx10fmt_int  = DXGI_FORMAT_R16G16_UINT};
      enum {isdepth=0};
      static uint ordering(uint r)  {return (r);}
    };

    /*template <> class DX10_format<unsigned short,3>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_R16G16B16_UNORM,
                      dx10fmt_int  = DXGI_FORMAT_R16G16B16_UINT};
      enum {isdepth=0};
      static uint ordering(uint r)  {return (r);}
    };*/

    template <> class DX10_format<unsigned short,4>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_R16G16B16A16_UNORM,
                      dx10fmt_int  = DXGI_FORMAT_R16G16B16A16_UINT};
      enum {isdepth=0};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class DX10_format<half,1>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_R16_FLOAT,
                      dx10fmt_int  = DXGI_FORMAT_R16_FLOAT};
      enum {isdepth = 0};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class DX10_format<half,2>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_R16G16_FLOAT,
                      dx10fmt_int  = DXGI_FORMAT_R16G16_FLOAT};
      enum {isdepth = 0};
      static uint ordering(uint r)  {return (r);}
    };

    /*template <> class DX10_format<half,3>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_R16G16B16_FLOAT,
                      dx10fmt_int  = DXGI_FORMAT_R16G16B16_FLOAT};
      enum {isdepth = 0};
      static uint ordering(uint r)  {return (r);}
    };*/

    template <> class DX10_format<half,4>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_R16G16B16A16_FLOAT,
                      dx10fmt_int  = DXGI_FORMAT_R16G16B16A16_FLOAT};
      enum {isdepth = 0};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class DX10_format<float,1>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_R32_FLOAT,
                      dx10fmt_int  = DXGI_FORMAT_R32_FLOAT};
      enum {isdepth = 0};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class DX10_format<float,2>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_R32G32_FLOAT,
                      dx10fmt_int  = DXGI_FORMAT_R32G32_FLOAT};
      enum {isdepth = 0};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class DX10_format<float,3>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_R32G32B32_FLOAT,
                      dx10fmt_int  = DXGI_FORMAT_R32G32B32_FLOAT};
      enum {isdepth = 0};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class DX10_format<float,4>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_R32G32B32A32_FLOAT,
                      dx10fmt_int  = DXGI_FORMAT_R32G32B32A32_FLOAT};
      enum {isdepth = 0};
      static uint ordering(uint r)  {return (r);}
    };

    // depth texture format

    template <> class DX10_format<depth32,1>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_D32_FLOAT,
                      dx10fmt_int  = DXGI_FORMAT_D32_FLOAT};
      enum {isdepth = 1};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class DX10_format<depth24,1>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_D24_UNORM_S8_UINT,
                      dx10fmt_int  = DXGI_FORMAT_D24_UNORM_S8_UINT};      
      enum {isdepth = 1};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class DX10_format<depth16,1>
    {
    public:
      enum dx10fmt   {dx10fmt_norm = DXGI_FORMAT_D16_UNORM,
                      dx10fmt_int  = DXGI_FORMAT_D16_UNORM};      
      enum {isdepth = 1};
      static uint ordering(uint r)  {return (r);}
    };

  } // namespace LibSL::GPUTex
} // namespace LibSL

// ------------------------------------------------------
