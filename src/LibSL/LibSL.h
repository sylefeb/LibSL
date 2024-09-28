/* --------------------------------------------------------------------
Author: Sylvain Lefebvre    sylvain.lefebvre@inria.fr

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
// LibSL - main include file
// ------------------------------------------------------
//
//
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-09
// ------------------------------------------------------

#pragma once
#define LIBSL_CORE_INCLUDED

// #pragma message("Including LibSL.h")

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <LibSL/LibSL.config.h>

#include <LibSL/Errors/Errors.h>
#include <LibSL/Memory/Pointer.h>

#include <LibSL/System/System.h>

#include <LibSL/CppHelpers/CppHelpers.h>
#include <LibSL/CppHelpers/BasicParser.h>
#include <LibSL/StlHelpers/StlHelpers.h>
#include <LibSL/SvgHelpers/SvgHelpers.h>

#include <LibSL/Memory/Array.h>
#include <LibSL/Memory/Array2D.h>
#include <LibSL/Memory/Array3D.h>
#include <LibSL/Memory/ArrayND.h>
#include <LibSL/Memory/ArrayTools.h>
#include <LibSL/Memory/ArrayRemap.h>

#include <LibSL/Image/Image.h>
#include <LibSL/Image/ImagePyramid.h>
#include <LibSL/Image/DistanceField.h>

#include <LibSL/Math/Math.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Vertex.h>
#include <LibSL/Math/Quaternion.h>
#include <LibSL/Math/Matrix4x4.h>
#include <LibSL/Math/Stats.h>
#include <LibSL/Math/Histogram.h>
#include <LibSL/Math/LloydClustering.h>

#include <LibSL/Mesh/Mesh.h>
#include <LibSL/Mesh/MeshEditing.h>
#include <LibSL/Mesh/AnimatedMesh.h>
#include <LibSL/Mesh/AnimatedMeshController.h>

#include <LibSL/Geometry/AAB.h>
#include <LibSL/Geometry/Plane.h>
#include <LibSL/Geometry/Polygon.h>
#include <LibSL/Geometry/Brush.h>
#include <LibSL/Geometry/BezierCurve.h>
#include <LibSL/Geometry/Bezier.h>
#include <LibSL/Geometry/Voxelizer.h>
#include <LibSL/Geometry/VoxelTools.h>
#include <LibSL/Geometry/ImplicitShape.h>
#include <LibSL/Geometry/Contour.h>
#include <LibSL/Geometry/Morpho.h>
#include <LibSL/Geometry/ConvexHull.h>
#include <LibSL/Geometry/PointTree.h>

#include <LibSL/Geometry/Intersections/Intersection_Plane_AABox.h>
#include <LibSL/Geometry/Intersections/Intersection_Polygon_AABox.h>
#include <LibSL/Geometry/Intersections/Intersection_Ray_AABox.h>
#include <LibSL/Geometry/Intersections/Intersection_Ray_Plane.h>
#include <LibSL/Geometry/Intersections/Intersection_Segment_Segment.h>
#include <LibSL/Geometry/Distances/Distance_Segment_Point.h>

#include <LibSL/DataStructures/Hierarchy.h>
#include <LibSL/DataStructures/Pow2Tree.h>
#include <LibSL/DataStructures/Graph.h>
#include <LibSL/DataStructures/GraphAlgorithms.h>
#include <LibSL/DataStructures/Pod.h>

#include <LibSL/TemplateHelpers/TemplateHelpers.h>

#include <LibSL/Win32Helpers/Win32Helpers.h>
#include <LibSL/Win32Helpers/TrueType.h>

/// namespace declaration

#ifndef LIBSL_SAFE_NAMESPACE

// using namespace LibSL;
using namespace LibSL::Errors;
using namespace LibSL::System::Types;
using namespace LibSL::System::Time;
using namespace LibSL::System::File;
// using namespace LibSL::System;       // DO NOT include System due to collisions with X11
using namespace LibSL::CppHelpers;
using namespace LibSL::Memory::Array;
using namespace LibSL::Memory::Pointer;
using namespace LibSL::Image;
using namespace LibSL::Math;
using namespace LibSL::Mesh;
using namespace LibSL::StlHelpers;
using namespace LibSL::DataStructures;
using namespace LibSL::Geometry;
using namespace LibSL::Geometry::Intersections;
using namespace LibSL::Filter;
using namespace LibSL::TemplateHelpers;

#endif

/// fixup for VisualC++ global object - singleton registration

//#if defined(_WIN32) || defined(_WIN64)

#include <LibSL/Image/ImageFormat_PNG.h>
#include <LibSL/Image/ImageFormat_JPG.h>
#include <LibSL/Image/ImageFormat_TGA.h>
#include <LibSL/Image/ImageFormat_float.h>
#include <LibSL/Image/ImageFormat_pfm.h>
#include <LibSL/Mesh/MeshFormat_3DS.h>
#include <LibSL/Mesh/MeshFormat_mesh.h>
#include <LibSL/Mesh/MeshFormat_OBJ.h>
#include <LibSL/Mesh/MeshFormat_wrl.h>
#include <LibSL/Mesh/MeshFormat_map.h>
#include <LibSL/Mesh/MeshFormat_proc.h>
#include <LibSL/Mesh/MeshFormat_dae.h>
#include <LibSL/Mesh/MeshFormat_stl.h>
#include <LibSL/Mesh/MeshFormat_off.h>
#include <LibSL/Mesh/MeshFormat_ply.h>

//#endif

#ifdef LIBSL_LIMITED //// TODO: get rid of this after CMAKE port

#define LIBSL_WIN32_FIX                  \
LibSL::Image::ImageFormat_PNG   s_PNG;   \
LibSL::Image::ImageFormat_float s_Float; \
LibSL::Image::ImageFormat_pfm   s_Pfm;   \
LibSL::Image::ImageFormat_TGA   s_TGA;   \
LibSL::Mesh::MeshFormat_mesh    s_mesh;  \
LibSL::Mesh::MeshFormat_OBJ     s_Obj;   \
LibSL::Mesh::MeshFormat_wrl     s_Wrl;   \
LibSL::Mesh::MeshFormat_map     s_Map;   \
LibSL::Mesh::MeshFormat_proc    s_Proc;  \
LibSL::Mesh::MeshFormat_dae     s_Dae;   \
LibSL::Mesh::MeshFormat_stl     s_Stl;   \
LibSL::Mesh::MeshFormat_ply     s_Ply;   \
LibSL::Mesh::MeshFormat_off     s_Off;

#define LIBSL_FILE_FORMATS LIBSL_WIN32_FIX

#else

#define LIBSL_WIN32_FIX                  \
LibSL::Image::ImageFormat_PNG   s_PNG;   \
LibSL::Image::ImageFormat_float s_Float; \
LibSL::Image::ImageFormat_pfm   s_Pfm;   \
LibSL::Image::ImageFormat_TGA   s_TGA;   \
LibSL::Image::ImageFormat_JPG   s_JPG;   \
LibSL::Mesh::MeshFormat_mesh    s_mesh;  \
LibSL::Mesh::MeshFormat_OBJ     s_Obj;   \
LibSL::Mesh::MeshFormat_wrl     s_Wrl;   \
LibSL::Mesh::MeshFormat_map     s_Map;   \
LibSL::Mesh::MeshFormat_3DS     s_3DS;   \
LibSL::Mesh::MeshFormat_proc    s_Proc;  \
LibSL::Mesh::MeshFormat_dae     s_Dae;   \
LibSL::Mesh::MeshFormat_ply     s_Ply;   \
LibSL::Mesh::MeshFormat_stl     s_Stl;   \
LibSL::Mesh::MeshFormat_off     s_Off;

#define LIBSL_FILE_FORMATS LIBSL_WIN32_FIX

#endif

// ------------------------------------------------------
