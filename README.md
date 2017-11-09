# LibSL - v1.11 - README - 2012-11-29 - Sylvain Lefebvre

# What is LibSL?
 
A simple C++ toolbox to simplify many of the basic tasks when doing research in Computer Graphics. It features simple, very straightforward image and mesh manipulation, texture and render targets, a simple UI with a trackball, an OpenGL extension loader, and many little things here and there. It supports several graphics API on several platforms, even though lately the focus has been essentially on OpenGL. The package also contains several other great libraries (libng, libjpg, AntTweakBar, OpenNL, sqlite, tinyXML, to name a few) with which LibSL easily interacts.

You won't be impressed, and you probably don't need it. The point is, most of my code relies on it and over the years it ended up being used by several students, colleagues and collaborators, so it seemed a good idea to make it easily accessible.

LibSL is primarily developed by Sylvain Lefebvre for his research work at INRIA and is under a Cecill-C licence.

# Disclaimer

IMPORTANT If you are downloading LibSL, it probably means we work together, you can skip the rest of this paragraph. If not, please be warned that I do not provide support for LibSL. Similarly, I will probably not answer any emails regarding setup, install, usage, etc.. It is here for your enjoyment and convenience but I do not have time to turn it into a nice, well coded and well documented library. It is messy, slow, badly designed -- but it works for me. Oh, and there is no documentation.

# Some history
 
LibSL was designed as a support library for doing shader intensive programming. Its primary design goals are simplicity of use and maintenance, easy switch between GL/DirectX, error checking, and non--intrusion. It is slow by design (who cares about CPU performance anyway when we have GPUs? (joking, of course!) ). It can be bypassed at all times by API-specific functions. There is no wrapper: Different graphics API are supported via template policies. This is likely not the best approach, but it works for me. Its development started back in 2006 and continues today. LibSL has been used in many successful research projects published at SIGGRAPH and EUROGRAPHICS.

# Contributors
 
Former students have suffered LibSL and went as far as contributing to it:

* Christian Eisenacher (image filtering, PCA)
* Matthäus Chajdas (voxel stuff, of course :), he also inspired the cmake build, the use of sqlite, etc. ) 
* Jérémie Dumas (many improvements, in particular to tuples)
    
# Compiling LibSL:
================

# Windows:
----------

- **IMPORTANT** Setup the environment variable LibSL_DIR to your LibSL root directory
- Make sure you have Visual C++ installed, or any other compiler supported by CMake
- [directx only] Install latest Microsoft DirectX SDK. Make sure the DirectX include and lib
  directories are in your VisualC++ paths ( $(DXSDK_DIR)\Include and $(DXSDK_DIR)\Lib\x86 ).
- [directx only] Compile DXUT in $(DXSDK_DIR)\Samples\C++\DXUT\Core\
- Download and install Perl from http://www.activestate.com/activeperl/
- Download and install Python 3
- Use CMake-gui to generate LibSL. Select desired builds
	- select DX only if the SDK is installed
	- GL4 will only work if you have GL4 compliant hardware and drivers
- Build the INSTALL project
- All libraries are now in %LibSL_DIR%\lib

# Linux:
--------

Use CMake. Do *not* use any left over Makefile in the LibSL source tree, these will be removed soon.

Install Perl, Python 2.7, X11 development libraries (freeglut, libxmu, libxi and libx11).

Make sure you do not have Mesa drivers (see header of /usr/include/GL/gl.h, there should be no mention
of Mesa there). On NVidia cards, install the nvidia-glx-XXX-dev package to get the headers (where XXX
is the driver version, for instance 180).

Type "make install" in the build tree. Libraries are copied into LibSL/lib/*

(LibSL was initally developped with g++ (GCC) 4.0.2, it is tested with gcc 4.3.3)

# Using LibSL:
============

*** Setup the environment variable LibSL_DIR to your LibSL root directory ***

To create a new project using LibSL and for instance LibSL_gl, use the following CmakeLists.txt template:

	CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
	PROJECT(myproject)

	FIND_PACKAGE(LibSL)

	SET(SOURCES
		main.cpp
	)

	SET(HEADERS
		main.h
	)

	INCLUDE_DIRECTORIES( ${LIBSL_INCLUDE_DIR} )

	ADD_EXECUTABLE(myexe ${SOURCES} ${HEADERS})
	TARGET_LINK_LIBRARIES(myexe ${LIBSL_LIBRARIES} ${LIBSL_GL_LIBRARIES})

# Documentation:
================

None at the time, but several tutorial are available in LibSL/src/tutorials
Tutorials binaries are compiled into LibSL/bin and meant to be executed
from there.

# Licence:
========

LibSL is distributed under the CeCILL-C licence (http://www.cecill.info).
See Licence_CeCILL-C_V1-en.txt for the complete licence text.

--
LibSL - (c) Sylvain Lefebvre 2006-2012
