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

#include "MeshFormat_wrl.h"
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
  NAMESPACE::MeshFormat_wrl s_wrl;  /// FIXME: this mechanism does not work with VC++
}                                   ///        see also MeshFormatManager constructor

//---------------------------------------------------------------------------

NAMESPACE::MeshFormat_wrl::MeshFormat_wrl()
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

NAMESPACE::TriangleMesh *NAMESPACE::MeshFormat_wrl::load(const char *fname) const
{
    throw Fatal("[MeshFormat_wrl::load] - not implemented: %s",fname);
}

//---------------------------------------------------------------------------

void NAMESPACE::MeshFormat_wrl::save(const char *fname,const NAMESPACE::TriangleMesh *mesh) const
{
  FILE *f = NULL;
	fopen_s(&f, fname, "w");
  if (f == NULL) {
    throw Fatal("[MeshFormat_wrl::save] - cannot open file '%s' for writing",fname);
  }
  AutoPtr<MVF> mvf = mesh->mvf();
  if (mvf.isNull()) {
    throw Fatal("[MeshFormat_wrl::save] - mesh has no MVF definition: %s",fname);
  }
  fprintf(f,"#VRML V2.0 utf8\n\
  \n\
#Written by LibSL\n\
  \n\
Shape {\n\
  geometry IndexedFaceSet {\n\
    solid TRUE\n\
    coord Coordinate {\n\
    point [\n");
  ForIndex(v,mesh->numVertices()) {
    fprintf(f,"%f %f %f,\n",mesh->posAt(v)[0],mesh->posAt(v)[1],mesh->posAt(v)[2]);
  }
  fprintf(f,"\
      ]  # end point\n\
    } # end coord\n\
    coordIndex [\n");
  ForIndex(t,mesh->numTriangles()) {
    v3u tr = mesh->triangleAt(t);
    fprintf(f,"%d %d %d -1,\n",tr[0],tr[1],tr[2]);
  }
  fprintf(f,"\
    ] # end coordIndex\n");
  if (mvf->hasAttr( MVF::Float, MVF::Color0, 3) ) {
    fprintf(f,"\n\
              colorPerVertex TRUE\n\
              color Color {\n\
              color [\n");
    ForIndex(v,mesh->numVertices()) {
      v3f *clr = (v3f*)mvf->attr( (void*)mesh->vertexDataAt(v), MVF::Float, MVF::Color0, 3 );
      fprintf(f,"%f %f %f,\n",(*clr)[0],(*clr)[1],(*clr)[2]);
    }
    fprintf(f,"\
              ] # end inner color group\n\
              } # end color node\n");
  }
  fprintf(f,"\
  } # end geometry\n\
} # end shape\n");
  fclose(f);
}

//---------------------------------------------------------------------------
