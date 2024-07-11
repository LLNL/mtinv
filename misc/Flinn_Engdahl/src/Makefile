include ../../../Makefile.inc

# CC = gcc
# JC = javac
# FC = gfortran

INCLUDE = -I.
CFLAGS = $(INCLUDE)
FFLAGS = 
LIBDIR = -L../../../lib
LIBS = -lget -lm
INSTALL_BIN = ../../../bin

#all : FlinnEngdahl FlinnEngdahlTest grn2db updateMTdb
all : FlinnEngdahl grn2db updateMTdb

updateMTdb.o : updateMTdb.c
grn2db.o : grn2db.c
FlinnEngdahl.o : FlinnEngdahl.c

updateMTdb : updateMTdb.o
	$(CC) $(CFLAGS) -o $@ updateMTdb.o $(LIBDIR) $(LIBS)
	mv $@ $(INSTALL_BIN)

grn2db : grn2db.o
	$(CC) $(CFLAGS) -o $@ grn2db.o
	mv $@ $(INSTALL_BIN)

FlinnEngdahl : FlinnEngdahl.o
	$(CC) $(CFLAGS) -o $@ FlinnEngdahl.o $(LIBDIR) $(LIBS)
	mv $@ $(INSTALL_BIN)

FlinnEngdahlTest: FlinnEngdahlTest.java 
	javac FlinnEngdahlTest.java

clean :
	rm -f *.o *.class FlinnEngdahl FlinnEngdahlTest grn2db updateMTdb
	
