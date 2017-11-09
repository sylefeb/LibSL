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

#include "MeshFormat_off.h"
using namespace LibSL::Mesh;

#include <LibSL/Errors/Errors.h>
using namespace LibSL::Errors;
#include <LibSL/Memory/Array.h>
using namespace LibSL::Memory::Array;
#include <LibSL/Memory/Pointer.h>
using namespace LibSL::Memory::Pointer;
#include <LibSL/Math/Vertex.h>
using namespace LibSL::Math;

#include <tinyxml/tinyxml.h>
#include <string>

//---------------------------------------------------------------------------

#define NAMESPACE LibSL::Mesh

//---------------------------------------------------------------------------

/// Declaring a global will automatically register the plugin
namespace {
  NAMESPACE::MeshFormat_off s_Off;  /// FIXME: this mechanism does not work with VC++
}                                   ///        see also MeshFormatManager constructor

//---------------------------------------------------------------------------

NAMESPACE::MeshFormat_off::MeshFormat_off()
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

NAMESPACE::TriangleMesh *NAMESPACE::MeshFormat_off::load(const char *fname) const
{
	LIBSL_BEGIN;

  BasicParser::FileStream stream(fname);
  BasicParser::Parser<BasicParser::FileStream> parser(stream);
  const char *s = parser.readString();
  if (strcmp(s,"OFF")) {
	 throw Fatal("[MeshFormat_off::load] - file '%s' is not an OFF",fname);
  }
  cerr << sprint("[off] loading mesh '%s', file '%s'",s,fname) << endl;
  int numv = parser.readInt();
  int numt = parser.readInt();
  int nume = parser.readInt();
  // allocate mesh
  TriangleMesh_generic<MeshFormat_off::t_VertexData> *mesh
    = new TriangleMesh_generic<MeshFormat_off::t_VertexData>(numv, numt, 0, AutoPtr<MVF>(MVF::make<MeshFormat_off::t_VertexFormat>()));
  // read vertices
  ForIndex(i,numv) {
	  if (parser.eof()) {
      throw Fatal("[MeshFormat_off::load] - premature end of file! '%s'",fname);
    }
    t_VertexData *d = (t_VertexData *)mesh->vertexDataAt( i );
		d->pos[0] = parser.readFloat();
		d->pos[1] = parser.readFloat();
		d->pos[2] = parser.readFloat();
    // cerr << d->pos << endl;
  }
  // read faces
  ForIndex(i,numt) {
	  if (parser.eof()) {
      throw Fatal("[MeshFormat_off::load] - premature end of file! (%s)",fname);
    }
    int n = parser.readInt();
	  if (n != 3) {
      throw Fatal("[MeshFormat_off::load] - face is not a triangle (currently unsupported) - '%s'",fname);
    }
    v3u tri;
    tri[0] = parser.readInt();
    tri[1] = parser.readInt();
    tri[2] = parser.readInt();
    mesh->triangleAt(i) = tri;
    // cerr << tri << endl;
  }
  // compute normals
  ForIndex(t,mesh->numTriangles()) {
    v3f pts[3];
    ForIndex(i,3) {
      pts[i] = mesh->posAt( mesh->triangleAt(t)[i] );
    }
    v3f nrm = normalize_safe(cross( pts[1]-pts[0] , pts[2]-pts[0] ));
    ForIndex(i,3) {
      t_VertexData *d = (t_VertexData *)mesh->vertexDataAt( mesh->triangleAt(t)[i] );
      d->nrm = nrm;
    }
  }
  return mesh;
	LIBSL_END;
}

//---------------------------------------------------------------------------

void NAMESPACE::MeshFormat_off::save(const char *fname,const NAMESPACE::TriangleMesh *mesh) const
{
  ofstream f(fname);
  if ( ! f ) {
    throw Fatal("[MeshFormat_off::save] - cannot open file '%s' for writing",fname);
  }
  f << "OFF\n";
  f << mesh->numVertices() << " " << mesh->numTriangles() << " 0\n";
  // vertices
  ForIndex(v,mesh->numVertices()) {
    f << mesh->posAt(v)[0] << " " << mesh->posAt(v)[1] << " " << mesh->posAt(v)[2] << "\n";
  }
  // triangles
  ForIndex(t,mesh->numTriangles()) {
    v3u tri = mesh->triangleAt(t);
    f << "3 " << tri[0] << " " << tri[1] << " " << tri[2] << "\n";
  }
}

//---------------------------------------------------------------------------
