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
// LibSL::GPUHelpers
// ------------------------------------------------------
//
// GPU helpers
// - occlusion queries
//
// NOTE:
//   Direct3D has 3 matrices: projection, view, world.
//   LibSL maps LIBSL_MODELVIEW_MATRIX on D3DRS_WORLD
//   D3DRS_WORLD is unused.
//
// TODO: 
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-29
// ------------------------------------------------------

#pragma once

#include <LibSL/Errors/Errors.h>
#include <LibSL/Memory/Pointer.h>
#include <LibSL/Image/Image.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Math/Matrix4x4.h>

// ------------------------------------------------------

#define LIBSL_OCCLUSIONQUERY_NOTYET -1

#define LIBSL_COLOR_BUFFER   1
#define LIBSL_DEPTH_BUFFER   2
#define LIBSL_STENCIL_BUFFER 4

#define LIBSL_PROJECTION                   1
#define LIBSL_MODELVIEW                    2
#define LIBSL_MODELVIEW_PROJECTION         3

#define LIBSL_PROJECTION_MATRIX            1
#define LIBSL_MODELVIEW_MATRIX             2
#define LIBSL_MODELVIEW_PROJECTION_MATRIX  3
#define LIBSL_TEXTURE_MATRIX               4

#define LIBSL_INVERSE                      1
#define LIBSL_TRANSPOSE                    2

// ------------------------------------------------------

namespace LibSL {
  namespace GPUHelpers {

    /// Occlusion query (generic)

    template <class P_OcclusionQuery>
    class OcclusionQuery_generic
    {
    public:

      typedef LibSL::Memory::Pointer::Pointer<
        OcclusionQuery_generic,
        LibSL::Memory::Pointer::CheckValid,
        LibSL::Memory::Pointer::TransferRefCountUInt>   t_AutoPtr;

    private:

      typename P_OcclusionQuery::t_Handle m_Handle;
      int                                 m_NumPixels;

    public:

      OcclusionQuery_generic()
      {
        m_Handle    = P_OcclusionQuery::create();
        m_NumPixels = -1;
      }

      ~OcclusionQuery_generic()
      {
        P_OcclusionQuery::destroy(m_Handle);
      }

      void start()
      {
        P_OcclusionQuery::start(m_Handle);
      }

      void stop()
      {
        P_OcclusionQuery::stop(m_Handle);
      }

      int done()
      {
        m_NumPixels = P_OcclusionQuery::done(m_Handle);
        return (m_NumPixels);
      }

      const typename P_OcclusionQuery::t_Handle& handle() const
      {
        return (m_Handle);
      }

      int numPixels() const { return (m_NumPixels); }

    };

    // RenderState singleton (generic)

    template <class P_RenderState>
    class RenderState_generic
    {
    protected:
      static RenderState_generic s_UniqueInstance;

      RenderState_generic() {}
    public:
      RenderState_generic *getUniqueInstance()
      {
        if (s_UniqueInstance == NULL) {
          s_UniqueInstance=new RenderState_generic();
        }
        return (s_UniqueInstance);
      }

      void enable(uint state)
      {
        P_RenderState::enable(state);
      }

      void disable(uint state)
      {
        P_RenderState::disable(state);
      }

    };

    // Various graphics API helper functions
         
    //! clear screen
    LIBSL_DLL void clearScreen(
        uint flags=LIBSL_COLOR_BUFFER|LIBSL_DEPTH_BUFFER|LIBSL_STENCIL_BUFFER,
        float r=0.0f,float g=0.0f,float b=0.0f,float a=0.0f,
        float depth=1.0f,
        int   stencil=0);

    //! 3D transforms
    namespace Transform
    {
      //! indentity matrix
      LIBSL_DLL void identity(uint matrixid);
      //! orthographic projection
      LIBSL_DLL void ortho2D(uint matrixid,float l,float r,float t,float b);
      //! orthographic projection with front/back culling
      LIBSL_DLL void ortho2D(uint matrixid,float l,float r,float t,float b,float znear,float zfar);
      //! perspective projection
      //! fov in radians
      LIBSL_DLL void perspective(uint matrixid,float fov_rad,float aspect,float znear,float zfar);
      //! lookat matrix
      LIBSL_DLL void lookat(uint matrixid,
        const LibSL::Math::v3f& eye,
        const LibSL::Math::v3f& at,
        const LibSL::Math::v3f& up);
      //! set arbitrary matrix
      LIBSL_DLL void set(uint matrixid,const LibSL::Math::m4x4f& m);

    } //namespace LibSL::GPUHelpers::Transform

    namespace Renderer
    {
      //! set viewport
      LIBSL_DLL void setViewport(uint x,uint y,uint w,uint h);
      //! get viewport
      LIBSL_DLL void getViewport(uint& _x,uint& _y,uint& _w,uint& _h);

      class ProtectViewport
      {
      private:
        uint m_X,m_Y,m_W,m_H;
      public:
        ProtectViewport()  { getViewport(m_X,m_Y,m_W,m_H); }
        ~ProtectViewport() { setViewport(m_X,m_Y,m_W,m_H); }
      };

    } //namespace LibSL::GPUHelpers::Renderer

    //! screen capture  TODO: other formats ?
    LIBSL_DLL LibSL::Image::ImageRGBA_Ptr captureScreen();

  } //namespace LibSL::GPUHelpers
} //namespace LibSL

// ------------------------------------------------------

// Render states
/*
#define LIBSL_COLOR_WRITE
#define LIBSL_DEPTH_WRITE
#define LIBSL_DEPTH_TEST
#define LIBSL_CULLING
#define LIBSL_LIGHTING
*/
// ------------------------------------------------------
