// ------------------------------------------------------
// LibSL::AnimatedMeshFormat_animesh
// ------------------------------------------------------
//
// Loads an animesh (LibSL format)
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2009-11-15
// ------------------------------------------------------

#pragma once

#include <LibSL/Mesh/AnimatedMesh.h>

#include <fstream>

// ------------------------------------------------------

namespace LibSL {
  namespace Mesh {

    class AnimatedMeshFormat_animesh : public AnimatedMeshFormat_plugin
    {
    private:

      void writeFrame            (LibSL::CppHelpers::obinstream&,const AnimatedMesh::t_frame& )                                          const;
      void writeSkinVertex       (LibSL::CppHelpers::obinstream&,const AnimatedMesh::t_skinvertex& )                                     const;
      void writeSkinnedMesh      (LibSL::CppHelpers::obinstream&,const AnimatedMesh::t_skinnedmesh& )                                    const;
      void writeBoneKeys         (LibSL::CppHelpers::obinstream&,const AnimatedMesh::t_bonekeys& )                                       const;
      void writeKeyFrameAnimation(LibSL::CppHelpers::obinstream&,const AnimatedMesh::t_keyframeanimation& )                              const;
      void writeAnimations       (LibSL::CppHelpers::obinstream&,const LibSL::Memory::Array::Array<AnimatedMesh::t_keyframeanimation>& ) const;
      void writeSkeleton         (LibSL::CppHelpers::obinstream&,LibSL::DataStructures::Hierarchy<AnimatedMesh::t_frame>::t_AutoPtr )    const;

      void readFrame             (LibSL::CppHelpers::ibinstream&,AnimatedMesh::t_frame& )                             const;
      void readSkinVertex        (LibSL::CppHelpers::ibinstream&,AnimatedMesh::t_skinvertex& )                        const;
      void readSkinnedMesh       (LibSL::CppHelpers::ibinstream&,AnimatedMesh::t_skinnedmesh& )                       const;
      void readBoneKeys          (LibSL::CppHelpers::ibinstream&,AnimatedMesh::t_bonekeys& )                          const;
      void readKeyFrameAnimation (LibSL::CppHelpers::ibinstream&,AnimatedMesh::t_keyframeanimation& )                 const;
      void readAnimations        (LibSL::CppHelpers::ibinstream&,AnimatedMesh *am)                                    const;
      LibSL::DataStructures::Hierarchy<AnimatedMesh::t_frame>::t_AutoPtr readSkeleton(LibSL::CppHelpers::ibinstream&) const;

    public:

      AnimatedMeshFormat_animesh();
      void            save(const char *,const AnimatedMesh *) const;
      AnimatedMesh   *load(const char *)                      const;
      const char     *signature()                             const {return "animesh";}

    };

  }; // namespace LibSL::Mesh
}; // namespace LibSL

// --------------------------------------------------------------
