#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/nrutil.h"     /** numerical recipes **/
#include "../include/mt.h"         /** global datatype and structure declarations **/

extern char progname[128];

#define NUM_COMP 4

/*** this is used by mtinv.c - computes the correlation-coefficient and lagtime for each 3-C ***/

void find_best_shift( EventInfo *ev, float cortol, float maxtimeshift, float *time_shift_all )
{
	int i;
	float xcor[NUM_COMP];
	float tlag[NUM_COMP];
	int ixcor[NUM_COMP];

	int ncmp = 3;
	int debug = 0;

	void floatsort( float *x, int n );
	void indexx( int n, float *arrin, int *indx );

	ixcor[1] = 1;
	ixcor[2] = 2;
	ixcor[3] = 3;

	xcor[1]  = ev->zxcor;
	xcor[2]  = ev->rxcor;
	xcor[3]  = ev->txcor;

	tlag[1]  = ev->ztlag;
	tlag[2]  = ev->rtlag;
	tlag[3]  = ev->ttlag;

	if(debug)
	{
	 for( i=1; i<=ncmp; i++ )
	 {
	  fprintf ( stdout, "i=%d ixcor[i]=%d xcor=%g tlag=%g\n",
                i, ixcor[i], xcor[ixcor[i]], tlag[ixcor[i]] );
	 }
	}

	indexx( ncmp, xcor, ixcor );

	if( debug )
	{
	 for( i=1; i<=ncmp; i++ )
	 {
	  fprintf ( stdout, "i=%d ixcor[i]=%d xcor=%g tlag=%g\n", 
		i, ixcor[i], xcor[ixcor[i]], tlag[ixcor[i]] );
	 }
	}

	if( xcor[ixcor[3]] > cortol && fabs(tlag[ixcor[3]]) <= fabs(maxtimeshift) )
	{
		*time_shift_all = tlag[ixcor[3]];
	}
	else
	{
		*time_shift_all = 0;
	}
}
