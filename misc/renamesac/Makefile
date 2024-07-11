include ../../Makefile.inc

#CC = gcc
INCLUDE = -I../../include
#CFLAGS = -m32 $(INCLUDE)
CFLAGS  = $(INCLUDE)
INSTALL = ../../bin

all : renamesac

renamesac : renamesac.c
	$(CC) $(CFLAGS) -o $@ renamesac.c
	mv $@ $(INSTALL)

clean :
	rm -f renamesac.o 
