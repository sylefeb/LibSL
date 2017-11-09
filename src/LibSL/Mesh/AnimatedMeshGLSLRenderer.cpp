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
//---------------------------------------------------------------------------

#include "AnimatedMeshGLSLRenderer.h"

#include <LibSL/Errors/Errors.h>
using namespace LibSL::Errors;
#include <LibSL/Memory/Array.h>
using namespace LibSL::Memory::Array;
#include <LibSL/Memory/Pointer.h>
using namespace LibSL::Memory::Pointer;
#include <LibSL/CppHelpers/CppHelpers.h>
using namespace LibSL::CppHelpers;
#include <LibSL/GPUHelpers/GPUHelpers.h>
using namespace LibSL::GPUHelpers;

using namespace LibSL::DataStructures;

#include <algorithm>
using namespace std;

//---------------------------------------------------------------------------

#define NAMESPACE LibSL::Mesh

//---------------------------------------------------------------------------

NAMESPACE::AnimatedMeshGLSLRenderer::AnimatedMeshGLSLRenderer(LibSL::Mesh::AnimatedMesh_Ptr m,
                                                              const char *custom_code)
{
  m_AnimatedMesh = m;
  // build shader
  // -> header
  ostringstream vp_ostr;
  vp_ostr << "#version 120\n";
  vp_ostr << "const int MAX_MATRICES = " << LIBSL_ANIMATEDMESH_MAX_MATRICES << ";\n";
  vp_ostr << "const int NumBones = " << m->mesh().bonespervertex << ";\n";
  string vp_code = vp_ostr.str();
  vp_code += "\
               uniform mat4 viewProj; \n\
               uniform mat4 SkeletonMatrices[MAX_MATRICES]; \n\
               \n\
               void main() \n\
               { \n\
               \n\
               vec3      Pos        = vec3(0.0); \n\
               vec3      Normal     = vec3(0.0); \n\
               float     LastWeight = 0.0; \n\
               \n\
               int   IndexVector[4];                       \n\
               float BlendWeightsArray[3];                 \n\
               IndexVector[0] = int(gl_MultiTexCoord2.x);       \n\
               IndexVector[1] = int(gl_MultiTexCoord2.y);       \n\
               IndexVector[2] = int(gl_MultiTexCoord2.z);       \n\
               IndexVector[3] = int(gl_MultiTexCoord2.w);       \n\
               BlendWeightsArray[0] = gl_MultiTexCoord1.x; \n\
               BlendWeightsArray[1] = gl_MultiTexCoord1.y; \n\
               BlendWeightsArray[2] = gl_MultiTexCoord1.z; \n\
               vec4 normal = vec4(gl_Normal,0.0);          \n\
               for (int iBone = 0; iBone < NumBones-1; iBone++) {           \n\
                 LastWeight = LastWeight + BlendWeightsArray[iBone];        \n\
                 vec4 tmp_pos = (SkeletonMatrices[IndexVector[iBone]] * gl_Vertex) * BlendWeightsArray[iBone]; \n\
                 vec4 tmp_nrm = (SkeletonMatrices[IndexVector[iBone]] * normal   ) * BlendWeightsArray[iBone]; \n\
                 Pos    += tmp_pos.xyz; \n\
                 Normal += tmp_nrm.xyz; \n\
               } \n\
               LastWeight = 1.0 - LastWeight; \n\
               vec4 tmp_pos = (SkeletonMatrices[IndexVector[NumBones-1]] * gl_Vertex) * LastWeight; \n\
               vec4 tmp_nrm = (SkeletonMatrices[IndexVector[NumBones-1]] * normal   ) * LastWeight; \n\
               Pos    += tmp_pos.xyz; \n\
               Normal += tmp_nrm.xyz; \n\
               \n\
               gl_Position        = gl_ModelViewProjectionMatrix * vec4(Pos,1.0); \n\
               gl_TexCoord[0]     = gl_MultiTexCoord0; \n\
               gl_TexCoord[1].xyz = Normal; \n\
               }";
  // -> custom code ?
  string fp_code;
  if (custom_code != NULL) {
    fp_code = string(custom_code);
  } else {
    fp_code ="\
           void main() \
           { \
             gl_FragColor = (vec4(normalize(gl_TexCoord[1].xyz)*0.5+0.5,1)); \
           } \
           ";
  }
  /* {
    // DEBUG
    ofstream fout("code.glsl");
    fout << vp_code;
    fout << "\n\n --------------------- \n\n";
    fout << fp_code;
    fout.close();
  } */

  m_glShader          .init(vp_code.c_str(),fp_code.c_str());
  m_glSkeletonMatrices.init(m_glShader,"SkeletonMatrices");

  Array<m4x4f> mids;
  mids.allocate( LIBSL_ANIMATEDMESH_MAX_MATRICES );
  m4x4f mid; mid.eqIdentity();
  mids.fill( mid );
  m_glShader.begin();
  m_glSkeletonMatrices.set( mids );
  m_glShader.end();

  // build mesh
  m_SkinnedMesh = AutoPtr<SkinnedMesh>(new SkinnedMesh());
  m_SkinnedMesh->begin(GPUMESH_TRIANGLELIST);
  ForArray(m_AnimatedMesh->mesh().vertices,n) {
    m_SkinnedMesh->normal(
      m_AnimatedMesh->mesh().vertices[n].nrm[0],
      m_AnimatedMesh->mesh().vertices[n].nrm[1],
      m_AnimatedMesh->mesh().vertices[n].nrm[2]);
    m_SkinnedMesh->texcoord0_2(
      m_AnimatedMesh->mesh().vertices[n].uv[0],
      m_AnimatedMesh->mesh().vertices[n].uv[1]);
    m_SkinnedMesh->texcoord1_3(
      m_AnimatedMesh->mesh().vertices[n].weight[0],
      m_AnimatedMesh->mesh().vertices[n].weight[1],
      m_AnimatedMesh->mesh().vertices[n].weight[2]);
    m_SkinnedMesh->texcoord2_4(
      m_AnimatedMesh->mesh().vertices[n].idx[0],
      m_AnimatedMesh->mesh().vertices[n].idx[1],
      m_AnimatedMesh->mesh().vertices[n].idx[2],
      m_AnimatedMesh->mesh().vertices[n].idx[3]);
    m_SkinnedMesh->vertex_3(
      m_AnimatedMesh->mesh().vertices[n].pos[0],
      m_AnimatedMesh->mesh().vertices[n].pos[1],
      m_AnimatedMesh->mesh().vertices[n].pos[2]);
  }
  ForArray(m_AnimatedMesh->mesh().indices,n) {
    m_SkinnedMesh->index(m_AnimatedMesh->mesh().indices[n]);
  }
  m_SkinnedMesh->end();

}

//---------------------------------------------------------------------------

NAMESPACE::AnimatedMeshGLSLRenderer::~AnimatedMeshGLSLRenderer()
{
  m_glShader.terminate();
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshGLSLRenderer::render(const AnimatedMeshController& controller)
{
  m_glShader           .begin();
  m_glSkeletonMatrices .set( controller.boneMatrices() );
  m_SkinnedMesh        ->render();
  m_glShader           .end();
}

//---------------------------------------------------------------------------
