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

#include "MeshFormat_mesh.h"
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

NAMESPACE::MeshFormat_mesh::MeshFormat_mesh()
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

NAMESPACE::TriangleMesh *NAMESPACE::MeshFormat_mesh::load(const char *fname) const
{
	TriangleMesh_generic<Array<uchar> > *m = NULL;

	size_t nr = 0;
	LIBSL_BEGIN;
	// open file
	FILE *f = NULL;
	fopen_s(&f, fname, "rb");
	if (f == NULL) {
		throw Fatal("[MeshFormat_mesh::load] - file '%s' not found",fname);
	}
	// read vertices
	uint sizeofVertex = 0;
	nr = fread(&sizeofVertex,sizeof(uint),1,f);
	uint numv         = 0;
	nr = fread(&numv,sizeof(uint),1,f);
	Array<uchar> vertices(numv*sizeofVertex);
	fread(vertices.raw(),sizeofVertex,numv,f);
	// read indices
	uint numt         = 0;
	nr = fread(&numt,sizeof(uint),1,f);
	uint numi         = numt*3;
	Array<uint> indices(numi);
	nr = fread(indices.raw(),sizeof(uint),numi,f);
	// read surface information (if exists)
	uint nums = 0;
	uint r = uint(fread(&nums,sizeof(uint),1,f));
	Array<std::string>  srfTexNames;
	Array<Array<uint> > srfTris;
	Array<v3f         > srfDiffuse;
	static char buffer[1024];
	if (r == 1 && nums > 0) {
		LIBSL_BEGIN;    
		srfTexNames.allocate(nums);
		srfTris    .allocate(nums);
		srfDiffuse .allocate(nums);
		ForIndex(s,nums) {
			// read texture name
			uint len = 0;
			nr = fread(&len,sizeof(uint),1,f);
			if (len >= 1024) {
				throw Fatal("[MeshFormat_mesh::load] - texture name is too long (file '%s')",fname);
			}
			nr = fread(buffer,1,len+1,f);
			srfTexNames[s]  = std::string(buffer);
			// read surface triangle ids
			uint numst = 0;
			nr = fread(&numst,sizeof(uint),1,f);
			srfTris[s].allocate(numst);
			// read ids
			ForIndex(st,numst) {
				nr = fread(&(srfTris[s][st]),sizeof(uint),1,f);
			}
			// read diffuse
			fread(&srfDiffuse[s][0],sizeof(float),3,f);
		}
		LIBSL_END;
	}

	// read dynamic MVF (if present)
	AutoPtr<MVF> mvf( new MVF() );
	if (!mvf->load(f)) {
		typedef MVF3(mvf_position_3f,mvf_normal_3f,mvf_texcoord0_2f) t_StdVertexFormat;
		if (MVF_sizeof<t_StdVertexFormat>::value == sizeofVertex) {
      mvf = AutoPtr<MVF>(MVF::make<t_StdVertexFormat>());
		} else {
			fclose(f);
			throw Fatal("[MeshFormat_mesh::load] - no vertex format given, and format does not match the default one <pos,nrm,texcoord>");
		}
	}
	// close file
	fclose(f);

	m = new TriangleMesh_generic<Array<uchar> >(numv,numt,nums,mvf);

		// build mesh
		LIBSL_BEGIN;
		ForIndex(t,numt) {
			ForIndex(k,3) {
				m->triangleAt(t)[k] = indices[t*3+k];
			}
		}
		ForIndex(s,nums) {
			m->surfaceAt(s).textureName = srfTexNames[s];
			m->surfaceAt(s).triangleIds = srfTris    [s];
			m->surfaceAt(s).diffuse     = srfDiffuse [s];
		}
		ForIndex(p,numv) {
			memcpy( m->vertexDataAt(p), vertices.raw()+p*sizeofVertex, sizeofVertex);
		}
		LIBSL_END;

	// done
	return (m);
	LIBSL_END;
}

//---------------------------------------------------------------------------

void NAMESPACE::MeshFormat_mesh::save(const char *fname,const NAMESPACE::TriangleMesh *mesh) const
{
  // open file
  FILE *f;
	fopen_s(&f, fname, "wb");
  if (f == NULL) {
    throw Fatal("[MeshFormat_mesh::save] - cannot create file '%s'",fname);
  }
  // write vertices
  uint sizeofVertex = mesh->sizeOfVertexData();
  fwrite(&sizeofVertex,sizeof(uint),1,f);
  uint numv         = mesh->numVertices();
  fwrite(&numv,sizeof(uint),1,f);
  ForIndex(p,numv) {
    fwrite(mesh->vertexDataAt(p),sizeofVertex,1,f);
  }
  // write triangles
  uint numt         = mesh->numTriangles();
  fwrite(&numt,sizeof(uint),1,f);
  ForIndex(t,numt) {
    ForIndex(k,3) {
      uint i = mesh->triangleAt(t)[k];
      fwrite(&i,sizeof(uint),1,f);
    }
  }
  // write surface information
  uint nums         = mesh->numSurfaces();
  fwrite(&nums,sizeof(uint),1,f);
  ForIndex(s,nums) {
    // write texture name
    std::string texName  = mesh->surfaceTextureName(s);
    uint len             = uint(texName.length());
    fwrite(&len,sizeof(uint),1,f);
    fwrite(texName.c_str(),1,len+1,f);
    // write number of triangles
    uint numst      = mesh->surfaceNumTriangles(s);
    fwrite(&numst,sizeof(uint),1,f);
    // write ids
    ForIndex(st,numst) {
      uint id     = mesh->surfaceTriangleIdAt(s,st);
      fwrite(&id,sizeof(uint),1,f);
    }
    // write diffuse
    fwrite(&mesh->surfaceAt(s).diffuse[0],sizeof(float),3,f);
  }
  // write MVF
  if (!mesh->mvf().isNull()) {
    mesh->mvf()->save(f);
  }
  // close file
  fclose(f);
}

//---------------------------------------------------------------------------
