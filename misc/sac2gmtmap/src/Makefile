#CC = gcc 
include ../../../Makefile.inc

INCLUDE = ../../../include
CFLAGS = -I$(INCLUDE) 
LIBDIRS = -L../../../lib
LIBS = -lm -lget
# INSTALL = $(HOME)/bin
INSTALL = ../../../bin

.c.o :
	$(CC) $(CFLAGS) -c $< -o $@

all : sac2gmtmap stats

OBJS = sac2gmtmap.o ../../../src/sacio/sacio.o ../../../src/timesubs.o ../../../src/misc_tools/distaz.o

sac2gmtmap : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBDIRS) $(LIBS)
	mv $@ $(INSTALL)

stats : stats.o stats_subs.o
	$(CC) $(CFLAGS) -o $@ stats.o stats_subs.o $(LIBDIRS) $(LIBS)
	mv $@ $(INSTALL)

clean :
	/bin/rm -rf *.o sac2gmtmap stats
