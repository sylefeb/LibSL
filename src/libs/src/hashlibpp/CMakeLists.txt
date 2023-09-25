CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(hashlibpp)

SET(LIBH_SOURCES 
src/hl_md5.cpp
src/hl_md5wrapper.cpp
src/hl_sha1.cpp
src/hl_sha1wrapper.cpp
src/hl_sha256.cpp
src/hl_sha256wrapper.cpp
src/hl_sha2ext.cpp
src/hl_sha384wrapper.cpp
src/hl_sha512wrapper.cpp
src/hl_wrapperfactory.cpp
)
		
SET(LIBH_HEADERS 
src/hashlibpp.h
src/hl_exception.h
src/hl_hashwrapper.h
src/hl_md5.h
src/hl_md5wrapper.h
src/hl_sha1.h
src/hl_sha1wrapper.h
src/hl_sha256.h
src/hl_sha256wrapper.h
src/hl_sha2ext.h
src/hl_sha2mac.h
src/hl_sha384wrapper.h
src/hl_sha512wrapper.h
src/hl_types.h
src/hl_wrapperfactory.h
)
				
ADD_LIBRARY(hashlibpp STATIC ${LIBH_SOURCES} ${LIBH_HEADERS})

SET_TARGET_PROPERTIES(hashlibpp PROPERTIES DEBUG_POSTFIX "-d")
# SET_TARGET_PROPERTIES(hashlibpp PROPERTIES VERSION "6b")

IF(LIBSL_USE_INSTALL_PREFIX)
  SET(LIBSL_INSTALL_LIB_DIR "lib")
ELSE()
  SET(LIBSL_INSTALL_LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
ENDIF()

INSTALL(TARGETS hashlibpp
  RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
  LIBRARY DESTINATION ${LIBSL_INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${LIBSL_INSTALL_LIB_DIR}
)
IF(NOT WIN32)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/libhashlibpp.a" DESTINATION ${LIBSL_INSTALL_LIB_DIR})
ENDIF(NOT WIN32)

