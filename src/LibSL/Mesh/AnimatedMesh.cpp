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

#include "AnimatedMesh.h"

#include <LibSL/Errors/Errors.h>
using namespace LibSL::Errors;
#include <LibSL/Memory/Array.h>
using namespace LibSL::Memory::Array;
#include <LibSL/Memory/Pointer.h>
using namespace LibSL::Memory::Pointer;
#include <LibSL/CppHelpers/CppHelpers.h>
using namespace LibSL::CppHelpers;

using namespace LibSL::DataStructures;

#include <algorithm>
using namespace std;

//---------------------------------------------------------------------------

#define NAMESPACE LibSL::Mesh

//---------------------------------------------------------------------------

// AnimatedMesh format manager unique instance
NAMESPACE::AnimatedMeshFormatManager *NAMESPACE::AnimatedMeshFormatManager::s_Manager = NULL;

//---------------------------------------------------------------------------

NAMESPACE::AnimatedMeshFormatManager *NAMESPACE::AnimatedMeshFormatManager::getUniqueInstance()
{
  if (s_Manager == NULL) {
    s_Manager = new AnimatedMeshFormatManager();
  }
  return (s_Manager);
}

//---------------------------------------------------------------------------

NAMESPACE::AnimatedMeshFormatManager::AnimatedMeshFormatManager()
{

}

//---------------------------------------------------------------------------

void NAMESPACE::AnimatedMeshFormatManager::registerPlugin(AnimatedMeshFormat_plugin *plugin)
{
  std::map<std::string,AnimatedMeshFormat_plugin *>::iterator 
    P=m_Plugins.find(std::string(plugin->signature()));
  if (P == m_Plugins.end()) {
    m_Plugins[std::string(plugin->signature())]=plugin;
    std::cerr << sprint("[AnimatedMeshFormatManager] registered %s format.\n",plugin->signature());
  } else {
    LIBSL_FATAL_ERROR_WITH_ARGS("AnimatedMeshFormatManager::registerPlugin - plugin '%s' is already present !",plugin->signature());
  }
}

//---------------------------------------------------------------------------

NAMESPACE::AnimatedMeshFormat_plugin *
NAMESPACE::AnimatedMeshFormatManager::getPlugin(const char *signature) const
{
  std::string ext=std::string(signature);
  std::transform(ext.begin(),ext.end(), ext.begin(), StlHelpers::toLower<char>);
   std::map<std::string,NAMESPACE::AnimatedMeshFormat_plugin *>::const_iterator 
    P=m_Plugins.find(ext);
  if (P == m_Plugins.end()) {
    LIBSL_FATAL_ERROR_WITH_ARGS("AnimatedMeshFormatManager - Cannot find any plugin for '%s' (unknown format)",signature);
  } else {
    return ((*P).second);
  }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

NAMESPACE::AnimatedMesh *NAMESPACE::loadAnimatedMesh(const char *fname)
{
  NAMESPACE::AnimatedMeshFormatManager& 
    manager=(*NAMESPACE::AnimatedMeshFormatManager::getUniqueInstance());
  const char *pos=strrchr(fname,'.');
  if (pos == NULL) {
    LIBSL_FATAL_ERROR_WITH_ARGS("AnimatedMesh - Cannot determine file type ('%s')",fname);
  }
  return (manager.getPlugin(pos+1)->load(fname));
}

//---------------------------------------------------------------------------

void NAMESPACE::saveAnimatedMesh(const char *fname,const NAMESPACE::AnimatedMesh *amesh)
{
  NAMESPACE::AnimatedMeshFormatManager& 
    manager=(*NAMESPACE::AnimatedMeshFormatManager::getUniqueInstance());
  const char *pos=strrchr(fname,'.');
  if (pos == NULL) {
    LIBSL_FATAL_ERROR_WITH_ARGS("AnimatedMesh - Cannot determine file type ('%s')",fname);
  }
  manager.getPlugin(pos+1)->save(fname,amesh);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

NAMESPACE::AnimatedMesh::AnimatedMesh()
{
  m_NumBones=0;
}

//---------------------------------------------------------------------------

AnimatedMesh::t_AnimId NAMESPACE::AnimatedMesh::animationIdFromName(const std::string& name)
{
  std::map<std::string,t_AnimId>::iterator I = m_AnimationsByName.find(name);
  if (I == m_AnimationsByName.end()) {
    throw Fatal("[AnimatedMesh::animationIdFromName] - animation '%s' not found",name.c_str());
  }
  return (*I).second;
}

//---------------------------------------------------------------------------
