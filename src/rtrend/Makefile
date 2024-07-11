CC = gcc
INCLUDE = -I../../include 
CFLAGS = $(INCLUDE)
LIBS = -lm -L../../lib -lget

all : driver 

driver : driver.o rtrend.o ../sacio/sacio.o ../timesubs.o ../math/math.o ../rmean.o
	$(CC) $(CFLAGS) -o $@ driver.o rtrend.o ../sacio/sacio.o ../timesubs.o ../math/math.o ../rmean.o $(LIBS)

clean :
	rm -f *.o driver
