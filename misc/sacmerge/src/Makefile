
include ../../../Makefile.inc

#CC = clang
#CC = gcc

INCLUDE = -I../../../include
CFLAGS = $(INCLUDE)
LIBDIRS = -L../../../lib
LIBS = -lglib -lm
INSTALL_BIN = ../../../bin

all : sacmerge

sacmerge : sacmerge.o version.h
	$(CC) $(CFLAGS) -o $@ sacmerge.o $(LIBDIRS) $(LIBS)
	mv $@ $(INSTALL_BIN)

clean : 
	rm -f *.o sacmerge
