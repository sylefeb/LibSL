CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(gl4xcore_tutorials)

SET(LibSL_DIR $ENV{LIBSL_DIR})
FIND_PACKAGE(LibSL)

INCLUDE_DIRECTORIES( ${LIBSL_INCLUDE_DIR} )

ADD_EXECUTABLE(gl4xcore_gl_tutorial0 
	gl4xcore_gl_tutorial0.cpp 
	gl4xcore_gl_tutorial0.fp 
	gl4xcore_gl_tutorial0.vp
	gl4xcore_gl_tutorial0.h)
TARGET_LINK_LIBRARIES(gl4xcore_gl_tutorial0 ${LIBSL_GL4CORE_LIBRARIES} ${LIBSL_LIBRARIES})

SET(SHADERS
	gl4xcore_gl_tutorial0
)

AUTO_BIND_SHADERS( ${SHADERS} )
