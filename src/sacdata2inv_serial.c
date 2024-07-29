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

/********************************************************************/
/*** loop over each station and load its SAC files for all 3 comp ***/
/********************************************************************/

#define MAXDATAPTS 1000000.0
#define METERS2CM  100.0

/*******************
for( ista=0; ista<nsta; ista++ ) { 
                sacdata2inv_serial( &(ev[ista]), ...
 access ev-> 
********************/

void sacdata2inv_serial(
		EventInfo *ev, 
		int ista,
		int ienvelope,    /*** envelope is deprecated gene.ichinose 07.01.2024 ***/
		int dumpsac,
		int verbose )
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
	int    irtrend = 1;

/**** function prototype ******/

	/*** misc_tools/scale_data.c ***/
	void  scale_data( float *x, int n, float a );

	/*** sacio.c ***/
	void sac_minmax( float *x, int n, float *max, float *min, float *mean );
	void set_sac_minmax( Sac_Header *s, float *data );

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

/*******************************************************/
/*************** start subroutine **********************/
/*******************************************************/

/*******************************************************/
/*** be verbose, send some information to the screen ***/
/*******************************************************/

	if( verbose )
	{
		fprintf( stdout,
		  "%s: %s: %s: ista=%03d sta=%s net=%s data=%s glib=%s ginv=%s\n",
			progname, __FILE__, __func__,
			ista, 
			ev->stnm, 
			ev->net,
			ev->data_filename, 
			ev->glib_filename,
			ev->ginv_filename );

		fprintf( stdout,
		  "%s: %s: %s: npole=%d pass=%d lf=%g hf=%g nt=%d dt=%g tr=%g tt=%g\n",
			progname, __FILE__, __func__,
			ev->npole, 
			ev->npass, 
			ev->lf,
			ev->hf, 
			ev->nt,
			ev->dt, 
			ev->tr, 
			ev->tt );

		fflush(stdout);
	}

/******************************************/
/*** convert from meters to centimeters ***/
/*** don't do this for rotational data? ***/
/******************************************/

	if( verbose )
	{
	   fprintf( stdout, "%s: %s: %s: converting from meters to cm METERS2CM=%lf\n", 
		progname, __FILE__, __func__, METERS2CM );
	   fflush(stdout);
	}

	/*
	set_sac_minmax( &(ev->ew.s), ev->ew.data );
        set_sac_minmax( &(ev->ns.s), ev->ns.data );
        set_sac_minmax( &(ev->z.s),  ev->z.data  );

	fprintf( stdout, "%s: %s: %s: (ew) min=%g max=%g (ns) min=%g max=%g (z) min=%g max=%g\n",
                progname, __FILE__, __func__,
                ev->ew.s.depmin, ev->ew.s.depmax,
                ev->ns.s.depmin, ev->ns.s.depmax,
                ev->z.s.depmin, ev->z.s.depmax );

        fflush( stdout );
	*/

/*** when subroutine was named "fmul()" linux system did not recongnized function call ***/
/*** so we renamed fmul to scale_data -- very wierd stuff with gcc-linux! ***/

	scale_data( ev->ew.data, ev->ew.s.npts, METERS2CM );
	scale_data( ev->ns.data, ev->ns.s.npts, METERS2CM );
	scale_data( ev->z.data,  ev->z.s.npts,  METERS2CM );

	set_sac_minmax( &(ev->ew.s), ev->ew.data );
	set_sac_minmax( &(ev->ns.s), ev->ns.data );
	set_sac_minmax( &(ev->z.s),  ev->z.data  );

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: (ew) min=%g max=%g (ns) min=%g max=%g (z) min=%g max=%g\n",
		progname, __FILE__, __func__,
		ev->ew.s.depmin, ev->ew.s.depmax,
		ev->ns.s.depmin, ev->ns.s.depmax,
		ev->z.s.depmin, ev->z.s.depmax );
	  fflush( stdout );
	}

/*************************************************/
/*** scale amplitudes my multiplication factor ***/
/*************************************************/

	if( ev->mul_factor != 1.0 )
	{
	  if( verbose )
	  {
		fprintf( stdout,
			"%s: %s: %s: scaling my mul_factor=%g\n", 
			progname, __FILE__, __func__,
			ev->mul_factor );
		fflush(stdout);
	  }

	  scale_data( &(ev->ew.data[0]), ev->ew.s.npts, ev->mul_factor );
	  scale_data( &(ev->ns.data[0]), ev->ns.s.npts, ev->mul_factor );
	  scale_data( &(ev->z.data[0]),  ev->z.s.npts,  ev->mul_factor );
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
		ev->stnm,
		ev->net,
		ev->ew.s.npts,
		ev->ew.s.delta,
		ev->ew.s.b );
	    fflush(stdout);
	  }

	  rtrend( 0.0, ev->ew.s.delta, ev->ew.data, ev->ew.s.npts, verbose );
          rtrend( 0.0, ev->ns.s.delta, ev->ns.data, ev->ns.s.npts, verbose );
          rtrend( 0.0, ev->z.s.delta,  ev->z.data,  ev->z.s.npts,  verbose );
	}

/********************************************************/
/*** add origin time from input file to time marker o ***/
/********************************************************/

	if( verbose )
	{
	  fprintf( stdout,
	    "%s: %s: %s: calling set_sac_time_marker(): adding origin time to sac files: ", 
			progname,  __FILE__, __func__ );
	  fflush(stdout);
	  WriteMyTime2STDOUT( &(ev->ot) );
	  fflush(stdout);
	}

	strcpy( kmarker, "o" );
	strcpy( timestring, "\0" );

	set_sac_time_marker( 1, &(ev->ew), timestring, &(ev->ot), kmarker, verbose );
	set_sac_time_marker( 1, &(ev->ns), timestring, &(ev->ot), kmarker, verbose );
	set_sac_time_marker( 1, &(ev->z),  timestring, &(ev->ot), kmarker, verbose );

/********************************************************/
/*** modification to accept reduction velocity g.redv ***/
/***  and cut from b=t0=rdist/redv to e=t0+nt*dt o=0  ***/
/********************************************************/

	if( verbose )
	{
	  fprintf( stdout, "%s: %s: %s: redv=%g ts0=%g tstart=%g tend=%g rdist=%g\n",
			progname, __FILE__, __func__,
			ev->redv, 
			ev->ts0,
			ev->tstart, 
			ev->tend,
			ev->rdist );
	  fflush(stdout);
	}

/********************************************************/
/*** NEED TO IMPLIMEnt REDV in greens structure       ***/
/********************************************************/

	if( ev->redv > 0 )
	{
		if(verbose)
		{
			fprintf( stdout, "%s: %s: %s: calling set_sac_time_marker(): ",
				progname,  __FILE__, __func__ );
			fprintf( stdout, " adding reduction velocity to sac files rdev=%g rdist=%g\n",
				ev->redv, ev->rdist );
			fflush(stdout);
		}

		clone_mytime( &(ev->ew.ot), &(ev->ew.t1) );
		clone_mytime( &(ev->ns.ot), &(ev->ns.t1) );
		clone_mytime( &(ev->z.ot),  &(ev->z.t1) );

		epoch2time( &(ev->ew.t1), ( ev->ew.ot.epoch + ev->ts0 ) );
		epoch2time( &(ev->ns.t1), ( ev->ns.ot.epoch + ev->ts0 ) );
		epoch2time( &(ev->z.t1),  ( ev->z.ot.epoch  + ev->ts0 ) );

		strcpy( kmarker, "t1" );
		strcpy( timestring, "\0" );

		set_sac_time_marker( 1, &(ev->ew), timestring, &(ev->ew.t1), kmarker, verbose );
		set_sac_time_marker( 1, &(ev->ns), timestring, &(ev->ns.t1), kmarker, verbose );
		set_sac_time_marker( 1, &(ev->z),  timestring, &(ev->z.t1),  kmarker, verbose );

	} /*** if redv > 0 ***/

/************************************************************************/
/*** cut the data at the origin time plus twincut (768 s = 12.8 min ) ***/
/************************************************************************/

	twincut = 900;
	precut  = 120;

	if( ev->redv > 0 )
		strcpy( kmarker, "t1" );
	else
		strcpy( kmarker, "o" );

	if( verbose )
	{
		fprintf( stdout, "%s: %s: %s: calling cut_sac(): cutting %g sec before and\n",
			progname, __FILE__, __func__, precut );
		fflush(stdout);

		fprintf( stdout, "%s: %s: %s:   %g sec after the origin time marker\n",
			progname, __FILE__, __func__, twincut );
		fflush(stdout);
	}

	ev->ew.data = (float *)cut_sac( &(ev->ew), precut, twincut, kmarker, verbose );
	ev->ns.data = (float *)cut_sac( &(ev->ns), precut, twincut, kmarker, verbose );
	ev->z.data  = (float *)cut_sac( &(ev->z),  precut, twincut, kmarker, verbose );

/**************************************************************/
/*** construct SAC Poles and Zeros instrument response file ***/
/***   and apply correction by convolution (mul spec)       ***/
/**************************************************************/

	transfer_response(
		ev->ew.data, 
		ev->ew.s.npts,
		ev->ew.s.delta,
		ev->ew.sacpzfile,
		verbose );

	transfer_response( 
		ev->ns.data,
		ev->ns.s.npts,
		ev->ns.s.delta,
		ev->ns.sacpzfile,
		verbose );

	transfer_response(
		ev->z.data,
		ev->z.s.npts,
		ev->z.s.delta,
		ev->z.sacpzfile,
		verbose );

/**************************************************/
/*** remove step function at first sample point ***/
/*** perhaps a taper is a better method?        ***/
/**************************************************/
	ampshift( ev->ew.data, ev->ew.s.npts, verbose );
	ampshift( ev->ns.data, ev->ns.s.npts, verbose );
	ampshift( ev->z.data,  ev->z.s.npts, verbose );

/*******************************************************************************/
/*** bandpass filter using Butterworth filter user defined poles and corners ***/
/*******************************************************************************/

	if(verbose)
	{
		fprintf( stdout, "%s: %s: %s: bandpass filtering data \n", 
			progname, __FILE__, __func__ );
		fflush(stdout);
	}

	iir_filter(
		ev->ew.data,
		ev->ew.s.npts,
		"BU",
		ev->trbndw,
		ev->a,
		ev->npole,
		"BP",
		ev->lf,
		ev->hf,
		ev->ew.s.delta,
		ev->npass );

	iir_filter(
		ev->ns.data,
		ev->ns.s.npts,
		"BU",
		ev->trbndw,
		ev->a,
		ev->npole,
		"BP",
		ev->lf,
		ev->hf,
		ev->ns.s.delta,
		ev->npass );

	iir_filter(
		ev->z.data,
		ev->z.s.npts,
		"BU",
		ev->trbndw,
		ev->a,
		ev->npole,
		"BP",
		ev->lf,
		ev->hf,
		ev->z.s.delta,
		ev->npass );

/*********************************************************************************/
/*** convert output to velocity (multiply by omega) or displacement do nothing ***/
/*********************************************************************************/

	if( ev->grd_mo_type == VELOCITY )
	{
		if(verbose)
                {
                   fprintf( stdout,
		     "%s: %s: %s: converting data to velocity\n", 
			progname, __FILE__, __func__ );
		   fflush(stdout);
                }

		DIFFoperator = 3;
		differentiate( ev->ew.data, ev->ew.s.npts, ev->ew.s.delta, DIFFoperator, verbose );
		differentiate( ev->ns.data, ev->ns.s.npts, ev->ns.s.delta, DIFFoperator, verbose );
		differentiate( ev->z.data,  ev->z.s.npts,  ev->z.s.delta,  DIFFoperator, verbose );
	}

/*******************************************************************************/
/*** the final cut, this time on the origin                                  ***/
/*******************************************************************************/

/*** need to add reduction velocity to Greens function ***/

	if( ev->redv > 0 )
		strcpy( kmarker, "t1" );
	else
		strcpy( kmarker, "o" );

	precut  = 0;
	twincut = ev->nt * ev->dt + 60;

	ev->ew.data = (float *)cut_sac( &(ev->ew), precut, twincut, kmarker, verbose );
	ev->ns.data = (float *)cut_sac( &(ev->ns), precut, twincut, kmarker, verbose );
	ev->z.data  = (float *)cut_sac( &(ev->z),  precut, twincut, kmarker, verbose );

	epoch2time( &(ev->ew.end), (ev->ew.beg.epoch + ev->nt * ev->dt) );
	epoch2time( &(ev->ns.end), (ev->ns.beg.epoch + ev->nt * ev->dt) );
	epoch2time( &(ev->z.end),  (ev->z.beg.epoch  + ev->nt * ev->dt) );

/*******************************************************************************/
/*** interpolate data to new nt and dt                                       ***/
/*******************************************************************************/

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: interpolate the data\n", 
		progname, __FILE__, __func__ );
	  fflush(stdout);
	}

/*** ensure that ev->dt < ev->z.s.delta ***/

	if( ev->dt >= ev->z.s.delta ) 
	{
		interpolate_fft( ev->ew.data, ev->ew.s.npts, ev->ew.s.delta, &old_nt, ev->dt );
		interpolate_fft( ev->ns.data, ev->ns.s.npts, ev->ns.s.delta, &old_nt, ev->dt );
		interpolate_fft( ev->z.data,  ev->z.s.npts,  ev->z.s.delta,  &old_nt, ev->dt );
	
		ev->ew.s.npts  = ev->nt;
		ev->ew.s.delta = ev->dt;
		ev->ns.s.npts  = ev->nt;
		ev->ns.s.delta = ev->dt;
		ev->z.s.npts   = ev->nt;
		ev->z.s.delta  = ev->dt;
	}
	else
	{
	/*** never be here. should have check this in loadsacdata ***/
		fprintf(stderr, "%s: %s: %s: Error reset dt\n", progname, __FILE__, __func__ );
		fflush(stderr);
		exit(-1);
	}

/****************************************************************************************/
/*** taper the ends using Hanning taper width = 0.1 to 0.3 depending on sampling rate ***/
/****************************************************************************************/

	strcpy( taper_type, "h\0" );

/***
        taper_frac_width = 0.40;
        if( ev->z.s.delta < 0.85 ) taper_frac_width = 0.25;
        if( ev->z.s.delta < 0.50 ) taper_frac_width = 0.10;
***/
        taper_frac_width = 0.05; /*** changed from taper_fac_width=0.2 March2014 G.A.I for SNR window ***/

        taper( ev->ew.data, ev->ew.s.npts, taper_type, taper_frac_width );
        taper( ev->ns.data, ev->ns.s.npts, taper_type, taper_frac_width );
        taper( ev->z.data,  ev->z.s.npts,  taper_type, taper_frac_width );

/****************************************************************************************/
/*** reset the azimuth values                                                         ***/
/****************************************************************************************/

	/* if( ev->ns.s.az == -12345. || ev->ew.s.az == -12345. ) */

	ev->ns.s.evla = ev->evla;
	ev->ns.s.evlo = ev->evlo;
	ev->ew.s.evla = ev->evla;
	ev->ew.s.evla = ev->evlo;
	ev->z.s.evla  = ev->evla;
	ev->z.s.evlo  = ev->evlo;

	distaz( (double)ev->evla,      (double)ev->evlo,
       		(double)ev->ns.s.stla, (double)ev->ns.s.stlo,
		&drdist, &daz, &dbaz );
 
	ev->ns.s.dist = (float)drdist;
	ev->ns.s.gcarc= (float)(drdist/111.13);
	ev->ns.s.az   = (float)daz;
	ev->ns.s.baz  = (float)dbaz;

	ev->ew.s.dist = (float)drdist;
	ev->ew.s.gcarc= (float)(drdist/111.13);
	ev->ew.s.az   = (float)daz;
	ev->ew.s.baz  = (float)dbaz;

	ev->z.s.dist = (float)drdist;
	ev->z.s.gcarc= (float)(drdist/111.13);
	ev->z.s.az   = (float)daz;
	ev->z.s.baz  = (float)dbaz;

 	fprintf(stdout,
	    "%s: %s: %s: Resetting distaz values ista=%d sta.net=%s.%s dist=%.1f az=%.1f baz=%.1f\n",
		progname, __FILE__, __func__,
		ista,
		ev->stnm,
		ev->net,
		ev->ns.s.dist,
		ev->ns.s.az,
		ev->ns.s.baz );
	fflush(stdout);

	ev->rdistkm = ev->ns.s.dist;
	ev->az      = ev->ns.s.az;
	ev->baz     = ev->ns.s.baz;

/****************************************************************************************/
/*** do not rotate the rotational data                                                ***/
/****************************************************************************************/

	if( strcmp( ev->wavetype, "Surf/Pnl" ) == 0 )
	{

/****************************************************************************************/
/*** before rotating the horizontals to the great circle path first rotate to pure NS ***/
/***  EW coordinate system                                                            ***/
/****************************************************************************************/

	  if( ev->ns.s.cmpaz != 0. && ev->ew.s.cmpaz != 90. )
	  {
		if(verbose)
		{
		  fprintf( stdout, "%s: %s: %s: ista=%d horizontals az not 0 and 90. ", 
			progname, __FILE__, __func__, ista );
		  fprintf( stdout, " az->(ns=%g,ew=%g) Rotating NS to 0 and EW to 90\n",
			ev->ns.s.cmpaz, ev->ew.s.cmpaz );
		  fflush(stdout);
		}

		angle = 360 - ev->ns.s.cmpaz;
		rotate( ev->ns.data, ev->ns.s.npts, &(ev->ns.s.cmpaz), ev->ns.s.cmpinc,
                        ev->ew.data, ev->ew.s.npts, &(ev->ew.s.cmpaz), ev->ew.s.cmpinc,
                        angle, verbose );
	  }

/***********************************************************************/
/*** rotate the horizontals to get radial and transverse             ***/
/*** radial component is written over the ns                         ***/
/*** transverse/tangential component is written over the ew          ***/
/***********************************************************************/

	  /* angle = ev->ns.s.az; */

	  angle = ev->ns.s.baz + 180;

	  rotate( ev->ns.data, ev->ns.s.npts, &(ev->ns.s.cmpaz), ev->ns.s.cmpinc,
                ev->ew.data, ev->ew.s.npts, &(ev->ew.s.cmpaz), ev->ew.s.cmpinc,
                angle, verbose );

	  if(verbose)
	  {
		fprintf( stdout, 
		  "%s: %s: %s: ista=%d horizontals rotated by angle=%g\n",
                     progname, __FILE__, __func__, ista, angle );
		fflush(stdout);
	  }

	} /*** rotate only if ev->wavetype == Surf/Pnl ... NOT Rotational! ***/

/***********************************************************************/
/*** compute peak to peak of signal and noise to measure SNR         ***/
/***********************************************************************/

/*** ew ***/
	ev->ew.pha[NOISE].gvlo = 8.5;  ev->ew.pha[NOISE].gvhi = 999.9;
	ev->ns.pha[NOISE].gvlo = 8.5;  ev->ns.pha[NOISE].gvhi = 999.9;
	ev->z.pha[NOISE].gvlo  = 8.5;  ev->z.pha[NOISE].gvhi = 999.9;

	ev->ew.pha[SIGNAL].gvlo = 1.5;  ev->ew.pha[SIGNAL].gvhi = 7.0;
	ev->ns.pha[SIGNAL].gvlo = 1.5;  ev->ns.pha[SIGNAL].gvhi = 7.0;
	ev->z.pha[SIGNAL].gvlo  = 1.5;  ev->z.pha[SIGNAL].gvhi = 7.0;

	if(verbose)
	{
	   fprintf( stdout, "%s: %s: %s: ista=%d %s.%s.%s NOISE=%d ew-chan gvlo=%g gvhi=%g SIGNAL=%d ew-chan gvlo=%g gvhi=%g lf=%g\n",
		progname, __FILE__, __func__, ista, ev->net, ev->stnm, ev->loc, 
		NOISE, ev->ew.pha[NOISE].gvlo, ev->ew.pha[NOISE].gvhi, 
		SIGNAL, ev->ew.pha[SIGNAL].gvlo, ev->ew.pha[SIGNAL].gvhi, ev->lf );
		
	  fflush(stdout);
	}

	compute_Peak_to_Peak( &(ev->ew.s), ev->ew.data, 1.0/ev->lf,
        	ev->ew.pha[NOISE].gvlo,  ev->ew.pha[NOISE].gvhi,
        	&(ev->ew.pha[NOISE].amp),  &(ev->ew.pha[NOISE].time),
        	&(ev->ew.pha[NOISE].duration), NOISE, 1 /*verbose*/ );

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: noise-EW-ista=%d per=%g gvlo=%g gvhi=%g amp=%g time=%g dur=%g\n",
                progname, __FILE__, __func__, 
		ista, (1.0/ev->lf),
                ev->ew.pha[NOISE].gvlo,
                ev->ew.pha[NOISE].gvhi,
                ev->ew.pha[NOISE].amp,
                ev->ew.pha[NOISE].time,
                ev->ew.pha[NOISE].duration );
	  fflush( stdout );
	}

	compute_Peak_to_Peak( &(ev->ew.s), ev->ew.data, 1/ev->lf,
        	ev->ew.pha[SIGNAL].gvlo, ev->ew.pha[SIGNAL].gvhi,
        	&(ev->ew.pha[SIGNAL].amp), &(ev->ew.pha[SIGNAL].time),
        	&(ev->ew.pha[SIGNAL].duration), SIGNAL, 1 /* verbose */ );

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: signal-EW-ista=%d per=%g gvlo=%g gvhi=%g amp=%g time=%g dur=%g\n",
                progname, __FILE__, __func__,
		ista, (1.0/ev->lf),
                ev->ew.pha[SIGNAL].gvlo,
                ev->ew.pha[SIGNAL].gvhi,
                ev->ew.pha[SIGNAL].amp,
                ev->ew.pha[SIGNAL].time,
                ev->ew.pha[SIGNAL].duration );
	  fflush(stdout);
	}

	ev->ew.pha[NOISE].period     = 2 * ev->ew.pha[NOISE].duration;
	ev->ew.pha[NOISE].frequency  = 1 / ev->ew.pha[NOISE].period;
	ev->ew.pha[SIGNAL].period    = 2 * ev->ew.pha[SIGNAL].duration;
	ev->ew.pha[SIGNAL].frequency = 1 / ev->ew.pha[SIGNAL].period;
	ev->ew.P2P_snr               = ev->ew.pha[SIGNAL].amp /
                                       ev->ew.pha[NOISE].amp;

/*** ns ***/
	compute_Peak_to_Peak( &(ev->ns.s), ev->ns.data, 1/ev->lf,
        	ev->ns.pha[NOISE].gvlo, ev->ns.pha[NOISE].gvhi,
        	&(ev->ns.pha[NOISE].amp), &(ev->ns.pha[NOISE].time),
        	&(ev->ns.pha[NOISE].duration), NOISE, verbose );

	compute_Peak_to_Peak( &(ev->ns.s), ev->ns.data, 1/ev->lf,
        	ev->ns.pha[SIGNAL].gvlo, ev->ns.pha[SIGNAL].gvhi,
        	&(ev->ns.pha[SIGNAL].amp), &(ev->ns.pha[SIGNAL].time),
        	&(ev->ns.pha[SIGNAL].duration), SIGNAL, verbose );

	ev->ns.pha[NOISE].period     = 2 * ev->ns.pha[NOISE].duration;
	ev->ns.pha[NOISE].frequency  = 1 / ev->ns.pha[NOISE].period;
	ev->ns.pha[SIGNAL].period    = 2 * ev->ns.pha[SIGNAL].duration;
	ev->ns.pha[SIGNAL].frequency = 1 / ev->ns.pha[SIGNAL].period;
	ev->ns.P2P_snr               = ev->ns.pha[SIGNAL].amp /
                                       ev->ns.pha[NOISE].amp;

/*** z ***/
	compute_Peak_to_Peak( &(ev->z.s), ev->z.data, 1/ev->lf,
        	ev->z.pha[NOISE].gvlo,  ev->z.pha[NOISE].gvhi,
        	&(ev->z.pha[NOISE].amp), &(ev->z.pha[NOISE].time),
        	&(ev->z.pha[NOISE].duration), NOISE,  verbose );

	compute_Peak_to_Peak( &(ev->z.s), ev->z.data, 1/ev->lf,
        	ev->z.pha[SIGNAL].gvlo, ev->z.pha[SIGNAL].gvhi,
        	&(ev->z.pha[SIGNAL].amp), &(ev->z.pha[SIGNAL].time),
        	&(ev->z.pha[SIGNAL].duration), SIGNAL, verbose );
                                                                                                                                                                
	ev->z.pha[NOISE].period     = 2 * ev->z.pha[NOISE].duration;
	ev->z.pha[NOISE].frequency  = 1 / ev->z.pha[NOISE].period;
	ev->z.pha[SIGNAL].period    = 2 * ev->z.pha[SIGNAL].duration;
	ev->z.pha[SIGNAL].frequency = 1 / ev->z.pha[SIGNAL].period;
	ev->z.P2P_snr               = ev->z.pha[SIGNAL].amp /
	                              ev->z.pha[NOISE].amp;

	ev->ienvelope = 0; /*** envelope is deprecated gene.ichinose 07.01.2024 ***/

/********************************************************/
/*** compute min max and mean extrema                 ***/
/********************************************************/

	sac_minmax( ev->z.data,  ev->z.s.npts,
		&(ev->z.s.depmax),  &(ev->z.s.depmin),  &(ev->z.s.depmen) );
	sac_minmax( ev->ew.data, ev->ew.s.npts,
		&(ev->ew.s.depmax), &(ev->ew.s.depmin), &(ev->ew.s.depmen) );
	sac_minmax( ev->ns.data, ev->ns.s.npts,
		&(ev->ns.s.depmax), &(ev->ns.s.depmin), &(ev->ns.s.depmen) );

	fprintf( stdout, "%s: %s: %s:\t sacdata2inv_serial(): ista=%d %s.%s Z  min=%g max=%g mean=%g\n",
		progname, __FILE__, __func__, ista, ev->stnm, ev->net, ev->z.s.depmin,
		ev->z.s.depmax,  ev->z.s.depmen );

	fprintf( stdout, "%s: %s: %s:\t sacdata2inv_serial(): ista=%d %s.%s NS min=%g max=%g mean=%g\n",
		progname, __FILE__, __func__, ista, ev->stnm, ev->net, ev->ns.s.depmin,
		ev->ns.s.depmax, ev->ns.s.depmen );

	fprintf( stdout, "%s: %s: %s:\t sacdata2inv_serial(): ista=%d %s.%s EW min=%g max=%g mean=%g\n",
		progname, __FILE__, __func__, ista, ev->stnm, ev->net, ev->ew.s.depmin,
		ev->ew.s.depmax, ev->ew.s.depmen );
	fflush(stdout);

/****************************************************************************************/
/*** write out individual SAC binary formated the files for inspection                ***/
/****************************************************************************************/

	if( dumpsac ) writesacfile( ev );

/****************************************************************************************/
/*** write a *.data file for inversion                                                ***/
/****************************************************************************************/

	fprintf( stdout, "%s: %s: %s: writting file %s\n",
		progname,  __FILE__, __func__, ev->data_filename );
	fflush( stdout );

	fpout = fopen( ev->data_filename, "w" );
	fwrite( ev, sizeof(EventInfo), 1, fpout );
	fwrite( &(ev->ew.data[0]), ev->ew.s.npts * sizeof(float), 1, fpout );
	fwrite( &(ev->ns.data[0]), ev->ns.s.npts * sizeof(float), 1, fpout );
	fwrite( &(ev->z.data[0]),  ev->z.s.npts  * sizeof(float), 1, fpout );
	fclose( fpout );
                                                                                                                                                                
/****************************************************************************************/
/*** clean up                                                                         ***/
/****************************************************************************************/
/*
	if(verbose)
	{
		fprintf( stdout, "%s: sacdata2inv_serial(): done with station ista=%d \n", progname, ista );
		fprintf( stdout, "%s: sacdata2inv_serial(): cleaning up memory\n", progname );
	}
	free(ev->ew.data);
	free(ev->ns.data);
	free(ev->z.data);
*/
	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: done with ista=%d\n\n\n", 
		progname, __FILE__, __func__, ista );
	  fflush(stdout);
	}
	fprintf( stdout, "\n\n\n" );
	fflush(stdout);
}
