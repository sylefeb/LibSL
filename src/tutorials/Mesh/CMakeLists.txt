CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Mesh_tutorial)

SET(LibSL_DIR $ENV{LIBSL_DIR})
FIND_PACKAGE(LibSL)

INCLUDE_DIRECTORIES( ${LIBSL_INCLUDE_DIR} )

ADD_DEFINITIONS(-DOPENGL)

ADD_EXECUTABLE(Mesh_tutorial0 Mesh_tutorial0.cpp)
TARGET_LINK_LIBRARIES(Mesh_tutorial0 ${LIBSL_GL_LIBRARIES} ${LIBSL_LIBRARIES})



IF(WIN32)
INSTALL(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/release/Mesh_tutorial0.exe" DESTINATION ${CMAKE_SOURCE_DIR}/bin)
ELSE(WIN32)
INSTALL(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/Mesh_tutorial0" DESTINATION ${CMAKE_SOURCE_DIR}/bin)
ENDIF(WIN32)

