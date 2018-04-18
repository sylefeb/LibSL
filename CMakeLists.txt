CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Removes warning about a new policy introduced by CMake 3.0
IF (POLICY CMP0048 OR (NOT CMAKE_VERSION VERSION_LESS 3.0))
	CMAKE_POLICY(SET CMP0048 OLD)
ENDIF ()

PROJECT(LibSL)

# MESSAGE(STATUS "[in LibSL/]")

SET(LibSL_DIR ${CMAKE_CURRENT_SOURCE_DIR})

#############################################################################
# Options
if(EMSCRIPTEN)

OPTION(LIBSL_BUILD_GL              "Build LibSL_gl" OFF)
OPTION(LIBSL_BUILD_GL4             "Build LibSL_gl for OpenGL 4.x" ON)
OPTION(LIBSL_BUILD_GL4CORE         "Build LibSL_gl for OpenGL 4.x core profile" OFF)
OPTION(LIBSL_BUILD_DX9             "Build LibSL_d3d" OFF)
OPTION(LIBSL_BUILD_DX10            "Build LibSL_dx10" OFF)
OPTION(LIBSL_BUILD_LINALG          "Build LibSL_linalg" ON)
OPTION(LIBSL_BUILD_MAX_PERFORMANCE "Build LibSL for performance rather than safety" ON)
OPTION(LIBSL_BUILD_SHARP           "Build LibSL_sharp" OFF)
OPTION(LIBSL_BUILD_TUTORIALS       "Build LibSL tutorials" ON)
OPTION(LIBSL_MULTITHREADED_STATIC  "Build LibSL with /MT flag in Visual C++" OFF)
OPTION(LIBSL_BUILD_GLUX            "Build gluX"   OFF)
OPTION(LIBSL_BUILD_LIBARCHIVE      "Build libarchive"   OFF)
OPTION(LIBSL_USE_GLUX              "Use gluX with LibSL_gl"   OFF)
OPTION(LIBSL_POWER_USER            "Power user mode (reserved, do not use)" OFF)

else(EMSCRIPTEN)

OPTION(LIBSL_BUILD_GL              "Build LibSL_gl" ON)
OPTION(LIBSL_BUILD_GL4             "Build LibSL_gl for OpenGL 4.x" ON)
OPTION(LIBSL_BUILD_GL4CORE         "Build LibSL_gl for OpenGL 4.x core profile" ON)
OPTION(LIBSL_BUILD_DX9             "Build LibSL_d3d" OFF)
OPTION(LIBSL_BUILD_DX10            "Build LibSL_dx10" OFF)
OPTION(LIBSL_BUILD_LINALG          "Build LibSL_linalg" ON)
OPTION(LIBSL_BUILD_MAX_PERFORMANCE "Build LibSL for performance rather than safety" ON)
if(WIN32)
OPTION(LIBSL_BUILD_SHARP           "Build LibSL_sharp" ON)
else(WIN32)
OPTION(LIBSL_BUILD_SHARP           "Build LibSL_sharp" OFF)
endif(WIN32)
OPTION(LIBSL_BUILD_TUTORIALS       "Build LibSL tutorials" ON)
OPTION(LIBSL_MULTITHREADED_STATIC  "Build LibSL with /MT flag in Visual C++" OFF)
OPTION(LIBSL_BUILD_GLUX            "Build gluX"   ON)
OPTION(LIBSL_BUILD_LIBARCHIVE      "Build libarchive"   OFF)
OPTION(LIBSL_USE_GLUX              "Use gluX with LibSL_gl"   ON)
OPTION(LIBSL_POWER_USER            "Power user mode (reserved, do not use)" OFF)

endif(EMSCRIPTEN)

#############################################################################
# Configure header

configure_file (
  "${PROJECT_SOURCE_DIR}/src/LibSL/LibSL.config.h.in"
  "${PROJECT_SOURCE_DIR}/src/LibSL/LibSL.config.h"
  )

configure_file (
  "${PROJECT_SOURCE_DIR}/src/LibSL/LibSL_gl.config.h.in"
  "${PROJECT_SOURCE_DIR}/src/LibSL/LibSL_gl.config.h"
  )

#############################################################################
# Include our custom macros
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/src/build/)

INCLUDE(macros)
INCLUDE(ForceOutOfSourceBuild)
Include(UseColors)

# Find Boost
# SET(LIBSL_BOOST_INCLUDE_PATH CACHE PATH "Boost include path")
# SET(LIBSL_BOOST_LIBRARY_PATH CACHE PATH "Boost library path")

# SET(BOOST_INCLUDEDIR "${LIBSL_BOOST_INCLUDE_PATH}")
# SET(BOOST_LIBRARYDIR "${LIBSL_BOOST_LIBRARY_PATH}")

# SET(Boost_USE_STATIC_LIBS ON)
# FIND_PACKAGE(Boost 1.35.0 COMPONENTS filesystem thread date_time program_options signals system)

# IF(NOT Boost_FOUND)
#   MESSAGE(SEND_ERROR "Boost not found")
# ENDIF(NOT Boost_FOUND)
#end find boost

INCLUDE_DIRECTORIES(
			${CMAKE_CURRENT_SOURCE_DIR}/src
			${CMAKE_CURRENT_SOURCE_DIR}/src/LibSL
			${CMAKE_CURRENT_SOURCE_DIR}/src/libs/src
			${CMAKE_CURRENT_SOURCE_DIR}/src/libs/src/loki/include
			${CMAKE_CURRENT_SOURCE_DIR}/src/libs/src/tclap/include
			${CMAKE_CURRENT_SOURCE_DIR}/src/libs/src/glux/src
			${CMAKE_CURRENT_SOURCE_DIR}/src/libs/src/png
			${CMAKE_CURRENT_SOURCE_DIR}/src/libs/src/jpeg
			${CMAKE_CURRENT_SOURCE_DIR}/src/libs/src/zlib
			${CMAKE_CURRENT_SOURCE_DIR}/src/libs/src/newmat
			${CMAKE_CURRENT_SOURCE_DIR}/src/libs/src/OpenNL/src
			${CMAKE_CURRENT_SOURCE_DIR}/src/libs/src/anttweakbar/include
			${CMAKE_CURRENT_SOURCE_DIR}/src/libs/src/SQLite
			${CMAKE_CURRENT_SOURCE_DIR}/src/libs/src/qhull/src/libqhull
			${CMAKE_CURRENT_SOURCE_DIR}/src/libs/src/rply
			)

if (LIBSL_ENABLE_NVIDIA_CG)
INCLUDE_DIRECTORIES($ENV{CG_INC_PATH})
LINK_DIRECTORIES($ENV{CG_LIB_PATH})
endif (LIBSL_ENABLE_NVIDIA_CG)

# Ugly, but force it to use the path we specified. Otherwise, it breaks on
# UNIX as most Linux distros come with a boost installation which gets
# found by default.
INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})
LINK_DIRECTORIES(${BOOST_LIBRARYDIR})

###############################################################################

if (LIBSL_MULTITHREADED_STATIC)
foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE)
   if(${flag_var} MATCHES "/MD")
      string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
   endif(${flag_var} MATCHES "/MD")
endforeach(flag_var)
endif (LIBSL_MULTITHREADED_STATIC)

###############################################################################

## Linux/GCC: add -fPIC so that LibSL can be linked against shared libraries
IF (NOT WIN32)
IF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC"  )
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC"  )
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fPIC"  )
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fPIC"  )
ENDIF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
ENDIF (NOT WIN32)

###############################################################################

if(EMSCRIPTEN)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1")
endif(EMSCRIPTEN)

###############################################################################

# Add C++11 to LibSL target
#Include(UseCXX17)
#AddGlobalCXX11()

ADD_SUBDIRECTORY(src)

# Building LibSL as part of a CMakeLists.txt tree makes the AUTO_BIND_SHADER macro available to the source tree (this behavior is what we want)
INCLUDE(src/tools/autobindshader/AutoBindShader.cmake)
