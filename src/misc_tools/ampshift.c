#include <stdio.h>

/*** 
* ampshift() is called by grnlib2sac and sacdata2inv, 
* ampshift.c removes the step function at the first sample point (data, synthetics & Greens functions)
* which causes a whole bunch of problems after filtering
***/

void ampshift( float *x, int n, int verbose )
{
	int i;
	float x0,xtmp;
	x0 = x[0];

	/* if( verbose ) fprintf( stdout, "ampshift(): n=%d x0=%g\n", n, x0 ); */

	for( i=0; i<n; i++ )
	{
		xtmp = x[i] - x0;
		x[i] = xtmp;
	}
}
