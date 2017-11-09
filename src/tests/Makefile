CC	  = g++

LIBRARY_PATH = $(HOME)/LIBRARIES
LIBSL_PATH   = $(HOME)/LIBRARIES/LibSL
LOKI_PATH    = $(HOME)/LIBRARIES/loki

CFLAGS  = -O5 -DUNIX -I/usr/local/include -I$(LIBSL_PATH)/src/ -I$(LOKI_PATH)/include
LDFLAGS = -L/usr/lib -L$(LIBSL_PATH)/lib -lloki -lpng -ljpeg -l3ds -lm  $(LIBSL_PATH)/lib/libSL.a

SRC_COMMON = \
	TestLibSL.cpp \
	test_memory_perf.cpp \
	test_memory.cpp \
	test_image.cpp \
	test_math.cpp \
	test_system.cpp \
	test_lloyd.cpp \
	test_graph.cpp \
	test_mesh.cpp \
	test_datastructures.cpp

all   : TestLibSL

TestLibSL : $(SRC_COMMON:.cpp=.o)
	$(CC) $(CFLAGS) $(addprefix Linux.BUILD/,$(SRC_COMMON:.cpp=.o)) $(LDFLAGS) -o ../../bin/TestLibSL
	sync

clean	:
		rm -f *.o *~ core core.*
		rm -rf ii_files

.SUFFIXES : .c .cpp .h .o

.cpp.o    :
	$(CC) $(CFLAGS) -c $< -o Linux.BUILD/$@
.c.o    :
	$(CC) $(CFLAGS) -c $<
