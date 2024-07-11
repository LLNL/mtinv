/***
 Performs integration using the trapezoidal rule
 This command uses the trapezoidal integration method.  The first
 output data point is set to zero and the number of points is
 reduced by one.  The data does not have to be evenly spaced.

 Tested against SAC for accuracy.
****/

#include <stdio.h>
#include <stdlib.h>

void integrate( float *x, float dt, int *nt, float *beg )
{
	float sum = 0, step;
	int i, npts;
	float *y;

	npts = *nt - 1;
	step = dt/2;
	y = calloc( npts, sizeof(float) );

	for( i = 0; i < npts; i++ )
	{
		sum += step * ( x[i+1] + x[i] );
		y[i] = sum;
	}

	for( i = 0; i < npts; i++ ) x[i] = y[i];
	y[*nt] = 0;
	x[*nt] = 0;

	*beg = *beg + (dt/2.0);
        *nt = npts;

	free(y);
	return;
}
