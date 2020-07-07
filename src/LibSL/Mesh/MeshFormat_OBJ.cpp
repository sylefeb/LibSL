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

#include "MeshFormat_OBJ.h"
using namespace LibSL::Mesh;

#include <LibSL/Errors/Errors.h>
using namespace LibSL::Errors;
#include <LibSL/Memory/Array.h>
using namespace LibSL::Memory::Array;
#include <LibSL/Memory/Pointer.h>
using namespace LibSL::Memory::Pointer;
#include <LibSL/Math/Vertex.h>
using namespace LibSL::Math;

#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
// #include <hash_map>

using namespace std;

//---------------------------------------------------------------------------

#define NAMESPACE LibSL::Mesh

//---------------------------------------------------------------------------

/// Declaring a global will automatically register the plugin
namespace {
	NAMESPACE::MeshFormat_OBJ s_Obj;  /// FIXME: this mechanism does not work with VC++
}                                   ///        see also MeshFormatManager constructor

//---------------------------------------------------------------------------

NAMESPACE::MeshFormat_OBJ::MeshFormat_OBJ()
{
	try {
		// register plugin
		MESH_FORMAT_MANAGER.registerPlugin(this);
	} catch (LibSL::Errors::Fatal& e) {
		std::cerr << e.message() << std::endl;
	}
}

//---------------------------------------------------------------------------

void NAMESPACE::MeshFormat_OBJ::parseMaterialLibrary(const char *matFile,std::map<std::string,std::string>& _textures) const
{
	cerr << "[MeshFormat_OBJ] reading materials from " << matFile << endl;
	LIBSL_BEGIN;
	BasicParser::FileStream stream(matFile);
	BasicParser::Parser<BasicParser::FileStream> parser(stream,false);
	string currentMaterial = "";
	while (!parser.eof()) {
		char *prop = parser.readString(" ");
		prop       = parser.trim( prop , " \t\r\n" );
		int len    = (int)strlen(prop);
		ForIndex(i,len) { prop[i] = tolower(prop[i]); }
		if ( strcmp(prop,"newmtl") == 0 ) {
			currentMaterial = parser.trim( parser.readString("\n") , " \t\r\n" );
			cerr << "New material: [" << currentMaterial << ']' << endl;
		} else if ( strcmp(prop,"map_kd") == 0 ) {
			const char *texName  = parser.trim( parser.readString("\n") , " \t\r\n" );
			std::cerr << '[' << currentMaterial << '|'<< texName << ']' << std::endl;
			_textures.insert(make_pair(currentMaterial,texName));
		} else {
			parser.reachChar('\n');
		}
	}
	LIBSL_END;
}

//---------------------------------------------------------------------------

/*
class hash_v3i
{
public:
    enum { bucket_size = 64, min_buckets = 16};
	size_t operator()(const v3i& v) const 
	{
		hash<int> h;
		return h(v[0])^h(v[1])^h(v[2]);
	}
	bool operator()(const v3i& v1,const v3i& v2) const 
	{ 
		return v1 < v2;
	}
};
*/

NAMESPACE::TriangleMesh *NAMESPACE::MeshFormat_OBJ::load(const char *fname) const
{
	typedef Tuple<v3i,3> t_TriNfo;

	Timer tm("[OBJ] load");

	bool        hastc      = false;
	uint        lineCnt    = 0;
	uint        faceCnt    = 0;
	uint        uvfaceCnt  = 0;
	uint        nrmfaceCnt = 0;

	const int id_pos = 0;
	const int id_tex = 1;
	const int id_nrm = 2;

	map<string,string>   textures;
	vector<v3f>          poss;
	vector<v2f>          tcs;
	vector<v3f>          nrms;
	vector<t_TriNfo>     tris;
#define N_FACE_MAX 16
	v3i                  face[N_FACE_MAX];

	typedef map<string,vector<int> > t_srfMap;
	t_srfMap  surfaces;
	surfaces[""]                      = vector<int>();
	t_srfMap::iterator currentSurface = surfaces.begin();

	LIBSL_BEGIN;
	Timer tm("[OBJ] parse");
	BasicParser::FileStream                      stream(fname);
	BasicParser::Parser<BasicParser::FileStream> parser(stream,false);
  Console::processingInit();
  bool ignore = false;
	while ( ! parser.eof() ) {
    Console::processingUpdate();
		lineCnt ++;
		const char *s = parser.trim( parser.readString() , " \t\r\n" );
		// cerr << "S = '" << s << '\'' << endl;
		if (parser.eof()) break;
		if (!strcmp(s,"v")) {
			v3f v;
			v[0] = parser.readFloat();
			v[1] = parser.readFloat();
			v[2] = parser.readFloat();
			poss.push_back(v);
		} else if (!strcmp(s,"vn")) {
			v3f vn;
			vn[0] = parser.readFloat();
			vn[1] = parser.readFloat();
			vn[2] = parser.readFloat();
      nrms.push_back(vn);
		} else if (!strcmp(s,"vt")) {
			v2f vt = 0;
			vt[0]  = parser.readFloat();
			vt[1]  = parser.readFloat();
      tcs.push_back(vt);
      int next = parser.readChar(false);
			if (isdigit(next) || next=='-') {
				// ignore third texture coordinate
				parser.readFloat();
			}
		} else if (!strcmp(s,"f")) {
			uint i = 0;
			bool has_uv = false;
			bool has_n  = false;
			bool stop   = false;
			int  nface  = 0;
			do {
				v3i vnfo  = -1; // vertex nfo
				// index of position
				vnfo[id_pos]  = parser.readInt();
				if (vnfo[id_pos] < 0) { vnfo[id_pos] = ((int)poss.size())+vnfo[id_pos]; } // deal with negative indices
				else                  { vnfo[id_pos] --; }
				if (parser.readChar(false) == '/') {
					parser.reachChar('/');
					if (parser.readChar(false) != '/') {
						has_uv  = true;
						// index of tex coord
						vnfo[id_tex] = parser.readInt();
						if (vnfo[id_tex] < 0) { vnfo[id_tex] = ((int)tcs.size())+vnfo[id_tex]; } // deal with negative indices
						else                  { vnfo[id_tex] --; }
					}
					if (parser.readChar(false) == '/') {
						parser.reachChar('/');
						has_n = true;
						// index of normal
						vnfo[id_nrm] = parser.readInt();
						if (vnfo[id_nrm] < 0) { vnfo[id_nrm] = ((int)nrms.size())+vnfo[id_nrm]; } // deal with negative indices
						else                  { vnfo[id_nrm] --; }
					}
				}
				if (nface >= N_FACE_MAX) {
					throw Fatal("[MeshFormat_OBJ::load] - unsupported number of faces %d (max. %d)", nface + 1, N_FACE_MAX);
				}
				face[ nface ++ ] = vnfo;
				char nextc = parser.readChar(false);
				if ( nextc == '\n' ) {
					stop = true;
				}
			} while ( ! stop );
			if (nface < 3) {
				cerr << "[MeshFormat_OBJ::load] - WARNING face has less than 3 vertices" << endl;
				continue;
			}
      if (!ignore) {
        faceCnt++;
        if (has_uv) uvfaceCnt++;
        if (has_n)  nrmfaceCnt++;
        sl_assert(nface > 2);
        ForRange(i, 2, nface - 1) {
          // add triangle
          tris.push_back(Triple(face[0], face[i - 1], face[i]));
          // add to surface
          currentSurface->second.push_back((int)tris.size() - 1);
        }
      }
		} else if (!strcmp(s,"g")) {
      //cerr << "[OBJ loader] object " << parser.readString("\n") << endl;
			parser.reachChar('\n');
		} else if (!strcmp(s,"o")) {
      /*
      /////// HACK HACK
      const char *objectName = parser.trim(parser.readString("\n"), " \t\r\n");
      if (strncmp("Basic", objectName, 5) == 0
        || strncmp("Compound", objectName, 8) == 0
        || strncmp("Floor", objectName, 5) == 0
        || strncmp("Poutre", objectName, 6) == 0
        || strncmp("Poteau", objectName, 6) == 0
        ) { 
        ignore = false;
        cerr << Console::white;
      } else {
        ignore = true;
      }
      cerr << "[OBJ loader] object " << objectName << endl;
      cerr << Console::gray;
      */
      parser.reachChar('\n');
    } else if (!strcmp(s, "mtllib")) {
			const char *matFile = parser.trim( parser.readString("\n") , " \t\r\n" );
			parseMaterialLibrary(matFile,textures);
		} else if (!strcmp(s,"usemtl")) {
			const char *matName = parser.trim( parser.readString("\n") , " \t\r\n" );
			t_srfMap::iterator S = surfaces.find(matName);
			if (S == surfaces.end()) {
				surfaces[ matName ] = vector<int>();
				currentSurface = surfaces.find(matName);
			} else {
				currentSurface = S;
			}
		} else if (!strcmp(s,"")) {
			// empty line
			parser.reachChar('\n');
		} else if (!strcmp(s,"s")) {
			// skip smoothing group
			parser.reachChar('\n');
		} else if (s[0] == '#') {
			//const char *str = parser.readUntil("\n");
			//cerr << "[OBJ loader] " << str << endl;
      parser.reachChar('\n');
		} else {
			throw Fatal("[MeshFormat_OBJ::load] - cannot understand string '%s', line %d",s,lineCnt);    
		}
	};
  Console::processingEnd();
	std::cerr << sprint(" (read %d lines, %d faces, %d uvFace, %d nrmFaces)\n",lineCnt,faceCnt,uvfaceCnt,nrmfaceCnt);
	LIBSL_END;

	// compute normals if none provided
	if (nrmfaceCnt == 0) {
		Timer tm("[OBJ] normals");
		std::cerr << "[MeshFormat_OBJ::load] - computing normals (none provided)\n"; 
		// compute per-vertex normals - use same indices as pos
		nrms.resize( poss.size() );
		ForIndex(n,nrms.size()) {
			nrms[n] = 0;
		}
		v3f p[3];
		ForIndex(t,tris.size()) {
			ForIndex(i,3) {
				sl_assert( tris[t][i][id_pos] >  -1 );
				sl_assert( tris[t][i][id_pos] <  (int)poss.size() );
				p[i] = poss[ tris[t][i][id_pos] ];
			}
			v3f nrm = normalize_safe( cross(p[1]-p[0],p[2]-p[0]) );
			ForIndex(i,3) {
				sl_assert( tris[t][i][id_nrm] == -1 );
				tris[t][i][id_nrm]          = tris[t][i][id_pos];
				nrms[ tris[t][i][id_nrm] ] += nrm;
			}
		}
		// normalize all
		ForIndex(n,nrms.size()) {
			nrms[n] = normalize_safe( nrms[n] );
		}
	}

	// gather unique vertices
	vector<v4i> vertices;
	Array<int>  unsort;
	Array<int>  vertids;
	int numunique = 0;
	{
		Timer tm("[OBJ] unique vertices");
		vertices.resize  (tris.size()*3);
		vertids .allocate((uint)tris.size()*3);
		unsort  .allocate((uint)tris.size()*3);
		ForArray(tris,t) {
			ForIndex(i,3) {
				vertices[t*3+i] = v4i( tris[t][i] , t*3+i );
			}
		}
		// sort to find out duplicates
		sort(vertices.begin(),vertices.end());
		ForIndex(v,vertices.size()) {
			unsort[ vertices[v][3] ] = v;
		}
		// number unique vertices
		int vid     = 0;
		v3i prev    = v3i(vertices[0]);
		vertids[0]  = 1+vid;
		ForRange(v,1,vertices.size()-1) {
			if (v3i(vertices[v]) != prev) {
				vid ++;
				vertids[v] =  (1+vid);
			} else {
				vertids[v] = -(1+vid); // duplicate
			}
			prev = v3i(vertices[v]);
		}
		numunique = vid + 1; // number of unique vertices: vid+1
	}
	std::cerr << "[MeshFormat_OBJ::load] - " << numunique << " unique vertices\n"; 
	// allocate triangles and vertices for the final mesh
	Array<MeshFormat_OBJ::t_VertexData> meshverts;
	Array<v3i>                          meshtris;
	meshverts.allocate( numunique );
	meshtris .allocate( (uint)tris.size() );
	// form final mesh
	// -> vertices
	ForIndex(v,vertices.size()) {
		sl_assert(vertids[v] != 0);
		if (vertids[v] > 0) {
			int vi     = vertids[v]-1;
			sl_assert( vi < numunique );
			int pos_i  = vertices[v][id_pos];
			int tex_i  = vertices[v][id_tex];
			int nrm_i  = vertices[v][id_nrm];
			meshverts[vi]                  .pos = poss[pos_i];
			if ( tex_i > -1 ) meshverts[vi].uv  = tcs [tex_i]; else meshverts[vi].uv  = 0;
			if ( nrm_i > -1 ) meshverts[vi].nrm = nrms[nrm_i]; else meshverts[vi].nrm = V3F(1,0,0);
		}
	}
	// -> triangles
	ForArray(tris,t) {
		ForIndex(i,3) {
			int vi         = abs( vertids[ unsort[t*3+i] ] ) - 1;
			sl_assert( vi >= 0 );
			sl_assert( vi < numunique );
			meshtris[t][i] = vi;
		}
	}

	{
		Timer tm("[OBJ] create mesh");
		// create the new mesh
		LIBSL_BEGIN;
		TriangleMesh_generic<MeshFormat_OBJ::t_VertexData> *mesh
			= new TriangleMesh_generic<MeshFormat_OBJ::t_VertexData>((uint)meshverts.size(), (uint)meshtris.size(), (uint)surfaces.size() - 1, AutoPtr<MVF>(MVF::make<MeshFormat_OBJ::t_VertexFormat>()));
		// vertices
		ForArray(mesh->vertices(),v) {
			mesh->vertexAt(v) = meshverts[v];
		}
		// faces
		ForArray(mesh->triangles(),t) {      
			mesh->triangleAt(t) = v3u( meshtris[t] );
		}
		// add surfaces
		if (surfaces.size() > 1) {
			int s = 0;
			ForIterator(t_srfMap,surfaces,S) {
				// skip first
				if (S->first == "") { continue; }
				// texture name
				mesh->surfaceAt(s).textureName = textures[ S->first ];
				// diffuse   // TODO
				mesh->surfaceAt(s).diffuse = V3F(1,1,1);
				// triangles
				mesh->surfaceAt(s).triangleIds.allocate((uint)S->second.size());
				ForIndex(t,S->second.size()) {
					mesh->surfaceAt(s).triangleIds[t] = S->second[t];
				}
				// next 
				s ++;
			}
		}
		// cerr << sprint("v: %d f: %d uvs: %d nrms: %d\n",poss.size(),tris.size(),uvtris.size(),nrmtris.size());
		return (mesh);
		LIBSL_END;
	}
}

//---------------------------------------------------------------------------

void NAMESPACE::MeshFormat_OBJ::save(const char *fname,const NAMESPACE::TriangleMesh *mesh) const
{

	// TODO/FIXME This assumes a vertex format compatible with MeshFormat_OBJ at a raw level
	//            *very* dangerous. Use a vertex attirbute accessor!

	//  throw Fatal("MeshFormat_OBJ::save - Sorry, cannot save in OBJ format (NYI)",fname);
	ofstream of(fname);
	if ( ! of ) {
		throw Fatal("MeshFormat_OBJ::save - cannot create '%s'",fname);
	}
	ForIndex(v,mesh->numVertices()) {
		MeshFormat_OBJ::t_VertexData *d = (MeshFormat_OBJ::t_VertexData*)mesh->vertexDataAt(v);
		of << "v " << d->pos[0] << ' ' << d->pos[1] << ' ' << d->pos[2] << '\n';
	}
  ForIndex(v,mesh->numVertices()) {
    MeshFormat_OBJ::t_VertexData *d = (MeshFormat_OBJ::t_VertexData*)mesh->vertexDataAt(v);
    of << "vt " << d->uv[0] << ' ' << d->uv[1] << '\n';
  }
	ForIndex(v,mesh->numVertices()) {
		MeshFormat_OBJ::t_VertexData *d = (MeshFormat_OBJ::t_VertexData*)mesh->vertexDataAt(v);
		of << "vn " << d->nrm[0] << ' ' << d->nrm[1] << ' ' << d->nrm[2] << '\n';
	}
	ForIndex(t,mesh->numTriangles()) {
		v3u tri = mesh->triangleAt(t);
		tri = tri + v3u(1);
		of << "f ";
		of << tri[0] << '/' << tri[0] << '/' << tri[0] << ' ';
		of << tri[1] << '/' << tri[1] << '/' << tri[1] << ' ';
		of << tri[2] << '/' << tri[2] << '/' << tri[2] << '\n';
	}
	of.close();
}

//---------------------------------------------------------------------------
