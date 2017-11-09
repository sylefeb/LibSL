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
#include <LibSL/DataStructures/Hierarchy.h>
using namespace LibSL::DataStructures;

#include "AnimatedMeshFxRenderer.h"

#include <algorithm>
using namespace std;

//---------------------------------------------------------------------------

#define NAMESPACE LibSL::Mesh

//---------------------------------------------------------------------------

NAMESPACE::AnimatedMeshFxRenderer::AnimatedMeshFxRenderer(LibSL::Mesh::AnimatedMesh_Ptr m,
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
                 float4 Normal : TEXCOORD1; \
               }; \
               \
               uniform float4x4 ViewProj; \
               static const int MAX_MATRICES = " + string(sprint("%d",LIBSL_ANIMATEDMESH_MAX_MATRICES)) + "; \
               uniform float4x4 SkeletonMatrices[MAX_MATRICES];";
  code += sprint("    static const int NumBones=%d;",m->mesh().bonespervertex);
  code += "    \
               void skinning(VS_INPUT In, out float4 pos,out float4 normal)\
               {\
                 pos    = float4(0.0f,0.0f,0.0f,0.0f); \
                 normal = float4(0.0f,0.0f,0.0f,0.0f); \
                 float LastWeight = 0.0f;              \
                 \
                 float  BlendWeightsArray[3] = (float[3])In.W;\
                 int    IndexVector[4]       = (int[4])  In.I;\
                 float4 in_nrm = float4(In.Normal,0); \
                 for (int iBone = 0; iBone < NumBones-1; iBone++) {\
                   LastWeight = LastWeight + BlendWeightsArray[iBone];        \
                   pos    += mul(SkeletonMatrices[IndexVector[iBone]],In.Pos) * BlendWeightsArray[iBone];   \
                   normal += mul(SkeletonMatrices[IndexVector[iBone]],in_nrm) * BlendWeightsArray[iBone];   \
                 }\
                 LastWeight = 1.0f - LastWeight; \
                 pos    += mul(SkeletonMatrices[IndexVector[NumBones-1]],In.Pos) * LastWeight; \
                 normal += mul(SkeletonMatrices[IndexVector[NumBones-1]],in_nrm) * LastWeight; \
               } \
               \
               VS_OUTPUT vp_animatedmesh(VS_INPUT In) \
               { \
                 VS_OUTPUT Out; \
                 \
                 float4 pos,normal;       \
                 skinning(In, pos,normal); \
                 \
                 Out.Pos    = mul(ViewProj,float4(pos.xyz,1)); \
                 Out.UV     = In.UV;  \
                 Out.Normal = normal; \
                 return Out; \
               }";
  // -> custom code ?
  if (custom_code != NULL) {
    code += string(custom_code);
  } else {
    code +="\
           float4 fp_animatedmesh(VS_OUTPUT In) : COLOR0 \
           { \
             return (float4(normalize(In.Normal.xyz)*0.5+0.5,1)); \
           } \
           ";
  }
  // -> footer
  code += "\
          technique t_animatedmesh { \
            pass P0 { \
              VertexShader = compile vs_2_0 vp_animatedmesh(); \
              PixelShader  = compile ps_2_0 fp_animatedmesh(); \
            } \
          } \
          ";

  m_fxShader.init(code.c_str());
  m_fxViewProj.init(m_fxShader,"ViewProj");

  m4x4f id; id.eqIdentity();
  ForIndex(n,LIBSL_ANIMATEDMESH_MAX_MATRICES) {
    m_fxSkeletonMatrices[n].init(m_fxShader,"SkeletonMatrices",n);
    m_fxSkeletonMatrices[n].set(id);
  }

  // build mesh
  m_SkinnedMesh = new SkinnedMesh();
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

NAMESPACE::AnimatedMeshFxRenderer::~AnimatedMeshFxRenderer()
{
  m_fxShader.terminate();
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshFxRenderer::render(const AnimatedMeshController& controller)
{
   
  m_fxViewProj.setAPIMatrix(LIBSL_MODELVIEW_PROJECTION_MATRIX);
  m_fxShader.begin();
  ForIndex(n,m_AnimatedMesh->numBones()) {
    //cerr << "BONE " << n << endl;
    //cerr << controller.boneMatrix(n);
    m_fxSkeletonMatrices[n].set(controller.boneMatrix(n));
  }
  m_SkinnedMesh->render();
  m_fxShader.end();
}

//---------------------------------------------------------------------------
