################################################################################
CMake_Minimum_Required(VERSION 2.8.0)
################################################################################

# TODO: Set C Compiler Flags

Message(STATUS "Setting C++11 compiler flags")

Include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

If(COMPILER_SUPPORTS_CXX11)
	Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
Elseif(COMPILER_SUPPORTS_CXX0X)
	Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
Else()
	If(NOT MSVC OR MSVC_VERSION VERSION_LESS 1800)
		Message(WARNING "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
	Endif()
Endif()
