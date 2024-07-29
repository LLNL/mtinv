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
#include <math.h>
#include "../include/mt.h"   /*** global datatype and structure declaration ***/

extern char progname[128];

int eig2lune_driver_main()
{
	/* Begin declaration of function prototypes */
	void  eig2lune( float *eig, float *latitude, float *longitude, int verbose );

	/* Begin declaration of local variables */
	float eig[3];
	int   verbose = 1;
	float lat=0.0f, lon=0.0f;

	eig[0] = 1.00f;
	eig[1] = 0.99f;
	eig[2] = 0.50f;

	eig2lune( eig, &lat, &lon, verbose );

	return 0;
}


/***************************************************************************
	eig2lune() - Tape and Tape (2012) J.Geophys.Int - A geometrical
		comparison of source-type plots for moment tensors

	eigenvalues ordered from largest positive to largest negative
		eig[0] > eig[1] > eig[2]  

	latitude = delta  longitude = gamma 
***************************************************************************/

void eig2lune( float *eig, float *latitude, float *longitude, int verbose )
{
	float lammag  = 0.0;
	float bdot    = 0.0;
	float rad2deg = 180.0f/M_PI;

	float lune_lat = 0.0;
        float lune_lon = 0.0;

        float tmpflt   = 0.0;
	float norm = 1;
	float *EigenValues     = (float *)calloc(4, sizeof(float));

	void floatsort( float *eig, int neig );
	float Zero = 1.0E-09;

/*** begin subroutinue ****/
	EigenValues[0] = 0;
	EigenValues[1] = eig[0];
	EigenValues[2] = eig[1];
	EigenValues[3] = eig[2];

/* Sorts ascending; ignores value in the zeroth */
/* index.  Need it however in ascending order   */
	floatsort( EigenValues, 3 );
	tmpflt         = EigenValues[1];
	EigenValues[1] = EigenValues[3];
	EigenValues[3] = tmpflt;

/*** normalize the eigenvalues because it is now in units of dyne*cm ***/
        norm = fabs( EigenValues[1] );
        EigenValues[1] /= norm;
        EigenValues[2] /= norm;
        EigenValues[3] /= norm;

	lammag = sqrt( EigenValues[1] * EigenValues[1] + EigenValues[2] * EigenValues[2] + EigenValues[3] * EigenValues[3] );

	bdot = ( EigenValues[1] + EigenValues[2] + EigenValues[3] ) / ( sqrt(3) * lammag );

	if( bdot > 1 ) bdot = 1;
	if( bdot < -1 ) bdot = -1;

	*latitude  = (float)( 90.0f - acosf( bdot ) * rad2deg );

	if( (EigenValues[1] - EigenValues[3]) != 0 )
		*longitude = atanf((-EigenValues[1] + 2 * EigenValues[2] - EigenValues[3]) / (sqrt(3)*(EigenValues[1] - EigenValues[3]) ) ) * rad2deg;
	else
		*longitude = atanf((-EigenValues[1] + 2 * EigenValues[2] - EigenValues[3]) / (sqrt(3)*(Zero) )) * rad2deg;

	if(verbose)
	{
		fprintf( stderr,
			"%s: %s: %s: longitude=%.2f latitude=%.2f eig0=%g eig1=%g eig2=%g\n",
			progname,
			__FILE__,
			__func__,
			*longitude,
			*latitude,
			EigenValues[1], EigenValues[2], EigenValues[3] );
	}

	return;
}


/***************************************************************************
	eig2lune_4mtinv() 

	- This version uses the sol structure inside mtinv
		
	- Tape and Tape (2012) J.Geophys.Int - A geometrical
	  comparison of source-type plots for moment tensors
                                                                                                            
	eigenvalues ordered from largest positive to largest negative
                eig[0] > eig[1] > eig[2]
                                                                                                            
	latitude = delta  
	longitude = gamma

	depends on floatsort() -- see mtinv_subs.c

	g.a.i 2017 modified when eigenvalues now unnormalized in units dyne-cm

***************************************************************************/

void eig2lune_4mtinv( Solution *sol, int iz, int verbose )
{
/* Begin declaration of function prototypes */
	void floatsort( float *, int );

/* Begin declaration of local variables */
	float *eig     = (float *)calloc(4, sizeof(float));
	double lammag   = 0.0;
	double bdot     = 0.0;
	float rad2deg  = 180.0f/M_PI;
	float lune_lat = 0.0;
	float lune_lon = 0.0;
	float tmpflt   = 0.0;
	float norm = 1;

	eig[0] = 0.0f;
	eig[1] = sol[iz].FullMT.eig[1].val;
	eig[2] = sol[iz].FullMT.eig[2].val;
	eig[3] = sol[iz].FullMT.eig[3].val;

/* Sorts ascending; ignores value in the zeroth */
/* index.  Need it however in ascending order   */
	floatsort( eig, 3 );
	tmpflt = eig[1];
	eig[1] = eig[3];
	eig[3] = tmpflt;

/*** normalize the eigenvalues because it is now in units of dyne*cm ***/
	norm = fabs( eig[1] );
	eig[1] /= norm;
	eig[2] /= norm;
	eig[3] /= norm;

	/*
	fprintf( stdout, "%s: %s: %s: eig1 = %e eig2 = %e eig3 = %e\n",
                progname, __FILE__, __func__,
		eig[1], eig[2], eig[3] );
	*/

	lammag = sqrt( eig[1] * eig[1] + eig[2] * eig[2] + eig[3] * eig[3] );

	/*
        fprintf( stdout, "%s: %s: %s: lammag = %e\n",
		progname, __FILE__, __func__, lammag );
	*/

	bdot   = ( eig[1] + eig[2] + eig[3] ) / ( sqrt(3) * lammag );

	/*
	fprintf( stdout, "%s: %s: %s: bdot = %e\n",
                progname, __FILE__, __func__, bdot );
	*/

	if( bdot >  1.0f ) bdot =  1.0;
	if( bdot < -1.0f ) bdot = -1.0;
                                                                                                            
	lune_lat = 90.0 - acos( bdot ) * rad2deg;

	if( (eig[1] - eig[3]) != 0 ) 
	 lune_lon = atanf(( -eig[1] + 2.0 * eig[2] - eig[3]) / (sqrt(3.0f)*(eig[1]-eig[3]))) * rad2deg;
        else
         lune_lon = atanf(( -eig[1] + 2.0 * eig[2] - eig[3]) / (sqrt(3.0f)*( 1.0E-09 ))) * rad2deg;                                                                                                   
	if(verbose)
	{
		fprintf( stdout,
		   "%s: %s: %s: longitude=%.2f latitude=%.2f eig0=%g eig1=%g eig2=%g\n",
			progname,
			__FILE__,
			__func__,
			lune_lon,
			lune_lat,
			eig[1], eig[2], eig[3] );
	}

	sol[iz].lune_lat = lune_lat;
	sol[iz].lune_lon = lune_lon;
}
