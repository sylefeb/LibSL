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

#include "MeshFormat_map.h"
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

#include <fstream>
using namespace std;

//---------------------------------------------------------------------------

#define NAMESPACE LibSL::Mesh

//---------------------------------------------------------------------------

/// Declaring a global will automatically register the plugin
namespace {
  NAMESPACE::MeshFormat_map s_Map;  /// FIXME: this mechanism does not work with VC++
}                                   ///        see also MeshFormatManager constructor

//---------------------------------------------------------------------------

NAMESPACE::MeshFormat_map::MeshFormat_map()
{
  try {
    // register plugin
    MESH_FORMAT_MANAGER.registerPlugin(this);
  } catch (LibSL::Errors::Fatal& e) {
    std::cerr << e.message() << std::endl;
  }
}

//---------------------------------------------------------------------------

NAMESPACE::TriangleMesh *NAMESPACE::MeshFormat_map::load(const char *fname) const
{
  vector<Brush>                      brushes;
  vector<Array2D<v3f> >              patches;
  vector<v3f>                        vertices;
  vector<v3f>                        normals;
  vector<v3f>                        uvs;
  vector<v3i>                        triangles;
  map<std::string,std::vector<int> > surfaces;

  uint numentities = 0;
  uint numportals  = 0;

  // load brushes and patches
  LIBSL_BEGIN;

  BasicParser::FileStream stream(fname);
  BasicParser::Parser<BasicParser::FileStream> parser(stream,true/*eol as space*/);
  cerr << "Doom3 map file: " << fname << endl;
  // parse entities
  while (!parser.eof()) {
    numentities ++;
    if (!parser.reachChar('{')) {
      break;
    }
    v3f    origin    = 0;
    string classname = "";
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
#ifdef WIN32
            sscanf_s(v.c_str(),"%f %f %f",&origin[0],&origin[1],&origin[2]);
#else
            sscanf(v.c_str(),"%f %f %f",&origin[0],&origin[1],&origin[2]);
#endif
          } else if (p == "classname") {
            classname = v;
          }
          // cerr << "       -> " << p << " = " << v << endl;
        }
        break; // case "
      case '{':
        {
          // primitive
          string s = parser.readString();
          if (s == "brushDef3") {
            // brush
            parser.reachChar('{');
            brushes.push_back(Brush());
            while (!parser.eof()) {
              int c      = parser.readChar();
              if (c == '(') {
                float nx = parser.readFloat();
                float ny = parser.readFloat();
                float nz = parser.readFloat();
                float d  = parser.readFloat();
                parser.reachChar('('); parser.reachChar('(');
                Brush::t_v6f uvtrsf = 0;
                uvtrsf[0] = parser.readFloat();
                uvtrsf[1] = parser.readFloat();
                uvtrsf[2] = parser.readFloat();
                parser.reachChar('(');
                uvtrsf[3] = parser.readFloat();
                uvtrsf[4] = parser.readFloat();
                uvtrsf[5] = parser.readFloat();
                parser.reachChar('\"');
                string texname = parser.readString("\"");
                brushes.back().addPlaneCW(Plane<3>(d,V3F(nx,ny,nz)),texname,uvtrsf);
                parser.reachChar('\n');
              } else if (c == '}') {
                // brush is complete
                // -> add origin
                ForIndex(p,brushes.back().polygons().size()) {
                  uint numpt   = brushes.back().polygons()[p].size();
                  ForIndex(i,numpt) {
                    brushes.back().polygons()[p].vertexAt(i) -= origin;
                  }
                }
                break;
              } else {
                cerr << char(c) << endl;
                throw Fatal("MeshFormat_map::load - error in file");
              }
            }
          } else if (s == "patchDef2") {
            // patch
            parser.reachChar('{');
            parser.reachChar('(');
            uint resx    = parser.readInt();
            uint resy    = parser.readInt();
            parser.reachChar('(');
            patches.push_back(Array2D<v3f>(resx,resy));
            ForIndex(x,resx) {
              parser.reachChar('(');
              ForIndex(y,resy) {
                parser.reachChar('(');
                float px = parser.readFloat();// + origin[0];
                float py = parser.readFloat();// + origin[1];
                float pz = parser.readFloat();// + origin[2];
                patches.back().at(x,y) = - V3F(px,py,pz);
              }
            }
            parser.reachChar('}');
          } else if (s == "patchDef3") {
            // patch
            parser.reachChar('(');
            uint resx    = parser.readInt();
            uint resy    = parser.readInt();
            uint subx    = parser.readInt();
            uint suby    = parser.readInt();
            parser.reachChar('(');
            patches.push_back(Array2D<v3f>(resx,resy));
            ForIndex(x,resx) {
              parser.reachChar('(');
              ForIndex(y,resy) {
                parser.reachChar('(');
                float px = parser.readFloat();// + origin[0];
                float py = parser.readFloat();// + origin[1];
                float pz = parser.readFloat();// + origin[2];
                patches.back().at(x,y) = - V3F(px,py,pz);
              }
            }
            parser.reachChar('}');
          } else {
            cerr << "       -> unknown primitive " << s << endl;
            // unknown
            break;
          }
          parser.reachChar('}');
          break; // case {
        }
      } // switch
      if (entity_end) {
        break;
      }
    }; // while entity content
  }; // while entities

  LIBSL_END;

  // add brushes geometry

  LIBSL_BEGIN;

  ForIndex(b,brushes.size()) {
    if (brushes[b].isClosed()) {
      ForIndex(p,brushes[b].polygons().size()) {
        // skip non-visible materials
        if ( strstr(brushes[b].materials()[p].c_str(),"nodraw"  ) != NULL ) {
          continue;
        }
        if ( strstr(brushes[b].materials()[p].c_str(),"player_clip"  ) != NULL ) {
          continue;
        }
        if (strstr(brushes[b].materials()[p].c_str(),"visportal") != NULL) {
          numportals ++;
          continue;
        }
        // add vertices
        uint v_start = uint(vertices.size());
        uint numpt   = brushes[b].polygons()[p].size();
        v3f  nrm     = brushes[b].polygons()[p].normal();
        Brush::t_v6f uvtrsf = brushes[b].uvTrsfms()[p];
        ForIndex(i,numpt) {
          v3f p3d     = brushes[b].polygons()[p].vertexAt(i);
          Plane<3> pl = brushes[b].planes()[p];
          pair<v3f,v3f> f = frame( pl.n() );
          v3f wuv     = v3f( V2F(
            dot(f.first  , (p3d - pl.o())) ,
            dot(f.second , (p3d - pl.o())) ) , 1);
          v3f uv      = V3F(
            dot(wuv , V3F(uvtrsf[0],uvtrsf[1],uvtrsf[2])) ,
            dot(wuv , V3F(uvtrsf[3],uvtrsf[4],uvtrsf[5])) ,
            0);
          vertices.push_back( p3d );
          normals .push_back( nrm );
          uvs     .push_back( uv  );
        }
        sl_assert(uvs     .size() == normals.size());
        sl_assert(vertices.size() == normals.size());
        // add triangles to global list and to surfaces
        std::vector<int>& snfo = surfaces[ brushes[b].materials()[p] ];
        for (uint i=1;i<numpt;i++) {
          triangles.push_back(V3I(v_start, v_start+i, v_start+((i+1)%numpt) ));
          snfo     .push_back( (int)triangles.size() - 1 );
        }

      }
    } else {
      cerr << "       -> warning: brush " << b << " not closed." << endl;
    }
  }

  LIBSL_END;

  // stats
  cerr << "       -> " << numentities    << " entities" << endl;
  cerr << "       -> " << brushes.size() << " brushes"  << endl;
  cerr << "       -> " << numportals     << " portals"  << endl;
  cerr << "       -> " << patches.size() << " patches"  << endl;

  // add patches geometry

  LIBSL_BEGIN;

  ForIndex(n,patches.size()) {
    Bezier::BezierSurfaceQuadratic<v3f> srf(patches[n]);
//    srf.tesselateFixed(12,12, vertices,normals,triangles);
    srf.tesselateAdaptive(4,4,0.5f, vertices,normals,uvs,triangles);
  } // patches

  LIBSL_END;

  // create final geometry

  LIBSL_BEGIN;

  TriangleMesh_generic<MeshFormat_map::t_VertexData> *mesh
    = new TriangleMesh_generic<MeshFormat_map::t_VertexData>(
    uint(vertices .size()),
    uint(triangles.size()),
    uint(surfaces .size()),
    AutoPtr<MVF>(MVF::make<MeshFormat_map::t_VertexFormat>()));

  ForArray(vertices,v) {
    mesh->vertexAt(v).pos = V3F( - vertices[v][0], - vertices[v][1], - vertices[v][2] );
    mesh->vertexAt(v).nrm = V3F( - normals [v][0], - normals [v][1], - normals [v][2] );
    mesh->vertexAt(v).uv  = v2f( uvs[v] );
  }

  ForArray(triangles,t) {
    ForIndex(p,3) {
      mesh->triangleAt(t)[p] = triangles[t][p];
    }
  }

  typedef map<std::string,std::vector<int> > t_map;
  int s = 0;
  ForConstIterator(t_map,surfaces,S) {
    mesh->surfaceAt(s).textureName = S->first;
    mesh->surfaceAt(s).diffuse     = V3F(1,1,1);
		mesh->surfaceAt(s).triangleIds.allocate((uint)S->second.size());
    ForIndex(i,S->second.size()) {
      mesh->surfaceAt(s).triangleIds[i] = S->second[i];
    }
    s ++;
  }

  return (mesh);

  LIBSL_END;
}

//---------------------------------------------------------------------------

void NAMESPACE::MeshFormat_map::save(const char *fname,const NAMESPACE::TriangleMesh *mesh) const
{
  throw Fatal("MeshFormat_map::save - Sorry, cannot save in map format: %s",fname);
}

//---------------------------------------------------------------------------
