/***********************************************************************************/
/*** Copyright 2024 Gene A. Ichinose (LLNL)                                      ***/
/***                                                                             ***/
/*** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” ***/
/*** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  ***/
/*** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE   ***/
/*** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         ***/
/*** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        ***/
/*** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    ***/
/*** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     ***/
/*** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     ***/
/*** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF      ***/
/*** THE POSSIBILITY OF SUCH DAMAGE.                                             ***/
/***                                                                             ***/
/*** Prepared by LLNL under Contract DE-AC52-07NA27344.                          ***/
/***********************************************************************************/

/*  
* subroutines to do cross correlations, used in mtinv.c to compute fit and time-shift (time lag) 
*    between data and synthetics computed from moment tensor inversion
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern char progname[128];

void xcorr(	float *d,
		float *s,
		int nt,
		float dt,
		int *ilag,
		float *tlag,
		float *xcorcoef, 
		float cortol,
		int verbose,
		int ishift )
{
	int i;
	float *utmp;
	float xcor, xlag;
	int lag;

/*** shift/shift.c ***/

	void shiftx( int n, float *aa, float *bb, int xlag );

	void max_xcorr( int, float *, float *, int *, float * );

/*** debug ***/
	float beg = 0;
	/* verbose = 1; */

/***************************************************************/
/*** do the cross correlation to get the coefficient and lag ***/
/***************************************************************/
	max_xcorr( nt, d, s, &lag, &xcor );
	xlag = (float)(lag-1) * dt;
	if(verbose)
	{
	  fprintf( stdout, 
		"%s: %s: %s: ilag=%d tlag=%g xcorcoef=%g\n",
			progname,  
			__FILE__,
			__func__,
			lag, 
			xlag, 
			xcor );
	}
	*xcorcoef = xcor;
	*tlag = xlag;
	*ilag = lag;

/*******************************************/
/*** shift syn vector to align if needed ***/
/*******************************************/
	
	if(verbose) 
	{
		fprintf( stdout, "%s: %s: %s: ishift=%d xcor=%g xlag=%g lag=%d cortol=%g\n",
			progname, 
			__FILE__,
                        __func__,
			ishift, xcor, xlag, lag, cortol );
	}

	if( ishift && (xcor > cortol) )
	{
		if(verbose)
		{
		  fprintf( stdout, "%s: %s: %s: shifting data lag=%d\n", 
			progname,
			__FILE__,
                        __func__,
			lag );
		}

		/* wrtnewsac( "dat.out.sac", dt, nt, d, beg ); */

		utmp = (float *)calloc( nt, sizeof(float));

		shiftx( nt, d, utmp, lag );

		for( i=0; i<nt; i++ ) 
			d[i] = utmp[i];

		free(utmp);
	}
}

/*** updated Mar2014 bug fix array out of bounds ***/
/*** causes corruption in xcorrelation value     ***/
/*** original lx = n; ***/
/*** original ly = n; ***/
/*** original lc = n+1; ***/
/*** new lx = n-1; ***/
/*** new ly = n-1; ***/
/*** new lc = n; ***/

void max_xcorr( int n, float *x, float *y, int *lag, float *cor )
{
	int lx, ly, lc, i, j, k;
	float *c1, *c2, coefxx, coefyy, coef1, coef2;
	int verbose = 0;
	int fwd_or_rev = -1; /** -1 forward +1 reverse ***/

	void cross( int, float *, int, float *, int, float * );
	float findmax( float *, int, int );
	int findlag( float *, float, int );

	lx=n-1;
	ly=n-1;
	lc=n;

/*********************************/
/*** find autocorrelation of x ***/
/*********************************/

	c1 = (float *)calloc(lc,sizeof(float));
	cross( lx, x, lx, x, lc, c1 );
	coefxx = findmax( c1, 0, lc );
	if(verbose)
	{
		fprintf(stdout, "%s: %s: %s: Autocorrelation x coefxx=%g\n",
			progname, __FILE__, __func__, coefxx );
	}
	free(c1);

/*********************************/
/*** find autocorrelation of y ***/
/*********************************/

	c1 = (float *)calloc(lc,sizeof(float));
	cross( ly, y, ly, y, lc, c1 );
	coefyy = findmax( c1, 0, lc );
	if(verbose)
	{
		fprintf( stdout, "%s: %s: %s: Autocorrelation y coefyy=%g\n",
			progname, __FILE__, __func__, coefyy );
	}
	free(c1);

/*********************************************************/
/*** do forward and reverse cross correlation to find  ***/
/***   the correct lag and max correlation             ***/
/*********************************************************/

	c1 = (float *)calloc(lc,sizeof(float));
	c2 = (float *)calloc(lc,sizeof(float));

	cross( lx, x, ly, y, lc, c1 );
	coef1 = findmax( c1, 0, lc );

	cross( ly, y, lx, x, lc, c2 );
	coef2 = findmax( c2, 0, lc );
	
	if( coef1 >= coef2 )
	{
		*lag = findlag( c1, coef1, lc );
		
		if( ( coefxx * coefyy ) <= 0 ) 
		{
		  *cor = 0;
		  *lag = 0;
		}
		else
		{
		  *cor = coef1/sqrt( coefxx * coefyy );
		  *lag *= -1;
		}

		if(verbose) 
		{
		  fprintf( stdout, "%s: %s: %s: maxcor: coef1 >= coef2 cor=%g lag=%d\n",
			progname, __FILE__, __func__, *cor, *lag );
		}
	}

	if( coef2 > coef1 )
	{
		*lag = findlag( c2, coef2, lc );

		if( ( coefxx * coefyy ) <= 0 ) 
		{
                  *cor = 0;
		  *lag = 0;
		}
                else
		{
		  *cor = coef2/sqrt(coefxx*coefyy);
		  *lag *= +1;
		}

		if(verbose)
		{
		  fprintf( stdout, "%s: %s: %s: maxcor: coef2 > coef1 cor=%g lag=%d\n",
			progname, __FILE__, __func__, *cor, *lag );
		}
	}
	free(c1);
	free(c2);
}

int findlag( float *x, float coef, int n )
{	
	int i, lag = 0;
	for( i=0; i<n; i++ )
	{
		if( coef == x[i] ) lag = i;
	}
	return lag;
}

float findmax( float *x, int start, int n )
{
	float max;
	int i;
	max = x[start];
	for( i=0; i<n; i++ )
	{
		if( x[i+start] > max ) max = x[i];
	}
	return max;
}

/*** updated Mar2014 bug fix array out of bounds ***/
/*** causes corruption in xcorrelation value     ***/
/***  original g[j] += x[j+i+1] * y[i]; ***/
/***   new     g[j] += x[j+i] * y[i];   ***/

void cross( int lx, float *x, int ly, float *y, int lg, float *g )
{
	int i, j, k;
	for( j=0; j<lg; j++ )
	{
		if( (lx-j+1) < ly ) k = (lx - j + 1);
		if( ly < (lx-j+1) ) k = ly;
		if( k > 0 )
		{
			for( i=0; i<k; i++ )
			{
				g[j] += x[j+i] * y[i];
			}
		}
		else
		{
			g[j] = 0;
		}
	}
}

float find_abs_max( float *x, int n )
{
	int i;
	float xmax, xtmp;
	xmax = fabs( x[0] );
	for( i=1; i<n; i++ )
	{
		xtmp = fabs(x[i]);
		if( xtmp > xmax ) xmax = xtmp;
	}
	return xmax;
}
