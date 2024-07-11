CC = gcc

all : driver driver3d

driver : driver.o beschb.o bessjy.o sphbes.o bessi0.o
	$(CC) -o driver driver.o beschb.o bessjy.o sphbes.o bessi0.o -lm -lget -L../../lib

driver3d : driver3d.o bessel_subs.o ../misc_tools/distaz.o
	$(CC) -o driver3d driver3d.o bessel_subs.o ../misc_tools/distaz.o -lm -lget -L../../lib


clean :
	/bin/rm -f *.o driver driver3d
