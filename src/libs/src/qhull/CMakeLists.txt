# CMakeLists.txt -- CMake configuration file for qhull, qhull6, and related programs
#
# To build with MSYS/mingw
#   cd build && cmake -G "MSYS Makefiles" .. && cmake ..
#   make
#   make install
#
# To uninstall on unix or MSYS/mingw
#   xargs rm <build/install_manifest.txt
#
# To build Qhull Visual Studio projects, run cmake twice
#   mkdir -p build-cmake && cd build-cmake && cmake -G "Visual Studio 8 2005" .. && cmake ..
#   Double click build-cmake/qhull-all.sln
#   Build INSTALL for C:/Program Files/qhull
#   You may need to copy bin/msvcr80.dll into C:/Program Files/qhull/bin
#   If using library debug targets, please rename with '_d' (e.g., qhullstatic_d.lib)
#
# To uninstall on Windows
#   Delete C:/Program Files/qhull
#
# For qhulltest, use the Qt build (src/qhull-all.pro)
#
# Qhull ships with cmake-derived sln and proj files
#   See eg/make-vcproj.sh -- it created the first versions of these files
#   Change to relative paths
#   Remove ZERO_CHECK, ALL_BUILD, and INSTALL projects
#   Change targets to bin/ and lib/ directories
#   Disable incremental linking and ilk files (LinkIncremental="1")
#   Disable Run-Time Type Info (rtti)
#   Remove src/libqhullcpp from most of the AdditionalIncludeDirectories
#   Remove CMAKE_INTDIR from PreprocessorDefinitions
#   Adjust target names and destinations (e.g., lib/libqhullstatic_d.a)
#
# $Id: //main/2011/qhull/CMakeLists.txt#23 $$Change: 1494 $
# $DateTime: 2012/02/21 22:41:40 $$Author: bbarber $

project(qhull)
cmake_minimum_required(VERSION 2.6)

# Define qhull_VERSION in CMakeLists.txt, Makefile, qhull-exports.def, qhull_p-exports.def, and qhull-warn.pri
string(REGEX REPLACE ".* ([0-9]+) .*"
    "6.3.1.\\1"  qhull_VERSION "$Change: 1494 $")
# qhull_SOVERSION: 2003 = empty, 2009 = 5 , 2010-2012 = 6
set(qhull_SOVERSION 6)
set(qhull_VERSION2 "2012.1 2012/02/18")

message(STATUS)
message(STATUS "========== qhull Build Information ==========")
message(STATUS "Build Version: 				   ${qhull_VERSION}")
message(STATUS "Build Type (CMAKE_BUILD_TYPE):             ${CMAKE_BUILD_TYPE}")
message(STATUS "To override these options, add -D{OPTION_NAME}=... to the cmake command")
message(STATUS "  Build the debug targets                  -DCMAKE_BUILD_TYPE=Debug")
message(STATUS)

# ---------------------------------------
# Define library source files and variables
#
# Files for individual targets are defined with the target
# ---------------------------------------

# Order libqhull object files by frequency of execution.  Small files at end.
set(
    libqhull_HEADERS
        src/libqhull_r/libqhull_r.h
        src/libqhull_r/geom_r.h
        src/libqhull_r/mem_r.h
        src/libqhull_r/merge_r.h
        src/libqhull_r/poly_r.h
        src/libqhull_r/qhull_ra.h
        src/libqhull_r/qset_r.h
        src/libqhull_r/random_r.h
        src/libqhull_r/stat_r.h
        src/libqhull_r/user_r.h
        src/libqhull_r/io_r.h
)
set(
    libqhull_SOURCES
        src/libqhull_r/global_r.c
        src/libqhull_r/stat_r.c
        src/libqhull_r/geom2_r.c
        src/libqhull_r/poly2_r.c
        src/libqhull_r/merge_r.c
        src/libqhull_r/libqhull_r.c
        src/libqhull_r/geom_r.c
        src/libqhull_r/poly_r.c
        src/libqhull_r/qset_r.c
        src/libqhull_r/mem_r.c
        src/libqhull_r/random_r.c
        src/libqhull_r/usermem_r.c
        src/libqhull_r/userprintf_r.c
        src/libqhull_r/user_r.c
        src/libqhull_r/io_r.c
        src/libqhull_r/rboxlib_r.c
        src/libqhull_r/userprintf_rbox_r.c
        ${libqhull_HEADERS}
)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/libqhull_r)

# ---------------------------------------
# Define static library qhull
# ---------------------------------------

add_library(qhull STATIC ${libqhull_SOURCES})
set_target_properties(qhull PROPERTIES VERSION ${qhull_VERSION})
set_target_properties(qhull PROPERTIES DEBUG_POSTFIX "-d")

if(UNIX)
    target_link_libraries(qhull m)
endif(UNIX)

# ---------------------------------------
# Define install
# ---------------------------------------

IF(LIBSL_USE_INSTALL_PREFIX)
  SET(LIBSL_INSTALL_LIB_DIR "lib")
ELSE()
  SET(LIBSL_INSTALL_LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
ENDIF()

install(TARGETS qhull
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
    LIBRARY DESTINATION ${LIBSL_INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${LIBSL_INSTALL_LIB_DIR}
)

if(NOT WIN32)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libqhull.a" DESTINATION ${LIBSL_INSTALL_LIB_DIR})
endif(NOT WIN32)
