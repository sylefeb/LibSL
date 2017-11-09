//---------------------------------------------------------------------------

#include <LibSL/LibSL.h>
#include "AnimatedMeshFormat_animesh.h"

#include <string>
using namespace std;

//---------------------------------------------------------------------------

#define NAMESPACE LibSL::Mesh

//---------------------------------------------------------------------------

NAMESPACE::AnimatedMeshFormat_animesh::AnimatedMeshFormat_animesh()
{
  try {
    // register plugin
    ANIMATEDMESH_FORMAT_MANAGER.registerPlugin(this);
  } catch (LibSL::Errors::Fatal& e) {
    std::cerr << e.message() << std::endl;
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshFormat_animesh::writeFrame(obinstream& o,const AnimatedMesh::t_frame& f) const
{
  o << f.name;
  o << f.hasMesh;
  ForIndex(i,4) {
    ForIndex(j,4) {
      o << f.frameMatrix.at(i,j);
    }
  }
  o << f.boneIndex;
  ForIndex(i,4) {
    ForIndex(j,4) {
      o << f.boneOffset.at(i,j);
    }
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshFormat_animesh::readFrame(ibinstream& s,AnimatedMesh::t_frame& f) const
{
  s >> f.name;
  s >> f.hasMesh;
  ForIndex(i,4) {
    ForIndex(j,4) {
      s >> f.frameMatrix.at(i,j);
    }
  }
  s >> f.boneIndex;
  ForIndex(i,4) {
    ForIndex(j,4) {
      s >> f.boneOffset.at(i,j);
    }
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshFormat_animesh::writeSkinVertex(obinstream& o,const AnimatedMesh::t_skinvertex& v) const
{
  ForIndex(i,3) {
    o << v.pos[i];
  }
  ForIndex(i,3) {
    o << v.nrm[i];
  }
  ForIndex(i,2) {
    o << v.uv[i];
  }
  ForIndex(i,3) {
    o << v.weight[i];
  }
  ForIndex(i,4) {
    o << v.idx[i];
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshFormat_animesh::readSkinVertex(ibinstream& s,AnimatedMesh::t_skinvertex& v) const
{
  ForIndex(i,3) {
    s >> v.pos[i];
  }
  ForIndex(i,3) {
    s >> v.nrm[i];
  }
  ForIndex(i,2) {
    s >> v.uv[i];
  }
  ForIndex(i,3) {
    s >> v.weight[i];
  }
  ForIndex(i,4) {
    s >> v.idx[i];
  }  
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshFormat_animesh::writeSkinnedMesh(obinstream& o,const AnimatedMesh::t_skinnedmesh& m) const
{
  o << m.vertices.size();
  ForIndex(i,m.vertices.size()) {
    writeSkinVertex(o,m.vertices[i]);
  }
  o << m.indices.size();
  ForIndex(i,m.indices.size()) {
    o << m.indices[i];
  }
  o << m.bonespervertex;
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshFormat_animesh::readSkinnedMesh(ibinstream& s,AnimatedMesh::t_skinnedmesh& m) const
{
  uint n=0;
  s >> n;
  m.vertices.allocate(n);
  ForIndex(i,m.vertices.size()) {
    readSkinVertex(s,m.vertices[i]);
  }
  s >> n;
  m.indices.allocate(n);
  ForIndex(i,m.indices.size()) {
    s >> m.indices[i];
  }
  s >> m.bonespervertex;
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshFormat_animesh::writeBoneKeys(obinstream& o,const AnimatedMesh::t_bonekeys& b) const
{
  o << b.name;
  o << b.quaternions.size();
  ForIndex(i,b.quaternions.size()) {
    ForIndex(j,4) {
      o << b.quaternions[i].first;
      o << b.quaternions[i].second[j];
    }
  }
  o << b.translations.size();
  ForIndex(i,b.translations.size()) {
    ForIndex(j,3) {
      o << b.translations[i].first;
      o << b.translations[i].second[j];
    }
  }
  o << b.scalings.size();
  ForIndex(i,b.scalings.size()) {
    ForIndex(j,3) {
      o << b.scalings[i].first;
      o << b.scalings[i].second[j];
    }
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshFormat_animesh::readBoneKeys(ibinstream& s,AnimatedMesh::t_bonekeys& b) const
{
  s >> b.name;
  uint n=0;
  s >> n;
  b.quaternions.allocate(n);
  ForIndex(i,b.quaternions.size()) {
    ForIndex(j,4) {
      s >> b.quaternions[i].first;
      s >> b.quaternions[i].second[j];
    }
  }
  s >> n;
  b.translations.allocate(n);
  ForIndex(i,b.translations.size()) {
    ForIndex(j,3) {
      s >> b.translations[i].first;
      s >> b.translations[i].second[j];
    }
  }
  s >> n;
  b.scalings.allocate(n);
  ForIndex(i,b.scalings.size()) {
    ForIndex(j,3) {
      s >> b.scalings[i].first;
      s >> b.scalings[i].second[j];
    }
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshFormat_animesh::writeKeyFrameAnimation(obinstream& o,const AnimatedMesh::t_keyframeanimation& a) const
{
  o << a.name;
  o << a.numkeys;
  o << a.bonekeys.size();
  ForIndex(i,a.bonekeys.size()) {
    writeBoneKeys(o,a.bonekeys[i]);
  }
  o << uint(a.boneNametoAnimationIndex.size());
  for (std::map<std::string,uint>::const_iterator I=a.boneNametoAnimationIndex.begin();
    I!=a.boneNametoAnimationIndex.end();I++) {
      o << (*I).first;
      o << (*I).second;
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshFormat_animesh::readKeyFrameAnimation(ibinstream& s,AnimatedMesh::t_keyframeanimation& a) const
{
  s >> a.name;
  s >> a.numkeys;
  uint n = 0;
  s >> n;
  a.bonekeys.allocate(n);
  ForIndex(i,a.bonekeys.size()) {
    readBoneKeys(s,a.bonekeys[i]);
  }
  s >> n;
  ForIndex(i,n) {
    string boneName;
    uint   index;
    s >> boneName;
    s >> index;
    a.boneNametoAnimationIndex[boneName] = index;
  }
  sl_assert(a.boneNametoAnimationIndex.size() == n);
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshFormat_animesh::writeAnimations(LibSL::CppHelpers::obinstream& o,const Array<AnimatedMesh::t_keyframeanimation>& a) const
{
  o << a.size();
  ForIndex(i,a.size()) {
    writeKeyFrameAnimation(o,a[i]);
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshFormat_animesh::readAnimations(LibSL::CppHelpers::ibinstream& s,
                                                           AnimatedMesh *am) const
{
  uint n = 0;
  s >> n;
  am->animations().allocate(n);
  ForIndex(i,am->animations().size()) {
    readKeyFrameAnimation(s,am->animations()[i]);
    am->animationsByName().insert( make_pair(am->animations()[i].name,i) );
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshFormat_animesh::writeSkeleton(obinstream& o,LibSL::DataStructures::Hierarchy<AnimatedMesh::t_frame>::t_AutoPtr node) const
{
  writeFrame(o,node->data());
  o << node->numChildren();
  for (LibSL::DataStructures::Hierarchy<AnimatedMesh::t_frame>::ChildrenIterator C=node->children();
    !C.end();C.next()) {
    writeSkeleton(o,C.current());
  }
}

//---------------------------------------------------------------------------

LibSL::DataStructures::Hierarchy<typename NAMESPACE::AnimatedMesh::t_frame>::t_AutoPtr 
NAMESPACE::AnimatedMeshFormat_animesh::readSkeleton(ibinstream& s) const
{
  AnimatedMesh::t_frame frame;
  readFrame(s,frame);
  LibSL::DataStructures::Hierarchy<typename AnimatedMesh::t_frame>::t_AutoPtr node
    = LibSL::DataStructures::Hierarchy<typename AnimatedMesh::t_frame>::t_AutoPtr(new LibSL::DataStructures::Hierarchy<typename AnimatedMesh::t_frame>(frame));
  uint n=0;
  s >> n;
  ForIndex (c,n) {
    LibSL::DataStructures::Hierarchy<typename AnimatedMesh::t_frame>::t_AutoPtr child = readSkeleton(s);
    node->addChild(child);
  }
  return (node);
}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshFormat_animesh::save(const char *fname,const AnimatedMesh *am) const
{
  obinstream f(fname,ios::out | ios::binary);
  string vid = "LibSL_animesh_1";
  f << vid;
  f << am->numBones();
  writeSkeleton   (f,am->skeleton());
  writeAnimations (f,am->animations());
  writeSkinnedMesh(f,am->mesh());
  ForIndex(n,3) {
    f << am->center()[n];
  }
  f << am->radius();
  f.close();
}

//---------------------------------------------------------------------------

AnimatedMesh *NAMESPACE::AnimatedMeshFormat_animesh::load(const char *fname) const 
{
  AnimatedMesh *am = new AnimatedMesh();
  ibinstream f(fname,ios::in | ios::binary);
  string vid;
  f >> vid;
  if (vid != "LibSL_animesh_1") {
    throw Fatal("[AnimatedMeshFormat_animesh::load] - file '%s' version mismatch",fname);
  }
  f >> am->numBones();
  am->skeleton() = readSkeleton(f);
  readAnimations (f,am);
  readSkinnedMesh(f,am->mesh());
  ForIndex(n,3) {
    f >> am->center()[n];
  }
  f >> am->radius();
  f.close();
  return am;
}

//---------------------------------------------------------------------------
