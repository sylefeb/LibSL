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
// LibSL_d3d - main include file for Direct3D apps
// ------------------------------------------------------
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-06-05
// ------------------------------------------------------

#pragma once

#define LIBSL_D3D10_INCLUDED
#define LIBSL_DX10_INCLUDED

#ifndef DIRECTX10
#define DIRECTX10
#endif

#ifdef OPENGL
ERROR_____either_OPENGL_or_DIRECTX10_must_be_defined__not_both _;
#endif

#ifdef DIRECT3D
ERROR_____either_DIRECT3D_or_DIRECTX10_must_be_defined__not_both _;
#endif

#ifndef LIBSL_CORE_INCLUDED
#include <LibSL/LibSL.h>
#endif

#include <windows.h>

#include <LibSL/GPUMesh/GPUMesh_dx10.h>
#include <LibSL/GPUTex/GPUTex_dx10.h>

//#include <LibSL/CgHelpers/CgHelpers.h>
//#include <LibSL/CgHelpers/CgImageProcessing.h>
//#include <LibSL/GPUHelpers/GPUFillArray2D.h>

#include <LibSL/D3DHelpers/D3DHelpers.h>
#include <LibSL/D3DHelpers/FxHelpers.h>
#include <LibSL/GPUHelpers/GPUHelpers.h>
#include <LibSL/GPUHelpers/Shapes.h>
#include <LibSL/GPUHelpers/Font.h>

#include <LibSL/UIHelpers/SimpleUI.h>
#include <LibSL/UIHelpers/TrackballUI.h>
#include <LibSL/UIHelpers/BindAntTweakBar.h>

#include <LibSL/Mesh/MeshRenderer.h>
#include <LibSL/Mesh/AnimatedMeshFxRenderer.h>

// #include <LibSL/Nature/Sky.h>

#ifndef LIBSL_SAFE_NAMESPACE

using namespace LibSL::GPUMesh;
using namespace LibSL::GPUTex;
//using namespace LibSL::CgHelpers;
using namespace LibSL::D3DHelpers;
using namespace LibSL::GPUHelpers;
// using namespace LibSL::Nature;
using namespace LibSL::UIHelpers;

#endif
