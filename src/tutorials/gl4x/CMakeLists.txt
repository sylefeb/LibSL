CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(gl4x_tutorials)

SET(LibSL_DIR $ENV{LIBSL_DIR})
FIND_PACKAGE(LibSL)

INCLUDE_DIRECTORIES( ${LIBSL_INCLUDE_DIR} )

ADD_EXECUTABLE(gl4x_gl_tutorial0 
	gl4x_gl_tutorial0.cpp 
	gl4x_gl_tutorial0.fp 
	gl4x_gl_tutorial0.vp
	gl4x_gl_tutorial0.h)
TARGET_LINK_LIBRARIES(gl4x_gl_tutorial0 ${LIBSL_GL4_LIBRARIES} ${LIBSL_LIBRARIES})

# ADD_EXECUTABLE(gl4x_gl_tutorial1
# 	gl4x_gl_tutorial1.cpp 
# 	gl4x_gl_tutorial1.cs 
# 	gl4x_gl_tutorial1.h)
# TARGET_LINK_LIBRARIES(gl4x_gl_tutorial1 ${LIBSL_GL4_LIBRARIES} ${LIBSL_LIBRARIES})

IF(WIN32)
INSTALL(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/release/gl4x_gl_tutorial0.exe" DESTINATION ${CMAKE_SOURCE_DIR}/bin)
INSTALL(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/release/gl4x_gl_tutorial1.exe" DESTINATION ${CMAKE_SOURCE_DIR}/bin)
ELSE(WIN32)
INSTALL(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/gl4x_gl_tutorial0" DESTINATION ${CMAKE_SOURCE_DIR}/bin)
INSTALL(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/gl4x_gl_tutorial1" DESTINATION ${CMAKE_SOURCE_DIR}/bin)
ENDIF(WIN32)

SET(SHADERS
	gl4x_gl_tutorial0
# 	gl4x_gl_tutorial1
)

AUTO_BIND_SHADERS( ${SHADERS} )
