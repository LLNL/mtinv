CC = gcc
CFLAGS = 
all :  driver

OBJS = sactransfer.o transfersubs.o fftsub.o

driver : driver.o fftsub.o
	$(CC) -o driver  driver.o fftsub.o -lm -L../../lib -lget

sactransfer : $(OBJS)
	$(CC) -o $@ $(OBJS) -lm -lget

clean : 
	rm -f *.o driver
