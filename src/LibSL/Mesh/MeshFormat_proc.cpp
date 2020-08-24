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

#include "MeshFormat_proc.h"
using namespace LibSL::Mesh;

#include <LibSL/Errors/Errors.h>
using namespace LibSL::Errors;
#include <LibSL/Memory/Array.h>
using namespace LibSL::Memory::Array;
#include <LibSL/Memory/Pointer.h>
using namespace LibSL::Memory::Pointer;
#include <LibSL/Math/Vertex.h>
using namespace LibSL::Math;
#include <LibSL/Geometry/Brush.h>
using namespace LibSL::Geometry;
#include <LibSL/CppHelpers/CppHelpers.h>
using namespace LibSL::CppHelpers;
#include <LibSL/System/System.h>
using namespace LibSL::System;

#include <fstream>
using namespace std;

//---------------------------------------------------------------------------

#define NAMESPACE LibSL::Mesh

//---------------------------------------------------------------------------

/// Declaring a global will automatically register the plugin
namespace {
  NAMESPACE::MeshFormat_proc s_Proc;  /// FIXME: this mechanism does not work with VC++
}                                     ///        see also MeshFormatManager constructor

//---------------------------------------------------------------------------

NAMESPACE::MeshFormat_proc::MeshFormat_proc()
{
  try {
    // register plugin
    MESH_FORMAT_MANAGER.registerPlugin(this);
  } catch (LibSL::Errors::Fatal& e) {
    std::cerr << e.message() << std::endl;
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::MeshFormat_proc::loadModelOriginsFromMap(const char *fname,map<string,v3f>& _origins) const
{
  string mapName = string(fname);
  mapName = mapName.substr(0,mapName.length()-5) + ".map";
  if (!System::File::exists(mapName.c_str())) {
    return;
  }
  LIBSL_BEGIN;
  BasicParser::FileStream stream(mapName.c_str());
  BasicParser::Parser<BasicParser::FileStream> parser(stream,true/*eol as space*/);
  cerr << "Parsing map file: " << mapName << endl;
  // parse entities
  while (!parser.eof()) {
    if (!parser.reachChar('{')) {
      break;
    }
    v3f    origin    = 0;
    string classname = "";
    string name = "";
    string model = "";
    // parse entity content
    while (!parser.eof()) {
      bool entity_end  = false;
      char c = char(parser.reachOneOf("\"{}"));
      switch (c)
      {
      case '}':
        entity_end = true;
        break;
      case '\"':
        {
          // property
          string p = parser.readString("\"");
          parser.reachChar('\"');
          string v = parser.readString("\"");
          if (p == "origin") {
#if defined(_WIN32) || defined(_WIN64)
            sscanf_s(v.c_str(),"%f %f %f",&origin[0],&origin[1],&origin[2]);
#else
            sscanf  (v.c_str(),"%f %f %f",&origin[0],&origin[1],&origin[2]);
#endif
          } else if (p == "name") {
            name = v;
          } else if (p == "classname") {
            classname = v;
          } else if (p == "model") {
            model = v;
          }
        }
        break; // case "
      case '{':
        {
          // primitive
          parser.reachChar('{');
          parser.reachChar('}');
          break;
        }
      }
      if (entity_end) {
        _origins.insert(make_pair(model,origin));
        break;
      }
    } // while entity content
  } // while entities
  LIBSL_END;
}

//---------------------------------------------------------------------------

NAMESPACE::TriangleMesh *NAMESPACE::MeshFormat_proc::load(const char *fname) const
{
  vector<v3f>           vertices;
  vector<v3f>           normals;
  vector<v2f>           uvs;
  vector<v3i>           triangles;
  vector<string>        surfaceTextures;
  vector<vector<uint> > surfaceTriangles;
  vector<vector<uint> > surfaceVertices;

  uint numsurfaces = 0;

  // try to load origins from map file
  map<string,v3f> origins;
  loadModelOriginsFromMap(fname,origins);
  // load surfaces
  LIBSL_BEGIN;
  BasicParser::FileStream stream(fname);
  BasicParser::Parser<BasicParser::FileStream> parser(stream,true/*eol as space*/);
  cerr << "Doom3 proc file: " << fname << endl;
  // parse entities
  uint sid = 0;
  while (!parser.eof()) {
    string s = parser.readString();
    if (s == "model") {
      parser.reachChar('{'); // model {
      parser.reachChar('/'); // /* name */
      parser.reachChar('/');
      parser.reachChar('\"');
      string modelName   = parser.readString("\"");
      v3f    modelOrigin = 0;
      map<string,v3f>::iterator O = origins.find(modelName);
      if (O != origins.end()) {
        modelOrigin = (*O).second;
      }
      if (modelName.substr(0,5) != "_area") { continue; }
      //if (modelName != "_area13") { continue; }
      cerr << "model " << m_Areas.size() << ": " << modelName << endl;
      m_Areas.push_back(vector<uint>());
      parser.reachChar('/'); // /* numSurfaces */
      parser.reachChar('/');
      uint numsrf = parser.readInt();
      //cerr << "Num SRF :" << numsrf << endl;
      ForIndex(s,numsrf) {
        bool skip = false;
        m_Areas.back().push_back(sid ++);
        // surfaces
        uint start_indice = (uint)vertices.size();
        parser.reachChar('/'); // /* surface x */
        parser.reachString("surface"," ");
        int srfId      = parser.readInt();
        sl_assert(srfId == s);
        //cerr << srfId << ' ';
        parser.reachChar('/');
        parser.reachChar('{');
        parser.reachChar('\"');
        string textureName = parser.readString("\"");
        if (textureName.find("decals") != string::npos) {
          // skip decals
          skip = true;
        }
        surfaceTextures.push_back(textureName);
        parser.reachChar('/'); // /* numVerts = */
        parser.readString("/");
        uint numverts      = parser.readInt();
        parser.reachChar('/'); // /* numIndexes = */
        parser.readString("/");
        uint numidxs      = parser.readInt();
        ForIndex(n,numverts) {
          parser.reachChar('(');
          float x  = parser.readFloat();
          float y  = parser.readFloat();
          float z  = parser.readFloat();
          float u  = parser.readFloat();
          float v  = parser.readFloat();
          float nx = parser.readFloat();
          float ny = parser.readFloat();
          float nz = parser.readFloat();
          parser.reachChar(')');
          if (!skip) {
            vertices.push_back(V3F(x,y,z) + modelOrigin);
            normals .push_back(V3F(nx,ny,nz));
            uvs     .push_back(V2F(u,v));
          }
        }
        Array<uint> idxs;
        idxs           .allocate(numidxs);
        idxs           .fill(-1);
        surfaceVertices.push_back(vector<uint>());
        ForIndex(i,numidxs) {
          int srfvid = parser.readInt();
          // cerr << srfvid << ' ';
          idxs[i]    = start_indice + srfvid;
          if (!skip) {
            surfaceVertices.back().push_back(idxs[i]);
          }
        }
        //cerr << endl;
        sl_assert(skip || surfaceVertices.back().size() == numidxs);
        sl_assert((idxs.size() % 3) == 0);
        surfaceTriangles.push_back(vector<uint>());
        if (!skip) {
          ForIndex(t,idxs.size()/3) {
            triangles              .push_back(V3I( idxs[t*3  ],idxs[t*3+1],idxs[t*3+2] ));
						surfaceTriangles.back().push_back((uint)triangles.size() - 1);
          }
        }
        parser.reachChar('}');
        numsurfaces ++;
      }; // for surfaces
      //cerr << endl;
      parser.reachChar('}');
    } // model
    else if (s == "interAreaPortals") {
      parser.reachChar('{'); // interAreaPortals {
      parser.reachChar('/'); // /* numAreas */
      parser.reachChar('/');
      uint numAreas = parser.readInt();
      parser.reachChar('/'); // /* numIAP */
      parser.reachChar('/');
      uint numIAP = parser.readInt();
      parser.reachChar('/'); // /* interAreaPortal format is: ... */
      parser.reachChar('/');
      m_Portals.resize(numIAP);
      ForIndex(iap,numIAP) {
        //cerr << iap << ": ";
        parser.reachChar('/'); // /* iap x */
        parser.reachChar('/');
        uint numVerts  = parser.readInt();
        uint posSide   = parser.readInt();
        uint negSide   = parser.readInt();
        m_Portals[iap] .resize(numVerts);
        ForIndex(v,numVerts) {
          parser.reachChar('(');
          float x  = parser.readFloat();
          float y  = parser.readFloat();
          float z  = parser.readFloat();
          parser.reachChar(')');
          m_Portals[iap][v] = V3F(x,y,z);
          //cerr << V3F(x,y,z) << " ";
        }
        //cerr << endl;
      }

    }
  }; // while !eof

  LIBSL_END;

  // stats
  cerr << "       -> " << numsurfaces      << " surfaces" << endl;
  cerr << "       -> " << vertices.size()  << " vertices" << endl;
  cerr << "       -> " << triangles.size() << " triangles" << endl;

  // create final geometry

  LIBSL_BEGIN;

  TriangleMesh_generic<MeshFormat_proc::t_VertexData> *mesh
    = new TriangleMesh_generic<MeshFormat_proc::t_VertexData>
    (uint(vertices.size()), uint(triangles.size()), uint(numsurfaces), AutoPtr<MVF>(MVF::make<MeshFormat_proc::t_VertexFormat>()));

  ForArray(vertices,v) {
    mesh->vertexAt(v).pos = vertices[v];
    mesh->vertexAt(v).nrm = normals [v];
    mesh->vertexAt(v).uv  = uvs     [v];
  }

  ForArray(triangles,t) {
    ForIndex(p,3) {
      mesh->triangleAt(t)[p] = triangles[t][p];
    }
  }

  ForArray(surfaceTextures,s) {
    mesh->surfaceAt(s)  .textureName = surfaceTextures[s];
    mesh->surfaceAt(s)  .diffuse     = V3F(1,1,1);
		mesh->surfaceAt(s).triangleIds.allocate((uint)surfaceTriangles[s].size());
    ForArray(surfaceTriangles[s],n) {
      mesh->surfaceAt(s).triangleIds[n] = surfaceTriangles[s][n];
    }
  }

  return (mesh);

  LIBSL_END;
}

//---------------------------------------------------------------------------

void NAMESPACE::MeshFormat_proc::save(const char *fname,const NAMESPACE::TriangleMesh *mesh) const
{
  throw Fatal("MeshFormat_proc::save - Sorry, cannot save in proc format: %s",fname);
}

//---------------------------------------------------------------------------
