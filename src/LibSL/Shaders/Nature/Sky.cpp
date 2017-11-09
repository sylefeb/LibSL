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
//---------------------------------------------------------------------------
#include "LibSL.precompiled.h"
// ------------------------------------------------------------

#include "Sky.h"

// ------------------------------------------------------------

#ifdef OPENGL
#include <LibSL/GPUMesh/GPUMesh_gl.h>
#include <Cg/cgGL.h>
#endif
#ifdef DIRECT3D
#include <LibSL/GPUMesh/GPUMesh_d3d.h>
#endif

#include <LibSL/Memory/Pointer.h>

#include <GL/glu.h>

// ------------------------------------------------------------

using namespace LibSL::Memory::Pointer;
using namespace LibSL::CgHelpers;
using namespace LibSL::Math;
using namespace GPUMesh;

// ------------------------------------------------------------

#include <string>
using namespace std;

// ------------------------------------------------------------

#define NAMESPACE LibSL::Shaders::Nature

// ------------------------------------------------------------

uint NAMESPACE::Sky::s_RefCount =0;

typedef MVF1(mvf_position_4f)          mvf_quad;
typedef GPUMesh_VertexBuffer<mvf_quad> QuadMesh;

static QuadMesh *s_Quad=NULL;

// ------------------------------------------------------------

static const char *LibSL_Nature_Atmosphere_code_string =
#include "../Cg/LibSL_Nature_Atmosphere.string"

static const char *LibSL_Math_Intersections_code_string =
#include "../Cg/LibSL_Math_Intersections.string"

// ------------------------------------------------------------

NAMESPACE::Sky::Sky()
{

}

NAMESPACE::Sky::~Sky()
{
  s_RefCount--;
  if (s_RefCount == 0) {
    delete (s_Quad);
    s_Quad=NULL;
  } 
}

// ------------------------------------------------------------

void NAMESPACE::Sky::allocateQuad()
{
  if (s_Quad==NULL) {
    s_Quad=new QuadMesh();
    s_Quad->begin(GPUMESH_TRIANGLELIST);

    s_Quad->vertex_4(-1.0f,-1.0f, 1.0f,1.0f);
    s_Quad->vertex_4( 1.0f,-1.0f, 1.0f,1.0f);
    s_Quad->vertex_4( 1.0f, 1.0f, 1.0f,1.0f);

    s_Quad->vertex_4(-1.0f,-1.0f, 1.0f,1.0f);
    s_Quad->vertex_4( 1.0f, 1.0f, 1.0f,1.0f);
    s_Quad->vertex_4(-1.0f, 1.0f, 1.0f,1.0f);

    s_Quad->end();
  }
  s_RefCount++;
}

// ------------------------------------------------------------

void NAMESPACE::Sky::drawSky(const v3f& camerapos,
			     const v3f& sundir)
{
  if (s_Quad == NULL || m_Shader == NULL)
    throw LibSL::Errors::Fatal("LibSL::Nature::Sky - Sky::bindTo(shader) must be called before using the Sky instance !");

  m_cgSundir.set(sundir);
  m_cgCamerapos.set(camerapos);
  
#ifdef OPENGL
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(0);
  m_cgModelview_inv .setAPIMatrix(LIBSL_MODELVIEW_MATRIX,LIBSL_INVERSE);
  m_cgProjection_inv.setAPIMatrix(LIBSL_PROJECTION_MATRIX,LIBSL_INVERSE);
  /*
  cgGLSetStateMatrixParameter(m_cgModelview_inv.handle(),
			      CG_GL_MODELVIEW_MATRIX,
			      CG_GL_MATRIX_INVERSE);
  cgGLSetStateMatrixParameter(m_cgProjection_inv.handle(),
			      CG_GL_PROJECTION_MATRIX,
			      CG_GL_MATRIX_INVERSE);
            */
  m_Shader->pushTechnique();
  m_cgSky.activate();
  m_Shader->begin();
  s_Quad->render();
  m_Shader->end();
  m_Shader->popTechnique();
  glDepthMask(0xFF);
  glPopAttrib();
#endif


#ifdef DIRECT3D

#endif

}

// ------------------------------------------------------------

void NAMESPACE::Sky::bindTo(LibSL::CgHelpers::CgShader& shader)
{
  m_Shader=&shader;

  allocateQuad();
  
  // This is not very efficient ...
  // TODO A better solution could be a bind mechanism with the shader ...
  m_cgSky.init(*m_Shader,"t_sky");
  m_cgSundir.init(*m_Shader,"Sky_SunDir");
  m_cgCamerapos.init(*m_Shader,"Sky_CameraPos");
  m_cgModelview_inv.init(*m_Shader,"Sky_ModelviewInverse_GL_MVP");
  m_cgProjection_inv.init(*m_Shader,"Sky_ProjectionInverse_GL_MVP");
}

// ------------------------------------------------------------

void NAMESPACE::Sky::addTo(std::string& code)
{
  code = std::string(LibSL_Nature_Atmosphere_code_string) + code;
  code = std::string(LibSL_Math_Intersections_code_string) + code;
}

// ------------------------------------------------------------
