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
// LibSL::GPUTex::D3D_format
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

#include <d3d9.h>

// ------------------------------------------------------

namespace LibSL  {
  namespace GPUTex {

    template <typename T_Type,int T_Num> class D3D_format;

    template <> class D3D_format<unsigned char,1>
    {
    public:
      enum {d3dfmt =D3DFMT_L8};
      enum {isdepth=0};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class D3D_format<unsigned char,3>
    {
    public:
      enum {d3dfmt=D3DFMT_R8G8B8};
      enum {isdepth=0};
      static uint ordering(uint r)  {return (2-r);}
    };

    template <> class D3D_format<unsigned char,4>
    {
    public:
      enum {d3dfmt=D3DFMT_A8R8G8B8};
      enum {isdepth=0};
      static uint ordering(uint r)  {return 3-((r+1)&3);}
    };

    template <> class D3D_format<unsigned short,1>
    {
    public:
      enum {d3dfmt=D3DFMT_L16};
      enum {isdepth=0};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class D3D_format<short,2>
    {
    public:
      enum {d3dfmt=D3DFMT_V16U16};
      enum {isdepth=0};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class D3D_format<unsigned short,2>
    {
    public:
      enum {d3dfmt=D3DFMT_G16R16};
      enum {isdepth=0};
      static uint ordering(uint r)  {return (r);}
    };
/*
    template <> class D3D_format<unsigned short,3>
    {
    public:
      enum {d3dfmt=ERROR_no_16bits_3_components_in_DX9};
      static uint ordering(uint r)  {return (r);}
    };
*/
    template <> class D3D_format<unsigned short,4>
    {
    public:
      enum {d3dfmt=D3DFMT_A16B16G16R16};
      enum {isdepth=0};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class D3D_format<half,1>
    {
    public:
      enum {d3dfmt=D3DFMT_R16F};
      enum {isdepth=0};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class D3D_format<half,2>
    {
    public:
      enum {d3dfmt=D3DFMT_G16R16F};
      enum {isdepth=0};
      static uint ordering(uint r)  {return (r);}
    };
/*
    template <> class D3D_format<half,3>
    {
    public:
    ERROR_format_half3_not_available_in_Direct3D _;
    enum {d3dfmt=-1};
    };
*/
    template <> class D3D_format<half,4>
    {
    public:
      enum {d3dfmt=D3DFMT_A16B16G16R16F};
      enum {isdepth=0};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class D3D_format<float,1>
    {
    public:
      enum {d3dfmt=D3DFMT_R32F};
      enum {isdepth=0};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class D3D_format<float,2>
    {
    public:
      enum {d3dfmt=D3DFMT_G32R32F};
      enum {isdepth=0};
      static uint ordering(uint r)  {return (r);}
    };
/*
    template <> class D3D_format<float,3>
    {
    public:
    ERROR_format_float3_not_available_in_Direct3D _;
    enum {d3dfmt=-1};
    };
*/
    template <> class D3D_format<float,4>
    {
    public:
      enum {d3dfmt  = D3DFMT_A32B32G32R32F};
      enum {isdepth = 0};
      static uint ordering(uint r)  {return (r);}
    };

    // depth texture format

    template <> class D3D_format<depth32,1>
    {
    public:
      enum {d3dfmt  = D3DFMT_D32};
      enum {isdepth = 1};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class D3D_format<depth24,1>
    {
    public:
      enum {d3dfmt  = D3DFMT_D24S8};
      enum {isdepth = 1};
      static uint ordering(uint r)  {return (r);}
    };

    template <> class D3D_format<depth16,1>
    {
    public:
      enum {d3dfmt  = D3DFMT_D16};
      enum {isdepth = 1};
      static uint ordering(uint r)  {return (r);}
    };

  } // namespace LibSL::GPUTex
} // namespace LibSL

// ------------------------------------------------------
