CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(zlib)

SET(ZLIB_SOURCES 
		adler32.c   gzio.c      
		inftrees.c  uncompr.c
		compress.c  
		zutil.c
		crc32.c     
		deflate.c   inffast.c  
		inflate.c   trees.c)
		
SET(ZLIB_HEADERS 
		crc32.h deflate.h inffast.h inffixed.h
		inftrees.h trees.h zlib.h
		zutil.h zconf.h)
				
ADD_LIBRARY(zlib STATIC ${ZLIB_SOURCES} ${ZLIB_HEADERS})

SET_TARGET_PROPERTIES(zlib PROPERTIES DEBUG_POSTFIX "-d")
SET_TARGET_PROPERTIES(zlib PROPERTIES VERSION "1.2.3")

INSTALL(TARGETS zlib
  RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
  LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib
  ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib
)
IF(NOT WIN32)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/libzlib.a" DESTINATION ${CMAKE_SOURCE_DIR}/lib)
ENDIF(NOT WIN32)

