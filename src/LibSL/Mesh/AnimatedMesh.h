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
// LibSL::AnimatedMesh
// ------------------------------------------------------
//
// 
// Note:
//   - quaternion, translation and rotation all have a different
//     set of keyframes (timings and length could be different).
//     However, it is here assumed they correspond => TODO: cleanup
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-07-14
// ------------------------------------------------------

#pragma once

#include <LibSL/Math/Matrix4x4.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Math/Quaternion.h>
#include <LibSL/Memory/Pointer.h>
#include <LibSL/Memory/Array.h>
#include <LibSL/DataStructures/Hierarchy.h>

#include <fstream>

// ------------------------------------------------------

#define LIBSL_ANIMATEDMESH_MAX_MATRICES 64
#define LIBSL_MAX_BONE_INFLUENCE         4

// ------------------------------------------------------

namespace LibSL {
  namespace Mesh {

    class AnimatedMeshFormat_plugin; // forward definition

    class AnimatedMesh
    {
    private:

      typedef LibSL::Math::m4x4f m4x4f;
      typedef LibSL::Math::quatf quatf;
      typedef LibSL::Math::v2f   v2f;
      typedef LibSL::Math::v3f   v3f;

    public:

      typedef struct s_frame {
        std::string name;
        bool        hasMesh;
        m4x4f       frameMatrix;
        int         boneIndex;
        m4x4f       boneOffset;
      } t_frame;

      typedef struct s_skinvertex
      {
        v3f    pos;
        v3f    nrm;
        v2f    uv;
        float  weight[LIBSL_MAX_BONE_INFLUENCE-1];
        uchar  idx   [LIBSL_MAX_BONE_INFLUENCE  ];
      } t_skinvertex;

      typedef struct s_skinnedmesh //// TODO: replace by a TriangleMesh
                                   //// with surfaces for multi-material
      {
        LibSL::Memory::Array::Array<t_skinvertex> vertices;
        LibSL::Memory::Array::Array<ushort>       indices;
        uint                                      bonespervertex;
      } t_skinnedmesh;

      typedef struct s_bonekeys
      {
        std::string                                          name;
        LibSL::Memory::Array::Array<std::pair<float,quatf> > quaternions;   // time,value
        LibSL::Memory::Array::Array<std::pair<float,v3f>   > translations;
        LibSL::Memory::Array::Array<std::pair<float,v3f>   > scalings;
      } t_bonekeys;

      typedef struct s_keyframeanimation
      {
        std::string                             name;
        uint                                    numkeys;
        LibSL::Memory::Array::Array<t_bonekeys> bonekeys;
        std::map<std::string,uint>              boneNametoAnimationIndex;
      } t_keyframeanimation;

      typedef LibSL::Memory::Pointer::AutoPtr<AnimatedMesh> t_AutoPtr;
      typedef uint                                          t_AnimId;

    protected:

      t_skinnedmesh                                        m_Mesh;
      uint                                                 m_NumBones;
      LibSL::DataStructures::Hierarchy<t_frame>::t_AutoPtr m_Skeleton;
      LibSL::Memory::Array::Array<t_keyframeanimation>     m_Animations;
      std::map<std::string,uint>                           m_AnimationsByName;
      v3f                                                  m_Center;
      float                                                m_Radius;

    public:

      AnimatedMesh();

      void readFromFile(const char *);
      void writeToFile (const char *);

      t_skinnedmesh&                 mesh()           { return (m_Mesh);     }
      const t_skinnedmesh&           mesh()     const { return (m_Mesh);     }

      uint&                          numBones()       { return (m_NumBones); }
      const uint&                    numBones() const { return (m_NumBones); }

      float&                         radius()         { return (m_Radius);   }
      const float&                   radius()   const { return (m_Radius);   }

      v3f&                           center()         { return (m_Center);   }
      const v3f&                     center()   const { return (m_Center);   }

      LibSL::DataStructures::Hierarchy<t_frame>::t_AutoPtr&       skeleton()         {return (m_Skeleton); }
      const LibSL::DataStructures::Hierarchy<t_frame>::t_AutoPtr& skeleton()   const {return (m_Skeleton); }

      LibSL::Memory::Array::Array<t_keyframeanimation>&           animations()       {return (m_Animations);}
      const LibSL::Memory::Array::Array<t_keyframeanimation>&     animations() const {return (m_Animations);}

      std::map<std::string,uint>&        animationsByName()       { return (m_AnimationsByName); }
      const std::map<std::string,uint>&  animationsByName() const { return (m_AnimationsByName); }

      t_keyframeanimation&           animation(t_AnimId aid)             {return (m_Animations[aid]);}
      t_AnimId                       animationIdFromName(const std::string& name);

    };

    /// autoptr for AnimatedMesh
    typedef AnimatedMesh::t_AutoPtr AnimatedMesh_Ptr;

    //// Loading and saving animated meshes

    /// AnimatedMesh format plugin (abstract)
    class AnimatedMeshFormat_plugin
      //  : public LibSL::Memory::TraceLeaks::LeakProbe<AnimatedMeshFormat_plugin>
    {
    public:
      virtual void          save(const char *,const AnimatedMesh *) const =0;
      virtual AnimatedMesh *load(const char *)                      const =0;
      virtual const char   *signature()                             const =0;
      virtual ~AnimatedMeshFormat_plugin() {}
    };

    /// AnimatedMesh format manager (singleton)
    class AnimatedMeshFormatManager
    {
    private:

      std::map<std::string,AnimatedMeshFormat_plugin*> m_Plugins;

      static AnimatedMeshFormatManager *s_Manager;
      AnimatedMeshFormatManager();

    public:

      ~AnimatedMeshFormatManager();

      void registerPlugin(AnimatedMeshFormat_plugin *plugin);
      AnimatedMeshFormat_plugin *getPlugin(const char *signature) const;

      static AnimatedMeshFormatManager *getUniqueInstance();
    };

    /// Load and save global methods
    
    /// load an animated mesh
    LIBSL_DLL AnimatedMesh     *loadAnimatedMesh(const char *);

    /// save a mesh
    LIBSL_DLL void              saveAnimatedMesh(const char *,const AnimatedMesh *);

    // --------------------------------------------------------------

  } //namespace LibSL::Mesh
} //namespace LibSL


// ------------------------------------------------------

#define ANIMATEDMESH_FORMAT_MANAGER (*LibSL::Mesh::AnimatedMeshFormatManager::getUniqueInstance())

// ------------------------------------------------------
