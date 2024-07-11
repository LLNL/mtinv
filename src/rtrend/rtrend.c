#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern char progname[128];

/* Now Local Variables ( float *a, float *b, float *sigmaa, float *sigmab, float *sigma, float *corcoef ) */

void rtrend( float x0, float dx, float *y, int n, int verbose )
{
	int i;
	float sumx, sumy, sumxy, sumx2, sumy2;
	float xi, yi;
	float sigma2, sigmaa2, sigmab2, den;
	float degfree;
	float ytmp, ypre;

	float a, b, sigmaa, sigmab, sigma, corcoef;

	sumx  = 0;
	sumy  = 0;
	sumxy = 0;
	sumx2 = 0;
	sumy2 = 0;
	degfree = (float)n - 2;

/*** loop over each data point and calculate sums ***/

	xi = x0;
	for( i = 0; i < n; i++ )
	{
		yi = y[i];
		sumx  += xi;
		sumy  += yi;
		sumxy += ( xi * yi );
		sumx2 += ( xi * xi );
		sumy2 += ( yi * yi );
		xi += dx;
	}

/* calculate linear fit */

	den = ( (float)n * sumx2 - sumx * sumx );
	b = ( sumx2 * sumy - sumx * sumxy ) / den;
	a = ( (float)n * sumxy - sumx * sumy ) / den;

/* estimate standard deviation */

	sigma2 = ( sumy2 + (float)n * b * b + a * a * sumx2 - 2 * b * sumy - 2 * a * sumxy + 2 * b * a * sumx ) / degfree;
	sigma = sqrt( sigma2 );

/* estimate erros in linear fit */

	sigmaa2 = (float)n * sigma2 / den;
	sigmab2 = sigma2 * sumx2 / den;
	sigmaa  = sqrt( sigmaa2 );
	sigmab  = sqrt( sigmab2 );

/* Calculate correlation coefficient between data and model. */

	corcoef = ( (float)n * sumxy - sumx * sumy ) / sqrt( den * ( (float)n * sumy2 - sumy*sumy ) );
	corcoef = fabs( corcoef );

/*** remove the mean and trend (y-intercept and slope) and return the new data vector **/

	xi = x0;
	for( i = 0; i < n; i++ )
	{
		ypre = b + a * xi;
		ytmp = y[i] - ypre;
		y[i] = ytmp;
		xi = xi + dx;
	}

	if(verbose)
	{
		fprintf( stdout, "%s: rtrend(): a=%g b=%g sigma=%g sigmaA=%g sigmaB=%g corcoef=%g\n",
			progname,
			a, 
			b, 
			sigma,
			sigmaa,
			sigmab,
			corcoef );
	}
}
