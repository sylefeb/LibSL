# Check for Win64 first
# IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
# 	SET(DX_ARCHITECTURE "x64")
# ELSEIF(CMAKE_SIZEOF_VOID_P EQUAL 4)
# 	SET(DX_ARCHITECTURE "x86")
# ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)

MESSAGE(STATUS "libsl-config.cmake")

OPTION(LIBSL_USE_GLUX  "Use gluX with LibSL_gl" ON) # TODO FIXME read this from libsl config

################################################################################

IF (NOT LibSL_DIR)
FIND_PATH(LibSL_DIR
	NAMES
		CREDITS.txt libsl-config.cmake
	HINTS
		ENV LibSL_DIR
		ENV LIBSL_DIR
	PATHS
		~/LIBRARIES/LibSL/
		~/LibSL
		./LibSL
)
ENDIF ()

file(TO_CMAKE_PATH ${LibSL_DIR} LibSL_DIR)

IF (NOT DEFINED ENABLE_LIBARCHIVE)
	SET (ENABLE_LIBARCHIVE FALSE)
ENDIF ()

################################################################################

SET(LIBSL_LIBRARY_DIR ${LibSL_DIR}/lib)
SET(LIBSL_INCLUDE_DIR
			${LibSL_DIR}/src
			${LibSL_DIR}/src/LibSL
			${LibSL_DIR}/src/libs/src
			${LibSL_DIR}/src/libs/src/loki/include
			${LibSL_DIR}/src/libs/src/tclap/include
			${LibSL_DIR}/src/libs/src/glux/src
			${LibSL_DIR}/src/libs/src/png
			${LibSL_DIR}/src/libs/src/jpeg
			${LibSL_DIR}/src/libs/src/zlib
			${LibSL_DIR}/src/libs/src/newmat
			${LibSL_DIR}/src/libs/src/tinyxml
			${LibSL_DIR}/src/libs/src/anttweakbar/include
			${LibSL_DIR}/src/libs/src/OpenNL/src
			${LibSL_DIR}/src/libs/bin/win32/TAUCS/include
			${LibSL_DIR}/src/libs/src/SQLite/
			${LibSL_DIR}/src/libs/src/hashlibpp/src/
			${LibSL_DIR}/src/libs/src/rply/
			${LibSL_DIR}/src/libs/src/imgui/
			$ENV{DXSDK_DIR}/include
			$ENV{DXSDK_DIR}/lib/x86
			$ENV{DXSDK_DIR}/Samples/C++/DXUT/Core)

IF (WIN32)

	FIND_LIBRARY(LIBSL_LIBRARY 		LibSL.lib 		PATHS ${LIBSL_LIBRARY_DIR})
	FIND_LIBRARY(LIBSL_LIBRARY_D 		LibSL-d.lib 		PATHS ${LIBSL_LIBRARY_DIR})

	FIND_LIBRARY(LIBSL_GL_LIBRARY 		LibSL_gl.lib 		PATHS ${LIBSL_LIBRARY_DIR})
	FIND_LIBRARY(LIBSL_GL_LIBRARY_D 	LibSL_gl-d.lib 		PATHS ${LIBSL_LIBRARY_DIR})

	FIND_LIBRARY(LIBSL_GL4_LIBRARY 		LibSL_gl4.lib 		PATHS ${LIBSL_LIBRARY_DIR})
	FIND_LIBRARY(LIBSL_GL4_LIBRARY_D 	LibSL_gl4-d.lib 	PATHS ${LIBSL_LIBRARY_DIR})

	FIND_LIBRARY(LIBSL_GL4CORE_LIBRARY 		LibSL_gl4core.lib 		PATHS ${LIBSL_LIBRARY_DIR})
	FIND_LIBRARY(LIBSL_GL4CORE_LIBRARY_D 	LibSL_gl4core-d.lib 	PATHS ${LIBSL_LIBRARY_DIR})

	FIND_LIBRARY(LIBSL_LINALG_LIBRARY 	LibSL_linalg.lib 	PATHS ${LIBSL_LIBRARY_DIR})
	FIND_LIBRARY(LIBSL_LINALG_LIBRARY_D	LibSL_linalg-d.lib 	PATHS ${LIBSL_LIBRARY_DIR})

	FIND_LIBRARY(LIBSL_DX9_LIBRARY 		LibSL_d3d.lib 		PATHS ${LIBSL_LIBRARY_DIR})
	FIND_LIBRARY(LIBSL_DX9_LIBRARY_D 	LibSL_d3d-d.lib 	PATHS ${LIBSL_LIBRARY_DIR})

	FIND_LIBRARY(LIBSL_DX10_LIBRARY 	LibSL_dx10.lib 		PATHS ${LIBSL_LIBRARY_DIR})
	FIND_LIBRARY(LIBSL_DX10_LIBRARY_D 	LibSL_dx10-d.lib 	PATHS ${LIBSL_LIBRARY_DIR})

	FIND_LIBRARY(LIBSL_SHARPBINDER_LIBRARY 		LibSL_sharp_binder.lib 		PATHS ${LIBSL_LIBRARY_DIR})
	FIND_LIBRARY(LIBSL_SHARPBINDER_LIBRARY_D 	LibSL_sharp_binder-d.lib 	PATHS ${LIBSL_LIBRARY_DIR})

	#	SET(TAUCS_LIB_DIR ${LibSL_DIR}/src/libs/bin/win32/TAUCS/lib)
#	SET(TAUCS_LIBS  "${TAUCS_LIB_DIR}/libatlas.lib;${TAUCS_LIB_DIR}/libcblas.lib;${TAUCS_LIB_DIR}/libf77blas.lib;${TAUCS_LIB_DIR}/liblapack.lib;${TAUCS_LIB_DIR}/libmetis.lib;${TAUCS_LIB_DIR}/vcf2c.lib;${TAUCS_LIB_DIR}/libtaucs.lib")

ELSE (WIN32)

	FIND_LIBRARY(LIBSL_LIBRARY 		libLibSL.a 		PATHS ${LIBSL_LIBRARY_DIR})
	FIND_LIBRARY(LIBSL_LIBRARY_D 		libLibSL-d.a 		PATHS ${LIBSL_LIBRARY_DIR})

	FIND_LIBRARY(LIBSL_GL_LIBRARY 		libLibSL_gl.a 		PATHS ${LIBSL_LIBRARY_DIR})
	FIND_LIBRARY(LIBSL_GL_LIBRARY_D 	libLibSL_gl-d.a 	PATHS ${LIBSL_LIBRARY_DIR})

	FIND_LIBRARY(LIBSL_GL4_LIBRARY 		libLibSL_gl4.a 		PATHS ${LIBSL_LIBRARY_DIR})
	FIND_LIBRARY(LIBSL_GL4_LIBRARY_D 	libLibSL_gl4-d.a 	PATHS ${LIBSL_LIBRARY_DIR})

	FIND_LIBRARY(LIBSL_GL4CORE_LIBRARY 		libLibSL_gl4core.a 		PATHS ${LIBSL_LIBRARY_DIR})
	FIND_LIBRARY(LIBSL_GL4CORE_LIBRARY_D 	libLibSL_gl4core-d.a 	PATHS ${LIBSL_LIBRARY_DIR})

	FIND_LIBRARY(LIBSL_LINALG_LIBRARY 	LibSL_linalg.a 		PATHS ${LIBSL_LIBRARY_DIR})
	FIND_LIBRARY(LIBSL_LINALG_LIBRARY_D	LibSL_linalg-d.a 	PATHS ${LIBSL_LIBRARY_DIR})

	FIND_LIBRARY(LIBSL_DX9_LIBRARY 		libLibSL_d3d.a 		PATHS ${LIBSL_LIBRARY_DIR})
	FIND_LIBRARY(LIBSL_DX9_LIBRARY_D 	libLibSL_d3d-d.a 	PATHS ${LIBSL_LIBRARY_DIR})

	FIND_LIBRARY(LIBSL_DX10_LIBRARY 	libLibSL_d3d.a 		PATHS ${LIBSL_LIBRARY_DIR})
	FIND_LIBRARY(LIBSL_DX10_LIBRARY_D 	libLibSL_d3d-d.a 	PATHS ${LIBSL_LIBRARY_DIR})

ENDIF (WIN32)

################################################################################

IF(NOT APPLE)
	IF (LIBSL_USE_GLUX)
		ADD_DEFINITIONS(-DUSE_GLUX)
		IF(NOT WIN32)
			ADD_DEFINITIONS(-DGL_ARB_multitexture=1)  # hack due to MESA headers including ARB_multitexture
		ENDIF(NOT WIN32)
	ENDIF (LIBSL_USE_GLUX)
ENDIF(NOT APPLE)

ADD_DEFINITIONS(-DUNICODE -D_UNICODE)
ADD_DEFINITIONS(-DTW_STATIC -DTW_NO_LIB_PRAGMA)

Find_Package(OpenGL)
IF (CMAKE_VERSION VERSION_LESS 2.8.0)
	# JD: With recent versions of CMake, FindGLU.cmake is basically
	# just an alias to FindOpenGL.cmake, so we can omit it safely.
	Find_Package(GLU)
ENDIF ()
Find_Package(GLUT)

################################################################################

# SL: This ended up being necessary due to an odd behavior of Find_Package(OpenGL)
#     returning 'nul' on Emscripten...
if (OPENGL_LIBRARIES STREQUAL "nul")
set(OPENGL_LIBRARIES "")
endif ()

################################################################################

IF (WIN32)

########################################
  SET(LIBSL_LIBRARIES
	optimized ${LIBSL_LIBRARY_DIR}/LibSL.lib
	optimized ${LIBSL_LIBRARY_DIR}/jpeg.lib
	optimized ${LIBSL_LIBRARY_DIR}/png.lib
	optimized ${LIBSL_LIBRARY_DIR}/zlib.lib
	optimized ${LIBSL_LIBRARY_DIR}/loki.lib
	optimized ${LIBSL_LIBRARY_DIR}/3ds.lib
	optimized ${LIBSL_LIBRARY_DIR}/tinyxml.lib
	optimized ${LIBSL_LIBRARY_DIR}/hashlibpp.lib
	optimized ${LIBSL_LIBRARY_DIR}/qhull.lib
	debug ${LIBSL_LIBRARY_DIR}/LibSL-d.lib
	debug ${LIBSL_LIBRARY_DIR}/jpeg-d.lib
	debug ${LIBSL_LIBRARY_DIR}/png-d.lib
	debug ${LIBSL_LIBRARY_DIR}/zlib-d.lib
	debug ${LIBSL_LIBRARY_DIR}/loki-d.lib
	debug ${LIBSL_LIBRARY_DIR}/3ds-d.lib
	debug ${LIBSL_LIBRARY_DIR}/tinyxml-d.lib
	debug ${LIBSL_LIBRARY_DIR}/hashlibpp-d.lib
	debug ${LIBSL_LIBRARY_DIR}/qhull-d.lib
	winmm.lib
	psapi.lib
	shlwapi.lib
  )

########################################
IF (NOT TARGET LibSL)
  ADD_LIBRARY(LibSL STATIC IMPORTED)
  SET_PROPERTY(TARGET LibSL PROPERTY IMPORTED_LOCATION_DEBUG
	${LIBSL_LIBRARY_DIR}/LibSL-d.lib)
  SET_PROPERTY(TARGET LibSL PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG
	${LIBSL_LIBRARY_DIR}/jpeg-d.lib
	${LIBSL_LIBRARY_DIR}/png-d.lib
	${LIBSL_LIBRARY_DIR}/zlib-d.lib
	${LIBSL_LIBRARY_DIR}/loki-d.lib
	${LIBSL_LIBRARY_DIR}/3ds-d.lib
	${LIBSL_LIBRARY_DIR}/tinyxml-d.lib
	${LIBSL_LIBRARY_DIR}/hashlibpp-d.lib
	${LIBSL_LIBRARY_DIR}/qhull-d.lib
	winmm.lib
	psapi.lib
	shlwapi.lib
  )
  SET_PROPERTY(TARGET LibSL PROPERTY IMPORTED_LOCATION_RELEASE
	${LIBSL_LIBRARY_DIR}/LibSL.lib)
  SET_PROPERTY(TARGET LibSL PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE
	${LIBSL_LIBRARY_DIR}/jpeg.lib
	${LIBSL_LIBRARY_DIR}/png.lib
	${LIBSL_LIBRARY_DIR}/zlib.lib
	${LIBSL_LIBRARY_DIR}/loki.lib
	${LIBSL_LIBRARY_DIR}/3ds.lib
	${LIBSL_LIBRARY_DIR}/tinyxml.lib
	${LIBSL_LIBRARY_DIR}/hashlibpp.lib
	${LIBSL_LIBRARY_DIR}/qhull.lib
	winmm.lib
	psapi.lib
	shlwapi.lib
  )
#  SET_PROPERTY(TARGET LibSL PROPERTY INTERFACE_COMPILE_DEFINITIONS
#	UNICODE
#	_UNICODE
#  )
  SET_PROPERTY(TARGET LibSL PROPERTY INTERFACE_INCLUDE_DIRECTORIES
			${LibSL_DIR}/src
			${LibSL_DIR}/src/LibSL
			${LibSL_DIR}/src/libs/src
			${LibSL_DIR}/src/libs/src/loki/include
			${LibSL_DIR}/src/libs/src/tclap/include
			${LibSL_DIR}/src/libs/src/glux/src
			${LibSL_DIR}/src/libs/src/png
			${LibSL_DIR}/src/libs/src/jpeg
			${LibSL_DIR}/src/libs/src/zlib
			${LibSL_DIR}/src/libs/src/newmat
			${LibSL_DIR}/src/libs/src/tinyxml
			${LibSL_DIR}/src/libs/src/anttweakbar/include
			${LibSL_DIR}/src/libs/src/OpenNL/src
			${LibSL_DIR}/src/libs/bin/win32/TAUCS/include
			${LibSL_DIR}/src/libs/src/SQLite/
			${LibSL_DIR}/src/libs/src/hashlibpp/src/
  )
ENDIF()

########################################
  SET(LIBSL_GL_LIBRARIES
  optimized ${LIBSL_LIBRARY_DIR}/LibSL_gl.lib
  optimized ${LIBSL_LIBRARY_DIR}/AntTweakBar.lib
  optimized ${LIBSL_LIBRARY_DIR}/glux.lib
  debug 	${LIBSL_LIBRARY_DIR}/LibSL_gl-d.lib
  debug 	${LIBSL_LIBRARY_DIR}/glux-d.lib
  debug 	${LIBSL_LIBRARY_DIR}/AntTweakBar-d.lib
  ${OPENGL_LIBRARIES}
  )

########################################
IF (NOT TARGET LibSL_gl)
  ADD_LIBRARY(LibSL_gl STATIC IMPORTED)
  SET_PROPERTY(TARGET LibSL_gl PROPERTY IMPORTED_LOCATION_DEBUG
	${LIBSL_LIBRARY_DIR}/LibSL_gl-d.lib)
  SET_PROPERTY(TARGET LibSL_gl PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG
	${LIBSL_LIBRARY_DIR}/glux-d.lib
	${LIBSL_LIBRARY_DIR}/AntTweakBar-d.lib
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
  )
  SET_PROPERTY(TARGET LibSL_gl PROPERTY IMPORTED_LOCATION_RELEASE
	${LIBSL_LIBRARY_DIR}/LibSL_gl.lib)
  SET_PROPERTY(TARGET LibSL_gl PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE
	${LIBSL_LIBRARY_DIR}/glux.lib
	${LIBSL_LIBRARY_DIR}/AntTweakBar.lib
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
  )
  SET_PROPERTY(TARGET LibSL_gl PROPERTY INTERFACE_COMPILE_DEFINITIONS
  	OPENGL
	TW_STATIC
	TW_NO_LIB_PRAGMA
  )
ENDIF()

########################################
SET(LIBSL_GL4_LIBRARIES
  optimized ${LIBSL_LIBRARY_DIR}/LibSL_gl4.lib
  optimized ${LIBSL_LIBRARY_DIR}/AntTweakBar.lib
  optimized ${LIBSL_LIBRARY_DIR}/glux.lib
  debug 	${LIBSL_LIBRARY_DIR}/LibSL_gl4-d.lib
  debug 	${LIBSL_LIBRARY_DIR}/glux-d.lib
  debug 	${LIBSL_LIBRARY_DIR}/AntTweakBar-d.lib
  ${OPENGL_LIBRARIES}
  )

IF (NOT TARGET LibSL_gl4)
  ADD_LIBRARY(LibSL_gl4 STATIC IMPORTED)
  SET_PROPERTY(TARGET LibSL_gl4 PROPERTY IMPORTED_LOCATION_DEBUG
	${LIBSL_LIBRARY_DIR}/LibSL_gl4-d.lib)
  SET_PROPERTY(TARGET LibSL_gl4 PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG
	${LIBSL_LIBRARY_DIR}/glux-d.lib
	${LIBSL_LIBRARY_DIR}/AntTweakBar-d.lib
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
  )
  SET_PROPERTY(TARGET LibSL_gl4 PROPERTY IMPORTED_LOCATION_RELEASE
	${LIBSL_LIBRARY_DIR}/LibSL_gl4.lib)
  SET_PROPERTY(TARGET LibSL_gl4 PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE
	${LIBSL_LIBRARY_DIR}/glux.lib
	${LIBSL_LIBRARY_DIR}/AntTweakBar.lib
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
  )
  SET_PROPERTY(TARGET LibSL_gl4 PROPERTY INTERFACE_COMPILE_DEFINITIONS
  	OPENGL
  	OPENGL4
	TW_STATIC
	TW_NO_LIB_PRAGMA
  )
ENDIF()

########################################
SET(LIBSL_GL4CORE_LIBRARIES
  optimized ${LIBSL_LIBRARY_DIR}/LibSL_gl4core.lib
  optimized ${LIBSL_LIBRARY_DIR}/AntTweakBar.lib
  optimized ${LIBSL_LIBRARY_DIR}/glux.lib
  debug 	${LIBSL_LIBRARY_DIR}/LibSL_gl4core-d.lib
  debug 	${LIBSL_LIBRARY_DIR}/glux-d.lib
  debug 	${LIBSL_LIBRARY_DIR}/AntTweakBar-d.lib
  ${OPENGL_LIBRARIES}
  )

IF (NOT TARGET LibSL_gl4core)
  ADD_LIBRARY(LibSL_gl4core STATIC IMPORTED)
  SET_PROPERTY(TARGET LibSL_gl4core PROPERTY IMPORTED_LOCATION_DEBUG
	${LIBSL_LIBRARY_DIR}/LibSL_gl4core-d.lib)
  SET_PROPERTY(TARGET LibSL_gl4core PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG
	${LIBSL_LIBRARY_DIR}/glux-d.lib
	${LIBSL_LIBRARY_DIR}/AntTweakBar-d.lib
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
  )
  SET_PROPERTY(TARGET LibSL_gl4core PROPERTY IMPORTED_LOCATION_RELEASE
	${LIBSL_LIBRARY_DIR}/LibSL_gl4core.lib)
  SET_PROPERTY(TARGET LibSL_gl4core PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE
	${LIBSL_LIBRARY_DIR}/glux.lib
	${LIBSL_LIBRARY_DIR}/AntTweakBar.lib
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
  )
  SET_PROPERTY(TARGET LibSL_gl4core PROPERTY INTERFACE_COMPILE_DEFINITIONS
  	OPENGL
  	OPENGL4
    OPENGLCORE
    TW_STATIC
    TW_NO_LIB_PRAGMA
  )
ENDIF()

########################################
  SET(LIBSL_LINALG_LIBRARIES
	optimized 	${LIBSL_LIBRARY_DIR}/LibSL_linalg.lib
	optimized 	${LIBSL_LIBRARY_DIR}/nl.lib
	optimized 	${LIBSL_LIBRARY_DIR}/newmat.lib
	debug 		${LIBSL_LIBRARY_DIR}/LibSL_linalg-d.lib
	debug 		${LIBSL_LIBRARY_DIR}/nl-d.lib
	debug 		${LIBSL_LIBRARY_DIR}/newmat-d.lib
	${TAUCS_LIBS}
  )

########################################
IF (NOT TARGET LibSL_linalg)
  ADD_LIBRARY(LibSL_linalg STATIC IMPORTED)
  SET_PROPERTY(TARGET LibSL_linalg PROPERTY IMPORTED_LOCATION_DEBUG
	${LIBSL_LIBRARY_DIR}/LibSL_linalg-d.lib)
  SET_PROPERTY(TARGET LibSL_linalg PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG
	${LIBSL_LIBRARY_DIR}/nl-d.lib
	${LIBSL_LIBRARY_DIR}/newmat-d.lib
  )
  SET_PROPERTY(TARGET LibSL_linalg PROPERTY IMPORTED_LOCATION_RELEASE
	${LIBSL_LIBRARY_DIR}/LibSL_linalg.lib)
  SET_PROPERTY(TARGET LibSL_linalg PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE
	${LIBSL_LIBRARY_DIR}/nl.lib
	${LIBSL_LIBRARY_DIR}/newmat.lib
  )
ENDIF()

########################################
  SET(LIBSL_DX9_LIBRARIES
	optimized ${LIBSL_LIBRARY_DIR}/LibSL_d3d.lib
	debug 	  ${LIBSL_LIBRARY_DIR}/LibSL_d3d-d.lib
	$ENV{DXSDK_DIR}/lib/x86/d3d9.lib
	$ENV{DXSDK_DIR}/lib/x86/d3dx9.lib
	$ENV{DXSDK_DIR}/lib/x86/dxerr.lib
	$ENV{DXSDK_DIR}/Samples/C++/DXUT/Core/Release/dxut.lib
	comctl32.lib
	)

########################################
IF (NOT TARGET LibSL_sharp_binder)
  ADD_LIBRARY(LibSL_sharp_binder STATIC IMPORTED)
  SET_PROPERTY(TARGET LibSL_sharp_binder PROPERTY IMPORTED_LOCATION_DEBUG ${LIBSL_LIBRARY_DIR}/LibSL_sharp_binder-d.lib)
  SET_PROPERTY(TARGET LibSL_sharp_binder PROPERTY IMPORTED_LOCATION_RELEASE ${LIBSL_LIBRARY_DIR}/LibSL_sharp_binder.lib)
  SET_PROPERTY(TARGET LibSL_sharp_binder PROPERTY INTERFACE_INCLUDE_DIRECTORIES
		${LibSL_DIR}/src/LibSL_sharp/GLPanel/C++/
  )
ENDIF()

########################################
IF (NOT TARGET LibSL_d3d)
  ADD_LIBRARY(LibSL_d3d STATIC IMPORTED)
  SET_PROPERTY(TARGET LibSL_d3d PROPERTY IMPORTED_LOCATION_DEBUG
	${LIBSL_LIBRARY_DIR}/LibSL_d3d-d.lib)
  SET_PROPERTY(TARGET LibSL_d3d PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG
	$ENV{DXSDK_DIR}/lib/x86/d3d9.lib
	$ENV{DXSDK_DIR}/lib/x86/d3dx9.lib
	$ENV{DXSDK_DIR}/lib/x86/dxerr.lib
	$ENV{DXSDK_DIR}/Samples/C++/DXUT/Core/Release/dxut.lib
	comctl32.lib
  )
  SET_PROPERTY(TARGET LibSL_d3d PROPERTY IMPORTED_LOCATION_RELEASE
	${LIBSL_LIBRARY_DIR}/LibSL_d3d.lib)
  SET_PROPERTY(TARGET LibSL_d3d PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE
	$ENV{DXSDK_DIR}/lib/x86/d3d9.lib
	$ENV{DXSDK_DIR}/lib/x86/d3dx9.lib
	$ENV{DXSDK_DIR}/lib/x86/dxerr.lib
	$ENV{DXSDK_DIR}/Samples/C++/DXUT/Core/Release/dxut.lib
	comctl32.lib
  )
#  SET_PROPERTY(TARGET LibSL_d3d PROPERTY COMPILE_DEFINITIONS
#	UNICODE
#	_UNICODE
#  )
  SET_PROPERTY(TARGET LibSL_d3d PROPERTY INTERFACE_INCLUDE_DIRECTORIES
		$ENV{DXSDK_DIR}/Include
		$ENV{DXSDK_DIR}/lib/x86
		$ENV{DXSDK_DIR}/Samples/C++/DXUT/Core/
  )
ENDIF()

########################################
	SET(LIBSL_DX10_LIBRARIES
		optimized	${LIBSL_LIBRARY_DIR}/LibSL_dx10.lib
		debug    	${LIBSL_LIBRARY_DIR}/LibSL_dx10.lib
		$ENV{DXSDK_DIR}/lib/x86/d3d11.lib
		$ENV{DXSDK_DIR}/lib/x86/d3dx11.lib
		$ENV{DXSDK_DIR}/lib/x86/dxerr.lib
		$ENV{DXSDK_DIR}/Samples/C++/DXUT/Core/Release/dxut.lib
		comctl32.lib
	)

########################################
IF (NOT TARGET LibSL_dx10)
  ADD_LIBRARY(LibSL_dx10 STATIC IMPORTED)
  SET_PROPERTY(TARGET LibSL_dx10 PROPERTY IMPORTED_LOCATION_DEBUG
	${LIBSL_LIBRARY_DIR}/LibSL_dx10-d.lib)
  SET_PROPERTY(TARGET LibSL_dx10 PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG
	$ENV{DXSDK_DIR}/lib/x86/d3d11.lib
	$ENV{DXSDK_DIR}/lib/x86/d3dx11.lib
	$ENV{DXSDK_DIR}/lib/x86/dxerr.lib
	$ENV{DXSDK_DIR}/Samples/C++/DXUT/Core/Release/dxut.lib
	comctl32.lib
  )
  SET_PROPERTY(TARGET LibSL_dx10 PROPERTY IMPORTED_LOCATION_RELEASE
	${LIBSL_LIBRARY_DIR}/LibSL_dx10.lib)
  SET_PROPERTY(TARGET LibSL_dx10 PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE
	$ENV{DXSDK_DIR}/lib/x86/d3d11.lib
	$ENV{DXSDK_DIR}/lib/x86/d3dx11.lib
	$ENV{DXSDK_DIR}/lib/x86/dxerr.lib
	$ENV{DXSDK_DIR}/Samples/C++/DXUT/Core/Release/dxut.lib
	comctl32.lib
  )
#  SET_PROPERTY(TARGET LibSL_dx10 PROPERTY COMPILE_DEFINITIONS
#	UNICODE
#	_UNICODE
#  )
  SET_PROPERTY(TARGET LibSL_dx10 PROPERTY INTERFACE_INCLUDE_DIRECTORIES
		$ENV{DXSDK_DIR}/Include
		$ENV{DXSDK_DIR}/lib/x86
		$ENV{DXSDK_DIR}/Samples/C++/DXUT/Core/
  )
ENDIF()

########################################
	SET(LIBSL_DX9_INCLUDE_DIR
		$ENV{DXSDK_DIR}/Samples/C++/DXUT/Core/
		$ENV{DXSDK_DIR}/Include)

	SET(LIBSL_DX10_INCLUDE_DIR
		$ENV{DXSDK_DIR}/Samples/C++/DXUT/Core/
		$ENV{DXSDK_DIR}/Include)

	ADD_DEFINITIONS(-D_UNICODE -DUNICODE)

################################################################################
ELSE (WIN32)

	include(FindGLUT)

	INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})

########################################
	SET(LIBSL_LIBRARIES         "${LIBSL_LIBRARY_DIR}/libLibSL.a;${LIBSL_LIBRARY_DIR}/libjpeg.a;${LIBSL_LIBRARY_DIR}/libpng.a;${LIBSL_LIBRARY_DIR}/libzlib.a;${LIBSL_LIBRARY_DIR}/libloki.a;${LIBSL_LIBRARY_DIR}/lib3ds.a;${LIBSL_LIBRARY_DIR}/libtinyxml.a;${LIBSL_LIBRARY_DIR}/libhashlibpp.a;${LIBSL_LIBRARY_DIR}/libqhull.a")

	if (NOT EMSCRIPTEN)
	  SET(LIBSL_GL_LIBRARIES      "${LIBSL_LIBRARY_DIR}/libLibSL_gl.a;${LIBSL_LIBRARY_DIR}/libglux.a;${LIBSL_LIBRARY_DIR}/libAntTweakBar.a" ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
	else (NOT EMSCRIPTEN)
  	  SET(LIBSL_GL_LIBRARIES      "${LIBSL_LIBRARY_DIR}/libLibSL_gl.a" ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
	endif (NOT EMSCRIPTEN)

	SET(LIBSL_GL4_LIBRARIES     "${LIBSL_LIBRARY_DIR}/libLibSL_gl4.a;${LIBSL_LIBRARY_DIR}/libglux.a;${LIBSL_LIBRARY_DIR}/libAntTweakBar.a" ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

	SET(LIBSL_GL4CORE_LIBRARIES     "${LIBSL_LIBRARY_DIR}/libLibSL_gl4core.a;${LIBSL_LIBRARY_DIR}/libglux.a;${LIBSL_LIBRARY_DIR}/libAntTweakBar.a" ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

	SET(LIBSL_LINALG_LIBRARIES  "${LIBSL_LIBRARY_DIR}/libLibSL_linalg.a;${LIBSL_LIBRARY_DIR}/libnl.a")

########################################
IF (NOT TARGET LibSL)
  ADD_LIBRARY(LibSL STATIC IMPORTED)
  SET_PROPERTY(TARGET LibSL PROPERTY IMPORTED_LOCATION
	${LIBSL_LIBRARY_DIR}/libLibSL.a)
  SET_PROPERTY(TARGET LibSL PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES
	${LIBSL_LIBRARY_DIR}/libjpeg.a
	${LIBSL_LIBRARY_DIR}/libpng.a
	${LIBSL_LIBRARY_DIR}/libzlib.a
	${LIBSL_LIBRARY_DIR}/libloki.a
	${LIBSL_LIBRARY_DIR}/lib3ds.a
	${LIBSL_LIBRARY_DIR}/libtinyxml.a
	${LIBSL_LIBRARY_DIR}/libhashlibpp.a
	${LIBSL_LIBRARY_DIR}/libqhull.a
  )

  SET_PROPERTY(TARGET LibSL PROPERTY INTERFACE_INCLUDE_DIRECTORIES
	${LibSL_DIR}/src
	${LibSL_DIR}/src/LibSL
	${LibSL_DIR}/src/libs/src
	${LibSL_DIR}/src/libs/src/loki/include
	${LibSL_DIR}/src/libs/src/tclap/include
	${LibSL_DIR}/src/libs/src/glux/src
	${LibSL_DIR}/src/libs/src/png
	${LibSL_DIR}/src/libs/src/jpeg
	${LibSL_DIR}/src/libs/src/zlib
	${LibSL_DIR}/src/libs/src/newmat
	${LibSL_DIR}/src/libs/src/tinyxml
	${LibSL_DIR}/src/libs/src/anttweakbar/include
	${LibSL_DIR}/src/libs/src/OpenNL/src
	${LibSL_DIR}/src/libs/bin/win32/TAUCS/include
	${LibSL_DIR}/src/libs/src/SQLite/
	${LibSL_DIR}/src/libs/src/hashlibpp/src/
  )
ENDIF()

########################################
IF (NOT TARGET AntTweakBar)
  IF(NOT APPLE)
	ADD_LIBRARY(AntTweakBar STATIC IMPORTED)
	IF (NOT EMSCRIPTEN)
	  FIND_PACKAGE(X11 REQUIRED)
	ENDIF (NOT EMSCRIPTEN)
	SET_PROPERTY(TARGET AntTweakBar PROPERTY IMPORTED_LOCATION
	${LIBSL_LIBRARY_DIR}/libAntTweakBar.a)
	SET_PROPERTY(TARGET AntTweakBar PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES
	${OPENGL_LIBRARIES}
	${X11_LIBRARIES}
	)
  ENDIF(APPLE)
  #IF(APPLE)
  #    ADD_DEFINITIONS(-D_MACOSX)
  #    ADD_DEFINITIONS(-D__PLACEMENT_NEW_INLINE)
  #    SET_TARGET_PROPERTIES(AntTweakBar PROPERTIES INTERFACE_COMPILE_DEFINITIONS "-O3 -arch i386 -arch x86_64 -fno-strict-aliasing -ObjC++")
  #ELSE(APPLE)
  #    SET_TARGET_PROPERTIES(AntTweakBar PROPERTIES INTERFACE_COMPILE_DEFINITIONS "-O3 -D_UNIX -D__PLACEMENT_NEW_INLINE")
  #ENDIF(APPLE)
ENDIF ()

########################################
IF (NOT TARGET LibSL_gl)
  ADD_LIBRARY(LibSL_gl STATIC IMPORTED)
  SET_PROPERTY(TARGET LibSL_gl PROPERTY IMPORTED_LOCATION	${LIBSL_LIBRARY_DIR}/libLibSL_gl.a)
  IF (NOT EMSCRIPTEN)
    SET_PROPERTY(TARGET LibSL_gl PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES
    LibSL
    AntTweakBar
    ${LIBSL_LIBRARY_DIR}/libglux.a
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
    )
  ELSE (NOT EMSCRIPTEN)
    SET_PROPERTY(TARGET LibSL_gl PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES
    LibSL
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
    )
  ENDIF (NOT EMSCRIPTEN)  
  SET_PROPERTY(TARGET LibSL_gl PROPERTY INTERFACE_COMPILE_DEFINITIONS
	OPENGL
	TW_STATIC
	TW_NO_LIB_PRAGMA
  )
ENDIF()

########################################
IF (NOT TARGET LibSL_gl4)
  ADD_LIBRARY(LibSL_gl4 STATIC IMPORTED)
  SET_PROPERTY(TARGET LibSL_gl4 PROPERTY IMPORTED_LOCATION
	${LIBSL_LIBRARY_DIR}/libLibSL_gl4.a)
  SET_PROPERTY(TARGET LibSL_gl4 PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES
	LibSL
	AntTweakBar
	${LIBSL_LIBRARY_DIR}/libglux.a
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
  )
  SET_PROPERTY(TARGET LibSL_gl4 PROPERTY INTERFACE_COMPILE_DEFINITIONS
  	OPENGL
  	OPENGL4
  	USE_GLUT
	TW_STATIC
	TW_NO_LIB_PRAGMA
  )
ENDIF()

########################################
IF (NOT TARGET LibSL_gl4core)
  ADD_LIBRARY(LibSL_gl4core STATIC IMPORTED)
  SET_PROPERTY(TARGET LibSL_gl4core PROPERTY IMPORTED_LOCATION
	${LIBSL_LIBRARY_DIR}/libLibSL_gl4core.a)
  SET_PROPERTY(TARGET LibSL_gl4core PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES
	LibSL
	AntTweakBar
	${LIBSL_LIBRARY_DIR}/libglux.a
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
  )
  SET_PROPERTY(TARGET LibSL_gl4core PROPERTY INTERFACE_COMPILE_DEFINITIONS
  	OPENGL
  	OPENGL4
  	USE_GLUT
	TW_STATIC
	TW_NO_LIB_PRAGMA
  )
ENDIF()

########################################
IF (NOT TARGET LibSL_linalg)
  ADD_LIBRARY(LibSL_linalg STATIC IMPORTED)
  SET_PROPERTY(TARGET LibSL_linalg PROPERTY IMPORTED_LOCATION
	${LIBSL_LIBRARY_DIR}/libLibSL_linalg.a)
  SET_PROPERTY(TARGET LibSL_linalg PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES
	${LIBSL_LIBRARY_DIR}/libnl.a
  )
ENDIF()

ENDIF (WIN32)

########################################
IF (ENABLE_LIBARCHIVE)
IF (NOT TARGET archive_static)
	ADD_SUBDIRECTORY(${LibSL_DIR}/src/libs/src/libarchive libsl/libarchive)
ENDIF ()
ENDIF ()

################################################################################

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibSL		DEFAULT_MSG 	LIBSL_LIBRARIES		LIBSL_INCLUDE_DIR)

INCLUDE(${LibSL_DIR}/src/tools/autobindshader/AutoBindShader.cmake)
