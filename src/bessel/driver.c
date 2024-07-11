/*** performs tests on various subroutines, to be used with mteig.c mteig_subs.c ***/

/*** see numerical recipes (https://numerical.recipes) ***/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int ac, char **av)
{
	float x_angle_rad, x_angle_deg;
	float small_radian =  1.74533e-05; /*** about 0.001 degrees ***/

	float d_angle = 0.1;
	float angle0 = -180, angle1 = 180;
	int i, n_angles;
	
	float vM_prob_dens, mean_rad = 0, kappa = 1;
	float vMf_prob_dens, mean_x, mean_y;
/*
	void sphbes( int order, float x, float *sj, float *sy, float *sjp, float *syp );
	void bessjy( float x, float xnu, float *rj, float *ry, float *rjp, float *ryp );
*/
	float bessi0( float x );

	int setpar(), mstpar(), getpar();
	void endpar();

	setpar( ac, av );
	mstpar( "kappa", "f", &kappa );
	getpar( "dangle", "f", &d_angle );
	endpar();

	n_angles = rint( ( angle1 - angle0 )/d_angle )+1;

	fprintf( stderr, "%s: n_anlges = %d d_angle = %g angle0 = %g angle1 = %g\n", 
		__FILE__, n_angles, d_angle, angle0, angle1 );

	for( i = 0; i < n_angles; i++ )
	{
		x_angle_deg = angle0 + (float)i*d_angle;
		x_angle_rad = fabs( x_angle_deg * ( M_PI/180.0 ) );

		vM_prob_dens = exp( kappa * cos( x_angle_rad - mean_rad ) ) / ( 2 * M_PI * bessi0(kappa) );

		fprintf( stdout,  "%g %g\n", x_angle_deg, vM_prob_dens );

	}
}

