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

/*** this is the main moment tensor inversion code ***/

/***

G. Ichinose Wed Jan 10 17:22:21 PST 2018
  Kludge to solve problems with random memory leak bug...

  changed all instances of ev[ista].ew.data[it] -> ev[ista].ew.data_safe[it]       
                           ev[ista].ns.data[it] -> ev[ista].ns.data_safe[it]
                           ev[ista].z.data[it]  -> ev[ista].z.data_safe[it]

         data[it] is dynamically allocated and still used important for sacdata2inv.c
         data_safe[it] is statically allocated with predefined size of 8192

Wed Aug  5 08:59:12 PDT 2020 g.ichinose
	added option for largest %VR DEV-MT solution search

Thu Aug  6 20:52:35 PDT 2020
	mtinv_gmtsubs.c: wfplot2_gmt5() add option (sort_by_value=none,dis,azi,baz) to plot waveforms ordered by distance, azimuth or baz in gmtwf.csh

Mon Aug 10 12:49:16 PDT 2020
	pass fixz write_results2() realtime_subs.c to mtbestfit.c:make_run() to include in run2.csh  script
	mtinv_gmtsubs.c: wfplot2_gmt5() added print_mt_info_text option of gmtwf.csh and gmtwf.*.jpg files

Fri Aug 14 22:29:50 PDT 2020
Todo list...
	1. need to merge changes from  mtinv_gmtsubs.c._from_jorge_2020March_ 
	2. mtinv_gmtsubs.c: wfplot2_gmt5() need add option to gmtwf.csh to go between plots grayscale or color
***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <errno.h>
#include "../include/nrutil.h"     /** numerical recipes **/
#include "../include/mt.h"         /** global datatype and structure declarations **/

char progname[128];
long evid;

int main(int ac, char **av)
{

/************************/
/*** event info stuff ***/
/************************/
	EventInfo *ev;
	int ista, nsta;
	char evinfo_filename[128];

	int it;

/************************/
/*** Greens stuff     ***/
/************************/
        Greens **grn;
	/* FILE *fpg; */
	int nz,iz;
	float *z;  /*** for backwards compat... also access by z[ista].z ***/
	DepthVector *zvec;  /*** see ../include/mt.h ***/

/****************/
/*** Solution ***/
/****************/
	Solution *sol;
	int iz_best;
	float bestfit, mechanism_size; 
	int npages;

/*******************/
/*** local stuff ***/
/*******************/
	/* int err; */
	int verbose=0;		/** verbose   1=yes 0=no default no verbose ***/
	int idump=0;
	int igmtmap=0;
	int mtdegfree=5;
	int forward=0;
	float ts0;  /***no default***/
	int check_on_status_ok;
	int FixMyiz;
	float FixMyZ = -99;
	int Distance_Normalize = 0;
	float DistNormR0 = 1; /*** default is R0 = 1 km  in R/R0 ****/

	int dev_mt_largest_vr = 0;  /*** best fit dev-mt uses only largest percent variance reduction no max dc ***/

	int ishift = 0;
	float cortol = 1.0;
	float maxtimeshift = 0; /*** about 1 cycle 25 < maxtimeshift < 5 sec ***/

	int PltXcorLabel = 1;
	int idumpxy = 0;
	int iorientation = LANDSCAPE; /* orientation for gmtwf.csh */
	char orientation[12];

	int sqlite3_db_write = 0;
	int mysql_db_write = 0;
	int oracle_db_write = 0;
	
	int igmt5 = 1; /*** default true GMT version 5.x.x ***/
	int iuse_snr = 0;
	float MINSNR = 3;
	extern long evid;
	char evidstr[32];
	int debug = 0;  /*** writes out data as sac files after loading ***/
	
	int specialLoadGrnMxy = 0;
	int compute_station_vred = 1;

/*******************/
/*** subroutines ***/
/*******************/

	/*** glib2inv_subs.c ***/
	EventInfo *glib2inv_get_input_parameters( char *filename, EventInfo *ev, int *n, int verbose );

	/*** writesacfile.c ***/
	void writesacfile( EventInfo *ev );

	/*** mtinv_subs.c ***/
	void mk_gmtplot_dir( char *output_pathname, int verbose );

	/*** mtinv_subs.c ***/
	void load_the_data( EventInfo *ev, int nsta, float ts0, int verbose );

	/*** mtinv_subs.c ***/

	/*** deprecated, replaced by Greens_subs.c:loadGlibAll()  ***/
	/* float *load_greens( EventInfo *ev, Greens **grn, int nsta, int *nz_tmp, int verbose ); */

	/*** Greens_subs.c:loadGlibAll() ****/
	Greens **loadGlibAll( Greens **grn, EventInfo *ev, DepthVector *z, int nsta, char *file_type, int verbose );

	/*** load_special_grns.c ***/
	float *load_special_grns( EventInfo *ev, Greens **grn, int nsta, int *nz_tmp, int verbose );

	/*** mtinv.c ***/
	void forward_calc(	
			EventInfo *ev,
			Greens **grn,
			int nsta,
			int nz,
			float *z,
			Solution *sol,
			int verbose,
			int idump, 
			int mtdegfree,
			int Distance_Normalize,
			float DistNormR0 );

	/*** mtinv.c ***/
	void invert(	EventInfo *ev,
			Greens **grn,
			int nsta,
			int nz,
			Solution *sol,
			int verbose, 
			int idump,
			int mtdegfree,
			int Distance_Normalize,
			float DistNormR0,
			int specialLoadGrnMxy,
			int dev_mt_largest_vr );

	/*** psplot.c ***/
	int psplot(	int nsta,
			int iz,
			char *filenbase,
			EventInfo *ev,
			Solution *sol,
			Greens **grn,
			int units,
			int verbose,
			int forward,
			int PltXcorLabel,
			int PS_Output_Screen );

	/*** check_depths.c ***/
	void check_depth( float FixMyZ, int *FixMyiz, float *z, int nz, int verbose );

	/*** mtinv_subs.c ***/
	void compute_synthetics( int is, int iz, EventInfo *ev, Greens **grn, Solution *sol, int mtdegfree );
	void compute_synthetics_special( int ista, int iz, EventInfo *ev, Greens **grn, Solution *sol, int mtdegfree );

	/*** mtinv_subs.c ***/
	int write_emails = 0; /* default off, dont write emails */
	void write_email_message( FILE *fp, int nsta, int iz, Solution *sol, EventInfo *ev, Greens **grn, int ifwd );

	/*** realtime_subs.c ***/
	void write_results2(
	        char *results_filename,
		Solution *sol,
		EventInfo *ev,
		Greens **grn,
		int iz,
		int nsta,
		int npages,
		int AutoAuth,
		int ishift,
		float cortol,
		float maxtimeshift,
		int iuse_snr,
		float minsnr, 	
		int igmt5, 
		int sqlite3_db_write,
		int mysql_db_write,
		int oracle_db_write,
		float FixMyZ );

	/*** mtinv_gmtsubs.c ***/
	void plot_z_vs_fit_gmt5( int iz, float *z, Solution *sol, EventInfo *ev );
	void plot_z_vs_fit_gmt4( int iz, float *z, Solution *sol, EventInfo *ev );

	/*** mtinv_gmtsubs.c ***/
	void plotmech_gmt5( int iz, Solution *sol, EventInfo *ev, float mechanism_size );
	void plotmech_gmt4( int iz, Solution *sol, EventInfo *ev, float mechanism_size );

	/*** mtinv_subs.c ***/
	void write_sol_rec( FILE *fp, int iz, int nsta, EventInfo *ev, Solution *sol, Greens **grn );

	/*** mtinv_gmtsubs.c ***/
	void make_map_gmt5( int iz, int nsta, EventInfo *ev, Solution *sol, Greens **g, int verbose );
	void make_map_gmt4( int iz, int nsta, EventInfo *ev, Solution *sol, Greens **g, int verbose );

	/*** crosscorrelation/cross_correlation.c ***/
	void xcorr(
		float *d, float *s, 
		int nt, float dt, int *ilag, float *tlag,
		float *xcorcoef, 
		float cortol, int verbose, int ishift );

	/*** find_best_shift.c ***/
	void find_best_shift( EventInfo *ev, float cortol, float maxtimeshift, float *time_shift_all );

	/*** mtinv_subs.c ***/
	void time_shift( EventInfo *ev, int nsta, int verbose );

	/*** mtinv_gmtsubs.c ***/
	void plot_results_gmt4( int iz, Solution *sol, EventInfo *ev );
	void plot_results_gmt5( int iz, Solution *sol, EventInfo *ev );

	/*** mtinv_subs.c ***/
	void calc_azi_gap_and_dmin( int iz, int nsta, EventInfo *ev, Solution *sol, Greens **g );

	/*** mtinv_subs.c ***/
	void write_mterror( FILE *fp, int nz, Solution *sol, EventInfo *ev, Greens **g );
	
	/*** mtinv.c ***/
	void Usage_Print( void );

	/*** mtinv_gmtsubs.c ***/
	void gmt4wfplot( EventInfo *ev, Solution *sol, Greens **grn, int nsta, int iz, int iorientation, int verbose );
	void gmt5wfplot( EventInfo *ev, Solution *sol, Greens **grn, int nsta, int iz, int iorientation, int verbose );

	/*** mtinv_subs.c ***/
	void dumpxy( EventInfo *ev, Solution *sol, Greens **grn, int nsta, int iz, int verbose );

	/*** mtinv_gmtsubs.c ***/
	char sort_by_value[32];
	int print_gmtwf_mt_info = 0;
	void wfplot2_gmt5( EventInfo *ev, Solution *sol, Greens **grn, int nsta, int iz, int verbose, char *sort_by_value, int print_gmtwf_mt_info );
	void wfplot2_gmt4( EventInfo *ev, Solution *sol, Greens **grn, int nsta, int iz, int verbose, char *sort_by_value, int print_gmtwf_mt_info );

	/*** dumpSAC.c ***/
	void computeStationChannel_VarianceReduction_v2(
                int nsta, int iz_best, EventInfo *ev, Greens **grn, Solution *sol, int verbose );

/*** update dbsubs.c routines  ***/

	void db_sqlite3_create( void );
	void  mysql_db_create( void );
	void oracle_db_create( void );

	void db_sqlite3_write( EventInfo *ev, Solution *sol, Greens **grn, int nsta, int iz, int AutoAuth, int verbose );
	void  db_mysql_write( EventInfo *ev, Solution *sol, Greens **grn, int nsta, int iz, int AutoAuth, int verbose );
	void db_oracle_write( EventInfo *ev, Solution *sol, Greens **grn, int nsta, int iz, int AutoAuth, int verbose ); 
	int AutoAuth = 0;
	/*** added 2/2022 by g.ichinose to work along side with sqlite3 output for llnl.GMPDB MTLoader ***/
	void llnl_db_write(  EventInfo *ev, Solution *sol, Greens **grn, int nsta, int iz, int AutoAuth, int verbose );

/*** ../misc/getpar/ - libget.a(getarg.o) ***/

	int setpar(int,char **);
	int mstpar( char *, char *, void * );
        int getpar( char *, char *, void * );
        void endpar();

/*** shorten_path.c ***/
        char *shorten_path( char *pathname, char *filename );

/**************/
/*** output ***/
/**************/
	char pathname[128];
	char asc_file_name[128], ps_plot_filename[128];
	FILE *fpasc;
	
/*********************************************************************************************************************************************/
/********************************** START mtinv.c(main()) ************************************************************************************/
/*********************************************************************************************************************************************/

/********************/
/*** program name ***/
/********************/
	strcpy( pathname, av[0] );
	shorten_path( pathname, progname );

	if( verbose )
	{
	  fprintf( stdout, "%s: %s: %s: STDOUT: Version=%s ReleaseDate=%s exec full path=%s\n",
                progname,
		__FILE__,
		__func__,
		Version_Label, Version_Date, pathname );
	}

        fprintf( stderr, "%s: %s: %s: STDERR: Version=%s ReleaseDate=%s exec full path=%s\n",
                progname,
		__FILE__,
                __func__,
		Version_Label, Version_Date, pathname );

/*************/
/*** USAGE ***/
/*************/
	if( ac <= 1 )
	{
		Usage_Print();
	}

/*****************************************************/
/*** get the input parameters foreach each station ***/
/*****************************************************/
	setpar(ac,av);
	getpar( "largest_vr", "b", &dev_mt_largest_vr );
	mstpar( "par", "s", &evinfo_filename );
	mstpar( "mtdegfree", "d", &mtdegfree );
	mstpar( "ts0", "f", &ts0 );

	getpar( "fwd", "b", &forward );
	getpar( "fixz", "f", &FixMyZ );
	getpar( "verbose", "b", &verbose );
	getpar( "dumpsac", "b", &idump );
	getpar( "gmtmap", "b", &igmtmap );
	getpar( "write_emails", "b", &write_emails );
	getpar( "norm", "b", &Distance_Normalize );
	if( Distance_Normalize )
	{
		mstpar( "R0", "f", &DistNormR0 );
	}
	getpar( "PltXcorLabel", "b", &PltXcorLabel );

	getpar( "shift", "b", &ishift );
	if(ishift) 
	{
		mstpar( "ctol", "f", &cortol );
		mstpar( "maxshift", "f", &maxtimeshift );
	}
	getpar( "use_snr", "b", &iuse_snr );
	if( iuse_snr )
	{
		mstpar( "minsnr", "f", &MINSNR );
	}
	getpar( "dumpxy", "b", &idumpxy );
	if( idumpxy ) 
	{
		getpar( "orientation", "s", &orientation );
		if( strcmp( orientation, "portrait" ) == 0 ) 
		{
			iorientation = PORTRAIT;
		}
		else if( strcmp( orientation, "landscape" ) == 0 ) 
		{
			iorientation = LANDSCAPE;
		}
		else
		{
		  fprintf( stdout, 
		    "%s: %s: %s: unknown orientation = %s(%d) (valid responses = portrait or landscale) default landscape\n",
			progname,
			__FILE__,
                	__func__,
			orientation, iorientation );
		}
	}

	getpar(   "sqlite", "b", &sqlite3_db_write );
	getpar(  "sqlite3", "b", &sqlite3_db_write );

	getpar(    "mysql", "b", &mysql_db_write );
	getpar(   "oracle", "b", &oracle_db_write );
	getpar( "AutoAuth", "b", &AutoAuth );
	evid = -1;
	getpar( "evid", "s", &evidstr );
	evid = strtol( evidstr, NULL, 10 );
	getpar( "gmt5", "b", &igmt5 );
	getpar( "special", "b", &specialLoadGrnMxy );
	strcpy( sort_by_value, "dist" ); /***Default dist, options: none, dist, azi, baz ***/
	getpar( "sort_by_value", "s", sort_by_value );
	getpar( "print_gmtwf_mt_info", "b", &print_gmtwf_mt_info );
	getpar( "compute_station_vred", "b", &compute_station_vred );
	endpar();

/*********************/
/**** START MAIN  ****/
/*********************/

	if(verbose)
	{
		fprintf(stderr, "%s: %s: %s: verbose ON\n", progname, __FILE__, __func__ );
		fprintf(stdout, "%s: %s: %s: verbose ON\n", progname, __FILE__, __func__ );
	}
	else
	{
		fprintf(stderr, "%s: %s: %s: verbose OFF\n", progname, __FILE__, __func__ );
	}
	
	if(iuse_snr)
	{
		if(verbose)
		fprintf(stdout, "%s: %s: %s: iuse_snr ON minsnr=%g\n", progname, __FILE__, __func__, MINSNR );
                fprintf(stderr, "%s: %s: %s: iuse_snr ON minsnr=%g\n", progname, __FILE__, __func__, MINSNR );
	}
	else
	{
		if(verbose)
		fprintf(stdout, "%s: %s: %s: iuse_snr OFF\n", progname, __FILE__, __func__ );
                fprintf(stderr, "%s: %s: %s: iuse_snr OFF\n", progname, __FILE__, __func__ );
	}

	if(verbose)
	{
		if(mtdegfree==1)fprintf(stdout, "%s: %s: %s: mtdegfree=%d EXPLOSION\n", progname, __FILE__, __func__, mtdegfree );
                if(mtdegfree==5)fprintf(stdout, "%s: %s: %s: mtdegfree=%d DEVIATORIC\n", progname, __FILE__, __func__, mtdegfree );
                if(mtdegfree==6)fprintf(stdout, "%s: %s: %s: mtdegfree=%d FULL_MT\n", progname, __FILE__, __func__, mtdegfree );
	}

	if(mtdegfree==1)fprintf(stderr, "%s: %s: %s: mtdegfree=%d EXPLOSION\n", progname, __FILE__, __func__, mtdegfree );
	if(mtdegfree==5)fprintf(stderr, "%s: %s: %s: mtdegfree=%d DEVIATORIC\n", progname, __FILE__, __func__, mtdegfree );
	if(mtdegfree==6)fprintf(stderr, "%s: %s: %s: mtdegfree=%d FULL_MT\n", progname, __FILE__, __func__, mtdegfree );

	if( Distance_Normalize )
	{
		if(verbose)
		fprintf( stdout, "%s: %s: %s: Distance_Normalize is ON R0=%g km\n", progname, __FILE__, __func__, DistNormR0 );
		fprintf( stderr, "%s: %s: %s: Distance_Normalize is ON R0=%g km\n", progname, __FILE__, __func__, DistNormR0 );
	}
	else
	{
		if(verbose)
                fprintf( stdout, "%s: %s: %s: Distance_Normalize is OFF\n", progname, __FILE__, __func__ );
                fprintf( stderr, "%s: %s: %s: Distance_Normalize is OFF\n", progname, __FILE__, __func__ );
	}

	if(ishift)
	{
		if(verbose)
		{
		  fprintf( stdout, 
		    "%s: %s: %s: Shiftng by max cross-correlation is ON cortol=%g maxtimeshift=%g\n", 
			progname, 
			__FILE__, __func__, cortol, maxtimeshift );
		}
		fprintf( stderr, 
		  "%s: %s: %s: Shiftng by max cross-correlation is ON cortol=%g maxtimeshift=%g\n", 
			progname, __FILE__, __func__, cortol, maxtimeshift );
	}
	else
	{
		if(verbose)
                fprintf( stdout, "%s: %s: %s: Shiftng by max cross-correlation is OFF\n", progname, __FILE__, __func__ );
                fprintf( stderr, "%s: %s: %s: Shiftng by max cross-correlation is OFF\n", progname, __FILE__, __func__ );
	}

	if( FixMyZ != -99 )
	{
		if(verbose)
		fprintf( stdout, "%s: %s: %s: Fix Depth is ON fixz=%g\n", progname, __FILE__, __func__, FixMyZ );
		fprintf( stderr, "%s: %s: %s: Fix Depth is ON fixz=%g\n", progname, __FILE__, __func__, FixMyZ );
	}
	else
	{
		if(verbose)
                fprintf( stdout, "%s: %s: %s: Fix Depth is OFF\n", progname, __FILE__, __func__ );
                fprintf( stderr, "%s: %s: %s: Fix Depth is OFF\n", progname, __FILE__, __func__ );
	}

/******************************************/
/*** allocate memory for parameter list ***/
/******************************************/
	if( verbose ) 
	{
		fprintf( stdout, "%s: allocating memory for data and event information\n", 
			progname );
	}

	ev  = (EventInfo *)malloc(sizeof(EventInfo));
	ev  = (EventInfo *)glib2inv_get_input_parameters( evinfo_filename, ev, &nsta, verbose );

	fprintf( stdout, "%s: %s: %s: ev[ista=0] evdp=%g my_z=%g str=%g dip=%g rak=%g Mw=%g\n",
		progname, __FILE__, __func__,
		ev[0].evdp,
		ev[0].my_z,
		ev[0].str,
		ev[0].dip,
		ev[0].rak,
		ev[0].Mw );

/********************************************/
/*** loop over stations and load the data ***/
/********************************************/
	if( verbose ) 
	{
		fprintf( stdout, "%s: mtinv(): reading data nsta=%d\n", progname, nsta );
	}

	fprintf( stderr, "%s: %s: %s: reading data nsta=%d\n", progname, __FILE__, __func__, nsta );

	load_the_data( ev, nsta, ts0, verbose );

	if(debug)
	{
	  for( ista = 0 ; ista < nsta ; ista++ )
          {
		  fprintf( stdout, "%s: %s: %s: STDOUT: ista=%d nt=%d %d %d dt=%g %g %g\n", 
			progname, 
			__FILE__, __func__,
			ista, 
			ev[ista].ew.s.npts, 
			ev[ista].ns.s.npts, 
			ev[ista].z.s.npts,
			ev[ista].ew.s.delta,
			ev[ista].ns.s.delta,
			ev[ista].z.s.delta );

/*** debug ***/
/****
	fprintf( stdout, "----------- DEBUG ista = %d sta = %s NS ------------------\n", ista, ev[ista].ns.s.kstnm );
		for( it = 0; it < ev[ista].ns.s.npts; it++ )
		{
			if( fabs( ev[ista].ns.data[it] ) > 1.0E-04 ) 
			  fprintf( stdout, "ista=%d it=%d ev[ista].ns.data[it]=%g\n",
				ista, it, ev[ista].ns.data[it] );
		}
	fprintf( stdout, "----------- DEBUG ista = %d sta = %s EW ------------------\n", ista, ev[ista].ew.s.kstnm );
		for( it = 0; it < ev[ista].ew.s.npts; it++ )
		{
			if( fabs( ev[ista].ew.data[it] ) > 1.0E-04 )
			  fprintf( stdout, "ista=%d it=%d ev[ista].ew.data[it]=%g\n",
				ista, it, ev[ista].ew.data[it] );
		}
	fprintf( stdout, "---------------------------------------------------------------\n" );
****/
		writesacfile( &ev[ista] );
	  }

        } /*** DEBUG ***/

/******************************/
/*** load green's functions ***/
/******************************/

	if(verbose)
	{
		fprintf( stdout, "%s: %s: %s: allocating memory for Green's function specialLoadGrnMxy=%d\n",
			progname, __FILE__, __func__, specialLoadGrnMxy );
	}

	if(specialLoadGrnMxy)
	{
	  nz = 1;
	  grn = (Greens **)malloc( nsta*sizeof(Greens *) );

	/*** previous version forgot to allocate memory for DepthVector:zvec ***/

	  zvec = (DepthVector *) calloc( nsta, sizeof(DepthVector) );

	/**** this loads from SAC files and is limited to just 1 depth ***/

	  z = (float *)load_special_grns( ev, grn, nsta, &nz, verbose );

	/*** print out to debug and demostration of load_special_grns return vals ***/
	  for( iz = 0; iz < nz; iz++ ) 
	  {
		fprintf( stdout, "%s: %s: %s: specialLoadGrnMxy = %d z[iz=%d]=%g\n", 
			progname, __FILE__, __func__, specialLoadGrnMxy, iz, z[iz] );
	  }

	  for( ista = 0; ista < nsta; ista++ )
	  {
		zvec[ista].nz = 1;
		zvec[ista].ista = ista;
		zvec[ista].z = (float *) calloc(zvec[ista].nz, sizeof(float));
		zvec[ista].z = z;
	  }
	}
	else
	{
	 zvec = (DepthVector *) malloc( nsta * sizeof(DepthVector) );

	 grn = (Greens **)malloc( nsta*sizeof(Greens *) );

	 /* z = (float *)load_greens( ev, grn, nsta, &nz, verbose ); */

	 grn = loadGlibAll( grn, ev, zvec, nsta, "ginv", verbose );

	 z = zvec[0].z;
	 nz = zvec[0].nz;
	}

/**************************************/
/*** check if fixing solution depth ***/
/*** iz_best get reset below        ***/
/**************************************/

	if( FixMyZ != -99 )
	{
	   check_depth( FixMyZ, &FixMyiz, z, nz, verbose );
	}

/***********************************************************************************/
/*** turn off stations that have all three channels/components with SNR < MINSNR ***/
/*** turn off only if iused was already turned off/ do not override cases where  ***/
/*** iused == 0 or iused == -1                                                   ***/
/***********************************************************************************/

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: STDOUT: iuse_snr = %d minsnr = %g\n", 
		progname, __FILE__, __func__, iuse_snr, MINSNR );
	}

	if( iuse_snr )
	{
	  for( ista=0; ista<nsta; ista++ )
	  {
		if( ( ev[ista].z.P2P_snr  < MINSNR) && 
		    ( ev[ista].ns.P2P_snr < MINSNR) && 
		    ( ev[ista].ew.P2P_snr < MINSNR) &&
		    ( ev[ista].iused == 1 ) )
		{
			ev[ista].iused = 0;
		}
		/*** else iused < 1, leave alone ***/
	  }
	}

/*****************************************************/
/*** Error check if any any stations are turned on ***/
/*****************************************************/

	check_on_status_ok = 0;
	for( ista=0; ista<nsta; ista++ )
	{
		if( ev[ista].iused == 1 ) check_on_status_ok=1;
	}

	if(verbose)
	fprintf( stdout, "%s: check_on_status_ok=%d\n", progname, check_on_status_ok );

	if( !check_on_status_ok )
	{
		fprintf( stdout, 
		  "%s: %s: %s: STDOUT: ERROR! no stations turned on in the par file %s\n",
			progname, __FILE__, __func__, evinfo_filename );

		fprintf( stderr,
		  "%s: %s: %s: STDERR: ERROR! no stations turned on in the par file %s\n",
                        progname, __FILE__, __func__, evinfo_filename );
		exit(-1);
	}

/*********************************/
/*** set the type of inversion ***/
/*********************************/

	sol = (Solution *)malloc(nz*sizeof(Solution));
	for( iz=0; iz<nz; iz++ )
	{
		if( mtdegfree == 1 )  sol[iz].mt_type = EXPLOSION;
		if( mtdegfree == 5 )  sol[iz].mt_type = DEVIATORIC;
		if( mtdegfree == 6 )  sol[iz].mt_type = FULL_MT;
	}

/****************************************************************/
/*** check to see if this is a forward or inverse calculation ***/
/****************************************************************/

	if( forward )
	{
		if( verbose )
			fprintf( stdout, "%s: calling forward calc mtdegfree=%d\n",
			  progname, mtdegfree );

		forward_calc(	ev,
				grn,
				nsta,
				nz,
				z,
				sol,
				verbose,
				idump,
				mtdegfree,
				Distance_Normalize,
				DistNormR0 );

		/*****************************/
		/*** what is the best fit  ***/
		/*****************************/
		iz_best = 0;
		bestfit = sol[0].total_fitness1;
		for( iz=0; iz<nz; iz++ )
		{
			if( ev[0].evdp == z[iz] )
			{
				if(verbose)
				{  
				  fprintf(stdout, "%s: mtinv.c: best iz=%d z=evdp=%g\n", 
					progname, iz, z[iz] );
				}
				iz_best = iz;
				bestfit = sol[iz].total_fitness1;
			}
		}
		if( FixMyZ != -99 ) iz_best = FixMyiz;
	}
	else /*** do inversion ***/
	{
		if( verbose ) fprintf( stdout, "%s: %s: %s: calling invert()\n",
			progname, __FILE__, __func__ );

		fprintf( stderr, "%s: %s: %s: calling invert()\n",
                        progname, __FILE__, __func__ );

	/****************************/
	/*** calling inversion    ***/
	/****************************/

		invert(	ev,
			grn,
			nsta,
			nz,
			sol,
			verbose,
			idump, 
			mtdegfree,
			Distance_Normalize, 
			DistNormR0, 
			specialLoadGrnMxy,
			dev_mt_largest_vr );
		
	/****************************/
	/*** what is the best fit ***/
	/****************************/

		iz_best = 0;
		bestfit = sol[0].total_fitness1;
		for( iz=0; iz<nz; iz++ )
		{
		  if(verbose)
		  {
		    fprintf( stdout, "%s: %s: %s: iz=%d z=%g ot=%g ts0=%g fit1[iz]=%g vred=%g ",
			progname, 
			__FILE__,
			__func__,
			iz, z[iz], ev[0].ot_shift, ev[0].ts0, 
			sol[iz].total_fitness1, sol[iz].var_red );

		    fprintf( stdout, "pdc=%g iz_best=%d fit1[iz_best]=%g fit2[iz]=%g fit2[best]=%g\n", 
			sol[iz].PDC, iz_best, sol[iz_best].total_fitness1,
			sol[iz].total_fitness2, sol[iz_best].total_fitness2 );
		    fflush(stdout);
		  }

	/**** only print the ts0 +/-1 sec from ts=0 ***/
		  if( fabs( ev[0].ts0 ) <= 1 ) 
		  {
		    fprintf( stderr, "%s: %s: %s: iz=%d z=%g ot=%g ts0=%g fit1[iz]=%g vred=%g ",
                        progname,
			__FILE__,
                        __func__,
                        iz, z[iz], ev[0].ot_shift, ev[0].ts0,
                        sol[iz].total_fitness1, sol[iz].var_red );

		    fprintf( stderr, "pdc=%g iz_best=%d fit1[iz_best]=%g fit2[iz]=%g fit2[best]=%g\n",
                        sol[iz].PDC, iz_best, sol[iz_best].total_fitness1,
                        sol[iz].total_fitness2, sol[iz_best].total_fitness2 );
		  }

		  if( sol[iz].total_fitness1 > bestfit ) 
		  {
			bestfit = sol[iz].total_fitness1;
			iz_best = iz;
		  }

		} /*** loop over each depth solution ***/

		if( FixMyZ != -99 ) iz_best = FixMyiz;

		if(verbose)
		{
		  fprintf( stdout, "%s: %s: %s: fixz=%.2f iz_best=%d z=%g ot=%g ts0=%g VR=%.2f pdc=%.0f fit1=%.2f fit2=%.2f\n",
			progname, 
			__FILE__,
			__func__,
			FixMyZ,
                        iz_best, 
			z[iz_best], 
			ev[0].ot_shift, 
			ev[0].ts0,
			sol[iz_best].var_red, 
                        sol[iz_best].PDC, 
			sol[iz_best].total_fitness1,
			sol[iz_best].total_fitness2 );
                   fflush(stdout);
		}

		fprintf( stderr, "%s: %s: %s: fixz=%.2f iz_best=%d z=%g ot=%g ts0=%g VR=%.2f pdc=%.0f fit1=%.2f fit2=%.2f\n",
                        progname,
			__FILE__,
			__func__,
			FixMyZ,
                        iz_best,
                        z[iz_best],
                        ev[0].ot_shift,
                        ev[0].ts0,
                        sol[iz_best].var_red,
                        sol[iz_best].PDC,
                        sol[iz_best].total_fitness1,
                        sol[iz_best].total_fitness2 );
		fflush(stderr);

	} /*** end inversion or forward option ***/
	
/******************************************************************/
/*** write out a gmt shell script for each time shift and depth ***/
/******************************************************************/

	mechanism_size = 0.4;

	if( igmt5 )
	{
		if(verbose)
		{
			fprintf( stdout, "%s: mtinv(): calling plotmech_gmt5():\n", progname );
		}
		plotmech_gmt5( iz_best, sol, ev, mechanism_size );
	}
	else
	{
		if(verbose)
                {
                        fprintf( stdout, "%s: mtinv(): calling plotmech_gmt4():\n", progname );
                }
                plotmech_gmt4( iz_best, sol, ev, mechanism_size );
	}

/********************************************/
/*** make an ascii plot for fast email    ***/
/********************************************/

	if(verbose)
	  fprintf( stdout, "%s: mtinv(): calling calc_azi_gap_and_dmin():\n", progname );

	calc_azi_gap_and_dmin( iz_best, nsta, ev, sol, grn );

/***************************************************************************/
/*** write out MT solution results to a text file format for email       ***/
/***************************************************************************/

	if( write_emails )
	{
		sprintf( asc_file_name, "email_T%05.1fsec_Z%05.1fkm_.txt", 
			sol[iz_best].ot, grn[0][iz_best].evdp );
		if(verbose)
		{
		  fprintf(stdout, "%s: mtinv.c: Writing Email Messages to %s\n", 
			progname, asc_file_name );
		}
	
		if( (fpasc=fopen( asc_file_name, "w" ) ) == NULL )
		{
		  fprintf(stdout, "cannot open file for writting\n");
		}

		if(verbose)
		{
	  	  fprintf( stdout, "%s: %s: %s: calling write_email_message():\n",
			progname, __FILE__, __func__ );
		}

		write_email_message( fpasc, nsta, iz_best, sol, ev, grn, forward );
		fclose(fpasc);
	}

/***************************************************************************/
/*** write out an ascii text file of the MT results                      ***/
/*** this output file is used by GMT C-shell script for plotting results ***/
/***************************************************************************/

	if(verbose)
	{
		fprintf( stdout, "%s: mtinv.c: bestfit=%g iz=%d\n", 
			progname, bestfit, iz_best );	
	}
	sprintf( asc_file_name, "results.%d.out", mtdegfree );

	if( (fpasc=fopen( asc_file_name, "a" ) ) == NULL )
	{
		fprintf(stdout, "cannot open file for writting\n");
	}

	if(verbose)
	{
		fprintf( stdout, "%s: %s: %s: calling write_sol_rec():\n",
			progname, __FILE__, __func__ );
	}

	write_sol_rec( fpasc, iz_best, nsta, ev, sol, grn );
	fclose(fpasc);

/*************************************/
/*** write out a gmt shell script ***/
/*************************************/

	if(igmt5)
	{
		if(verbose) 
		{
		  fprintf( stdout, "%s: %s: %s: calling plot_z_vs_fit_gmt5(): \n", 
			progname, __FILE__, __func__ );
		}
		plot_z_vs_fit_gmt5( iz_best, z, sol, ev );
	}
	else
	{
		if(verbose)
                {
                  fprintf( stdout, "%s: %s: %s: calling plot_z_vs_fit_gmt4(): \n",
                        progname, __FILE__, __func__ );
                }
                plot_z_vs_fit_gmt4( iz_best, z, sol, ev );
	}

/****************************************************************************************************/
/*** compute synthetics for calculation of cross correlation and PSPLOT graphics and GMT plotting ***/
/****************************************************************************************************/

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: recalculate the synthetics for iz_best=%d\n",
		progname, __FILE__, __func__, iz_best );
	}

	for( ista=0; ista<nsta; ista++ )
	{
		fprintf( stdout, "%s: %s: %s: allocating memory for synthetics ista=%d ev.nt=%d grn.nt=%d\n",
			progname, __FILE__, __func__, ista, ev[ista].nt, grn[ista][0].nt );
		fflush( stdout );

		ev[ista].syn_r.data = calloc( ev[ista].nt, sizeof(float) );
		ev[ista].syn_z.data = calloc( ev[ista].nt, sizeof(float) );
		ev[ista].syn_t.data = calloc( ev[ista].nt, sizeof(float) );

		if(verbose)
		{
		  if(specialLoadGrnMxy)
		    fprintf( stdout, 
		      "%s: %s: %s: calling compute_synthetics_special() specialLoadGrnMxy=%d iz_best=%d ista=%d nt=%d dt=%g\n",
			progname, 
			__FILE__,
			__func__,
			specialLoadGrnMxy,
			iz_best, ista, ev[ista].nt, ev[ista].dt );
		  else
		    fprintf( stdout,
		      "%s: %s: %s: calling compute_synthetics()  iz_best=%d ista=%d nt=%d dt=%g mtdegfree=%d wavetype=%s\n",
			progname,
                        __FILE__,
                        __func__,
                        iz_best, ista, ev[ista].nt, ev[ista].dt, mtdegfree, ev[ista].wavetype );
		}

		if(specialLoadGrnMxy)
		   compute_synthetics_special( ista, iz_best, ev, grn, sol, mtdegfree );
		 else
		   compute_synthetics( ista, iz_best, ev, grn, sol, mtdegfree );
	}

/***************************************************************************************/
/*** from here call computeStationChannel_VarianceReduction version 2.  Version 1    ***/
/*** works with s_vector which is only available after invert() call from main       ***/
/*** and EventInfo ev structure is overwritten at each depth so useless              ***/
/*** create new version 2 which can be called after call to invert() and             ***/
/*** compute_synthetics()  to do the data and syn from the best solution iz=iz_best  ***/
/****************************************************************************************/

	if(compute_station_vred)
	{
	  if(verbose)
	  {
            fprintf( stdout, 
            "%s: %s: %s: calling computeStationChannel_VarianceReduction_v2(): iz_best=%d\n",
                progname,
                __FILE__,
                __func__,
                iz_best );
	  }

	  computeStationChannel_VarianceReduction_v2( nsta, iz_best, ev, grn, sol, verbose );
	}

/*******************************************************************************************/
/*** for each station do a cross correlation to find the lag time and correlation values ***/
/*******************************************************************************************/

	if(verbose)
	{
	  fprintf( stdout, 
	    "%s: %s: %s: calling cross correlation for iz_best=%d cortol=%f maxshift=%f ishift=%d\n", 
		progname, __FILE__, __func__,
		iz_best, cortol, maxtimeshift, ishift );

	  fflush(stdout);
	}

/*
	fprintf( stderr,
            "%s: %s: %s: calling cross correlation for iz_best=%d cortol=%f maxshift=%f ishift=%d\n",
                progname, __FILE__, __func__,
                iz_best, cortol, maxtimeshift, ishift );
        fflush(stderr);
*/

	for( ista=0; ista<nsta; ista++ )
	{
		/*
		fprintf( stderr, "%s: %s: %s: calling xcorr() ista=%d \n", progname, __FILE__, __func__, ista );
		fflush(stderr);
		*/

		/*** force ishift = 0 ***/
		/*
		xcorr( &(ev[ista].z.data[0]), &(ev[ista].syn_z.data[0]), ev[ista].nt, ev[ista].dt, 
			&(ev[ista].izlag), &(ev[ista].ztlag), &(ev[ista].zxcor), cortol, verbose, 0 );

		xcorr( &(ev[ista].ns.data[0]), &(ev[ista].syn_r.data[0]), ev[ista].nt, ev[ista].dt,
                        &(ev[ista].irlag), &(ev[ista].rtlag), &(ev[ista].rxcor), cortol, verbose, 0 );

		xcorr( &(ev[ista].ew.data[0]), &(ev[ista].syn_t.data[0]), ev[ista].nt, ev[ista].dt,
                        &(ev[ista].itlag), &(ev[ista].ttlag), &(ev[ista].txcor), cortol, verbose, 0 );
		*/

		xcorr( &(ev[ista].z.data_safe[0]), &(ev[ista].syn_z.data[0]), ev[ista].nt, ev[ista].dt,
                        &(ev[ista].izlag), &(ev[ista].ztlag), &(ev[ista].zxcor), cortol, verbose, 0 );

                xcorr( &(ev[ista].ns.data_safe[0]), &(ev[ista].syn_r.data[0]), ev[ista].nt, ev[ista].dt,
                        &(ev[ista].irlag), &(ev[ista].rtlag), &(ev[ista].rxcor), cortol, verbose, 0 );

                xcorr( &(ev[ista].ew.data_safe[0]), &(ev[ista].syn_t.data[0]), ev[ista].nt, ev[ista].dt,
                        &(ev[ista].itlag), &(ev[ista].ttlag), &(ev[ista].txcor), cortol, verbose, 0 );

	/*** based on max cross correlation shift all data by same amount ***/
	
		if(ishift)
		{
		/*
		  fprintf( stderr, "%s: %s: %s: calling find_best_shift() ista=%d \n", progname, __FILE__, __func__, ista );
		  fflush(stderr);
		*/
		  find_best_shift( &(ev[ista]), cortol, maxtimeshift, &(ev[ista].time_shift_all) );
		}

		if(verbose)
		{
		  fprintf( stdout, "%s: %s: %s: %s.%s.%s: otshift=%g stashift=%g\n",
			progname, __FILE__, __func__,
			ev[ista].ew.s.kstnm, 
			ev[ista].ew.s.knetwk,
			ev[ista].ew.s.kcmpnm, 
			ev[ista].ts0,
			ev[ista].time_shift_all );

		  fprintf( stdout, "%s: %s: %s:\t izlag=%3d ztlag=%5.2f zxcor=%.2f\n",
			progname, __FILE__, __func__,
			ev[ista].izlag,
			ev[ista].ztlag,
			ev[ista].zxcor ); 
		
		  fprintf( stdout, "%s: %s: %s:\t irlag=%3d rtlag=%5.2f rxcor=%.2f\n", 
			progname, __FILE__, __func__,
                        ev[ista].irlag,
			ev[ista].rtlag, 
			ev[ista].rxcor );

		  fprintf( stdout, "%s: %s: %s: \t itlag=%3d ttlag=%5.2f txcor=%.2f\n", 
			progname, __FILE__, __func__,
                        ev[ista].itlag,
			ev[ista].ttlag,
			ev[ista].txcor );
		}

	} /*** loop over stations and cross correlate data/syn ***/

/*** do the time shift, and cross correlate again to update the plot ***/

	if(ishift)
	{
		/*
		fprintf( stderr, "%s: %s: %s calling time_shift()\n", progname, __FILE__, __func__ );
		fflush(stderr);
		*/

		if(verbose)
		{
		  fprintf( stdout, "%s: mtinv(): calling time_shift()\n", progname );
		}
		
		time_shift( ev, nsta, verbose );

		for( ista=0; ista<nsta; ista++ )
		{
		/* 
		 fprintf( stderr, "%s: %s: %s: calling xcorr() ista=%d \n", progname, __FILE__, __func__, ista );
                 fflush(stderr);
		*/

		 /*** force ishift = 0 ***/
		/*
                 xcorr( &(ev[ista].z.data[0]), &(ev[ista].syn_z.data[0]), ev[ista].nt, ev[ista].dt,
                        &(ev[ista].izlag), &(ev[ista].ztlag), &(ev[ista].zxcor), cortol, verbose, 0 );

                 xcorr( &(ev[ista].ns.data[0]), &(ev[ista].syn_r.data[0]), ev[ista].nt, ev[ista].dt,
                        &(ev[ista].irlag), &(ev[ista].rtlag), &(ev[ista].rxcor), cortol, verbose, 0 );

                 xcorr( &(ev[ista].ew.data[0]), &(ev[ista].syn_t.data[0]), ev[ista].nt, ev[ista].dt,
                        &(ev[ista].itlag), &(ev[ista].ttlag), &(ev[ista].txcor), cortol, verbose, 0 );
		*/

		 xcorr( &(ev[ista].z.data_safe[0]), &(ev[ista].syn_z.data[0]), ev[ista].nt, ev[ista].dt,
                        &(ev[ista].izlag), &(ev[ista].ztlag), &(ev[ista].zxcor), cortol, verbose, 0 );

                 xcorr( &(ev[ista].ns.data_safe[0]), &(ev[ista].syn_r.data[0]), ev[ista].nt, ev[ista].dt,
                        &(ev[ista].irlag), &(ev[ista].rtlag), &(ev[ista].rxcor), cortol, verbose, 0 );

                 xcorr( &(ev[ista].ew.data_safe[0]), &(ev[ista].syn_t.data[0]), ev[ista].nt, ev[ista].dt,
                        &(ev[ista].itlag), &(ev[ista].ttlag), &(ev[ista].txcor), cortol, verbose, 0 );
		}
	}

/*************************************/
/*** Cgraphics PS library routines ***/
/*************************************/

	sprintf( ps_plot_filename, "plot_T%05.1fsec_Z%05.1fkm_", 
		sol[iz_best].ot, grn[0][iz_best].evdp );

	fprintf( stderr, "%s: %s: %s: Plotting Postscript Plot %s\n",
                        progname, __FILE__, __func__, ps_plot_filename );
	fprintf( stderr, "%s: %s: %s: calling psplot()\n", progname, __FILE__, __func__ );
	fflush(stderr);

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: Plotting Postscript Plot %s\n",
                        progname, __FILE__, __func__, ps_plot_filename );
          fprintf( stdout, "%s: %s: %s: calling psplot()\n", progname, __FILE__, __func__ );
          fflush(stdout);
	}

	/*** psplot.c ***/
	npages = psplot( 
		nsta,
		iz_best,
		ps_plot_filename,
		ev,
		sol,
		grn,
		0,
		verbose,
		forward,
		PltXcorLabel,
		LANDSCAPE );

/*****************************************/
/*** write out results for mtbestfit.c ***/
/*** and for automt                    ***/
/*****************************************/

	fprintf( stderr, "%s: %s: %s: calling write_results2() to file automt.txt\n",
		progname, __FILE__, __func__ );
	fflush(stderr);

	if(verbose)
	{
		fprintf( stdout, "%s: %s: %s: calling write_results2() to file automt.txt\n",
			progname, __FILE__, __func__ );
		fflush(stdout);
	}

	write_results2(
		"automt.txt",
		sol,
		ev,
		grn,
		iz_best,
		nsta,
		npages,
		AutoAuth,
		ishift,
		cortol,
		maxtimeshift,
		iuse_snr,
                MINSNR,
		igmt5,
		sqlite3_db_write,
		mysql_db_write,
		oracle_db_write,
		FixMyZ );	

/**********************/
/*** make a GMT map ***/
/**********************/

	fprintf(stderr, "%s: %s: %s: making a GMT map for plotting\n", progname, __FILE__, __func__ );
	fflush(stderr);

	if( igmtmap )
	{
		if(verbose)
		{
		   fprintf(stdout, "%s: %s: %s: making a GMT map for plotting\n", 
			progname, __FILE__, __func__ );
		   fflush(stdout);
		}

	  if( igmt5 )
	  {
	  	if(verbose)
	  	{
			fprintf(stdout, 
			  "%s: %s: %s:  calling make_map_gmt5(), making a GMT map for plotting\n",  
				progname, __FILE__, __func__ );
			fflush(stdout);
		}
		make_map_gmt5( iz_best, nsta, ev, sol, grn, verbose );
	  }
	  else
	  {
		if(verbose)
                {
                        fprintf(stdout,
                          "%s: %s: %s: calling make_map_gmt4(), making a GMT map for plotting\n",
                                progname, __FILE__, __func__ );
			fflush(stdout);
                }
                make_map_gmt4( iz_best, nsta, ev, sol, grn, verbose );
	  }
	}

/*********************************************/
/*** plot origin time depth fit trade offs ***/
/*********************************************/

	fprintf(stderr, "%s: %s: %s: plot origin time depth fit trade offs\n", progname, __FILE__, __func__ );
        fflush(stderr);

	if(verbose)
	{
		fprintf(stdout, "%s: %s: %s: plot origin time depth fit trade offs\n", 
			progname, __FILE__, __func__ );
        	fflush(stdout);
	}

	if( igmt5 )
	{
		if(verbose)
		{
		  fprintf(stdout, "%s: %s: %s: calling plot_results_gmt5()\n", 
			progname, __FILE__, __func__ );
		  fflush(stdout);
		}
		plot_results_gmt5( iz_best, sol, ev );
	}
	else
	{
		if(verbose)
		{
		  fprintf(stdout, "%s: %s: %s: calling plot_results_gmt4()\n",
                        progname, __FILE__, __func__ );
		  fflush(stdout);
		}
		plot_results_gmt4( iz_best, sol, ev );
	}

/**************************************************************************/
/*** dump out the data and synthetics as ascii text files for GMT plots ***/
/**************************************************************************/

	if( idumpxy )
	{
		if(verbose)
		{
                  fprintf(stdout, "%s: %s: %s: calling dumpxy()\n", 
			progname, __FILE__, __func__ );
		  fflush(stdout);
		}

		dumpxy( ev, sol, grn, nsta, iz_best, verbose );

		if( igmt5 )
		{
		  if(verbose)
		  {
			fprintf(stdout, "%s: %s: %s: calling wfplot2_gmt5( GMT 5.x.x and 6.x.x Versions )\n", 
				progname, __FILE__, __func__ );
			fflush(stdout);
		  }
		  wfplot2_gmt5( ev, sol, grn, nsta, iz_best, verbose, sort_by_value, print_gmtwf_mt_info );
		}
		else
		{
		  if(verbose)
		  {
		    fprintf(stdout, "%s: %s: %s: calling wfplot2_gmt4t( GMT 4.5.x version )\n", 
			progname, __FILE__, __func__ );
		    fflush(stdout);
		  }
		  wfplot2_gmt4( ev, sol, grn, nsta, iz_best, verbose, sort_by_value, print_gmtwf_mt_info );
		}
	}

	if( sqlite3_db_write )
	{
		if(verbose)
		{
		  fprintf(stdout, "%s: %s: %s: calling db_sqlite3_create()\n", 
			progname, __FILE__, __func__ );
		  fflush(stdout);
		}

		db_sqlite3_create();

		if(verbose)
		{
		  fprintf(stdout, "%s: %s: %s: calling db_sqlite3_write()\n", 
			progname, __FILE__, __func__ );
		  fflush(stdout);
		}

		db_sqlite3_write(  ev, sol, grn, nsta, iz_best, AutoAuth, verbose );

		if(verbose)
		{
		  fprintf( stdout, "%s: %s: %s: calling llnl_db_write\n", 
			progname, __FILE__, __func__ );
		  fflush(stdout);
		}
		llnl_db_write( ev, sol, grn, nsta, iz_best, AutoAuth, verbose );
	}

	if( mysql_db_write )
	{
		if(verbose)
		  fprintf(stdout, "%s: mtinv.c: mtinv(): calling mysql_db_create()\n", progname );

		mysql_db_create();

		if(verbose)
                  fprintf(stdout, "%s: mtinv.c: mtinv(): calling db_write()\n", progname );

		db_mysql_write(  ev, sol, grn, nsta, iz_best, AutoAuth, verbose );
	}

	if( oracle_db_write )
	{
		if(verbose)
                  fprintf(stdout, "%s: mtinv.c: mtinv(): calling oracle_db_create()\n", progname );

		oracle_db_create();

		if(verbose)
                  fprintf(stdout, "%s: mtinv.c: mtinv(): calling db_oracle_write()\n", progname );

		db_oracle_write( ev, sol, grn, nsta, iz_best, AutoAuth, verbose );
	}

/*****************************/
/*** unallocate the memory ***/
/*****************************/
/****
	if(verbose)
	{
	  fprintf(stdout, 
	    "%s: %s: %s: Trying to free memory...", 
		progname, __FILE__, __func__ );
	  fflush(stdout);
	}

	free(sol);
	free(z);

	for( ista=0; ista<nsta; ista++ )
	{
		free( ev[ista].ew.data );
		free( ev[ista].ns.data );
		free( ev[ista].z.data );

		free( ev[ista].syn_t.data );
		free( ev[ista].syn_r.data );
		free( ev[ista].syn_z.data );
	}
	free(ev);
	free(grn);

	if(verbose) 
	{
		fprintf(stdout, " Done.\n" );
		fflush(stdout);
	}
***/

	if(verbose)
	fprintf(stdout, "%s: %s: %s: STDOUT: Program finished.  Bye-Bye!\n\n", progname, __FILE__, __func__ );
	fflush(stdout);
        fprintf(stderr, "%s: %s: %s: STDERR: Program finished.  Bye-Bye!\n\n", progname, __FILE__, __func__ );
	fflush(stderr);

	exit(0);

} /*** end of mtinv.c ***/


/***************************/
/*** forward calculation ***/
/***************************/

void forward_calc( EventInfo *ev,
		Greens **grn,
		int nsta,
		int nz, float *z, 
		Solution *sol,
		int verbose,
		int idump,
		int mtdegfree, 
		int Distance_Normalize,
		float DistNormR0 )
{
	int i, j, rows, cols;

/* see include/mt.h Mo = math.pow( 10.0, 1.5*(Mw+10.73) ) = 1.2445146117713818e+16; Reference Mw = 0.0 */

	int ista, iz, myiz;
	int notfound = 1;
	float **mt;
	MomentTensor Ma, Mn;
	
	float **a_matrix;
	float *s_vector;
	float *x_vector;
	float *b_vector;
	
	float **matrix( int start_row, int stop_row, int start_col, int stop_col );
	float *vector( int start, int end );

	float variance_reduction( float *data, float *synthetic, int nstart, int nend );
	float compute_l2norm_error( float *data, float *synthetic, int npts );

	void set_moment_tensor( MomentTensor *Ma, float *x_vector, int degfree, int verbose );
	void normalize_moment_tensor( MomentTensor *Ma, MomentTensor *Mn, float Mo, int );
	void sdr_to_mt( float **mt, float str, float dip, float rak, float Mw, int verbose );

	void mt2eig( MomentTensor Ma, Solution *sol, int iz, int verbose );
	void eig2iso( Solution *sol, int iz, int verbose );
	void Eig2MajorDC( Solution *sol, int iz, int verbose );
	void Eig2MinorDC( Solution *sol, int iz, int verbose );
	void eig2lune_4mtinv( Solution *sol, int iz, int verbose );

	void make_amatrix(
        	EventInfo *ev,
        	Greens **grn, 
        	int nsta,
        	int iz,
        	float **a_matrix,
        	float *b_vector,
        	int mtdegfree,
        	int Distance_Normalize,
        	float DistNormR0 );

	void matmul( int conj, float **bb, int nx, float *x, int ny, float *y );

	void grn2disp( Greens *g, EventInfo *ev, int verbose, int mtdegfree );

/************************************************************************************************/
/*** initalize memory ***/
/************************************************************************************************/
	fprintf(stdout, "%s: %s: %s: allocating memory for mt[][] and x_vector[]\n",
		progname, __FILE__, __func__ );

	cols = 6;
	mt = matrix( 0, cols+1, 0, cols+1 );
	x_vector = vector( 0, cols+1 );

/***********************************************************************/
/*** if "EV" event tag present in input file then compute synthetics ***/
/***********************************************************************/

	for( ista=0; ista<nsta; ista++ )
	{
		if(verbose)
		{
		  fprintf( stdout, "%s: %s: %s: ista=%d %s.%s my_z=%g evdp=%g str=%g dip=%g rak=%g Mw=%g mtdegfree=%d Mij=(%g,%g,%g,%g,%g,%g) Mo=%g\n",
			progname,
			__FILE__,
			__func__,
			ista,
			ev[ista].stnm,
			ev[ista].net,
			ev[ista].my_z,
			ev[ista].evdp,
			ev[ista].str,
			ev[ista].dip,
			ev[ista].rak,
			ev[ista].Mw,
			mtdegfree,
			ev[ista].Mxx, ev[ista].Mxy, ev[ista].Mxz, 
			ev[ista].Myy, ev[ista].Myz, ev[ista].Mzz, ev[ista].Mo );
		}

	/*** unknown why I do this? Perhaps skip ***/
		notfound = 1;
		for( iz=0; iz<nz; iz++ )
		{
			if( ev[ista].my_z == z[iz] && ev[ista].evdp == z[iz] )
			{
			  if(verbose)
			  {
			    fprintf( stderr, "%s: %s: %s: calling grn2disp\n",
				progname, __FILE__, __func__ );
			  }

			  notfound = 0;
			  grn[ista][iz].ver = calloc( grn[ista][iz].nt, sizeof(float) );
                          grn[ista][iz].rad = calloc( grn[ista][iz].nt, sizeof(float) );
                          grn[ista][iz].tra = calloc( grn[ista][iz].nt, sizeof(float) );
			  grn2disp( &(grn[ista][iz]), &ev[ista], verbose, mtdegfree );
			  myiz = iz;
			}
		}

		if(notfound)
		{
		  fprintf( stderr, "%s: %s: %s: ista=%d matching depth not found for station %s.%s ... looking for my_z=%g ",
			progname, 
			__FILE__,
			__func__,
			ista, 
			ev[ista].stnm,
			ev[ista].net,
			ev[ista].my_z );

		  for( iz=0; iz<nz; iz++ )
		  {
			fprintf( stderr, "\t iz=%d z=%18.7f\n", iz, z[iz] );
		  }
		  exit(0);
		}
		else
		{
		  if(verbose)
		    fprintf( stdout, "%s: %s: %s: ista=%d matching depth z=%g iz=%d found for station=%s.%s\n",
			progname,
			__FILE__,
			__func__,
			ista, 
			ev[ista].my_z,
			iz,
			ev[ista].stnm,
			ev[ista].net );
		}
	}
	ista = 0;
	iz = myiz;

/*** sdr_to_mt() returns mt[][] normalized by base_moment ***/

	if( ( ev[ista].str != -999 ) && ( ev[ista].dip != -999 ) && ( ev[ista].rak != -999 )  )
	{ 
	  sdr_to_mt( mt, ev[ista].str, ev[ista].dip, ev[ista].rak, ev[ista].Mw, verbose );
	  ev[ista].Mo = pow( 10.0, 1.5*( ev[ista].Mw + 10.73 ) );

	  if( mtdegfree == 1 ) 
	  {
		mt[1][2] = 0;
		mt[1][3] = 0;
		mt[2][1] = 0;
		mt[2][3] = 0;
		mt[3][1] = 0;
		mt[3][2] = 0;
		mt[1][1] = ev[ista].Mo/base_moment;
		mt[2][2] = ev[ista].Mo/base_moment;
		mt[3][3] = ev[ista].Mo/base_moment;
	  }

	  if( mtdegfree == 5 )
	  {
		mt[3][3] = -( mt[1][1] + mt[2][2] );

	  }
	}
	else
	{
		if( mtdegfree == 1 )
		{
		  mt[1][2] = 0;
                  mt[1][3] = 0;
                  mt[2][1] = 0;
                  mt[2][3] = 0;
                  mt[3][1] = 0;
                  mt[3][2] = 0;
                  mt[1][1] = ev[ista].Mxx * (ev[ista].Mo/base_moment);
                  mt[2][2] = ev[ista].Myy * (ev[ista].Mo/base_moment);
                  mt[3][3] = ev[ista].Mzz * (ev[ista].Mo/base_moment);
		}

		if( mtdegfree == 5 )
		{
		  mt[1][1] = ev[ista].Mxx * (ev[ista].Mo/base_moment);
                  mt[1][2] = ev[ista].Mxy * (ev[ista].Mo/base_moment);
                  mt[1][3] = ev[ista].Mxz * (ev[ista].Mo/base_moment);
                  mt[2][1] = mt[1][2];
                  mt[2][2] = ev[ista].Myy * (ev[ista].Mo/base_moment);
                  mt[2][3] = ev[ista].Myz * (ev[ista].Mo/base_moment);
                  mt[3][1] = mt[1][3];
                  mt[3][2] = mt[2][3];
		  mt[3][3] = -( mt[1][1] + mt[2][2] );
		}

		if( mtdegfree == 6 )
		{
		  mt[1][1] = ev[ista].Mxx * (ev[ista].Mo/base_moment);
		  mt[1][2] = ev[ista].Mxy * (ev[ista].Mo/base_moment);
                  mt[1][3] = ev[ista].Mxz * (ev[ista].Mo/base_moment);
                  mt[2][1] = mt[1][2];
		  mt[2][2] = ev[ista].Myy * (ev[ista].Mo/base_moment);
                  mt[2][3] = ev[ista].Myz * (ev[ista].Mo/base_moment);
                  mt[3][1] = mt[1][3];
                  mt[3][2] = mt[2][3];
		  mt[3][3] = ev[ista].Mzz * (ev[ista].Mo/base_moment);
		}
	}

/*** compute synthetics require unnormalized ***/
	sol[iz].moment_tensor[1][1] = mt[1][1] * base_moment;
	sol[iz].moment_tensor[1][2] = mt[1][2] * base_moment;
	sol[iz].moment_tensor[1][3] = mt[1][3] * base_moment;
	sol[iz].moment_tensor[2][1] = mt[2][1] * base_moment;
	sol[iz].moment_tensor[2][2] = mt[2][2] * base_moment;
	sol[iz].moment_tensor[2][3] = mt[2][3] * base_moment;
	sol[iz].moment_tensor[3][1] = mt[3][1] * base_moment;
	sol[iz].moment_tensor[3][2] = mt[3][2] * base_moment;
	sol[iz].moment_tensor[3][3] = mt[3][3] * base_moment;

/******************************************************************/
/*** form the moment tensor from solution vector x              ***/
/*** col1-Mxx, col2-Myy, col3-Mxy, col4-Mxz, col5-Myz, col6-Mzz ***/
/*** store the original moment tensor to solution structure     ***/
/******************************************************************/
	x_vector[1] = mt[1][1];
        x_vector[2] = mt[2][2];
        x_vector[3] = mt[1][2];
        x_vector[4] = mt[1][3];
        x_vector[5] = mt[2][3];
        x_vector[6] = mt[3][3];
	for( i = 1; i <= 6; i++ ) x_vector[i] *= base_moment;

/************************************************************************/
/*** FROM mtinv.c:main():invert():                                    ***/
/*** eigenvalue analysis on moment tensor to get PDC and str/dip/rak  ***/
/*** for both nodal planes and T- P- and N-axis.  Clone moment tensor ***/
/*** so not to change the values by the next two routines.  Find      ***/
/*** eigenvalues and eigenvectors by decomposing Mij using eigenvalue ***/
/*** analysis.  calculate the eigenvalues from the moment tensor ***/
/************************************************************************/
	
	set_moment_tensor( &Ma, x_vector, mtdegfree, verbose );

	mt2eig( Ma, sol, iz, verbose );

	eig2iso( sol, iz, verbose );

	normalize_moment_tensor( &Ma, &Mn, sol[iz].Mtotal, verbose );

/*** normalize all moment tensor and eigenvalues ***/

	sol[iz].dmoment = Ma.moment;
	sol[iz].mw      = Ma.Mw;
	sol[iz].exponent = Ma.expon;
	sol[iz].abcassa = Ma.abcassa;

	sol[iz].FullMT.eig[1].val /= pow(10.0, sol[iz].exponent );
	sol[iz].FullMT.eig[2].val /= pow(10.0, sol[iz].exponent );
	sol[iz].FullMT.eig[3].val /= pow(10.0, sol[iz].exponent );

	sol[iz].Dev.eig[1].val /= pow(10.0, sol[iz].exponent );
	sol[iz].Dev.eig[2].val /= pow(10.0, sol[iz].exponent );
	sol[iz].Dev.eig[3].val /= pow(10.0, sol[iz].exponent );

	sol[iz].FullMT.T.ev /= pow(10.0, sol[iz].exponent );
	sol[iz].FullMT.B.ev /= pow(10.0, sol[iz].exponent );
	sol[iz].FullMT.P.ev /= pow(10.0, sol[iz].exponent );
	
	sol[iz].mrr = Mn.rr;
	sol[iz].mtt = Mn.tt;
	sol[iz].mff = Mn.ff;
	sol[iz].mrt = Mn.rt;
	sol[iz].mrf = Mn.rf;
	sol[iz].mtf = Mn.tf;

	sol[iz].mxx = Mn.xx;
	sol[iz].mxy = Mn.xy;
	sol[iz].mxz = Mn.xz;
	sol[iz].myy = Mn.yy;
	sol[iz].myz = Mn.yz;
	sol[iz].mzz = Mn.zz;

	Eig2MajorDC( sol, iz, verbose );
	Eig2MinorDC( sol, iz, verbose );
	eig2lune_4mtinv( sol, iz, verbose );

	if( verbose )
	{
          /***1          2     3    4       5         6             7   8        9   10    11   12   13  ***/
	  fprintf( stdout,
	    "%s: %s: %s: iz=%d z=%g Mo=%6.2e Mw=%5.2f Mo=%5.2f x10^ %2d Mij=%.2f %.2f %.2f %.2f %.2f %.2f\n",
		progname,         	/* 1 */
		__FILE__,
		__func__,
		iz, 			/* 2 */
		grn[0][iz].evdp,        /* 3 */
		sol[iz].dmoment,        /* 4 */
		sol[iz].mw,		/* 5 */
		sol[iz].abcassa, 	/* 6 */
		sol[iz].exponent, 	/* 7 */
		sol[iz].mrr, 		/* 8 */
		sol[iz].mtt,		/* 9 */
		sol[iz].mff, 		/* 10 */
		sol[iz].mrt, 		/* 11 */
		sol[iz].mrf, 		/* 12 */
		sol[iz].mtf 		/* 13 */
		);
	}

/*** return x_vector from Moment in units of dyne*cm to scaled by base_moment Mw=0 (Mo=1.65e+15) **/

	for( i = 1; i <= 6; i++ ) x_vector[i] /= base_moment;

/*** initialize the a matrix ***/
	if( verbose )
	{
		fprintf( stdout,	
			"%s: %s: %s: initalizing matrix for forward calculation nsta=%d iz=%d\n",
			progname,
			__FILE__,
			__func__,
			nsta, iz );
	}
	rows = 1;
	for( ista=0; ista<nsta; ista++ )
	{
		if( ev[ista].iused == 1 )
		{
			rows += 3 * grn[ista][iz].nt;
		}
	}

	if(verbose)
	{
	  fprintf( stdout,
		"%s: %s: %s: allocating memory for a_matrix[][], s_vector[], and b_vector[], rows=%d cols=%d\n",
		progname, __FILE__, __func__, rows, cols );
	}

	a_matrix = matrix( 0, rows+1, 0, cols+1 );
	s_vector = vector( 0, rows+1 );
	b_vector = vector( 0, rows+1 );

	for( j=1; j<=cols; j++ )
	{
		for( i=1; i<=rows; i++ )
		{
			a_matrix[i][j] = 0;
		}
	}
	for( i=1; i<=rows; i++ )
	{
		s_vector[i] = 0;
		b_vector[i] = 0;
	}
	
	if(verbose)
	{
		for( i=1; i<=6; i++ )
		{
			fprintf( stdout, "%s: %s: %s: x(%d)=%g\n",
				progname, __FILE__, __func__, i, x_vector[i] );
		}
		fprintf( stdout, "\n");
	}

/****************************************************/
/*** make the A matrix do the forward calculation ***/
/*** then compute the variance reduction          ***/
/****************************************************/
	fprintf( stdout, "%s: %s: %s: iz=%d calculating b-vector: calling make_amatrix() and matmul() A-matrix*x_vector = b-vector\n",
                        progname, __FILE__, __func__, iz );
                fflush(stdout);

	make_amatrix( ev, grn, nsta, iz, a_matrix, b_vector, mtdegfree, Distance_Normalize, DistNormR0 );

	matmul( 0, a_matrix, cols, x_vector, rows, s_vector );

	sol[iz].var_red = variance_reduction( b_vector, s_vector, 1, rows+1 );

	if(verbose)
	{
		fprintf( stdout, "%s: %s: %s: called variance_reduction() iz=%d %%VRED=%g\n",
			progname, __FILE__, __func__, iz, sol[iz].var_red );
		fflush(stdout);
	}

	sol[iz].l2norm_error = compute_l2norm_error( b_vector, s_vector, rows );

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: called compute_l2norm_error(): iz=%d %%L2NORM=%g L2NORM/(NPTS-1)=%g NPTS=%d\n", 
		progname,
		__FILE__, __func__,
		iz,	
		sol[iz].l2norm_error,
		sol[iz].l2norm_error/(rows-1), rows );
	  fflush(stdout);
	}

	sol[iz].total_fitness1 = sol[iz].var_red;
	sol[iz].total_fitness2 = sol[iz].l2norm_error;

/*** get the solution from the first station ***/
	sol[iz].evlo = ev[0].evlo;
	sol[iz].evla = ev[0].evla;
	/* sol[iz].evdp = ev[0].evdp; */
	sol[iz].evdp  = grn[0][iz].evdp;
	sol[iz].ot   = ev[0].ot_shift;

	if(verbose) 
	{
	  fprintf( stdout, "%s: %s: %s: freeing memory inside forward_calc()\n",
		progname,
		__FILE__, __func__ );
	  fflush(stdout);
	}

	free_matrix( mt, 0, cols+1, 0, cols+1 );
	free_vector( x_vector, 0, cols+1 ); 
	free_matrix( a_matrix, 0, rows+1, 0, cols+1 );
	free_vector( s_vector, 0, rows+1 );
	free_vector( b_vector, 0, rows+1 );

	if(verbose)
	{
		fprintf( stdout, "%s: %s: %s: leaving forward calculation\n",
			progname, __FILE__, __func__ );
		fflush(stdout);
	}
}

/*******************************************************/
/*** this subroutine does the inversion by looping   ***/
/*** over all the greens functions in the library    ***/
/*******************************************************/

void invert(	
	EventInfo *ev,
	Greens **grn,
	int nsta,
	int nz,
	Solution *sol, 
	int verbose,
	int idump,
	int mtdegfree,
	int Distance_Normalize,
	float DistNormR0, 
	int specialLoadGrnMxy,
	int dev_mt_largest_vr )
{
/* see include/mt.h Mo = math.pow( 10.0, 1.5*(Mw+10.73) ) = 1.2445146117713818e+16; Reference Mw = 0.0 */

	int debug_invert = 0;  /*** write out the b_vector and s_vector as sac files ***/

	int ista, iz;
	float wmax, wmin;
	MomentTensor Ma, Mn;

/*********************************************************/
/*** cols = 6 colums of symetic moment tensor elements ***/
/*********************************************************/

	int i, j, rows, cols;

	float **a_matrix;   /*** A matrux with dimensions a[1..rows][1..cols] ***/
	float **u_matrix;   /*** temp space               u[1..rows][1..cols] ***/
	float **v_matrix;   /*** temp space               v[1..cols][1..cols] ***/
	float *w_vector;    /*** temp space               w[1..cols]          ***/
	float *b_vector;    /*** data                     b[1..rows]          ***/
	float *s_vector;    /*** synthetic                s[1..rows]          ***/
	float *x_vector;    /*** solution (mom ten)       x[1..cols]          ***/
	float *e_vector;    /*** error vector             e[1..cols]          ***/
	/* float **cv_matrix; */ /*** covariance matrix       cv[1..cols][1..cols] ***/ 
	
	float **matrix( int nrl, int nrh, int ncl, int nch );
	void free_matrix(float **m, int nrl, int nrh, int ncl, int nch );

	float *vector(int nl, int nh );
	void free_vector(float *v, int nl, int nh );

	float variance_reduction( float *data, float *synthetic, int nstart, int nend );
	float compute_l2norm_error( float *a, float *b, int n );

	void mtinv_dumpSAC( EventInfo *ev, Greens **grn, int nsta, int iz, float *s_vector, int verbose );

	/* void set_moment_tensor( MomentTensor *Ma, float *x_vector, int degfree, int verbose ); */
	void set_moment_tensor( MomentTensor *Ma, float *x_vector, int degfree, int verbose );

	/* void normalize_moment_tensor( MomentTensor *Ma, MomentTensor *Mn, float Mo, int ); */
        void normalize_moment_tensor( MomentTensor *Ma, MomentTensor *Mn, float Mo, int );

        void mt2eig( MomentTensor Ma, Solution *sol, int iz, int verbose );
        void eig2iso( Solution *sol, int iz, int verbose );
        void Eig2MajorDC( Solution *sol, int iz, int verbose );
        void Eig2MinorDC( Solution *sol, int iz, int verbose );
        void eig2lune_4mtinv( Solution *sol, int iz, int verbose );

/*** if specialLoadGrnMxy == 0 ***/
	void make_amatrix(
        	EventInfo *ev,
        	Greens **grn, 
        	int nsta,
        	int iz,
        	float **a_matrix,
        	float *b_vector,
        	int mtdegfree,
        	int Distance_Normalize,
        	float DistNormR0 );
	
/*** if specialLoadGrnMxy == 1 ***/
	void make_amatrix_special( 
		EventInfo *ev,
                Greens **grn,
                int nsta,
                int iz,
                float **a_matrix,
                float *b_vector,
                int mtdegfree,
                int Distance_Normalize,
                float DistNormR0 );
                

        void grn2disp( Greens *g, EventInfo *ev, int verbose, int mtdegfree );

	char sacfilename[256];
	void wrtnewsac( char *FO, float dt, int ns, float *ar, float b );
	/*** writesacfile.c ***/
        void writesacfile( EventInfo *ev );

	/*** mtinv_subs.c ***/
	void write_gmt_xy_values( Solution *sol,
				  EventInfo *ev,
        			  Greens **grn, 
				  int iz, 
				  int nsta, 
				  int verbose );

	void svbksb( float **, float *, float **, int, int, float *, float * );
	void svdcmp( float **, int, int, float *, float ** );
	/* void svdvar( float **, int, float *, float ** ); */

	void matmul( int conj, float **bb, int nx, float *x, int ny, float *y );
	
	 /* int write_matrix = 1; */
	int write_matrix = 0;
	 void writeMatrix( int rows, int cols, float **a_matrix, float *b_vector, float *s_vector, float *x_vector );

/*************************************************/
/*** get the total data length rows the matrix ***/
/*************************************************/

	cols = 6;

	for( iz = 0 ; iz < nz ; iz++ )
	{
		 sol[iz].evdp = grn[0][iz].evdp;
		rows = 1;
		for(ista=0; ista<nsta; ista++ )
		{
			if( ev[ista].iused == 1 )
			{
				rows += 3*grn[ista][iz].nt;
			}
		}
	}

	if( verbose ) 
	{
	  fprintf( stdout, "%s: %s: %s: A matrix: rows=%d cols=%d\n",
		progname, __FILE__, __func__, rows, cols);
	}
	
/*************************/
/*** Allocating memory ***/
/*************************/

	if(verbose) fprintf( stdout, "%s: %s: %s: Allocating memory\n", progname, __FILE__, __func__ );
	fflush(stdout);

	if(verbose) fprintf( stdout, "%s: %s: %s: Allocating memory for a_matrix\n", progname, __FILE__, __func__ );
	fflush(stdout);
	a_matrix  = matrix( 0, rows+1, 0, cols+1 );

	if(verbose)fprintf( stdout, "%s: %s: %s: Allocating memory for u_matrix\n", progname, __FILE__, __func__ );
	u_matrix  = matrix( 0, rows+1, 0, cols+1 );

	if(verbose)fprintf( stdout, "%s: %s: %s: Allocating memory for v_matrix\n", progname, __FILE__, __func__ );
	v_matrix  = matrix( 0, cols+1, 0, cols+1 );

	/* if(verbose)fprintf( stdout, "%s: %s: %s: Allocating memory for cv_matrix\n", progname, __FILE__, __func__ ); */
	/* cv_matrix = matrix( 0, cols+1, 0, cols+1 ); */

	if(verbose)fprintf( stdout, "%s: %s: %s: Allocating memory for w_vector\n", progname, __FILE__, __func__ );
	w_vector = vector( 0, cols+1 );

	if(verbose)fprintf( stdout, "%s: %s: %s: Allocating memory for e_vector\n", progname, __FILE__, __func__ );
	e_vector = vector( 0, cols+1 );

	if(verbose)fprintf( stdout, "%s: %s: %s: Allocating memory for x_vector\n", progname, __FILE__, __func__ );
	x_vector = vector( 0, cols+1 );

	if(verbose)fprintf( stdout, "%s: %s: %s: Allocating memory for b_vector\n", progname, __FILE__, __func__ );
	b_vector = vector( 0, rows+1 );

	if(verbose)fprintf( stdout, "%s: %s: %s: Allocating memory for s_vector\n", progname, __FILE__, __func__ );
	s_vector = vector( 0, rows+1 );
	
/************************************************************************************************/
/*** LOOP OVER DEPTH ****************************************************************************/
/************************************************************************************************/
	
/*****************************************/
/***** initialize memory invert() ********/
/****************************************/
	fprintf( stdout, "%s: %s: %s: ======================================= START DEPTH LOOP ==================================================================================\n",
		progname, __FILE__, __func__ );

	for( iz = 0 ; iz < nz ; iz++ )
	{
		fprintf( stdout, "%s: %s: %s: ================================ DEPTH LOOP ITERATION iz=%d/nz=%d z=%g z=%g  ==================================================================================\n",
                progname, __FILE__, __func__, iz, nz, sol[iz].evdp, grn[0][iz].evdp  );
		fflush(stdout);

		for( j=1; j<=cols; j++ )
		{
			/****
			fprintf( stdout, "%s: %s: %s: hello 1 j=%d cols=%d\n",
				progname, __FILE__, __func__,
				j, cols ); fflush(stdout);
			fflush( stdout );
			***/

			for( i=1; i<=rows; i++ )
			{
				/****
				fprintf( stdout, "%s: %s: %s: hello 2 i=%d rows=%d u_matrix[%d][%d]=%g a_matrix[%d][%d]=%g\n",
                                    progname, __FILE__, __func__, 
                                    i, rows, i, j, u_matrix[i][j], i, j, a_matrix[i][j] );
				  fflush(stdout);
				****/

				u_matrix[i][j] = 0;
				a_matrix[i][j] = 0;
			}
			w_vector[j] = 0;
			e_vector[j] = 0;
			x_vector[j] = 0;
		}

		for( j=1; j<=cols; j++ )
		{
			for( i=0; i<=cols; i++ )
			{
				/* cv_matrix[i][j] = 0;	 */
				v_matrix[i][j]  = 0;
			}
		}

		for( i=1; i<=rows; i++ )
		{
			b_vector[i] = 0;
			s_vector[i] = 0;
		}

	/*********************************************************/
	/*** set up the A matrix and data vector for inversion ***/
	/*********************************************************/

		if(specialLoadGrnMxy==0)
		{
		  fprintf( stdout, "%s: %s: %s: calling make_amatrix(): \n", progname, __FILE__, __func__ );
		  fflush(stdout);
		  make_amatrix( ev, grn, nsta, iz, a_matrix, b_vector, mtdegfree, Distance_Normalize, DistNormR0 );
		}

		if(specialLoadGrnMxy==1)
		{
		  fprintf( stdout, "%s: %s: %s: calling make_amatrix_special(): \n", progname, __FILE__, __func__ );
		  fflush(stdout);
		  make_amatrix_special( ev, grn, nsta, iz, a_matrix, b_vector, mtdegfree, Distance_Normalize, DistNormR0 );
		}

	/***************************************/
	/*** copy A matrix into the U matrix ***/
	/***************************************/

		for( j=1; j<=cols; j++ )
		{
			for( i=1; i<=rows; i++ )
			{
				u_matrix[i][j] = a_matrix[i][j];	
			}
		}

	/******************************/
	/***** do the inversion *******/
	/******************************/

		if( verbose )
		{
		  fprintf( stdout, "%s: %s: %s: inverting... calling svdcmp() rows=%d cols=%d\n", 
			progname, __FILE__, __func__, rows, cols );
		  fflush(stdout);
		}

		svdcmp( u_matrix, rows, cols, w_vector, v_matrix );

	/********************************************************************************************/
	/*** this is where we set the threshold for singular values allowed to be nonzero.        ***/
	/*** The constant is typical but not universal. experiment with values other than 1.0E-06 ***/
	/*** this will be the maximum singular value obtained                                     ***/
	/********************************************************************************************/

		wmax = 0;
		for( j=1; j<=cols; j++)
		{
			if( w_vector[j] > wmax) wmax=w_vector[j];
		}

		wmin = wmax * 1.0E-5;
		for( j=1; j<=cols; j++)
		{
			if( w_vector[j] < wmin ) w_vector[j]=0.0;
		}

		if( verbose )
		{
		  fprintf( stdout, "%s: %s: %s: inverting... calling svbksb() rows=%d cols=%d\n", 
			progname, __FILE__, __func__, rows, cols );
		  fflush(stdout);
		}

		svbksb( u_matrix, w_vector, v_matrix, rows, cols, b_vector, x_vector );

	/****************************************************/
	/*** multiply solution x vector with a matrix or  ***/
	/*** greens function matrix to get the s vector   ***/
	/*** or synthetics vector then....                ***/
	/*** compute the variance reduction and           ***/
	/*** l2_norm error                                ***/
	/****************************************************/

		matmul( 0, a_matrix, cols, x_vector, rows, s_vector );

		if( write_matrix )
			writeMatrix( rows, cols, a_matrix, b_vector, s_vector, x_vector );

		if(debug_invert)
		{
		/*** solution vector, a-matrix * x-matrix ***/
		  sprintf( sacfilename, "s_vector.%02d.sac", iz );
		  wrtnewsac( sacfilename, 1.0, rows, s_vector, 0 );

		/*** the data matrix, preshift? ***/
		  sprintf( sacfilename, "b_vector.%02d.old.sac", iz );
		  wrtnewsac( sacfilename, 1.0, rows, b_vector, 0 );
		}

		sol[iz].var_red = variance_reduction( b_vector, s_vector, 1, rows+1 );
		
		if(verbose) 
		{
			fprintf( stdout, "%s: %s: %s: iz=%d %%VRED=%g\n", 
				progname, __FILE__, __func__,
				iz, sol[iz].var_red );
			fflush(stdout);
		}

		sol[iz].l2norm_error = compute_l2norm_error( b_vector, s_vector, rows );

	/****************************************************/
	/*** this is the right one ***/
	/****************************************************/

		if(verbose)
		{
		  fprintf(stdout, "%s: %s: %s: iz=%d z=%g z=%g %%L2NORM=%g L2NORM/(NPTS-1)=%g NPTS=%d\n",
                     progname,
			__FILE__, __func__,
			iz, 
			sol[iz].evdp, 
			grn[0][iz].evdp,
			sol[iz].l2norm_error,
                     sol[iz].l2norm_error/(rows-1), rows );
		  fflush(stdout);
		}

	/****************************************************/
	/*** calculate the covariance matrix              ***/
	/*** default is sigma = 1                         ***/
	/*** get sigma from RMS preevent noise ? level    ***/
	/*** error is 1.96 * sqrt( diag(CV_matrix) )      ***/
	/****************************************************/

		/* svdvar( v_matrix, cols, w_vector, cv_matrix ); */

	/**********************************************************************************/
	/*** Demultiplex synthetics from s_vector & write out syn and data to SAC files ***/
	/*** used to debug the matrix inversion by comparing direct fit between         ***/
	/*** synthetics and data. getpar flag dumpsac=idump  and dumpxy flag idumpxy    ***/
	/*** This is not used by gmtwf.csh see dumpxy() and compute_synthetics()        ***/
	/**********************************************************************************/
		if( idump )
		{
			fprintf( stdout, "%s: %s: %s: idump=%d calling mtinv_dumpSAC()\n",
				progname, __FILE__, __func__, idump );
			fflush(stdout);
			mtinv_dumpSAC( ev, grn, nsta, iz, s_vector, verbose );
		}

	/******************************************************************/
        /*** form the moment tensor from solution vector x              ***/
        /*** col1-Mxx, col2-Myy, col3-Mxy, col4-Mxz, col5-Myz, col6-Mzz ***/
	/*** store the original moment tensor to solution structure     ***/
        /******************************************************************/
		for( i = 1; i <= 6; i++ ) 
		{
			x_vector[i] *= base_moment;

		/*
		  fprintf( stderr, "%s: %s: %s: iz=%d i=%d x_vector[i]=%e Dyn*cm base_moment=%e x_vector[i]=%e N*m\n",
		    progname, __FILE__, __func__, iz, i, x_vector[i], base_moment, x_vector[i]*1.0E-07 );
		*/
		}
	
                sol[iz].moment_tensor[1][1] = x_vector[1]; /* Mxx */
                sol[iz].moment_tensor[2][2] = x_vector[2]; /* Myy */
                sol[iz].moment_tensor[1][2] = x_vector[3]; /* Mxy */
                sol[iz].moment_tensor[1][3] = x_vector[4]; /* Mxz */
                sol[iz].moment_tensor[2][3] = x_vector[5]; /* Myz */
                sol[iz].moment_tensor[3][3] = x_vector[6]; /* Mzz */

                sol[iz].moment_tensor[2][1] = x_vector[3]; /* Myx */
                sol[iz].moment_tensor[3][1] = x_vector[4]; /* Mzx */
                sol[iz].moment_tensor[3][2] = x_vector[5]; /* Mzy */

                if( mtdegfree == 5 )
                {
		  sol[iz].moment_tensor[3][3] = -(x_vector[1] + x_vector[2]);
                }

                set_moment_tensor( &Ma, x_vector, mtdegfree, verbose );

	/************************************************************************/
	/*** eigenvalue analysis on moment tensor to get PDC and str/dip/rak  ***/
	/*** for both nodal planes and T- P- and N-axis.  Clone moment tensor ***/
	/*** so not to change the values by the next two routines.  Find      ***/
	/*** eigenvalues and eigenvectors by decomposing Mij using eigenvalue ***/
	/*** analysis                                                         ***/
	/************************************************************************/
	
		
		fprintf( stdout, "%s: %s: %s: calling mt2eig: iz=%d ts0=%g\n", 
			progname, __FILE__, __func__, iz, ev[0].ts0 );
		fflush(stdout);

		mt2eig( Ma, sol, iz, verbose );
		eig2iso( sol, iz, verbose );

		normalize_moment_tensor( &Ma, &Mn, sol[iz].Mtotal, verbose );

/*** normalize all moment tensor and eigenvalues ***/

		sol[iz].dmoment  = Ma.moment;
                sol[iz].mw       = Ma.Mw;
                sol[iz].exponent = Ma.expon;
                sol[iz].abcassa  = Ma.abcassa;

		sol[iz].FullMT.eig[1].val /= pow(10.0, sol[iz].exponent );
		sol[iz].FullMT.eig[2].val /= pow(10.0, sol[iz].exponent );
		sol[iz].FullMT.eig[3].val /= pow(10.0, sol[iz].exponent );

		sol[iz].Dev.eig[1].val /= pow(10.0, sol[iz].exponent );
		sol[iz].Dev.eig[2].val /= pow(10.0, sol[iz].exponent );
		sol[iz].Dev.eig[3].val /= pow(10.0, sol[iz].exponent );

		sol[iz].FullMT.T.ev /= pow(10.0, sol[iz].exponent );
		sol[iz].FullMT.B.ev /= pow(10.0, sol[iz].exponent );
		sol[iz].FullMT.P.ev /= pow(10.0, sol[iz].exponent );

		sol[iz].mrr = Mn.rr;
                sol[iz].mtt = Mn.tt;
                sol[iz].mff = Mn.ff;
                sol[iz].mrt = Mn.rt;
                sol[iz].mrf = Mn.rf;
                sol[iz].mtf = Mn.tf;

                sol[iz].mxx = Mn.xx;
                sol[iz].mxy = Mn.xy;
                sol[iz].mxz = Mn.xz;
                sol[iz].myy = Mn.yy;
                sol[iz].myz = Mn.yz;
                sol[iz].mzz = Mn.zz;

		Eig2MajorDC( sol, iz, verbose );
		Eig2MinorDC( sol, iz, verbose );
		eig2lune_4mtinv( sol, iz, verbose );

	/**********************************/
	/***    output for GMT plots	***/
	/**********************************/
		sol[iz].total_fitness1 = 0;
		sol[iz].total_fitness2 = 0;

		if( mtdegfree == 1 || mtdegfree == 6 || dev_mt_largest_vr )
		{
			sol[iz].total_fitness1 = sol[iz].var_red;
			sol[iz].total_fitness2 = sol[iz].var_red;
		}
		else if( mtdegfree == 5 && !dev_mt_largest_vr )
		{
		   sol[iz].total_fitness1 = 
			sol[iz].var_red / ( 101.0 - sol[iz].PDC );

		   sol[iz].total_fitness2 = 
			18 + sol[iz].var_red / ( 9 + (100-sol[iz].PDC) );

		  if( sol[iz].total_fitness2 > 35 ) sol[iz].total_fitness2 = 35;
		  if( sol[iz].total_fitness2 < 18 ) sol[iz].total_fitness2 = 18;
		}
		sol[iz].evlo = ev[0].evlo;
		sol[iz].evla = ev[0].evla;
		/* sol[iz].evdp = ev[0].evdp; */
		sol[iz].ot   = ev[0].ot_shift;

	/******************************************************************/
	/*** write the output to GMT files for plotting in plotmech.csh ***/
	/******************************************************************/

		if(verbose)
		{
		  fprintf( stdout,
		   "%s: %s: %s: calling write_gmt_xy_values(): iz=%d nsta=%d.\n", 
			progname, __FILE__, __func__, iz, nsta );
		  fflush(stdout);
		}

		write_gmt_xy_values( sol, ev, grn, iz, nsta, verbose );

		if(verbose)
		{
		  fprintf( stdout, "%s: %s: %s: finish iteration loop over depth iz=%d nz=%d z=%g z=%g\n",
                	progname, __FILE__, __func__,
                	iz, nz, sol[iz].evdp, grn[0][iz].evdp );
		  fflush(stdout);
		}

	} /*** loop over iz ***/

/******************************************/
/*** free memory clean up from invert() ***/
/******************************************/

	if( verbose ) 
	{
	  fprintf(stdout, 
		"%s: %s: %s: freeing memory inside invert\n\trows=%d\n\tcols=%d\n\n", 
			progname, __FILE__, __func__, rows, cols );
	  fflush(stdout);
	}

/****
	if(verbose) fprintf(stdout, "%s: %s: %s: freeing memory for a_matrix\n", progname, __FILE__, __func__ );
	free_matrix( a_matrix, 0, rows+1, 0, cols+1 );

	if(verbose) fprintf(stdout, "%s: %s: %s: freeing memory for u_matrix\n", progname, __FILE__, __func__ ); 
	free_matrix( u_matrix, 0, rows+1, 0, cols+1 );

	if(verbose) fprintf(stdout, "%s: %s: %s: freeing memory for v_matrix\n", progname, __FILE__, __func__ ); 
	free_matrix( v_matrix, 0, cols+1, 0, cols+1 );

	if(verbose) fprintf(stdout, "%s: %s: %s: freeing memory for cv_matrix\n", progname, __FILE__, __func__ );
	free_matrix( cv_matrix, 0, cols+1, 0, cols+1 );

	if(verbose) fprintf(stdout, "%s: %s: %s: freeing memory for x_vector\n", progname, __FILE__, __func__ ); 
	free_vector( x_vector, 0, cols+1 );

	if(verbose) fprintf(stdout, "%s: %s: %s: freeing memory for s_vector\n", progname, __FILE__, __func__ ); 
	free_vector( s_vector, 0, rows+1 );

	if(verbose) fprintf(stdout, "%s: %s: %s: freeing memory for w_vector\n", progname, __FILE__, __func__ ); 
	free_vector( w_vector, 0, cols+1 );

	if(verbose) fprintf(stdout, "%s: %s: %s: freeing memory for b_vector\n", progname, __FILE__, __func__ ); 
	free_vector( b_vector, 0, rows+1 );

	if(verbose) fprintf(stdout, "%s: %s: %s: freeing memory for e_vector\n", progname, __FILE__, __func__ ); 
	free_vector( e_vector, 0, cols+1 );

****/
	if(verbose)
	{
	 fprintf( stdout, "%s: %s: %s: Exiting from invert()\n\n", 
		progname, __FILE__, __func__ );
	 fflush(stderr);
	 fflush(stdout);
	}
}

void Usage_Print()
{
	fprintf( stderr, "\n" );
	fprintf( stderr, "USAGE: (examples)\n" );
	fprintf( stderr, "\tdeviatoric MT:\n");
	fprintf( stderr, "\t\t %s par=mtinv.par mtdegfree=5 ts0=0 gmt5 use_snr minsnr=3 shift ctol=0.85 maxshift=10\n", progname );
	fprintf( stderr, "\t      full MT:\n");
	fprintf( stderr, "\t\t %s par=mtinv.par mtdegfree=6 ts0=0 gmt5 use_snr minsnr=3 shift ctol=0.85 maxshift=10\n", progname );
	fprintf( stderr, "\n" );

	fprintf( stderr, "USAGE: (all args)\n" );
	fprintf( stderr, "\t %s par=(string) mtdegfree=(float) ts0=(float) [no]fwd [no]verbose [no]gmtmap [no]dumpsac [no]PltXcorLabel no]gmt5 \\ \n", progname );
	fprintf( stderr, "\t          [no]compute_station_vred [no]AutoAuth evid=(long) fixz=(float) [no]norm R0=(float)\\ \n" );
	fprintf( stderr, "\t          [no]shift ctol=(float) maxshift=(float) [no]use_snr minsnr=3 [no]dumpxy orientation=(string) sort_by_value=(string)\\ \n" );
	fprintf( stderr, "\t          [no]print_gmtwf_mt_info [no]mysql [no]oracle [no]sqlite [no]sqlite3 [no]special\n" );
	fprintf( stderr, "\n" );

	fprintf( stderr, "REQUIRED PARAMETERS:\n" );
 	fprintf( stderr, "\t par=(mtinv.par)     mtinv station-filter-model parameter PAR file (string)\n" );
	fprintf( stderr, "\t mtdegfree=(integer) 1=Isotropic MT, 5=Deviatoric MT, 6=Full MT (only 1, 5, or 6 allowed)\n" );
	fprintf( stderr, "\t ts0=(float)         Origin Time Shift in seconds relative to absolute time in mtinv.par\n" );
	fprintf( stderr, "\n" );

	fprintf( stderr, "OPTIONAL PARAMETERS: [DEFAULTS]\n" );
	fprintf( stderr, "\t [no]fwd      (boolean) Do forward calculation using Str/Dip/Rak Mw and depth input from mtinv.par. default is off\n" );
	fprintf( stderr, "\t [no]verbose  (boolean) Give verbose print to stdout. Default is off.\n" );
	fprintf( stderr, "\t [no]gmtmap   (boolean) Make a C-shell GMT script to plot a map of stations and solution, Default is off.\n" );
	fprintf( stderr, "\t [no]dumpsac  (boolean) Write out data and synthetics as SAC files. default is off\n" );
	fprintf( stderr, "\t [no]PltXcorLabel (boolean) label lag times and cross correlations in the PostScript plot for each chan [Default is on]\n" );
	fprintf( stderr, "\t [no]gmt5      (boolean) C-shell scripts using GMT plotting output version 5+  else version 4 [default on]\n" );
	fprintf( stderr, "\t [no]compute_station_vred (boolean)\n" );
        fprintf( stderr, "\t              computes variance reductions for each station and channel.  Outputs file vred.out [default on] \n" );
        fprintf( stderr, "\t [no]AutoAuth   (boolean) use AutoMT as Database output author else uses Operating System Enviroment username default off\n" );
        fprintf( stderr, "\t evid           (long) EventID limited to 10 digit long int, default -1 none\n" );
	fprintf( stderr, "\n" );

	fprintf( stderr, "\tOPTIONAL DEPTH PARAMETERS: \n" );
	fprintf( stderr, "\t fixz         (float) Fix the depth in kilometers. [Default is [-99] free depth MT inversion)\n" );
	fprintf( stderr, "\n" );

	fprintf( stderr, "\tOPTIONAL NORMALIZATION PARAMETERS: \n" );
	fprintf( stderr, "\t [no]norm     (boolean) Distance normalization default is off\n" );
	fprintf( stderr, "\t R0           (float) If norm true then normalize Green functions to distance in km of R/R0 required if norm is set\n" );
	fprintf( stderr, "\n" );

	fprintf( stderr, "\tOPTIONAL SIGNAL QUALITY CONTROL PARAMETERS: (cross-correlation time shift, Signal-Noise-Ratio SNR)\n" );
	fprintf( stderr, "\t [no]shift    (boolean) Shift the data automatically by cross correlation peak. default is off\n" );
	fprintf( stderr, "\t                  if shift is on then only determines optimal lag times based on max cross-correlation\n" );
	fprintf( stderr, "\t ctol=[0.6]   (float) Correlation coefficient tolerance to shift the data when coef > ctol. defaut is off\n" );
	fprintf( stderr, "\t maxshift=    (float) Maximum time in seconds a shift is allowed. default off\n" );
	fprintf( stderr, "\t                 user must manually transfer the time shifts into the run.csh file mtinv.par section to take effect\n" );  
	fprintf( stderr, "\t [no]use_snr   (boolean) use peak-to-peak amplitude based Signal-Noise Ratio to make stations non-defining in inversion [default off]\n" );
	fprintf( stderr, "\t                    see output file snr.out for values\n" );
	fprintf( stderr, "\t minsnr=3      (float) minimum snr threshold.  all 3-components must be less than minsnr to set non-defining in inversion [default 3]\n" );
	fprintf( stderr, "\t                  use_snr and minsnr only applies to stations that are defining and does not override users settings \n" );
	fprintf( stderr, "\t                  there is basic logic to account for radiation pattern of love and rayleigh waves \n" );
	fprintf( stderr, "\n" );

	fprintf( stderr, "\tOPTIONAL HIGH QUALITY GMT WAVEFORM PLOT:\n" );
	fprintf( stderr, "\t [no]dumpxy   (boolean) write out (x,y) ascii text files for the data and synthetics for GMT plots [Default is off]\n" );
	fprintf( stderr, "\t                  includes GMT C-shell script (gmtwf.csh) which generates pages of postscripts plots - use mtbestfit pretty_plot\n" );
	fprintf( stderr, "\t orientation   (string) if dumpxy on then plot orientation (\"portrait\" or \"landscape\") default landscape\n" );
	fprintf( stderr, "\t sort_by_value (string) if dumpxy on then sorts plot order of waveforms in gmtwf.csh sort_by_value (dist, none, azi, baz) default dist\n" );
	fprintf( stderr, "\t [no]print_gmtwf_mt_info (boolean) if dumpxy on then print_gmtwf_mt_info controls amount of information about mt solution in gmtwf.csh plot\n" );
	fprintf( stderr, "\n" );

	fprintf( stderr, "\tOPTIONAL DATABASE OUTPUT PARAMETERS:\n");
	fprintf( stderr, "\t [no]mysql     (boolean) write out mysql source scripts (create.sql,insert.sql) based on the NNSA custom schema [Default is off]\n" );
	fprintf( stderr, "\t [no]oracle    (boolean) write out oracle scripts (create.sql,insert.sql) based on the NNSA custom schema [Default is off]\n" );
	fprintf( stderr, "\t [no]sqlite or sqlite3 (boolean) write out sqlite scripts (create.sql,insert.sql) based on the NNSA custom schema [Default is off]\n" );
	fprintf( stderr, "\n" );

	fprintf( stderr, "\tOPTIONAL NEW FEATURE:\n" );
	fprintf( stderr, "\t [no]special   (boolean) reads Greens function from SAC files in Mij(Z,R,T) format (17 files) not \n" );
	fprintf( stderr, "\t                 RSS,RDS,RDD,REP,ZSS,ZDS... format (10 fundamental faulting orientations) [default off] see grn2Mxy and glib2inv test_special\n" );
	fprintf( stderr, "\n" );


	fprintf( stderr, "\tDESCRIPTION: \n" );
	fprintf( stderr, "\t mtinv performs a time-domain long-period regional moment tensor inversion \n" );
	fprintf( stderr, "\t See mtinv man page or manual for mtinv.par PAR file formating details\n" );
	fprintf( stderr, "\t mtinv returns best fit MT solution for all depths in the Green's function library\n" );
	fprintf( stderr, "\t user shifts origin-time using the ts0 parameter. For example, here is a for loop over +/-8 sec around the absolute origin-time: \n" );
	fprintf( stderr, "\t foreach ts0 ( -8 -7 -6 -5 -4 -3 -2 -1 0 1 2 3 4 5 6 7 8 )\n" );
	fprintf( stderr, "\t   mtinv par=mtinv.par mtdegfree=5 ts0=${ts0}\n" );
	fprintf( stderr, "\t end\n" );
	fprintf( stderr, "\n" );
}
