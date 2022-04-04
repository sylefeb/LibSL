################################################################################
CMake_Minimum_Required(VERSION 3.14)
################################################################################

# The method used here is a bit involved and incomplete, so feel free to improve it!

# TODO: Pass the argument to the necessary targets ...

################################################################################

Function(AddCompileDefinition foo def)
	If(CMAKE_VERSION VERSION_LESS 2.8.11)
		Set_Property(TARGET ${foo} APPEND PROPERTY COMPILE_DEFINITIONS "${def}")
		Set_Property(TARGET ${foo} APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG "${def}")
		Set_Property(TARGET ${foo} APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE "${def}")
		Set_Property(TARGET ${foo} APPEND PROPERTY COMPILE_DEFINITIONS_MINSIZEREL "${def}")
		Set_Property(TARGET ${foo} APPEND PROPERTY COMPILE_DEFINITIONS_RELWITHDEBINFO "${def}")
	Else()
		Target_Compile_Definitions(${foo} PRIVATE ${def})
	Endif()
	Message(STATUS "Adding compile defininion ${def} for target ${foo}")
EndFunction(AddCompileDefinition)

################################################################################

# Because CMake sucks major dicks
# http://stackoverflow.com/questions/11783932/how-to-add-linker-or-compile-flag-in-cmake-file
Function(AppendCompileCXXFlag foo flag)

	# Before CMake 3.3 it is not possible to define per-target C++ compile flag
	If(CMAKE_VERSION VERSION_LESS 3.3)
		Get_Target_Property(MY_FLAGS ${foo} COMPILE_FLAGS)
		If(MY_FLAGS STREQUAL "MY_FLAGS-NOTFOUND")
			Set(MY_FLAGS "") # set to empty string
		Else()
			Set(MY_FLAGS "${MY_FLAGS} ") # a space to cleanly separate from existing content
		Endif()

	    Set(MY_FLAGS "${MY_FLAGS}${flag}") # append our value
	    Set_Target_Properties(${foo} PROPERTIES COMPILE_FLAGS ${MY_FLAGS})
	Elseif(NOT MSVC)
		Target_Compile_Options(${foo} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${flag}>)
	Endif()

EndFunction(AppendCompileCXXFlag)

################################################################################

Function(AddTargetCXX11 foo)

	If(NOT TARGET ${foo})
		Message(FATAL_ERROR "${foo} is not a valid target")
	Endif()

	# TODO: Set C Compiler Flags
	Message(STATUS "Setting C++11 compiler flags for target ${foo}")

	Include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
	CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

	Set(CXX11_SUPPORTED FALSE)
	If(COMPILER_SUPPORTS_CXX11)
		AppendCompileCXXFlag(${foo} -std=c++11)
		Set(CXX11_SUPPORTED TRUE)
	Elseif(COMPILER_SUPPORTS_CXX0X)
		AppendCompileCXXFlag(${foo} -std=c++0x)
		Set(CXX11_SUPPORTED TRUE)
	Else()
		If(NOT MSVC OR MSVC_VERSION VERSION_LESS 1800)
			Message(WARNING "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
		Else()
			Set(CXX11_SUPPORTED TRUE)
		Endif()
	Endif()

	If (CXX11_SUPPORTED)
		AddCompileDefinition (${foo} "USE_CXX11")

		Include (CheckCXXCompilerFlag)
		# With C++11, we still get a ton of warning about deprecated stuff
		# We should probably fix them at some point though
		Set(FLAG -Wno-deprecated-declarations)
		Check_CXX_Compiler_Flag("${FLAG}" IS_SUPPORTED_${FLAG})
		If (IS_SUPPORTED_${FLAG})
			AppendCompileCXXFlag(${foo} ${FLAG})
		Endif ()
	Endif ()

EndFunction(AddTargetCXX11)

################################################################################

Function(AddGlobalCXX11)

	# TODO: Set C Compiler Flags
	Message(STATUS "Setting C++11 compiler flags")

	Include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
	CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

	If(COMPILER_SUPPORTS_CXX11)
		Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
		Add_Definitions("-DUSE_CXX11")
	Elseif(COMPILER_SUPPORTS_CXX0X)
		Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
		Add_Definitions("-DUSE_CXX11")
	Else()
		If(NOT MSVC OR MSVC_VERSION VERSION_LESS 1800)
			Message(WARNING "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
		Else()
			Add_Definitions("-DUSE_CXX11")
		Endif()
	Endif()

EndFunction(AddGlobalCXX11)
