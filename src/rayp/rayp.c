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
#include <math.h>
#include "../../include/mt.h"
                                                                                                                              
char progname[128];
                                                                                                                              
int main( int ac, char **av )
{
	Greens g;
	double drdist,daz,dbaz;
	float ttime, p, raybot, takeoff, arg;
	int verbose = 0, iverbose;
	int isrclayer;
	char stadb_filename[128];
	VelMod vmint;
	float dlaykm;
	char pol;

/*** from modsubs.c ***/
	void create_mod( VelMod * );
	void print_mod0( VelMod * );
	void print_mod1( FILE *, VelMod * );
	void compute_rigidity( Greens *, int );

/*** from stasubs.o ***/
	void getsta( char *, Greens *, int * );

/*** from rayp_subs.c ***/
	void interpolate_model( VelMod *, VelMod *, float );
	void calc_1D_tt( float, float, float *, float *, float *, int *, VelMod *, int );
	void earth_flattening_transformation( VelMod * );
	float calc_takeoff_angle( float, float, float, float );

	int distaz( double, double, double, double, double *, double *, double * );
	int setpar(),mstpar(),getpar();
	void endpar();
	
	strcpy( progname, av[0] );
	if(verbose) fprintf(stderr, "%s: \n", progname );

	setpar(ac,av);
	mstpar( "stadb", "s", stadb_filename );
        getpar( "verbose", "b", &verbose );
	mstpar( "z",    "f", &g.evdp );
	mstpar( "evla", "f", &g.evla );
        mstpar( "evlo", "f", &g.evlo );
	mstpar( "modeldb", "s", &(g.v.modpath) );
        mstpar( "velmod", "s", &(g.v.modfile) );
	mstpar( "stnm", "s", &(g.stnm) );
	mstpar( "net",  "s", &(g.net) );
	endpar();

	if(verbose)
	{
	  fprintf(stderr, 
	"%s: evdp=%g evla=%g evlo=%g modpath=%s modfile=%s stnm=%s net=%s stadb_filename=%s verbose=%d\n",
		progname, 
		g.evdp,
		g.evla,
		g.evlo,
		g.v.modpath,
		g.v.modfile,
		g.stnm,
		g.net,
		stadb_filename,
		verbose );
	}

/*********************************************/
/*** set the velocity model data structure ***/
/*********************************************/
	create_mod( &(g.v) );
	if(verbose)print_mod0( &(g.v) );

/**********************************************************/
/*** make an interpolated version of the original model ***/
/**********************************************************/

	dlaykm = 1.0;
	interpolate_model(  &(g.v), &vmint, dlaykm );
	if(verbose) print_mod0( &vmint );

/**********************************************************/
/*** Earth Flattening Transformation ***/
/**********************************************************/

	earth_flattening_transformation( &vmint );
	if(verbose)print_mod0( &vmint );

/***********************************/
/*** get the station information ***/
/***********************************/
        g.stel = 0;

	iverbose = 0;
        getsta( stadb_filename, &g, &iverbose );

	if(verbose)
	{
          fprintf(stdout, "%s: stnm=%s net=%s stla=%g stlo=%g stel=%g evla=%g evlo=%g evdp=%g\n",
                progname, g.stnm, g.net, g.stla, g.stlo, g.stel, g.evla, g.evlo, g.evdp );
	}

        if( distaz( (double)g.evla, (double)g.evlo, (double)g.stla, (double)g.stlo, &drdist, &daz, &dbaz ) == 0 )
        {
                g.rdist = (float)drdist;
                g.az    = (float)daz;
                g.baz   = (float)dbaz;
                if(verbose)
		  printf("%s: distaz: r=%g az=%g baz=%g\n", 
			progname, g.rdist, g.az, g.baz );
        }
	else
	{	
		fprintf(stderr, "error occurred in distaz\n" );
		exit(-1);
	}

	compute_rigidity( &g, verbose );

	if(verbose) fprintf(stdout, "%s: rigid=%g\n", progname, g.rigidity );

	calc_1D_tt( g.rdist, g.evdp, &p, &ttime, &raybot, &isrclayer, &vmint, verbose );

	takeoff = calc_takeoff_angle( vmint.vp[isrclayer], p, g.rdist, g.evdp );

	if( takeoff > 90 ) 
		pol = 'c';
	else	
		pol = 'd';
	
	fprintf( stdout, 
	  "%-8s %3.0f %3.0f %c %10s %8.2f %8.2f %4.1f %4.1f %8.5f %6.1f %7.2f %4.1f\n", 
		g.stnm, 
		g.az,
		takeoff,
		pol,
		g.v.modfile,
		g.rdist, 
		g.evdp,
		g.az, 
		g.baz,
		p, 
		raybot, 
		ttime, 
		takeoff );
}
