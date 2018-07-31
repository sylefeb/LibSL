CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

################################################################################
# Hides most of the ELF symbols which would have previously (and unnecessarily)
# been public. This is the case by default on Windows, but on Linux we need to
# enable the option when the compiler supports it
################################################################################
Include (CheckCCompilerFlag)
Include (CheckCXXCompilerFlag)

# The magic flag!
Set(FLAG "-fvisibility=hidden")

# C compiler
Check_C_Compiler_Flag("${FLAG}" C_SUPPORTS_${FLAG})
If (C_SUPPORTS_${FLAG})
	Set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
Endif ()

# CXX compiler
Check_CXX_Compiler_Flag("${FLAG}" CXX_SUPPORTS_${FLAG})
If (CXX_SUPPORTS_${FLAG})
	Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
Endif ()
################################################################################

ADD_SUBDIRECTORY(jpeg)

ADD_SUBDIRECTORY(zlib)

ADD_SUBDIRECTORY(png)

ADD_SUBDIRECTORY(loki)

ADD_SUBDIRECTORY(newmat)

ADD_SUBDIRECTORY(lib3ds)

if (LIBSL_BUILD_GLUX)
ADD_SUBDIRECTORY(glux)
endif(LIBSL_BUILD_GLUX)

if (LIBSL_BUILD_GL)

IF(NOT EMSCRIPTEN)
ADD_SUBDIRECTORY(anttweakbar)
ENDIF(NOT EMSCRIPTEN)

else (LIBSL_BUILD_GL)
if (LIBSL_BUILD_GL4)

IF(NOT EMSCRIPTEN)
ADD_SUBDIRECTORY(anttweakbar)
ENDIF(NOT EMSCRIPTEN)

endif(LIBSL_BUILD_GL4)
endif(LIBSL_BUILD_GL)

if (LIBSL_BUILD_LINALG)
ADD_SUBDIRECTORY(OpenNL)
endif (LIBSL_BUILD_LINALG)

ADD_SUBDIRECTORY(tinyxml)

ADD_SUBDIRECTORY(hashlibpp)

if (LIBSL_BUILD_LIBARCHIVE)
ADD_SUBDIRECTORY(libarchive)
endif (LIBSL_BUILD_LIBARCHIVE)

ADD_SUBDIRECTORY(qhull)
