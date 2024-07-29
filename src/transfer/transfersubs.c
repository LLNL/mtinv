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
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "../../include/sac.h"
#include "../../include/complex.h"

extern char progname[128];

void transfer_response( float *data, int npts, float delta, char *sacpzfile, int verbose )
{
	double *sre, *sim, *xre, *xim;
	double srej,srei,simj,simi,xrej,ximj,denr,aa,bb,df,scale,freq;
	int i, j, nfft, nfreq;
	int next_power_of_2( int );
	Dcomplex *s;
	double min,max,mean;
	float fmin, fmax, fmean;

	void polezero( int, double, double *, double *, char *, int );
	void sac_dminmax( double *, int, double *, double *, double * );
	void sac_minmax( float *, int, float *, float *, float * );
	void fft( int, Dcomplex *, double, double );

	nfft  = next_power_of_2( npts );
	nfreq = nfft/2 + 1;
	df    = 1/((double)nfft * (double)delta);
	if( verbose )
	{
	  fprintf(stdout, "%s: transfer_response(): allocating memory: ", progname );
	  fprintf(stdout, " nfft=%d nfreq=%d npts=%d dt=%g df=%g\n",
			nfft, nfreq, npts, delta, df );
	}

	xre = (double *)   calloc( nfreq*2, sizeof(double) );
	xim = (double *)   calloc( nfreq*2, sizeof(double) );
	sre = (double *)   calloc( nfft*2,  sizeof(double) );
	sim = (double *)   calloc( nfft*2,  sizeof(double) );
	s   = (Dcomplex *) calloc( nfft*2, sizeof(Dcomplex));
	for( i=0; i<nfreq; i++ ) xre[i]=1.;

/*** read in SAC Pole Zero file and compute spectral response from Poles and zeros ***/
	polezero( nfreq, df, xre, xim, sacpzfile, verbose );

	if( verbose )
	{
		sac_dminmax( xre, nfreq, &max, &min, &mean ); 
		fprintf(stdout, "%s: transfer_response(): xre min=%g max=%g mean=%g\n", 
			progname,min,max,mean);

		sac_dminmax( xim, nfreq, &max, &min, &mean ); 
		fprintf(stdout, "%s: transfer_response(): xim min=%g max=%g mean=%g\n", 
			progname, min,max,mean);
	}

/*** transform the data into spectra ***/
	for( i=0; i<npts; i++ )
	{
		s[i].re = (double)data[i];
		s[i].im = 0.;
	}
	sac_minmax( data, npts, &fmax, &fmin, &fmean );
	if(verbose)
	{
	  fprintf(stdout, "%s: transfer_response(): data min=%g max=%g mean=%g\n", 
		progname, fmin, fmax, fmean);
	}

/***  Fast Fourier Transform the data ***/
	/* scale = sqrt( 1.0/(double)nfft * ((double)delta/df) ); */	
	scale = delta;
	if(verbose) fprintf(stdout,"nfft=%d scale=%g\n", nfft, scale);
	fft( nfft, s, FORWARD, scale );
	if( verbose )
	{
		for( i=1; i<nfft; i++ )
		{
			sre[i] = s[i].re;
			sim[i] = s[i].im;
		}
		sac_dminmax( sre, nfreq, &max, &min, &mean ); 
		fprintf(stdout, "%s: transfer_response(): sre min=%g max=%g mean=%g\n", 
			progname, min,max,mean);

        	sac_dminmax( sim, nfreq, &max, &min, &mean ); 
		fprintf(stdout, "%s: transfer_response(): sim min=%g max=%g mean=%g\n", 
			progname, min,max,mean);
	}

/*** Multiply by the transfer operator with the data ***/
	for( i=1; i<nfreq; i++ )
	{
		srej = s[i].re;
		simj = s[i].im;
		xrej = xre[i];
		ximj = xim[i];
		aa   = xrej * srej - ximj * simj;
		bb   = xrej * simj + ximj * srej;
	
		s[i].re = aa;
		s[i].im = bb;
		sre[i]=aa;
		sim[i]=bb;
		j = nfft - i;
		s[j].re = aa;
		s[j].im = -bb;
		sre[j]=aa;
		sim[j]=-bb;
	}
	s[0].re = 0;
	s[0].im = 0;
	s[nfreq-1].re = sqrt( (s[nfreq-1].re*s[nfreq-1].re) + (s[nfreq-1].im*s[nfreq-1].im) );
	s[nfreq-1].im = 0.;

	sre[0] = 0.;
	sim[0] = 0.;
	sre[nfreq-1] = s[nfreq-1].re;
	sim[nfreq-1] = s[nfreq-1].im;
	if( verbose )
	{
		sac_dminmax( sre, nfft, &max, &min, &mean ); 
		fprintf(stdout, "%s: transfer_response(): sre min=%g max=%g mean=%g\n", 
			progname, min,max,mean);

        	sac_dminmax( sim, nfft, &max, &min, &mean ); 
		fprintf(stdout, "%s: transfer_response(): sim min=%g max=%g mean=%g\n", 
			progname, min,max,mean);
	}

/*** Inverse Fast Fourier Transfrom ***/
	/* scale = sqrt( (1.0/(double)nfft) * (df/(double)delta) ); */
	scale = delta;
	fft( nfft, s, INVERSE, scale );
	for( i=0; i<npts; i++ ) data[i] = s[i].re;

	free(s);
	free(sre);
	free(sim);
	free(xre);
	free(xim);
}

void sac_dminmax( double *x, int n, double *max, double *min, double *mean )
{
        int i;
        double sum=0.;

        *max = x[0];
        *min = x[0];
        *mean = x[0];
        for( i=1; i<n; i++ )
        {
                if( x[i] > *max ) *max=x[i];
                if( x[i] > *min ) *min=x[i];
                sum = sum + x[i];
        }
        *mean = sum/(double)n;
}

int next_power_of_2( int nt )
{
	float a;
	int nout, npower;

	a = nt - 1;
	npower = (int)( log(a)/log(2)  ) + 1;
	nout = pow( 2, npower );
	return nout;
}

#define POLES_ID 1
#define ZEROS_ID 2
#define CONST_ID 3
#define UNKNOWN  4
int getid( char *rec )
{
	int i;
	char c;
	for( i=0;i<9;i++ ) 
	{
		c=toupper( rec[i] );
		rec[i]=c;
	}
	if(strncmp(rec, "CONSTANT", 8 ) == 0 )	return CONST_ID;
	if(strncmp(rec, "POLES", 5 ) == 0 ) 	return POLES_ID;
	if(strncmp(rec, "ZEROS", 5 ) == 0 ) 	return ZEROS_ID;
	return UNKNOWN;
}

void polezero( int nfreq, double df, double *xre, double *xim, char *sacpzfile, int verbose )
{
	int i, iz, ip, nz=0, np=0, nitems, id, getid( char *rec ), last_item;
	Dcomplex *z;
	Dcomplex *p;
	char rec[128], dum[32];
	FILE *fp;
	double denr, constant=1;
	void getran( int, double, double, int, Dcomplex *, int, Dcomplex *, double *, double * );

	if( (fp=fopen( sacpzfile, "r" )) == NULL )
	{
		fprintf(stderr, "%s: polezero(): Error opening file %s\n", 
			progname, sacpzfile );
		exit(-1);
	}
	while( fgets( rec, 128, fp ) != NULL )
	{
		if( rec[0] == '*' )
		{
			if(verbose)
				fprintf( stdout, "comment=%s", rec );
			continue;
		}

		id = getid( rec );
		switch( id )
		{
			case CONST_ID:
				sscanf( rec, "%s %lf", dum, &constant );
				last_item = id;
				break;
			case POLES_ID:
				sscanf( rec, "%s %d", dum, &np );
				ip = 1;
				p = (Dcomplex *)calloc( np+1, sizeof(Dcomplex) );
				last_item = id;
				break;
			case ZEROS_ID:
				sscanf( rec, "%s %d", dum, &nz );
				z = (Dcomplex *)calloc( nz+1, sizeof(Dcomplex) );
				for( iz=0;iz<=nz;iz++ )
				{
					z[iz].re = 0; z[iz].im = 0;
				}	
				iz = 1;
				last_item = id;
				break;
			case UNKNOWN:
				if( last_item == POLES_ID )
				{
					nitems = sscanf(rec, "%lf %lf", &(p[ip].re), &(p[ip].im) );
					if( nitems != 2 )
					{
					  fprintf(stderr, "ERROR reading file %s\n\t%s\n", sacpzfile, rec );
					  exit(-1);
					}
					ip++;
				}
				if( last_item == ZEROS_ID )
				{
					nitems = sscanf(rec, "%lf %lf", &(z[iz].re), &(z[iz].im) );
					if( nitems != 2 )
					{
					  fprintf(stderr, "ERROR reading file %s\n\t%s\n", sacpzfile, rec );
					  exit(-1);
					}
					iz++;
				}
				break;
			default  :
				/*** never suppose to be here ***/
				fprintf(stderr, "ERROR reading file %s\n\t%s\n", sacpzfile, rec );
				exit(-1);
			
		}
	}
	fclose(fp);

	if( verbose ) /*** write out input ***/
	{
		fprintf( stdout, "%s: polezero(): file=%s npoles=%d nzeros=%d constant=%g\n", 
			progname, sacpzfile, np, nz, constant );
		printf("\tPOLES:\n");
		for( ip=1; ip<= np; ip++ ) 
			fprintf( stdout, "\t\t%d %g %g\n", ip, p[ip].re, p[ip].im );
		printf("\tZEROS:\n");
		for( iz=1; iz<= nz; iz++ ) 
			fprintf( stdout, "\t\t%d %g %g\n", iz, z[iz].re, z[iz].im );
	}

/*** convert poles and zeros to spectrum ***/
	getran( nfreq, df, constant, nz, z, np, p, xre, xim );

	free(z);
	free(p);
}

void getran( int nfreq, double df, double constant, 
    int nz, Dcomplex *z, int np, Dcomplex *p, double *xre, double * xim )
{
	double twopi, delomg, omega, trn, tin, tr, ti, tr0, ti0, trd, tid, fac, denr;
	int i, j;

	twopi = M_PI*2.0;
	twopi = 6.283185307179586;
	delomg = twopi * df;
	for( j=0; j<nfreq; j++ )
	{
		omega = delomg * (double)j;
		trn = 1.;
		tin = 0.;
		if( nz != 0 ) 
		{
			for( i=1; i<=nz; i++ )
			{
				tr = -z[i].re;
				ti = omega - z[i].im;
				tr0 = trn * tr - tin * ti;
				ti0 = trn * ti + tin * tr;
				trn = tr0;
				tin = ti0;
			}
		}
		trd = 1;
		tid = 0;
	
		if( np != 0 )
		{
			for( i=1; i<=np; i++ )
			{
				tr = - p[i].re;
				ti = omega - p[i].im;
				tr0 = trd * tr - tid * ti;
				ti0 = trd * ti + tid * tr;
				trd = tr0;
				tid = ti0;
			}
		}

		fac = constant / ( (trd*trd) + (tid*tid) );
		xre[j] = fac * ( trn * trd + tin * tid );
		xim[j] = fac * ( trd * tin - trn * tid );
	}
/*** normalize spectrum undefined at i=0 ***/
	for( i=1; i<nfreq; i++ )
	{
		denr = 1/( (xre[i]*xre[i]) + (xim[i]*xim[i]) );
		xre[i] = xre[i] * denr;
		xim[i] = xim[i] * denr;   /*** old version xim[i] = xim[i] * -denr; ***/
	}
}
