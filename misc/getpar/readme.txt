This is getpar library originally written by Caltech Prof. Robert W. Clayton from the 1980-1990's
Here we adopted just the C parts be also Fortran available but not used.
Widely used in all kinds of C applications, see more comments in src code, e.g., ...

 * DESCRIPTION
 *	retreive command line arguments.  Acquired from Caltech.
 *
 *	See getpar.3 for details.
 * copyright (c) Robert W. Clayton
 *		 Seismological Laboratory
 *		 Caltech
 *		 Pasadena, CA 91125
 *
 * Getpar routines:
 *
 * Externally visible routines:
 *
 *		setpar(argc,argv)
 *		getpar(name,type,valptr)
 *		mstpar(name,type,valptr)
 *		endpar()
 *
 * To get C-version:
 *		cc -c getpar.c
 *
 * To get F77-version:
 *		cp getpar.c fgetpar.c
 *		cc -c -DFORTRAN fgetpar.c
 *		rm fgetpar.c
 *
 * To get the environment processing stuff add the flag
