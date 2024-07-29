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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/** external variables for medfit ***/
int ndatat=0;   /* defining declaration */
float *xt=0,*yt=0,aa=0.0,abdevt=0.0;    /* defining declaration */

char progname[128];

void rtrend_sac( float *x, int n, float dt, float beg, int verbose )
{
	int i;
	float a, b, abdev, *t, *y;
	float xtmp;
	float min, max, mean;

	void medfit(float *t, float *x, int n, float *a, float *b, float *absdev );

	if(verbose)
	{
	  printf( "%s: rtrend_sac(): n=%d dt=%g beg=%g verbose=%d\n",
		progname, n, dt, beg, verbose );
	}

	y = (float *)malloc(n*sizeof(float));
	t = (float *)malloc(n*sizeof(float));

	beg = 0;
	for(i=0; i<n; i++) 
		t[i] = beg + (float)i*dt;

	medfit( t, x, n, &a, &b, &abdev );

	if(verbose)
	{
	   printf( "%s: rtrend_sac(): a=%g b=%g absdev=%g\n", 
		progname, a, b, abdev);
	}

	for(i=0; i<n; i++)
	{
		y[i] = a + t[i] * b;
		xtmp = x[i] - y[i];
		x[i] = xtmp;
	}

/*
	min=x[0]; max=x[0]; mean=x[0];
	for(i=1; i<n; i++)
	{
		if( x[i] < min ) min=x[i];	
		if( x[i] > max ) max=x[i];
		mean=mean+x[0];
	}
	mean=mean/n;
	if(verbose)
	{
	  printf( "%s: min=%g max=%g mean=%g \n", 
		progname, min, max, mean );
	}
*/

	free(y);
	free(t);
}

void medfit( float *x, float *y, int ndata, float *a, float *b, float *abdev )
{
	int j;
	float bb,b1,b2,del,f,f1,f2,sigb,temp;
	float sx=0.0,sy=0.0,sxy=0.0,sxx=0.0,chisq=0.0;

	float rofunc( float );

	ndatat=ndata;
	xt=x;
	yt=y;
	for (j=0;j<ndata;j++) {
		sx += x[j];
		sy += y[j];
		sxy += x[j]*y[j];
		sxx += x[j]*x[j];
	}
	del=ndata*sxx-sx*sx;
	aa=(sxx*sy-sx*sxy)/del;
	bb=(ndata*sxy-sx*sy)/del;

	for (j=0;j<ndata;j++)
	{
		chisq += (temp=y[j]-(aa+bb*x[j]),temp*temp);
	}

	sigb=sqrt(chisq/del);
	b1=bb;
	f1=rofunc(b1);
	b2=bb+((f1 > 0.0) ? fabs(3.0*sigb) : -fabs(3.0*sigb));

	f2=rofunc(b2);

	while (f1*f2 > 0.0) {
		bb=2.0*b2-b1;
		b1=b2;
		f1=f2;
		b2=bb;
		f2=rofunc(b2);
	}

	sigb=0.01*sigb;
	while (fabs(b2-b1) > sigb) {
		bb=0.5*(b1+b2);
		if (bb == b1 || bb == b2) break;
		f=rofunc(bb);
		if (f*f1 >= 0.0) {
			f1=f;
			b1=bb;
		} else {
			f2=f;
			b2=bb;
		}
	}
	*a=aa;
	*b=bb;
	*abdev=abdevt/ndata;
}

float rofunc( float b )
{
	int j,n1,nmh,nml;
	float *arr,d,sum=0.0;
	float *vector(int,int);
	void sort( int, float [] );
	void free_vector();
	extern int ndatat;      /* defined in MEDFIT */
	extern float *xt,*yt,aa,abdevt;

	arr=vector(0,ndatat);
	n1=ndatat+1;
	nml=n1/2;
	nmh=n1-nml;

	for (j=0;j<ndatat;j++) arr[j]=yt[j]-b*xt[j];

	sort(ndatat,arr);
	aa=0.5*(arr[nml]+arr[nmh]);
	abdevt=0.0;
	for (j=0;j<ndatat;j++) {
		d=yt[j]-(b*xt[j]+aa);
		abdevt += fabs(d);
		sum += d > 0.0 ? xt[j] : -xt[j];
	}
	free_vector(arr,0,ndatat);
	return sum;
}

void sort( int n, float ra[] )
{
	int l,j,ir,i;
	float rra;

	l=(n >> 1)+1;
	ir=n;
	for (;;) {
		if( l > 1 )
			rra=ra[--l];
		else {
			rra=ra[ir];
			ra[ir]=ra[1];
			if (--ir == 1) {
				ra[1]=rra;
				return;
			}
		}
		i=l;
		j=l << 1;
		while (j <= ir) {
			if (j < ir && ra[j] < ra[j+1]) ++j;
			if (rra < ra[j]) {
				ra[i]=ra[j];
				j += (i=j);
			}
			else j=ir+1;
		}
		ra[i]=rra;
	}
}
