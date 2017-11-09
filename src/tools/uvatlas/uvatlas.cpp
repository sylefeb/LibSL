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
// --------------------------------------------------------------

#include <iostream>
#include <ctime>
#include <cmath>
#include <cstring>

/* -------------------------------------------------------- */

#include <tclap/CmdLine.h>

/* -------------------------------------------------------- */

#include <LibSL/LibSL.h>
#include <LibSL/LibSL_d3d.h>

LIBSL_WIN32_FIX;

/* -------------------------------------------------------- */
using namespace std;
/* -------------------------------------------------------- */

void mainKeyboard(unsigned char key) 
{
  if (key == 'q') {
    SimpleUI::exit();
  }
}

/* -------------------------------------------------------- */

void mainRender()
{

}

/* -------------------------------------------------------- */

HRESULT CALLBACK uvAtlasCallback(FLOAT fPercentDone,LPVOID lpUserContext)
{
  //cerr << "CB " << fPercentDone << endl;
  Console::progressTextUpdate(uint(fPercentDone*100.0f));
  return S_OK;
}

/* -------------------------------------------------------- */

typedef struct
{
  LibSL::Math::v3f pos;
  LibSL::Math::v3f nrm;
  LibSL::Math::v2f uv;
} t_VertexData;

typedef MVF3(mvf_vertex_3f,mvf_normal_3f,mvf_texcoord0_2f) t_VertexFormat;

typedef struct {
  float x,y,z;
  float nx,ny,nz;
  float u,v;
} t_vdata;

int main(int argc, char **argv) 
{
  try {

    sl_assert(sizeof(t_vdata) == sizeof(t_VertexData));

    // init UI
    SimpleUI::onRender     = mainRender;
    SimpleUI::onKeyPressed = mainKeyboard;
    SimpleUI::init(256,256);

    TCLAP::CmdLine cmd("Output is saved as 'out.mesh'", ' ', "1.0", false);

    TCLAP::ValueArg<std::string> argMesh   ("m","mesh"   ,"Mesh to load (.3ds/.obj)",true,"","string");
    TCLAP::ValueArg<int>         argWidth  ("w","width"  ,"Texture width"           ,false,256,"int");
    TCLAP::ValueArg<int>         argHeight ("h","height" ,"Texture height"          ,false,256,"int");
    TCLAP::ValueArg<float>       argStretch("s","stretch","Maximum stretch"         ,false,0.5f,"float");
    TCLAP::ValueArg<float>       argGutter ("g","gutter" ,"Gutter in between charts",false,1.0f,"float");
    TCLAP::ValueArg<int>         argNCharts("c","charts" ,"Maximum number of charts",false,0,"int");
    TCLAP::SwitchArg             argMerge  ("" ,"merge"  ,"Merge vertices"          );

    cmd.add( argMesh    );
    cmd.add( argWidth   );
    cmd.add( argHeight  );
    cmd.add( argStretch );
    cmd.add( argGutter  );
    cmd.add( argNCharts );
    cmd.add( argMerge   );
    
    cmd.parse( argc, argv );


    //SimpleUI::loop();

    {
      TriangleMesh_Ptr mesh( loadTriangleMesh<t_VertexData,t_VertexFormat>(argMesh.getValue().c_str()) );
      cerr << sprint("Mesh has %d vertices, %d triangles\n",mesh->numVertices(),mesh->numTriangles());

      if (argMerge.isSet()) {
        mesh->mergeVertices( 1e-6f );
      }
        
      const DWORD meshFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
      ID3DXMesh*  pMesh   = NULL;

      LIBSL_D3D_CHECK_ERROR(D3DXCreateMeshFVF(
        mesh->numTriangles(),
        mesh->numVertices(),
        D3DXMESH_32BIT | D3DXMESH_SYSTEMMEM,
        meshFVF,
        LIBSL_D3D_DEVICE,
        &pMesh));

      {
        // send data into the D3DX mesh
        t_VertexData *vd3d = NULL;
        LIBSL_D3D_CHECK_ERROR(pMesh->LockVertexBuffer( 0, (LPVOID*) &vd3d));
        ForIndex(v,mesh->numVertices()) {
          t_VertexData *vert = (t_VertexData *)mesh->vertexDataAt(v);
          vd3d[v].pos = vert->pos;
          vd3d[v].nrm = vert->nrm;
          vd3d[v].uv  = vert->uv;
        }
        LIBSL_D3D_CHECK_ERROR(pMesh->UnlockVertexBuffer());

        uint *id3d = NULL;
        LIBSL_D3D_CHECK_ERROR(pMesh->LockIndexBuffer( 0, (LPVOID*) &id3d));
        ForIndex(t,mesh->numTriangles()) {
          ForIndex(i,3) {
            id3d[t*3+i] = mesh->triangleAt(t)[i];
          }
        }
        LIBSL_D3D_CHECK_ERROR(pMesh->UnlockIndexBuffer());
      }

      ///////////// parameter to control this!

      // compute adjacency
      // Array<DWORD> adjacency(pMesh->GetNumFaces() * 3);
      // LIBSL_D3D_CHECK_ERROR(pMesh->GenerateAdjacency(1e-6f,adjacency.raw()));

      // compute normals
      // LIBSL_D3D_CHECK_ERROR( D3DXComputeNormals( pMesh, adjacency.raw() ) );

      /*
      // compute ITM
      ID3DXBuffer *imtData = NULL;
      Array<FLOAT> perVertexSignal(pMesh->GetNumVertices());
      perVertexSignal.fill(1.0f);
      LIBSL_D3D_CHECK_ERROR( 
        D3DXComputeIMTFromPerVertexSignal(pMesh,perVertexSignal.raw(),1,sizeof(FLOAT), D3DXIMT_WRAP_UV,(LPD3DXUVATLASCB)uvAtlasCallback,NULL,&imtData)
        );
      */

      // parameterize the mesh
      FLOAT        maxStretch = 0;
      UINT         numCharts  = 0;
      ID3DXBuffer *facePartitioning = NULL;
      ID3DXBuffer *vertexRemap      = NULL;
      Array<DWORD> falseEdges(pMesh->GetNumFaces() * 3);
      falseEdges.fill(-1);
      ID3DXMesh*   pParamMesh   = NULL;
      cerr << "Parameterizing ... ";
      Console::progressTextInit(100);
      LIBSL_D3D_CHECK_ERROR( D3DXUVAtlasCreate(
        pMesh,
        argNCharts.getValue(),
        argStretch.getValue(),
        argWidth  .getValue(),argHeight.getValue(),
        argGutter .getValue(),
        0,
        NULL, //adjacency .raw(),
        NULL, //falseEdges.raw(),
        NULL, //(const FLOAT *)imtData->GetBufferPointer(),
        uvAtlasCallback,
        0.0001f,
        NULL,
        D3DXUVATLAS_DEFAULT,
        &pParamMesh,
        &facePartitioning,
        &vertexRemap,
        &maxStretch,
        &numCharts) );
      Console::progressTextEnd();
      cerr << Console::white << sprint("%d charts, max stretch = %f\n",numCharts,maxStretch) << Console::gray;

      //  LIBSL_D3D_CHECK_ERROR( pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, adjacency, NULL, NULL, NULL) );

      // create a new triangle mesh
      TriangleMesh_generic<t_VertexData> *newmesh = new TriangleMesh_generic<t_VertexData>(pParamMesh->GetNumVertices(),pParamMesh->GetNumFaces());
      {
        // read back data
        t_VertexData *vd3d = NULL;
        LIBSL_D3D_CHECK_ERROR(pParamMesh->LockVertexBuffer( 0, (LPVOID*) &vd3d));
        ForIndex(v,newmesh->numVertices()) {
          t_VertexData *vert = (t_VertexData *)newmesh->vertexDataAt(v);
          vert->pos = vd3d[v].pos;
          vert->nrm = vd3d[v].nrm;
          vert->uv  = vd3d[v].uv;
        }
        LIBSL_D3D_CHECK_ERROR(pParamMesh->UnlockVertexBuffer());

        uint *id3d = NULL;
        LIBSL_D3D_CHECK_ERROR(pParamMesh->LockIndexBuffer( 0, (LPVOID*) &id3d));
        ForIndex(t,newmesh->numTriangles()) {
          ForIndex(i,3) {
            newmesh->triangleAt(t)[i] = id3d[t*3+i];
          }
        }
        LIBSL_D3D_CHECK_ERROR(pParamMesh->UnlockIndexBuffer());
      }

      // save
      cerr << "Saving in '" << sprint("%s.mesh",argMesh.getValue().c_str()) << "'\n";
      saveTriangleMesh(sprint("%s.mesh",argMesh.getValue().c_str()),newmesh);

      // delete meshes
      LIBSL_D3D_SAFE_RELEASE( pMesh );
      LIBSL_D3D_SAFE_RELEASE( pParamMesh );
      delete (newmesh);

    }

    // free ressources
    SimpleUI::shutdown();

  } catch (Fatal& e) {
    cerr << e.message() << endl;
    return (-1);
  } catch (TCLAP::ArgException &e) {
      cerr << Console::red << "Command line error: " << e.error() << " for arg " << e.argId() << Console::gray << endl;
    return (-1);
  }

  return (0);
}

/* -------------------------------------------------------- */
