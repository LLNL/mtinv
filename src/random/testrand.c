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

/*** test to see if srandom(seed) is suitable substitute for srandomdev() and sranddev() BSD unix ***/

float getRandFloat( const float A, const float B );
void getRandSphere( float *e1, float *e2, float *e3 );

int main( int ac, char **av )
{
	int i, n = 1;
	float e[3] = { 0, 0, 0 };
	float e1,e2,e3;
	float u;
	float w;
	int seed = -1;

	void random_sphere( float *x, int seed );
	float random_float( int seed, float min, float max );
	
	int mstpar(),setpar();
	void endpar();

	setpar(ac,av);
	mstpar( "n", "d", &n );
	mstpar( "seed", "d", &seed );
	endpar();

	if(seed>0)
		srandom(seed);
	else
		srand(seed);

	for( i = 0; i < n; i++ )
	{
		getRandSphere( &e1, &e2, &e3 );
		random_sphere( e, seed );

		u = random_float( seed, 0.0, 1.0 );
		w = getRandFloat( 0.0, 1.0 );
		
		fprintf( stdout, "%.8f %.8f  %+.8f %+.8f  %+.8f %+.8f   %+.8f %+.8f\n",
			u, w, e[0], e1, e[1], e2, e[2], e3 );
	}
	exit(0);
}

void random_sphere( float *x, int seed )
{
	float theta, u;
	static float twopi = 6.283185307179586;

/*** srandomdev() - stdlib.h - is a BSD unix, not portable to Linux ***/
/*** srandomdev() - routine initializes a state array using pseudo-random obtained from the kernel ***/
/*** Note that this particular seeding pro-
     cedure can	generate states	which are impossible to	reproduce by calling
     srandom() with any	value, since the succeeding terms in the state buffer
     are no longer derived from	the LC algorithm applied to a fixed seed.
****/
/*** sranddev() stdlib.h - function initializes a seed	using pseudo-random numbers obtained from the kernel ***/

	if( seed > 0 )
	{
		theta = twopi * ( (float) random() / (RAND_MAX) );          /* 0 to 2pi */
		u     = 2.0 * ( ( (float) random() / (RAND_MAX) ) - 0.5 );  /* -1 to +1 */

	}
	else
	{
		/* sranddev(); */
		theta = twopi * ( (float) rand() / (RAND_MAX) );          /* 0 to 2pi */
		u     = 2.0 * ( ( (float) rand() / (RAND_MAX) ) - 0.5 );  /* -1 to +1 */
	}

	x[0]  = sqrt(1 - (u*u)) * cos(theta);
	x[1]  = sqrt(1 - (u*u)) * sin(theta);
	x[2]  = u;
}

/**********************************************************************************/
/*** create a random repeatable or non-repeatable floating point number between ***/
/*** min and max ...  used by rand_rotation() and random_sphere()               ***/
/**********************************************************************************/
float random_float( int seed, float min, float max )
{
	float r = 0;
/**** non-repeatable random number uniformly distributed ***/
	if( seed > 0 ) 
	{
		r = (max-min) * ( (float) random() / (RAND_MAX) ) + min;  /* min to max */
		/* fprintf( stdout, "%s: %s: %s: r=%g\n", progname, __FILE__, __func__, r ); */
	}
	else
	{
		/* sranddev(); */
		r = (max-min) * ( (float) rand() / (RAND_MAX) ) + min;  /* min to max */
		/* fprintf( stdout, "%s: %s: %s: r=%g\n", progname, __FILE__, __func__, r ); */
	}
	return r;
}
