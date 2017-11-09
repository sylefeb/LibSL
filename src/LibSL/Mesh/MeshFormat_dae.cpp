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

#include "MeshFormat_dae.h"
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
  NAMESPACE::MeshFormat_dae s_dae;  /// FIXME: this mechanism does not work with VC++
}                                   ///        see also MeshFormatManager constructor

//---------------------------------------------------------------------------

NAMESPACE::MeshFormat_dae::MeshFormat_dae()
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

void findNodes(TiXmlNode *root,vector<TiXmlNode*>& _nodes,const char *name)
{
  vector<TiXmlNode*> toVisit;

  toVisit.push_back(root);
  TiXmlNode *node;
  while ( !toVisit.empty() ) {
    node = toVisit.back();
    toVisit.pop_back();
    TiXmlNode *child;
    for ( child = node->FirstChild(); child; child = child->NextSibling() )
    {
      if ( !strcmp( child->Value(), name ) ) {
        _nodes.push_back( child );
      } else {
        toVisit.push_back( child );
      }
    }
  }
}

//---------------------------------------------------------------------------

TiXmlElement *findElement( const char *name,  TiXmlNode *node )
{
  for ( TiXmlElement *child = node->FirstChildElement(); child; child = child->NextSiblingElement() ) {
    if ( !strcmp( child->Value(), name ) ) {
        return child;
      }
  }
  return NULL;
}

//---------------------------------------------------------------------------

void readSources( TiXmlNode *mesh,  map< string, vector<float> >& _sources )
{
  for ( TiXmlElement *child = mesh->FirstChildElement(); child; child = child->NextSiblingElement() ) {
    if ( !strcmp( child->Value(), "source" ) ) {
      TiXmlElement *ar = findElement( "float_array", child);
      if ( ar != NULL ) {
        uint nfloats = atoi( ar->Attribute("count") );
        // verbose
        //cerr << child->Attribute("id") << endl;
        //cerr << nfloats << " floats\n";
        // read the floats
        vector<float>& vf = _sources[ child->Attribute("id") ];
        vf.resize( nfloats);
        string all = ar->GetText();
        istringstream sstr( all );
        ForIndex(f,nfloats) {
          sstr >> vf[ f ];
        }
      }
    }
  }
}

//---------------------------------------------------------------------------

typedef struct {
  v3f    color;
  string texture;
} t_effect;

//---------------------------------------------------------------------------

NAMESPACE::TriangleMesh *NAMESPACE::MeshFormat_dae::load(const char *fname) const
{
  LIBSL_BEGIN;

  // open file
	TiXmlDocument doc( fname );

  //cerr << Console::white << fname << Console::gray <<endl;

  bool ok = doc.LoadFile();
  if ( !ok ) {
    throw Fatal("[MeshFormat_dae::load] - cannot load '%s'",fname);
  }
  // search for all images
  vector< TiXmlNode * > xml_images;
  findNodes( doc.RootElement(), xml_images, "image" );
  map<string,string> textures;
  ForIndex(i,xml_images.size()) {
    textures[ dynamic_cast<TiXmlElement*>(xml_images[i])->Attribute("id") ] = xml_images[i]->FirstChildElement()->GetText();
    //cerr << dynamic_cast<TiXmlElement*>(xml_images[i])->Attribute("id") << " - " << xml_images[i]->FirstChildElement()->GetText() << endl;
  }
  // search for all effects
  vector< TiXmlNode * > xml_effects;
  findNodes( doc.RootElement(), xml_effects, "effect" );
  map<string,t_effect> effects;
  ForIndex(i,xml_effects.size()) {
    string id = dynamic_cast<TiXmlElement*>(xml_effects[i])->Attribute("id");
    t_effect ef;
    ef.color   = v3f(1);
    ef.texture = "";
    vector< TiXmlNode *> clr;
    findNodes( xml_effects[i], clr, "color" );
    if ( ! clr.empty()) {
      istringstream istr( dynamic_cast<TiXmlElement*>(clr[0])->GetText() );
      istr >> ef.color[0];
      istr >> ef.color[1];
      istr >> ef.color[2];
    } else {
      vector< TiXmlNode *> tex;
      findNodes( xml_effects[i], tex, "init_from" );
      if (!tex.empty()) {
        string id_tex = dynamic_cast<TiXmlElement*>(tex[0])->GetText();
        ef.texture    = textures[ id_tex ];
      }
    }
    // cerr << "EFFECT " << id << " tex " << ef.texture << endl;
    effects[ id ] = ef;
  }
  // search for all materials
  vector< TiXmlNode * > xml_materials;
  findNodes( doc.RootElement(), xml_materials, "material" );
  map<string,string> material2texture;
  ForIndex(i,xml_materials.size()) {
    TiXmlElement *ef = xml_materials[i]->FirstChildElement();
    string strid = string(ef->Attribute("url"));
    string efid  = strid.substr(1,strid.length()-1);
    string matid = dynamic_cast<TiXmlElement*>(xml_materials[i])->Attribute("id");
    material2texture.insert( make_pair(matid, effects[ efid ].texture ) );
    cerr << "material " << dynamic_cast<TiXmlElement*>(xml_materials[i])->Attribute("name") << " id: " <<
      dynamic_cast<TiXmlElement*>(xml_materials[i])->Attribute("id") << " effect: " << efid;
    cerr << " color: " << effects[ efid ].color << " texture: " << effects[ efid ].texture << endl;
  }
  // search for all instance_geometry
  vector< TiXmlNode * > xml_instance_geometry;
  map<string,string> geometry2material;
  findNodes( doc.RootElement(), xml_instance_geometry, "instance_geometry" );
  ForIndex(i,xml_instance_geometry.size()) {
    vector< TiXmlNode * > instance_material;
    findNodes( xml_instance_geometry[i], instance_material, "instance_material" );
    if ( instance_material.size() > 0) {
      ForIndex(m,instance_material.size()) {
        string geoid = dynamic_cast<TiXmlElement*>(xml_instance_geometry[i])->Attribute("url");
        geoid = geoid.substr(1,geoid.length()-1);
        string matid = dynamic_cast<TiXmlElement*>(instance_material[m]    )->Attribute("target");
        matid = matid.substr(1,matid.length()-1);
        if ( ! material2texture[matid].empty()) {
          geometry2material.insert( make_pair(geoid,matid) );
          break;
        }
      }
    }
  }
  // search for all meshes
  vector< TiXmlNode * > meshes;
  findNodes( doc.RootElement(), meshes, "mesh" );
  // parse and create final mesh
  vector< t_VertexData >  vertices;
  vector< v3u >           triangles;
  vector< Array< uint > > surfaces;
  map<int,string>         surface2geometry;
  ForIndex( m, meshes.size() ) {
    // find parent geometry ID
    string parentid = dynamic_cast<TiXmlElement*>(meshes[m]->Parent())->Attribute("id");
    // cerr << Console::white << parentid << Console::gray <<endl;
    // get all sources
    map< string, vector<float> > sources;
    readSources( meshes[m], sources );
    // get vertex definition
    TiXmlElement *verts = findElement( "vertices", meshes[m] );
    string id_pos,id_nrm;
    for ( TiXmlElement *input = verts->FirstChildElement(); input; input = input->NextSiblingElement() ) {
      if ( !strcmp(input->Value(),"input")) {
        if ( !strcmp(input->Attribute("semantic"),"POSITION") ) {
          string strid = string(input->Attribute("source"));
          id_pos = strid.substr(1,strid.length()-1);
          // cerr << "positions: " << id_pos << endl;
        } else if ( !strcmp(input->Attribute("semantic"),"NORMAL") ) {
          string strid = string(input->Attribute("source"));
          id_nrm = strid.substr(1,strid.length()-1);
          // cerr << "normals  : " << id_nrm << endl;
          sl_assert( sources[id_nrm].size() % 3 == 0 );
        }
      }
    }
    sl_assert(!id_pos.empty());
/*
    uint nverts    = sources[id_pos].size()/3;
    // append vertices
    if (!id_pos.empty()) {
      sl_assert( sources[id_pos].size() % 3 == 0 );
      if (!id_nrm.empty()) sl_assert( sources[id_nrm].size() % 3 == 0 );
      ForIndex(p,sources[id_pos].size()/3) {
        t_VertexData dta;
        dta.pos = V3F( sources[id_pos][p*3+0],sources[id_pos][p*3+1],sources[id_pos][p*3+2] );
        if (!id_nrm.empty()) dta.nrm = V3F( sources[id_nrm][p*3+0],sources[id_nrm][p*3+1],sources[id_nrm][p*3+2] );
        vertices.push_back( dta );
      }
    }
*/
    // read triangles
    for ( TiXmlElement *elem = meshes[m]->FirstChildElement(); elem; elem = elem->NextSiblingElement() ) {
      if ( !strcmp(elem->Value(),"triangles")) {
        bool tri_has_uv = false;
        string id_uv;
        // check inputs
        for ( TiXmlElement *input = elem->FirstChildElement(); input; input = input->NextSiblingElement() ) {
          if ( !strcmp(input->Value(),"input")) {
            if ( !strcmp(input->Attribute("semantic"),"TEXCOORD") ) {
              tri_has_uv = true;
              string strid = string(input->Attribute("source"));
              id_uv  = strid.substr(1,strid.length()-1);
              // cerr << " ID UV = " << id_uv << endl;
            }
          }
        }
        uint ntris = atoi( elem->Attribute("count") );
        // prepare surface
        surface2geometry.insert( make_pair((int)surfaces.size(), parentid) );
        surfaces.push_back( Array<uint>() );
        surfaces.back().allocate( ntris );
        // read indices
        TiXmlElement *idxs = findElement( "p", elem );
        string        all  = idxs->GetText();
        istringstream sstr( all );
        int                      nverts = 0;
        map< v2i,int > vertset;
        ForIndex(t,ntris) {
          v3u tri;
          int posid,texid=-1;
          ForIndex(i,3) {
            sstr >> posid;
            if (tri_has_uv) {
              sstr >> texid;
            }
            int vid = 0;
            if ( vertset.find( V2I(posid,texid) ) == vertset.end() ) {
              // add vertex
              vid = nverts;
              vertset.insert( make_pair( V2I(posid,texid) , vid ) );
              nverts ++;
            } else {
              vid = vertset[ V2I(posid,texid) ];
            }
            tri[i] = (int)vertices.size() + vid;
          }
          triangles.push_back( tri );
          surfaces .back()[ t ] = (int)triangles.size() - 1;
        }
        // append vertices
        int start = (int)vertices.size();
        vertices.resize( vertices.size() + nverts );
        for ( map< v2i,int >::const_iterator V = vertset.begin(); V != vertset.end() ; V++ ) {
          t_VertexData dta;
          int vid = V->first[0];
          int tid = V->first[1];
          dta.pos = V3F( sources[id_pos][vid*3+0],sources[id_pos][vid*3+1],sources[id_pos][vid*3+2] );
          if ( tid != -1 )      { dta.uv  = V2F( sources[id_uv ][tid*2+0],1.0f - sources[id_uv ][tid*2+1] ); }
          if ( ! id_nrm.empty() ) dta.nrm = V3F( sources[id_nrm][vid*3+0],sources[id_nrm][vid*3+1],sources[id_nrm][vid*3+2] );
          vertices[ start + V->second ] = dta;
        }
      }
    }
  }
  // build mesh
  {
    TriangleMesh_generic<MeshFormat_dae::t_VertexData> *m = new TriangleMesh_generic<MeshFormat_dae::t_VertexData>(
			(uint)vertices.size(), (uint)triangles.size(), (uint)surfaces.size(),
      AutoPtr<MVF>(MVF::make<MeshFormat_dae::t_VertexFormat>()));
    LIBSL_BEGIN;
    ForIndex(t,triangles.size()) {
      m->triangleAt(t) = triangles[t];
    }
    /*
    int cpos = string(fname).rfind('\\'); // TODO: this creates trouble under linux, etc.
    string path;
    if (cpos != string::npos) {
      path = string(fname).substr(0,cpos+1);
    }
    */
    ForIndex(s,surfaces.size()) {
      string tex = material2texture[geometry2material[surface2geometry[s]]];
      if ( ! tex.empty() ) {
        m->surfaceAt(s).textureName =  /*path +*/ tex;
      } else {
        m->surfaceAt(s).textureName = "default";
      }
      // cerr << "SURFACE " << m->surfaceAt(s).textureName << endl;
      m->surfaceAt(s).triangleIds = surfaces[s];
      m->surfaceAt(s).diffuse     = V3F(1,1,1);
    }
    ForIndex(p,vertices.size()) {
      m->vertexAt(p) = vertices[p];
    }
    LIBSL_END;
    // done
    return (m);
  }
  LIBSL_END;
}

//---------------------------------------------------------------------------

void NAMESPACE::MeshFormat_dae::save(const char *fname,const NAMESPACE::TriangleMesh *mesh) const
{
    throw Fatal("[MeshFormat_dae::save] - not implemented");
}

//---------------------------------------------------------------------------
