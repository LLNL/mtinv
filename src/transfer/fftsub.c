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

/*      Fft.c
        1-d fast Fourier transform subroutine.
        From Claerbout (1985) p. 70.
                        fft(lx, cx, signi, scale);
        Arguments:      lx      number of elements of vector to transform
                                MUST BE A POWER OF 2
                        cx      pointer to vector of complex structures
                        signi   sign of transform- +1 forward to Fourier domain
                                                   -1 inverse to real domain
       fft( npts, &samp[0], FORWARD, scale)
**/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../include/complex.h"

#define  rmul(x,y)      (x.re * y.re - x.im * y.im)
#define  imul(x,y)      (x.im * y.re + x.re * y.im)

void fft( int lx, Dcomplex *cx, double signi, double dt )
{
        int i, j, k, m, istep;
        double arg, pi, df, scale;
        Dcomplex cw, ct;
	int get_pow_2( int );

/*** check lx is power of 2 ***/
	if( lx != get_pow_2(lx) )
	{
		fprintf(stderr, "%s: %s: fft error: lx=%d must be power of 2\n", 
			__FILE__, __func__, lx);
		exit(-1);
	}

/*** set scaling and constants ***/
	pi = acos(-1.0);
	df = 1.0/(lx*dt);
	if( signi == FORWARD ) scale = sqrt( (1.0/(double)lx) * (dt/df) );
	if( signi == INVERSE ) scale = sqrt( (1.0/(double)lx) * (df/dt) );

	/* printf("dt=%g df=%g signi=%g scale=%g lx=%d\n", dt, df, signi, scale, lx ); */

	j = 0;
	for( i = 0; i < lx; i++)
	{
		if(i <= j ) 
		{
			ct.re    = scale * cx[j].re;
			ct.im    = scale * cx[j].im;
			cx[j].re = scale * cx[i].re;
			cx[j].im = scale * cx[i].im;
			cx[i].re = ct.re;
			cx[i].im = ct.im;
		}
		m = lx/2;
		while( j > (m-1) && (m > 1) )
		{
			j = j - m;
			m = m / 2;
		}
		j = j + m;
	}
	k = 1;
	do {
		istep = 2 * k;
		for( m = 0; m < k; m++  )
		{
			arg  = pi * signi * m/k;
			cw.re = cos( arg );
			cw.im = sin( arg );
		
			for( i = m; i < lx; i += istep )
			{
				ct.re      = cw.re * cx[i+k].re - cw.im * cx[i+k].im; 
				ct.im      = cw.im * cx[i+k].re + cw.re * cx[i+k].im; 
				cx[i+k].re = cx[i].re - ct.re;
				cx[i+k].im = cx[i].im - ct.im;
				cx[i].re   = cx[i].re + ct.re;
				cx[i].im   = cx[i].im + ct.im;
			}
		}
		k = istep;
	} while( k < lx );
	return;
}


int get_pow_2(int inum)
{
        int j=1;
        while( j<inum ) j=j*2;
        return j;
}

/*** differentiate function by multiplying the spectrum with omega 
	 overwrites incoming array
****/
void mulomega(float *x,  int npts, float delta )
{
	Dcomplex *z, zt, *w;
	double dt, df, pi;
	int nt, i, kk, n2;
	
/*** check for power of 2 ***/
	dt = (double)delta;
	nt = get_pow_2( npts );
	df = 1.0/((double)nt*dt);
	n2 = (int)nt/2.;
	pi = acos(-1.0);

/*** construct data space with padded zeros ***/
	z = (Dcomplex *)calloc( nt, sizeof(Dcomplex) );
	w = (Dcomplex *)calloc( nt, sizeof(Dcomplex) );

	for( i=0; i<npts; i++ )
	{
		z[i].re = (double)x[i];
		z[i].im = 0;
	}

/*** forward fast fourier transform into frequency domain ***/
	fft( nt, z, FORWARD, dt );

/*** frequency domain differentiation ***/
	for( i=0; i<n2; i++ )
	{
		w[i].re = 0;
		w[i].im = -4.0 * pi * df * (double)i;
	}
	for( i=0; i<n2; i++ )
	{
		zt.re = w[i].re * z[i].re - w[i].im * z[i].im;
		zt.im = w[i].im * z[i].re + w[i].re * z[i].im;
		z[i].re = zt.re;
		z[i].im = zt.im;
	}
	for( kk=0, i=n2; i<nt; i++ )
	{
		zt.re = w[i-kk].re * z[i].re - w[i-kk].im * z[i].im;
		zt.im = w[i-kk].im * z[i].re + w[i-kk].re * z[i].im;
		z[i].re = zt.re;
		z[i].im = zt.im;
		kk++;
	}
	fft( nt, z, INVERSE, dt );
	for( i=0; i<npts; i++ ) x[i] = (float)z[i].re;
	free(w);
	free(z);
}
