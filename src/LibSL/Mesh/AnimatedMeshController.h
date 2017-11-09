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
// 
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-07-14
// ------------------------------------------------------

#pragma once

#include <LibSL/Mesh/AnimatedMesh.h>

// ------------------------------------------------------

namespace LibSL {
  namespace Mesh {

    //---------------------------------------------------------------------------

    class AnimatedMeshController
    {
    protected:

      typedef LibSL::Math::m4x4f m4x4f;
      typedef LibSL::Math::quatf quatf;
      typedef LibSL::Math::v2f   v2f;
      typedef LibSL::Math::v3f   v3f;

    protected:

      LibSL::Memory::Array::Array<m4x4f>   m_BoneMatrices;
      LibSL::Mesh::AnimatedMesh_Ptr        m_AnimatedMesh;
      m4x4f                                m_Position;

      void  updateMatrices(
        LibSL::DataStructures::Hierarchy<LibSL::Mesh::AnimatedMesh::t_frame>::t_AutoPtr node,
        const m4x4f&);
      void  updateMatrices();

      System::Time::t_time time();

    public:

      class ControllerCallback
      {
      public:
        virtual ~ControllerCallback() {}
        virtual void execute()=0;
      };

    public:

      AnimatedMeshController(LibSL::Mesh::AnimatedMesh_Ptr);
      virtual ~AnimatedMeshController() {}

      virtual void computeAnimatedFrame(
        LibSL::DataStructures::Hierarchy<LibSL::Mesh::AnimatedMesh::t_frame>::t_AutoPtr node,
        quatf& _q,v3f& _t,v3f& _s)=0;

      virtual void animate(float elapsed=0,bool updatematrices=true);
      // restart animation
      virtual void restart()           {}
      // return translation resulting of the animation
      virtual v3f  motionTranslation() { return LibSL::Math::V3F(0,0,0); }
      // compute matrices for pose
      void         computePose()       { updateMatrices(); }

      const m4x4f&                              boneMatrix  (uint i) const {return (m_BoneMatrices[i]);}
      const LibSL::Memory::Array::Array<m4x4f>& boneMatrices()       const {return (m_BoneMatrices);}
      m4x4f&                                    position()                 {return (m_Position);}

      typedef LibSL::Memory::Pointer::AutoPtr<AnimatedMeshController> t_AutoPtr;
    };

    //! autoptr for controller callbacks
    typedef LibSL::Memory::Pointer::AutoPtr<AnimatedMeshController::ControllerCallback> ControllerCallback_Ptr;

    //---------------------------------------------------------------------------

    //! RestPoseController (only used for debug purposes, to see the rest pose)
    class RestPoseController : public AnimatedMeshController
    {
    public:

      RestPoseController(LibSL::Mesh::AnimatedMesh_Ptr);
      
      void computeAnimatedFrame(
        LibSL::DataStructures::Hierarchy<LibSL::Mesh::AnimatedMesh::t_frame>::t_AutoPtr node,
        quatf& _q,v3f& _t,v3f& _s);

      void animate(float elapsed=0,bool updatematrices=true);

      typedef LibSL::Memory::Pointer::AutoPtr<RestPoseController> t_AutoPtr;
    };

	//---------------------------------------------------------------------------

    //! SimpleKeyframeController
    class SimpleKeyframeController : public AnimatedMeshController
    {
    protected:

      AnimatedMesh::t_AnimId               m_AnimId;
      uint                                 m_CurrentKey;

      AnimatedMesh::t_keyframeanimation& keyframes() { return m_AnimatedMesh->animation(m_AnimId); }

    public:

      SimpleKeyframeController(LibSL::Mesh::AnimatedMesh_Ptr,AnimatedMesh::t_AnimId animId = 0);
      
      void computeAnimatedFrame(
        LibSL::DataStructures::Hierarchy<LibSL::Mesh::AnimatedMesh::t_frame>::t_AutoPtr node,
        quatf& _q,v3f& _t,v3f& _s);

      void animate(float elapsed=0,bool updatematrices=true);

      typedef LibSL::Memory::Pointer::AutoPtr<SimpleKeyframeController> t_AutoPtr;
    };

    //---------------------------------------------------------------------------

    //! KeyframeController
    class KeyframeController : public AnimatedMeshController
    {
    protected:

      AnimatedMesh::t_AnimId               m_AnimId;
      uint                                 m_CurrentKey;
      uint                                 m_FirstKey;
      uint                                 m_LastKey;
      bool                                 m_Loop;
      bool                                 m_Done;
      bool                                 m_Reverse;
      uint                                 m_NextKey;
      System::Time::t_time                 m_LastStartTime;
      double                               m_StartKeyTime;
      float                                m_InterpValue;
      ControllerCallback_Ptr               m_Callback;
      float                                m_Speed;
      v3f                                  m_MotionTranslation;

      double keyTimeStamp(uint key);
      uint   nextKey     (uint key);
      double nextKeyTime (uint key);
      uint   prevKey     (uint key);
      double prevKeyTime (uint key);

      AnimatedMesh::t_keyframeanimation& keyframes() { return m_AnimatedMesh->animation(m_AnimId); }

      void init(
        LibSL::Mesh::AnimatedMesh_Ptr mesh,
        float                         speed,
        const v3f&                    motionTranslation,
        AnimatedMesh::t_AnimId        animationId,
        uint                          firstkey,
        uint                          lastkey,
        bool                          loop,
        ControllerCallback_Ptr        callback);

    public:

      KeyframeController(
        LibSL::Mesh::AnimatedMesh_Ptr,
        float      speed,
        const v3f& motionTranslation,
        AnimatedMesh::t_AnimId animationId,
        uint firstkey,uint lastkey,bool loop,
        ControllerCallback_Ptr callback = ControllerCallback_Ptr());

      KeyframeController(
        LibSL::Mesh::AnimatedMesh_Ptr,
        float      speed,
        const v3f& motionTranslation,
        AnimatedMesh::t_AnimId animationId,bool loop,
        ControllerCallback_Ptr callback = ControllerCallback_Ptr());

      void computeAnimatedFrame(
        LibSL::DataStructures::Hierarchy<LibSL::Mesh::AnimatedMesh::t_frame>::t_AutoPtr node,
        quatf& _q,v3f& _t,v3f& _s);

      void animate(float elapsed=0,bool updatematrices=true);
      void restart();
      bool done() {return (m_Done);}
      v3f  motionTranslation() { return (m_MotionTranslation); }

      uint getCurrentFrame() const {return (m_CurrentKey);}
      void setCurrentFrame(uint k);

      typedef LibSL::Memory::Pointer::AutoPtr<KeyframeController> t_AutoPtr;
    };

    //---------------------------------------------------------------------------

    //! TransitionController
    class TransitionController : public AnimatedMeshController
    {
    protected:

      AnimatedMeshController::t_AutoPtr m_ControllerA;
      AnimatedMeshController::t_AutoPtr m_ControllerB;

      System::Time::t_time m_StartTime;
      float                m_Duration;
      float                m_InterpValue;

    public:
      
      TransitionController(
        LibSL::Mesh::AnimatedMesh_Ptr,
        AnimatedMeshController::t_AutoPtr a,
        AnimatedMeshController::t_AutoPtr b,
        float duration);
      
      void computeAnimatedFrame(
        LibSL::DataStructures::Hierarchy<LibSL::Mesh::AnimatedMesh::t_frame>::t_AutoPtr node,
        quatf& _q,v3f& _t,v3f& _s);

      bool done();
      void animate(float elapsed=0,bool updatematrices=true);
      v3f  motionTranslation();

      typedef LibSL::Memory::Pointer::AutoPtr<TransitionController> t_AutoPtr;

      AnimatedMeshController::t_AutoPtr controllerA() {return (m_ControllerA);}
      AnimatedMeshController::t_AutoPtr controllerB() {return (m_ControllerB);}
    };

    //---------------------------------------------------------------------------

    //! CombineController
    class CombineController : public AnimatedMeshController
    {
    protected:

      AnimatedMeshController::t_AutoPtr m_ControllerA;
      AnimatedMeshController::t_AutoPtr m_ControllerB;

    public:
      
      CombineController(
        LibSL::Mesh::AnimatedMesh_Ptr     mesh,
        AnimatedMeshController::t_AutoPtr a,
        AnimatedMeshController::t_AutoPtr b);
      
      void computeAnimatedFrame(
        LibSL::DataStructures::Hierarchy<LibSL::Mesh::AnimatedMesh::t_frame>::t_AutoPtr node,
        quatf& _q,v3f& _t,v3f& _s);

      void animate(float elapsed=0,bool updatematrices=true);
      v3f  motionTranslation();

      typedef LibSL::Memory::Pointer::AutoPtr<CombineController> t_AutoPtr;

      AnimatedMeshController::t_AutoPtr controllerA() {return (m_ControllerA);}
      AnimatedMeshController::t_AutoPtr controllerB() {return (m_ControllerB);}
    };

    //! autoptr for controllers
    typedef AnimatedMeshController::t_AutoPtr   AnimatedMeshController_Ptr;
    typedef RestPoseController::t_AutoPtr       RestPoseController_Ptr;
    typedef SimpleKeyframeController::t_AutoPtr SimpleKeyframeController_Ptr;
    typedef KeyframeController::t_AutoPtr       KeyframeController_Ptr;
    typedef TransitionController::t_AutoPtr     TransitionController_Ptr;
    typedef CombineController::t_AutoPtr        CombineController_Ptr;

  } //namespace LibSL::Mesh
} //namespace LibSL
