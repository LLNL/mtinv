CC = gcc
FC = gfortran
INCLUDE = -I../include
GRN_LIBS = -lm -L../../lib -lglib -lget
PLOT_LIBS = -L../../lib -lcgraph
INSTALL = ../../bin
CFLAGS = $(INCLUDE)
FFLAGS = 

.c.o :
	$(CC) $(CFLAGS) -c $< -o $@

.f.o :
	$(FC) $(FFLAGS) -c $< -o $@

all : rayp

rayp :	rayp.o rayp_subs.o ../stasubs.o ../modsubs.o ../misc_tools/distaz.o
	$(CC) -o rayp rayp.o rayp_subs.o ../stasubs.o ../modsubs.o ../misc_tools/distaz.o -lm -L../../lib -lglib -lget

clean : 
	rm -f *.o rayp
