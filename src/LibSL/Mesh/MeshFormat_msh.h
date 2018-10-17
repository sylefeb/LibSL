// ------------------------------------------------------
// LibSL::Mesh::MeshFormat_msh
// ------------------------------------------------------
//
// Load/save LibSL 'msh' file format
//
// ------------------------------------------------------
// Jimmy ETIENNE - 2018-10-11
// ------------------------------------------------------

#pragma once

#include <LibSL/Math/Vertex.h>
#include <LibSL/Mesh/Mesh.h>

namespace LibSL {
  namespace Mesh {

    class MeshFormat_msh : public TriangleMeshFormat_plugin
    {
    public:

      typedef struct
      {
        LibSL::Math::v3f pos;
        LibSL::Math::v3f nrm;
      } t_VertexData;

      typedef MVF2(mvf_position_3f, mvf_normal_3f) t_VertexFormat;

    public:
      MeshFormat_msh();
      void           save(const char *,const TriangleMesh *) const;
      TriangleMesh  *load(const char *)                      const;
      const char    *signature()                             const {return "msh";}
    };

  } //namespace LibSL::Mesh
} //namespace LibSL

// ------------------------------------------------------
