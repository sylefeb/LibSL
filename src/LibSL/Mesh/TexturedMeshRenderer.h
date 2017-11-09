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
// ------------------------------------------------------
// LibSL::TexturedMeshRenderer
// ------------------------------------------------------
//
// Renderer for textured meshes
//
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2008-09-07
// ------------------------------------------------------

#pragma once

#include <LibSL/Mesh/Mesh.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Image/Image.h>
#include <LibSL/GPUTex/GPUTex.h>

#include <set>
#include <map>

namespace LibSL {
  namespace Mesh {

    using namespace LibSL::Math;
    using namespace LibSL::CppHelpers;
    using namespace LibSL::Image;

      // ----------------

      // Base texture provider
      class TextureProvider
      {
      public:
        typedef LibSL::GPUTex::Tex2D_api::t_API::t_Handle2D                  t_TexHandle;
      public:
        TextureProvider() {}
        virtual ~TextureProvider() {}
        virtual TextureProvider::t_TexHandle getTexture (const std::string& tname) = 0;
      }; // class TextureProvider

      // ----------------

      // DefaultTextureProvider - simple implementation of a texture provider
      class DefaultTextureProvider : public TextureProvider
      {
      protected:

        std::string                     m_TexturePath;
        std::string                     m_TextureExtension;
        std::vector<std::string>        m_TextureSuffixes;
        LibSL::GPUTex::Tex2DRGBA_Ptr    m_DefaultTexture;

        std::map<std::string,LibSL::GPUTex::Tex2DRGBA_Ptr> m_Loaded;

      protected:

        /* Resolve the name of the texture as 'name[suffix][extension]'
        All suffixes will be tried when loading, so if you pass suffix0..N, it will
        try those first, and then no suffix. The extension will be always appended.
        */
        virtual std::string resolveName(const std::string& name)
        {
          // try all tname + extension + suffixes combinations
          ForIndex(s,m_TextureSuffixes.size())
          {
            std::string resolved = (m_TexturePath + name + m_TextureSuffixes[s] + m_TextureExtension);
            if ( System::File::exists(System::File::adaptPath( resolved.c_str() )) ) {
              return resolved;
            }
          }
          // try without suffix
          std::string resolved = (m_TexturePath + name + m_TextureExtension);
          if ( System::File::exists(System::File::adaptPath( resolved.c_str() )) ) {
            return resolved;
          }
          return name; // could not find, return name and see what happens ...
        }

        // Create a default texture in case of missing file
        TextureProvider::t_TexHandle getDefaultTextureHandle ()
        {
          if (m_DefaultTexture.isNull ()) {
            buildDefaultTexture ();
          }
          return m_DefaultTexture->texture ();
        }

        void buildDefaultTexture()
        {
          std::cerr << "Building default texture.\n";
          LibSL::Image::ImageRGBA cb(256,256);
          ForImage((&cb),i,j) {
            cb.pixel(i,j) = V4B(255,255,255,255);
          }
          m_DefaultTexture = LibSL::GPUTex::Tex2DRGBA_Ptr(new LibSL::GPUTex::Tex2DRGBA(cb.pixels(), GPUTEX_AUTOGEN_MIPMAP));
        }

        bool loadTexture(const std::string& name,TextureProvider::t_TexHandle& _out)
        {
          std::cerr << "Loading texture: '" << name << '\'';
          // Check if the file exists
          if (! System::File::exists (name.c_str ())) {
            std::cerr << Console::red << " **not found**" << Console::gray << std::endl;
            return false;
          }
          // check if already loaded
          const std::map<std::string,LibSL::GPUTex::Tex2DRGBA_Ptr>::const_iterator T = m_Loaded.find(name);
          if (T != m_Loaded.end()) {
            _out = (*T).second->texture();
            std::cerr << " [OK]" << std::endl;
            return true;
          } else {
            // load image from file
            LibSL::Image::Image *img  = loadImage(name.c_str());
            sl_assert( img != NULL );
            // try to cast
            ImageRGBA           *rgba = dynamic_cast<ImageRGBA*>( img );
            if (rgba == NULL) {
              ImageRGB          *rgb  = dynamic_cast<ImageRGB*>( img );
              if (rgb == NULL) {
                std::cerr << Console::red << " **format mismatch**" << Console::gray << std::endl;
                return false;
              } else {
                rgba = rgb->cast<ImageRGBA>();
                ForImage(rgba,i,j) { rgba->pixel(i,j)[3] = 255; }
              }
            }
            sl_assert( rgba != NULL );
            LibSL::GPUTex::Tex2DRGBA_Ptr tex = LibSL::GPUTex::Tex2DRGBA_Ptr(new LibSL::GPUTex::Tex2DRGBA(rgba->pixels(), GPUTEX_AUTOGEN_MIPMAP));
            m_Loaded[name]                   = tex;
            _out                             = tex->texture();
            std::cerr << " [OK]" << std::endl;
            return true;
          }
        }

      public:

        DefaultTextureProvider() { }

        DefaultTextureProvider(
          const std::string&             texturePath,
          LibSL::GPUTex::Tex2DRGBA_Ptr   defaultTex = LibSL::GPUTex::Tex2DRGBA_Ptr(),
          const std::string&             extension  = std::string (),
          const std::vector<std::string> suffixes   = std::vector<std::string> ())
          :
          m_TexturePath(texturePath),
          m_TextureExtension(extension),
          m_TextureSuffixes (suffixes),
          m_DefaultTexture(defaultTex)
        { }

        // Get a texture
        virtual TextureProvider::t_TexHandle getTexture (const std::string& name)
        {
          // resolve name
          std::string resolved = resolveName( name );
          // load texture
          TextureProvider::t_TexHandle handle = 0;
          if (loadTexture(resolved, handle)) {
            return handle;
          } else {
            return getDefaultTextureHandle ();
          }
        }

      }; // class DefaultTextureProvider

      // ----------------

      //  CallbackTextureProvider - provides a callback function called each time a texture has been selected
      class CallbackTextureProvider : public DefaultTextureProvider
      {
      public:

        typedef void (*OnTextureSelected) (const std::string& name,const std::string& requested);

      protected:

        OnTextureSelected								m_TextureSelectedCallback;

      public:

        CallbackTextureProvider() { }
        CallbackTextureProvider(
          const std::string&             texturePath,
          LibSL::GPUTex::Tex2DRGBA_Ptr   defaultTex = LibSL::GPUTex::Tex2DRGBA_Ptr(),
          const std::string&             extension  = std::string (),
          const std::vector<std::string> suffixes   = std::vector<std::string> (),
          OnTextureSelected              callback   = NULL)
          :
        DefaultTextureProvider(texturePath,defaultTex,extension,suffixes), m_TextureSelectedCallback (callback)
        { }

        virtual TextureProvider::t_TexHandle getTexture(const std::string& name)
        {
          // resolve name
          std::string resolved = resolveName( name );
          // load texture
          TextureProvider::t_TexHandle handle = 0;
          if (loadTexture(resolved, handle)) {
            if (m_TextureSelectedCallback != NULL) {
              m_TextureSelectedCallback(name,resolved);
            }
            return handle;
          } else {
            return getDefaultTextureHandle ();
          }
        }

      }; // class CallbackTextureProvider


    // TexturedMeshRenderer - handles automatically rendering of textured meshes
    template <typename T_VertexFormat>
    class TexturedMeshRenderer
    {
    public:

      typedef LibSL::Memory::Pointer::AutoPtr<TexturedMeshRenderer>        t_AutoPtr;
      typedef GPUMesh::GPUMesh_Indexed_VertexBuffer<T_VertexFormat,uint>   t_RenderMesh;

    protected:

      typedef struct {
        TextureProvider::t_TexHandle                   texture;
        v3f                                            diffuse;
        LibSL::Memory::Pointer::AutoPtr<t_RenderMesh>  renderMesh;
      } t_SurfaceMesh;

      LibSL::Memory::Array::Array<t_SurfaceMesh>       m_Surfaces;

    protected:

      LibSL::Memory::Pointer::AutoPtr<TextureProvider> m_Provider;

    public:

      TexturedMeshRenderer(
        LibSL::Mesh::TriangleMesh_Ptr                    m,
        LibSL::Memory::Pointer::AutoPtr<TextureProvider> provider = LibSL::Memory::Pointer::AutoPtr<TextureProvider>(new DefaultTextureProvider()))
      {
        if (m->sizeOfVertexData() != MVF_sizeof<T_VertexFormat>::value) {
          throw LibSL::Errors::Fatal("[TexturedMeshRenderer::TexturedMeshRenderer] - vertex format non compatible with vertex data! (sizeof(vf)=%d sizeof(vd)=%d)",MVF_sizeof<T_VertexFormat>::value,m->sizeOfVertexData());
        }
        if (m->numSurfaces() == 0) {
          throw LibSL::Errors::Fatal("[TexturedMeshRenderer::TexturedMeshRenderer] - mesh has no textured surface");
        }
        // provider
        m_Provider = provider;
        // build meshes for rendering
        m_Surfaces.allocate(m->numSurfaces());
        ForIndex(s,m->numSurfaces()) {
          if (m->surfaceNumTriangles(s) == 0) { continue; }
          m_Surfaces[s].renderMesh = LibSL::Memory::Pointer::AutoPtr<t_RenderMesh>(new t_RenderMesh(GPUMESH_DYNAMIC));
          m_Surfaces[s].renderMesh->begin( GPUMESH_TRIANGLELIST );
          // gather surface vertices
          std::set<uint>      verts;
          ForIndex(t,m->surfaceNumTriangles(s)) {
            uint                                         tid = m->surfaceTriangleIdAt(s,t);
            const LibSL::Mesh::TriangleMesh::t_Triangle& tri = m->triangleAt(tid);
            ForIndex(i,3) { verts.insert( tri[i] ); }
          }
          // make vertices
          std::map<uint,uint> vMap;
          int n = 0;
          ForConstIterator(std::set<uint>,verts,V) {
            uint vid = (*V);
            m_Surfaces[s].renderMesh->vertex_raw( m->vertexDataAt(vid),m->sizeOfVertexData() );
            vMap.insert( std::make_pair(vid,n++) );
          }
          // make triangles
          ForIndex(t,m->surfaceNumTriangles(s)) {
            uint tid = m->surfaceTriangleIdAt(s,t);
            const LibSL::Mesh::TriangleMesh::t_Triangle& tri = m->triangleAt(tid);
            ForIndex(i,3) {
              sl_assert(vMap.find(tri[i]) != vMap.end())
                m_Surfaces[s].renderMesh->index( vMap[ tri[i] ] );
            }
          }
          m_Surfaces[s].renderMesh->end();
          // cerr << "NAME: " << m->surfaceTextureName(s) << endl;
          sl_assert( ! m_Provider.isNull() );
          m_Surfaces[s].texture = m_Provider->getTexture( m->surfaceTextureName(s) );
        }
      }

      void update(LibSL::Mesh::TriangleMesh_Ptr m) /// TODO / FIXME improve this mechanism
      {
        // update vertices
        sl_assert(m->numSurfaces() == m_Surfaces.size());
        ForIndex(s,m->numSurfaces()) {
          if (m->surfaceNumTriangles(s) == 0) { continue; }
          // -> gather surface vertices
          std::set<uint>      verts;
          ForIndex(t,m->surfaceNumTriangles(s)) {
            uint                                         tid = m->surfaceTriangleIdAt(s,t);
            const LibSL::Mesh::TriangleMesh::t_Triangle& tri = m->triangleAt(tid);
            ForIndex(i,3) { verts.insert( tri[i] ); }
          }
          // -> update data
          m_Surfaces[s].renderMesh->beginVertexUpdate();
          ForConstIterator(std::set<uint>,verts,V) {
            int vid = (*V);
            m_Surfaces[s].renderMesh->vertex_raw( m->vertexDataAt(vid),m->sizeOfVertexData() );
          }
          m_Surfaces[s].renderMesh->endVertexUpdate();
        }
      }

      void render()
      {
        ForIndex(s,m_Surfaces.size()) {
          renderSurface(s);
        }
      }

      // NOTE: this ignores the surface diffuse properties
      //       to enable diffuse color use a shader or implement
      //       your own version of TexturedRenderer
      void renderSurface(int s)
      {
        if (m_Surfaces[s].renderMesh.isNull()) { return; }
        LibSL::GPUTex::Tex2D_api::t_API::bindTexture2D(m_Surfaces[s].texture);
        m_Surfaces[s].renderMesh->render();
      }

    };

  } //namespace LibSL::Mesh
} //namespace LibSL
