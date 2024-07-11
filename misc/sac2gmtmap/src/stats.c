/*** not really needed in this mtinv version, previously used by sacqc - Quality control sac formated data ***/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*** reads column of data - floating pt nums - and computes statistics ***/

int main( int ac, char **av )
{
	int kk, nsamps;
	float *x;
	float ave=0,adev=0,sdev=0,svar=0,skew=0,curt=0,med=0;
	float min=0, max=0;

	void median( float *x, int n, float *med, float *min, float *max );
	void moment( float *data, int n, float *ave, float *adev, float *sdev, float *svar, float *skew, float *curt );

	float percentiles( float *x, int nsamps, float percent );
	/* float percent_ninety_five, percent_five; */
	float percent_ninety, percent_fifty, percent_ten;

/*** begin main ****/
	x = (float *)malloc(2*sizeof(float));
	kk=1;
	while( fscanf(stdin, "%f", &x[kk]) == 1 )
	{
		kk++;
		x = (float *)realloc(x, (kk+2)*sizeof(float));
	}
	nsamps = kk - 1;

	moment( x, nsamps, &ave, &adev, &sdev, &svar, &skew, &curt );

	median( x, nsamps, &med, &min, &max);

	/* percent_ninety_five = percentiles( x, nsamps, 0.95 ); */
	percent_ninety      = percentiles( x, nsamps, 0.90 );
	percent_fifty       = percentiles( x, nsamps, 0.50 );
	/* percentiles( x, nsamps, 0.20 ); */
	percent_ten         = percentiles( x, nsamps, 0.10 );
	/* percent_five        = percentiles( x, nsamps, 0.05 ); */
	
	printf("n= %d min= %g med= %g max= %g ave= %g adev= %g sdev= %g svar= %g skew= %g curt= %g percentiles 10%%= %g 50%%= %g 90%%= %g\n",
		nsamps,min,med,max,ave,adev,sdev,svar,skew,curt, percent_ten, percent_fifty, percent_ninety );

	free(x);
}
