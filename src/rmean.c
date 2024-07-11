#include <stdio.h>

void remove_mean( float *x, int n )
{
	int i;
	float xtmp = 0, sum = 0, mean = 0;
	for( i = 0; i < n; i++ ) sum += x[i];
	mean = sum/(float)n;
	for( i = 0; i < n; i++ )
	{
		xtmp = x[i]-mean;
		x[i] = xtmp;
	}
}
