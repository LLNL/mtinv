#include <math.h>

void sac_minmax( float *x, int n, float *max, float *min, float *mean )
{
        int i;
        float sum=0;
        *max = x[0];
        *min = x[0];
        *mean = 0;
        for( i=0; i<n; i++ )
        {
                if( x[i] > *max ) *max=x[i];
                if( x[i] < *min ) *min=x[i];
                sum = sum + x[i];
        }
        *mean = sum/n;
}

void sac_absmax( float *x, int n, float *absmax )
{
	int i;
	*absmax = fabs( x[0] );
	for( i=0; i<n; i++ )
	{
		if( fabs( x[i] ) > *absmax ) 
			*absmax = fabs( x[i] );
	}
}

/*** THESE WERE FOUND IN SACIO/SACIO.C ***/

/*** duplicate in sacextreama.c ***/
/*
void sac_absmax( float *x, int n, float *absmax )
{
        int i;
        *absmax = fabs( x[0] );
        for( i=0; i<n; i++ )
        {
                if( fabs( x[i] ) > *absmax )
                        *absmax = fabs( x[i] );
        }
}
*/

/*** duplicate in sacextreama.c ***/
/*
void sac_minmax( float *x, int n, float *max, float *min, float *mean )
{
        int i;
        float sum=0;
        *max = x[0];
        *min = x[0];
        *mean = 0;
        for( i=0; i<n; i++ )
        {
                if( x[i] > *max ) *max=x[i];
                if( x[i] < *min ) *min=x[i];
                sum = sum + x[i];
        }
        *mean = sum/n;
}
*/

