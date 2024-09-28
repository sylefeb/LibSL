# CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(TestLibSL)

INCLUDE(${LibSL_DIR}/libsl-config.cmake)

INCLUDE_DIRECTORIES(
  ${LIBSL_INCLUDE_DIR}
)
LINK_DIRECTORIES(
  ${LIBSL_BINARY_DIR}
)

SET(TESTS_SOURCES
TestLibSL.cpp
# test_aab.cpp
# test_brush.cpp
# test_datastructures.cpp
# test_graph.cpp
# test_hermitcurve.cpp
# test_image.cpp
# test_lloyd.cpp
# test_math.cpp
test_memory.cpp
# test_memory_perf.cpp
# test_mesh.cpp
# test_polygon.cpp
# test_quadtree.cpp
# test_system.cpp
# test_contour.cpp
)

SET(TESTS_HEADERS
)

SET(TESTS_SHADERS
  test_sh_gl
  test_sh_gl4core
)

AUTO_BIND_SHADERS( ${TESTS_SHADERS} )

INCLUDE_DIRECTORIES(..)

ADD_EXECUTABLE(TestLibSL ${TESTS_SOURCES})
TARGET_LINK_LIBRARIES(TestLibSL LibSL)

if(LIBSL_BUILD_GL4CORE)
ADD_EXECUTABLE(test_sh_gl4core test_sh_gl4core.cpp test_sh_gl4core.fp test_sh_gl4core.vp test_sh_gl4core.h)
TARGET_LINK_LIBRARIES(test_sh_gl4core LibSL LibSL_gl4core)
SET_TARGET_PROPERTIES(test_sh_gl4core PROPERTIES COMPILE_DEFINITIONS "USE_GLUX;OPENGL4;OPENGLCORE")
endif(LIBSL_BUILD_GL4CORE)

if(LIBSL_BUILD_GL4CORE)
ADD_EXECUTABLE(test_sh_gl test_sh_gl.cpp test_sh_gl.fp test_sh_gl.vp test_sh_gl.h)
TARGET_LINK_LIBRARIES(test_sh_gl LibSL LibSL_gl)
SET_TARGET_PROPERTIES(test_sh_gl PROPERTIES COMPILE_DEFINITIONS "USE_GLUX;OPENGL")
endif(LIBSL_BUILD_GL4CORE)

INSTALL(TARGETS TestLibSL
	RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
	LIBRARY DESTINATION ${LIBSL_INSTALL_LIB_DIR}
	ARCHIVE DESTINATION ${LIBSL_INSTALL_LIB_DIR}
)
