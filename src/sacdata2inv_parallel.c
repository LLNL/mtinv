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
#include <unistd.h>
#include <math.h>

#include <pthread.h>    /* POSIX Threads */

#include "../include/mt.h"         /** global datatype and structure declarations **/
#include "../include/nrutil.h"     /** numerical recipes **/

extern char progname[128];

typedef struct thread_data
{
	EventInfo *ev;
	int ista;
	int ienvelope;  /*** envelope is deprecated gene.ichinose 07.01.2024 ***/
	int dumpsac;
	int verbose;
} ThreadData;

void sacdata2inv_parallel( 
	EventInfo *ev,
        int nsta,
        int ienvelope,
        int dumpsac,
        int verbose )
{

/*** pthreads stuff ***/
	pthread_t *thread;
	ThreadData *td;
	pthread_attr_t attr;

/*** misc, func prototypes ***/
	int ista;
	void *sacdata2inv_staproc_thread( void *ptr );

/*********************************************************************/
/*** start the subroutine                                          ***/
/*********************************************************************/

	if(verbose)
	{
	  fprintf( stdout,
	    "%s: %s: %s: nsta=%d ienvelope=%d dumpsac=%d verbose=%d\n",
		progname,
		__FILE__, __func__, 
		nsta,
		ienvelope,
		dumpsac,
		verbose );
	}

	thread = (pthread_t *)malloc( nsta * sizeof(pthread_t) );

	td = malloc( nsta * sizeof(ThreadData) );

	for( ista = 0; ista < nsta; ista++ )
	{
		td[ista].ev         = ev;
		td[ista].ista       = ista;
		td[ista].ienvelope  = ienvelope;
		td[ista].dumpsac    = dumpsac;
		td[ista].verbose    = verbose;
	}

	for( ista = 0; ista < nsta; ista++ )
	{
		if(verbose)
		{
		  fprintf( stderr,
		    "%s: %s: %s: ista=%d stnm=%s net=%s nt=%d dt=%g lf=%g hf=%g npts=%d delta=%g ienvelope=%d dumpsac=%d verbose=%d\n",
			progname,
			__FILE__, __func__, 
			ista,
			td[ista].ev[ista].stnm,
			td[ista].ev[ista].net,
			td[ista].ev[ista].nt,
			td[ista].ev[ista].dt,
			td[ista].ev[ista].lf,
			td[ista].ev[ista].hf,
			td[ista].ev[ista].z.s.npts,
			td[ista].ev[ista].z.s.delta,
                	td[ista].ienvelope, 
                	td[ista].dumpsac, 
                	td[ista].verbose );
		}
	}

/*********************************************************************/
/*** start the pthreads                                            ***/
/*********************************************************************/

	pthread_attr_init( &attr );

	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );

	/* pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED ); */
	
	for( ista = 0; ista < nsta; ista++ )
	{
		fprintf( stderr,
		  "%s: %s: %s: ista=%d stnm=%s net=%s nt=%d dt=%g \n", 
			progname, 
			__FILE__, __func__, 
			ista,
			td[ista].ev[ista].stnm,
			td[ista].ev[ista].net,
			td[ista].ev[ista].nt,
			td[ista].ev[ista].dt );
		fflush( stderr );

		pthread_create( &thread[ista], &attr, sacdata2inv_staproc_thread, &td[ista] );

	} /*** ista ***/

	for( ista = 0; ista < nsta; ista++ )
	{
		/*
		fprintf( stderr,
		  "%s: pthread_join() ista=%d\n", progname, ista );
		fflush( stderr );
		*/
		pthread_join( thread[ista], NULL );
	} 

	pthread_attr_destroy( &attr );
}

/********************************************************************/
/*** loop over each station and load its SAC files for all 3 comp ***/
/********************************************************************/

#define METERS2CM  100

void *sacdata2inv_staproc_thread( void *ptr ) 
{

/*******************************************************/
/*** variable declarations *****************************/
/*******************************************************/
	FILE   *fpout;

	int    old_nt;
	float  precut, twincut;  /*** windowing data ***/
        char   timestring[24], kmarker[4]; /***  misc ***/
        char   taper_type[3];              /*** taper data ***/
        float  taper_frac_width;
	float  angle;    /*** angle of horizontal component rotation ***/
        double drdist, daz, dbaz;
        int    DIFFoperator;

	int irtrend = 1;

/**** function prototype ******/

	/*** misc_tools/scale_data.c ***/
	void  scale_data( float *x, int n, float a );

	/*** rmean.c ***/
	void remove_mean( float *x, int n );

	/*** rtrend/rtrend.c ***/
	void rtrend( float x0, float dx, float *y, int n, int verbose );

	/*** saccut/saccut_subs.c ***/
        void  set_sac_time_marker(
                int mode,
                Sac_File *sacfile,
                char *timestring,
                MyTime *t,
                char *kmarker,
                int verbose );

        /*** saccut/saccut_subs.c ***/
        float *cut_sac(
                Sac_File *sacfile,
                float precut,
                float twincut,
                char *kmarker,
                int verbose );

        /*** transfer/sactransfer.c ***/
        void  transfer_response(
                float *data,
                int npts,
                float delta,
                char *sacpzfile,
                int verbose );

        /*** misc_tools/ampshift.c ***/
        void  ampshift( float *x, int n, int verbose );

        /*** filter/filtersubs.c ***/
        void  iir_filter(
                float *data,
                int nsamps,
                char *filter_type,
                float trbndw,
                float a,
                int iord,
                char *operation_type,
                float flo,
                float fhi,
                float ts,
                int passes );

        /*** tdif/Differentiates.c ***/
        void  differentiate( float *x, int n, float dt, int op, int verbose );

        /*** interpolate/interpolate_subs.c ***/
        void  interpolate_fft(
                float *data,
                int old_npts,
                float old_delta,
                int *new_npts,
                float new_delta );

        /*** taper/taper_subs.c ***/
        void  taper( float *data, int nt, char *taper_type, float taper_frac_width );

        /*** misc_tools/distaz.c ***/
        int   distaz(
                double olat,
                double olon,
                double tlat,
                double tlon,
                double *del,
                double *az,
                double *baz );

        /*** rotate/rotate.c ***/
        void  rotate(   float *ns, int ns_nt, float *ns_cmpaz, float ns_cmpinc,
                        float *ew, int ew_nt, float *ew_cmpaz, float ew_cmpinc,
                        float angle, int verbose );

        /*** sacextrema/sacextrema.c ***/
        void  sac_minmax( float *x, int n, float *max, float *min, float *mean );

        /*** sacdata2inv_subs.c ***/
        void  writesacfile( EventInfo *ev );

/*** added 3/2014 for calculation of SNR ***/

        /*** compute_Peak_to_Peak.c ***/
        void compute_Peak_to_Peak(
                Sac_Header *header,
                float *data,
                float period,
                float vmin,
                float vmax,
                float *amp,
                float *time,
                float *duration,
                int SignalWindowFlag,
                int verbose );

/******************************/
/*** thread Data interface ****/
/******************************/
	ThreadData *td;
	td = (ThreadData *) ptr;

	EventInfo *ev;
	int ista;
	int ienvelope;   /*** envelope is deprecated gene.ichinose 07.01.2024 ***/
	int dumpsac;
	int verbose;

/*******************************************************/
/*************** start subroutine **********************/
/*******************************************************/

	ev         = (EventInfo *) td->ev;
	ista       = td->ista;
	ienvelope  = td->ienvelope;    /*** envelope is deprecated gene.ichinose 07.01.2024 ***/
	dumpsac    = td->dumpsac;
	verbose    = td->verbose;

	if(verbose)
	{
	  fprintf( stdout,
		"%s: %s: %s: ista=%d ienvelope=%d dumpsac=%d verbose=%d\n",
			progname, 
			__FILE__, __func__, 
			ista,
			ienvelope,
			dumpsac,
			verbose );
	  fflush( stdout );
	}

/*******************************************************/
/*** be verbose, send some information to the screen ***/
/*******************************************************/

	if( verbose )
	{
		fprintf( stdout,
		  "%s: %s: %s: ista=%03d sta=%s net=%s data=%s glib=%s ginv=%s\n",
			progname, __FILE__, __func__, 
			ista, 
			ev[ista].stnm, 
			ev[ista].net,
			ev[ista].data_filename, 
			ev[ista].glib_filename,
			ev[ista].ginv_filename );
		fflush( stdout );

		fprintf( stdout,
		  "%s: %s: %s: npole=%d pass=%d lf=%g hf=%g nt=%d dt=%g tr=%g tt=%g\n",
			progname, 
			__FILE__, __func__, 
			ev[ista].npole, 
			ev[ista].npass, 
			ev[ista].lf,
			ev[ista].hf, 
			ev[ista].nt,
			ev[ista].dt, 
			ev[ista].tr, 
			ev[ista].tt );
	  	fflush( stdout );
	}

/******************************************/
/*** convert from meters to centimeters ***/
/******************************************/

	if( verbose )
	{
	  fprintf( stdout, "%s: %s: %s: converting from meters to cm\n", progname, __FILE__, __func__ );
	  fflush( stdout );
	}

	scale_data( &(ev[ista].ew.data[0]), ev[ista].ew.s.npts, METERS2CM );
	scale_data( &(ev[ista].ns.data[0]), ev[ista].ns.s.npts, METERS2CM );
	scale_data( &(ev[ista].z.data[0]),  ev[ista].z.s.npts,  METERS2CM );

/*************************************************/
/*** scale amplitudes my multiplication factor ***/
/*************************************************/

	if( ev[ista].mul_factor != 1.0 )
	{
	  if( verbose )
	  {
		fprintf( stdout,
			"%s: %s: %s: scaling my mul_factor=%g\n", 
			progname,
			__FILE__, __func__, 
			ev[ista].mul_factor );
		fflush( stdout );
	  }

	  scale_data( &(ev[ista].ew.data[0]), ev[ista].ew.s.npts, ev[ista].mul_factor );
	  scale_data( &(ev[ista].ns.data[0]), ev[ista].ns.s.npts, ev[ista].mul_factor );
	  scale_data( &(ev[ista].z.data[0]),  ev[ista].z.s.npts,  ev[ista].mul_factor );
	}

/**************************************/
/*** remove the data trend and mean ***/
/**************************************/

	if(irtrend)
	{
	  if(verbose)
	  {
	    fprintf( stdout,
	      "%s: %s: %s: calling rtrend() ista=%d stnm=%s.%s nt=%d dt=%g b=%g\n",
		progname, __FILE__, __func__, 
		ista,
		ev[ista].stnm,
		ev[ista].net,
		ev[ista].ew.s.npts,
		ev[ista].ew.s.delta,
		ev[ista].ew.s.b );
	    fflush( stdout );
	  }
	  rtrend( 0.0, ev[ista].ew.s.delta, ev[ista].ew.data, ev[ista].ew.s.npts, verbose );
	  rtrend( 0.0, ev[ista].ns.s.delta, ev[ista].ns.data, ev[ista].ns.s.npts, verbose );
	  rtrend( 0.0, ev[ista].z.s.delta,  ev[ista].z.data,  ev[ista].z.s.npts,  verbose );
	}

/********************************************************/
/*** add origin time from input file to time marker o ***/
/********************************************************/

	if( verbose )
	{
	  fprintf( stdout,
	    "%s: %s: %s: calling set_sac_time_marker(): adding origin time to sac files: ", 
			progname, __FILE__, __func__ );
	  WriteMyTime2STDOUT( &(ev[ista].ot) );
	  fflush( stdout );
	}

	strcpy( kmarker, "o" );
	strcpy( timestring, "\0" );

	set_sac_time_marker( 1, &(ev[ista].ew), timestring, &(ev[ista].ot), kmarker, verbose );
	set_sac_time_marker( 1, &(ev[ista].ns), timestring, &(ev[ista].ot), kmarker, verbose );
	set_sac_time_marker( 1, &(ev[ista].z),  timestring, &(ev[ista].ot), kmarker, verbose );

/********************************************************/
/*** modification to accept reduction velocity g.redv ***/
/***  and cut from b=t0=rdist/redv to e=t0+nt*dt o=0  ***/
/********************************************************/

	if( verbose )
	{
		fprintf( stdout, "%s: %s: %s: redv=%g ts0=%g tstart=%g tend=%g\n",
			progname, __FILE__, __func__, 
			ev[ista].redv, 
			ev[ista].ts0,
			ev[ista].tstart, 
			ev[ista].tend );
		fflush( stdout );
	}

/********************************************************/
/*** NEED TO IMPLIMEnt REDV in greens structure       ***/
/********************************************************/

	if( ev[ista].redv > 0 )
	{
		if(verbose)
		{
			fprintf( stdout, "%s: %s: %s: calling set_sac_time_marker(): ",
				progname, __FILE__, __func__ );
			fflush( stdout );

			fprintf( stdout, " adding reduction velocity to sac files rdev=%g rdist=%g\n",
				ev[ista].redv, ev[ista].rdist );
			fflush( stdout );
		}

		clone_mytime( &(ev[ista].ew.ot), &(ev[ista].ew.t1) );
		clone_mytime( &(ev[ista].ns.ot), &(ev[ista].ns.t1) );
		clone_mytime( &(ev[ista].z.ot),  &(ev[ista].z.t1) );

		epoch2time( &(ev[ista].ew.t1), ( ev[ista].ew.ot.epoch + ev[ista].ts0 ) );
		epoch2time( &(ev[ista].ns.t1), ( ev[ista].ns.ot.epoch + ev[ista].ts0 ) );
		epoch2time( &(ev[ista].z.t1),  ( ev[ista].z.ot.epoch  + ev[ista].ts0 ) );

		strcpy( kmarker, "t1" );
		strcpy( timestring, "\0" );

		set_sac_time_marker( 1, &(ev[ista].ew), timestring, &(ev[ista].ew.t1), kmarker, verbose );
		set_sac_time_marker( 1, &(ev[ista].ns), timestring, &(ev[ista].ns.t1), kmarker, verbose );
		set_sac_time_marker( 1, &(ev[ista].z),  timestring, &(ev[ista].z.t1),  kmarker, verbose );
	} 

/************************************************************************/
/*** cut the data at the origin time plus twincut (768 s = 12.8 min ) ***/
/************************************************************************/

	twincut = 900;
	precut  = 120;

	if( ev[ista].redv > 0 )
		strcpy( kmarker, "t1" );
	else
		strcpy( kmarker, "o" );

	if( verbose )
	{
		fprintf( stdout, "%s: %s: %s: calling cut_sac(): cutting %g sec before and\n",
			progname, __FILE__, __func__,  precut );
		fflush( stdout );

		fprintf( stdout, "%s: %s: %s:  %g sec after the origin time marker\n",
			progname, __FILE__, __func__, twincut );
		fflush( stdout );
	}

	ev[ista].ew.data = (float *)cut_sac( &(ev[ista].ew), precut, twincut, kmarker, verbose );
	ev[ista].ns.data = (float *)cut_sac( &(ev[ista].ns), precut, twincut, kmarker, verbose );
	ev[ista].z.data  = (float *)cut_sac( &(ev[ista].z),  precut, twincut, kmarker, verbose );

/**************************************************************/
/*** construct SAC Poles and Zeros instrument response file ***/
/***   and apply correction by convolution (mul spec)       ***/
/**************************************************************/

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: calling transfer_response() \n", 
		progname, __FILE__, __func__ );
	  fflush( stdout );
	}

	transfer_response(
		ev[ista].ew.data, 
		ev[ista].ew.s.npts,
		ev[ista].ew.s.delta,
		ev[ista].ew.sacpzfile,
		verbose );

	transfer_response( 
		ev[ista].ns.data,
		ev[ista].ns.s.npts,
		ev[ista].ns.s.delta,
		ev[ista].ns.sacpzfile,
		verbose );

	transfer_response(
		ev[ista].z.data,
		ev[ista].z.s.npts,
		ev[ista].z.s.delta,
		ev[ista].z.sacpzfile,
		verbose );

/**************************************************/
/*** remove step function at first sample point ***/
/*** perhaps a taper is a better method?        ***/
/**************************************************/

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: calling ampshift() \n", 
                progname, __FILE__, __func__ );
          fflush( stdout );
	}

	ampshift( ev[ista].ew.data, ev[ista].ew.s.npts, verbose );
	ampshift( ev[ista].ns.data, ev[ista].ns.s.npts, verbose );
	ampshift( ev[ista].z.data,  ev[ista].z.s.npts, verbose );

/*******************************************************************************/
/*** bandpass filter using Butterworth filter user defined poles and corners ***/
/*******************************************************************************/

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: bandpass filtering data \n",
		progname, __FILE__, __func__ );
	  fflush( stdout );
	}

	iir_filter(
		ev[ista].ew.data,
		ev[ista].ew.s.npts,
		"BU",
		ev[ista].trbndw,
		ev[ista].a,
		ev[ista].npole,
		"BP",
		ev[ista].lf,
		ev[ista].hf,
		ev[ista].ew.s.delta,
		ev[ista].npass );

	iir_filter(
		ev[ista].ns.data,
		ev[ista].ns.s.npts,
		"BU",
		ev[ista].trbndw,
		ev[ista].a,
		ev[ista].npole,
		"BP",
		ev[ista].lf,
		ev[ista].hf,
		ev[ista].ns.s.delta,
		ev[ista].npass );

	iir_filter(
		ev[ista].z.data,
		ev[ista].z.s.npts,
		"BU",
		ev[ista].trbndw,
		ev[ista].a,
		ev[ista].npole,
		"BP",
		ev[ista].lf,
		ev[ista].hf,
		ev[ista].z.s.delta,
		ev[ista].npass );

/*********************************************************************************/
/*** convert output to velocity (multiply by omega) or displacement do nothing ***/
/*********************************************************************************/

	if( ev[ista].grd_mo_type == VELOCITY )
	{
		if(verbose)
                {
                  fprintf( stdout,
			"%s: %s: %s: converting data to velocity\n", progname, __FILE__, __func__ );
		  fflush( stdout );
                }

		DIFFoperator = 3;
		differentiate( ev[ista].ew.data, ev[ista].ew.s.npts, ev[ista].ew.s.delta, DIFFoperator, verbose );
		differentiate( ev[ista].ns.data, ev[ista].ns.s.npts, ev[ista].ns.s.delta, DIFFoperator, verbose );
		differentiate( ev[ista].z.data,  ev[ista].z.s.npts,  ev[ista].z.s.delta,  DIFFoperator, verbose );
	}

/*******************************************************************************/
/*** the final cut, this time on the origin                                  ***/
/*** need to add reduction velocity to Greens function                       ***/
/*******************************************************************************/

	if( ev[ista].redv > 0 )
		strcpy( kmarker, "t1" );
	else
		strcpy( kmarker, "o" );

	precut  = 0;
	twincut = ev[ista].nt * ev[ista].dt + 60;

	ev[ista].ew.data = (float *)cut_sac( &(ev[ista].ew), precut, twincut, kmarker, verbose );
	ev[ista].ns.data = (float *)cut_sac( &(ev[ista].ns), precut, twincut, kmarker, verbose );
	ev[ista].z.data  = (float *)cut_sac( &(ev[ista].z),  precut, twincut, kmarker, verbose );

	epoch2time( &(ev[ista].ew.end), (ev[ista].ew.beg.epoch + ev[ista].nt * ev[ista].dt) );
	epoch2time( &(ev[ista].ns.end), (ev[ista].ns.beg.epoch + ev[ista].nt * ev[ista].dt) );
	epoch2time( &(ev[ista].z.end),  (ev[ista].z.beg.epoch  + ev[ista].nt * ev[ista].dt) );
                                                                                                                                                              
/*******************************************************************************/
/*** interpolate data to new nt and dt                                       ***/
/*******************************************************************************/

	if(verbose)
	{
	  fprintf( stdout, "%s: sacdata2inv_staproc_thread() interpolate the data\n", progname );
	  fflush( stdout );
	}

	if( ev[ista].dt >= ev[ista].z.s.delta )
	{
		interpolate_fft( ev[ista].ew.data, ev[ista].ew.s.npts, ev[ista].ew.s.delta, &old_nt, ev[ista].dt );
		interpolate_fft( ev[ista].ns.data, ev[ista].ns.s.npts, ev[ista].ns.s.delta, &old_nt, ev[ista].dt );
		interpolate_fft( ev[ista].z.data,  ev[ista].z.s.npts,  ev[ista].z.s.delta,  &old_nt, ev[ista].dt );
                                                                                                                                                                
		ev[ista].ew.s.npts  = ev[ista].nt;
		ev[ista].ew.s.delta = ev[ista].dt;
		ev[ista].ns.s.npts  = ev[ista].nt;
		ev[ista].ns.s.delta = ev[ista].dt;
		ev[ista].z.s.npts   = ev[ista].nt;
		ev[ista].z.s.delta  = ev[ista].dt;
	}
	else
	{
	/*** never be here. should of check this in loadsacdata ***/
		fprintf( stderr, "%s: %s: %s: Error reset dt\n", progname, __FILE__, __func__ );
		fflush(stderr);
	}

/****************************************************************************************/
/*** taper the ends using Hanning taper width = 0.1 to 0.3 depending on sampling rate ***/
/****************************************************************************************/

	strcpy( taper_type, "h\0" );
        taper_frac_width = 0.05; /*** changed from taper_fac_width=0.2 March2014 G.A.I for SNR window ***/

        taper( ev[ista].ew.data, ev[ista].ew.s.npts, taper_type, taper_frac_width );
        taper( ev[ista].ns.data, ev[ista].ns.s.npts, taper_type, taper_frac_width );
        taper( ev[ista].z.data,  ev[ista].z.s.npts,  taper_type, taper_frac_width );

/****************************************************************************************/
/*** reset the azimuth values                                                         ***/
/****************************************************************************************/

	/* if( ev[ista].ns.s.az == -12345. || ev[ista].ew.s.az == -12345. ) */

	ev[ista].ns.s.evla = ev[ista].evla;
	ev[ista].ns.s.evlo = ev[ista].evlo;
	ev[ista].ew.s.evla = ev[ista].evla;
	ev[ista].ew.s.evla = ev[ista].evlo;
	ev[ista].z.s.evla  = ev[ista].evla;
	ev[ista].z.s.evlo  = ev[ista].evlo;
                                                                                                                                                                
	distaz( (double)ev[ista].evla,      (double)ev[ista].evlo,
       		(double)ev[ista].ns.s.stla, (double)ev[ista].ns.s.stlo,
		&drdist, &daz, &dbaz );

	ev[ista].ns.s.dist = (float)drdist;
	ev[ista].ns.s.gcarc= (float)(drdist/111.13);
	ev[ista].ns.s.az   = (float)daz;
	ev[ista].ns.s.baz  = (float)dbaz;

	ev[ista].ew.s.dist = (float)drdist;
	ev[ista].ew.s.gcarc= (float)(drdist/111.13);
	ev[ista].ew.s.az   = (float)daz;
	ev[ista].ew.s.baz  = (float)dbaz;
        
	ev[ista].z.s.dist = (float)drdist;
	ev[ista].z.s.gcarc= (float)(drdist/111.13);
	ev[ista].z.s.az   = (float)daz;
	ev[ista].z.s.baz  = (float)dbaz;
       
	if( verbose )
	{
 	  fprintf(stdout,
	    "%s: sacdata2inv_staproc_thread(): Resetting distaz values ista=%d sta.net=%s.%s dist=%.1f az=%.1f baz=%.1f\n",
		progname,
		ista,
		ev[ista].stnm,
		ev[ista].net,
		ev[ista].ns.s.dist,
		ev[ista].ns.s.az,
		ev[ista].ns.s.baz );
	  fflush( stdout );
	}

	ev[ista].rdistkm = ev[ista].ns.s.dist;
	ev[ista].az      = ev[ista].ns.s.az;
	ev[ista].baz     = ev[ista].ns.s.baz;

/****************************************************************************************/
/*** before rotating the horizontals to the great circle path first rotate to pure NS ***/
/***  EW coordinate system                                                            ***/
/****************************************************************************************/


/****************************************************************************************/
/*** do not rotate the rotational data                                                ***/
/****************************************************************************************/

	if( strcmp( ev[ista].wavetype, "Surf/Pnl" ) == 0 )
	{

	  if( ev[ista].ns.s.cmpaz != 0. && ev[ista].ew.s.cmpaz != 90. )
	  {
		if(verbose)
		{
		  fprintf( stdout, "%s: sacdata2inv_staproc_thread() ista=%d horizontals az not 0 and 90. ", 
			progname, ista );
		  fflush( stdout );

		  fprintf( stdout, " az->(ns=%g,ew=%g) Rotating NS to 0 and EW to 90\n",
			ev[ista].ns.s.cmpaz, ev[ista].ew.s.cmpaz );
		  fflush( stdout );
		}

		angle = 360 - ev[ista].ns.s.cmpaz;
		rotate( ev[ista].ns.data, ev[ista].ns.s.npts, &(ev[ista].ns.s.cmpaz), ev[ista].ns.s.cmpinc,
                        ev[ista].ew.data, ev[ista].ew.s.npts, &(ev[ista].ew.s.cmpaz), ev[ista].ew.s.cmpinc,
                        angle, verbose );
	  }

/***********************************************************************/
/*** rotate the horizontals to get radial and transverse             ***/
/*** radial component is written over the ns                         ***/
/*** transverse/tangential component is written over the ew          ***/
/***********************************************************************/

	  /* angle = ev[ista].ns.s.az; */

	  angle = ev[ista].ns.s.baz + 180;

	  rotate( ev[ista].ns.data, ev[ista].ns.s.npts, &(ev[ista].ns.s.cmpaz), ev[ista].ns.s.cmpinc,
                ev[ista].ew.data, ev[ista].ew.s.npts, &(ev[ista].ew.s.cmpaz), ev[ista].ew.s.cmpinc,
                angle, verbose );

	  if(verbose)
	  {
		fprintf( stdout, 
		  "%s: sacdata2inv_staproc_thread(): ista=%d horizontals rotated by angle=%g\n",
                     progname, ista, angle );
		fflush( stdout );
	  }

	}  /*** rotate only if ev->wavetype == Surf/Pnl ... NOT Rotational! ***/

/***********************************************************************/
/*** compute peak to peak of signal and noise to measure SNR         ***/
/***********************************************************************/

/*** ew ***/
	ev[ista].ew.pha[NOISE].gvlo = 8.5;  ev[ista].ew.pha[NOISE].gvhi = 999.9;
	ev[ista].ns.pha[NOISE].gvlo = 8.5;  ev[ista].ns.pha[NOISE].gvhi = 999.9;
	ev[ista].z.pha[NOISE].gvlo  = 8.5;  ev[ista].z.pha[NOISE].gvhi = 999.9;

	ev[ista].ew.pha[SIGNAL].gvlo = 1.5;  ev[ista].ew.pha[SIGNAL].gvhi = 7.0;
	ev[ista].ns.pha[SIGNAL].gvlo = 1.5;  ev[ista].ns.pha[SIGNAL].gvhi = 7.0;
	ev[ista].z.pha[SIGNAL].gvlo  = 1.5;  ev[ista].z.pha[SIGNAL].gvhi = 7.0;
                                                                                                                                                              
	compute_Peak_to_Peak( &(ev[ista].ew.s), ev[ista].ew.data, 1/ev[ista].lf,
        	ev[ista].ew.pha[NOISE].gvlo,  ev[ista].ew.pha[NOISE].gvhi,
        	&(ev[ista].ew.pha[NOISE].amp),  &(ev[ista].ew.pha[NOISE].time),
        	&(ev[ista].ew.pha[NOISE].duration), NOISE, verbose );

	if(verbose)
	fprintf( stderr, "%s: %s: %s: noise-EW-ista=%d per=%g gvlo=%g gvhi=%g amp=%g time=%g dur=%g\n",
		progname, __FILE__, __func__, ista, (1/ev[ista].lf), 
		ev[ista].ew.pha[NOISE].gvlo,  
		ev[ista].ew.pha[NOISE].gvhi, 
		ev[ista].ew.pha[NOISE].amp,
		ev[ista].ew.pha[NOISE].time, 
		ev[ista].ew.pha[NOISE].duration );

	compute_Peak_to_Peak( &(ev[ista].ew.s), ev[ista].ew.data, 1/ev[ista].lf,
        	ev[ista].ew.pha[SIGNAL].gvlo, ev[ista].ew.pha[SIGNAL].gvhi,
        	&(ev[ista].ew.pha[SIGNAL].amp), &(ev[ista].ew.pha[SIGNAL].time),
        	&(ev[ista].ew.pha[SIGNAL].duration), SIGNAL, verbose );

	if(verbose)
	fprintf( stderr, "%s: %s: %s: signal-EW-ista=%d per=%g gvlo=%g gvhi=%g amp=%g time=%g dur=%g\n",
                progname, __FILE__, __func__, ista, (1/ev[ista].lf),
                ev[ista].ew.pha[SIGNAL].gvlo,
                ev[ista].ew.pha[SIGNAL].gvhi,
                ev[ista].ew.pha[SIGNAL].amp,
                ev[ista].ew.pha[SIGNAL].time,
                ev[ista].ew.pha[SIGNAL].duration );

	ev[ista].ew.pha[NOISE].period     = 2 * ev[ista].ew.pha[NOISE].duration;
	ev[ista].ew.pha[NOISE].frequency  = 1 / ev[ista].ew.pha[NOISE].period;
	ev[ista].ew.pha[SIGNAL].period    = 2 * ev[ista].ew.pha[SIGNAL].duration;
	ev[ista].ew.pha[SIGNAL].frequency = 1 / ev[ista].ew.pha[SIGNAL].period;
	ev[ista].ew.P2P_snr               = ev[ista].ew.pha[SIGNAL].amp /
                                            ev[ista].ew.pha[NOISE].amp;

/*** ns ***/
	compute_Peak_to_Peak( &(ev[ista].ns.s), ev[ista].ns.data, 1/ev[ista].lf,
        	ev[ista].ns.pha[NOISE].gvlo, ev[ista].ns.pha[NOISE].gvhi,
        	&(ev[ista].ns.pha[NOISE].amp), &(ev[ista].ns.pha[NOISE].time),
        	&(ev[ista].ns.pha[NOISE].duration), NOISE, verbose );

	compute_Peak_to_Peak( &(ev[ista].ns.s), ev[ista].ns.data, 1/ev[ista].lf,
        	ev[ista].ns.pha[SIGNAL].gvlo, ev[ista].ns.pha[SIGNAL].gvhi,
        	&(ev[ista].ns.pha[SIGNAL].amp), &(ev[ista].ns.pha[SIGNAL].time),
        	&(ev[ista].ns.pha[SIGNAL].duration), SIGNAL, verbose );

	ev[ista].ns.pha[NOISE].period     = 2 * ev[ista].ns.pha[NOISE].duration;
	ev[ista].ns.pha[NOISE].frequency  = 1 / ev[ista].ns.pha[NOISE].period;
	ev[ista].ns.pha[SIGNAL].period    = 2 * ev[ista].ns.pha[SIGNAL].duration;
	ev[ista].ns.pha[SIGNAL].frequency = 1 / ev[ista].ns.pha[SIGNAL].period;
	ev[ista].ns.P2P_snr               = ev[ista].ns.pha[SIGNAL].amp /
                                            ev[ista].ns.pha[NOISE].amp;

/*** z ***/
	compute_Peak_to_Peak( &(ev[ista].z.s), ev[ista].z.data, 1/ev[ista].lf,
        	ev[ista].z.pha[NOISE].gvlo,  ev[ista].z.pha[NOISE].gvhi,
        	&(ev[ista].z.pha[NOISE].amp), &(ev[ista].z.pha[NOISE].time),
        	&(ev[ista].z.pha[NOISE].duration), NOISE,  verbose );

	compute_Peak_to_Peak( &(ev[ista].z.s), ev[ista].z.data, 1/ev[ista].lf,
        	ev[ista].z.pha[SIGNAL].gvlo, ev[ista].z.pha[SIGNAL].gvhi,
        	&(ev[ista].z.pha[SIGNAL].amp), &(ev[ista].z.pha[SIGNAL].time),
        	&(ev[ista].z.pha[SIGNAL].duration), SIGNAL, verbose );
                                                                                                                                                                
	ev[ista].z.pha[NOISE].period     = 2 * ev[ista].z.pha[NOISE].duration;
	ev[ista].z.pha[NOISE].frequency  = 1 / ev[ista].z.pha[NOISE].period;
	ev[ista].z.pha[SIGNAL].period    = 2 * ev[ista].z.pha[SIGNAL].duration;
	ev[ista].z.pha[SIGNAL].frequency = 1 / ev[ista].z.pha[SIGNAL].period;
	ev[ista].z.P2P_snr               = ev[ista].z.pha[SIGNAL].amp /
	                                   ev[ista].z.pha[NOISE].amp;

/********************************************************/
/*** compute min max and mean extrema                 ***/
/********************************************************/

	sac_minmax( ev[ista].z.data,  ev[ista].z.s.npts,
		&(ev[ista].z.s.depmax),  &(ev[ista].z.s.depmin),  &(ev[ista].z.s.depmen) );
	sac_minmax( ev[ista].ew.data, ev[ista].ew.s.npts,
		&(ev[ista].ew.s.depmax), &(ev[ista].ew.s.depmin), &(ev[ista].ew.s.depmen) );
	sac_minmax( ev[ista].ns.data, ev[ista].ns.s.npts,
		&(ev[ista].ns.s.depmax), &(ev[ista].ns.s.depmin), &(ev[ista].ns.s.depmen) );

	if(verbose )
	{
	  fprintf( stdout, "%s:\t sacdata2inv_staproc_thread(): ista=%d %s.%s Z  min=%g max=%g mean=%g\n",
		progname, ista, ev[ista].stnm, ev[ista].net, ev[ista].z.s.depmin,
		ev[ista].z.s.depmax,  ev[ista].z.s.depmen );
 	  fflush( stdout );

	  fprintf( stdout, "%s:\t sacdata2inv_staproc_thread(): ista=%d %s.%s NS min=%g max=%g mean=%g\n",
		progname, ista, ev[ista].stnm, ev[ista].net, ev[ista].ns.s.depmin,
		ev[ista].ns.s.depmax, ev[ista].ns.s.depmen );
          fflush( stdout );
                                                                                                                                                       
	  fprintf( stdout, "%s:\t sacdata2inv_staproc_thread(): ista=%d %s.%s EW min=%g max=%g mean=%g\n",
		progname, ista, ev[ista].stnm, ev[ista].net, ev[ista].ew.s.depmin,
		ev[ista].ew.s.depmax, ev[ista].ew.s.depmen );
	  fflush( stdout );
	}

/****************************************************************************************/
/*** write out individual SAC binary formated the files for inspection                ***/
/****************************************************************************************/

	if( dumpsac ) 
	{
		writesacfile( &ev[ista] );
	}

/****************************************************************************************/
/*** write a *.data file for inversion                                                ***/
/****************************************************************************************/

	fprintf( stdout, "%s: sacdata2inv_staproc_thread(): writting file %s\n",
		progname, ev[ista].data_filename );

	fpout = fopen( ev[ista].data_filename, "w" );
	fwrite( &(ev[ista]), sizeof(EventInfo), 1, fpout );
	fwrite( &(ev[ista].ew.data[0]), ev[ista].ew.s.npts * sizeof(float), 1, fpout );
	fwrite( &(ev[ista].ns.data[0]), ev[ista].ns.s.npts * sizeof(float), 1, fpout );
	fwrite( &(ev[ista].z.data[0]),  ev[ista].z.s.npts  * sizeof(float), 1, fpout );
	fclose( fpout );

/****************************************************************************************/
/*** clean up                                                                         ***/
/****************************************************************************************/

/*****
	if(verbose)
	{
		fprintf( stdout, "%s: sacdata2inv_staproc_thread(): cleaning up memory\n", progname );
		fflush( stdout );
	}
	free(ev[ista].ew.data);
	free(ev[ista].ns.data);
	free(ev[ista].z.data);
****/

	if(verbose)
	{
	  fprintf( stdout, "%s: STDOUT sacdata2inv_staproc_thread(): done with ista=%d\n\n\n", progname, ista );
	  fprintf( stdout, "\n\n\n" );
	  fflush( stdout );
	}
	pthread_exit((void *)0);
}
