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


      typedef struct
      {
        float version_number;
        int   file_type; // 0 = ASCII, 1 = binary
        int   data_size = 0;
        int   number_of_nodes;
        int   number_of_elements;
        int   number_of_tags;

        int number_of_string_tags;
        int number_of_real_tags;
        int number_of_integer_tags;

        std::vector<v3f>              nodes;
        std::vector<std::vector<int>> elements;

        std::vector<std::string> string_tags;
        std::vector<float>  real_tags;
        std::vector<int>    integer_tags;
      } msh_format_info;

    public:
      MeshFormat_msh();
      void           save(const char *,const TriangleMesh *) const;
      TriangleMesh  *load(const char *)                      const;
      const char    *signature()                             const {return "msh";}
    
    public:
      mutable std::vector<int> m_meshes;
    };

  } //namespace LibSL::Mesh
} //namespace LibSL

// ------------------------------------------------------
