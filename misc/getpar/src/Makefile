#CC = gcc
#CC = clang

include ../../../Makefile.inc

#CFLAGS = -Wall -Wunused-function
CFLAGS = 
RM = /bin/rm
MV = /bin/mv
#RANLIB = /usr/bin/ranlib
RANLIB = ranlib
INSTALL = ../../../lib

all : libget

.c.o :
	$(CC) $(CFLAGS) -c $< -o $@

defpar.o : defpar.c
getpar.o : getpar.c
getarg.o : getarg.c
getlocation.o : getlocation.c

# fgetpar.o : getpar.c
#	cp getpar.c fgetpar.c
#	$(CC) $(CFLAGS) -DFORTRAN -c fgetpar.c
#	$(RM) -f fgetpar.c

# fgetarg.o : getarg.c
#	cp getarg.c fgetarg.c
#	$(CC) $(CFLAGS) -DFORTRAN -c fgetarg.c
#	$(RM) -f fgetarg.c

# OBJS = defpar.o getpar.o getarg.o getlocation.o fgetpar.o fgetarg.o
OBJS = defpar.o getpar.o getarg.o getlocation.o

libget : $(OBJS) 
	ar cq libget.a $(OBJS)
	$(MV) libget.a $(INSTALL)
	$(RANLIB) $(INSTALL)/libget.a

clean :
	rm -rf defpar.o getpar.o getarg.o getlocation.o fgetpar.o fgetarg.o libget.a core

