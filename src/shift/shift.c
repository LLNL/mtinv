#include <stdio.h>
#include <stdlib.h>

void shiftx( int n, float *aa, float *bb, int xlag )
{
	int i;

/*** no shift ***/
	if( xlag == 0 )
	{
		for( i=0; i<n; i++ )
			bb[i] = aa[i];
		return;
	}

/*** shift back in time ***/
	if( xlag > 0 )
	{
		for( i=0; i<n; i++ ) 
			bb[i] = 0;

		for( i=xlag; i<n; i++ )
			bb[i] = aa[i-xlag];
		return;
	}

/*** shift forward in time ***/
	if( xlag < 0 )
	{
		xlag = abs(xlag);

		for( i=0; i<n; i++ ) 
			bb[i] = 0;

		for( i=xlag; i<n; i++ )
			bb[i-xlag] = aa[i];
		return;
	}

	fprintf(stderr, "Error occurred in shiftx()\n");
	exit(-1);
}
