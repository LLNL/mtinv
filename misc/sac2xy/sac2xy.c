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

/***
	Original found in /Users/ichinose1/Work/mtinv.v3.0.5/misc/sac2xy

        this is needed by Greens_subs.c & compare_grn_gmt.c - converts SAC files into (time,amplitude) pairs
        for waveform plots, Greens function plots, etc...
***/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strings.h>

#include "sac.h"
#include "mytime.h"
#include "version.h"

char progname[256];

int main( int ac, char **av)
{
	Sac_Header s;
	int i, xreset=FALSE;
	float *x, *y;
	int norm = 0, xdeg = 0, xkm = 0;
	float ymax, scale = 1.0, ydist = 0;
	int setpar(int, char **);
	int getpar( char *, char *, void * );
	void endpar();
	int verbose = 0;
	int normfac = 0;
	int ifixZeroDiv = 0;
	float xmax;
	float tshift = 0;

/*** timesubs.c ***/
	MyTime ref; /*** sac file reference time ***/
	MyTime ot;  /*** the zero time on the plot ***/
	char ot_string[32];
	void WriteMyTime2STDOUT( MyTime *t );
	void WriteMyTime2STDERR( MyTime *t );
	void parsestring( MyTime *t, char *str );
	MyTime *sac2mytime( MyTime *t, Sac_Header *sp );

	strcpy( ot_string, "\0" );

/*** begin main ***/

	sprintf( progname, "%s", av[0] );

	fprintf( stderr, "%s: %s: %s: Version      = %s\n", progname, __FILE__, __func__, Version_Label );
        fprintf( stderr, "%s: %s: %s: Version Date = %s\n", progname, __FILE__, __func__, Version_Date );
        fprintf( stderr, "%s: %s: %s: Source       = %s\n", progname, __FILE__, __func__, Version_SrcDir );

	setpar( ac, av );
	getpar( "xreset", "b", &xreset);
	getpar( "tshift", "f", &tshift );
	getpar( "norm",   "b", &norm );
	getpar( "sc",     "f", &scale );
	getpar( "xdeg",   "b", &xdeg );
	getpar( "xkm",     "b", &xkm );
	getpar( "normfac", "b", &normfac );
	getpar( "verbose", "b", &verbose );
	getpar( "fixZeroDiv", "b", &ifixZeroDiv );
	getpar( "ot", "s", ot_string );
	endpar();

/*** end getpar ***/

	if( xdeg && xkm )
	{
		fprintf( stderr, "%s: error only specify one xdeg=%d or xkm=%d\n",
			progname, xdeg, xkm );
		exit(-1);
	}

	if(verbose)
	{
	  fprintf( stderr, "%s: xreset=%d norm=%d sc=%g xdeg=%d xkm=%d ifixZeroDiv=%d\n",
		progname, xreset, norm, scale, xdeg, xkm, ifixZeroDiv );
	}

/*** read the data and allocate memory ***/

	fread(&s, sizeof(Sac_Header), 1, stdin);
	x = (float *)calloc(s.npts, sizeof(float));
        y = (float *)calloc(s.npts, sizeof(float));
        fread(&y[0], s.npts*sizeof(float), 1, stdin);

	if(verbose) fprintf( stderr, "%s: npts=%d delta=%g b=%g sta=%s\n",
		progname, s.npts, s.delta, s.b, s.kstnm );

/*** set the zero time ***/
	if( strcmp( ot_string, "\0" ) != 0 )
        {
                parsestring( &ot, ot_string );
                sac2mytime( &ref, &s );
	/*
		WriteMyTime2STDERR( &ot );
		WriteMyTime2STDERR( &ref );
	*/
		s.b = (ref.epoch - ot.epoch) + s.b + tshift;
        }
        else
        {
	/*** reset the x-axes to 0 and add the tshift - time shift ***/
        	s.b = s.b + tshift;
        }

	/* if( s.iftype == ITIME ) s.b=0; */
	if( xreset == TRUE ) s.b=0;
	xmax = s.b + ( s.delta * (float)s.npts );

/*** find the normalizing factor ***/
	if( norm || normfac )
	{
		ymax = fabs(y[0]);
		for( i=1; i<s.npts; i++ )
		{
			if( fabs(y[i]) > ymax ) ymax = fabs(y[i]);
		}
	}
	
	if( normfac )
	{
		fprintf( stdout, "%s: normfac=%d norm=%d ymax=%6.3e %e %g", 
			progname, normfac, norm, ymax, 1.0/ymax, xmax );
		exit(0);
	}

/*** write the results ***/

	if( xdeg == 1 )
                ydist = s.gcarc;
	else if( xkm == 1 )
		ydist = s.dist;
	else
		ydist = 0;

	fprintf( stdout, ">  %-8.8s %-8.8s %-8.8s %-8.8s dist=%g gcarc=%g xkm=%d xdeg=%d norm=%d ymax=%g ydist=%g\n",
                s.knetwk, s.kstnm, s.khole, s.kcmpnm, s.dist, s.gcarc, xkm, xdeg, norm, ymax, ydist );

        for( i=0; i<s.npts; i++)
	{
		x[i]=s.b + (float)i*s.delta;

	/*** prevent division by zero ***/
		if(ifixZeroDiv)	
		{
		 if( x[i] == 0 ) x[i]=1.0e-9; 
		 if( y[i] == 0 ) y[i]=1.0e-9; 
		}

		if( norm )
			fprintf( stdout, "%g %g\n", x[i], scale*(y[i]/ymax) + ydist );
		else
                	fprintf( stdout, "%g %g\n", x[i], scale*y[i] + ydist );
	}

	if(verbose) fprintf( stderr, "%s: exiting \n", progname );
}
