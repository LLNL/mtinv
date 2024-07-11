#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/******************************************************************/
/*** phi = azimuth angle of the fiber cable
     theta = azimuth between the point of the cable and source ***/
/******************************************************************/

void radiation_pattern_driver_main( int ac, char **av )
{
	int i, n;

	float factor_l, factor_t;
	float thetad, thetar;
	float dthetad = 0.5;
	float deg2rad, phi0d, phi0r;

	/* float radpat_fiber_long_radians( float thetar, float phi0r ); */
	float radpat_fiber_long_degrees( float thetad, float phi0d );

	/* float radpat_fiber_trans_radians( float thetar, float phi0r ); */
	float radpat_fiber_trans_degrees( float thetad, float phi0d );
	char wvtype[8]; /* t=transverse l=longitudinal */

	int setpar(int ac, char **av);
	int mstpar( char *, char *, void * );
	int getpar( char *, char *, void * );
	void endpar();

/*** start ***/
	setpar(ac,av);
	mstpar( "type", "s", &wvtype );
	mstpar( "phi_shift", "f", &phi0d );
	endpar();

	deg2rad = M_PI / 180;

	phi0r = deg2rad * -1*phi0d;

	n = (360/dthetad)+1;

	for( i = 0; i < n; i++ )
	{
		thetad = (float)i * dthetad;
		thetar = thetad * deg2rad;

		factor_l = radpat_fiber_long_degrees( thetad, phi0d );
		factor_t = radpat_fiber_trans_degrees( thetad, phi0d );

		/* fprintf( stdout, "%g %g %g\n", thetad, factor_l, factor_t ); */
		if( strcmp( wvtype, "l" ) == 0 ) fprintf( stdout, "%g %g %s\n", thetad, factor_l, wvtype );
		if( strcmp( wvtype, "t" ) == 0 ) fprintf( stdout, "%g %g %s\n", thetad, factor_t, wvtype );
	}
}

float radpat_fiber_trans_degrees( float thetad, float phi0d )
{
	float tmp, thetar, phi0r, deg2rad;
	float radpat_fiber_trans_radians( float thetar, float phi0r );
	float scale_factor = 4;

	deg2rad = M_PI / 180;
	thetar = deg2rad * thetad;
	phi0r =  deg2rad * -1*phi0d;
	tmp = scale_factor * radpat_fiber_trans_radians( thetar, phi0r );
	return tmp;
}

float radpat_fiber_trans_radians( float thetar, float phi0r )
{
        float tmp;
        tmp = cos( thetar + phi0r )*sin( thetar + phi0r );
        return (tmp*tmp);
}

/******************************************************************/
/*** phi = azimuth angle of the fiber cable 
     theta = azimuth between the point of the cable and source ***/
/******************************************************************/

float radpat_fiber_long_degrees( float thetad, float phi0d )
{
	float tmp, thetar, phi0r, deg2rad;
	float radpat_fiber_long_radians( float thetar, float phi0r );

	deg2rad = M_PI / 180;
	thetar = deg2rad * thetad;
	phi0r =  deg2rad * -1*phi0d;
	
	tmp = radpat_fiber_long_radians( thetar, phi0r );
	return tmp;
}

float radpat_fiber_long_radians( float thetar, float phi0r )
{
	float tmp;
	tmp = cos( thetar + phi0r ) ;
	return (tmp*tmp); /*** returns cosine squared ***/
}
