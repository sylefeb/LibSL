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

#include "MeshFormat_3DS.h"
using namespace LibSL::Mesh;

#include <LibSL/Errors/Errors.h>
using namespace LibSL::Errors;
#include <LibSL/Memory/Array.h>
using namespace LibSL::Memory::Array;
#include <LibSL/Memory/Pointer.h>
using namespace LibSL::Memory::Pointer;
#include <LibSL/Math/Vertex.h>
using namespace LibSL::Math;

extern "C" {
#include <lib3ds/src/lib3ds.h>
}

//---------------------------------------------------------------------------

#define NAMESPACE LibSL::Mesh

//---------------------------------------------------------------------------

/// Declaring a global will automatically register the plugin
namespace {
  NAMESPACE::MeshFormat_3DS s_3DS;  /// FIXME: this mechanism does not work with VC++
}                                   ///        see also MeshFormatManager constructor

//---------------------------------------------------------------------------

NAMESPACE::MeshFormat_3DS::MeshFormat_3DS()
{
  try {
    // register plugin
    MESH_FORMAT_MANAGER.registerPlugin(this);
  } catch (LibSL::Errors::Fatal& e) {
    std::cerr << e.message() << std::endl;
  }
}

//---------------------------------------------------------------------------

NAMESPACE::TriangleMesh *NAMESPACE::MeshFormat_3DS::load(const char *fname) const
{
  unsigned       i;

  Lib3dsFile    *model;
  float        **normals;

  const char    *order = "xyz";

  model = lib3ds_file_open(fname);
  if (NULL == model) {
    throw Fatal("[MeshFormat_3DS::load] - file '%s' not found",fname);
  }

  // count
  uint numvert  =0;
  uint numtris  =0;
  uint nummeshes=0;
  ForIndex(n,model->nmeshes) {
    numvert  += model->meshes[n]->nvertices;
    numtris  += model->meshes[n]->nfaces;
    nummeshes ++;
  }
  uint numsrfs = model->nmaterials;

  TriangleMesh_generic<MeshFormat_3DS::t_VertexData> *mesh
    = new TriangleMesh_generic<MeshFormat_3DS::t_VertexData>(numvert,numtris,numsrfs,AutoPtr<MVF>(MVF::make<MeshFormat_3DS::t_VertexFormat>()));
  std::cerr << "[3ds] " << numvert << " vertices, " << numtris << " triangles " << numsrfs << " surfaces" << std::endl;
  normals = new float*[nummeshes];
  // reorder and compute normals
  ForIndex(n,model->nmeshes) {
    for (i=0; i<model->meshes[n]->nvertices; i++) {
      float x = model->meshes[n]->vertices[i][order[0]-'x'];
      float y = model->meshes[n]->vertices[i][order[1]-'x'];
      float z = model->meshes[n]->vertices[i][order[2]-'x'];
      model->meshes[n]->vertices[i][0] = x;
      model->meshes[n]->vertices[i][1] = y;
      model->meshes[n]->vertices[i][2] = z;
    }
    normals[n] = new float[(3*3*sizeof(float)*model->meshes[n]->nfaces)];
    lib3ds_mesh_calculate_vertex_normals(model->meshes[n],(float(*)[3])normals[n]);
  }

  std::cerr << "[3DS] num materials: " << model->materials_size << std::endl;

  // for each model face
  uint fi  =0;
  uint offs=0;
  Array< std::vector<uint> > srfs;
  srfs.allocate( numsrfs );
  ForIndex(n,model->nmeshes) {
    ForIndex(f,model->meshes[n]->nfaces) {
      mesh->triangleAt(fi)[0] = model->meshes[n]->faces[f].index[0] + offs;
      mesh->triangleAt(fi)[1] = model->meshes[n]->faces[f].index[1] + offs;
      mesh->triangleAt(fi)[2] = model->meshes[n]->faces[f].index[2] + offs;
      // normals
      ForIndex(k,3) {
        mesh->vertexAt(mesh->triangleAt(fi)[k]).nrm[0] = normals[n][3*(f*3+k)+order[0]-'x'];
        mesh->vertexAt(mesh->triangleAt(fi)[k]).nrm[1] = normals[n][3*(f*3+k)+order[1]-'x'];
        mesh->vertexAt(mesh->triangleAt(fi)[k]).nrm[2] = normals[n][3*(f*3+k)+order[2]-'x'];
      }
      // surface
      if (model->meshes[n]->faces[f].material > -1) {
        sl_assert(model->meshes[n]->faces[f].material < (int)numsrfs);
        srfs[ model->meshes[n]->faces[f].material ].push_back( fi );
      }
      // next triangle
      fi ++;
    }
    offs += model->meshes[n]->nvertices;
  }

  // for each model vertex
  uint pi =0;
  ForIndex(n,model->nmeshes) {
    ForIndex(p,model->meshes[n]->nvertices) {
      v3f pt = V3F(
        model->meshes[n]->vertices[p][0],
        model->meshes[n]->vertices[p][1],
        model->meshes[n]->vertices[p][2]);
      mesh->vertexAt(pi).pos = pt;
      if (model->meshes[n]->texcos != NULL) {
        mesh->vertexAt(pi).uv = V2F(float(model->meshes[n]->texcos[p][0]),1.0f - float(model->meshes[n]->texcos[p][1]));
      } else {
        mesh->vertexAt(pi).uv = 0.0f;
      }
      pi++;
    }
  }

  // add surfaces
  ForIndex(s,numsrfs) {
      mesh->surfaceAt(s).triangleIds.allocate( (uint)srfs[s].size() );
      ForIndex(t,srfs[s].size()) {
        mesh->surfaceAt(s).triangleIds[t] = srfs[s][t];
      }
      float *diffuse             = model->materials[s]->diffuse;
      mesh->surfaceAt(s).diffuse = V3F(diffuse[0],diffuse[1],diffuse[2]);
  }

  return (mesh);

//    throw Fatal("MeshFormat_3DS::load - unsupported number of components (%d, %s)",cinfo.output_components,name);
}

//---------------------------------------------------------------------------

void NAMESPACE::MeshFormat_3DS::save(const char *fname,const NAMESPACE::TriangleMesh *mesh) const
{
  throw Fatal("MeshFormat_3DS::save - Sorry, cannot save in 3DS format (NYI): %s",fname);
}

//---------------------------------------------------------------------------
