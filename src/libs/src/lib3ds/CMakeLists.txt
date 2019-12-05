CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(3ds)

SET(LIB3DS_SOURCES 
src/lib3ds_atmosphere.c
src/lib3ds_background.c
src/lib3ds_camera.c
src/lib3ds_chunk.c
src/lib3ds_chunktable.c
src/lib3ds_file.c
src/lib3ds_io.c
src/lib3ds_light.c
src/lib3ds_material.c
src/lib3ds_math.c
src/lib3ds_matrix.c
src/lib3ds_mesh.c
src/lib3ds_node.c
src/lib3ds_quat.c
src/lib3ds_shadow.c
src/lib3ds_track.c
src/lib3ds_util.c
src/lib3ds_vector.c
src/lib3ds_viewport.c
)
		
SET(LIB3DS_HEADERS 
src/lib3ds.h
src/lib3ds_impl.h
)
		
ADD_DEFINITIONS( -DLIB3DS_EXPORTS )
	
ADD_LIBRARY(3ds STATIC ${LIB3DS_SOURCES} ${LIB3DS_HEADERS})

SET_TARGET_PROPERTIES(3ds PROPERTIES DEBUG_POSTFIX "-d")

INSTALL(TARGETS 3ds
  RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
  LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib
  ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib
)
