CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(AutoBindShader_tutorials)

SET(LibSL_DIR $ENV{LIBSL_DIR})
FIND_PACKAGE(LibSL)

INCLUDE_DIRECTORIES( ${LIBSL_INCLUDE_DIR} )

ADD_EXECUTABLE(AutoBindShader_gl_tutorial0 
	AutoBindShader_gl_tutorial0.cpp 
	shader_tutorial0.fp
	shader_tutorial0.h)
TARGET_LINK_LIBRARIES(AutoBindShader_gl_tutorial0 ${LIBSL_GL_LIBRARIES} ${LIBSL_LIBRARIES})

ADD_EXECUTABLE(AutoBindShader_gl_tutorial1_geometryshader 
	AutoBindShader_gl_tutorial1_geometryshader.cpp 
	shader_tutorial1.vp 
	shader_tutorial1.gs 
	shader_tutorial1.fp 
	shader_tutorial1.h)
TARGET_LINK_LIBRARIES(AutoBindShader_gl_tutorial1_geometryshader ${LIBSL_GL_LIBRARIES} ${LIBSL_LIBRARIES})

ADD_EXECUTABLE(AutoBindShader_gl_tutorial2_anttweakbar
	AutoBindShader_gl_tutorial2_anttweakbar.cpp 
	shader_tutorial2.fp 
	shader_tutorial2.h)
TARGET_LINK_LIBRARIES(AutoBindShader_gl_tutorial2_anttweakbar ${LIBSL_GL_LIBRARIES} ${LIBSL_LIBRARIES})

IF(WIN32)
INSTALL(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/release/AutoBindShader_gl_tutorial0.exe" DESTINATION ${CMAKE_SOURCE_DIR}/bin)
ELSE(WIN32)
INSTALL(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/AutoBindShader_gl_tutorial0" DESTINATION ${CMAKE_SOURCE_DIR}/bin)
ENDIF(WIN32)

SET(SHADERS
	shader_tutorial0
	shader_tutorial1
	shader_tutorial2
)

AUTO_BIND_SHADERS( ${SHADERS} )

