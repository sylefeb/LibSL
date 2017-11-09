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
// LibSL::GPUHelpers::CgFillArray2D
// ------------------------------------------------------
//
//  Class to compute values in a 2D array using the GPU
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-07
//                    2006-03-29 plateform independent (Cg)
// ------------------------------------------------------
// 
//   The code string must contain a function as follows:
//   "  float compute(float2 ij) { return 2.71828; } "
//   The ij input coordinates are integer coordinates within the table
// 
//   When an input named "table" is given, a corresponding table( ) function
//   is created to access it from the Cg code. A variable named table_SIZE
//   is also created. table_SIZE = float4(W,H,1/W,1/H) where W,H are the 
//   dimensions of the input table.
//
//   Inputs and result are 2 dimensionnal arrays (Matrix) but, of course, one
//   of the dimension can be 1 to emulate 1D arrays.
// 
//   Cell types are float1, float2, ... float4. Channels are accessed with 
//   the [] operator (including 'float1' eg.   float1 v; v[0]=0;).
//
//   Byte* types are available for input tables. 
//   Bytes are automatically mapped to integers using floor(255.5*v)
//
//   All tables must have no dimension greater than 4096 or Direct3D will
//   produce an error.
// 
//   See "test_gpufillarray" for complete examples
// 
// ------------------------------------------------------

// TODO: - changeInput

#pragma once

#include <LibSL/LibSL.common.h>

#include <LibSL/Errors/Errors.h>
#include <LibSL/Memory/TraceLeaks.h>
#include <LibSL/CppHelpers/CppHelpers.h>

#include <LibSL/Memory/Array2D.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/CgHelpers/CgImageProcessing.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <Cg/cg.h>

#ifdef OPENGL
#include <LibSL/GPUTex/GPUTex_gl.h>
#include <LibSL/GPUHelpers/GPUHelpers_gl.h>
#endif

#ifdef DIRECT3D
#include <LibSL/D3DHelpers/D3DHelpers.h>
#include <LibSL/GPUTex/GPUTex_d3d.h>
#include <LibSL/GPUHelpers/GPUHelpers_d3d.h>
#endif

#ifndef OPENGL
#ifndef DIRECT3D
ERROR_____either_OPENGL_or_DIRECT3D_must_be_defined;
#endif
#endif

#include <string>
#include <map>

namespace LibSL {
  namespace GPUHelpers {

    typedef LibSL::Math::Tuple<float,1> float1;
    typedef LibSL::Math::Tuple<float,2> float2;
    typedef LibSL::Math::Tuple<float,3> float3;
    typedef LibSL::Math::Tuple<float,4> float4;

    typedef LibSL::Math::Tuple<unsigned char,1> byte1;
    typedef LibSL::Math::Tuple<unsigned char,2> byte2;
    typedef LibSL::Math::Tuple<unsigned char,3> byte3;
    typedef LibSL::Math::Tuple<unsigned char,4> byte4;

    // helper class to retrieve d3d format information from formats
    template <typename T> class FormatInfo { public: 
    static int indexcomp(int c) {return (c);} 
    static const char *decoder(){return ("v");}
    };
    template <> class FormatInfo<float1> { public: 
    static int indexcomp(int c) {return (c);} 
    static const char *decoder(){return ("v");}
    };
    template <> class FormatInfo<float2> { public: 
    static int indexcomp(int c) {return (c);} 
    static const char *decoder(){return ("v");}
    };
    template <> class FormatInfo<float3> { public: 
    static int indexcomp(int c) {return (c);} 
    static const char *decoder(){return ("v");}
    };
    template <> class FormatInfo<float4> { public: 
    static int indexcomp(int c) {return (c);} 
    static const char *decoder(){return ("v");}
    };
    template <> class FormatInfo<byte1> { public: 
    static int indexcomp(int c) {return (c);} 
    static const char *decoder(){return ("floor(255.5*v)");}
    };
    template <> class FormatInfo<byte2> { public: 
    static int indexcomp(int c) {return (c);} 
    static const char *decoder(){return ("floor(255.5*v)");}
    };
    template <> class FormatInfo<byte3> { public: 
    static int indexcomp(int c) {return (c < 3 ? (2-c) : 3 );} 
    static const char *decoder(){return ("floor(255.5*v)");}
    };
    template <> class FormatInfo<byte4> { public: 
    static int indexcomp(int c) {return (c < 3 ? (2-c) : 3 );} 
    static const char *decoder(){return ("floor(255.5*v)");}
    };      

    class LIBSL_DLL CgFillArray2D : public LibSL::Memory::TraceLeaks::LeakProbe<CgFillArray2D>
    {
    public:

      typedef LibSL::GPUTex::RenderTarget2DRGBA32F    t_RenderTarget;      
      typedef t_RenderTarget::t_PixelArray            t_ResultArray;
      typedef LibSL::Memory::Pointer::Pointer<
        CgFillArray2D,
        LibSL::Memory::Pointer::CheckValid,
        LibSL::Memory::Pointer::TransferRefCountUInt> t_AutoPtr;

    protected:

      typedef struct s_input_nfo
      {
        LibSL::GPUTex::Tex2D_Ptr           texture;
        LibSL::GPUTex::Tex2D_api::t_Handle api_texture;
        uint                           w;
        uint                           h;
        uint                           numcomp;
        int                            offsetx;
        int                            offsety;
        const char                    *decoder;
      }input_nfo;

      LibSL::CgHelpers::CgImageProcessing   m_cgShader;
      std::string                           m_UserCode;
      bool                                  m_bNeedCompile;
      int                                   m_W;
      int                                   m_H;
      OcclusionQuery_Ptr                    m_OcclusionQuery;
      bool                                  m_DoQuery;
      uint                                  m_PixelCounter;
      t_RenderTarget::t_AutoPtr             m_RenderTarget;

      std::map<std::string,input_nfo>                 m_Inputs;
      std::list<std::pair<std::string,std::string> >  m_Techniques;

      void setShaderParameters();

    public:

      CgFillArray2D(const char *code,int xsize,int ysize);
      ~CgFillArray2D();

      LIBSL_DISABLE_COPY(CgFillArray2D);

      /// Launch the computation (compiles the code if necessary)
      //  NOTE (D3D): Calls BeginScene and EndScene - *will* produce an error
      //              if called from within a BeginScene/EndScene block!
      void compute();
      /// Launch the computation on a sub-region (compiles the code if necessary)
      void computeRegion(int x,int y,int w,int h);

      /// Generate the code in a stream - usefull for debug
      void generateCode(std::ostream& code);
      /// Compile the code (calls generateCode)
      void compile();

      /// Change the shader code
      void setCode(const char *code);

      /// Enable pixel counter
      void enablePixelCounter();
      /// Disable pixel counter
      void disablePixelCounter();
      /// Returns pixel counter value
      uint pixelCounter() const {return (m_PixelCounter);}

      /// Add a technique
      void addTechnique(const char *techname,const char *entrypoint);

      /// Select technique
      void selectTechnique(const char *techname) 
      { LibSL::CgHelpers::CgTechnique tech; tech.init(shader(),techname); tech.activate(); }

      int width()  const {return (m_W);}
      int height() const {return (m_H);}

      LibSL::CgHelpers::CgImageProcessing& shader() {compile(); return (m_cgShader);}

      /// Retrieve the result after computation
      ///   requires a readback: assume this call is slow
      ///
      void readBackResult(t_ResultArray& array) {
        m_RenderTarget->readBack(array);
      }

      /// Add an input array
      ///
      template <typename T_Pix> 
      bool addInput(
        const char *name,
        const LibSL::Memory::Array::Array2D<T_Pix>& array,
        const char *decoder=NULL,
        int offsetx=0,int offsety=0)
      {
        std::map<std::string,input_nfo>::iterator F=m_Inputs.find(std::string(name));
        if (F != m_Inputs.end()) {
          throw LibSL::Errors::Fatal("LibSL::CgFillArray2D::addInput - input '%s' already exists",name);
        }
        m_bNeedCompile=true;
        input_nfo nfo;
        nfo.texture=new LibSL::GPUTex::Tex2D<T_Pix>(array);
        nfo.api_texture=nfo.texture->texture();
        nfo.w=nfo.texture->w();
        nfo.h=nfo.texture->h();
        nfo.numcomp=T_Pix::e_Size;
        if (decoder == NULL) {
          nfo.decoder=FormatInfo<T_Pix>::decoder();
        } else {
          nfo.decoder=decoder;
        }
        nfo.offsetx=offsetx;
        nfo.offsety=offsety;
        m_Inputs[std::string(name)]=nfo;
        return (updateInput(name,array,offsetx,offsety));
      }

      /// Update the content of an input array
      ///
      template <typename T_Pix> bool updateInput(
        const char *name,
        const LibSL::Memory::Array::Array2D<T_Pix>& array,
        int offsetx=0,int offsety=0)
      {
        std::map<std::string,input_nfo>::iterator I=m_Inputs.find(name);
        if (I == m_Inputs.end()) {
          throw LibSL::Errors::Fatal("LibSL::CgFillArray2D::updateInput - input '%s' does not exist",name);
        }
        if ((*I).second.texture.isNull()) {
          throw LibSL::Errors::Fatal("LibSL::CgFillArray2D::updateInput - this instance does not own input '%s'",name);
        }
        // send to GPU

        return (true);
      }

      /// Add another CgFillArray2D as input
      ///    note: it cannot be the same GPUFillAray instance (cannot read/write to GPU buffers)
      ///
      bool addInput(
        const char *name,
        const CgFillArray2D *gpufillarray,
        const char *decoder=NULL,
        int offsetx=0,int offsety=0)
      {
        if (gpufillarray == this) {
          LIBSL_FATAL_ERROR("LibSL::CgFillArray2D::addInput - cannot loop a CgFillArray2D on itself");
        }
        return (addInput(name,
          gpufillarray->m_RenderTarget->texture(),
          gpufillarray->m_RenderTarget->w(),
          gpufillarray->m_RenderTarget->h(),
          decoder,
          offsetx,offsety));
      }

      /// Add a texture as an input
      ///
      bool addInput(
        const char   *name,
        const LibSL::GPUTex::Tex2D_api::t_Handle tex,
        uint w,uint h,
        const char   *decoder=NULL,
        int offsetx=0,int offsety=0)
      {
        std::map<std::string,input_nfo>::iterator F=m_Inputs.find(std::string(name));
        if (F != m_Inputs.end()) {
          throw LibSL::Errors::Fatal("LibSL::CgFillArray2D::addInput - input '%s' already exists",name);
        }
        m_bNeedCompile=true;
        input_nfo nfo;
        // nfo.texture=NULL; // texture has been created by caller
        sl_assert(nfo.texture.isNull());
        nfo.api_texture=tex;
        nfo.w=w;
        nfo.h=h;
        nfo.numcomp=t_RenderTarget::t_PixelArray::t_Element::e_Size;
        if (decoder == NULL) {
          nfo.decoder=FormatInfo<t_RenderTarget::t_PixelArray::t_Element>::decoder();
        } else {
          nfo.decoder=decoder;
        }
        nfo.offsetx=offsetx;
        nfo.offsety=offsety;
        m_Inputs[std::string(name)]=nfo;
        return (true);
      }

      /// Removes an input
      ///
      void removeInput(const char *name)
      {
        std::map<std::string,input_nfo>::iterator F=m_Inputs.find(std::string(name));
        if (F == m_Inputs.end()) {
          throw LibSL::Errors::Fatal("LibSL::CgFillArray2D::removeInput - input '%s' does not exist",name);
        }
        m_bNeedCompile=true;
        m_Inputs.erase(F);
      }

      /// Get compiled code
      ///
      std::string getCompiledCode();

    };

  } //namespace LibSL::GPUHelpers
} //namespace LibSL

// -----------------------------------------------------------------------

#define CgFillArray2D_SET_PARAMETER(GFA,PNAME,VAL) \
{                                             \
  CgParameter param;                          \
  param.init(GFA->shader(),PNAME);            \
  param.set(VAL);                             \
}

// -----------------------------------------------------------------------
