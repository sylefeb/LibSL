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

#include "AnimatedMeshCgRenderer.h"

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

NAMESPACE::AnimatedMeshCgRenderer::AnimatedMeshCgRenderer(LibSL::Mesh::AnimatedMesh_Ptr m,
                                                          const char *custom_code)
{
  m_AnimatedMesh=m;
  // build shader
  // -> header
  string code= " \
               \
               struct VS_INPUT \
               { \
               float4 Pos    : POSITION;  \
               float3 Normal : NORMAL;    \
               float2 UV     : TEXCOORD0; \
               float3 W      : TEXCOORD1; \
               float4 I      : TEXCOORD2; \
               }; \
               \
               struct VS_OUTPUT \
               { \
               float4 Pos    : POSITION;  \
               float2 UV     : TEXCOORD0; \
               float3 Normal : TEXCOORD1; \
               }; \
               \
               static const int MAX_MATRICES = 40;";
  code += sprint("    static const int NumBones=%d;",m->mesh().bonespervertex);
  code += "\
               uniform float4x4 ViewProj; \
               uniform float4x4 SkeletonMatrices[MAX_MATRICES]; \
               \
               VS_OUTPUT vp_animatedmesh(VS_INPUT In) \
               { \
               VS_OUTPUT Out; \
               \
               float3      Pos = 0.0f;\
               float3      Normal = 0.0f;\
               float       LastWeight = 0.0f;\
               \
               float IndexVector[4];\
               float BlendWeightsArray[3];\
               IndexVector[0]=In.I.x;\
               IndexVector[1]=In.I.y;\
               IndexVector[2]=In.I.z;\
               IndexVector[3]=In.I.w;\
               BlendWeightsArray[0] = In.W.x;\
               BlendWeightsArray[1] = In.W.y;\
               BlendWeightsArray[2] = In.W.z;\
               float4 normal = float4(In.Normal,0); \
               for (int iBone = 0; iBone < NumBones-1; iBone++) {\
                 LastWeight = LastWeight + BlendWeightsArray[iBone];        \
                 Pos    += mul(SkeletonMatrices[IndexVector[iBone]],In.Pos) * BlendWeightsArray[iBone];   \
                 Normal += mul(SkeletonMatrices[IndexVector[iBone]],normal) * BlendWeightsArray[iBone];   \
               }\
               LastWeight = 1.0f - LastWeight; \
               Pos    += mul(SkeletonMatrices[IndexVector[NumBones-1]],In.Pos) * LastWeight; \
               Normal += mul(SkeletonMatrices[IndexVector[NumBones-1]],normal) * LastWeight; \
               \
               Out.Pos   = mul(ViewProj,float4(Pos,1)); \
               Out.UV    = In.UV; \
               Out.Normal= Normal; \
               return Out; \
               }";
  // -> custom code ?
  if (custom_code != NULL) {
    code += string(custom_code);
  } else {
    code +="\
           float4 fp_animatedmesh(VS_OUTPUT In) : COLOR0 \
           { \
           return (float4(normalize(In.Normal)*0.5+0.5,1)); \
           } \
           ";
  }
  // -> footer
  code += "\
          technique t_animatedmesh { \
          pass P0 { \
          VertexShader = compile CG_PROFILE_VP vp_animatedmesh(); \
          PixelShader  = compile CG_PROFILE_FP fp_animatedmesh(); \
          } \
          } \
          ";
  /* {
    // DEBUG
    ofstream fout("code.cg");
    fout << code;
    fout.close();
  } */

  m_cgShader.init(code.c_str());
  m_cgViewProj.init(m_cgShader,"ViewProj");

  m4x4f id; id.eqIdentity();
  ForIndex(n,LIBSL_ANIMATEDMESH_MAX_MATRICES) {
    m_cgSkeletonMatrices[n].init(m_cgShader,"SkeletonMatrices",n);
    m_cgSkeletonMatrices[n].set(id);
  }

  //m_cgSkeletonMatricesArray.init(m_cgShader,"SkeletonMatrices");

  // build mesh
  m_SkinnedMesh=new SkinnedMesh();
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

NAMESPACE::AnimatedMeshCgRenderer::~AnimatedMeshCgRenderer()
{
  m_cgShader.terminate();
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshCgRenderer::render(const AnimatedMeshController& controller)
{
  ForIndex(n,m_AnimatedMesh->numBones()) {
    m_cgSkeletonMatrices[n].set(controller.boneMatrix(n));
  }

//  m_cgSkeletonMatricesArray.set(m_AnimatedMesh->numBones(),&(controller.boneMatrix(0)));
  
  m_cgViewProj.setAPIMatrix(LIBSL_MODELVIEW_PROJECTION_MATRIX);
  m_cgShader.begin();
  m_SkinnedMesh->render();
  m_cgShader.end();
}

//---------------------------------------------------------------------------
