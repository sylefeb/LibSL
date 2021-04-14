//---------------------------------------------------------------------------
#include "LibSL.precompiled.h"
//---------------------------------------------------------------------------

#include "MeshFormat_msh.h"
using namespace LibSL::Mesh;

#include <LibSL/Errors/Errors.h>
using namespace LibSL::Errors;
#include <LibSL/Memory/Array.h>
using namespace LibSL::Memory::Array;
#include <LibSL/Memory/Pointer.h>
using namespace LibSL::Memory::Pointer;
#include <LibSL/Math/Vertex.h>
using namespace LibSL::Math;

//---------------------------------------------------------------------------

#define NAMESPACE LibSL::Mesh

//---------------------------------------------------------------------------

/// Declaring a global will automatically register the plugin
namespace {
  NAMESPACE::MeshFormat_mesh s_Mesh;  /// FIXME: this mechanism does not work with VC++
}                                     ///        see also MeshFormatManager constructor

//---------------------------------------------------------------------------

NAMESPACE::MeshFormat_msh::MeshFormat_msh()
{
  try {
    // register plugin
    MESH_FORMAT_MANAGER.registerPlugin(this);
  } catch (LibSL::Errors::Fatal& e) {
    std::cerr << e.message() << std::endl;
  }
}

//---------------------------------------------------------------------------

using namespace std;

//---------------------------------------------------------------------------

void parseWhiteSpace(ifstream& stream) {
  char next = stream.peek();
  while (next == '\n' || next == ' ' || next == '\t' || next == '\r') {
    stream.get();
    next = stream.peek();
  }
}

void parseMeshFormat(ifstream& stream, MeshFormat_msh::msh_format_info& format) {
  stream >> format.version_number;
  parseWhiteSpace(stream);
  stream >> format.file_type;
  parseWhiteSpace(stream);
  stream >> format.data_size;
  parseWhiteSpace(stream);

  if (format.file_type == 1) { // binary
    int check = 1;
    stream.read(reinterpret_cast<char*>(&check), sizeof(int));
    sl_assert(check == 1); // make sure the file is compatible (little or big endian)
  }
}

void parsePhysicalNames(ifstream& stream, MeshFormat_msh::msh_format_info& format) {
}

void parseNodes(ifstream& stream, MeshFormat_msh::msh_format_info& format) {
  stream >> format.number_of_nodes;
  parseWhiteSpace(stream);

  int num_i;
  double xyz[3];
  
  if (format.file_type == 1) {
    for (int i = 0; i < format.number_of_nodes; i++) {
	  stream.read(reinterpret_cast<char*>(&num_i), sizeof(int));
	  stream.read(reinterpret_cast<char*>(&xyz),   sizeof(xyz));
	  format.nodes.emplace_back(v3f(xyz[0], xyz[1], xyz[2]));
    }
  } else {
	  for (int i = 0; i < format.number_of_nodes; i++) {
      char trash;
      stream >> num_i;
	    stream >> xyz[0] >> trash >> xyz[1] >> trash >> xyz[2];
	    parseWhiteSpace(stream);
	  }
  }
}

void parseElements(ifstream& stream, MeshFormat_msh::msh_format_info& format) {
  stream >> format.number_of_elements;
  parseWhiteSpace(stream);

  int header[3];
  int nb_elems = 0;
  while (nb_elems < format.number_of_elements) {
    
	if (format.file_type == 1) {
	  stream.read(reinterpret_cast<char*>(&header), sizeof(header));
    } else {
	  stream >> header[0];
	  parseWhiteSpace(stream);
	}
	
	int elem_size = 0;
	
    switch (header[0]) {
    case 1: // 2-node line
      break;
    case 2: // 3-node triangle
	  elem_size = 3;
      break;
    case 3: // 4-node quadrangle
      elem_size = 4;
	  break;
    case 4: // 4-node tetrahedron 
      elem_size = 4;
      break;
    case 5: // 8-node hexahedron
      break;
    case 6: 
      break;
    }
	
	for (int elem = 0; elem < header[1]; elem++) {
      for (int tag = 0; tag < header[2]; tag++) {
        int data;
        stream.read(reinterpret_cast<char*>(&data), sizeof(data));
      }
      int num_i;
      stream.read(reinterpret_cast<char*>(&num_i), sizeof(num_i));

      int data[4];
      stream.read(reinterpret_cast<char*>(&data), sizeof(data));
      vector<int> elems;
	  
	    ForIndex (i, elem_size) {
	      elems.emplace_back(data[i]);
	    }
	    format.elements.emplace_back(elems);
    }
    nb_elems += header[1];
  }
}

void parseElementData(ifstream& stream, MeshFormat_msh::msh_format_info& format) {
  {
    stream >> format.number_of_string_tags;
    std::string tag;
    ForIndex(i, format.number_of_string_tags) {
      stream >> tag;
      format.string_tags.emplace_back(tag);
      parseWhiteSpace(stream);
    }
  }
  {
    stream >> format.number_of_real_tags;
    float tag;
    ForIndex(i, format.number_of_real_tags) {
      stream >> tag;
      format.real_tags.emplace_back(tag);
      parseWhiteSpace(stream);
    }
  }
  {
    stream >> format.number_of_integer_tags;
    int tag;
    ForIndex(i, format.number_of_integer_tags) {
      stream >> tag;
      format.integer_tags.emplace_back(tag);
      parseWhiteSpace(stream);
    }
  }

  size_t nb_values = format.integer_tags[1];
  size_t nb_elems  = format.integer_tags[2];

  size_t num_bytes = (nb_values * format.data_size + 4) * nb_elems;
  char* data = new char[num_bytes];
  stream.read(data, num_bytes);

  vector<vector<float>> field(nb_elems);
  
  for (size_t i = 0; i < nb_elems; i++) {
    int elem = *reinterpret_cast<int*>(&data[i*(4 + nb_values * format.data_size)]);
    elem -= 1;
    size_t base_idx = i * (4 + nb_values * format.data_size) + 4;

    field[elem * nb_values] = vector<float>(nb_values);
    for (size_t j = 0; j < nb_values; j++) {
      field[elem * nb_values][j] = *reinterpret_cast<float*>(&data[base_idx + j * format.data_size]);
    }
  }

  /*ForIndex(i, number_of_nodes) {
    stream.read(reinterpret_cast<char*>(&elm_number), sizeof(elm_number));
    stream.read(reinterpret_cast<char*>(&value), sizeof(value));
  }*/
}

//---------------------------------------------------------------------------



NAMESPACE::TriangleMesh* NAMESPACE::MeshFormat_msh::load(const char *fname) const
{
	size_t nr = 0;
	LIBSL_BEGIN;
	// open file
	FILE *f = NULL;
	fopen_s(&f, fname, "rb");
	if (f == NULL) {
		throw Fatal("[MeshFormat_msh::load] - file '%s' not found",fname);
	}

  ifstream stream(f);

  msh_format_info format;

  std::string blockname, blockend;
  while (!stream.eof()) {
    stream >> blockname;

    if (blockname == "$MeshFormat") {
      parseMeshFormat(stream, format);
      stream >> blockend;
      sl_assert(blockend == "$EndMeshFormat");
    } else if (blockname == "$PhysicalNames") {
      parsePhysicalNames(stream, format);
      stream >> blockend;
      sl_assert(blockend == "$EndPhysicalNames");
    } else if (blockname == "$Nodes") {
      parseNodes(stream, format);
      stream >> blockend;
      sl_assert(blockend == "$EndNodes");
    } else if (blockname == "$Elements") {
      parseElements(stream, format);
      stream >> blockend;
      sl_assert(blockend == "$EndElements");
    } else if (blockname == "$ElementData") {
      parseElementData(stream, format);
      stream >> blockend;
      sl_assert(blockend == "$EndElementData");
    } else {
      cerr << "Unknown field" << endl;
      // TODO: READ UNTIL END OF BLOCK
      stream >> blockend;
      sl_assert(blockend == "$End" + blockname.substr(1, blockname.size() - 1));
    }

    parseWhiteSpace(stream);

  }
	fclose(f);



  TriangleMesh_generic<MeshFormat_msh::t_VertexData> *mesh
    = new TriangleMesh_generic<MeshFormat_msh::t_VertexData>(format.number_of_nodes, format.number_of_elements * 4, 0, AutoPtr<MVF>(MVF::make<MeshFormat_msh::t_VertexFormat>()));

  // faces
  uint fi = 0;
  ForArray(format.elements, n_tet) { // TODO: manage non tet
    ForIndex(off, 4) {
      ForIndex(k, 3) {
        int vertex_number = format.elements.at(n_tet)[(off + k) % 4];
        mesh->triangleAt(fi)[k] = vertex_number - 1;
      }
      m_meshes.push_back(format.elements.at(n_tet)[off] - 1);
      fi++;
    }
  }

  //vertices
  ForArray (format.nodes, n_ver) {
    v3f pos = format.nodes.at(n_ver);
    mesh->vertexAt(n_ver).pos = pos;
  }

	// done
	return (mesh);
	LIBSL_END;
}

//---------------------------------------------------------------------------

void NAMESPACE::MeshFormat_msh::save(const char *fname,const NAMESPACE::TriangleMesh *mesh) const
{
}

//---------------------------------------------------------------------------