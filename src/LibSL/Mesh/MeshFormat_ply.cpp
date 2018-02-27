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

#include "MeshFormat_ply.h"
using namespace LibSL::Mesh;

#include <LibSL/Errors/Errors.h>
using namespace LibSL::Errors;
#include <LibSL/Memory/Array.h>
using namespace LibSL::Memory::Array;
#include <LibSL/Memory/Pointer.h>
using namespace LibSL::Memory::Pointer;
#include <LibSL/Math/Vertex.h>
using namespace LibSL::Math;

#include "rply.h"
#include <string>

//---------------------------------------------------------------------------

#define NAMESPACE LibSL::Mesh

//---------------------------------------------------------------------------

/// Declaring a global will automatically register the plugin
namespace {
  NAMESPACE::MeshFormat_ply s_Ply;  /// FIXME: this mechanism does not work with VC++
}                                   ///        see also MeshFormatManager constructor

//---------------------------------------------------------------------------

NAMESPACE::MeshFormat_ply::MeshFormat_ply()
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

NAMESPACE::TriangleMesh *NAMESPACE::MeshFormat_ply::load(const char *fname) const
{
	LIBSL_BEGIN;
  throw Fatal("[MeshFormat_ply::load] - not yet implemented");
	LIBSL_END;
}

//---------------------------------------------------------------------------

void NAMESPACE::MeshFormat_ply::save(const char *fname, const NAMESPACE::TriangleMesh *mesh) const
{
  p_ply oply = ply_create(fname, PLY_LITTLE_ENDIAN, NULL, 0, NULL);
  if (!oply) {
    throw Fatal("[MeshFormat_ply::save] - cannot open file '%s' for writing",fname);
  }
  ply_add_element(oply, "vertex", mesh->numVertices());
  ply_add_scalar_property(oply, "x", e_ply_type::PLY_FLOAT);
  ply_add_scalar_property(oply, "y", e_ply_type::PLY_FLOAT);
  ply_add_scalar_property(oply, "z", e_ply_type::PLY_FLOAT);
  ply_add_scalar_property(oply, "red", e_ply_type::PLY_UCHAR);
  ply_add_scalar_property(oply, "green", e_ply_type::PLY_UCHAR);
  ply_add_scalar_property(oply, "blue", e_ply_type::PLY_UCHAR);
  ply_add_element(oply, "face", mesh->numTriangles());
  ply_add_list_property(oply, "vertex_indices", e_ply_type::PLY_UCHAR, e_ply_type::PLY_INT);
  ply_write_header(oply);
  // vertices
  ForIndex(v,mesh->numVertices()) {
    t_VertexData *dta = (t_VertexData*)mesh->vertexDataAt(v);
    ply_write(oply, dta->pos[0]);
    ply_write(oply, dta->pos[1]);
    ply_write(oply, dta->pos[2]);
    ply_write(oply, dta->clr[0]);
    ply_write(oply, dta->clr[1]);
    ply_write(oply, dta->clr[2]);
  }
  // triangles
  ForIndex(t,mesh->numTriangles()) {
    ply_write(oply, 3);
    v3u tri = mesh->triangleAt(t);
    ply_write(oply, tri[0]);
    ply_write(oply, tri[1]);
    ply_write(oply, tri[2]);
  }
  ply_close(oply);
}

//---------------------------------------------------------------------------
