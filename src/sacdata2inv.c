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

/*** reads SAC files and outputs *.data files processed (3-C) data for moment tensor inversion - mtinv.c ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>

#include "../include/mt.h"         /** global datatype and structure declarations **/
#include "../include/nrutil.h"     /** numerical recipes **/

char progname[128];

int main( int ac, char **av )
{

/****************************************************************************************/
/*** event information stuff including 3-component                                    ***/
/****************************************************************************************/

	EventInfo *ev;
	int ista, nsta;
        char evinfo_filename[128];

	/*** glib2inv_subs.c ***/
	EventInfo *glib2inv_get_input_parameters(
		char *filename,
		EventInfo *ev,
		int *n,
		int verbose );

/****************************************************************************************/
/*** Greens function header and synthetics library structure                          ***/
/****************************************************************************************/

	Greens **grn;
        FILE *fpg;
	int iz, nz;
	float *z;

/****************************************************************************************/
/*** sac file handaling, data processing, and data filtering parameters               ***/
/****************************************************************************************/

/** max number of files in a directory is 2048 ***/

	char SAC_DATA_pathname[256], respdir[256];

/****************************************************************************************/
/*** output options                                                                   ***/
/****************************************************************************************/
        int verbose=0;          /*** verbose   1=yes 0=no default no verbose ***/
	int dumpsac=0;          /*** dump the processed and filtered data as SAC binary files ***/
	int ienvelope = 0;     /*** envelope is deprecated gene.ichinose 07-01-2024 ***/
	int iparallel = 0;

	char currentdir[128];
	
/*******************************/
/*** subroutine prototypes   ***/
/*******************************/

	/*** sacdata2inv_subs.c ***/
	void  writesacfile( EventInfo *ev );

	FILE *fp_SNR;

/*** misc ***/
	void  Print_Usage(void);
	int   setpar(int,char **);
	int mstpar( char *, char *, void * );
        int getpar( char *, char *, void * );
	void  endpar();
	char exec_pathname[128];
	char *shorten_path( char *pathname, char *filename );

/*** sacdata2inv_loaddata.c ***/
	void sacdata2inv_loaddata(
	        EventInfo *ev,
        	int ista,
        	char *sacdatadir,
        	char *respdir,
        	char *currentdir,
        	int verbose );

/*** sacdata2inv_serial.c  ***/
	void sacdata2inv_serial( 
		EventInfo *ev, 
		int ista,
		int ienvelope,
		int dumpsac, 
		int verbose );

/**** sacdata2inv_parallel *****/
	void sacdata2inv_parallel(
        	EventInfo *ev,
        	int nsta,
        	int ienvelope,
        	int dumpsac,
        	int verbose );

/****************************************************************************************/
/*** get the directory location for output files                                      ***/
/****************************************************************************************/
	strcpy( exec_pathname, av[0] );
	shorten_path( exec_pathname, progname );

	if( verbose )
	{
	  fprintf( stdout, "%s: STDOUT: Version=%s ReleaseDate=%s exec full path=%s\n",
		progname, Version_Label, Version_Date, exec_pathname );
	}

	fprintf( stderr, "%s: STDERR: Version=%s ReleaseDate=%s exec full path=%s\n",
		progname, Version_Label, Version_Date, exec_pathname );

/****************************************************************************************/
/*** USAGE                                                                            ***/
/****************************************************************************************/

	if( ac <= 1 ) Print_Usage();

/****************************************************************************************/
/*** get the input parameters from command line                                       ***/
/****************************************************************************************/

	strcpy( SAC_DATA_pathname, ".\0" );
	strcpy( respdir, ".\0" );
        setpar(ac,av);
        mstpar( "par",     "s", &evinfo_filename );
        getpar( "path",    "s", &SAC_DATA_pathname );
	getpar( "respdir", "s", &respdir );
        getpar( "verbose", "b", &verbose );
        getpar( "dumpsac", "b", &dumpsac );
	getpar( "parallel", "b", &iparallel );
	endpar();

/****************************************************************************************/
/****************************************************************************************/
	getcwd( currentdir, 128 );
/*
	if( verbose )
	{
	  fprintf( stdout, "\n\n%s: current working directory is %s\n", 
		progname, currentdir );
	}
*/

/**************************************************************/
/*** get the input parameters from the input parameter file ***/
/**************************************************************/
	ev  = (EventInfo *)malloc(sizeof(EventInfo));
	ev  = (EventInfo *)
	  glib2inv_get_input_parameters( 
		evinfo_filename, 
		ev, 
		&nsta, 
		verbose );

/*********************************************************************/
/*** loop over each station and load the Green's function header   ***/
/*** to load in the values for Reduction Velocity and ts0          ***/
/*********************************************************************/

	grn = (Greens **) malloc(nsta*sizeof(Greens *));
	for( ista=0; ista<nsta; ista++ )
	{
		if(verbose)
		{
		  fprintf(stdout, "%s: loading green's functions %s\n", 
			progname, ev[ista].glib_filename );
		}

		if( (fpg=fopen( ev[ista].glib_filename, "rb" )) == NULL )
		{
			fprintf( stderr, "%s: Fatal Error, cannot open file %s\n",
				progname, ev[ista].glib_filename );
			exit(-1);
		}

		fread( &nz, sizeof(int), 1, fpg );
		z = malloc( nz*sizeof(float) );
		fread( &z[0], nz*sizeof(float), 1, fpg );
		grn[ista] = (Greens *)malloc(nz*sizeof(Greens) );

		for( iz=0; iz<nz; iz++ )
		{
			fread( &grn[ista][iz], sizeof(Greens), 1, fpg );
			if(verbose)
			{
			  fprintf( stdout,
			    "%s: iz=%d z=%g rdist=%g az=%g evdp=%g t0=%g dt=%g nt=%d %s\n",
				progname,
				iz,
				z[iz],
				grn[ista][iz].rdist,
				grn[ista][iz].az,
				grn[ista][iz].evdp,
				grn[ista][iz].t0,
				grn[ista][iz].dt,
				grn[ista][iz].nt,
				grn[ista][iz].filename );
			}
		}
		fclose(fpg);

		ev[ista].redv   = grn[ista][0].redv;
		ev[ista].ts0    = grn[ista][0].ts0;
		ev[ista].tstart = grn[ista][0].tstart;
		ev[ista].tend   = grn[ista][0].tend;
		ev[ista].rdist  = grn[ista][0].rdist;
	}

/********************************************************************/
/*** loop over each station and load its SAC files for all 3 comp ***/
/********************************************************************/
	fprintf( stdout, "%s: %d number of stations to process.\n",
		progname, nsta );

	sacdata2inv_loaddata( ev, nsta, SAC_DATA_pathname, respdir, currentdir, verbose );

	if( iparallel )
	{
		sacdata2inv_parallel(
			ev,
			nsta, 
			ienvelope,
			dumpsac,
			verbose );
	}
	else
	{
	  for( ista=0; ista<nsta; ista++ )
	  {
		sacdata2inv_serial(
			&(ev[ista]),
			ista, 
			ienvelope,
			dumpsac,
			verbose );
	  }
	}

/********************************************************************/
/*** output snr                                                   ***/
/********************************************************************/

	fprintf( stdout, "\n\n%s: Writting to file snr.out\n", progname );
	fp_SNR = fopen( "snr.out", "w" );

	for( ista=0; ista<nsta; ista++ )
	{
		if( strcmp( ev[ista].loc, "" ) == 0 ) strcpy( ev[ista].loc, "--" );

		fprintf( stdout, 
"\n%s.%s.%s\n\tZ:  Tnoi=%.1f Tsig=%.1f log10amp=%.5f/%.5f snr=%.5f\n\tNS: Tnoi=%.1f Tsig=%.1f log10amp=%.5f/%.5f snr=%.5f\n\tEW: Tnoi=%.1f Tsig=%.1f log10amp=%.5f/%.5f snr=%.5f\n",
	ev[ista].net, 
	ev[ista].stnm, 
	ev[ista].loc,
  ev[ista].z.pha[NOISE].period,  ev[ista].z.pha[SIGNAL].period,  log10(ev[ista].z.pha[SIGNAL].amp),  log10(ev[ista].z.pha[NOISE].amp),  ev[ista].z.P2P_snr, 
  ev[ista].ns.pha[NOISE].period, ev[ista].ns.pha[SIGNAL].period, log10(ev[ista].ns.pha[SIGNAL].amp), log10(ev[ista].ns.pha[NOISE].amp), ev[ista].ns.P2P_snr,
  ev[ista].ew.pha[NOISE].period, ev[ista].ew.pha[SIGNAL].period, log10(ev[ista].ew.pha[SIGNAL].amp), log10(ev[ista].ew.pha[NOISE].amp), ev[ista].ew.P2P_snr );

		fprintf( fp_SNR, 
		  "%8s %8s %8s %8s %6.3f %6.3f %7.1f %3.0f %3.0f %7.2f %7.2f %9.5f %9.5f %11.5f\n",
			ev[ista].stnm,
			ev[ista].net,
			ev[ista].loc,
			ev[ista].z.s.kcmpnm,
			ev[ista].lf,
			ev[ista].hf, 
			ev[ista].rdistkm,
			ev[ista].az,
			ev[ista].baz,
			ev[ista].z.pha[NOISE].period,
			ev[ista].z.pha[SIGNAL].period,
			log10(ev[ista].z.pha[SIGNAL].amp),
			log10(ev[ista].z.pha[NOISE].amp),
			ev[ista].z.P2P_snr );

		fprintf( fp_SNR,
                  "%8s %8s %8s %8s %6.3f %6.3f %7.1f %3.0f %3.0f %7.2f %7.2f %9.5f %9.5f %11.5f\n",
                        ev[ista].stnm,
                        ev[ista].net,
			ev[ista].loc,
                        ev[ista].ns.s.kcmpnm,
			ev[ista].lf,
                        ev[ista].hf,
                        ev[ista].rdistkm,
                        ev[ista].az,
                        ev[ista].baz,
			ev[ista].ns.pha[NOISE].period,
			ev[ista].ns.pha[SIGNAL].period,
			log10(ev[ista].ns.pha[SIGNAL].amp),
			log10(ev[ista].ns.pha[NOISE].amp),
			ev[ista].ns.P2P_snr );

		fprintf( fp_SNR,
                  "%8s %8s %8s %8s %6.3f %6.3f %7.1f %3.0f %3.0f %7.2f %7.2f %9.5f %9.5f %11.5f\n",
			ev[ista].stnm,
                        ev[ista].net,
			ev[ista].loc,
			ev[ista].ew.s.kcmpnm,
			ev[ista].lf,
                        ev[ista].hf,
                        ev[ista].rdistkm,
                        ev[ista].az,
                        ev[ista].baz,
			ev[ista].ew.pha[NOISE].period, 
			ev[ista].ew.pha[SIGNAL].period, 
			log10(ev[ista].ew.pha[SIGNAL].amp), 
			log10(ev[ista].ew.pha[NOISE].amp), 
			ev[ista].ew.P2P_snr );

	} /*** loop over ista ***/
	fclose(fp_SNR);

	if(verbose)
	fprintf(stdout, "\n%s: sacdata2inv(): STDOUT program finished. Bye-Bye!\n\n\n", progname );
	fprintf(stderr, "\n%s: sacdata2inv(): STDERR program finished. Bye-Bye!\n\n\n", progname );

	return 0;

} /*** end of main ***/

void Print_Usage( void )
{
	fprintf( stderr, "\nUsage: %s par= path= respdir= [no]dumpsac [no]verbose [no]parallel\n",
		progname );

	fprintf( stderr, "\nREQUIRED PARAMETERS:\n" );

	fprintf( stderr, "\t par=mtinv.par        parameter file\n" );

	fprintf( stderr, "\t path=/my/data/dir    directory path for *.SAC data files\n" );
	fprintf( stderr, "\t\t    DEFAULT is current working directory\n" );

	fprintf( stderr, "\t respdir=/my/resp/dir directory path for SAC POLE ZERO Response files\n" );
	fprintf( stderr, "\t\t    DEFAULT is current working directory\n" );

	fprintf( stderr, "\nOPTIONAL PARAMETERS:\n" );
	fprintf( stderr, "\t [no]verbose         be verbosy DEFAULT is off\n" );
	fprintf( stderr, "\t [no]dumpsac         write out the data as SAC formatted files DEFAULT off\n" );
	fprintf( stderr, "\t [no]parallel        use multithreading, each station runs on seperate thread DEFAULT off\n\n" );

	fprintf( stderr, "\t see file snr.out for Signal-to-noise ratios and period for peak-to-peak amplitudes\n\n" );
}
