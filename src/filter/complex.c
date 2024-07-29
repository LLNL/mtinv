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

/*** ./mtinv/src/filter/complex.c are my custom functions for complex numbers ***/
/*** these get called by ./mtinv/src/filter/filtersubs.c ***/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../include/complex.h"

float c_abs(Complex z)
{
	float x;
	float mycabs(float, float);
	x = mycabs( z.re, z.im );
	return x;
}

Complex c_cos(Complex z)
{
	Complex c;
	c.re =  cos(z.re) * cosh(z.im);
	c.im = -sin(z.re) * sinh(z.im);
	return c;
}

Complex c_div(Complex a, Complex b)
{
	float abr, abi, ratio, den;
	Complex c;
	if( (abr = b.re) < 0.) abr = - abr;
	if( (abi = b.im) < 0.) abi = - abi;
	if( abr <= abi )
	{
		if(abi == 0)
		{
			exit(-1);
			fprintf(stderr, "fatal error complex division by zero\n");
		}
		ratio = b.re / b.im ;
		den = b.im * (1 + ratio*ratio);
		c.re = (a.re*ratio + a.im) / den;
		c.im = (a.im*ratio - a.re) / den;
	}
	else
	{
		ratio = b.im / b.re ;
		den = b.re * (1 + ratio*ratio);
		c.re = (a.re + a.im*ratio) / den;
		c.im = (a.im - a.re*ratio) / den;
	}
	return c;
}

Complex c_mult(Complex a, Complex b)
{
	Complex c;
	c.re = a.re * b.re - a.im * b.im;
	c.im = a.im * b.re + a.re * b.im;
	return c;
}

/**** fortran complex conjugate *****/
Complex c_conjg(Complex a)
{
	a.im = -a.im;
	return a;
}

/*
float c_congj(Complex a)
{
	return (float)((a.re*a.re) + (a.im*a.im));
}
*/

Complex c_add( Complex a, Complex b )
{
	Complex c;
	c.re = a.re + b.re;
	c.im = a.im + b.im;
	return c;
}

Complex c_sub( Complex a, Complex b)
{
	Complex c;
	c.re = a.re - b.re;
	c.im = a.im - b.im;
	return c;
}

Complex c_exp( Complex z)
{
	Complex c;
	float expx;
	expx = exp(z.re);
	c.re = expx * cos(z.im);
	c.im = expx * sin(z.im);
	return c;
}

Complex c_log(Complex z)
{
	float mycabs( float, float );
	Complex c;
	c.im = atan2(z.im, z.re);
	c.re = log( mycabs(z.re, z.im) );
	return c;
}

Complex c_sin( Complex z )
{
	Complex c;
	c.re = sin(z.re) * cosh(z.im);
	c.im = cos(z.re) * sinh(z.im);
	return c;
}

Complex c_sqrt( Complex z )
{
	float mag;
	float mycabs(float,float);
	Complex c;

	if( (mag = mycabs(z.re, z.im)) == 0.)
	{
		c.re = 0;
		c.im = 0;
	}
	else if(z.re > 0)
	{
		c.re = sqrt(0.5 * (mag + z.re) );
		c.im = z.im / c.re / 2;
	}
	else
	{
		c.im = sqrt(0.5 * (mag - z.re) );
		if(z.im < 0)
			c.im = - c.im;
		c.re = z.im / c.im /2;
	}
	return c;
}

float mycabs(float re, float im )
{
	float temp;
	if(re < 0) re = -re;
	if(im < 0) im = -im;
	if(im > re)
	{
		temp = re;
		re = im;
		im = temp;
	}
	if((re+im) == re) return(re);
	temp = im/re;
	temp = re*sqrt(1.0 + temp*temp);  /*overflow!!*/
	return(temp);
}

Complex c_scale( Complex x, float a )
{
        Complex c;
        c.re = x.re * a;
        c.im = x.im * a;
        return c;
}

float c_real( Complex a )
{
	return a.re;
}

float c_imag( Complex a )
{
	return a.im;
}
