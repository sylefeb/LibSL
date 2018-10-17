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

float version_number;
int    file_type; // 0 = ASCII, 1 = binary
int    data_size = 0;
int    number_of_nodes;
int    number_of_elements;
int    number_of_tags;

int number_of_string_tags;
int number_of_real_tags;
int number_of_integer_tags;


vector<v3f> nodes;
vector<v4i> tetrahedrons;

vector<v3f> normals;

vector<string> string_tags;
vector<float>  real_tags;
vector<int>    integer_tags;

//---------------------------------------------------------------------------

void parseWhiteSpace(ifstream& stream) {
  char next = stream.peek();
  while (next == '\n' || next == ' ' || next == '\t' || next == '\r') {
    stream.get();
    next = stream.peek();
  }
}

void parseMeshFormat(ifstream& stream) {
  stream >> version_number;
  parseWhiteSpace(stream);
  stream >> file_type;
  parseWhiteSpace(stream);
  stream >> data_size;
  parseWhiteSpace(stream);

  if (file_type == 1) { // binary
    int check = 1;
    stream.read(reinterpret_cast<char*>(&check), sizeof(int));
    sl_assert(check == 1); // make sure the file is compatible (little or big endian)
  }
}

void parsePhysicalNames(ifstream& stream) {
}

void parseNodes(ifstream& stream) {
  stream >> number_of_nodes;
  parseWhiteSpace(stream);

  int num_i;
  double xyz[3];
  for (int i = 0; i < number_of_nodes; i++) {
    stream.read(reinterpret_cast<char*>(&num_i), sizeof(int));
    stream.read(reinterpret_cast<char*>(&xyz),   sizeof(xyz));
    nodes.emplace_back(v3f(xyz[0], xyz[1], xyz[2]));
  }
}

void parseElements(ifstream& stream) {
  stream >> number_of_elements;
  parseWhiteSpace(stream);

  int header[3];
  int nb_elems = 0;
  while (nb_elems < number_of_elements) {
    stream.read(reinterpret_cast<char*>(&header), sizeof(header));

    switch (header[0]) {
    case 1: // 2-node line
      break;
    case 2: // 3-node triangle
      break;
    case 3: // 4-node quadrangle
      break;
    case 4: // 4-node tetrahedron 
    {
      for (int elem = 0; elem < header[1]; elem++) {
        for (int tag = 0; tag < header[2]; tag++) {
          int data;
          stream.read(reinterpret_cast<char*>(&data), sizeof(data));
        }
        int num_i;
        stream.read(reinterpret_cast<char*>(&num_i), sizeof(num_i));

        int data[4];
        stream.read(reinterpret_cast<char*>(&data), sizeof(data));
        tetrahedrons.emplace_back(v4i(data[0], data[1], data[2], data[3]));
      }
      nb_elems += header[1];
    }
    break;
    case 5: // 8-node hexahedron
      break;
    case 6: 
      break;
    }
  }
}

void parseElementData(ifstream& stream) {
  {
    stream >> number_of_string_tags;
    std::string tag;
    ForIndex(i, number_of_string_tags) {
      stream >> tag;
      string_tags.emplace_back(tag);
      parseWhiteSpace(stream);
    }
  }
  {
    stream >> number_of_real_tags;
    float tag;
    ForIndex(i, number_of_real_tags) {
      stream >> tag;
      real_tags.emplace_back(tag);
      parseWhiteSpace(stream);
    }
  }
  {
    stream >> number_of_integer_tags;
    int tag;
    ForIndex(i, number_of_integer_tags) {
      stream >> tag;
      integer_tags.emplace_back(tag);
      parseWhiteSpace(stream);
    }
  }

  int nb_values = integer_tags[1];
  int nb_elems  = integer_tags[2];

  size_t num_bytes = (nb_values * data_size + 4) * nb_elems;
  char* data = new char[num_bytes];
  stream.read(data, num_bytes);

  vector<vector<float>> field(nb_elems);
  
  for (size_t i = 0; i < nb_elems; i++) {
    int elem = *reinterpret_cast<int*>(&data[i*(4 + nb_values * data_size)]);
    elem -= 1;
    size_t base_idx = i * (4 + nb_values * data_size) + 4;

    field[elem * nb_values] = vector<float>(nb_values);
    for (size_t j = 0; j < nb_values; j++) {
      field[elem * nb_values][j] = *reinterpret_cast<float*>(&data[base_idx + j * data_size]);
    }
  }

  /*ForIndex(i, number_of_nodes) {
    stream.read(reinterpret_cast<char*>(&elm_number), sizeof(elm_number));
    stream.read(reinterpret_cast<char*>(&value), sizeof(value));
  }*/
}

//---------------------------------------------------------------------------



NAMESPACE::TriangleMesh *NAMESPACE::MeshFormat_msh::load(const char *fname) const
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

  std::string blockname, blockend;
  while (!stream.eof()) {
    stream >> blockname;

    if (blockname == "$MeshFormat") {
      parseMeshFormat(stream);
      stream >> blockend;
      sl_assert(blockend == "$EndMeshFormat");
    } else if (blockname == "$PhysicalNames") {
      parsePhysicalNames(stream);
      stream >> blockend;
      sl_assert(blockend == "$EndPhysicalNames");
    } else if (blockname == "$Nodes") {
      parseNodes(stream);
      stream >> blockend;
      sl_assert(blockend == "$EndNodes");
    } else if (blockname == "$Elements") {
      parseElements(stream);
      stream >> blockend;
      sl_assert(blockend == "$EndElements");
    } else if (blockname == "$ElementData") {
      parseElementData(stream);
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
    = new TriangleMesh_generic<MeshFormat_msh::t_VertexData>(number_of_nodes, number_of_elements * 4, 0, AutoPtr<MVF>(MVF::make<MeshFormat_msh::t_VertexFormat>()));

  // faces
  uint fi = 0;
  ForArray(tetrahedrons, n_tet) {
    ForIndex(off, 4) {
      ForIndex(k, 3) {
        int vertex_number = tetrahedrons.at(n_tet)[(off + k) % 4];
        mesh->triangleAt(fi)[k] = vertex_number - 1;
      }
      fi++;
    }
  }

  //vertices
  ForArray (nodes, n_ver) {
    v3f pos = nodes.at(n_ver);
    mesh->vertexAt(n_ver).pos = pos;
  }

  //normals
  normals.resize(nodes.size());
  ForIndex(n, normals.size()) {
    normals[n] = 0;
  }
  v3f p[3];
  ForIndex(t, mesh->numTriangles() - 1) {
    ForIndex(i, 3) {
      p[i] = nodes[mesh->triangleAt(t)[i]];
    }

    v3f nrm = normalize_safe(cross(p[1] - p[0], p[2] - p[0]));
    ForIndex(i, 3) {
      normals[mesh->triangleAt(t)[i]] = nrm;
    }
  }
  ForArray(nodes, n) {
    mesh->vertexAt(n).nrm = normalize_safe(normals[n]);
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
