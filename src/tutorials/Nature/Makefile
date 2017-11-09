LIBRARY_PATH    = $(HOME)/lib.linux
GLUX_PATH       = $(LIBRARY_PATH)/glux
LOKI_PATH       = $(LIBRARY_PATH)/loki
LIBSL_PATH      = $(LIBRARY_PATH)/LibSL
BIN_PATH	= $(LIBRARY_PATH)/LibSL/bin

CC	= g++
CFLAGS  = -g                     \
	  -DUSE_GLUX             \
          -I/usr/local/include   \
	  -I$(GLUX_PATH)/include \
          -I$(LOKI_PATH)/include \
          -I$(LIBSL_PATH)/src    \
          -I$(LIBSL_PATH)/src/LibSL/GPUMesh

LDFLAGS = -L/usr/lib -L/usr/X11R6/lib -L$(GLUX_PATH)/lib -L$(LIBSL_PATH)/lib -lSL -lSL_gl -l3ds -ljpeg -lglux -lglut -L -lGL -lGLU -lpthread -lXt -lXi -lXmu -lX11 -lm

all : Nature_tutorial_Sky0 Nature_tutorial_Sky1 Nature_tutorial_Sky2

Nature_tutorial_Sky0 : Nature_tutorial_Sky0.o
	$(CC) $(CFLAGS) -g Nature_tutorial_Sky0.o $(LDFLAGS) -o $(BIN_PATH)/Nature_tutorial_Sky0

Nature_tutorial_Sky1 : Nature_tutorial_Sky1.o
	$(CC) $(CFLAGS) -g Nature_tutorial_Sky1.o $(LDFLAGS) -o $(BIN_PATH)/Nature_tutorial_Sky1

Nature_tutorial_Sky2 : Nature_tutorial_Sky2.o
	$(CC) $(CFLAGS) -g Nature_tutorial_Sky2.o $(LDFLAGS) -o $(BIN_PATH)/Nature_tutorial_Sky2

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
