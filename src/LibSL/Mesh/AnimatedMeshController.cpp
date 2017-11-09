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

#include "AnimatedMeshController.h"

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
using namespace LibSL::Math;

#include <algorithm>
using namespace std;

//---------------------------------------------------------------------------

#define NAMESPACE LibSL::Mesh

//---------------------------------------------------------------------------

NAMESPACE::AnimatedMeshController::AnimatedMeshController(LibSL::Mesh::AnimatedMesh_Ptr m)
{
  m_AnimatedMesh = m;
  m_Position.eqIdentity();
  // allocate bone matrices
  m_BoneMatrices.allocate(m_AnimatedMesh->numBones());
  m4x4f id; id.eqIdentity();
  ForIndex(i,m_BoneMatrices.size()) {
    m_BoneMatrices[i] = id;
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshController::updateMatrices(Hierarchy<AnimatedMesh::t_frame>::t_AutoPtr node,
                                                       const m4x4f& parent)
{
  m4x4f combined;

  quatf q; v3f t; v3f s;
  computeAnimatedFrame(node,q,t,s);
  m4x4f animated(q.inverse(),s,t);
  // combine it to parents
  
  combined   = parent * animated;
  if (node->data().boneIndex > -1) {
    // compute final bone matrix
    m4x4f bone = combined * node->data().boneOffset;
    // store
    m_BoneMatrices[node->data().boneIndex] = bone;
  }
  
  // combined   = parent * node->data().frameMatrix; // DEBUG: force rest pose

  // recurse
  for (Hierarchy<AnimatedMesh::t_frame>::ChildrenIterator I=node->children();!I.end();I.next()) {
    updateMatrices(I.current(),combined);
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshController::updateMatrices()
{
  LIBSL_BEGIN;
  updateMatrices(m_AnimatedMesh->skeleton(),m_Position);
  LIBSL_END;
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshController::animate(float elapsed,bool updatematrices)
{
  if (updatematrices) {
    updateMatrices();
  }
}

//---------------------------------------------------------------------------

t_time NAMESPACE::AnimatedMeshController::time()
{
  return t_time(System::Time::milliseconds());
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/// Rest pose controller

NAMESPACE::RestPoseController::RestPoseController(LibSL::Mesh::AnimatedMesh_Ptr m)
  : NAMESPACE::AnimatedMeshController(m)
{
  sl_assert(false); //// NOT YET IMPLEMENTED
  // update matrices with first key
  updateMatrices();
}

//---------------------------------------------------------------------------

void NAMESPACE::RestPoseController::computeAnimatedFrame(
  Hierarchy<AnimatedMesh::t_frame>::t_AutoPtr node,quatf& _q,v3f& _t,v3f& _s)
{
  _q       = quatf(0,0,0,1);
  _t       = V3F(0,0,0);
  _s       = V3F(1,1,1);
}

//---------------------------------------------------------------------------

void NAMESPACE::RestPoseController::animate(float elapsed,bool updatematrices)
{

}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/// Simple keyframe controller

NAMESPACE::SimpleKeyframeController::SimpleKeyframeController(LibSL::Mesh::AnimatedMesh_Ptr m,AnimatedMesh::t_AnimId animId)
  : NAMESPACE::AnimatedMeshController(m)
{
  m_AnimId     = animId;
  m_CurrentKey = 0;
  // update matrices with first key
  updateMatrices();
}

//---------------------------------------------------------------------------

void NAMESPACE::SimpleKeyframeController::computeAnimatedFrame(
  Hierarchy<AnimatedMesh::t_frame>::t_AutoPtr node,quatf& _q,v3f& _t,v3f& _s)
{
  map<string,uint>::iterator I=keyframes().boneNametoAnimationIndex.find(node->data().name);
  if (I != keyframes().boneNametoAnimationIndex.end()) {
    uint idx = (*I).second;
    _q       = keyframes().bonekeys[idx].quaternions[m_CurrentKey] .second;
    _t       = keyframes().bonekeys[idx].translations[m_CurrentKey].second;
    _s       = keyframes().bonekeys[idx].scalings[m_CurrentKey]    .second;
  } else {
    _q       = quatf(0,0,0,1);
    _t       = V3F(0,0,0);
    _s       = V3F(1,1,1);
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::SimpleKeyframeController::animate(float elapsed,bool updatematrices)
{
  m_CurrentKey = (m_CurrentKey+1) % keyframes().numkeys;
  // update matrices
  if (updatematrices) {
    updateMatrices();
  }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/// Keyframe controller

NAMESPACE::KeyframeController::KeyframeController(
  LibSL::Mesh::AnimatedMesh_Ptr m,
  float                  speed,
  const v3f&             motionTranslation,
  AnimatedMesh::t_AnimId animId,
  uint                   firstkey,
  uint                   lastkey,
  bool                   loop,
  ControllerCallback_Ptr callback)
  : NAMESPACE::AnimatedMeshController(m)
{
  init(m,speed,motionTranslation,animId,firstkey,lastkey,loop,callback);
}

NAMESPACE::KeyframeController::KeyframeController(
  LibSL::Mesh::AnimatedMesh_Ptr m,
  float                  speed,
  const v3f&             motionTranslation,
  AnimatedMesh::t_AnimId animId,
  bool                   loop,
  ControllerCallback_Ptr callback)
  : NAMESPACE::AnimatedMeshController(m)
{
  init(m,speed,motionTranslation,animId,0,m->animation(animId).numkeys-1,loop,callback);
}

void NAMESPACE::KeyframeController::init(
  LibSL::Mesh::AnimatedMesh_Ptr m,
  float                  speed,
  const v3f&             motionTranslation,
  AnimatedMesh::t_AnimId animId,
  uint                   firstkey,
  uint                   lastkey,
  bool                   loop,
  ControllerCallback_Ptr callback)
{
  m_Speed    = speed;
  m_AnimId   = animId;
  m_FirstKey = firstkey;
  m_LastKey  = lastkey;
  m_Loop     = loop;
  m_Done     = false;
  m_Callback = callback;
  if (m_FirstKey > m_LastKey) {
    m_Reverse  = true;
    uint tmp   = m_FirstKey;
    m_FirstKey = m_LastKey;
    m_LastKey  = tmp;
  } else {
    m_Reverse  = false;
  }
  sl_assert(lastkey-firstkey+1 >= 1);
  if (!m_Reverse) {
    // current key is first
    m_CurrentKey = m_FirstKey;
    m_NextKey    = m_FirstKey;
  } else {
    // current key is last
    m_CurrentKey = m_LastKey;
    m_NextKey    = m_LastKey;
  }
  m_MotionTranslation = motionTranslation;
  m_InterpValue       = 0;
  // get start key time
  sl_assert(keyframes().bonekeys.size() > 1);
  m_StartKeyTime  = keyTimeStamp(m_CurrentKey);
  // last start time is now
  m_LastStartTime = time();
  // update matrices with first key
  updateMatrices();
}

//---------------------------------------------------------------------------

void NAMESPACE::KeyframeController::computeAnimatedFrame(
  Hierarchy<AnimatedMesh::t_frame>::t_AutoPtr node,quatf& _q,v3f& _t,v3f& _s)
{
  map<string,uint>::iterator I=keyframes().boneNametoAnimationIndex.find(node->data().name);
  if (I != keyframes().boneNametoAnimationIndex.end()) {
    uint idx=(*I).second;
    // compute animated frame matrix
    const quatf& q0 = keyframes().bonekeys[idx].quaternions[m_CurrentKey] .second;
    const v3f&   t0 = keyframes().bonekeys[idx].translations[m_CurrentKey].second;
    const v3f&   s0 = keyframes().bonekeys[idx].scalings[m_CurrentKey]    .second;
    const quatf& q1 = keyframes().bonekeys[idx].quaternions[m_NextKey]    .second;
    const v3f&   t1 = keyframes().bonekeys[idx].translations[m_NextKey]   .second;
    const v3f&   s1 = keyframes().bonekeys[idx].scalings[m_NextKey]       .second;
    _q = quatf::slerp(m_InterpValue,q0,q1);
    _t = t0*(1.0f-m_InterpValue)+t1*m_InterpValue;
    _s = s0*(1.0f-m_InterpValue)+s1*m_InterpValue;
  } else {
    _q = quatf(0,0,0,1);
    _t = V3F(0,0,0);
    _s = V3F(1,1,1);
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::KeyframeController::restart()
{
  m_Done = false;
  if (!m_Reverse) {
    m_CurrentKey    = m_FirstKey;
    m_NextKey       = m_FirstKey;
    m_InterpValue   = 0;
    m_LastStartTime = time();
  } else {
    m_CurrentKey    = m_LastKey;
    m_NextKey       = m_LastKey;
    m_InterpValue   = 0;
    m_LastStartTime = time();
  }
}

//---------------------------------------------------------------------------

double NAMESPACE::KeyframeController::keyTimeStamp(uint key)
{
  sl_assert( key < keyframes().bonekeys[0].quaternions.size() );
  return (double(keyframes().bonekeys[0].quaternions[key].first) / double(m_Speed));
}

//---------------------------------------------------------------------------

uint  NAMESPACE::KeyframeController::nextKey(uint key)
{
  uint nextkey = key + 1;
  if (nextkey > m_LastKey) {
    if (m_Loop) nextkey = m_FirstKey;
    else        nextkey = m_LastKey;
  }
  return (nextkey);
}

//---------------------------------------------------------------------------

uint  NAMESPACE::KeyframeController::prevKey(uint key)
{
  int nextkey = (int)key - 1;
  if (nextkey < (int)m_FirstKey) {
    if (m_Loop) nextkey = m_LastKey;
    else        nextkey = m_FirstKey;
  }
  return (uint)(nextkey);
}

//---------------------------------------------------------------------------

double NAMESPACE::KeyframeController::nextKeyTime(uint key)
{
  uint nextkey = key + 1;
  uint numKeys = keyframes().bonekeys[0].quaternions.size();
  if (nextkey > m_LastKey) {
    if (m_Loop) {
      double deltat = double(keyTimeStamp((m_FirstKey+1)%numKeys))-double(keyTimeStamp(m_FirstKey));
      return (double(keyTimeStamp(key))-double(m_StartKeyTime)+deltat);
    } else {
      return (double(keyTimeStamp(m_LastKey))-double(m_StartKeyTime));
    }
  } else {
    return (double(keyTimeStamp(nextkey))-double(m_StartKeyTime));
  }
}

//---------------------------------------------------------------------------

double NAMESPACE::KeyframeController::prevKeyTime(uint key)
{
  int prevkey = (int)key - 1;
  if (prevkey < (int)m_FirstKey) {
    if (m_Loop) {
      double deltat = double(keyTimeStamp(m_LastKey))-double(keyTimeStamp((int)m_LastKey-1));
      return (double(keyTimeStamp(key))-double(m_StartKeyTime)-deltat);
    } else {
      return (double(keyTimeStamp(m_FirstKey))-m_StartKeyTime);
    }
  } else {
    return (double(keyTimeStamp(prevkey))-m_StartKeyTime);
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::KeyframeController::animate(float elapsed,bool updatematrices)
{
  LIBSL_BEGIN;

  t_time tm = time();

  if (!m_Reverse) {

    // check whether to go to next key frame
    while (1) {
      if (!m_Loop && (m_CurrentKey == m_LastKey)) {
        if (!m_Done)
        if (!m_Done && !m_Callback.isNull()) {
          m_Callback->execute();
        }
        m_Done=true;
        break;
      }
      if ( double( t_time(tm) - t_time(m_LastStartTime) ) > nextKeyTime( m_CurrentKey ) ) {
        //cerr << sprint("current = %d, going to %d, (m_LastStartTime-tm = %f)\n",m_CurrentKey,nextKeyTime(m_CurrentKey),float(m_LastStartTime-tm));
        m_CurrentKey = nextKey( m_CurrentKey );
        if (m_CurrentKey == m_FirstKey) { // did just loop ?
          m_LastStartTime = tm;
        }
      } else {
        break;
      }
    }
    double currentkeytime = keyTimeStamp( m_CurrentKey ) - m_StartKeyTime;
    //sl_dbg_assert((tm-m_LastStartTime) >= currentkeytime);
    // find next key time
    m_NextKey          = nextKey    ( m_CurrentKey );
    double nextkeytime = nextKeyTime( m_CurrentKey );
    // compute interpolation value
    if (nextkeytime == currentkeytime) {
      m_InterpValue=0;
    } else {
      m_InterpValue = float( ( double(t_time(tm)-t_time(m_LastStartTime)) - currentkeytime ) / (nextkeytime - currentkeytime) );
    }

  } else { // reverse

    // check whether to go to previous key frame
    while (1) {
      if (!m_Loop && (m_CurrentKey == m_FirstKey)) {
        if (!m_Done && !m_Callback.isNull()) {
          m_Callback->execute();
        }
        m_Done=true;
        break;
      }
      if ( double( t_time(m_LastStartTime) - t_time(tm) ) < prevKeyTime( m_CurrentKey ) ) {
        //cerr << sprint("current = %d, going to %d, (m_LastStartTime-tm = %f)\n",m_CurrentKey,prevKey(m_CurrentKey),float(m_LastStartTime-tm));
        m_CurrentKey = prevKey( m_CurrentKey );
        if (m_CurrentKey == m_LastKey) { // did just loop ?
          m_LastStartTime = tm;
        }
      } else {
        break;
      }
    }
    double currentkeytime = keyTimeStamp(m_CurrentKey)-m_StartKeyTime;
    //sl_dbg_assert((m_LastStartTime-tm) <= currentkeytime);
    // find next key time
    m_NextKey          = prevKey    ( m_CurrentKey );
    double nextkeytime = prevKeyTime( m_CurrentKey );
    // compute interpolation value
    if (nextkeytime == currentkeytime) {
      m_InterpValue = 0;
    } else {
      m_InterpValue = float( ( double( t_time(m_LastStartTime) - t_time(tm) ) - currentkeytime ) / ( nextkeytime - currentkeytime ) );
    }

  }

  // update matrices
  if (updatematrices) {
    updateMatrices();
  }

  LIBSL_END;
}

//---------------------------------------------------------------------------

void NAMESPACE::KeyframeController::setCurrentFrame(uint k)
{
  sl_assert(k >= m_FirstKey && k <= m_LastKey); 
  // adjust time
  double tmcurrent = keyTimeStamp(m_CurrentKey)-m_StartKeyTime;
  double tmnew     = keyTimeStamp(k)           -m_StartKeyTime;
  t_time tm        = time();
  // set key
  m_CurrentKey=k;
  if (!m_Reverse) {
    m_NextKey       = nextKey(k);
    m_LastStartTime = tm - t_time(tmnew + tmcurrent);
  } else {
    m_NextKey       = prevKey(k);
    m_LastStartTime = tm + t_time(tmnew - tmcurrent);
  }
  m_InterpValue=0;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/// Transition controller

NAMESPACE::TransitionController::TransitionController(
  LibSL::Mesh::AnimatedMesh_Ptr m,
  AnimatedMeshController::t_AutoPtr a,
  AnimatedMeshController::t_AutoPtr b,
  float duration)
  : NAMESPACE::AnimatedMeshController(m)
{
  sl_assert(!a.isNull() && !b.isNull());
  m_ControllerA = a;
  m_ControllerB = b;
  m_Duration    = duration;
  m_StartTime   = time();
  m_InterpValue = 0;
  // init matrices
  updateMatrices();
}

//---------------------------------------------------------------------------

void NAMESPACE::TransitionController::computeAnimatedFrame(
  Hierarchy<AnimatedMesh::t_frame>::t_AutoPtr node,quatf& _q,v3f& _t,v3f& _s)
{
  if (m_InterpValue > 1.0) {
    m_ControllerB->computeAnimatedFrame(node,_q,_t,_s);
  } else {
    quatf qA; v3f tA,sA;
    m_ControllerA->computeAnimatedFrame(node,qA,tA,sA);
    quatf qB; v3f tB,sB;
    m_ControllerB->computeAnimatedFrame(node,qB,tB,sB);

    _q = quatf::slerp(m_InterpValue,qA,qB);
    _t = tA * (1.0f-m_InterpValue) + tB * m_InterpValue;
    _s = sA * (1.0f-m_InterpValue) + sB * m_InterpValue;
  }
}

//---------------------------------------------------------------------------

bool NAMESPACE::TransitionController::done()
{
  return ((time()-m_StartTime) > m_Duration);
}

//---------------------------------------------------------------------------

void NAMESPACE::TransitionController::animate(float elapsed,bool updatematrices)
{
  m_ControllerB->animate(elapsed,false);
  if (m_InterpValue > 1.0) {
    if (!m_ControllerA.isNull()) {
      m_ControllerA = AnimatedMeshController::t_AutoPtr();
    }
  } else {
    m_ControllerA->animate(elapsed,false);
    m_InterpValue = float( (time()-m_StartTime)/m_Duration );
  }

  // update matrices
  if (updatematrices) {
    updateMatrices();
  }
}

//---------------------------------------------------------------------------

v3f NAMESPACE::TransitionController::motionTranslation()
{
  if (m_InterpValue > 1.0) {
    return (m_ControllerB->motionTranslation());
  } else {
    return (m_ControllerA->motionTranslation()*(1.0f-m_InterpValue) + m_ControllerB->motionTranslation()*m_InterpValue);
  }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/// Combine controller

NAMESPACE::CombineController::CombineController(
  LibSL::Mesh::AnimatedMesh_Ptr m,
  AnimatedMeshController::t_AutoPtr a,
  AnimatedMeshController::t_AutoPtr b)
  : NAMESPACE::AnimatedMeshController(m)
{
  sl_assert(!a.isNull() && !b.isNull());
  m_ControllerA = a;
  m_ControllerB = b;
  // init matrices
  updateMatrices();
}

//---------------------------------------------------------------------------

void NAMESPACE::CombineController::computeAnimatedFrame(
  Hierarchy<AnimatedMesh::t_frame>::t_AutoPtr node,quatf& _q,v3f& _t,v3f& _s)
{
  quatf qA; v3f tA,sA;
  m_ControllerA->computeAnimatedFrame(node,qA,tA,sA);
  quatf qB; v3f tB,sB;
  m_ControllerB->computeAnimatedFrame(node,qB,tB,sB);

  _q = qA*qB;
  _t = tA+tB;
  _s = sA*sB;
}

//---------------------------------------------------------------------------

void NAMESPACE::CombineController::animate(float elapsed,bool updatematrices)
{
  m_ControllerA->animate(elapsed,false);
  m_ControllerB->animate(elapsed,false);
  // update matrices
  if (updatematrices) {
    updateMatrices();
  }
}

//---------------------------------------------------------------------------

v3f NAMESPACE::CombineController::motionTranslation()
{
  return (m_ControllerA->motionTranslation() + m_ControllerB->motionTranslation());
}

//---------------------------------------------------------------------------
