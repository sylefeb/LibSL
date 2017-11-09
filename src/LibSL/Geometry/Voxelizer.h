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
// LibSL::Geometry::Voxelizer
// ------------------------------------------------------
//
// Voxelizer for triangle meshes
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2007-12-10
// ------------------------------------------------------

#pragma once

#include <LibSL/Errors/Errors.h>
#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Math.h>
#include <LibSL/Geometry/AAB.h>
#include <LibSL/Mesh/Mesh.h>

#include <map>
#include <vector>

// ------------------------------------------------------

namespace LibSL {
  namespace Geometry {

    using namespace Math;
    using namespace CppHelpers;
    using namespace Memory::Array;

    /// A single voxel
    //  This is an interface from which to derive to specify your own voxel class
    //  The three methods to implement are:
    //   - shouldSubdivide:            returns true if voxel is to be subdivided
    //   - shouldDeleteAfterSubdivide: returns true if voxel must be deleted after subdivision
    //                                 (for instance if only leaves are desired)
    //   - shouldKeepChild():          returns true if a child voxel (after subdivision) must be kept
    //                                 NOTE: triangles from parent are already distributed in voxel
    class Voxel
    {
    protected:
      LibSL::Geometry::AAB<3> m_Box;       // voxel box
      std::vector<uint>       m_Triangles; // triangles hit by voxel box
      LibSL::Math::v3s				m_Pos;
      uchar                   m_Depth;     // voxel depth
    public:

      Voxel() {}

      virtual bool shouldSubdivide()                = 0;
      virtual bool shouldDeleteAfterSubdivide()     = 0;
      virtual bool shouldKeepChild()                = 0;

      uchar&													depth()           {return (m_Depth); }
      const uchar&										depth()     const {return (m_Depth); }
      LibSL::Geometry::AAB<3>&				box()             {return (m_Box); }
      const LibSL::Geometry::AAB<3>&	box()       const {return (m_Box); }
      std::vector<uint>&							triangles()       {return (m_Triangles); }
      const std::vector<uint>&				triangles() const {return (m_Triangles); }
      const LibSL::Math::v3s&					pos () const			{return (m_Pos);	}
      LibSL::Math::v3s&								pos ()						{return (m_Pos);	}
    };

    struct DefaultVoxelFactory
    {
    };


    template <typename T_Voxel,typename Factory>
    struct FactoryWrapper
    {
      static T_Voxel Create (const Factory& factory)
      {
        return factory.Create ();
      }
    };

    template <typename T_Voxel>
    struct FactoryWrapper<T_Voxel,DefaultVoxelFactory>
    {
      static T_Voxel Create (const DefaultVoxelFactory&)
      {
        return T_Voxel ();
      }
    };

    /**
    * A VoxelFactory must provide a copy-constructor and a member
    * function T_Voxel Create() const, which returns a new voxel.
    *
    * By default, the voxels will be created by calling T_Voxel().
    */
    template<class T_Voxel, bool T_Verbose=true, typename T_VoxelFactory = DefaultVoxelFactory > // T_Voxel must inherit Voxelizer::Voxel
    class Voxelizer
    {

    public:

      typedef FactoryWrapper<T_Voxel,T_VoxelFactory> t_FactoryWrapper;
      typedef T_Voxel                                t_Voxel;
      typedef std::map< LibSL::Math::v3s, T_Voxel >  t_VoxelMap;
      typedef std::vector<t_VoxelMap>                t_Voxelization;

      T_VoxelFactory m_Factory;

      Voxelizer (const T_VoxelFactory& factory = T_VoxelFactory ())
        : m_Factory (factory)
      {
      }

    private:

      // -------

      void voxelize_rec(
        const LibSL::Mesh::TriangleMesh_Ptr mesh,
        t_Voxelization&                     voxels,
        const uint                          depth,
        const uint                          mind,
        const uint                          maxd) const
      {
        //LIBSL_BEGIN;
        if (depth > maxd) {
          return;
        }
        voxels.push_back(t_VoxelMap());
        uint numcolliding = 0;
        if (T_Verbose) { Console::processingInit(); }
        // go through previous depth voxels, decide which to keep or subdivide
        Array<bool> shouldSubdiv;
        std::vector<v3s>   deleteAfter;
        shouldSubdiv.allocate((uint)voxels[depth-1].size());
        if (depth < mind) {
          shouldSubdiv.fill(true);
        } else {
          shouldSubdiv.fill(false);
          uint v = 0;
          ForIterator_typename(t_VoxelMap,voxels[depth-1],V) {
            if ((*V).second.shouldSubdivide()) {
              shouldSubdiv[v] = true;
            }
            v ++;
          }
        }
        // flag for deletion after subdiv
        uint v = 0;
        ForIterator_typename(t_VoxelMap,voxels[depth-1],V) {
          if (shouldSubdiv[v]) {
            if ((*V).second.shouldDeleteAfterSubdivide()) {
              deleteAfter.push_back((*V).first);
            }
          }
          v++;
        }
        // subdivide
        v = 0;
        ForIterator_typename(t_VoxelMap,voxels[depth-1],V) {
          // Move this here, as this is constant for the whole loop
          //const float depth_grid_sz = static_cast<float> (1 << depth);
          //const v3f   step          = v3f (1.0f) / depth_grid_sz;

          if (shouldSubdiv[v]) {
            // for each potential child
            ForTuple_int(3,cpos,2) {
              if (T_Verbose) {
                static System::Time::Every every(50);
                if (every.expired()) {
                  Console::processingUpdate();
                  std::cerr << sprint(" %d voxels.",voxels.back().size());
                }
              }

              // compute child boxes
              const Tuple<float,3> extent = V->second.box().maxCorner() - V->second.box().minCorner();
              const Tuple<float,3> corner = V->second.box().minCorner() + Tuple<float,3>(cpos)*0.5f*extent;
              T_Voxel vx					= t_FactoryWrapper::Create(m_Factory);
              vx.depth()					= depth;

              // voxel grid pos
              const v3s vpos			= V->first*short(2) + v3s(cpos);
              vx.pos()						= vpos;
              //const v3f boxOrigin = v3f (vpos) / depth_grid_sz;
              vx.box()						= AAB<3> (corner, corner + 0.5f * extent);

              bool collides				= false;

              // test against parent voxel triangles
              ForIndex(t,V->second.triangles().size())
              {
                Tuple<v3f,3> tri;

                ForIndex (p,3) {
                  tri[p] = mesh->posAt(mesh->triangleAt(V->second.triangles()[t])[p]);
                }
                if (Triangle_AABox(tri[0],tri[1],tri[2],vx.box())) {
                  collides = true;
                  vx.triangles().push_back(V->second.triangles()[t]);
                }
              }
              // collides? then add to current depth
              if (vx.shouldKeepChild()) {
                voxels[depth].insert(std::make_pair(vpos,vx));
                numcolliding ++;
              }
            } // cpos
          } // shouldSubdiv[v]
          ++ v;
        } // v
        // clean up
        ForIterator(std::vector<v3s>,deleteAfter,D) {
          voxels[depth-1].erase((*D));
        }
        deleteAfter .clear();
        shouldSubdiv.erase();
        // done at this level
        if (T_Verbose) {
          Console::processingEnd();
          std::cerr << sprint("[voxelizer] level %d, %d voxels\n",depth,numcolliding);
        }
        // go down
        if (numcolliding > 0) {
          voxelize_rec(mesh,voxels,depth+1,mind,maxd);
        }
        //LIBSL_END;
      }

    public:

      void voxelize(
        const LibSL::Mesh::TriangleMesh_Ptr  mesh,
        const uint                           vminres,
        const uint                           vmaxres,
        bool                                 keep_aspect_ratio,
        std::vector<t_VoxelMap>&            _voxels) const
      {
        uint res = vmaxres;
        uint maxd = 0;
        while ((res>>=1) > 0) {
          maxd ++;
        }
        res       = vminres;
        uint mind = 0;
        while ((res>>=1) > 0) {
          mind ++;
        }

        _voxels.push_back(t_VoxelMap());
        T_Voxel vx   = t_FactoryWrapper::Create(m_Factory);

        vx.depth()   = 0;
        if ( ! keep_aspect_ratio ) {
          vx.box()     = mesh->bbox();
        } else {
          v3f exts     = mesh->bbox().extent();
          vx.box().addPoint( mesh->bbox().minCorner());
          vx.box().addPoint( mesh->bbox().minCorner() + v3f(tupleMax( exts )) );
        }

        v3f eps = vx.box().extent() / (float)(1<<vmaxres);
        vx.box().enlarge( eps );
        vx.box().minCorner() -= eps/2.0f;
        vx.pos()	 = V3S(0,0,0);
        ForIndex(t,mesh->numTriangles()) {
          vx.triangles().push_back(t);
        }
        _voxels[0][v3s(Triple(0,0,0))] = vx;
        if (T_Verbose) { std::cerr << sprint("[voxelizer] %d triangles, min depth %d, max depth %d.\n",_voxels[0][v3s(Triple(0,0,0))].triangles().size(),mind,maxd); }
        voxelize_rec(mesh,_voxels,1,mind,maxd);
      }

    };

  } // namespace LibSL::Geometry
} // namespace LibSL
