FIND_PACKAGE(PythonInterp)

IF(NOT PYTHONINTERP_FOUND)
	MESSAGE(FATAL_ERROR "[AutoBindShader] Python not found!")
ENDIF(NOT PYTHONINTERP_FOUND)

SET(PYTHON3_EXECUTABLE ${PYTHON_EXECUTABLE} CACHE INTERNAL "")

#FIND_PROGRAM(PYTHON3_EXECUTABLE
#  NAMES python.exe
#  PATHS
#  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\3.0\\InstallPath]
#  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\3.1\\InstallPath]
#  )

IF (NOT AutoBindShader_DIR)
SET(AutoBindShader_DIR ${LibSL_DIR}/src/tools/autobindshader CACHE INTERNAL "")
ENDIF (NOT AutoBindShader_DIR)

MACRO(AUTO_BIND_SHADERS)
	INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
	FOREACH (SHADER ${ARGN})
		IF(IS_ABSOLUTE ${SHADER})
			FILE(GLOB inputs ${SHADER}.* )
		ELSE()
			FILE(GLOB inputs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}.* )
			SET (ABS_TARGET_BINARY_SHADER ${CMAKE_CURRENT_BINARY_DIR}/${SHADER}.h)
		ENDIF()
		SEPARATE_ARGUMENTS(inputs)
		MESSAGE(STATUS "SHADER FILES : ${inputs}")

		IF (IS_ABSOLUTE ${SHADER})
			INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
		ELSE ()
			# Create a subdirectory if it does not exists already
			IF(CMAKE_VERSION VERSION_LESS 2.8.12)
				GET_FILENAME_COMPONENT(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${SHADER}.h PATH)
			ELSE()
				GET_FILENAME_COMPONENT(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${SHADER}.h DIRECTORY)
			ENDIF()
			FILE(MAKE_DIRECTORY "${OUTPUT_DIR}")
			INCLUDE_DIRECTORIES( ${OUTPUT_DIR} )
		ENDIF ()

		IF(IS_ABSOLUTE ${SHADER})
			GET_FILENAME_COMPONENT(OUTPUT_FILE ${SHADER}.h NAME)
			ADD_CUSTOM_COMMAND(
							OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FILE}
							COMMAND ${PYTHON3_EXECUTABLE} ${AutoBindShader_DIR}/autobindshader.py "${SHADER}.fp"
							DEPENDS ${inputs}
							WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
							COMMENT "AutoBindShader")
		ELSE()
			ADD_CUSTOM_COMMAND(
							OUTPUT ${SHADER}.h
							COMMAND ${PYTHON3_EXECUTABLE} ${AutoBindShader_DIR}/autobindshader.py "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}.fp"
							DEPENDS ${inputs}
							WORKING_DIRECTORY ${OUTPUT_DIR}
							COMMENT "AutoBindShader")
		ENDIF()
		MESSAGE(STATUS "Adding rule for AutoBindShader <${SHADER}>")
	ENDFOREACH (SHADER)
ENDMACRO(AUTO_BIND_SHADERS)


function(target_bind_shaders TARGET_NAME)
	foreach(SHADER_FILE ${ARGN})
		get_filename_component(SHADER_BASE ${SHADER_FILE} NAME)
		file(GLOB INPUTS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}.*)

		# Log message
		set(INPUTS_BASE "")
		foreach(INPUT ${INPUTS})
			get_filename_component(RES ${INPUT} NAME)
			set(INPUTS_BASE ${INPUTS_BASE} ${RES})
		endforeach()
		message(STATUS "Binding shader (${SHADER_BASE}): ${INPUTS_BASE}")

		# Custom build command
		set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})
		set(OUTPUT_FILE ${OUTPUT_DIR}/${SHADER_BASE}.h)

		# Be careful with custom commands:
		# https://samthursfield.wordpress.com/2015/11/21/cmake-dependencies-between-targets-and-files-and-custom-commands/
		add_custom_command(
			OUTPUT ${OUTPUT_FILE}
			COMMAND ${PYTHON3_EXECUTABLE} ${AutoBindShader_DIR}/autobindshader.py "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}.fp"
			DEPENDS ${INPUTS}
			WORKING_DIRECTORY ${OUTPUT_DIR}
			COMMENT "AutoBindShader"
		)
		add_custom_target(
			${TARGET_NAME}_${SHADER_BASE}
			DEPENDS ${OUTPUT_FILE}
		)
		set_source_files_properties(${OUTPUT_FILE} PROPERTIES GENERATED TRUE)
		add_dependencies(${TARGET_NAME} ${TARGET_NAME}_${SHADER_BASE})
		target_include_directories(${TARGET_NAME} PUBLIC ${OUTPUT_DIR})

		# Organize shader targets in folders
		set_property(GLOBAL PROPERTY USE_FOLDERS ON)
		set_target_properties(${TARGET_NAME}_${SHADER_BASE} PROPERTIES FOLDER "${TARGET_NAME}_shaders")
	endforeach ()
endfunction ()
