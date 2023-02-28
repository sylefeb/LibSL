CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(newmat)

SET(NEWMAT_SOURCES 
bandmat.cpp
cholesky.cpp
evalue.cpp
#example.cpp
fft.cpp
hholder.cpp
jacobi.cpp
myexcept.cpp
newfft.cpp
newmat1.cpp
newmat2.cpp
newmat3.cpp
newmat4.cpp
newmat5.cpp
newmat6.cpp
newmat7.cpp
newmat8.cpp
newmat9.cpp
newmatex.cpp
newmatnl.cpp
newmatrm.cpp
solution.cpp
sort.cpp
submat.cpp
svd.cpp
)
		
SET(NEWMAT_HEADERS 
boolean.h
controlw.h
include.h
myexcept.h
newmat.h
newmatap.h
newmatio.h
newmatnl.h
newmatrc.h
newmatrm.h
precisio.h
solution.h
)
		
ADD_LIBRARY(newmat STATIC ${NEWMAT_SOURCES} ${NEWMAT_HEADERS})

SET_TARGET_PROPERTIES(newmat PROPERTIES DEBUG_POSTFIX "-d")

INSTALL(TARGETS newmat
  RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
  LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib
  ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib
)
