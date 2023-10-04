CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
PROJECT(jpeg)

SET(LIBJPEG_SOURCES 
		jcapimin.c jcapistd.c jccoefct.c jccolor.c jcdctmgr.c jchuff.c
        jcinit.c jcmainct.c jcmarker.c jcmaster.c jcomapi.c jcparam.c
        jcphuff.c jcprepct.c jcsample.c jctrans.c jdapimin.c jdapistd.c
        jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c jddctmgr.c jdhuff.c
        jdinput.c jdmainct.c jdmarker.c jdmaster.c jdmerge.c jdphuff.c
        jdpostct.c jdsample.c jdtrans.c jerror.c jfdctflt.c jfdctfst.c
        jfdctint.c jidctflt.c jidctfst.c jidctint.c jidctred.c jquant1.c
        jquant2.c jutils.c jmemmgr.c jmemansi.c)
		
SET(LIBJPEG_HEADERS 
		jchuff.h jdhuff.h jdct.h jerror.h jinclude.h jmemsys.h jmorecfg.h
        jpegint.h jpeglib.h jversion.h cdjpeg.h cderror.h transupp.h)
				
ADD_LIBRARY(jpeg STATIC ${LIBJPEG_SOURCES} ${LIBJPEG_HEADERS})

SET_TARGET_PROPERTIES(jpeg PROPERTIES DEBUG_POSTFIX "-d")
SET_TARGET_PROPERTIES(jpeg PROPERTIES VERSION "6b")

IF(LIBSL_USE_INSTALL_PREFIX)
	SET(LIBSL_INSTALL_LIB_DIR "lib")
ELSE()
	SET(LIBSL_INSTALL_LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
ENDIF()

INSTALL(TARGETS jpeg
  RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
  LIBRARY DESTINATION ${LIBSL_INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${LIBSL_INSTALL_LIB_DIR}
)
IF(NOT WIN32)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/libjpeg.a" DESTINATION ${LIBSL_INSTALL_LIB_DIR})
ENDIF(NOT WIN32)

