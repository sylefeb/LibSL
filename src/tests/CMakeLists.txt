# CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(TestLibSL)

INCLUDE(${CMAKE_SOURCE_DIR}/libsl-config.cmake)

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
  test_sh
)

AUTO_BIND_SHADERS( ${TESTS_SHADERS} )

INCLUDE_DIRECTORIES(..)

ADD_EXECUTABLE(TestLibSL ${TESTS_SOURCES})
TARGET_LINK_LIBRARIES(TestLibSL LibSL)

if(LIBSL_BUILD_GL)
ADD_EXECUTABLE(test_sh test_sh.cpp test_sh.fp test_sh.vp test_sh.h)
TARGET_LINK_LIBRARIES(test_sh LibSL LibSL_gl)
endif(LIBSL_BUILD_GL)

ADD_DEFINITIONS(-DOPENGL -DUSE_GLUX)

INSTALL(TARGETS TestLibSL
	RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
	LIBRARY DESTINATION ${LIBSL_INSTALL_LIB_DIR}
	ARCHIVE DESTINATION ${LIBSL_INSTALL_LIB_DIR}
)
