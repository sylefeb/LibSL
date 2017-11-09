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
#include "precompiled.h"

#include <iostream>
#include <ctime>
#include <vector>

using namespace std;

// --------------------------------------------------------

typedef struct
{
  LibSL::Math::v3f pos;
  LibSL::Math::v2f uv;
} t_VertexData1;

typedef MVF2(mvf_vertex_3f,mvf_texcoord0_2f) t_VertexFormat1;

typedef struct
{
  LibSL::Math::v2f uv;
  LibSL::Math::v3f nrm;
  LibSL::Math::v3f pos;
} t_VertexData2;

typedef MVF3(mvf_texcoord0_2f,mvf_normal_3f,mvf_vertex_3f) t_VertexFormat2;


// --------------------------------------------------------

      // define a structure for vertex data
      typedef struct
      {
        LibSL::Math::v3f pos;
        LibSL::Math::v3f nrm;
        LibSL::Math::v2f uv;
      } t_VertexData;

      // describe the vertex format
      // this must match the structure definition
      typedef MVF3(mvf_vertex_3f,mvf_normal_3f,mvf_texcoord0_2f) t_VertexFormat;

      // define our own mesh type
      typedef TriangleMesh_generic<t_VertexData> t_Mesh;

// --------------------------------------------------------

void test_mesh()
{
  try {

    /// Test vertex format conversion
    AutoPtr<TriangleMesh_generic<t_VertexData1> > mesh1 = new TriangleMesh_generic<t_VertexData1>(16,1);
    ForIndex(v,mesh1->numVertices()) {
      mesh1->vertexAt(v).pos = V3F(v,v+1,v+2);
      mesh1->vertexAt(v).uv  = V2F(100+v,200+v);
    }

    //// Static cast
        cerr << endl;
        cerr << endl;
    cerr << "=== Mesh static cast ===" << endl;
    {
      // should not compile
      //    TriangleMesh_generic<t_VertexData2> mesh2 = mesh1->cast<t_VertexFormat2,t_VertexFormat2>();

      cerr << sprint(" Source data size: %d; source format size: %d \n",sizeof(t_VertexData1),MVF_sizeof<t_VertexFormat1>::value);
      AutoPtr<TriangleMesh_generic<t_VertexData2> > mesh2 = mesh1->staticCast<t_VertexData2,t_VertexFormat2, t_VertexFormat1>();
      ForIndex(v,mesh2->numVertices()) {
        cerr 
          << " xyz " << mesh2->vertexAt(v).pos 
          << " uv "  << mesh2->vertexAt(v).uv 
          << " nrm " << mesh2->vertexAt(v).nrm 
          << endl;
      }
      cerr << "mesh2 MVF: \n";
      cerr << *mesh2->mvf() << endl;
    }

    //// Dynamic cast
        cerr << endl;
        cerr << endl;
    cerr << "=== Mesh dynamic cast ===" << endl;
    {
      AutoPtr<MVF> mvf1 = MVF::make<t_VertexFormat1>();
      AutoPtr<MVF> mvf2 = MVF::make<t_VertexFormat2>();

      cerr << "dynamic MVF 1: \n";
      cerr << *mvf1 << endl;
      cerr << "dynamic MVF 2: \n";
      cerr << *mvf2 << endl;

      AutoPtr<TriangleMesh_generic<t_VertexData2> > mesh2 = mesh1->dynamicCast<t_VertexData2>(mvf2,mvf1);
      ForIndex(v,mesh2->numVertices()) {
        cerr 
          << " xyz " << mesh2->vertexAt(v).pos 
          << " uv "  << mesh2->vertexAt(v).uv 
          << " nrm " << mesh2->vertexAt(v).nrm 
          << endl;
      }
      cerr << "mesh2 MVF: \n";
      cerr << *mesh2->mvf() << endl;
    }

        cerr << endl;
        cerr << endl;
    cerr << "=== Mesh dynamic MVF ===" << endl;
    {
      // mesh creation with specified dynamic MVF
      AutoPtr<TriangleMesh_generic<t_VertexData1> > mesh3 = new TriangleMesh_generic<t_VertexData1>(16,1,0,MVF::make<t_VertexFormat1>());
      cerr << "mesh3 MVF: \n";
      cerr << *mesh3->mvf() << endl;
      // try to set a wrong MVF
      try {
        mesh3->setMvf(MVF::make<t_VertexFormat2>());
      } catch (Fatal& e) {
        cerr << Console::yellow << e.message() << Console::gray << endl;
      }
    }

    cerr << endl;
    cerr << endl;
    cerr << "=== Mesh generic with Array per vertex and dynamic MVF ===" << endl;
    {
      {
        cerr << endl;
        cerr << "mesh with Array per vertex, converted from struct per vertex mesh: \n";
        cerr << endl;

        // conversion test
        AutoPtr<MVF> mvfsrc = MVF::make<t_VertexFormat1>();
        AutoPtr<MVF> mvfdst = MVF::make<t_VertexFormat2>();

        AutoPtr<TriangleMesh_generic<Array<float> > > meshB = mesh1->dynamicCast<Array<float> >(mvfdst,mvfsrc);
        ForIndex(v,meshB->numVertices()) {
          ForIndex(i,meshB->vertexAt(v).size()) {
            cerr << meshB->vertexAt(v)[i] << ' ';
          }
          cerr << endl;
        }
      }

      {
        cerr << endl;
        cerr << "mesh with Array per vertex, converted from a std::vector per vertex mesh: \n";
        cerr << endl;

        // conversion test
        AutoPtr<MVF> mvf1 = MVF::make<t_VertexFormat1>();
        AutoPtr<TriangleMesh_generic<vector<float> > > mesh1 = new TriangleMesh_generic<vector<float> >(9,1,0,mvf1);
        ForIndex(v,mesh1->numVertices()) {
          mesh1->vertexAt(v)[0] = v+1;
          mesh1->vertexAt(v)[1] = v+2;
          mesh1->vertexAt(v)[2] = v+3;
          mesh1->vertexAt(v)[3] = 100+v;
          mesh1->vertexAt(v)[4] = 200+v;
        }
        cerr << "  first mesh: " << endl;
        ForIndex(v,mesh1->numVertices()) {
          ForIndex(i,mesh1->vertexAt(v).size()) {
            cerr << mesh1->vertexAt(v)[i] << ' ';
          }
          cerr << endl;
        }

        cerr << "  second mesh: " << endl;
        AutoPtr<MVF> mvfdst = MVF::make<t_VertexFormat2>();
        AutoPtr<TriangleMesh_generic<Array<float> > > meshB = mesh1->dynamicCast<Array<float> >(mvfdst,mvf1);
        ForIndex(v,meshB->numVertices()) {
          ForIndex(i,meshB->vertexAt(v).size()) {
            cerr << meshB->vertexAt(v)[i] << ' ';
          }
          cerr << endl;
        }
      }
    }

    cerr << endl;
    cerr << endl;
    cerr << "=== Save / load mesh with MVF ===" << endl;
    {
      cerr << "  save with mvf 1\n";
      mesh1->setMvf(MVF::make<t_VertexFormat1>());
      saveTriangleMesh("mesh1.mesh",mesh1);
      cerr << "  load as mvf 2\n";
      AutoPtr<TriangleMesh_generic<t_VertexData2> > mesh2 = 
        loadTriangleMesh<t_VertexData2,t_VertexFormat2>("mesh1.mesh");
      ForIndex(v,mesh2->numVertices()) {
        cerr 
          << " xyz " << mesh2->vertexAt(v).pos 
          << " uv "  << mesh2->vertexAt(v).uv 
          << " nrm " << mesh2->vertexAt(v).nrm 
          << endl;
      }
      cerr << "mesh2 MVF: \n";
      cerr << *mesh2->mvf() << endl;
    }

    cerr << endl;
    cerr << endl;
    cerr << "=== Code snippet; load mesh and convert ===" << endl;
    {

      // load a mesh and ask for conversion into our type
      AutoPtr<t_Mesh> myMesh;
      myMesh = loadTriangleMesh<t_VertexData,t_VertexFormat>("media\\plane.obj");

      // go through all triangles
      ForIndex(t,myMesh->numTriangles()) {
        v3u tri = myMesh->triangleAt(t);
        cerr << "Triangle " << t << endl;
        // go through triangle vertices
        ForIndex(i,3) {
          int vid = tri[i];
          v2f uv  = myMesh->vertexAt(vid).uv;
          v3f nrm = myMesh->vertexAt(vid).nrm;
          v3f pos = myMesh->vertexAt(vid).pos;
          cerr << "  <"
          << " p" << pos
          << " t"  << uv
          << " n" << nrm
          << '>' << endl;
        }
        cerr << endl;
      }

      // save as mesh
      saveTriangleMesh("test.mesh",myMesh);

      // load again as base mesh
      TriangleMesh_Ptr mesh = loadTriangleMesh("test.mesh");

      // out put positions
      ForIndex(v,mesh->numVertices()) {
        const v3f& p = mesh->posAt(v);
        cerr << "v" << v << " :" << p << endl;
        mesh->posAt(v) += V3F(10,20,30);
      }
      // after modif
      cerr << "  modified:" << endl;
      ForIndex(v,mesh->numVertices()) {
        const v3f& p = mesh->posAt(v);
        cerr << "v" << v << " :" << p << endl;
      }

      // mvf
      MVF *mvf = MVF::make<t_VertexFormat>();
      ForIndex(n,mvf->attributes().size()) {
        cerr << mvf->attributes()[n].binding       << endl;
        cerr << mvf->attributes()[n].type          << endl;
        cerr << mvf->attributes()[n].numComponents << endl;
        cerr << mvf->attributes()[n].index         << endl;
        cerr << mvf->attributes()[n].size_of       << endl;
        cerr << mvf->attributes()[n].offset        << endl;
      }

    }

  } catch (Fatal& e) {
    cerr << e.message() << endl;
  }

}

/* -------------------------------------------------------- */
