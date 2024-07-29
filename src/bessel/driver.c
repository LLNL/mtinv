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

