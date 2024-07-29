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

/*** Statistics subroutines for stats.c ****/
/*** from Numerical Receipes. see https://numerical.recipes/licenseinfo.html ***/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void median( float *x, int n, float *med, float *min, float *max )
{
	int *indx;
	void indexx( int n, float *arrin, int *indx );

	indx = (int *)malloc( (n+1) * sizeof(int));

	indexx( n, x, indx );

	if( (n%2) == 0 ) 
	{
		*med=x[indx[n/2]];
	}
	else 
	{
		*med=x[indx[(n+1)/2]];
	}
	*min = x[indx[1]];
	*max = x[indx[n]];

	free(indx);
}

void moment( float *data, int n, float *ave, float *adev, float *sdev, float *svar, float *skew, float *curt )
{
        int j;
        float s,p;

        if (n <= 1) 
	{
		fprintf(stderr, "n must be at least 2 in MOMENT\n");
		exit(-1);
	}
        s=0.0;
        for (j=1;j<=n;j++) s += data[j];
        *ave=s/n;
        *adev=(*svar)=(*skew)=(*curt)=0.0;
        for (j=1;j<=n;j++) {
                *adev += fabs(s=data[j]-(*ave));
                *svar += (p=s*s);
                *skew += (p *= s);
                *curt += (p *= s);
        }
        *adev /= n;
        *svar /= (n-1);
        *sdev=sqrt(*svar);
        if (*svar) 
	{
                *skew /= (n*(*svar)*(*sdev));
                *curt=(*curt)/(n*(*svar)*(*svar))-3.0;
        } 
	else 
	{
		fprintf(stderr, "No skew/kurtosis when variance = 0 (in MOMENT)\n");
	}
}

void indexx( int n, float *arrin, int *indx )
{
	int l,j,ir,indxt,i;
	float q;

	for (j=1;j<=n;j++) indx[j]=j;
	l=(n >> 1) + 1;
	ir=n;
	for (;;) {
		if (l > 1)
			q=arrin[(indxt=indx[--l])];
		else {
			q=arrin[(indxt=indx[ir])];
			indx[ir]=indx[1];
			if (--ir == 1) {
				indx[1]=indxt;
				return;
			}
		}
		i=l;
		j=l << 1;
		while (j <= ir) {
			if (j < ir && arrin[indx[j]] < arrin[indx[j+1]]) j++;
			if (q < arrin[indx[j]]) {
				indx[i]=indx[j];
				j += (i=j);
			}
			else j=ir+1;
		}
		indx[i]=indxt;
	}
}

/**** this is similar approach to estimation of median values ***/

float percentiles( float *x, int ns, float percent )
{
        int *indx, ptr;
        float fptr;
        float value;
        void indexx( int ns, float *arrin, int *indx );
        int debug = 0;

/* order the values */
        indx = (int *)malloc( (ns+1) * sizeof(int));
        indexx( ns, x, indx );

/* multiply percent by the total number of values n to get the index pointer to percentile */

        fptr = percent * ns;

/** if the index is not a whole number then round it up to the nearest ***/
        ptr  = (int)roundf( percent * ns );

/** if index is not whole number then that is the percentile,
     otherwise average value and the value of next index ***/

        if( (fptr - roundf( percent * ns )) == 0 )
                value = ( x[indx[ptr]] + x[indx[ptr+1]] )/2;
        else
                value = x[indx[ptr]];

        if(debug)
        {
          fprintf( stderr, "%s: %s: percent=%g value=%g ptr=%d indx=%d ns=%d fptr=%g\n",
                __FILE__, __func__,
                percent,
                value,
                ptr,
                indx[ptr],
                ns,
                fptr );
        }

        return value;
}
