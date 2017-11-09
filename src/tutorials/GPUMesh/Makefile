LIBRARY_PATH    = $(HOME)/lib.linux
GLUX_PATH       = $(LIBRARY_PATH)/glux
LOKI_PATH       = $(LIBRARY_PATH)/loki
LIBSL_PATH      = $(LIBRARY_PATH)/LibSL
BIN_PATH	= $(LIBRARY_PATH)/LibSL/bin
GPUMESH_PATH    = $(LIBRARY_PATH)/LibSL/src/LibSL/GPUMesh

CC	= g++
CFLAGS  = -g -I/usr/local/include -I$(GLUX_PATH)/include -I$(LOKI_PATH)/include -I$(GPUMESH_PATH) -DUSE_GLUX
LDFLAGS = -L/usr/lib -L/usr/X11R6/lib -L$(GLUX_PATH)/lib -ljpeg -lglux -lglut -l3ds -lGL -lGLU -lXt -lXi -lXmu -lX11 -lm

all : GPUMesh_gl tutorial0_gl tutorial1_gl tutorial2_gl

GPUMesh_gl : GPUMesh_gl.o
	$(CC) $(CFLAGS) -g GPUMesh_gl.o $(LDFLAGS) -o $(BIN_PATH)/GPUMesh_gl

tutorial0_gl : tutorial0_gl.o
	$(CC) $(CFLAGS) -g tutorial0_gl.o $(LDFLAGS) -o $(BIN_PATH)/GPUMesh_tutorial0_gl

tutorial1_gl : tutorial1_gl.o
	$(CC) $(CFLAGS) -g tutorial1_gl.o $(LDFLAGS) -o $(BIN_PATH)/GPUMesh_tutorial1_gl

tutorial2_gl : tutorial2_gl.o
	$(CC) $(CFLAGS) -g tutorial2_gl.o $(LDFLAGS) -o $(BIN_PATH)/GPUMesh_tutorial2_gl

clean	:
		rm -f *.o *~
		rm -rf ii_files

edit	:
		emacs $(SRC)

.SUFFIXES : .c .cpp .h .o

.cpp.o    :
	$(CC)  $(CFLAGS) -c $<
.c.o    :
	$(CC)  $(CFLAGS) -c $<
