CC	= gcc
FC 	= gfortran
INCLUDE = -I../../include
FFLAGS  = $(INCLUDE)
CFLAGS  = $(INCLUDE)
INSTALL = ../../bin

.f.o: ; $(FC) $(FFLAGS) -c $*.f
.c.o: ; $(CC) $(CFLAGS) -c $*.c

EXEC 	= interpolate_drv
OBJS    = interpolate_drv.o interpolate_subs.o

all : $(EXEC)

$(EXEC) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lm -L../../lib -lget
	mv $(EXEC) $(INSTALL)

clean :
	rm -rf *.o $(EXEC)
