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

/*** filtering subroutines : these are similar to those filters used in seismic analysis code ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../include/complex.h"

#define TRUE 0
#define FALSE 1

typedef struct {
        char name[5];
} Stype;

void iir_filter( float *data, int nsamps, char *filter_type, float trbndw, 
  float a, int iord, char *operation_type, float flo, float fhi, float ts, int passes )
{
	float fnyq;
	float *sn;  /*** Numerator polynomials for second order sections ***/
	float *sd;  /*** Denominator polynomials for second order sections ***/
	int zp, nsects=0;

	void iir_apply( float *, int, int, float *, float *, int );
	void iir_design( int, char *, char *, float, float, float, float, float, 
		float *, float *, int * );

/*** check for one or two pass(zero phase) filter ***/
	if( passes != 1 && passes != 2 )
	{
		printf("number of passes=%d must be either 1 or 2\n", passes);
		exit(-1);
	}
	if( passes == 1 )
	{
		zp = FALSE;
	}
	else
	{
		zp = TRUE;
	}
/*** check range for number of poles ***/
	if( iord >= 11 || iord <= 0 )
	{
		printf("number of poles=%d needs to be between 1 and 10\n", iord );
		exit(-1);
	}
/*** check the filter corners ***/
	fnyq   = 0.5/ts;
	if( strncmp(operation_type, "BP", 2)==0 || strncmp(operation_type, "BR", 2)==0 )
	{
		if( flo > fnyq || flo == -1 )
		{
			printf("ERROR: lf=%g < fnyq=%g\n", flo, fnyq);
			exit(-1);
		}
		if( fhi > fnyq || flo == -1 )
		{
			printf("ERROR: hf=%g > fnyq=%g\n", fhi, fnyq);
			exit(-1);
		}
	}
	if( strncmp(operation_type, "LP", 2)==0 )
	{
		if( fhi == -1 )
		{
			fprintf(stderr, "ERROR: Lowpass corner frequency undefined\n");
			exit(-1);
		}
		if( fhi > fnyq )
		{
			printf("ERROR: Lowpass corner frequency > fnyq\n");
			exit(-1);
		}
	}
	if( strncmp(operation_type, "HP", 2)==0 )
	{
		if( flo == -1 )
		{
			fprintf(stderr, "ERROR: highpass corner frequency undefined\n");
			exit(-1);
		}
		if( flo > fnyq )
		{
			fprintf(stderr, "ERROR: highpass corner frequency > nyquist frequency\n");
			exit(-1);
		}
	}

/*** initialize memmory for filter design ***/
	sn = (float *)calloc( 30, sizeof(float));
	sd = (float *)calloc( 30, sizeof(float));
	iir_design( iord, operation_type, filter_type, a, trbndw, flo, fhi, ts, &sn[0], &sd[0], &nsects );

/*** filter the data ***/
	iir_apply( data, nsamps, zp, sn, sd, nsects );

	free(sn);
	free(sd);
}

void iir_apply( float *data, int nsamps, int zp, float *sn, float *sd, int nsects )
{
	int i,j,jptr;
	float output,x1,x2,y1,y2,b0,b1,b2,a1,a2;

	jptr = 1;
	for( j=1; j <= nsects; j++ )
	{
		x1 = 0;
		x2 = 0;
		y1 = 0;
		y2 = 0;
		b0 = sn[ jptr ];
		b1 = sn[ jptr+1 ];
		b2 = sn[ jptr+2 ];
		a1 = sd[ jptr+1 ];
		a2 = sd[ jptr+2 ];
		for( i=0; i<nsamps; i++ )
		{
			output = ( b0 * data[i] + b1*x1 + b2*x2 ) - ( a1*y1 + a2*y2 );
			y2 = y1;
			y1 = output;
			x2 = x1;
			x1 = data[i];
			data[i] = output;
		}
		jptr = jptr + 3;
	}
	if( zp == TRUE ) 
	{
		jptr = 1;
		for( j=1; j <= nsects; j++ )
		{
			x1 = 0;
			x2 = 0;
			y1 = 0;
			y2 = 0;
			b0 = sn[ jptr ];
			b1 = sn[ jptr+1 ];
			b2 = sn[ jptr+2 ];
			a1 = sd[ jptr+1 ];
			a2 = sd[ jptr+2 ];
			for( i=(nsamps-1); i>=0; i-- )
			{
				output = ( b0 * data[i] + b1*x1 + b2*x2 ) - ( a1*y1 + a2*y2 );
				y2 = y1;
				y1 = output;
				x2 = x1;
				x1 = data[i];
				data[i] = output;

			}
			jptr = jptr + 3;
		}
	}
}

void bilinear( float *sn, float *sd, int nsects )
{
	float a0,a1,a2,scale;
	int i,iptr;

	iptr = 1;
	for( i=1; i <= nsects; i++ )
	{
		a0 = sd[ iptr ];
		a1 = sd[ iptr+1 ];
		a2 = sd[ iptr+2 ];
		scale = a2 + a1 + a0;
		sd[ iptr ]   = 1;
		sd[ iptr+1 ] = (2*(a0-a2))/scale;
		sd[ iptr+2 ] = (a2-a1+a0)/scale;
		a0 = sn[ iptr ];
		a1 = sn[ iptr+1 ];
		a2 = sn[ iptr+2 ];
		sn[ iptr ]   = (a2  + a1 + a0)/scale;
		sn[ iptr+1 ] = (2. * (a0 -a2))/scale;
		sn[ iptr+2 ] = (a2  - a1 + a0)/scale;
		iptr = iptr + 3;
	}
}

float warp( float f, float ts )
{
	float twopi, angle, output;
	twopi = 2 * M_PI;
	angle = twopi * f * ts/2;
	output = ( 2 * (tan(angle)/ts) )/twopi;
	return output;
}

int butterworth_roots( int iord, Complex *p, Stype *stype, float *dcval )
{
	int k,half,ns;
	float pi, angle;
	
	pi = M_PI;
	half = iord/2; /** default C uses floor and does not round **/
	ns = 0;
	if( (2*half) < iord ) 
	{
		p[1].re = -1;
		p[1].im =  0;
		ns++;
		sprintf( stype[1].name, "SP" );
	}

	for( k=1; k<=half; k++ )
	{
		angle = pi * ( 0.5 + (2.*(float)k-1.)/(2.*(float)iord) );
		ns++;
		p[ns].re = cos(angle);
		p[ns].im = sin(angle);
		sprintf( stype[ns].name, "CP" );
	}
	*dcval = 1;
	return ns;
}

void iir_design( int iord, char *operation_type, char *filter_type, float a, 
   float trbndw, float fl, float fh, float ts, float *sn, float *sd, int *nsects )
{
	Complex *p, *z;
	Stype *stype;
	float dcvalue, flw, fhw, omegar, eps, ripple;
	int i, iptr;

	void bilinear( float *, float *, int );
	float warp(float,float);
	int butterworth_roots( int, Complex *, Stype *, float * );
	void lowpass_to_bandpass( Complex *, Complex *, float, int *, 
		float, float, float *, float *, Stype * );
	void bessel_roots( Complex *, Stype *, float *, int *, int );
	void chebyshev_parameters( float, float, int, float *, float * );
	void chebyshev1_roots( Complex *, Stype *, float *, int *, int, float );
	void chebyshev2_roots( Complex *, Complex *, Stype *, float *, int *, 
		int, float, float );
	void lowpass_to_bandreject( Complex *, Complex *, Stype *, float, int *, 
		float, float, float *, float * );

	void lowpass( Complex *, Complex *, Stype *, float, int *, float *, float * );

	void cutoffs( float *, float *, int *, float );

	void lowpass_to_highpass( Complex *, Complex *, Stype *, 
		float, int *, float *, float * );

	p = (Complex *)calloc( 30, sizeof(Complex));
	z = (Complex *)calloc( 30, sizeof(Complex));
	stype = (Stype *)calloc( 30, sizeof(Stype));

/** analog prototype selection **/
	if( strcmp( filter_type, "BU") == 0 ) 
	{
		*nsects = butterworth_roots( iord, &p[0], stype, &dcvalue );
	}
	else if( strcmp( filter_type, "BE") == 0 )
	{
		bessel_roots( &p[0], stype, &dcvalue, nsects, iord );
	}
	else if( strcmp( filter_type, "C1") == 0 )
	{
		chebyshev_parameters( a, trbndw, iord, &eps, &ripple );
		chebyshev1_roots( &p[0], stype, &dcvalue, nsects, iord, eps );
	}
	else if( strcmp( filter_type, "C2") == 0 )
	{
		omegar = 1 + trbndw;
		chebyshev2_roots( &p[0], &z[0], stype, &dcvalue, nsects, iord, a, omegar );
	}
	else
	{
		fprintf(stderr, "Fatal Error: filter_type=%s unknown.\n", filter_type );
		fprintf(stderr, "\t Known types: BU=Butterworth      BE=Bessel\n");
		fprintf(stderr, "\t              C1=Chebyshev type I C2=Chebyshev Type II\n");
		exit(-1);
	}

/** analog mapping selection **/
	if( strcmp( operation_type, "BP" ) == 0 )
	{
		flw = warp( fl*ts/2.0, 2.0 );
		fhw = warp( fh*ts/2.0, 2.0 );
		lowpass_to_bandpass( &p[0], &z[0], dcvalue, nsects, flw, fhw, &sn[0], &sd[0], stype );
	}
	else if( strcmp( operation_type, "BR" ) == 0 )
	{
		flw = warp( fl*ts/2.0, 2.0 );
		fhw = warp( fh*ts/2.0, 2.0 );
		lowpass_to_bandreject( &p[0], &z[0], stype, dcvalue, nsects, flw, fhw, &sn[0], &sd[0] );
	}
	else if( strcmp( operation_type, "LP" ) == 0 )
	{
		fhw = warp( fh*ts/2.0, 2.0 );
		lowpass( &p[0], &z[0], stype, dcvalue, nsects, &sn[0], &sd[0] );
		cutoffs( &sn[0], &sd[0], nsects, fhw );
	}
	else if( strcmp( operation_type, "HP" ) == 0 )
	{
		flw = warp( fl*ts/2.0, 2.0 );
		lowpass_to_highpass( &p[0], &z[0], stype, dcvalue, nsects, &sn[0], &sd[0] );
		cutoffs( &sn[0], &sd[0], nsects, flw );
	}
	else
	{
		fprintf(stderr, "Fatal Error: filter operation type=%s unknown.\n", operation_type );
		fprintf(stderr, "\t BP=Bandpass HP=Highpass LP=Lowpass BR=Bandreject\n" );
                exit(-1);
	}

/*** bilinear analog to digitial transformation ***/
	bilinear( &sn[0], &sd[0], *nsects );

	free(p);
	free(z);
}

void lowpass_to_bandpass( Complex *p, Complex *z, float dcvalue, int *nsects, 
	float fl, float fh, float *sn, float *sd, Stype *stype )
{
	Complex ctemp,p1,p2,z1,z2,s,h;
	Complex aarg,barg,carg;
	float twopi,pi,a,b,scale;
	int i,n,iptr;

	pi = M_PI;
	twopi = 2*pi;
	a = twopi * twopi * fl * fh;
	b = twopi * ( fh - fl );

	n = *nsects;
	*nsects = 0;
	iptr = 1;
	for( i=1; i<=n; i++ )
	{
		if( strcmp( stype[i].name, "CPZ" ) == 0 ) 
		{
			barg     = c_scale( z[i], b );
			carg     = c_mult( barg, barg );
			ctemp.re = carg.re - 4.0*a;
			ctemp.im = carg.im;
			ctemp = c_sqrt( ctemp );

			z1.re = 0.5 * ( b*z[i].re + ctemp.re );
			z1.im = 0.5 * ( b*z[i].im + ctemp.im );
			z2.re = 0.5 * ( b*z[i].re - ctemp.re );
			z2.im = 0.5 * ( b*z[i].im - ctemp.im );

			barg     = c_scale( p[i], b );
			carg     = c_mult( barg, barg );
			ctemp.re = carg.re - 4.0*a;
			ctemp.im = carg.im;
			ctemp = c_sqrt( ctemp );

			p1.re = 0.5 * ( b * p[i].re + ctemp.re );
			p1.im = 0.5 * ( b * p[i].im + ctemp.im );
			p2.re = 0.5 * ( b * p[i].re - ctemp.re );
                        p2.im = 0.5 * ( b * p[i].im - ctemp.im );

			aarg         = c_mult(z1, c_conjg(z1));
			sn[ iptr ]   = aarg.re;
			sn[ iptr+1 ] = -2 * z1.re;
			sn[ iptr+2 ] =  1;
			aarg         = c_mult(p1, c_conjg(p1));
			sd[ iptr ]   = aarg.re;
			sd[ iptr+1 ] = -2. * p1.re;
			sd[ iptr+2 ] =  1;
			iptr = iptr + 3;
			aarg         = c_mult(z2, c_conjg(z2));
			sn[ iptr ]   = aarg.re;
                        sn[ iptr+1 ] = -2 * z2.re;
                        sn[ iptr+2 ] =  1;
			aarg         = c_mult(p2, c_conjg(p2));
                        sd[ iptr ]   = aarg.re;
                        sd[ iptr+1 ] = -2. * p2.re;
                        sd[ iptr+2 ] =  1;
			iptr = iptr + 3;
			*nsects = *nsects + 2;
		}
		else if( strcmp( stype[i].name, "CP" ) == 0 )
		{
			barg     = c_scale( p[i], b );
			carg     = c_mult( barg, barg );
			ctemp.re = carg.re - 4.0*a;
			ctemp.im = carg.im;
			ctemp = c_sqrt( ctemp );
			p1.re = 0.5 * ( b * p[i].re + ctemp.re );
			p1.im = 0.5 * ( b * p[i].im + ctemp.im );
			p2.re = 0.5 * ( b * p[i].re - ctemp.re );
			p2.im = 0.5 * ( b * p[i].im - ctemp.im );
			sn[ iptr ]   = 0;
			sn[ iptr+1 ] = b;
			sn[ iptr+2 ] = 0;
			aarg         =  c_mult( p1, c_conjg(p1) );
			sd[ iptr ]   =  aarg.re;
			sd[ iptr+1 ] = -2 * p1.re;
			sd[ iptr+2 ] = 1;
			iptr = iptr + 3;
			sn[ iptr ]   = 0;
			sn[ iptr+1 ] = b;
			sn[ iptr+2 ] = 0;
			aarg         =  c_mult( p2, c_conjg(p2) );
			sd[ iptr ]   = aarg.re;
			sd[ iptr+1 ] = -2. * p2.re;
			sd[ iptr+2 ] = 1;
			iptr = iptr + 3;
			*nsects = *nsects + 2;
		}
		else if ( strcmp( stype[i].name, "SP" ) == 0 )
		{
			sn[ iptr ]   = 0;
			sn[ iptr+1 ] = b;
			sn[ iptr+2 ] = 0;
			sd[ iptr ]   = a;
			sd[ iptr+1 ] = -b*p[i].re;
			sd[ iptr+2 ] = 1;
			iptr = iptr + 3;
			*nsects = *nsects + 1;
		}
	}
/*** Scaling - use the fact that the bandpass filter amplitude at sqrt( omega_l )
     equals the amplitude of the lowpass prototype at d.c. ***/
	s.re = 0;	s.im = sqrt(a);
	h.re = 1;	h.im = 0;
	iptr = 1;
	for( i=1; i<= *nsects; i++ )
	{
		aarg    = c_scale( s, sn[iptr+2] );
		aarg.re = aarg.re + sn[iptr+1];
		barg    = c_mult( s, aarg );
		barg.re = barg.re + sn[iptr];
		carg    = c_mult( h, barg );
		aarg    = c_scale( s, sd[iptr+2] );
		aarg.re = aarg.re + sd[iptr+1];
		barg    = c_mult( s, aarg );
 		barg.re = barg.re + sd[iptr];
		h       = c_div( carg, barg );
		iptr = iptr + 3;
	}
	aarg = c_conjg( h );
	scale = dcvalue / sqrt( h.re * aarg.re );

	sn[1] = sn[1] * scale;
	sn[2] = sn[2] * scale;
	sn[3] = sn[3] * scale;
}

void cutoffs( float *sn, float *sd, int *nsects, float f )
{
        float scale,pi;
        int i,iptr;
                                                                                                              
        pi = M_PI;
        scale = 2 * pi * f;
        iptr = 1;
        for( i=1; i <= *nsects; i++ )
        {
                sn[ iptr + 1 ] = sn[ iptr + 1 ]/scale;
                sn[ iptr + 2 ] = sn[ iptr + 2 ]/(scale*scale);
                sd[ iptr + 1 ] = sd[ iptr + 1 ]/scale;
                sd[ iptr + 2 ] = sd[ iptr + 2 ]/(scale*scale);
                iptr = iptr + 3;
        }
}

void bessel_roots( Complex *p, Stype *stype, float *dcvalue, int *nsects, int iord )
{
        if( iord == 1 )
        {
                p[1].re = -1;
                p[1].im =  0;
		sprintf( stype[1].name, "SP" );
        }
        else if( iord == 2 )
        {
                p[1].re = -1.0474091;
                p[1].im =  0.6360098;
		sprintf( stype[1].name, "CP" );
        }
        else if( iord == 3 )
        {
                p[1].re = -1.0474091;
                p[1].im =  0.9992645;
                p[2].re = -1.3226758;
                p[2].im =  0;
                sprintf( stype[1].name, "CP" );
		sprintf( stype[2].name, "SP" );
        }
        else if( iord == 4 )
        {
                p[1].re = -0.9952088;
                p[1].im =  1.2571058;
                p[2].re = -1.3700679;
                p[2].im =  0.4102497;
		sprintf( stype[1].name, "CP" );
		sprintf( stype[2].name, "CP" );
        }
        else if( iord == 5 )
        {
                p[1].re = -0.9576766;
                p[1].im =  1.4711244;
                p[2].re = -1.3808774;
                p[2].im =  0.7179096;
                p[3].re = -1.5023160;
                p[3].im =  0;
		sprintf( stype[1].name, "CP" );
		sprintf( stype[2].name, "CP" );
		sprintf( stype[3].name, "SP" );
        }
        else if( iord == 6 )
        {
                p[1].re = -0.9306565;
                p[1].im =  1.6618633;
                p[2].re = -1.3818581;
                p[2].im =  0.9714719;
                p[3].re = -1.5714904;
                p[3].im =  0.3208964;
		sprintf( stype[1].name, "CP" );
		sprintf( stype[2].name, "CP" );
		sprintf( stype[3].name, "CP" );
        }
        else if( iord == 7 )
        {
                p[1].re = -0.9098678;
                p[1].im =  1.8364514;
                p[2].re = -1.3789032;
                p[2].im =  1.1915667;
                p[3].re = -1.6120388;
                p[3].im =  0.5892445;
                p[4].re = -1.6843682;
                p[4].im =  0;
		sprintf( stype[1].name, "CP" );
		sprintf( stype[2].name, "CP" );
		sprintf( stype[3].name, "CP" );
		sprintf( stype[4].name, "SP" );
        }
        else if( iord == 8 )
        {
                p[1].re = -0.8928710;
                p[1].im =  1.9983286;
                p[2].re = -1.3738431;
                p[2].im =  1.3883585;
                p[3].re = -1.6369417;
                p[3].im =  0.8227968;
                p[4].re = -1.7574108;
                p[4].im =  0.2728679;
		sprintf( stype[1].name, "CP" );
		sprintf( stype[2].name, "CP" );
		sprintf( stype[3].name, "CP" );
		sprintf( stype[4].name, "CP" );
        }
        *nsects = iord - iord/2;
        *dcvalue = 1;
}

void chebyshev1_roots( Complex *p, Stype *stype, float *dcvalue, 
	int *nsects, int iord, float eps )
{
	int ns, i, half;
	float pi,gamma,s,c,angle,sigma,omega;
	pi = M_PI;
	half = iord/2;  /** default C uses floor and does not round **/

/*** intermediate parameters ***/
        gamma = ( 1 + sqrt( 1 + eps*eps ) )/eps;
        gamma = log( gamma ) / iord;
        gamma = exp( gamma );
        s = 0.5 * ( gamma - 1/gamma );
        c = 0.5 * ( gamma + 1/gamma );

/*** calculate poles ***/
        ns = 0;
        for( i=1; i<= half; i++ )
        {
		sprintf( stype[1].name, "CP" );
                angle = (2*i-1) * pi/(2*iord);
                sigma = -s * sin(angle);
                omega =  c * cos(angle);
                p[i].re = sigma;
                p[i].im = omega;
                ns++;
        }
        if( 2*half < iord )
        {
		sprintf( stype[half+1].name, "SP" );
                p[half+1].re = -s;
                p[half+1].im =  0;
                ns++;
                *dcvalue = 1;
        }
        else
        {
                *dcvalue = 1/sqrt( 1 + eps*eps );
        }
	*nsects = ns;
}

void chebyshev2_roots( Complex *p, Complex *z, Stype *stype, 
	float *dcvalue, int *nsects, int iord, float a, float omegar )
{
        int ns, half, i;
        float pi,gamma,s,c,angle,alpha,beta,denom,omega,sigma;

        pi = M_PI;
        half = iord/2; /** default C uses floor and does not round **/

        gamma = ( a + sqrt( a*a - 1 ) );
        gamma = log( gamma )/iord;
        gamma = exp(gamma);
        s = 0.5 * ( gamma-1/gamma );
        c = 0.5 * ( gamma+1/gamma );

	ns = 0;
        for( i=1; i<= half; i++ )
        {
   /*** calculate poles ***/
		sprintf( stype[i].name, "CPZ" );
                angle = (2*i-1) * pi/(2*iord);
                alpha = -s * sin(angle);
                beta  =  c * cos(angle);
                denom =  alpha * alpha + beta * beta;
                sigma =  omegar * alpha/denom;
                omega =  -omegar * beta / denom;
                p[i].re = sigma;
                p[i].im = omega;
        /*** calculate zeros ***/
                omega = omegar/cos(angle);
                z[i].re = 0;
                z[i].im = omega;
                ns++;
        }
/*** odd order filters ***/
        if( (2*half) < iord )
        {
		sprintf( stype[half+1].name, "SP" );
                p[half+1].re = -omegar/s;
                p[half+1].im = 0;
		z[half+1].re = 0;
		z[half+1].im = 0;
                ns++;
        }
        *dcvalue = 1;
	*nsects = ns;
}
                                                                                                                                         
void chebyshev_parameters( float a, float trbndw, 
	int iord, float *eps, float *ripple )
{
        float omegar, alpha, g;

        omegar = 1 + trbndw;
        alpha = pow( omegar + sqrt( omegar*omegar - 1 ), iord );
        g = ( alpha*alpha + 1 ) / ( 2 * alpha );
        *eps = sqrt( a*a - 1 ) / g;
        *ripple = 1 / sqrt( 1 + *eps * *eps );
}

void lowpass_to_highpass( Complex *p, Complex *z, Stype *stype, 
	float dcvalue, int *nsects, float *sn, float *sd )
{
	int i,iptr;
	Complex aarg,barg,carg;
	float scale;
	
	iptr = 1;
	for( i=1; i<= *nsects; i++ )
	{
		if( strcmp( stype[i].name, "CPZ" ) == 0 )
		{
			aarg         = c_mult( p[i], c_conjg(p[i]) );
			barg         = c_mult( z[i], c_conjg(z[i]) );
			scale        = c_real( aarg ) / c_real( barg );
			sn[ iptr ]   = scale;
			sn[ iptr+1 ] = -2 * z[i].re * scale;
			aarg = c_mult( z[i], c_conjg( z[i] ) );
			sn[ iptr+2 ] = aarg.re * scale;
			sd[ iptr ]   = 1;
			sd[ iptr+1 ] = -2 * p[i].re;
			barg         = c_mult( p[i], c_conjg(p[i]) );
			sd[ iptr+2 ] = barg.re;
			iptr = iptr + 3;
		}
		else if( strcmp( stype[i].name, "CP" ) == 0 )
		{
			barg         = c_mult( p[i], c_conjg(p[i]) );
			scale        = barg.re;
			sn[ iptr ]   = 0;
			sn[ iptr+1 ] = 0;
			sn[ iptr+2 ] = scale;
			sd[ iptr ]   = 1;
			sd[ iptr+1 ] = -2 * p[i].re;
			barg         = c_mult( p[i], c_conjg(p[i]) );
			sd[ iptr+2 ] = barg.re;
                  	iptr = iptr + 3;
		}
		else if( strcmp( stype[i].name, "SP" ) == 0 )
		{
			scale        = -p[i].re;
			sn[ iptr ]   = 0;
			sn[ iptr+1 ] = scale;
			sn[ iptr+2 ] = 0;
			sd[ iptr ]   = 1;
			sd[ iptr+1 ] = -p[i].re;
			sd[ iptr+2 ] = 0;
			iptr = iptr + 3;
		}
	}
        sn[1] = sn[1] * dcvalue;
	sn[2] = sn[2] * dcvalue;
	sn[3] = sn[3] * dcvalue;
}

void lowpass( Complex *p, Complex *z, Stype *stype, float dcvalue, int *nsects, float *sn, float *sd )
{
	int i,iptr;
	float scale;
	Complex aarg, barg, carg;

	iptr = 1;
	for( i=1; i<= *nsects; i++ )
	{
		if( strcmp( stype[i].name, "CPZ" ) == 0 )
		{
			aarg         = c_mult( p[i], c_conjg(p[i]) );
			barg         = c_mult( z[i], c_conjg(z[i]) );
			scale        = aarg.re / barg.re ;

			aarg         = c_mult( z[i], c_conjg(z[i]) );
			sn[ iptr ]   = aarg.re * scale;
			sn[ iptr+1 ] = -2 * z[i].re * scale;
			sn[ iptr+2 ] = scale;

			aarg         = c_mult( p[i], c_conjg(p[i]) );
			sd[ iptr ]   = aarg.re;
			sd[ iptr+1 ] = -2 * p[i].re;
			sd[ iptr+2 ] = 1;
			iptr = iptr + 3;
		}
		else if( strcmp( stype[i].name, "CP" ) == 0 )
		{
			aarg         = c_mult( p[i], c_conjg(p[i]) );
			scale        = aarg.re;
			sn[ iptr ]   = scale;
			sn[ iptr+1 ] = 0;
			sn[ iptr+2 ] = 0;
			aarg         = c_mult( p[i], c_conjg(p[i]) );
			sd[ iptr ]   = aarg.re;
			sd[ iptr+1 ] = -2 * p[i].re;
			sd[ iptr+2 ] = 1;
			iptr = iptr + 3;
		}
		else if( strcmp( stype[i].name, "SP" ) == 0 )
		{
			scale = -p[i].re;
			sn[ iptr ]   = scale;
			sn[ iptr+1 ] = 0;
			sn[ iptr+2 ] = 0;
			sd[ iptr ]   = -p[i].re;;
			sd[ iptr+1 ] = 1;
			sd[ iptr+2 ] = 0;
			iptr = iptr + 3;
		}
	}
	sn[1] = dcvalue * sn[1];
	sn[2] = dcvalue * sn[2];
	sn[3] = dcvalue * sn[3];
}

void lowpass_to_bandreject( Complex *p, Complex *z, Stype *stype, 
	float dcvalue, int *nsects, float fl, float fh, float *sn, float *sd )
{
        int n,i,iptr;
        float twopi,pi,a,b,scale,h;
        Complex cinv, ctemp, p1, p2, z1, z2, aarg, barg, carg;
	Complex one;
	/* Complex cone; */

	pi = M_PI;
	twopi = 2*pi;
	a = twopi * twopi * fl * fh;
	b = twopi * ( fh - fl );
	n = *nsects;
	*nsects = 0;

        iptr = 1;
	one.re = 1; one.im = 0;
	/* cone.re = 0; cone.im = 1; */

        for( i=1; i<=n; i++ )
        {
                if( strcmp( stype[i].name, "CPZ" ) == 0 )
                {
			cinv    = c_div( one, z[i] );
			aarg    = c_scale( cinv, b );
			barg    = c_mult( aarg, aarg );
			carg.re = barg.re - 4 * a;
			ctemp   = c_sqrt( carg );
			z1.re   = 0.5 * ( b*cinv.re + ctemp.re );
			z1.im   = 0.5 * ( b*cinv.im + ctemp.im );
			z2.re   = 0.5 * ( b*cinv.re - ctemp.re );
			z2.im   = 0.5 * ( b*cinv.im - ctemp.im );
			
			cinv    = c_div( one, p[i] );
			aarg    = c_scale( cinv, b );
			barg    = c_mult( aarg, aarg );
			carg.re = barg.re - 4 * a;
			ctemp   = c_sqrt( carg );
			p1.re   = 0.5 * ( b*cinv.re + ctemp.re );
			p1.im   = 0.5 * ( b*cinv.im + ctemp.im );
			p2.re   = 0.5 * ( b*cinv.re - ctemp.re );
			p2.im   = 0.5 * ( b*cinv.im - ctemp.im );

			aarg         = c_mult( z1, c_conjg( z1 ) );
			sn[ iptr ]   = aarg.re;
			sn[ iptr+1 ] = -2. * z1.re;
			sn[ iptr+2 ] =  1;

			aarg         = c_mult( p1, c_conjg( p1 ) );
			sd[ iptr ]   = aarg.re;
			sd[ iptr+1 ] = -2 * p1.re;
			sd[ iptr+2 ] = 1;
			
			iptr = iptr + 3;

			aarg         = c_mult( z2, c_conjg( z2 ) );
			sn[ iptr ]   = aarg.re;
			sn[ iptr+1 ] = -2. * z2.re;
			sn[ iptr+2 ] =  1;

			aarg         = c_mult( p2, c_conjg( p2 ) );
			sd[ iptr ]   = aarg.re;
			sd[ iptr+1 ] = -2 * p2.re;
			sd[ iptr+2 ] = 1;

			iptr = iptr + 3;
			*nsects = *nsects + 2;
		}
		else if( strcmp( stype[i].name, "CP" ) == 0 )
		{
			cinv    = c_div( one, p[i] );
			aarg    = c_scale( cinv, b );
			barg    = c_mult( aarg, aarg );
			carg.re = barg.re - 4 * a;
			ctemp   = c_sqrt( carg );
			
			p1.re = 0.5 * ( b*cinv.re + ctemp.re );
			p1.im = 0.5 * ( b*cinv.im + ctemp.im );
			p2.re = 0.5 * ( b*cinv.re - ctemp.re );
			p2.im = 0.5 * ( b*cinv.im - ctemp.im );

			sn[ iptr ]   = a;
			sn[ iptr+1 ] = 0;
			sn[ iptr+2 ] = 1;
	
			aarg         = c_mult( p1, c_conjg( p1 ) );
			sd[ iptr ]   = aarg.re;
			sd[ iptr+1 ] = -2 * p1.re;
			sd[ iptr+2 ] = 1;
			iptr = iptr + 3;
			
			sn[ iptr ]   = a;
			sn[ iptr+1 ] = 0;
			sn[ iptr+2 ] = 1;
			aarg         = c_mult( p2, c_conjg( p2 ) );
			sd[ iptr ]   = aarg.re;
			sd[ iptr+1 ] = -2 * p2.re;
			sd[ iptr+2 ] = 1;
			iptr = iptr + 3;
			*nsects = *nsects + 2;
		}
		else if( strcmp( stype[i].name, "SP" ) == 0 )
		{
			sn[ iptr ]   = a;
			sn[ iptr+1 ] = 0;
			sn[ iptr+2 ] = 1;
			sd[ iptr ]   = -a * p[i].re;
			sd[ iptr+1 ] = b;
			sd[ iptr+2 ] = -p[i].re;
			iptr = iptr + 3;
			/* *nsects++; */
		}
	}
	h = 1;
	iptr = 1;
	for( i=1; i <= *nsects; i++ )
	{
		h = h * sn[iptr] / sd[iptr];
		iptr = iptr + 3;
	}
	scale = dcvalue / fabs(h);
	sn[1] = sn[1] * scale;
	sn[2] = sn[2] * scale;
	sn[3] = sn[3] * scale;
}
