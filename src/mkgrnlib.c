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

#include "../include/mt.h" /** global datatype and structure declarations **/

char progname[128];

/*** computes Greens functions just one station at a time ***/
/*** see ../misc/multithread_makeglib/multithread_mkgrnlib.c uses fork-multiprocessing ***/

int main( int ac, char **av )
{
	extern Greens grn_;
	int iz;
	float *z;
	Depth_Info depth_info;
	FILE *fp;
	int verbose=0, idump=0, ista;
	void Usage_Print(void);
	int it;  /*** this version computes the rigidity here ***/
	float cm2km, UnitArea, rigidity, slip4pi;
	cm2km = 1.0E+05;
	UnitArea = 1 * cm2km * cm2km;

/* see include/mt.h Mo = math.pow( 10.0, 1.5*(Mw+10.73) ) = 1.2445146117713818e+16; Reference Mw = 0.0 */

	int isrclayer;
	VelMod vmint;
	float dlaykm  = 1.0;

	void greensf_( int * );

	/*** modified in version 4.0, added string char *wavetype = "Surf/Pnl" or "Rotational" ***/
	void wrtgrn2sac( Greens *g, int ista, char *wavetype, char *fmt, int make_output_dirs );

	void getparameters( int, char **, Greens *, Depth_Info *, int *, int * );

/*** from modsubs.c ***/
        void create_mod( VelMod * );
        void print_mod0( VelMod * );
        void print_mod1( FILE *, VelMod * );
        void compute_rigidity( Greens *, int );

/*****************************/
/*** from rayp/rayp_subs.c ***/
/*****************************/
        void interpolate_model( VelMod *, VelMod *, float );
        void calc_1D_tt( float, float, float *, float *, float *, int *, VelMod *, int );
        void earth_flattening_transformation( VelMod * );
        float calc_takeoff_angle( float, float, float, float );

/*** misc ***/
	char pathname[128];
	char *shorten_path( char *pathname, char *filename );

/*****************************/
/*** start program main    ***/
/*****************************/
	strcpy( pathname, av[0] );

	shorten_path( pathname, progname );

	fprintf( stdout, "%s: %s: %s: STDOUT: Version=%s Release Date=%s exec full path=%s\n", 
		progname, 
		__FILE__,
		__func__,
		Version_Label,
		Version_Date,
		pathname );

	if( ac <= 1 ) Usage_Print();

/*******************************************************/
/*** get input parameters from file and command line ***/
/*******************************************************/
	depth_info.zvec = calloc(200,sizeof(float));
	getparameters( ac, av, &grn_, &depth_info, &verbose, &idump );

/**********************************************************/
/*** make an interpolated version of the original model ***/
/*** clones the original velocity model for use to calc ***/
/*** 1D travel-times and ray-parameters                 ***/
/**********************************************************/
	interpolate_model( &(grn_.v), &vmint, dlaykm );
	if(verbose) print_mod0( &vmint );
                                                                                                                                 
/**********************************************************/
/*** Earth Flattening Transformation                    ***/
/*** EFT is applied only onto interpolated velocity     ***/
/*** model from above and use to calc 1D travel-times   ***/
/**********************************************************/
	earth_flattening_transformation( &vmint );
	if(verbose)print_mod0( &vmint );

/*******************************************************/
/*** set the depth vector                            ***/
/*******************************************************/
	if( depth_info.type == 0 || depth_info.type == 1 )
	{
	  z = (float *) calloc( depth_info.nz, sizeof(float) );
	  if(verbose) printf("%s: \t\t", progname );
	  for( iz=0; iz < depth_info.nz; iz++ )
	  {
		z[iz] = depth_info.zmin + ( iz * depth_info.zinc );
		if(verbose)
		{
			if( iz == depth_info.nz - 1 ) printf("%g\n", z[iz] );
			else printf("%g ", z[iz] );
		}
	  }
	}
	else if( depth_info.type==2 )
	{
	  z = (float *) calloc( depth_info.nz, sizeof(float) );
	  if(verbose) printf("%s: \t\t", progname );
	  for( iz=0; iz < depth_info.nz; iz++ )
	  {
		z[iz] = depth_info.zvec[iz];
	        if(verbose)
		{
			if( iz == depth_info.nz - 1 ) printf("%g\n", z[iz] );
			else printf("%g ", z[iz] );
	 	}
	  }
	}

/*******************************************************/
/*** open the file and write the source depth        ***/
/*** references in the header of the output file     ***/
/*******************************************************/
	if( (fp = fopen(grn_.filename,"wb")) == NULL )
	{
		printf("%s: Fatal Error, cannot open file %s\n", 
			progname, grn_.filename );
		exit(-1);
	}
	fwrite(&(depth_info.nz),sizeof(int),1,fp);
	fwrite(&z[0],depth_info.nz*sizeof(float),1,fp);
	fflush(fp);

/*******************************************************/
/*** loop over depth                                 ***/
/*******************************************************/
	for( iz=0; iz< depth_info.nz; iz++ )
	{

	/*******************************************************/
	/*** compute the rigidity at the source for the      ***/
	/*** source depth's velocity and density             ***/
	/*******************************************************/
		if( verbose ) printf("%s: iz=%d z=%g\n", progname, iz, z[iz] );
		grn_.evdp = z[iz];
		compute_rigidity( &grn_, verbose );

	/********************************************************/
	/*** compute the Green's functions Ref:               ***/
	/*** Zeng and Anderson (1995) Bull. Seismol. Soc. Am. ***/
	/********************************************************/
		greensf_( &verbose );

	/***********************************************************/
	/*** dump part of the structure for debugging            ***/
	/***********************************************************/
		if(verbose)
		{
		  fprintf(stdout, "%s: filename=%s stnm=%s net=%s loc=%s stla=%f stlo=%f ",
		    progname, grn_.filename, grn_.stnm, grn_.net, grn_.loc, grn_.stla, grn_.stlo );

		  fprintf(stdout, "evla=%f evlo=%f evdp=%f rdist=%g az=%f baz=%f ",
		    grn_.evla, grn_.evlo, grn_.evdp, grn_.rdist, grn_.az, grn_.baz );

		  fprintf(stdout, "t0=%f dt=%f fmax=%f damp=%f eps=%f smin=%f\n",
		    grn_.t0, grn_.dt, grn_.fmax, grn_.damp, grn_.eps, grn_.smin );
		}

		fprintf(stdout, 
	"%s: %s: %s: %s evdp=%6.2f rdist=%8.2f az=%3.0f nt=%4d dt=%5.3f fmax=%5.2f t0=%g twin=%g ",
			progname,
			__FILE__,
			__func__,
			grn_.filename,
			grn_.evdp,
			grn_.rdist,
			grn_.az,
			grn_.nt,
			grn_.dt,
			grn_.fmax,
			grn_.t0,
			grn_.twin );

	/**************************/
	/*** apply the rigidity ***/
	/**************************/
		rigidity = grn_.rigidity;
		slip4pi =  (base_moment/(rigidity * UnitArea))/(4*M_PI);
		for( it = 0; it < grn_.nt; it++ )
		{
			grn_.g.rss[it] *= slip4pi;
			grn_.g.rds[it] *= slip4pi;
			grn_.g.rdd[it] *= slip4pi;
			grn_.g.rep[it] *= slip4pi;
			grn_.g.zss[it] *= slip4pi;
			grn_.g.zds[it] *= slip4pi;
			grn_.g.zdd[it] *= slip4pi;
			grn_.g.zep[it] *= slip4pi;
			grn_.g.tss[it] *= slip4pi;
			grn_.g.tds[it] *= slip4pi;
		}

	/************************************/
	/*** introduce reduction velocity ***/
	/************************************/
		grn_.ts0	= grn_.t0;
		grn_.tstart	= grn_.ts0;
		grn_.tend	= grn_.ts0 + grn_.dt * grn_.nt;

	/*********************************************************************/
	/*** calculate travel time, ray parameter, and ray bottoming depth ***/
	/*********************************************************************/
		verbose = 0;
		calc_1D_tt( grn_.rdist, 
			grn_.evdp, 
			&(grn_.Prayparameter), 
			&(grn_.Pttime), 
			&(grn_.Praybottom), 
			&isrclayer, 
			&vmint, 
			verbose );

		grn_.Ptakeoff = calc_takeoff_angle( 
			vmint.vp[isrclayer], 
			grn_.Prayparameter, 
			grn_.rdist, 
			grn_.evdp );

		  fprintf( stdout, " p=%6.4f rb=%6.1f tt=%7.2f toa=%5.1f\n",
                    grn_.Prayparameter,
                    grn_.Praybottom,
                    grn_.Pttime,
                    grn_.Ptakeoff );

	/***********************************************************/
	/*** write out greens functions as SAC files for testing ***/
	/*** this code only computes translational GFs not RotGF ***/
	/***********************************************************/
		ista = 0;
		if( idump ) wrtgrn2sac( &grn_, ista, "Surf/Pnl", "\0", 0 /* int make_output_dirs */ );

	/**************************************************************/
	/*** write out binary greens functions library for glib2inv ***/
	/**************************************************************/
		fwrite(&grn_,sizeof(Greens),1,fp);
	}

/****************/
/*** clean up ***/
/****************/
	fclose(fp);
	free(z);
	printf("%s: program finished. Bye-Bye!\n\n\n", progname );
	exit(0);
}

void getparameters( int ac, char **av, Greens *g, 
		Depth_Info *depth_info, int *verbose, int *idump )
{
	double drdist,daz,dbaz;
	float nyquist_frequency;
	char stadb_filename[128];

	int distaz( double, double, double, double, double *, double *, double * );
	int setpar( int, char ** );
	int getpar( char *, char *, void * );
	int mstpar( char *, char *, void * );
	void endpar();
	void create_mod( VelMod * );
	void print_mod0( VelMod * );
	void getsta( char *, Greens *, int * );

/***depth***/
	float single_src_depth_km = -99; /*** type=0 ***/
	int   nz_multiple_irregular = 0; /*** type=2 ***/
	/*** temp space for getpar() ***/
	float zvec[3];
	char  zstr[8];
	int   iz;

/***************************** begin **********************************/

	setpar(ac,av);
/*************************/
/**** output settings ****/
/*************************/
	getpar("verbose", "b", verbose );
	getpar("dump", "b", idump );

/**************************************/
/**** set the depth or depth range ****/
/**************************************/
	getpar( "z_depth_km", "f", &single_src_depth_km );
	getpar( "nz", "d", &nz_multiple_irregular );

	if( single_src_depth_km == -99 && nz_multiple_irregular == 0 )
	{
	 mstpar("zrange", "vf[3]", &zvec );
	 depth_info->type = 1;
	 depth_info->zmin = zvec[0];
	 depth_info->zinc = zvec[1];
	 depth_info->zmax = zvec[2];
	 depth_info->nz   = ((depth_info->zmax-depth_info->zmin)/depth_info->zinc) + 1;
	}
	else if( single_src_depth_km == -99 && nz_multiple_irregular > 0 )
	{
	 depth_info->type = 2;
	 depth_info->nz   = nz_multiple_irregular;
	 depth_info->zvec = (float *)realloc( depth_info->zvec, nz_multiple_irregular * sizeof(float) );
	 sprintf( zstr, "vf[%d]", nz_multiple_irregular );
	 mstpar( "z_multiple", zstr, depth_info->zvec );
	 for( iz = 0; iz < nz_multiple_irregular; iz++ )
	 {
		fprintf( stdout,  "iz=%d z_multiple_irregular=%g\n", iz, depth_info->zvec[iz] );
	 }
	}
	else if( single_src_depth_km != -99 && nz_multiple_irregular == 0 ) /* kludge for single src depth */
	{
	 depth_info->type = 0;
	 depth_info->nz   = 1;
	 depth_info->zmin = single_src_depth_km;
	 depth_info->zinc = single_src_depth_km*2;
	 depth_info->zmax = single_src_depth_km;
	}
	else
	{
		fprintf(stderr,"%s: %s: %s: ERROR! in depth_info\n", progname, __FILE__, __func__ );
		exit(-1);
	}

	if( *verbose )
	{
	  printf("%s: Compute Greens Functions for:\n", progname );
	  printf("%s: \t\t zmin=%g zinc=%g zmax=%g nz=%d\n",
	    progname, depth_info->zmin, depth_info->zinc, 
		depth_info->zmax, depth_info->nz );
	}

/************************************************************************/
/*** get the station location information, later get this information ***/
/*** from station geometry file including station and network         ***/
/************************************************************************/
	mstpar("stnm", "s", &(g->stnm) );
	mstpar("net",  "s", &(g->net) );
	mstpar("loc",  "s", &(g->loc) );
	mstpar("stadb", "s", stadb_filename );

/*************************************/
/*** source latitude and longitude ***/
/*************************************/
	mstpar("evla", "f", &(g->evla) );
	mstpar("evlo", "f", &(g->evlo) );

/*** important stuff ***/
	mstpar("nt",	"d", &(g->nt) );
	mstpar("dt",	"f", &(g->dt) );
	g->twin = g->nt * g->dt;

/*** f-k stuff ***/
	mstpar("eps",	"f", &(g->eps) );
	mstpar("smin",	"f", &(g->smin) );
	g->damp = 1.0;
	getpar("damp",  "f", &(g->damp) );
	g->kmax = 10000000;
	getpar("kmax",  "d", &(g->kmax) );

/**************************/
/*** reduction velocity ***/
/**************************/
	g->redv  = -1.;
	getpar("redv",  "f", &(g->redv) );
	g->t0 = 0;
	getpar("t0", "f", &(g->t0) );

/***************************************************************/
/*** leave off the .mod in filename append only when opening ***/
/***************************************************************/
	mstpar("modeldb", "s", &(g->v.modpath) );
	mstpar("velmod", "s", &(g->v.modfile) );

/******************************************************************************/
/*** default fmax is the nyquist frequency unless otherwise set to be lower ***/
/******************************************************************************/
	nyquist_frequency = 1/(2 * g->dt);
	getpar("fmax", "f", &(g->fmax) );
	if( g->fmax > nyquist_frequency ) g->fmax = nyquist_frequency;

	g->twin = g->nt * g->dt;

	endpar();

/***********************************/
/*** get the station information ***/
/***********************************/
	g->stel = 0;
	getsta( stadb_filename, g, verbose );
	fprintf( stdout, "%s: %s: %s: Net.Sta.Loc=(%s.%s.%s) stla=%g stlo=%g stel=%g evla=%g evlo=%g\n",
		progname, __FILE__, __func__,
		g->net, g->stnm, g->loc, g->stla, g->stlo, g->stel,
		g->evla, g->evlo );

	if( distaz( (double)g->evla, (double)g->evlo, 
		(double)g->stla, (double)g->stlo, &drdist, &daz, &dbaz ) == 0 )
	{
		g->rdist = (float)drdist;
		g->az    = (float)daz;
		g->baz   = (float)dbaz;
		fprintf( stdout, 
			"%s: %s: %s: distazs(r=%g, az=%g, baz=%g)\n", 
			progname, __FILE__, __func__, 
			g->rdist, g->az, g->baz );
	}
	else
	{
		fprintf( stdout, "%s: %s: %s: distaz: returned a fatal error\n", 
			progname, __FILE__, __func__ );
		exit(-1);
	}

/********************************************************************/
/*** use the reduction velocity (km/sec) to set the t0 time shift ***/
/********************************************************************/
	if ( g->redv > 0 ) 
	{
	  g->t0     = g->rdist/g->redv;
	  g->tstart = g->ts0 = g->t0;
	  g->tend   = g->tstart + g->twin;

	  fprintf( stdout, "%s: %s: %s: dist=%g reduction velocity=%g(km/sec) t0=%g ",
		progname, __FILE__, __func__, g->rdist, g->redv, g->t0 );

	  fprintf( stdout, "tstart=%g tend=%g twin=%g\n",
		g->tstart, g->tend, g->twin ); 
	}

/*************************************************/
/*** set the green's function file name        ***/
/*** changes 2019/Dec G. Ichinose includes loc ***/
/*** net.sta.loc.model                         ***/
/*************************************************/
			/* 256 -> 8 + 1 + 8 + 1 + 8 + 256 = 285 */ 
/*** 
clang -I../include -c mkgrnlib.c -o mkgrnlib.o
mkgrnlib.c:449:2: warning: 'snprintf' will always overflow; destination buffer has size 256, but size argument is 285 [-Wbuiltin-memcpy-chk-size]
        snprintf( g->filename, 285,
***/
	snprintf( g->filename, 256,
		"%s.%s.%s.%s.glib",
		g->net,
		g->stnm,
		g->loc,
		g->v.modfile );

       	fprintf( stdout, "%s: %s: %s: Writing glib file to %s\n", 
		progname, __FILE__, __func__, g->filename );

/*********************************************/
/*** set the velocity model data structure ***/
/*********************************************/
	create_mod( &(g->v) );
	/** if( *verbose ) print_mod0( &(g->v) ); **/
	print_mod0( &(g->v) );
}

void Usage_Print()
{
	fprintf(stderr, "\n");
	fprintf(stderr, " USAGE: %s par=(string)\n", progname );
	fprintf(stderr, "\n"); 
	fprintf(stderr, " USAGE: %s stnm=(string) net=(string) loc=(string) velmod=(string)  modeldb=(string) \\ \n", progname );
	fprintf(stderr, "                 stadb=(string) zrange=(float,float,float) evla=(float) evlo=(float) dt=(float) \\ \n" );
	fprintf(stderr, "                 nt=(integer) fmax=(float) t0=(float) rdev=(float) damp=(float) kmax=(integer) \\ \n" );
	fprintf(stderr, "                 eps=(float) smin=(float) [no]dump [no]verbose\n" );
	fprintf(stderr, "\n"); 
	fprintf(stderr, "REQUIRED PARAMETERS:\n" );
	fprintf(stderr, "\t stnm=(string)        Station Code, required cannot be NULL\n" );
	fprintf(stderr, "\t net=(string)         Network Code, required cannot be NULL\n" );
	fprintf(stderr, "\t loc=(string)         Location Code, required for null use empty quotes i.e., loc=\"\")\n" );
	fprintf(stderr, "\t velmod=(string)      Velocity model base name without .mod extenstion\n" );
	fprintf(stderr, "\t modeldb=(string)     Directory path to the velocity model files\n" );
	fprintf(stderr, "\t stadb=(string)       Directory path and filename of the station locations file\n" );
	fprintf(stderr, "\n"); 
	fprintf(stderr, "\t There are three ways to do source depth for Green's function library:\n" );
	fprintf(stderr, "\t zrange=(flt,flt,flt) Range of source depths in km, e.g., zrange=starting,increment,stop \n");
	fprintf(stderr, "\t z_depth_km=(float)   Single source depth in km [optional: if set then zrange skipped]\n");
	fprintf(stderr, "\t nz=(int) z_multiple=(float,float,...)  multiple depths any set or order\n" );
	fprintf(stderr, "\t                           [optional: if nz!= then zrange and z_depth_km skipped]\n" );
	fprintf(stderr, "\n"); 
	fprintf(stderr, "\t evla=(float)         Source latitude, in decimal degrees\n" );
	fprintf(stderr, "\t evlo=(float)         Source longitude, in decimal degrees\n" );
	fprintf(stderr, "\t dt=(float)           Green's function sample rate (samples per seconds)\n" );
	fprintf(stderr, "\t nt=(integer)         Green's function number points (power of 2)\n" );
	fprintf(stderr, "\t fmax=(float)         Green's function maximum frequency (max is Nyquist)\n" );
	fprintf(stderr, "\n"); 
	fprintf(stderr, "\t There are two ways to do starting time for Green's function synthetic origin-time:\n" );
	fprintf(stderr, "\t t0=(float)           Green's function starting time in seconds\n" );
	fprintf(stderr, "\t rdev=(float)         Green's function starting from a reduction velocity (km/sec)\n");
	fprintf(stderr, "\t                                     t0_sec = ( distance_km / reduction_velocity )\n");
	fprintf(stderr, "\n"); 
	fprintf(stderr, "\t damp=(float)         Green's function damping (1 is OK)\n" );
	fprintf(stderr, "\t kmax=(integer)       Green's function maximum wavenumber\n" );
	fprintf(stderr, "\t eps=(float)          Green's function accuracy tolorance parameter 1\n" );
	fprintf(stderr, "\t smin=(float)         Green's function accuracy tolorance parameter 2\n" );
	fprintf(stderr, "\n"); 
	fprintf(stderr, "OPTIONAL PARAMETERS:\n" );
	fprintf(stderr, "\t [no]verbose          Be verbosy [default off]\n" );
	fprintf(stderr, "\t [no]dump             Write out the GF synthetics in SAC files [default off]\n" );
	fprintf(stderr, "\n" );
	fprintf(stderr, "EXAMPLE:\n" );
	fprintf(stderr, "\t %s stnm=CCM net=IU loc=00 velmod=cus modeldb=${MTINV_PATH}/data/modeldb \\ \n", progname );
	fprintf(stderr, "\t          zrange=1,1,25 evla=36.9077 evlo=-90.543 dt=0.15 nt=2048 fmax=0.4 t0=0. redv=18. \\ \n" );
	fprintf(stderr, "\t          damp=1. kmax=20000 eps=0.0005 smin=0.0005 stadb=../Data/rdseed.stations noverbose nodump\n" );
	fprintf(stderr, "\n" );
	fprintf(stderr, "\t Outputs a ~10MB Green's function library file \"IU.CCM.00.cus.glib\" in current directory\n" );
	fprintf(stderr, "\n" );
}
