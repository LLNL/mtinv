### linux
# CC = gcc
# CXX = g++
### macosx
CC = gcc
CXX = g++

INCLUDE = ../include
CFLAGS =  -Wall -I$(INCLUDE)
CXFLAGS = -std=c++11  -std=gnu++11  -Wall -I$(INCLUDE)
LIBS = -lm -L../../lib -lget
INSTALL = $(HOME)/bin

.c.o :
	$(CC) $(CFLAGS) -c $< -o $@
.cpp.o : 
	$(CXX) $(CXFLAGS) -c $< -o $@


all : testgasdev gaussian cauchy testrand

testrand : testrand.o getRandFloat.o getRandSphere.o
	$(CXX) $(CXFLAGS) -o testrand testrand.o getRandFloat.o getRandSphere.o $(LIBS)

testgasdev : testgasdev.o ransubs.o
	$(CC) $(CFLAGS) -o $@ testgasdev.o ransubs.o $(LIBS)
	mv $@ $(INSTALL)

gaussian : gaussian.o
	$(CC) $(CFLAGS) -o $@ gaussian.o $(LIBS)
	mv $@ $(INSTALL)

cauchy : cauchy.o
	$(CC) $(CFLAGS) -o $@ cauchy.o $(LIBS)
	mv $@ $(INSTALL)

clean :
	/bin/rm -f *.o testgasdev gaussian cauchy testrand
