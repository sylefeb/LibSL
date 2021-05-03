PROJECT(LIBSL)

###############################

SET(LIBSL_CORE_HEADERS
	./LibSL.common.h
	./LibSL.h
	./LibSL.precompiled.h
	CppHelpers/BasicParser.h
	CppHelpers/CppHelpers.h
	DataStructures/Graph.h
	DataStructures/GraphAlgorithms.h
	DataStructures/Hierarchy.h
	DataStructures/Pod.h
        DataStructures/Pow2Tree.h
	Errors/Errors.h
	Geometry/AAB.h
	Geometry/Bezier.h
	Geometry/BezierCurve.h
	Geometry/Brush.h
	Geometry/ConvexHull.h
	Geometry/ImplicitShape.h
	Geometry/LookUpTable.h
	Geometry/MarchingCubes.h
	Geometry/Plane.h
	Geometry/Polygon.h
	Geometry/Voxelizer.h
	Geometry/VoxelTools.h
	Geometry/Contour.h
	Geometry/Morpho.h
	Geometry/PointTree.h
	Geometry/Components.h
	Geometry/Distances/Distance_Segment_Point.h
	Geometry/Intersections/Intersection_Plane_AABox.h
	Geometry/Intersections/Intersection_Polygon_AABox.h
	Geometry/Intersections/Intersection_Ray_AABox.h
	Geometry/Intersections/Intersection_Ray_Plane.h
	Geometry/Intersections/Intersection_Segment_Segment.h
	Image/Filter.h
	Image/Image.h
	Image/ImageFormat_dds.h
	Image/ImageFormat_float.h
	Image/ImageFormat_half.h
	Image/ImageFormat_JPG.h
	Image/ImageFormat_pfm.h
	Image/ImageFormat_PNG.h
	Image/ImageFormat_TGA.h
	Image/ImagePyramid.h
	Image/tga.h
	Image/DistanceField.h
	Math/Frame.h
	Math/Histogram.h
	Math/LBGClustering.h
	Math/LloydClustering.h
	Math/LloydClusteringOnQuantized.h
	Math/Math.h
	Math/Matrix4x4.h
	Math/Quaternion.h
	Math/Stats.h
	Math/Tuple.h
	Math/Vertex.h
	Memory/Array.h
	Memory/Array2D.h
	Memory/Array3D.h
	Memory/ArrayND.h
	Memory/ArrayRemap.h
	Memory/Cache.h
	Memory/Pointer.h
	Memory/TraceLeaks.h
	Mesh/AnimatedMesh.h
	Mesh/AnimatedMeshGLSLRenderer.h
	Mesh/AnimatedMeshController.h
	Mesh/AnimatedMeshFxRenderer.h
	Mesh/Mesh.h
	Mesh/MeshEditing.h
	Mesh/MeshFormat_3DS.h
	Mesh/MeshFormat_map.h
	Mesh/MeshFormat_mesh.h
	Mesh/MeshFormat_OBJ.h
	Mesh/MeshFormat_wrl.h
	Mesh/MeshFormat_proc.h
	Mesh/MeshFormat_dae.h
	Mesh/MeshFormat_stl.h
	Mesh/MeshFormat_off.h
	Mesh/MeshFormat_ply.h
	Mesh/MeshRenderer.h
	Mesh/TexturedMeshRenderer.h
	Mesh/VertexFormat.h
	Mesh/VertexFormat_base.h
	Mesh/VertexFormat_descriptor.h
	Mesh/VertexFormat_dynamic.h
	Mesh/VertexFormat_reference.h
  Mesh/AnimatedMeshFormat_animesh.h
	Shaders/Nature/Sky.h
	StlHelpers/StlHelpers.h
	ArrayHelpers/ArrayHelpers.h
	SvgHelpers/SvgHelpers.h
	System/eLut.h
	System/half.h
	System/System.h
	System/toFloat.h
	System/Types.h
	TemplateHelpers/ExecuteOnTypeList.h
	TemplateHelpers/TemplateHelpers.h
	UIHelpers/Manipulator.h
	UIHelpers/SimpleUI.h
	UIHelpers/StyleManager.h
	UIHelpers/Trackball.h
	UIHelpers/TrackballUI.h
	UIHelpers/BindImGui.h
	Win32Helpers/TrueType.h
	Win32Helpers/Win32Helpers.h
	)

SET(LIBSL_CORE_SOURCE
#	./LibSL.cpp
	StlHelpers/StlHelpers.cpp
	ArrayHelpers/ArrayHelpers.cpp
	Memory/TraceLeaks.cpp
	Memory/ArrayRemap.cpp
	Image/tga.cpp
	Image/Image.cpp
	Image/ImageFormat_PNG.cpp
	Image/ImageFormat_JPG.cpp
	Image/ImageFormat_TGA.cpp
	Image/ImageFormat_float.cpp
	Image/ImageFormat_pfm.cpp
	Image/DistanceField.cpp
	Math/Vertex.cpp
	System/System.cpp
	CppHelpers/CppHelpers.cpp
	SvgHelpers/SvgHelpers.cpp
	Math/Math.cpp
	Mesh/Mesh.cpp
	Mesh/MeshFormat_3DS.cpp
	Mesh/MeshFormat_OBJ.cpp
	Mesh/MeshFormat_wrl.cpp
	Mesh/MeshFormat_mesh.cpp
	Mesh/MeshFormat_map.cpp
	Mesh/MeshFormat_proc.cpp
	Mesh/MeshFormat_dae.cpp
	Mesh/MeshFormat_stl.cpp
	Mesh/MeshFormat_off.cpp
	Mesh/MeshFormat_ply.cpp
	Mesh/VertexFormat_dynamic.cpp
	Mesh/AnimatedMesh.cpp
	Mesh/AnimatedMeshController.cpp
  Mesh/AnimatedMeshFormat_animesh.cpp
	Geometry/Intersections/Intersection_Plane_AABox.cpp
	Geometry/Intersections/Intersection_Polygon_AABox.cpp
	Geometry/Intersections/Intersection_Ray_Plane.cpp
	Geometry/Intersections/Intersection_Segment_Segment.cpp
	Geometry/Intersections/triangle_aabox.cpp
	Geometry/Intersections/triangle_ray.cpp
	Geometry/Distances/Distance_Segment_Point.cpp
	Geometry/ImplicitShape.cpp
	Geometry/MarchingCubes.cpp
#	Geometry/Voxelizer.cpp
	Geometry/Contour.cpp
	Geometry/Components.cpp
	Geometry/ConvexHull.cpp
	Geometry/Morpho.cpp
  Geometry/PointTree.cpp
	System/half.cpp
	../libs/src/SQLite/sqlite3.c
	../libs/src/rply/rply.c
	)

CreateSourceGroups("Header files" src "${LIBSL_CORE_HEADERS}")
CreateSourceGroups("Source files" src "${LIBSL_CORE_SOURCE}")

###############################

SET(LIBSL_GL_HEADERS
	./LibSL_gl.h
	Mesh/AnimatedMeshGLSLRenderer.h
	GLHelpers/GLHelpers.h
  GLHelpers/GLBasicPipeline.h
	UIHelpers/BindAntTweakBar.h
 	UIHelpers/BindImGui.h
	UIHelpers/StyleManager.h
  ../libs/src/imgui/imgui.h
  ../libs/src/imgui/misc/cpp/imgui_stdlib.h
  )

SET(LIBSL_GL_SOURCE
	GLHelpers/GLHelpers.cpp
	GLHelpers/AutoBindShader.cpp
	GLHelpers/GLBasicPipeline.cpp
	GLHelpers/Triangulate.cpp
	GPUHelpers/Font.cpp
	GPUHelpers/GPUHelpers.cpp
	GPUHelpers/Profiler.cpp
	GPUHelpers/Shapes.cpp
	Mesh/AnimatedMeshGLSLRenderer.cpp
	UIHelpers/Manipulator.cpp
	UIHelpers/SimpleUI.cpp
  UIHelpers/TrackballUI.cpp
	UIHelpers/Trackball.cpp
	UIHelpers/BindAntTweakBar.cpp
	UIHelpers/BindImGui.cpp
	UIHelpers/StyleManager.cpp
  ../libs/src/imgui/imgui.cpp
  ../libs/src/imgui/misc/cpp/imgui_stdlib.cpp
  ../libs/src/imgui/imgui_draw.cpp
  ../libs/src/imgui/imgui_widgets.cpp
  ../libs/src/imgui/imgui_tables.cpp
)

SET(LIBSL_GL4_HEADERS
	./LibSL_gl4.h
	Mesh/AnimatedMeshGLSLRenderer.h
	GLHelpers/GLHelpers.h
  GLHelpers/GLBasicPipeline.h
	UIHelpers/BindAntTweakBar.h
 	UIHelpers/BindImGui.h
	UIHelpers/StyleManager.h
  ../libs/src/imgui/imgui.h
  ../libs/src/imgui/misc/cpp/imgui_stdlib.h
  )

SET(LIBSL_GL4_SOURCE
	GLHelpers/GLHelpers.cpp
	GLHelpers/AutoBindShader.cpp
  GLHelpers/GLBasicPipeline.cpp
	GLHelpers/Triangulate.cpp
	GPUHelpers/Font.cpp
	GPUHelpers/GPUHelpers.cpp
	GPUHelpers/Profiler.cpp
	GPUHelpers/Shapes.cpp
	Mesh/AnimatedMeshGLSLRenderer.cpp
	UIHelpers/Manipulator.cpp
	UIHelpers/SimpleUI.cpp
	UIHelpers/TrackballUI.cpp
	UIHelpers/Trackball.cpp
	UIHelpers/StyleManager.cpp
	UIHelpers/BindAntTweakBar.cpp
 	UIHelpers/BindImGui.cpp
  ../libs/src/imgui/imgui.cpp
  ../libs/src/imgui/misc/cpp/imgui_stdlib.cpp
  ../libs/src/imgui/imgui_draw.cpp
  ../libs/src/imgui/imgui_widgets.cpp
  ../libs/src/imgui/imgui_tables.cpp
)

SET(LIBSL_GL4CORE_HEADERS
	./LibSL_gl4core.h
	GLHelpers/GLHelpers.h
  GLHelpers/GLBasicPipeline.h
	UIHelpers/BindAntTweakBar.h
 	UIHelpers/BindImGui.h
	UIHelpers/StyleManager.h
  ../libs/src/imgui/imgui.h
  ../libs/src/imgui/misc/cpp/imgui_stdlib.h
  )

SET(LIBSL_GL4CORE_SOURCE
	GLHelpers/GLHelpers.cpp
	GLHelpers/AutoBindShader.cpp
  GLHelpers/GLBasicPipeline.cpp
	GLHelpers/Triangulate.cpp
	GPUHelpers/Font.cpp
	GPUHelpers/GPUHelpers.cpp
	GPUHelpers/Profiler.cpp
	GPUHelpers/Shapes.cpp
	UIHelpers/Manipulator.cpp
	UIHelpers/SimpleUI.cpp
	UIHelpers/StyleManager.cpp
	UIHelpers/TrackballUI.cpp
	UIHelpers/Trackball.cpp
	UIHelpers/BindAntTweakBar.cpp
 	UIHelpers/BindImGui.cpp
  ../libs/src/imgui/imgui.cpp
  ../libs/src/imgui/misc/cpp/imgui_stdlib.cpp
  ../libs/src/imgui/imgui_draw.cpp
  ../libs/src/imgui/imgui_widgets.cpp
  ../libs/src/imgui/imgui_tables.cpp
)

CreateSourceGroups("Header files" src "${LIBSL_GL_HEADERS}")
CreateSourceGroups("Source files" src "${LIBSL_GL_SOURCE}")

CreateSourceGroups("Header files" src "${LIBSL_GL4_HEADERS}")
CreateSourceGroups("Source files" src "${LIBSL_GL4_SOURCE}")

CreateSourceGroups("Header files" src "${LIBSL_GL4CORE_HEADERS}")
CreateSourceGroups("Source files" src "${LIBSL_GL4CORE_SOURCE}")

###############################

SET(LIBSL_DX9_HEADERS
	LibSL_d3d.h
	D3DHelpers/D3DHelpers.h
	D3DHelpers/FxImageProcessing.h
	D3DHelpers/D3DQuad.h
	D3DHelpers/FxHelpers.h
	D3DHelpers/FxHelpers_dx9_policy.h
	GPUHelpers/Font.h
	GPUHelpers/GPUHelpers.h
	GPUHelpers/GPUHelpers_d3d.h
	GPUHelpers/Profiler.h
	GPUHelpers/Shapes.h
	Image/ImageFormat_dds.h
	Mesh/AnimatedMeshFxRenderer.h
	Mesh/MeshRenderer.h
	Mesh/TexturedMeshRenderer.h
	UIHelpers/Manipulator.h
	UIHelpers/SimpleUI.h
	UIHelpers/TrackballUI.h
	UIHelpers/Trackball.h
)

SET(LIBSL_DX9_SOURCE
	D3DHelpers/D3DHelpers.cpp
	D3DHelpers/FxImageProcessing.cpp
	D3DHelpers/D3DQuad.cpp
	GPUHelpers/Font.cpp
	GPUHelpers/GPUHelpers.cpp
	GPUHelpers/Profiler.cpp
	GPUHelpers/Shapes.cpp
	Image/ImageFormat_dds.cpp
	Mesh/AnimatedMeshFxRenderer.cpp
	UIHelpers/Manipulator.cpp
	UIHelpers/SimpleUI.cpp
	UIHelpers/TrackballUI.cpp
	UIHelpers/Trackball.cpp
	UIHelpers/BindAntTweakBar.cpp
)

CreateSourceGroups("Header files" src "${LIBSL_DX9_HEADERS}")
CreateSourceGroups("Source files" src "${LIBSL_DX9_SOURCE}")

###############################

SET(LIBSL_DX10_HEADERS
	LibSL_dx10.h
	D3DHelpers/D3DHelpers.h
	D3DHelpers/FxImageProcessing.h
	D3DHelpers/D3DQuad.h
	D3DHelpers/FxHelpers.h
	D3DHelpers/FxHelpers_dx10_policy.h
	GPUHelpers/Font.h
	GPUHelpers/GPUHelpers.h
	GPUHelpers/GPUHelpers_d3d.h
	GPUHelpers/Profiler.h
	GPUHelpers/Shapes.h
	Image/ImageFormat_dds.h
	Mesh/AnimatedMeshFxRenderer.h
	Mesh/MeshRenderer.h
	Mesh/TexturedMeshRenderer.h
	UIHelpers/Manipulator.h
	UIHelpers/SimpleUI.h
	UIHelpers/TrackballUI.h
	UIHelpers/Trackball.h
)

SET(LIBSL_DX10_SOURCE
	D3DHelpers/D3DHelpers.cpp
	D3DHelpers/FxImageProcessing.cpp
	D3DHelpers/D3DQuad.cpp
	GPUHelpers/Font.cpp
	GPUHelpers/GPUHelpers.cpp
	GPUHelpers/Profiler.cpp
	GPUHelpers/Shapes.cpp
	Image/ImageFormat_dds.cpp
	Mesh/AnimatedMeshFxRenderer.cpp
	UIHelpers/Manipulator.cpp
	UIHelpers/SimpleUI.cpp
	UIHelpers/TrackballUI.cpp
	UIHelpers/Trackball.cpp
	UIHelpers/BindAntTweakBar.cpp
)

CreateSourceGroups("Header files" src "${LIBSL_DX10_HEADERS}")
CreateSourceGroups("Source files" src "${LIBSL_DX10_SOURCE}")

###############################

# IF    (WIN32)
# ADD_DEFINITIONS("-DHAS_TAUCS")
# ENDIF (WIN32)

SET(LIBSL_LINALG_HEADERS
	./LibSL_linalg.h
	LinAlg/LinearSolver.h
	LinAlg/PCA.h
	LinAlg/SparseMatrix.h
	Image/PushPull.h
)

SET(LIBSL_LINALG_SOURCE
	LinAlg/PCA.cpp
)

CreateSourceGroups("Header files" src "${LIBSL_LINALG_HEADERS}")
CreateSourceGroups("Source files" src "${LIBSL_LINALG_SOURCE}")

###############################

ADD_LIBRARY(LibSL STATIC ${LIBSL_CORE_SOURCE} ${LIBSL_CORE_HEADERS})
SET_TARGET_PROPERTIES(LibSL PROPERTIES DEBUG_POSTFIX "-d")
SET_PROPERTY(TARGET LibSL APPEND PROPERTY
   INTERFACE_INCLUDE_DIRECTORIES
 			${CMAKE_CURRENT_SOURCE_DIR}/..
			${CMAKE_CURRENT_SOURCE_DIR}/../LibSL
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/src
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/loki/include
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/tclap/include
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/glux/src
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/png
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/jpeg
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/zlib
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/newmat
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/tinyxml
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/anttweakbar/include
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/OpenNL/src
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/bin/win32/TAUCS/include
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/SQLite/
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/hashlibpp/src/
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/rply/
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/imgui/
			${CMAKE_CURRENT_SOURCE_DIR}/../libs/src/qhull/src/libqhull_r/
			$ENV{DXSDK_DIR}/include
			$ENV{DXSDK_DIR}/lib/x86
			$ENV{DXSDK_DIR}/Samples/C++/DXUT/Core)

ADD_DEFINITIONS(-D_UNICODE -DUNICODE)
ADD_DEFINITIONS(-DTW_STATIC -DTW_NO_LIB_PRAGMA)

# Compile LibSL with CXX11 support if available (ConvexHull with multithread support)
IF (EMSCRIPTEN)
	Include(UseCXX11)
	AddTargetCXX11(LibSL)
ELSE()
	IF(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
		INCLUDE(UseCXX11_clang)
	ELSE()
		Include(UseCXX11)
		AddTargetCXX11(LibSL)
	ENDIF()
ENDIF()

IF    (LIBSL_BUILD_MAX_PERFORMANCE)
ADD_DEFINITIONS("-DNDEBUG")
ENDIF (LIBSL_BUILD_MAX_PERFORMANCE)

IF (LIBSL_POWER_USER)
ADD_DEFINITIONS("-DLIBSL_IDKFK")
ENDIF (LIBSL_POWER_USER)

IF(WIN32)
#TARGET_LINK_LIBRARIES(LibSL jpeg png 3ds zlib loki qhull shlwapi psapi winmm tinyxml hashlibpp)
TARGET_LINK_LIBRARIES(LibSL jpeg png 3ds zlib qhull shlwapi psapi winmm tinyxml hashlibpp)
ELSE(WIN32)
#TARGET_LINK_LIBRARIES(LibSL jpeg png 3ds zlib loki qhull tinyxml hashlibpp)
TARGET_LINK_LIBRARIES(LibSL jpeg png 3ds zlib qhull tinyxml hashlibpp)
ENDIF(WIN32)

INSTALL(TARGETS LibSL
  RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
  LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib
  ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib
)

IF(NOT WIN32)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/libLibSL.a" DESTINATION ${CMAKE_SOURCE_DIR}/lib)
ENDIF(NOT WIN32)

###############################

#if (WIN32)
#    set_source_files_properties(LibSL.precompiled.cpp
#        PROPERTIES
#        COMPILE_FLAGS "/YcLibSL.precompiled.h"
#        )
#    foreach( src_file ${LIBSL_CORE_SOURCE} )
#        set_source_files_properties(
#            ${src_file}
#            PROPERTIES
#            COMPILE_FLAGS "/YuLibSL.precompiled.h"
#            )
#    endforeach( src_file ${LIBSL_CORE_SOURCE} )
#    list(APPEND LIBSL_CORE_SOURCE LibSL.precompiled.cpp)
#endif (WIN32)

IF(LIBSL_BUILD_GL4)
	IF(NOT LIBSL_BUILD_GL)
		message( FATAL_ERROR "LibSL for OpenGL 4.x requires to build the standard OpenGL version")
	ENDIF(NOT LIBSL_BUILD_GL)
ENDIF(LIBSL_BUILD_GL4)

IF(LIBSL_BUILD_GL4CORE)
	IF(NOT LIBSL_BUILD_GL)
		message( FATAL_ERROR "LibSL for OpenGL 4.x core profile requires to build the standard OpenGL version")
	ENDIF(NOT LIBSL_BUILD_GL)
ENDIF(LIBSL_BUILD_GL4CORE)

###############################

IF(LIBSL_BUILD_GL)

IF(NOT ANDROID)
	FIND_PACKAGE(OpenGL)
ELSEIF(NOT ANDROID)
	SET(OPENGL_LIBRARIES EGL GLESv2)
ENDIF(NOT ANDROID)

	ADD_LIBRARY(LibSL_gl STATIC ${LIBSL_GL_SOURCE} ${LIBSL_GL_HEADERS})

#  IF (NOT EMSCRIPTEN)
	IF (LIBSL_USE_GLUX)
	  ADD_DEFINITIONS(-DUSE_GLUX)
	  SET_TARGET_PROPERTIES(LibSL_gl PROPERTIES COMPILE_DEFINITIONS "USE_GLUX")
	  SET_PROPERTY(TARGET LibSL_gl APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS USE_GLUX)
	  IF(NOT WIN32)
	    # hack due to MESA headers including ARB_multitexture
	    ADD_DEFINITIONS(-DGL_ARB_multitexture=1)
	    SET_PROPERTY(TARGET LibSL_gl APPEND PROPERTY COMPILE_DEFINITIONS GL_ARB_multitexture=1)
	    SET_PROPERTY(TARGET LibSL_gl APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS GL_ARB_multitexture=1)
	  ENDIF(NOT WIN32)
	ENDIF (LIBSL_USE_GLUX)
#  ENDIF (NOT EMSCRIPTEN)

  SET_TARGET_PROPERTIES(LibSL_gl PROPERTIES DEBUG_POSTFIX "-d")
	IF(WIN32)
		SET_TARGET_PROPERTIES(LibSL_gl PROPERTIES COMPILE_DEFINITIONS "OPENGL;_WIN32_WINNT=0x0400;WINVER=0x500")
	ELSE(WIN32)
    IF (ANDROID)
      SET_TARGET_PROPERTIES(LibSL_gl PROPERTIES COMPILE_DEFINITIONS "OPENGL")
      TARGET_LINK_LIBRARIES(LibSL_gl ${OPENGL_LIBRARIES})
      TARGET_LINK_LIBRARIES(LibSL_gl ${GLUT_LIBRARIES})
      INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/libLibSL_gl.a" DESTINATION ${CMAKE_SOURCE_DIR}/lib)
    ELSE(ANDROID)
      FIND_PACKAGE(OpenGL)
      IF(LIBSL_USE_GLFW)
        #FIND_PACKAGE(glfw3)
        SET_TARGET_PROPERTIES(LibSL_gl PROPERTIES COMPILE_DEFINITIONS "OPENGL;USE_GLFW")
        TARGET_LINK_LIBRARIES(LibSL_gl ${OPENGL_LIBRARIES})
        TARGET_LINK_LIBRARIES(LibSL_gl glfw)
      ELSE(LIBSL_USE_GLFW)
        FIND_PACKAGE(GLUT)
        SET_TARGET_PROPERTIES(LibSL_gl PROPERTIES COMPILE_DEFINITIONS "OPENGL;USE_GLUT")
        TARGET_LINK_LIBRARIES(LibSL_gl ${OPENGL_LIBRARIES})
        TARGET_LINK_LIBRARIES(LibSL_gl ${GLUT_LIBRARIES})
      ENDIF(LIBSL_USE_GLFW)
      INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/libLibSL_gl.a" DESTINATION ${CMAKE_SOURCE_DIR}/lib)
    ENDIF(ANDROID)
	ENDIF(WIN32)
	IF(APPLE)
		ADD_DEFINITIONS(-DGL_GLEXT_LEGACY)
	ENDIF(APPLE)
	SET_PROPERTY(TARGET LibSL_gl APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS TW_STATIC TW_NO_LIB_PRAGMA)

  IF (LIBSL_USE_GLUX)
  IF (ANDROID)
	TARGET_LINK_LIBRARIES(LibSL_gl LibSL)
  ELSE(ANDROID)
	TARGET_LINK_LIBRARIES(LibSL_gl LibSL glux)
  ENDIF(ANDROID)
  ELSE(LIBSL_USE_GLUX)
	TARGET_LINK_LIBRARIES(LibSL_gl LibSL)
  ENDIF(LIBSL_USE_GLUX)
  
	INSTALL(TARGETS LibSL_gl
		RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
		LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib
		ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib
	)

  #####
  
IF (LIBSL_BUILD_GL4)
	ADD_LIBRARY(LibSL_gl4 STATIC ${LIBSL_GL4_SOURCE} ${LIBSL_GL4_HEADERS})
	SET_TARGET_PROPERTIES(LibSL_gl4 PROPERTIES DEBUG_POSTFIX "-d")

	IF (LIBSL_USE_GLUX)
	  ADD_DEFINITIONS(-DUSE_GLUX)
	  SET_TARGET_PROPERTIES(LibSL_gl4 PROPERTIES COMPILE_DEFINITIONS "USE_GLUX")
	  SET_PROPERTY(TARGET LibSL_gl4 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS USE_GLUX)
	  IF(NOT WIN32)
	    # hack due to MESA headers including ARB_multitexture
	    ADD_DEFINITIONS(-DGL_ARB_multitexture=1)
	    SET_PROPERTY(TARGET LibSL_gl4 APPEND PROPERTY COMPILE_DEFINITIONS GL_ARB_multitexture=1)
	    SET_PROPERTY(TARGET LibSL_gl4 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS GL_ARB_multitexture=1)
	  ENDIF(NOT WIN32)
	ENDIF (LIBSL_USE_GLUX)

	IF(WIN32)
		SET_TARGET_PROPERTIES(LibSL_gl4 PROPERTIES COMPILE_DEFINITIONS "OPENGL;OPENGL4;_WIN32_WINNT=0x0400;WINVER=0x500")
	ELSE(WIN32)
		FIND_PACKAGE(OpenGL)
    IF(LIBSL_USE_GLFW)
      FIND_PACKAGE(glfw3 REQUIRED)
      SET_TARGET_PROPERTIES(LibSL_gl4 PROPERTIES COMPILE_DEFINITIONS "OPENGL;OPENGL4;USE_GLFW")
      TARGET_LINK_LIBRARIES(LibSL_gl4 ${OPENGL_LIBRARIES})
      TARGET_LINK_LIBRARIES(LibSL_gl4 glfw) #${GLFW3_LIBRARIES}
    ELSE(LIBSL_USE_GLFW)
      FIND_PACKAGE(GLUT)
      SET_TARGET_PROPERTIES(LibSL_gl4 PROPERTIES COMPILE_DEFINITIONS "OPENGL;OPENGL4;USE_GLUT")
      TARGET_LINK_LIBRARIES(LibSL_gl4 ${OPENGL_LIBRARIES})
      TARGET_LINK_LIBRARIES(LibSL_gl4 ${GLUT_LIBRARIES})
    ENDIF(LIBSL_USE_GLFW)
		INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/libLibSL_gl4.a" DESTINATION ${CMAKE_SOURCE_DIR}/lib)
	ENDIF(WIN32)
	IF(APPLE)
		ADD_DEFINITIONS(-DGL_GLEXT_LEGACY)
	ENDIF(APPLE)
	SET_PROPERTY(TARGET LibSL_gl4 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS TW_STATIC TW_NO_LIB_PRAGMA)

  IF (LIBSL_USE_GLUX)
	TARGET_LINK_LIBRARIES(LibSL_gl4 LibSL glux)
  ELSE(LIBSL_USE_GLUX)
	TARGET_LINK_LIBRARIES(LibSL_gl4 LibSL)
  ENDIF(LIBSL_USE_GLUX)

	INSTALL(TARGETS LibSL_gl4
		RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
		LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib
		ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib
	)
ENDIF(LIBSL_BUILD_GL4)

  #####
  
IF (LIBSL_BUILD_GL4CORE)
	ADD_LIBRARY(LibSL_gl4core STATIC ${LIBSL_GL4CORE_SOURCE} ${LIBSL_GL4CORE_HEADERS})
	SET_TARGET_PROPERTIES(LibSL_gl4core PROPERTIES DEBUG_POSTFIX "-d")

	IF (LIBSL_USE_GLUX)
	  ADD_DEFINITIONS(-DUSE_GLUX)
	  SET_TARGET_PROPERTIES(LibSL_gl4core PROPERTIES COMPILE_DEFINITIONS "USE_GLUX")
	  SET_PROPERTY(TARGET LibSL_gl4core APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS USE_GLUX)
	  IF(NOT WIN32)
	    # hack due to MESA headers including ARB_multitexture
	    ADD_DEFINITIONS(-DGL_ARB_multitexture=1)
	    SET_PROPERTY(TARGET LibSL_gl4core APPEND PROPERTY COMPILE_DEFINITIONS GL_ARB_multitexture=1)
	    SET_PROPERTY(TARGET LibSL_gl4core APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS GL_ARB_multitexture=1)
	  ENDIF(NOT WIN32)
	ENDIF (LIBSL_USE_GLUX)

	IF(WIN32)
		SET_TARGET_PROPERTIES(LibSL_gl4core PROPERTIES COMPILE_DEFINITIONS "OPENGL;OPENGL4;OPENGLCORE;_WIN32_WINNT=0x0400;WINVER=0x500")
	ELSE(WIN32)
		FIND_PACKAGE(OpenGL)
                IF(LIBSL_USE_GLFW)
                    FIND_PACKAGE(glfw3 REQUIRED)
                    SET_TARGET_PROPERTIES(LibSL_gl4core PROPERTIES COMPILE_DEFINITIONS "OPENGL;OPENGL4;OPENGLCORE;USE_GLFW")
                    TARGET_LINK_LIBRARIES(LibSL_gl4core ${OPENGL_LIBRARIES})
                    TARGET_LINK_LIBRARIES(LibSL_gl4core glfw) #${GLFW3_LIBRARIES}
                ELSE(LIBSL_USE_GLFW)
                    FIND_PACKAGE(GLUT)
                    SET_TARGET_PROPERTIES(LibSL_gl4core PROPERTIES COMPILE_DEFINITIONS "OPENGL;OPENGL4;OPENGLCORE;USE_GLUT")
                    TARGET_LINK_LIBRARIES(LibSL_gl4core ${OPENGL_LIBRARIES})
                    TARGET_LINK_LIBRARIES(LibSL_gl4core ${GLUT_LIBRARIES})
                ENDIF(LIBSL_USE_GLFW)
                INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/libLibSL_gl4core.a" DESTINATION ${CMAKE_SOURCE_DIR}/lib)
	ENDIF(WIN32)
	IF(APPLE)
		ADD_DEFINITIONS(-DGL_GLEXT_LEGACY)
	ENDIF(APPLE)
	SET_PROPERTY(TARGET LibSL_gl4core APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS TW_STATIC TW_NO_LIB_PRAGMA)

	TARGET_LINK_LIBRARIES(LibSL_gl4core LibSL glux)
	INSTALL(TARGETS LibSL_gl4core
		RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
		LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib
		ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib
	)
ENDIF(LIBSL_BUILD_GL4CORE)


ENDIF(LIBSL_BUILD_GL)

###############################

IF(LIBSL_BUILD_DX9)
	INCLUDE_DIRECTORIES($ENV{DXSDK_DIR}/Samples/C++/DXUT/Core)
	INCLUDE_DIRECTORIES($ENV{DXSDK_DIR}/Include)
	ADD_LIBRARY(LibSL_d3d STATIC ${LIBSL_DX9_SOURCE} ${LIBSL_DX9_HEADERS})
	SET_TARGET_PROPERTIES(LibSL_d3d PROPERTIES COMPILE_FLAGS -DDIRECT3D)
	SET_TARGET_PROPERTIES(LibSL_d3d PROPERTIES DEBUG_POSTFIX "-d")
	TARGET_LINK_LIBRARIES(LibSL_d3d LibSL)
	INSTALL(TARGETS LibSL_d3d
		RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
		LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib
		ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib
	)
ENDIF(LIBSL_BUILD_DX9)

###############################

IF(LIBSL_BUILD_DX10)
	INCLUDE_DIRECTORIES($ENV{DXSDK_DIR}/Samples/C++/DXUT/Core)
	INCLUDE_DIRECTORIES($ENV{DXSDK_DIR}/Include)
	ADD_LIBRARY(LibSL_dx10 STATIC ${LIBSL_DX10_SOURCE} ${LIBSL_DX10_HEADERS})
	SET_TARGET_PROPERTIES(LibSL_dx10 PROPERTIES COMPILE_FLAGS -DDIRECTX10)
	SET_TARGET_PROPERTIES(LibSL_dx10 PROPERTIES DEBUG_POSTFIX "-d")
	TARGET_LINK_LIBRARIES(LibSL_dx10 LibSL)
	INSTALL(TARGETS LibSL_dx10
		RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
		LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib
		ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib
	)
ENDIF(LIBSL_BUILD_DX10)

###############################

IF(LIBSL_BUILD_LINALG)
#	IF(WIN32)
#		INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/libs/bin/win32/TAUCS/include)
#		SET(TAUCS_LIB_PATH ${CMAKE_SOURCE_DIR}/src/libs/bin/win32/TAUCS/lib)
#	ENDIF(WIN32)
	ADD_LIBRARY(LibSL_linalg STATIC ${LIBSL_LINALG_SOURCE} ${LIBSL_LINALG_HEADERS})
	SET_TARGET_PROPERTIES(LibSL_linalg PROPERTIES DEBUG_POSTFIX "-d")
	TARGET_LINK_LIBRARIES(LibSL_linalg LibSL)
	TARGET_LINK_LIBRARIES(LibSL_linalg nl newmat)
#	IF(WIN32)
#		TARGET_LINK_LIBRARIES(LibSL_linalg ${TAUCS_LIB_PATH}/libatlas.lib ${TAUCS_LIB_PATH}/libcblas.lib ${TAUCS_LIB_PATH}/libf77blas.lib ${TAUCS_LIB_PATH}/liblapack.lib ${TAUCS_LIB_PATH}/libmetis.lib ${TAUCS_LIB_PATH}/vcf2c.lib ${TAUCS_LIB_PATH}/libtaucs.lib)
#	ENDIF(WIN32)
	INSTALL(TARGETS LibSL_linalg
		RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
		LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib
		ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib
	)
ENDIF(LIBSL_BUILD_LINALG)

###############################

# various fixes
IF(APPLE)
SET(CMAKE_C_ARCHIVE_FINISH "<CMAKE_RANLIB> -c <TARGET>")
SET(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -c <TARGET>")
SET(CMAKE_Fortran_ARCHIVE_FINISH "<CMAKE_RANLIB> -c <TARGET>")
ENDIF(APPLE)

###############################
