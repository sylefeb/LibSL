CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(tinyxml)

SET(TINYXML_SOURCES 
	tinystr.cpp
	tinyxml.cpp
	tinyxmlerror.cpp
	tinyxmlparser.cpp
	)
		
SET(TINYXML_HEADERS 
	tinystr.h
	tinyxml.h
	)
				
ADD_LIBRARY(tinyxml STATIC ${TINYXML_SOURCES} ${TINYXML_HEADERS})

SET_TARGET_PROPERTIES(tinyxml PROPERTIES DEBUG_POSTFIX "-d")
SET_TARGET_PROPERTIES(tinyxml PROPERTIES VERSION "2.6.1")

IF(LIBSL_USE_INSTALL_PREFIX)
  SET(LIBSL_INSTALL_LIB_DIR "lib")
ELSE()
  SET(LIBSL_INSTALL_LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
ENDIF()

INSTALL(TARGETS tinyxml
  RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
  LIBRARY DESTINATION ${LIBSL_INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${LIBSL_INSTALL_LIB_DIR}
)

IF(NOT WIN32)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/libtinyxml.a" DESTINATION ${LIBSL_INSTALL_LIB_DIR})
ENDIF(NOT WIN32)
