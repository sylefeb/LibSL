
IF(WIN32)
    IF(BUILD_SHARED_LIBS)
        ADD_DEFINITIONS(-DNL_EXPORTS)
        ADD_DEFINITIONS(-DNL_SHARED_LIBS)
    ENDIF(BUILD_SHARED_LIBS)
ENDIF(WIN32)

AUX_SOURCE_DIRECTORY(NL SOURCES)

# extensions
IF(USE_CNC)
    AUX_SOURCE_DIRECTORY(plugins/cnc SOURCES)
    set(SOURCES ${SOURCES};plugins/cnc/cnc_cuda_blas.cu)
ENDIF(USE_CNC)

IF(USE_ATLAS)
    ADD_DEFINITIONS(-DNL_USE_ATLAS)
ENDIF(USE_ATLAS)


IF(USE_SUPERLU)
    ADD_DEFINITIONS(-DNL_USE_SUPERLU)
    
    SET(SUPERLU_INCS ${SUPERLU_HOME}/SRC)
    
    INCLUDE_DIRECTORIES(${SUPERLU_INCS})
ENDIF(USE_SUPERLU)


# creation of the library
IF(USE_CNC)
    IF(WIN32)
	    CUDA_ADD_LIBRARY(nl ${SOURCES} ${HeaderFiles})
    ELSE(WIN32)
        CUDA_ADD_LIBRARY(nl ${SOURCES} )
        set_target_properties(nl PROPERTIES COMPILE_FLAGS "-malign-double")
    ENDIF(WIN32)
    CUDA_ADD_CUBLAS_TO_TARGET(nl)
ELSE(USE_CNC)
    IF(WIN32)
	    ADD_LIBRARY(nl ${SOURCES} ${HeaderFiles})
    ELSE(WIN32)
	    ADD_LIBRARY(nl ${SOURCES} )
    ENDIF(WIN32)
ENDIF(USE_CNC)


SET_TARGET_PROPERTIES(nl PROPERTIES DEBUG_POSTFIX "-d")

IF(LIBSL_USE_INSTALL_PREFIX)
  SET(LIBSL_INSTALL_LIB_DIR "lib")
ELSE()
  SET(LIBSL_INSTALL_LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
ENDIF()

INSTALL(TARGETS nl
  RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
  LIBRARY DESTINATION ${LIBSL_INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${LIBSL_INSTALL_LIB_DIR}
)
