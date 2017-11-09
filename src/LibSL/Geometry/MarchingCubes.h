/**
* @file    MarchingCubes.h
* @author  Thomas Lewiner <tomlew@mat.puc-rio.br>
* @author  Math Dept, PUC-Rio
* @version 2008.1
* @date    07/03/2008
*
* @brief   MarchingCubes Algorithm
*/
//________________________________________________


#ifndef _MARCHINGCUBES_H_
#define _MARCHINGCUBES_H_

#if (!defined(WIN32) || defined(__CYGWIN__)) && !defined(__clang__)
#pragma interface
#endif // WIN32

namespace LibSL {
  namespace Geometry {
    namespace MarchingCubes {

      //_____________________________________________________________________________
      // types
      /** unsigned char alias */
      typedef unsigned char uchar ;
      /** signed char alias */
      typedef   signed char schar ;

      //-----------------------------------------------------------------------------
      // Vertex structure
      /** \struct Vertex "MarchingCubes.h" MarchingCubes
      * Position and normal of a vertex
      * \brief vertex structure
      * \param x X coordinate
      * \param y Y coordinate
      * \param z Z coordinate
      * \param nx X component of the normal
      * \param ny Y component of the normal
      * \param nz Z component of the normal
      */
      typedef struct
      {
        float  x,  y,  z ;  /**< Vertex coordinates */
        float nx, ny, nz ;  /**< Vertex normal */
      } Vertex ;

      //-----------------------------------------------------------------------------
      // Triangle structure
      /** \struct Triangle "MarchingCubes.h" MarchingCubes
      * Indices of the oriented triange vertices
      * \brief triangle structure
      * \param v1 First vertex index
      * \param v2 Second vertex index
      * \param v3 Third vertex index
      */
      typedef struct
      {
        int v1,v2,v3 ;  /**< Triangle vertices */
      } Triangle ;
      //_____________________________________________________________________________



      //_____________________________________________________________________________
      /** Marching Cubes algorithm wrapper */
      /** \class MarchingCubes
      * \brief Marching Cubes algorithm.
      */
      class MarchingCubes
        //-----------------------------------------------------------------------------
      {
        // Constructors
      public :
        /**
        * Main and default constructor
        * \brief constructor
        * \param size_x width  of the grid
        * \param size_y depth  of the grid
        * \param size_z height of the grid
        */
        MarchingCubes ( const int size_x = -1, const int size_y = -1, const int size_z = -1 ) ;
        /** Destructor */
        ~MarchingCubes() ;

        //-----------------------------------------------------------------------------
        // Accessors
      public :
        /** accesses the number of vertices of the generated mesh */
        inline int nverts() const { return _nverts ; }
        /** accesses the number of triangles of the generated mesh */
        inline int ntrigs() const { return _ntrigs ; }
        /** accesses a specific vertex of the generated mesh */
        inline Vertex   * vert( const int i ) const { if( i < 0  || i >= _nverts ) return ( Vertex *)NULL ; return _vertices  + i ; }
        /** accesses a specific triangle of the generated mesh */
        inline Triangle * trig( const int i ) const { if( i < 0  || i >= _ntrigs ) return (Triangle*)NULL ; return _triangles + i ; }

        /** accesses the vertex buffer of the generated mesh */
        inline Vertex   *vertices () { return _vertices  ; }
        /** accesses the triangle buffer of the generated mesh */
        inline Triangle *triangles() { return _triangles ; }

        /**  accesses the width  of the grid */
        inline int size_x() const { return _size_x ; }
        /**  accesses the depth  of the grid */
        inline int size_y() const { return _size_y ; }
        /**  accesses the height of the grid */
        inline int size_z() const { return _size_z ; }

        /**
        * changes the size of the grid
        * \param size_x width  of the grid
        * \param size_y depth  of the grid
        * \param size_z height of the grid
        */
        inline void set_resolution( const int size_x, const int size_y, const int size_z ) { _size_x = size_x ;  _size_y = size_y ;  _size_z = size_z ; }
        /**
        * selects wether the algorithm will use the enhanced topologically controlled lookup table or the original MarchingCubes
        * \param originalMC true for the original Marching Cubes
        */
        inline void set_method    ( const bool originalMC = false ) { _originalMC = originalMC ; }
        /**
        * selects to use data from another class
        * \param data is the pointer to the external data, allocated as a size_x*size_y*size_z vector running in x first
        */
        inline void set_ext_data  ( float *data )
        { if( !_ext_data ) delete [] _data ;  _ext_data = data != NULL ;  if( _ext_data ) _data = data ; }
        /**
        * selects to allocate data
        */
        inline void set_int_data  () { _ext_data = false ;  _data = NULL ; }

        // Data access
        /**
        * accesses a specific cube of the grid
        * \param i abscisse of the cube
        * \param j ordinate of the cube
        * \param k height of the cube
        */
        inline float get_data  ( const int i, const int j, const int k ) const { return _data[ i + j*_size_x + k*_size_x*_size_y] ; }
        /**
        * sets a specific cube of the grid
        * \param val new value for the cube
        * \param i abscisse of the cube
        * \param j ordinate of the cube
        * \param k height of the cube
        */
        inline void  set_data  ( const float val, const int i, const int j, const int k ) { _data[ i + j*_size_x + k*_size_x*_size_y] = val ; }

        // Data initialization
        /** inits temporary structures (must set sizes before call) : the grid and the vertex index per cube */
        void init_temps () ;
        /** inits all structures (must set sizes before call) : the temporary structures and the mesh buffers */
        void init_all   () ;
        /** clears temporary structures : the grid and the main */
        void clean_temps() ;
        /** clears all structures : the temporary structures and the mesh buffers */
        void clean_all  () ;
        /** restart with a new mesh: erases all vertices and faces */
        void restart() ;

        //-----------------------------------------------------------------------------
        // Algorithm
      public :
        /**
        * Main algorithm : must be called after init_all
        * \param iso isovalue
        */
        void run( float iso = (float)0.0 ) ;

      protected :
        /** tesselates one cube */
        void process_cube ()             ;
        /** tests if the components of the tesselation of the cube should be connected by the interior of an ambiguous face */
        bool test_face    ( schar face ) ;
        /** tests if the components of the tesselation of the cube should be connected through the interior of the cube */
        bool test_interior( schar s )    ;


        //-----------------------------------------------------------------------------
        // Operations
      protected :
        /**
        * computes almost all the vertices of the mesh by interpolation along the cubes edges
        * \param iso isovalue
        */
        void compute_intersection_points( float iso ) ;

        /**
        * routine to add a triangle to the mesh
        * \param trig the code for the triangle as a sequence of edges index
        * \param n    the number of triangles to produce
        * \param v12  the index of the interior vertex to use, if necessary
        */
        void add_triangle ( const char* trig, char n, int v12 = -1 ) ;

        /** tests and eventually doubles the vertex buffer capacity for a new vertex insertion */
        void test_vertex_addition() ;
        /** adds a vertex on the current horizontal edge */
        int add_x_vertex() ;
        /** adds a vertex on the current longitudinal edge */
        int add_y_vertex() ;
        /** adds a vertex on the current vertical edge */
        int add_z_vertex() ;
        /** adds a vertex inside the current cube */
        int add_c_vertex() ;

        /**
        * interpolates the horizontal gradient of the implicit function at the lower vertex of the specified cube
        * \param i abscisse of the cube
        * \param j ordinate of the cube
        * \param k height of the cube
        */
        float get_x_grad( const int i, const int j, const int k ) const ;
        /**
        * interpolates the longitudinal gradient of the implicit function at the lower vertex of the specified cube
        * \param i abscisse of the cube
        * \param j ordinate of the cube
        * \param k height of the cube
        */
        float get_y_grad( const int i, const int j, const int k ) const ;
        /**
        * interpolates the vertical gradient of the implicit function at the lower vertex of the specified cube
        * \param i abscisse of the cube
        * \param j ordinate of the cube
        * \param k height of the cube
        */
        float get_z_grad( const int i, const int j, const int k ) const ;

        /**
        * accesses the pre-computed vertex index on the lower horizontal edge of a specific cube
        * \param i abscisse of the cube
        * \param j ordinate of the cube
        * \param k height of the cube
        */
        inline int   get_x_vert( const int i, const int j, const int k ) const { return _x_verts[ i + j*_size_x + k*_size_x*_size_y] ; }
        /**
        * accesses the pre-computed vertex index on the lower longitudinal edge of a specific cube
        * \param i abscisse of the cube
        * \param j ordinate of the cube
        * \param k height of the cube
        */
        inline int   get_y_vert( const int i, const int j, const int k ) const { return _y_verts[ i + j*_size_x + k*_size_x*_size_y] ; }
        /**
        * accesses the pre-computed vertex index on the lower vertical edge of a specific cube
        * \param i abscisse of the cube
        * \param j ordinate of the cube
        * \param k height of the cube
        */
        inline int   get_z_vert( const int i, const int j, const int k ) const { return _z_verts[ i + j*_size_x + k*_size_x*_size_y] ; }

        /**
        * sets the pre-computed vertex index on the lower horizontal edge of a specific cube
        * \param val the index of the new vertex
        * \param i abscisse of the cube
        * \param j ordinate of the cube
        * \param k height of the cube
        */
        inline void  set_x_vert( const int val, const int i, const int j, const int k ) { _x_verts[ i + j*_size_x + k*_size_x*_size_y] = val ; }
        /**
        * sets the pre-computed vertex index on the lower longitudinal edge of a specific cube
        * \param val the index of the new vertex
        * \param i abscisse of the cube
        * \param j ordinate of the cube
        * \param k height of the cube
        */
        inline void  set_y_vert( const int val, const int i, const int j, const int k ) { _y_verts[ i + j*_size_x + k*_size_x*_size_y] = val ; }
        /**
        * sets the pre-computed vertex index on the lower vertical edge of a specific cube
        * \param val the index of the new vertex
        * \param i abscisse of the cube
        * \param j ordinate of the cube
        * \param k height of the cube
        */
        inline void  set_z_vert( const int val, const int i, const int j, const int k ) { _z_verts[ i + j*_size_x + k*_size_x*_size_y] = val ; }

        /** prints cube for debug */
        void    print_cube();

        //-----------------------------------------------------------------------------
        // Elements
      protected :
        bool      _originalMC ;   /**< selects wether the algorithm will use the enhanced topologically controlled lookup table or the original MarchingCubes */
        bool      _ext_data   ;   /**< selects wether to allocate data or use data from another class */

        int       _size_x     ;  /**< width  of the grid */
        int       _size_y     ;  /**< depth  of the grid */
        int       _size_z     ;  /**< height of the grid */
        float     *_data       ;  /**< implicit function values sampled on the grid */

        int      *_x_verts    ;  /**< pre-computed vertex indices on the lower horizontal   edge of each cube */
        int      *_y_verts    ;  /**< pre-computed vertex indices on the lower longitudinal edge of each cube */
        int      *_z_verts    ;  /**< pre-computed vertex indices on the lower vertical     edge of each cube */

        int       _nverts     ;  /**< number of allocated vertices  in the vertex   buffer */
        int       _ntrigs     ;  /**< number of allocated triangles in the triangle buffer */
        int       _Nverts     ;  /**< size of the vertex   buffer */
        int       _Ntrigs     ;  /**< size of the triangle buffer */
        Vertex   *_vertices   ;  /**< vertex   buffer */
        Triangle *_triangles  ;  /**< triangle buffer */

        int       _i          ;  /**< abscisse of the active cube */
        int       _j          ;  /**< height of the active cube */
        int       _k          ;  /**< ordinate of the active cube */

        float      _cube[8]    ;  /**< values of the implicit function on the active cube */
        uchar     _lut_entry  ;  /**< cube sign representation in [0..255] */
        uchar     _case       ;  /**< case of the active cube in [0..15] */
        uchar     _config     ;  /**< configuration of the active cube */
        uchar     _subconfig  ;  /**< subconfiguration of the active cube */
      };
      //_____________________________________________________________________________

    } // namespace LibSL
  } // namespace Geometry
} // namespace MarchingCubes

#endif // _MARCHINGCUBES_H_

// ----------------------------------------------------

/// SL: TODO: integrate this below back into a helper class
/*
public:
// Simple triangle mesh format to export isosurfaces from 3D array
typedef struct
{
LibSL::Math::v3f pos;
LibSL::Math::v3f nrm;
} t_VertexData;

typedef MVF2(LibSL::Mesh::mvf_position_3f, LibSL::Mesh::mvf_normal_3f) t_VertexFormat;
typedef LibSL::Mesh::TriangleMesh_generic<t_VertexData>               t_Mesh;

// generates mesh from 3D array using marching cubes
t_Mesh *generateShape(float iso = 0.5f) const;

template <typename T_Type, template <typename> class P_Init, class P_Check>
typename LibSL::Memory::Array::Array3D<T_Type, P_Init, P_Check>::t_Mesh *
LibSL::Memory::Array::Array3D<T_Type, P_Init, P_Check>::generateShape(float iso) const
{
// Init marching cube structure
LibSL::Geometry::MarchingCubes::MarchingCubes mc(m_XSize, m_YSize, m_ZSize);
mc.init_all();

// Fill 3D grid
ForIndex(k, m_ZSize) {
ForIndex(j, m_YSize) {
ForIndex(i, m_ZSize) {
const float v = m_Array[(k*m_YSize + j)*m_XSize + i];
mc.set_data(v, i, j, k);
}
}
}

// Run MC algorithm
mc.run(iso);

if (mc.nverts() == 0 || mc.ntrigs() == 0) {
return NULL;
}

t_Mesh *mesh = new t_Mesh( mc.nverts(), mc.ntrigs() );

ForIndex(i, mc.nverts()) {
float x = mc.vertices()[i].x;
float y = mc.vertices()[i].y;
float z = mc.vertices()[i].z;
mesh->vertexAt(i).pos = LibSL::Math::v3f(x,y,z);
mesh->vertexAt(i).nrm = 0;
}

ForIndex(i, mc.ntrigs()) {
LibSL::Math::v3u tri;
tri[0] = mc.triangles()[i].v1;
tri[1] = mc.triangles()[i].v3;
tri[2] = mc.triangles()[i].v2;
mesh->triangleAt(i) = tri;
}

// compute normals
LibSL::Memory::Array::FastArray<LibSL::Math::v3f> nrms(mesh->numVertices());
nrms.fill(0);
ForIndex(t, mesh->numTriangles()) {
int vids[3];
LibSL::Math::v3f pts[3];
ForIndex(i,3) {
vids[i] = mesh->triangleAt(t)[i];
pts[i]  = mesh->vertexAt(vids[i]).pos;
}
LibSL::Math::v3f n = normalize_safe( cross(pts[1]-pts[0],pts[2]-pts[0]) );
ForIndex(i, 3) {
nrms[vids[i]] += n;
}
}

ForIndex(v, mesh->numVertices()) {
mesh->vertexAt(v).nrm = normalize_safe(nrms[v]);
}

return mesh;
}
*/
