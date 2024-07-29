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

/*** some functions taken from numerical reciepies and some custom math functions ***/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../include/nrutil.h"     /** numerical recipes **/

extern char progname[128];

float **matrix( int nrl, int nrh, int ncl, int nch )
{
        int i;
        float **m;

        m=(float **) calloc((unsigned)(nrh-nrl+1),sizeof(float*));
        m -= nrl;
        for(i=nrl;i<=nrh;i++) {
                m[i]=(float *) calloc((unsigned)(nch-ncl+1), sizeof(float));
                if (!m[i]) nrerror("allocation failure 2 in matrix()");
                m[i] -= ncl;
        }
        return(float **)m;
}

float *vector(int nl, int nh )
{
        float *v;
        v=(float *)malloc((unsigned) (nh-nl+1)*sizeof(float));
	if (!v) nrerror("allocation failure in vector()");
        return v-nl;
}

void free_matrix(float **m, int nrl, int nrh, int ncl, int nch )
{
        int i;
        for(i=nrh;i>=nrl;i--)
                free((char*) (m[i]+ncl));
        free((char*) (m+nrl));
}

void free_vector(float *v, int nl, int nh )
{
        free((char*) (v+nl));
}

/*** returns vector z diagonal elements of matrix xx ***/

void diag( int n, float **xx, float *z )
{
	int i, j;
	for( i=1; i<=n; i++ )
	{
		for( j=1; j<=n; j++ )
		{
			if( i == j ) z[i] = xx[i][j];
		}
	}
}

void matmul( int conj, float **bb, int nx, float *x, int ny, float *y )
{
	int ix, iy;
	void conjzero( int, int, int, float *, int, float * );

	conjzero( conj, 0, nx, x, ny, y );
	if( conj == 0 )
	{
		for(ix=1;ix<=nx;ix++)
		{
			for(iy=1;iy<=ny;iy++)
			{
				y[iy]=y[iy]+bb[iy][ix] * x[ix];
			}
		}
	}
	else
	{
		for(ix=1;ix<=nx;ix++)
		{
			for(iy=1;iy<=ny;iy++)
			{
				x[ix]=x[ix]+bb[iy][ix]*y[iy];
			}
		}
	}
}

void conjzero( int conj, int add, int nx, float *x, int ny, float *y )
{
        int ix, iy;
        if( add == 0 )
        {
                if( conj == 0 )
                        for(iy=1;iy<=ny;iy++)y[iy]=0;
                else
                        for(ix=1;ix<=nx;ix++)x[ix]=0;
        }
}

void nrerror(char *error_text)
{
        /* void exit(); */

        fprintf(stderr,"Numerical Recipes run-time error...\n");
        fprintf(stderr,"%s\n",error_text);
        fprintf(stderr,"...now exiting to system...\n");
        exit(1);
}

float root_mean_square( float *x, int n )
{
	int i;
	float rms, sum=0;
	for( i=0; i<n; i++ )
	{
		sum += x[i]*x[i];
	}
	rms = sqrt(sum)/(float)n;
	return rms;
}
