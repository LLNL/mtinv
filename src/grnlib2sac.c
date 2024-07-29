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

#include "../include/nrutil.h"     /** numerical recipes **/
#include "../include/mt.h"         /** global datatype and structure declarations **/

struct event { 
	float str,dip,rak,Mw,Mo,tr,tt; 
	float Miso,Mdev,Mclvd,Mdc,Mtotal;
	float Piso,Pdev,Pclvd,Pdc;
	float stla,stlo,evla,evlo,evdp;
	float Mxx,Myy,Mzz,Mxy,Myz,Mxz;
	MyTime ot;
	MyTime beg;
};

char progname[128];

#define MT_INPUT	0
#define SDR_MO_INPUT	1
#define SDR_MISO_INPUT	2
#define ISO_CLVD_INPUT	3

#define FIBER_NONE 0
#define FIBER_PSV  1 /* longitudinal waves */
#define FIBER_SH   2 /* transverse waves */



/**** stand-alone program that reads Greens functions *.glib library files and computes synthetics ****/

int main( int ac, char **av )
{
        Greens grn;
	int nz,iz;
	int ista;
	FILE *fp;
	char filename[256];
	float *z, my_z, ztol;
	struct event ev;

	int verbose=0;
	int mtdegfree=5;
	int ifoundz=0;
	int input_type;

	int idumpgrn=0;
	int gmt_plot_grn = 0;
	int make_output_dirs = 0;
	char wavetype_string[32]; /*** wavetype_string is populated with either "Surf/Pnl" or "Rotational" ***/

	int inoise = 0;
	int iseed = 1;
	int dounits_cm2m = 1;
	int dointerp = 0;
	float noise_Mw = 3.8;
	float my_src_to_rec_azimuth_override = -999;

/******************************************************************************/
/******************************************************************************/
/*** function prototypes ***/
/******************************************************************************/
/******************************************************************************/

/*** radiation_pattern.o ***/
	int fiber = FIBER_NONE; /**** 0 no fiber, 1=longitudinal, 2=shear ***/
	char wave_type_fiber[32]; /* l,longitudinal or t,transverse,shear */
	float azi_fiber = 0; /*** or phi0d ***/

/*** see timesubs.c ***/
	char dateid[256];
	MyTime ot;
	void parsestring( MyTime *t, char *str );
        void clone_mytime(  MyTime *t1, MyTime *t2 );
        void WriteMyTime2STDOUT( MyTime *t );

/**** see wrtgrn2sac.c ***/
	void plotgrnlib_GMT5( Greens *g, int ista, char *wavetype, int make_output_dirs );

	void wrtgrn2sac( Greens *g, int verbose, char *wavetype_string, char *fmt, int make_output_dirs );

/*** see this file ***/
	void grn2disp2( Greens *grn, struct event ev, int verbose,
		int mtdegfree, int inoise, float noise_Mw, int iseed,
		int input_type, int dounits_cm2m, int dointerp,
		float my_src_to_rec_azimuth_override, int fiber, float azi_fiber );

	void Usage_Print();

/*** misc see ../lib/libget.a ***/
	int setpar( int ac, char **av );
	int mstpar( char *, char *, void * );
	int getpar( char *, char *, void * );
	void endpar();

/**********************/
/**** begin program ***/
/**********************/
	strcpy( progname, av[0] );
	fprintf( stderr, "\n%s: Version=%s Release Date=%s\n\n", progname, Version_Label, Version_Date );
	fprintf( stdout, "\n%s: Version=%s Release Date=%s\n\n", progname, Version_Label, Version_Date );

	if( ac <= 1 ) Usage_Print();

	setpar(ac,av);
	mstpar("glib",	  "s", filename );
	mstpar("z",	  "f", &my_z );
	getpar("verbose", "b", &verbose );

	getpar("dumpgrn", "b", &idumpgrn );
	if( idumpgrn == 1 )
	{	/*** default ***/
		sprintf( wavetype_string, "Surf/Pnl" );
		/** sprintf( wavetype_string, "Rotational" ); **/

		getpar( "plotgrn", "b", &gmt_plot_grn );
		getpar( "wavetype", "s", wavetype_string );
		getpar( "mkdir", "b", &make_output_dirs );
	}

	strcpy( dateid, "2008/01/01,00:00:00\0" );
	getpar( "date", "s", dateid );
	getpar( "dounits_cm2m", "b", &dounits_cm2m );
	getpar( "dointerp",  "b", &dointerp );

	getpar( "az", "f", &my_src_to_rec_azimuth_override );

	getpar("noise",   "b", &inoise );
	if( inoise )
	{
		mstpar("nMw", "f", &noise_Mw );
		getpar("seed", "d", &iseed );
	}
	
	if( !idumpgrn )
	{
	  	if(verbose)
		  fprintf(stderr, "%s: %s: %s: idumpgrn is off\n",
			progname, __FILE__, __func__ );

		input_type = SDR_MO_INPUT;  /*** default ***/
		mstpar("type", "d", &input_type );

		if( input_type == MT_INPUT )  /*** type=0 ***/
		{
			mtdegfree = 6;
			mstpar("Mxx","f",&ev.Mxx );
			mstpar("Myy","f",&ev.Myy );
			mstpar("Mzz","f",&ev.Mzz );
			mstpar("Mxy","f",&ev.Mxy );
			mstpar("Mxz","f",&ev.Mxz );
			mstpar("Myz","f",&ev.Myz );
			mstpar("Mo", "f",&ev.Mo );
		}
		else if( input_type == SDR_MO_INPUT ) /*** type=1 ***/
		{
			mtdegfree = 5;
			mstpar("str",	"f", &ev.str );
			mstpar("dip",	"f", &ev.dip );
			mstpar("rak",	"f", &ev.rak );
			mstpar("Mw",	"f", &ev.Mw );
			ev.Piso = 0.0;
			ev.Pdev = 1.0;
			ev.Pdc  = 1.0;
			ev.Pclvd = 0.0;
		}
		else if( input_type == SDR_MISO_INPUT )  /*** type=2 ***/
		{
			mtdegfree = 5;
			mstpar("str",   "f", &ev.str );
			mstpar("dip",   "f", &ev.dip );
			mstpar("rak",   "f", &ev.rak );
			mstpar("Mw",    "f", &ev.Mw );

			mstpar("Piso",  "f", &ev.Piso );
			mstpar("Pclvd", "f", &ev.Pclvd );
			mstpar("Pdc",   "f", &ev.Pdc );

			ev.Pdev = ev.Pclvd + ev.Pdc;
		}

	/*** repeat the input ***/

		if( input_type == MT_INPUT )
		{
			fprintf( stdout, "%s: %s: %s: input_type=0(MT_INPUT) Mxx=%g Myy=%g Mzz=%g Mxy=%g Mxz=%g Myz=%g Mo=%g\n",
				progname, __FILE__, __func__, ev.Mxx, ev.Myy, ev.Mzz, ev.Mxy, ev.Mxz, ev.Myz, ev.Mo );
		}
		else if( input_type == SDR_MO_INPUT )
		{
			fprintf( stdout, "%s: %s: %s: input_type=1(SDR_MO_INPUT) str=%g dip=%g rak=%g Mw=%g\n", 
				progname, __FILE__, __func__, ev.str, ev.dip, ev.rak, ev.Mw );
		}
		else if( input_type == SDR_MISO_INPUT )
		{
			fprintf( stdout, "%s: %s: %s: input_type=1(SDR_MO_INPUT) str=%g dip=%g rak=%g Mw=%g Piso=%g Pclvd=%g Pdc=%g\n",
                                progname, __FILE__, __func__, ev.str, ev.dip, ev.rak, ev.Mw, ev.Piso, ev.Pclvd, ev.Pdc );
		}
		else {
			fprintf( stdout, "%s: %s: %s: input_type=%d unknown\n", progname, __FILE__, __func__, input_type );
			exit(0);
		}

	/*** check the ratio of isotropic to deviatoric moment only when input_type != MT_INPUT ***/

		if( input_type != MT_INPUT )
		{
		  if( ( fabs(ev.Pclvd) + fabs(ev.Pdc) ) == ev.Pdev )
		  {
			fprintf(stdout, "%s: %s: %s: Pdev= %g Pclvd+Pdc=%g\n",
			  progname, __FILE__, __func__, ev.Pdev, fabs(ev.Pclvd) + fabs(ev.Pdc)  );
		  }
		  else
		  {
			fprintf(stderr,
			  "%s: %s: %s: ERROR Percent Deviatoric does not equal 100%% - Isotropic: Pdev= %g Pclvd+Pdc=%g\n",
			  progname, __FILE__, __func__, ev.Pdev, fabs(ev.Pclvd) + fabs(ev.Pdc)  );
			/* exit(-1); */
		  }

	/*** check that all the percent moments adds to 100 percent ***/

		  if( ( fabs(ev.Piso) + fabs(ev.Pclvd) + fabs(ev.Pdc) ) == 1.0 )
		  {
			fprintf(stdout, "%s: %s: %s: Piso=%g Pclvd=%g Pdc=%g\n",
				progname, __FILE__, __func__, 
				ev.Piso, ev.Pclvd, ev.Pdc );
		  }
		  else
		  {
			fprintf(stderr,
			  "%s: %s: %s: ERROR Percent Iso+Clvd+DC greater than 100%% Piso=%g Pclvd=%g Pdc=%g\n",
				progname, __FILE__, __func__, ev.Piso, ev.Pclvd, ev.Pdc );
			/* exit(-1); */
		  }
		}

		ev.tr = 0; 
		ev.tt = 0;
		getpar("tr",    "f", &ev.tr );
		getpar("tt",    "f", &ev.tt );
	}
	else /* if !idumpgrn */
	{
		if(verbose)
		 fprintf(stderr, "%s: %s: %s: dumpgrn is on\n",
			progname, __FILE__, __func__ );
	}

	strcpy( wave_type_fiber, "none" );
	getpar( "wave_type_fiber", "s", wave_type_fiber );

	if( strcmp( wave_type_fiber, "none" ) == 0 ) 
	{
		fiber = FIBER_NONE;
		fprintf( stderr, "%s: %s: %s: wave_type_fiber=%s fiber=%d FIBER_NONE\n", 
			progname, __FILE__, __func__, wave_type_fiber, fiber );

	}
	else if( strcmp( wave_type_fiber, "l" ) == 0 || strcmp( wave_type_fiber, "long" ) == 0 || 
		strcmp( wave_type_fiber, "longitudinal" ) == 0 || strcmp( wave_type_fiber, "compressional" ) == 0 )
	{
		fiber = FIBER_PSV;
		fprintf( stderr, "%s: %s: %s: wave_type_fiber=%s fiber=%d FIBER_PSV \n", 
			progname, __FILE__, __func__, wave_type_fiber, fiber );
	}
	else if( strcmp( wave_type_fiber, "t" ) == 0 || strcmp( wave_type_fiber, "shear" ) == 0 || 
		strcmp( wave_type_fiber, "transverse" ) == 0 || strcmp( wave_type_fiber, "trans" ) == 0 )
	{
		fiber = FIBER_SH;
		fprintf( stderr, "%s: %s: %s: wave_type_fiber=%s fiber=%d FIBER_SH \n",
			progname, __FILE__, __func__, wave_type_fiber, fiber );
	}
	else
	{
		fiber = FIBER_NONE;
	}
	
	if( fiber == FIBER_PSV || fiber == FIBER_SH )
		mstpar( "azi_fiber", "f", &azi_fiber );

	endpar();


/***********************************************************************************/
/*** END OF GETPAR                                                               ***/
/***********************************************************************************/
	
/*********************************************************/
/*** parse dateid string  and put into event structure ***/
/*********************************************************/
	if(verbose) 
		fprintf( stdout, "%s: %s: %s: dateid=%s\n",
			progname, __FILE__, __func__, dateid ); 

	parsestring( &ot, dateid );
	clone_mytime( &ot, &(ev.ot) );
	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: origin time=", progname, __FILE__, __func__ );
	  WriteMyTime2STDOUT( &(ev.ot) ); 
	}

	clone_mytime( &ot, &(ev.beg) );
	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: beginn time=", progname, __FILE__, __func__ );
	  WriteMyTime2STDOUT( &(ev.beg) ); 
	}

	if( verbose )
		fprintf(stderr, "%s: %s: %s: verbose is on\n", progname, __FILE__, __func__ );

/**********************************************/
/*** open green's function file and read in ***/
/**********************************************/
	if( (fp = fopen( filename, "rb" ) ) == NULL )
	{
		fprintf(stderr, "%s: %s: %s: Fatal Error, cannot open file %s\n", 
			progname, __FILE__, __func__, filename );
		exit(-1);
	}

	fread(&nz,sizeof(int),1,fp);
	z = malloc(nz*sizeof(float));
	fread(&z[0],nz*sizeof(float),1,fp);

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: nz=%d ", progname, __FILE__, __func__, nz );
	  for(iz=0;iz<nz;iz++)printf(" z[%d]=%g ", iz, z[iz] );
	  printf("\n");
	}

	ifoundz = -1;
	ztol = 1.0E-05;
	for( iz=0; iz<nz; iz++ )
	{
		if( (my_z > z[iz]-ztol) && (my_z < z[iz]+ztol) )
		{
			ifoundz = iz;
			if(verbose)
			  fprintf( stdout, "%s: %s: %s: found iz=%d z=%g\n", 
			    progname, __FILE__, __func__, ifoundz, z[ifoundz] );
			break;
		}
	}
	if( ifoundz < 0 )
	{
	  fprintf( stderr, "%s: %s: %s: Fatal ERROR My_z=%g not found in z=",
		progname, __FILE__, __func__, my_z );
	}
	else
	{
		if(verbose)
		  fprintf(stderr, "%s: %s: %s: found my_z=%g ifoundz=%d iz=%d z=%g\n",
			progname, __FILE__, __func__, my_z, ifoundz, iz, z[ifoundz] );
	}

	for( iz=0; iz<nz; iz++ )
	{
		fread(&grn,sizeof(Greens),1,fp);
		if(verbose)
		{
		  fprintf( stdout, "%s: %s: %s: iz=%d z=%g rdist=%g az=%g evdp=%g t0=%g dt=%g nt=%d %s\n",
			progname, __FILE__, __func__,
			iz, z[iz], grn.rdist, grn.az, grn.evdp, grn.t0, 
			grn.dt, grn.nt, grn.filename );
		}

		if( iz == ifoundz )
		{
		  ista = 0;

		
		  if( idumpgrn == 1 ) 
		  {
			if(verbose) 
			{
				fprintf( stdout, "%s: %s: %s: idumpgrn=%d calling wrtgrn2sac: wavetype=%s\n",
					progname, __FILE__, __func__, idumpgrn, wavetype_string );
			}

			if( strcmp( wavetype_string, "Rotational" ) == 0 )
			{
				wrtgrn2sac( &grn, ista, wavetype_string, "\0", make_output_dirs );

				/*** has to be run afterwards ***/
				if(gmt_plot_grn)
					plotgrnlib_GMT5( &grn, ista, "Rotational", make_output_dirs );

			}
			else if( strcmp( wavetype_string, "Surf/Pnl" ) == 0 )
			{
				wrtgrn2sac( &grn, ista, wavetype_string, "\0", make_output_dirs );

				/*** has to be run afterwards ***/
				if(gmt_plot_grn)
					plotgrnlib_GMT5( &grn, ista, "Surf/Pnl", make_output_dirs );
			}
			else
			{
				fprintf( stdout, 
				  "%s: %s: %s: ERROR! idumpgrn=%d unknown wavetype = %s (only \"Surf/Pnl\" or \"Rotational\" allowed)\n",
					progname, __FILE__, __func__, idumpgrn, wavetype_string );
				exit(-1);
			}
		  }


		  if( idumpgrn == 0 ) 
		  {
		    if(verbose)
			fprintf( stdout, "%s: %s: %s: calling grn2disp2():\n",
				progname, __FILE__, __func__ );

		    grn2disp2( &grn, ev, verbose, mtdegfree, inoise,
				noise_Mw, iseed, input_type, dounits_cm2m,
				dointerp, my_src_to_rec_azimuth_override,
				fiber, azi_fiber );
		  }
	          break;
		}
	}
	fclose(fp);
	free(z);

} /*** end of main.c ***/

/**************************************************************************************************/
/*** grn2disp2() *******/
/**************************************************************************************************/

void grn2disp2( Greens *g, struct event ev, int verbose, int mtdegfree, 
  int inoise, float noise_Mw, int iseed, int input_type, int dounits_cm2m,
  int dointerp, float my_src_to_rec_azimuth_override, int fiber, float azi_fiber )
{
	float *tra, *rad, *ver, *ns, *ew;
	float *rss, *rds, *rdd, *rep, *zss, *zds, *zdd, *zep, *tss, *tds;
	float a1,a2,a3,a4,a5;
	Tensor M;
	float strr,dipr,rakr,Miso,tmpaz,angle,cmpinc,az;
	Sac_Header sp;
	char sacfile[256];
	FILE *fp;
	int it;
	float pi, d2r, dt, t0, e, tt, tr, fi, twin, r, dist, azimuth, baz;
	float slip4pi_iso, slip4pi_dev, slip4pi_tot;
	float third, half, sixth;
	int nt;
	float cm2m = 0.01; /*** converts meters to centimeters 1m/100cm  ***/
	
	char nslc[16], nsl[16]; /*network.station.location.channel*/
	
/*** radiation_pattern.c ***/
	float radpat_fiber_long_degrees( float thetad, float phi0d );
	float radpat_fiber_trans_degrees( float thetad, float phi0d );
	float tmpr, tmpt, tmpz;

/* see include/mt.h Mo = math.pow( 10.0, 1.5*(Mw+10.73) ) = 1.2445146117713818e+16; Reference Mw = 0.0 */

	int new_nt;
	float new_dt;
	float *xtra, *xrad, *xver, *xns, *xew;

	float fact = 1, facl = 1; /* for fiber */
	float fac = 1; /* for noise */

	float noise_moment;
	float gasdev( int *iseed );
	void source_time_function( float *, int, float, float, float );
	void  rotate( float *, int, float *, float, float *, int, float *, float, float, int );

	void duplicate_vector( float *x, float *y, int n );
	void write_SACPZ_file( char *net, char *sta, char *loc, char *chan, int year, int jday, int hour, int min );

	float peak, tmp;
	float find_abspeak_value_from_float_array( float *x, int nt );

 /*** interpolate/interpolate_subs.c ***/
        void  interpolate_fft(
                float *data,
                int old_npts,
                float old_delta,
                int *new_npts,
                float new_delta );

/**************************************************/
/*** start subroutinnue ***/
/**************************************************/
	sprintf( nsl, "%s.%s.%s", g->net, g->stnm, g->loc );
	
	half = 0.5;
	third = 0.33333333;
	sixth = 0.166666667;
	d2r = M_PI / 180.0;
	nt = g->nt;
	dt = g->dt;
	t0 = g->t0;
	twin = (float)nt*dt;
	e = t0 + (nt*dt);
	tt = ev.tt;
	tr = ev.tr;

/**************************************************************************************************/
/*** allocate memory for displacement vector ***/
/**************************************************************************************************/

	tra = malloc( nt*sizeof(float) );
	rad = malloc( nt*sizeof(float) );
	ver = malloc( nt*sizeof(float) );

/**************************************************************************************************/
/*** set t0 from reduction velocity redv ***/
/**************************************************************************************************/

	if( g->redv > 0 ) g->t0 = g->rdist/g->redv;
	t0 = g->t0;

/**************************************************************************************************/
/*** simplify by assigning local pointers to structure ***/
/**************************************************************************************************/

	rss = g->g.rss;
	rds = g->g.rds;
        rdd = g->g.rdd;
        rep = g->g.rep;
        zss = g->g.zss;
        zds = g->g.zds;
        zdd = g->g.zdd;
        zep = g->g.zep;
        tss = g->g.tss;
        tds = g->g.tds;

/**************************************************************************************************/
/*** find the peak value of the GFs for noise scaling ***/
/**************************************************************************************************/

	peak = 1E-29;
	if( (tmp = find_abspeak_value_from_float_array( rss, nt )) > peak ) peak = tmp;
	if( (tmp = find_abspeak_value_from_float_array( rds, nt )) > peak ) peak = tmp;
	if( (tmp = find_abspeak_value_from_float_array( rdd, nt )) > peak ) peak = tmp;
	if( (tmp = find_abspeak_value_from_float_array( rep, nt )) > peak ) peak = tmp;
	if( (tmp = find_abspeak_value_from_float_array( zss, nt )) > peak ) peak = tmp;
	if( (tmp = find_abspeak_value_from_float_array( zds, nt )) > peak ) peak = tmp;
	if( (tmp = find_abspeak_value_from_float_array( zdd, nt )) > peak ) peak = tmp;
	if( (tmp = find_abspeak_value_from_float_array( zep, nt )) > peak ) peak = tmp;
	if( (tmp = find_abspeak_value_from_float_array( tss, nt )) > peak ) peak = tmp;
	if( (tmp = find_abspeak_value_from_float_array( tds, nt )) > peak ) peak = tmp;

/**************************************************************************************************/
/*** convolve source time funciton ***/
/**************************************************************************************************/

	source_time_function( rss, nt, dt, tr, tt );
	source_time_function( rds, nt, dt, tr, tt );
	source_time_function( rdd, nt, dt, tr, tt );
	source_time_function( rep, nt, dt, tr, tt );
	source_time_function( zss, nt, dt, tr, tt );
	source_time_function( zds, nt, dt, tr, tt );
	source_time_function( zdd, nt, dt, tr, tt );
	source_time_function( zep, nt, dt, tr, tt );
	source_time_function( tss, nt, dt, tr, tt );
	source_time_function( tds, nt, dt, tr, tt );

/**************************************************************************************************/
/*** compute the 3 component displacements ***/
/**************************************************************************************************/

	if( my_src_to_rec_azimuth_override != -999 )
	{
		fprintf( stdout, "%s: %s: %s: %s *** my_src_to_rec_azimuth_override = %g ***\n",
			progname, __FILE__, __func__, nsl, my_src_to_rec_azimuth_override );
		g->az = my_src_to_rec_azimuth_override;
		fi = my_src_to_rec_azimuth_override * d2r;
	}
	else
	{
		fprintf( stdout, "%s: %s: %s: %s *** src_to_rec_azimuth = %g no azimuth override ***\n",
			progname, __FILE__, __func__, nsl, g->az );
		fi = g->az * d2r;
	}

/****************************************************************************************/
/*** in ../include/mt.h : static float base_moment = 1.2445146117713818e+16;          ***/
/* Mo = math.pow( 10.0, 1.5*(Mw+10.73) ) = 1.2445146117713818e+16; Reference Mw = 0.0 ***/
/* Typically base_moment is 10^20 dyne*cm  so conversion factor is 8035.2612218561935 ***/
/****************************************************************************************/

	if( input_type == SDR_MO_INPUT || input_type == SDR_MISO_INPUT )
	{
		if(verbose)
			fprintf( stdout, "%s: %s: %s: %s SDR_MO_INPUT or SDR_MISO_INPUT format\n",
				progname, __FILE__, __func__, nsl );

		ev.Mo = pow( 10., (1.5*(ev.Mw + 10.73)));

		ev.Miso = ev.Piso * ev.Mo/base_moment;

		ev.Mdc  = ev.Pdc * ev.Mo/base_moment;

		ev.Mclvd = ev.Pclvd * ev.Mo/base_moment;

		ev.Mtotal = ev.Miso + ev.Mclvd + ev.Mdc;

		if(verbose) 
		{
		  fprintf( stdout, "%s: %s: %s: %s Mw=%g Mo=%g Miso=%g(%0.f%%) Mclvd=%g(%0.f%%) Mdc=%g(%0.f%%) Mtotal=%g\n",
			progname, __FILE__, __func__, nsl, 
			ev.Mw, 
			ev.Mo, 
			ev.Miso   * base_moment, ev.Piso  * 100, 
			ev.Mclvd  * base_moment, ev.Pclvd * 100, 
			ev.Mdc    * base_moment, ev.Pdc   * 100,
			ev.Mtotal * base_moment );
		}

		strr = ev.str * d2r;
		dipr = ev.dip * d2r;
		rakr = ev.rak * d2r;

	/*************************************************************************************************************/
	/*** from jost&Herrmann (1989) Eq. 18 page 40 SSA-SRL ***/
	/*** also box 4 Aki and Richards Quantitative Seismology 2ndEd ***/
	/*************************************************************************************************************/
		M.xx = -( sin(dipr) * cos(rakr) * sin(2*strr) + sin(2*dipr) * sin(rakr) * sin(strr)*sin(strr) );
		M.yy =  ( sin(dipr) * cos(rakr) * sin(2*strr) - sin(2*dipr) * sin(rakr) * cos(strr)*cos(strr) );
		M.zz =  ( sin(2*dipr) * sin( rakr ) );
		M.xy =  ( sin(dipr) * cos(rakr) * cos(2*strr) + 0.5*sin(2*dipr) * sin(rakr) * sin(2*strr) );
		M.xz = -( cos(dipr) * cos(rakr) * cos(strr) + cos(2*dipr) * sin(rakr) * sin(strr) );
		M.yz = -( cos(dipr) * cos(rakr) * sin(strr) - cos(2*dipr) * sin(rakr) * cos(strr) );

		if(verbose)
		{
		  fprintf( stdout, "%s: %s: %s: %s str=%g dip=%g rak=%g Mw=%g Mo=%g\n",
		    progname, __FILE__, __func__, nsl,
			ev.str, ev.dip, ev.rak, ev.Mw, ev.Mo );

		  fprintf( stdout, "%s: %s: %s: %s az=%g(%g) fi=%g strr=%g dipr=%g rakr=%g Tensor M.xx=%g M.yy=%g M.zz=%g M.xy=%g M.xz=%g M.yz=%g\n",
			progname, __FILE__, __func__, nsl, 
			g->az, my_src_to_rec_azimuth_override, fi, strr, dipr, rakr,
			M.xx, M.yy, M.zz, M.xy, M.xz, M.yz);
		}

	/************************************************************************************************/
	/*** these from Langston (1983) reason why negatives signs needed above in M.ij see a1 and a5 ***/
	/*** in this paper the fi is at 45 degrees                                                    ***/
	/************************************************************************************************/
	/***
		a1 = 0.5 * ( M.yy - M.xx ) * cos(2*fi) - M.xy * sin(2.0*fi);
		a2 = M.xz * cos( fi ) + M.yz * sin( fi );
		a3 = 0.5 * ( M.xx + M.yy );
		a4 = 0.5 * ( M.xx - M.yy ) * sin( 2.0 * fi ) - M.xy * cos( 2 * fi );
		a5 = M.yz * cos( fi ) - M.xz * sin( fi );
	***/

	/************************************/
	/*** compute the coefficients     ***/
	/*** these from Minson&Dreger2008 ***/
	/************************************/
		
		a1 = half * ( M.xx - M.yy ) * cos( 2.0 * fi ) + M.xy * sin( 2.0 * fi );
		a2 = M.xz * cos( fi ) + M.yz * sin( fi );
		a3 = -half*( M.xx + M.yy );
		
	/*** for deviatoric only, clvd and iso extracted out ***/
		if( input_type == MT_INPUT )
		 a3 = -sixth*( M.xx + M.yy - 2 * M.zz ); 

		a4 = half * ( M.xx - M.yy ) * sin( 2.0 * fi ) - M.xy * cos( 2.0 * fi );
		a5 = -M.yz * cos( fi ) + M.xz * sin( fi );
 
		if( verbose )
		{
	 	  fprintf( stdout, "%s: %s: %s: %s MT\t a1=%g\t a2=%g\t a3=%g\t a4=%g\t a5=%g\n", 
			progname, __FILE__, __func__, nsl, a1, a2, a3, a4, a5 );
		}



		for( it=0; it<nt; it++)
		{
		  ver[it] = ( a1 * zss[it] + a2 * zds[it] + a3 * zdd[it] ) * ev.Mdc + ( half * zdd[it] * ev.Mclvd ) + zep[it] * ev.Miso;
		  rad[it] = ( a1 * rss[it] + a2 * rds[it] + a3 * rdd[it] ) * ev.Mdc + ( half * rdd[it] * ev.Mclvd ) + rep[it] * ev.Miso;
		  tra[it] = ( a4 * tss[it] + a5 * tds[it] ) * ev.Mdc;           
		}
	}

	if( input_type == MT_INPUT )
	{
		ev.Mtotal = ev.Mo/base_moment;
		if( verbose ) 
		{
		  fprintf( stdout, "%s: %s: %s: %s Mtotal=%e Mo=%e\n",
			progname, __FILE__, __func__, nsl, ev.Mtotal, ev.Mo );
		}

		M.xx = ev.Mxx;
		M.yy = ev.Myy;
		M.zz = ev.Mzz;
		M.xy = ev.Mxy;
		M.xz = ev.Mxz;
		M.yz = ev.Myz;

		if( verbose )
                  fprintf( stdout, "%s: %s: %s: %s az=%g fi=%g MT_INPUT format Mxx=%g Mxx=%g Mzz=%g Mxy=%g Mxz=%g Myz=%g\n",
                        progname, __FILE__, __func__, nsl, g->az, fi,
                        M.xx, M.yy, M.zz, M.xy, M.xz, M.yz );

	/*** just to display and compare ***/	
		a1 = half * ( M.xx - M.yy ) * cos( 2.0 * fi ) + M.xy * sin( 2.0 * fi );
                a2 = M.xz * cos( fi ) + M.yz * sin( fi );
                /* a3 = -half*( M.xx + M.yy ); */
                a3 = -sixth*( M.xx + M.yy - 2 * M.zz );
                a4 = half * ( M.xx - M.yy ) * sin( 2.0 * fi ) - M.xy * cos( 2.0 * fi );
                a5 = -M.yz * cos( fi ) + M.xz * sin( fi );

                if( verbose )
                  fprintf( stdout, "%s: %s: %s: %s a1=%g a2=%g a3=%g a4=%g a5=%g\n",
                        progname, __FILE__, __func__, nsl, a1, a2, a3, a4, a5 );

	/*** just to display and compare ***/

	/**** Hutchenson and Herrmann (1993) - not Jost&Herrmann(1989) - see Minson and Dreger (2008) GJI ***/

		for( it = 0; it < nt; it++ )
		{
		  tra[it]=( M.xx * (  half * sin(2*fi) * tss[it] )
			+ M.yy * ( -half * sin(2*fi) * tss[it] )
			+ M.xy * (       -cos(2*fi) * tss[it] )
			+ M.xz * (        sin(fi)   * tds[it] )
			+ M.yz * (       -cos(fi)   * tds[it] ) ) * ev.Mtotal;

		  rad[it]=( M.xx * ( -sixth*rdd[it] + half*cos(2*fi)*rss[it] + third*rep[it] )
			+ M.yy * ( -sixth*rdd[it] - half*cos(2*fi)*rss[it] + third*rep[it] )
			+ M.xy * (  sin(2*fi) * rss[it] )
			+ M.xz * (  cos(fi)   * rds[it] )
			+ M.yz * (  sin(fi)   * rds[it] )
			+ M.zz * (  third*rdd[it] + third*rep[it] ) ) * ev.Mtotal;

		  ver[it]=( M.xx * ( -sixth*zdd[it] + half*cos(2*fi)*zss[it] + third*zep[it] )
			+ M.yy * ( -sixth*zdd[it] - half*cos(2*fi)*zss[it] + third*zep[it] )
			+ M.xy * (  sin(2*fi) * zss[it] )
			+ M.xz * (  cos(fi)   * zds[it] )
			+ M.yz * (  sin(fi)   * zds[it] )
			+ M.zz * (  third*zdd[it] + third*zep[it] ) ) * ev.Mtotal;
		}
	}

	if( fiber == FIBER_SH )
	{
		fact = radpat_fiber_trans_degrees( g->baz, azi_fiber );

		fprintf( stderr, "%s: %s: %s: called radpat_fiber_trans_degrees() fiber=%d(FIBER_SH) fact=%g gbaz=%g azi_fiber=%g\n",
                        progname, __FILE__, __func__, fiber,
                        fact, g->baz, azi_fiber );

		for( it=0; it<nt; it++ )
                {
                        tmpt = fact * tra[it];
                        tmpr = fact * rad[it];
                        tmpz = fact * ver[it];
                        tra[it] = tmpt;
                        rad[it] = tmpr;
                        ver[it] = tmpz;
                }
	}
	else if( fiber == FIBER_PSV )
	{
		facl = radpat_fiber_long_degrees( g->baz, azi_fiber );

		fprintf( stderr, "%s: %s: %s: called radpat_fiber_long_degrees() fiber=%d(FIBER_PSV) facl=%g gbaz=%g azi_fiber=%g\n",
			progname, __FILE__, __func__, fiber, 
			facl, g->baz, azi_fiber );

		for( it=0; it<nt; it++ )
		{
			tmpt = facl * tra[it];
			tmpr = facl * rad[it];
			tmpz = facl * ver[it];
			tra[it] = tmpt;
			rad[it] = tmpr;
			ver[it] = tmpz;
		}
	}
	else if( fiber == FIBER_NONE )
	{
		fprintf( stderr, "%s: %s: %s: fiber=%d(FIBER_NONE) not using fiber correction\n",
			progname, __FILE__, __func__, fiber );
	}
	else
	{
		fprintf( stderr, "%s: %s: %s: unknown fiber = %d option\n",
			progname, __FILE__, __func__, fiber );
	}

	if( inoise )
	{
		noise_moment = pow( 10, (1.5*(noise_Mw + 10.73)));
		fac = 0.1 * peak * ( noise_moment/base_moment );

		fprintf( stdout, "%s: %s: %s: fac=%e peak=%e noise_moment=%e noise_Mw=%g iseed=%d\n",
			progname, __FILE__, __func__,
			fac, peak, noise_moment, noise_Mw, iseed );

		for( it=0; it<nt; it++ )
		{
			tra[it] += fac*gasdev( &iseed );
			rad[it] += fac*gasdev( &iseed );
			ver[it] += fac*gasdev( &iseed );
		}
	}

/**************************************************************************************************/
/*** convert displacement from meters to cm ***/
/**************************************************************************************************/
	fprintf( stdout, "%s: %s: %s: %s dounits_cm2m=%d [t,r,z] = amps * (cm2m=%g)\n",
		progname, __FILE__, __func__, nsl, dounits_cm2m, cm2m );

	if( dounits_cm2m )
	{
	 for( it=0; it<nt; it++ )
	 {
		tra[it] *= cm2m;
		rad[it] *= cm2m;
		ver[it] *= cm2m;
	 }

	 if(verbose) 
	 {
		fprintf( stdout, "%s: %s: %s: %s converting amplitudes cm to meters dounits_cm2m=%d\n",
			progname, __FILE__, __func__, nsl, dounits_cm2m );
	 }
	}

/**************************************************************************************************/
/*** rotate ***/
/**************************************************************************************************/

	ns = calloc( nt, sizeof(float) );
	ew = calloc( nt, sizeof(float) );
	for( it=0; it<nt; it++ )
	{
		ew[it] = tra[it];
		ns[it] = rad[it];
	}

/*** polarities reverse ***/
/***
	angle = -1 * g->baz;
	az = g->baz;
	tmpaz = g->baz + 90;
***/
/**** this seems to work, tested with SAC rotate command ***/
	angle =  360 - g->az;
        az = g->az;
        tmpaz = g->az + 90;
/***********/
	cmpinc = 90;
	rotate( ns, nt, &az, cmpinc, ew, nt, &tmpaz, cmpinc, angle, 1 );

	if(verbose) 
	{
	  fprintf( stdout, "%s: %s: %s: rotated to RADIAL(%g)->NS TRANSVERSE(%g)->EW by angle = %g\n",
		progname, __FILE__, __func__, g->az, tmpaz, angle );
	}

/**************************************************************************************************/
/*** interpolate to dt = 0.05 sec/sample using interpolation scheme interpolate_fft()          ***/
/**************************************************************************************************/

	if(verbose)
		fprintf( stdout, "%s: %s: %s: dointerp=%d interpolate using interpolate_fft()\n",
			progname, __FILE__, __func__, dointerp );

	if( dointerp )
	{
	 new_dt = 0.05;
	 new_dt = 0.02;

	 new_nt = twin/new_dt;

	 xtra = calloc( new_nt, sizeof(float) );
	 xrad = calloc( new_nt, sizeof(float) );
	 xver = calloc( new_nt, sizeof(float) );
	 xew  = calloc( new_nt, sizeof(float) );
	 xns  = calloc( new_nt, sizeof(float) );

	 duplicate_vector( tra, xtra, new_nt );
         duplicate_vector( rad, xrad, new_nt );
         duplicate_vector( ver, xver, new_nt );
         duplicate_vector(  ew,  xew, new_nt );
         duplicate_vector(  ns,  xns, new_nt );

 /*** interpolate/interpolate_subs.c ***/

	 interpolate_fft( xtra, nt, dt, &new_nt, new_dt );
	 interpolate_fft( xrad, nt, dt, &new_nt, new_dt );
	 interpolate_fft( xver, nt, dt, &new_nt, new_dt );
	 interpolate_fft( xns,  nt, dt, &new_nt, new_dt );
	 interpolate_fft( xew,  nt, dt, &new_nt, new_dt );

	 if(verbose) 
		fprintf( stdout, "%s: interpolated to nt=%d dt=%g\n", 
			progname, new_nt, new_dt );
	}	
	else
	{
		new_nt = nt;
		new_dt = dt;
		xtra = calloc( new_nt, sizeof(float) );
        	xrad = calloc( new_nt, sizeof(float) );
        	xver = calloc( new_nt, sizeof(float) );
        	xew  = calloc( new_nt, sizeof(float) );
        	xns  = calloc( new_nt, sizeof(float) );

		duplicate_vector( tra, xtra, new_nt );
		duplicate_vector( rad, xrad, new_nt );
		duplicate_vector( ver, xver, new_nt );
		duplicate_vector(  ew,  xew, new_nt );
		duplicate_vector(  ns,  xns, new_nt );
	}

	nt = new_nt;
	dt = new_dt;

	if(verbose)
		fprintf( stdout, "%s: %s: %s: nt=%d dt=%g\n", progname, __FILE__, __func__, nt, dt );

/*****************************************/
/**** SAC HEADER                     *****/
/*****************************************/
	sp = sac_null;

	sp.nvhdr = 6;
	sp.norid = 0;
	sp.nevid = 0;
	sp.iftype = ITIME;
	sp.idep = IUNKN;
	sp.iztype = IO;
	sp.ievtyp = IQUAKE;
	sp.leven = TRUE;
	sp.lpspol = FALSE;
	sp.lcalda = TRUE;

	sp.npts = nt;
	sp.delta = dt;
	sp.b = t0;
	sp.e = e;

/**************************************************************************************************/
/*** set origin time ***/
/**************************************************************************************************/

	sp.nzyear = ev.ot.year;
	sp.nzjday = ev.ot.jday;
	sp.nzhour = ev.ot.hour;
	sp.nzmin  = ev.ot.min;
	sp.nzsec  = ev.ot.isec;
	sp.nzmsec = ev.ot.msec;
	sp.o      = 0;
	strcpy( sp.ko, "OT" );

/**************************************************************************************************/
/*** set origin hypocenter and receiver locations ***/
/**************************************************************************************************/

	sp.evla = g->evla;
	sp.evlo = g->evlo;
	sp.evdp = g->evdp;
	sp.stla = g->stla;
	sp.stlo = g->stlo;
	sp.stel = g->stel;

	/*** Reduction Velocity km/sec ***/
	if( g->redv > 0 )
	{
         fprintf( stderr, "%s: %s: %s: setting reduced velocity redv=%g (km/sec) rdist=%g\n",
		progname, __FILE__, __func__, g->redv, g->rdist );

	 sprintf( sp.kt1, "%.1fkps", g->redv ); 

	 sp.t1 = g->rdist / g->redv;
	}
	sp.user0 = ev.str; strcpy( sp.kuser0, "Str" );
	sp.user1 = ev.dip; strcpy( sp.kuser1, "Dip" );
	sp.user2 = ev.rak; strcpy( sp.kuser2, "Rak" );

	sp.user3 = ev.Mo;
	sp.user4 = ev.Mw;
	sp.user5 = ev.tr;
	sp.user6 = ev.tt;
	sp.mag  = ev.Mw;

	strcpy( sp.kstnm, g->stnm );
	strcpy( sp.knetwk, g->net );
	strcpy( sp.khole, g->loc );

/**************************************************************************************************/
/*** EAST-WEST component ***/
/**************************************************************************************************/

	strcpy( sp.kcmpnm, "BHE" );
	sp.cmpinc = 90;
	sp.cmpaz  = 90;

	write_SACPZ_file( sp.kstnm, sp.knetwk, sp.khole, sp.kcmpnm, sp.nzyear, sp.nzjday, sp.nzhour, sp.nzmin );

	sprintf( sacfile, "%4d.%03d.%02d.%02d.%02d.0000.%s.%s.%s.%s.SAC",
                sp.nzyear, sp.nzjday, sp.nzhour, sp.nzmin, sp.nzsec,
                sp.knetwk, sp.kstnm, sp.khole, sp.kcmpnm );

	fprintf( stdout, "%s: %s: %s: writting file %s\n",
		progname, __FILE__, __func__, sacfile );

	if( (fp=fopen(sacfile,"w")) == NULL )
		fprintf( stderr, "%s: %s: %s: ERROR cannot write to file\n", progname, __FILE__, __func__ );
	fwrite( &sp, sizeof(Sac_Header), 1, fp );
	fwrite( &xew[0], sp.npts*sizeof(float), 1, fp );
	fclose(fp);

/**************************************************************************************************/
/*** NORTH-SOUTH Component ***/
/**************************************************************************************************/

	strcpy( sp.kcmpnm, "BHN" );
	sp.cmpinc = 90;
	sp.cmpaz  = 0;

	write_SACPZ_file( sp.kstnm, sp.knetwk, sp.khole, sp.kcmpnm, sp.nzyear, sp.nzjday, sp.nzhour, sp.nzmin );

	sprintf( sacfile, "%4d.%03d.%02d.%02d.%02d.0000.%s.%s.%s.%s.SAC",
                sp.nzyear, sp.nzjday, sp.nzhour, sp.nzmin, sp.nzsec,
                sp.knetwk, sp.kstnm, sp.khole, sp.kcmpnm );

	fprintf( stdout, "%s: %s: %s: writting file %s\n",
                progname, __FILE__, __func__, sacfile );

	if( (fp=fopen(sacfile,"w")) == NULL )
		fprintf( stderr, "%s: %s: %s: ERROR cannot write to file\n", progname, __FILE__, __func__ );
	fwrite( &sp, sizeof(Sac_Header), 1, fp );
	fwrite( &xns[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

/**************************************************************************************************/
/*** radial ***/
/**************************************************************************************************/

	sprintf( sp.kcmpnm, "BH1" );
	sp.cmpinc = 90;
	sp.cmpaz = g->az;

	write_SACPZ_file( sp.kstnm, sp.knetwk, sp.khole, sp.kcmpnm, sp.nzyear, sp.nzjday, sp.nzhour, sp.nzmin );

	sprintf( sacfile, "%4d.%03d.%02d.%02d.%02d.0000.%s.%s.%s.%s.SAC",
                sp.nzyear, sp.nzjday, sp.nzhour, sp.nzmin, sp.nzsec,
                sp.knetwk, sp.kstnm, sp.khole, sp.kcmpnm );

	fprintf( stdout, "%s: %s: %s: writting file %s\n",
                progname, __FILE__, __func__, sacfile );

	if( (fp=fopen(sacfile,"w")) == NULL )
		fprintf( stderr, "%s: %s: %s: ERROR cannot write to file\n", progname, __FILE__, __func__ );
	fwrite( &sp, sizeof(Sac_Header), 1, fp );
	fwrite( &xrad[0], sp.npts*sizeof(float), 1, fp );
	fclose(fp);

/**************************************************************************************************/
/*** vertical ***/
/**************************************************************************************************/

	sprintf( sp.kcmpnm, "BHZ" );
	sp.cmpinc = 0;
	sp.cmpaz = 0;

	write_SACPZ_file( sp.kstnm, sp.knetwk, sp.khole, sp.kcmpnm, sp.nzyear, sp.nzjday, sp.nzhour, sp.nzmin );

	sprintf( sacfile, "%4d.%03d.%02d.%02d.%02d.0000.%s.%s.%s.%s.SAC",
                sp.nzyear, sp.nzjday, sp.nzhour, sp.nzmin, sp.nzsec,
                sp.knetwk, sp.kstnm, sp.khole, sp.kcmpnm );

	fprintf( stdout, "%s: %s: %s: writting file %s\n",
                progname, __FILE__, __func__, sacfile );

	if( (fp=fopen(sacfile,"w")) == NULL ) 
		fprintf( stderr, "%s: %s: %s: ERROR cannot write to file\n", progname, __FILE__, __func__ );
	fwrite( &sp, sizeof(Sac_Header), 1, fp );
	fwrite( &xver[0], sp.npts*sizeof(float), 1, fp );
	fclose(fp);

/**************************************************************************************************/
/*** transverse ***/
/**************************************************************************************************/

	tmpaz = g->az+90;
	if( tmpaz>360 ) tmpaz-=360;
	sp.cmpaz = tmpaz;
	sp.cmpinc = 90;
	sprintf( sp.kcmpnm, "BH2" );

	write_SACPZ_file( sp.kstnm, sp.knetwk, sp.khole, sp.kcmpnm, sp.nzyear, sp.nzjday, sp.nzhour, sp.nzmin );

	sprintf( sacfile, "%4d.%03d.%02d.%02d.%02d.0000.%s.%s.%s.%s.SAC",
                sp.nzyear, sp.nzjday, sp.nzhour, sp.nzmin, sp.nzsec,
                sp.knetwk, sp.kstnm, sp.khole, sp.kcmpnm );

        fprintf( stdout, "%s: %s: %s: writting file %s\n",
                progname, __FILE__, __func__, sacfile );
        if( (fp=fopen(sacfile,"w")) == NULL ) 
		fprintf( stderr, "%s: %s: %s: ERROR cannot write to file\n", progname, __FILE__, __func__ );
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &xtra[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	free( tra );
	free( rad );
	free( ver );
	free( xtra );
	free( xrad );
	free( xver );
	free( ns );
	free( ew );
	free( xns );
	free( xew );

} /*** end of grn2disp2.c ***/

void Usage_Print()
{
	fprintf(stderr, "\n");
	fprintf(stderr, "USAGE: %s: glib= z= [no]verbose ( [no]dumpgrn wavetype= [no]plotgrn ) tr=[0] tt=[0]", progname );

	fprintf(stderr, "  type=[no default see below for additional options]\n\n" );

	fprintf(stderr, "\t REQUIRED ARGUMENTS:\n");
	fprintf(stderr, "\t\t glib= Green's function library (GLIB) name computed from mkgrnlib.c [string required]\n");
	fprintf(stderr, "\t\t z= the depth of the source to look up in the GLIB file [float required]\n");
	fprintf(stderr, "\n");

	fprintf(stderr, "\t OPTIONAL PARAMETERS:\n");
	fprintf(stderr, "\t\t [no]dumpgrn  write out only Green's functions [boolean default off]\n");
	fprintf(stderr, "\t\t\t if dumpgrn then wavetype is required with either wavetype=\"Surf/Pnl\" or wavetype=\"Rotational\" no default\n" );
	fprintf(stderr, "\t\t\t if dumpgrn then plotgrn is optional, makes GMT plot of Greens functions default off\n" );

	fprintf(stderr, "\t\t [no]verbose  verbosy output for diagnosis [boolean default off]\n");
	fprintf(stderr, "\t\t date         origin-time format: YYYY/MM/DD,HH24:mm:ss.ss [string default \"2008/01/01:00:00:00.00\"]\n");
	fprintf(stderr, "\t\t dounits_cm2m multiply ground motion output by 0.01 to convert cm to meters boolean [boolean default off]\n" );
	fprintf(stderr, "\t\t dointerp     do Wiggins interpolation to new sampling rate [boolean default off] new dt = 0.02 sec/samp\n" );
	fprintf(stderr, "\n");

	fprintf(stderr, "\t\t OPTIONAL NOISE PARAMETERS:\n");
	fprintf(stderr, "\t\t [no]noise    add white Gaussian noise (default off)\n");
        fprintf(stderr, "\t\t nMw=3.8      level of the noise in units of Mw for freq band of interest [no default required]\n");
        fprintf(stderr, "\t\t seed=1       random seed default 1\n");
        fprintf(stderr, "\n");

	fprintf(stderr, "\t\t OPTIONAL SOURCE TIME FUNCTION:\n" );
	fprintf(stderr, "\t\t tr=0.0      Rise time in seconds (triangle or trapazoid src time func) [float default 0]\n");
	fprintf(stderr, "\t\t tt=0.0      source duration in sec.  tt=0 if triangle, or tt>0 if trapazoid [float default 0]\n");
	fprintf(stderr, "\n");

	fprintf(stderr, "\t\t OPTIONAL Source Input Mode Type\n");
	fprintf(stderr, "\t\t\t =0   Input Moment Tensor + Mw\n");
	fprintf(stderr, "\t\t\t =1   Input Double-Couple Source (Strike/Dip/Rake) + Mw\n");
	fprintf(stderr, "\t\t\t =2   Input Deviatoric source plus isotropic component\n");
	fprintf(stderr, "\n"); 

	fprintf(stderr, "\t\t Input Moment Tensor (type=0)\n");
	fprintf(stderr, "\t\t\t Mxx=Normalized Element\n");
	fprintf(stderr, "\t\t\t Myy=Normalized Element\n");
	fprintf(stderr, "\t\t\t Mzz=Normalized Element\n");
	fprintf(stderr, "\t\t\t Mxy=Normalized Element\n");
	fprintf(stderr, "\t\t\t Mxz=Normalized Element\n");
	fprintf(stderr, "\t\t\t Myz=Normalized Element\n");
	fprintf(stderr, "\t\t\t Mo=Total Moment\n");
	fprintf(stderr, "\n");

	fprintf(stderr, "\t\t Input Pure Double-Couple Source (type=1)\n");
	fprintf(stderr, "\t\t\t str= fault strike\n");
	fprintf(stderr, "\t\t\t dip= fault dip\n");
	fprintf(stderr, "\t\t\t rak= fault rake\n");
	fprintf(stderr, "\t\t\t Mw= Total Moment Magnitude\n");
	fprintf(stderr, "\n");

	fprintf(stderr, "\t\t Input Deviatoric source plus isotropic component (type=2)\n");
	fprintf(stderr, "\t\t\t str= fault strike\n");
	fprintf(stderr, "\t\t\t dip= fault dip\n");
	fprintf(stderr, "\t\t\t rak= fault rake\n");
	fprintf(stderr, "\t\t\t Mw= Total Moment Magnitude\n");
	fprintf(stderr, "\t\t\t Piso=  [0..1] The percent of the total moment allocated to isotropic\n");
	fprintf(stderr, "\t\t\t Pclvd= [0..1] The percent of the total moment allocated to clvd\n");
	fprintf(stderr, "\t\t\t Pdc=   [0..1] The percent of the total moment allocated to double-couple\n");
	fprintf(stderr, "\t\t\t Piso+Pclvd+Pdc must add up to 1.0 \n" );
	fprintf(stderr, "\n");
	
	fprintf(stderr, "\t\t OPTIONAL DAS ARRAY PROCESSING:\n" );
	fprintf(stderr, "\t az=%%f  source-to-receiver azimuth override [float default -999 off], good for simulating array processing\n" );
	fprintf(stderr, "\t wave_type_fiber=%%s  [default: none] options=none,long,l,lognitudinal,shear,t,compressional,transverse,trans\n");
	fprintf(stderr, "\t azi_fiber=%%f direction of the cable (0-360 degrees)\n");
	fprintf(stderr, "\n");

} /*** end of Print_Usage.c  ***/


/* write_SACPZ_file( sp.kstnm, sp.knetwk, sp.khole, sp.kcmpnm, sp.nzyear, sp.nzjday, sp.nzhour, sp.nzmin ); */

void write_SACPZ_file( char *sta, char *net, char *loc, char *cmp, int year, int jday, int hour, int min )
{
	FILE *fp;
	char sacpzfilename[256];
	sprintf( sacpzfilename, "SAC_PZs_%s_%s_%s_%s_%4d.%03d.%02d.%02d.00.0000",
		net, sta, loc, cmp, year, jday, hour, min );
	fp = fopen( sacpzfilename, "w" );
	fprintf( fp, "ZEROS 0\n" );
	fprintf( fp, "POLES 0\n" );
	fprintf( fp, "CONSTANT 1.00E+00\n" );
	fclose(fp);
}
