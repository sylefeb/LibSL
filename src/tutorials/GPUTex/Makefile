LIBRARY_PATH    = $(HOME)/lib.linux
GLUX_PATH       = $(LIBRARY_PATH)/glux
LOKI_PATH       = $(LIBRARY_PATH)/loki
LIBSL_PATH      = $(LIBRARY_PATH)/LibSL
BIN_PATH	= $(LIBRARY_PATH)/LibSL/bin

CC	= g++
CFLAGS  = -g -I/usr/local/include \
          -I$(GLUX_PATH)/include \
          -I$(LOKI_PATH)/include \
          -I$(LIBSL_PATH)/src \
          -I$(LIBSL_PATH)/src/LibSL/GPUMesh \
          -DOPENGL               \
	  -DUSE_GLUX
LDFLAGS = -L/usr/lib -L/usr/X11R6/lib -L$(GLUX_PATH)/lib -L$(LIBSL_PATH)/lib -l3ds -lSL -lSL_gl -ljpeg -lglux -lglut -L -lGL -lGLU -lpthread -lXt -lXi -lXmu -lX11 -lm

all : GPUTex_tutorial0 GPUTex_tutorial1 GPUTex_tutorial2

GPUTex_tutorial0 : GPUTex_tutorial0.o
	$(CC) $(CFLAGS) -g GPUTex_tutorial0.o $(LDFLAGS) -o $(BIN_PATH)/GPUTex_tutorial0

GPUTex_tutorial1 : GPUTex_tutorial1.o
	$(CC) $(CFLAGS) -g GPUTex_tutorial1.o $(LDFLAGS) -o $(BIN_PATH)/GPUTex_tutorial1

GPUTex_tutorial2 : GPUTex_tutorial2.o
	$(CC) $(CFLAGS) -g GPUTex_tutorial2.o $(LDFLAGS) -o $(BIN_PATH)/GPUTex_tutorial2

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
