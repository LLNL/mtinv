CC	= gcc
FC	= gfortran
INCLUDE	= -I../../include
CFLAGS  =
FFLAGS  =
LIBS	= -lm -lget
LIBDIR	= -L../../lib
INSTALL	= ../../bin

EXEC = saccut
all : $(EXEC)
OBJS = saccut.o saccut_subs.o ../timesubs.o ../sacextrema/sacextrema.o
saccut : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS) $(LIBDIR)
	mv $@ $(INSTALL)
clean :
	rm -f *.o $(EXEC)
