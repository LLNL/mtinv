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

/****************************************************************************/
/*** compute_Peak_to_Peak.c                                               ***/
/***                                                                      ***/
/*** G. Ichinose   Sun Apr  1 10:54:45 EDT 2012                           ***/
/***                                                                      ***/
/*** compute the peak-to-peak (p2p) value between a start and stop time   ***/
/*** set by a group velocities.  There is a shifting window set by the    ***/
/*** corner of the low pass filter corner period.  The p2p is measured    ***/
/*** within this smaller window so that the period of the p2p measurement ***/
/*** is consistent with the rules of p2p.                                 ***/
/***                                                                      ***/
/*** this is called from sacdata2inv_parallel.c and sacdata2inv_serial.c  ***/
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include "sac.h"

extern char progname[128];

void compute_Peak_to_Peak( 
	Sac_Header *header, 
	float *data, 
	float period, 
	float vmin, 
	float vmax, 
	float *p2pamp, 
	float *p2ptime, 
	float *p2p_duration,
	int SignalWindowFlag, 
	int verbose )
{
	int i, j, k, it, nt;
	float dt, beg, ot, dist;
	float ampmax = 0, timemax = 0;
	float ampmin = 0, timemin = 0;
	int WindowLength;
	int ivmin, ivmax;
	float tvmin, tvmax;
	float duration, tmpp2p, maxp2p, tmptimemax, tmptimemin, tmpampmax, tmpampmin;
	float sign(float x );
	int debug = 0;

	nt   = header->npts;
	dt   = header->delta;
	beg  = header->b;
	ot   = header->o;
	dist = header->dist;

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: period=%g vmin=%g vmax=%g nt=%d dt=%g dist=%g ot=%g beg=%g SignalWindowFlag=%d\n",
		progname, __FILE__, __func__, period, vmin, vmax, nt, dt, dist, ot, beg, SignalWindowFlag );
	  fflush( stdout );
	}

/***************************************************************************/
/*** compute p2p window length from lowpass corner filter period         ***/
/*** in points                                                           ***/
/***************************************************************************/

	WindowLength = rint( period / dt );

/***************************************************************************/
/*** run the p2p window across the group velocity window            ***/
/*** compute the start and stop times for the group velocity window ***/
/***************************************************************************/

	tvmin = (( dist / vmin ) + ( ot - beg ));
	tvmax = (( dist / vmax ) + ( ot - beg ));

	ivmin = rint( tvmin/dt );
	ivmax = rint( tvmax/dt );

/**** always shuts the window down before nt is reached ***/
	if( ivmax < beg ) ivmax = 0;
	if( ivmin > nt  ) ivmin = nt;

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: ivmax=%d ivmin=%d WindowLength=%d\n",
		progname, __FILE__, __func__, 
		 ivmax, ivmin, WindowLength );	
	  fflush(stdout);
	}

/***************************************************************************/
/*** group velocity window loop shift the p2p window across the        ***/
/*** group velocity start-to-stop times                                ***/
/***************************************************************************/

/*
	ampmax = data[ivmax];
	ampmin = data[ivmin];
	maxp2p = fabs( ampmax ) + fabs( ampmin ); 
*/
	maxp2p = 1.0E-19;
	tmpampmax = +1.0E-19;
        tmpampmin = -1.0E-19;
	tmptimemin = 0;
	tmptimemax = 0;

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: SignalWindow=%d ivmax=%d ivmin=%d WindowLength=%d nt=%d dt*nt=%g \n", 
		progname, __FILE__, __func__, 
		SignalWindowFlag, ivmax, ivmin, WindowLength, nt, dt*nt );
	  fflush(stdout);
	}
	for( it = ivmax; it <= ivmin - WindowLength; it++ )
	{
		tmpampmax = +1.0E-19;
		tmpampmin = -1.0E-19;
		tmptimemin = 0;
		tmptimemax = 0;

		for( k = it; k < it + WindowLength; k++ )
		{
			if( data[k] > tmpampmax ) 
			{
				tmpampmax = data[k];
				tmptimemax = beg + (float)k*dt;
			}
			if( data[k] < tmpampmin ) 
			{
				tmpampmin = data[k]; 
				tmptimemin = beg + (float)k*dt;
			}

		} /*** loop over the period window ***/
		
	/***************************************************/
	/*** this is the p2p for this period window      ***/
        /*** and point in the group vel window           ***/
	/***************************************************/

		duration = fabs( fabs( tmptimemax ) - fabs( tmptimemin ) );
		tmpp2p = fabs( tmpampmax ) + fabs( tmpampmin );

		if(debug)
		{
		  fprintf( stdout, 
"%s: %s: %s: it=%d SignalWindow=%d maxp2p=%g tmpp2p=%g 2*duration=%g, period=%g ampmax=%g ampmin=%g tmpampmax=%g tmpampmin=%g tmptimemin=%g tmptimemax=%g \n",
			progname, __FILE__, __func__, 
			it, SignalWindowFlag, maxp2p, tmpp2p, 2*duration, period, ampmax, ampmin, tmpampmax, tmpampmin, tmptimemin, tmptimemax );
	 	  fflush( stdout );
		}

		if( ((2*duration ) <= period ) && ( tmpp2p > maxp2p ) )
		{	
			ampmax = tmpampmax;
			ampmin = tmpampmin;
			timemin = tmptimemin;
			timemax = tmptimemax;
			maxp2p = tmpp2p;
		}

	} /*** loop over group velocity window ***/

/***************************************************************/
/*** if the search above returned without finding p2p        ***/
/*** then do a special search just within the grp vel window ***/
/***************************************************************/

	if( timemax == 0 && timemin == 0 )
	{
		if(verbose)	
		{
		  fprintf( stdout, 
		    "%s: %s: %s: Error timemax=%g timemin=%g Searching Group Velocity Window\n", 
			progname, __FILE__, __func__, 
			timemax, timemin );
		  fflush( stdout );
		}

		tmpampmax = 0;
                tmpampmin = 0;
		tmptimemax = 0;
		tmptimemin = 0;

		for( it = ivmax; it <= ivmin; it++ )
		{
			if( data[it] > tmpampmax )
			{
				tmpampmax = data[it];
				tmptimemax = beg + (float)it*dt;
			}
			if( data[it] < tmpampmin )
			{
				tmpampmin = data[it];
				tmptimemin = beg + (float)it*dt;
			}
		}

		duration = fabs( tmptimemax - tmptimemin );
		tmpp2p = fabs( tmpampmax ) + fabs( tmpampmin );

		ampmax = tmpampmax;
		ampmin = tmpampmin;
		timemin = tmptimemin;
		timemax = tmptimemax;
		maxp2p = tmpp2p;
	
	  if( verbose )
	  {
	    fprintf( stdout, "%s: %s: %s: ampmin=%g ampmax=%g timemin=%g timemax=%g maxp2p=%g\n",
		progname, __FILE__, __func__,
		ampmin,
		ampmax,
		timemin,
		timemax,
		maxp2p );
	    fflush( stdout );
	  }
	}

/*****************************************/
/*** write the p2p times to SAC header ***/
/*****************************************/

	if(SignalWindowFlag) /** signal window **/
	{
	  header->t1 = tvmax;
	  header->t2 = tvmin;
	  strcpy( header->kt1, "S1" );
	  strcpy( header->kt2, "S2" );
	  header->t5 = timemax;
	  header->t6 = timemin;
	  strcpy( header->kt5, "A1" );
	  strcpy( header->kt6, "A2" );
	}

	if(!SignalWindowFlag) /** noise window **/
	{
	  header->a  = tvmax;
	  header->t0 = tvmin;
	  strcpy( header->ka,  "N1" );
          strcpy( header->kt0, "N2" );
	  header->t3 = timemax;
	  header->t4 = timemin;
	  strcpy( header->kt3, "B1" );
          strcpy( header->kt4, "B2" );
	}

/***************************************************************************/
/*** return the p2p values for entire group velocity window ***/
/***************************************************************************/

	if( fabs( ampmax ) > fabs( ampmin ) ) 
	{
		*p2ptime = timemax;
	}
	else
	{
		*p2ptime = timemin;
	}

	*p2pamp  = fabs(ampmax) + fabs(ampmin);

	*p2p_duration  = fabs( timemax - timemin );

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: bye-bye\n", progname, __FILE__, __func__ );
	  fflush( stdout );
	}

	return;
}

float sign( float x )
{
	if( x >= 0 ) return 1;
	if( x < 0 ) return -1;
	fprintf( stderr, "compute_Peak_to_Peak.c:sign(): Fatal error\n" );
	exit(-1);
}
