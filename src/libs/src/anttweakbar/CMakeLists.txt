CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(AntTweakBar)

FIND_PACKAGE(OpenGL)

IF(WIN32)

SET(ATB_SOURCES
#  src/LoadOGLCore.cpp
#  TwOpenGLCore.cpp
  src/LoadOGL.cpp
  src/TwBar.cpp
  src/TwColors.cpp
  src/TwEventSDL12.c
  src/TwEventSDL13.c
  src/TwEventSFML.cpp
  src/TwFonts.cpp
  src/TwMgr.cpp
  src/TwOpenGL.cpp
  src/TwEventGLUT.c
#  src/TwPrecomp.cpp
  src/TwEventWin.c
  src/TwDirect3D9.cpp
  src/TwDirect3D10.cpp
#  src/TwDirect3D11.cpp
)

SET(ATB_HEADERS
  src/LoadOGL.h
  src/TwBar.h
  src/TwColors.h
  src/TwDirect3D10.h
  src/TwDirect3D9.h
  src/TwFonts.h
  src/TwMgr.h
  src/TwOpenGL.h
  src/TwPrecomp.h
)

ELSE(WIN32)

# Unix

SET(ATB_SOURCES
#  src/LoadOGLCore.cpp
#  src/TwOpenGLCore.cpp
  src/LoadOGL.cpp
  src/TwBar.cpp
  src/TwColors.cpp
  src/TwEventSDL.c
  src/TwEventSDL12.c
  src/TwEventSDL13.c
  src/TwEventSFML.cpp
  src/TwFonts.cpp
  src/TwMgr.cpp
  src/TwOpenGL.cpp
  src/TwEventGLUT.c
#  src/TwPrecomp.cpp
)

SET(ATB_HEADERS
  src/LoadOGL.h
  src/TwBar.h
  src/TwColors.h
  src/TwFonts.h
  src/TwMgr.h
  src/TwOpenGL.h
  src/TwPrecomp.h
)

ENDIF(WIN32)


INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	$ENV{DXSDK_DIR}/Include
	${OpenGL_INCLUDE_DIRS}
)

ADD_LIBRARY(AntTweakBar STATIC ${ATB_SOURCES} ${ATB_HEADERS})
TARGET_LINK_LIBRARIES(AntTweakBar ${OpenGL_LIBRARIES})

IF(WIN32)
	SET_TARGET_PROPERTIES(AntTweakBar PROPERTIES COMPILE_FLAGS "-DTW_STATIC")
ELSE(WIN32)
IF (EMSCRIPTEN)
	SET_TARGET_PROPERTIES(AntTweakBar PROPERTIES COMPILE_FLAGS "-DTW_STATIC")
ELSE(EMSCRIPTEN)
	  FIND_PACKAGE(X11 REQUIRED)
	  TARGET_LINK_LIBRARIES(AntTweakBar ${X11_LIBRARIES})
    IF(APPLE)
        ADD_DEFINITIONS(-D_MACOSX)
        ADD_DEFINITIONS(-D__PLACEMENT_NEW_INLINE)
        SET_TARGET_PROPERTIES(AntTweakBar PROPERTIES COMPILE_FLAGS "-O3 -arch i386 -arch x86_64 -fno-strict-aliasing -ObjC++")
    ELSE(APPLE)
        SET_TARGET_PROPERTIES(AntTweakBar PROPERTIES COMPILE_FLAGS "-O3 -D_UNIX -D__PLACEMENT_NEW_INLINE")
    ENDIF(APPLE)
ENDIF(EMSCRIPTEN)
ENDIF(WIN32)


SET_TARGET_PROPERTIES(AntTweakBar PROPERTIES DEBUG_POSTFIX "-d")
SET_TARGET_PROPERTIES(AntTweakBar PROPERTIES VERSION "1.2.3")


INSTALL(TARGETS AntTweakBar
  RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
  LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib
  ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib
)

IF(NOT WIN32)
  INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/libAntTweakBar.a" DESTINATION ${CMAKE_SOURCE_DIR}/lib)
ENDIF(NOT WIN32)

