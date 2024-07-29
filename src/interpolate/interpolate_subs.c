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
#include <strings.h>
#include <math.h>
#include "interpolate_subs.h"

#define FALSE   0
#define TRUE    1

/* This progam interpolates the data in SAC format data file to any given
 *      sample interval. This is done in the frequency domain by either adding
 *      zeros or truncating the frequency response. A mixed-radix discrete fft
 *      is used to avoid the necessicity of have data lengths constrained to be
 *      a power of two.
 *
 */

void interpolate_fft( float *data, int old_npts, float old_delta, int *new_npts, float new_delta ) 
{
	int i, j;
	float *x;
	float sample_inter, data_length, eps=1.5;
	int ndatao,ndataf,array_length;
	int factor( int );
	int real_dis_fft( float *, int, int );
	
/*** calculate the new record length ***/
	sample_inter = new_delta;
	ndatao = old_npts;
	do {
		ndatao = factor( ndatao );
		data_length = (ndatao-1) * old_delta;
		ndataf = (int)(data_length/sample_inter+eps);
		ndatao++;
	} while( ndataf != factor( ndataf ));
	ndatao--;
	if( ndatao < ndataf ) ndataf--;
	array_length = ndatao;
	if( ndataf > ndatao ) array_length = ndataf;

	x = (float *)calloc( array_length*2+2, sizeof(float) );
	for( i=0; i < old_npts; i++ ) x[i] = data[i];

/*** do fft ***/
	real_dis_fft(x,ndatao,1);

/*** adjust frequency spectrum by padding zeros ***/
	if(ndataf > ndatao)
		for(i=ndatao+2; i<ndataf+1; i++)x[i]=0.0;

/*** do inverse fft ***/
	real_dis_fft(x,ndataf,-1);

/*** get amplitude of output right and reshuffle complex array ***/
/****
	printf("realloc memory for data old_npts=%d new_npts=%d old_delta=%g new_delta=%g\n",
		ndatao, ndataf, old_delta, sample_inter );
	data = (float *)realloc( data, ndataf * sizeof(float) );
	for(i=0;i<ndataf;i++)
		data[i]= x[i]/ndatao;
*****/
	if( ndataf > ndatao ) ndataf = ndatao;
	for(i=0;i<ndataf;i++) data[i]= x[i]/ndatao;

/*** change appropriate header values ***/
	*new_npts = ndataf;

	free(x);
	/* return (float *)data; */
}


/* factor
 *      returns the smallest number >= n that can be factored into
 *      products of primes in the array "primes".
 */
int factor( int n )
{
	int primes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37 };
	int n_primes;
	int factor;
	int *p;
	while( TRUE )
        {
		factor = n;
		p = primes;
		n_primes=12;
		
		while( n_primes-- )
		{
			while( !(factor % (*p)) )
				factor /= (*p);
			++p;
		}
		if( factor == 1 ) return( n );
		++n;
	}
}

/* subroutine discrete_fft constructs the discrete fast fourier transform
 * of a time series using the Cooley-Tukey mixed Radix method.  Algorithum
 * is modified from Conte and de Boor, Elementary Numerical Analysis, page
 * 281.
 *
 * Input:
 *	z1=pointer to complex array of data to be fourier transformed
 *         Length of array should be n+1
 *	n=number of data points in data
 *	isign=sign of transform; 1 for fft, -1 for inverse fft
 *
 * Output:
 *	z1=pointer to complex array fourier transformed data
 *		note: input data array is over written
 *			Also if a data set is transformed and then
 *			inverse transformed, output amplitudes should
 *			be divided by the n
 */

void discrete_fft( complex *z1, int n, int isign )
{

	complex *z2;   /* temporary work space */
	int after,now,before,next,nextmx,inz,i;	
	static int prime[12]={2,3,5,7,11,13,17,19,23,29,31,37};
	void fftstp( complex *, int, int, int, complex *, int );

	after=1;
	before=n;
	next=0;
	nextmx=12;
	inz=1; /* flip-flop variable to keep track of transform */

/* open up temporary work space */

	if((z2=(complex *)calloc((unsigned)(n*2+1),sizeof(float))) == NULL)
	{
		fprintf(stderr,"Error allocating space for fft\n");
		exit(-2);
	}

	while(before != 1)
	{
nextprime:	if((before/prime[next])*prime[next] < before) /* find smallest prime */
		{
			++next;
			if(next < nextmx) goto nextprime;
			now=before;  /* set up variables for transform */
			before=1;
		}
		else
		{
			now=prime[next]; /* set up variables for transform */
			before=before/prime[next];
		}
		if(inz == 1) /* perform one step of transform */
		{
			fftstp(z1,after,now,before,z2,isign);
		}
		else
		{
			fftstp(z2,after,now,before,z1,isign);
		}
		inz=3-(inz); /* keep track of output */
		after*=now;
	}
	
	if(inz == 2) bcopy((char *)z2,(char *)z1,sizeof(complex)*n);  /* output is in z2 move it to z1 */
	free(z2); /* free temporary space */
}

/* subroutine carries out one step of the discrete fourier transform 
 */

void fftstp( complex *zin, int after, int now, int before, complex *zout, int isign )
{

	int ia,ib,in,j,inter_fact2,inter_fact3;
	complex arg,omega,value,temp,*pointer1,*pointer2,*array_end2;
	complex *array_end1,*pointer3;
	float angle,twopi=6.283185307;

/* pointer1 & pointer2 -> zin ; pointer3 -> zout */

	angle=twopi/(float)(isign*now*after);
	omega.r=cos(angle);
	omega.i=(-1.0*sin(angle));
	arg.r=1.0;
	arg.i=0.0;

	inter_fact2=after*before;  /* array increment for zin */
	inter_fact3=now*after;  /* array increment for zout */

	for(j=1;j<=now;j++)
	{
		for(ia=1;ia<=after;ia++)
		{

			array_end1=zin+(ia-1+after*(before-1+before*(now-1))); /* loop end criteria for zin */
			for(ib=1,pointer3=zout+ia-1+after*(j-1),pointer1=zin+ia-1+after*before*(now-1);pointer1<=array_end1;ib++,pointer3+=inter_fact3,pointer1+=after)
			{
				value.r=pointer1->r;
				value.i=pointer1->i;

				array_end2=zin+(ia-1+after*(ib-1)); /* loop end criteria for zin */
				for(pointer2=zin+(ia-1+after*(ib-1+before*(now-2)));pointer2>=array_end2;pointer2-=inter_fact2)
				{
					temp.r=value.r*arg.r-value.i*arg.i;
					temp.i=value.r*arg.i+value.i*arg.r;
					value.r=temp.r+pointer2->r;
					value.i=temp.i+pointer2->i;
				}
				pointer3->r=value.r;
				pointer3->i=value.i;
			}
			temp.r=arg.r*omega.r-arg.i*omega.i;
			arg.i=arg.r*omega.i+arg.i*omega.r;
			arg.r=temp.r;
		}
	}
}
/* real_dis_fft computes the discrete fourier transform of a real input data
 * array. 
 *
 * Inputs;
 *		data=real array of input data
 *			Note: must be dimensioned to 2*(npts+1)
 *		npts=number of data points in data
 *		isign=sign of fourier transform 1 for transform -1 for inverse
 *
 * Outputs;
 *		data=array of transformed data
 */

int real_dis_fft( float *data, int npts, int isign )
{
	int i;
	void discrete_fft( complex *, int, int );

	if(isign == 1)  /* convert real array to complex array with zero imag */
	{
		for(i=npts;i>=0;--i)
		{
			data[i*2]=data[i];
			data[i*2+1]=0.0;
		}
	}
	else /* complete complex array using inherent symmetry */
	{
		for(i=0;i<npts;i+=2)
		{
			data[2*(npts)-i]=data[i];
			data[2*(npts)-i+1]=(-data[i+1]);
		}
	}
	discrete_fft((complex *)data,npts,isign);

	if(isign == -1) /* collapse complex array to a real array */
	{
		for(i=0;i<=npts;i++)
		{
			data[i]=data[i*2];
		}
	}
	return 0;
}


