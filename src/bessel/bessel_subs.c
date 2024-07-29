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

/*** from numerical recipes (https://numerical.recipes) ***/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float distradians_gcp( float lat0, float lon0, float lat1, float lon1 )
{
	double distdeg, distkm, az, baz;
	float deg2rad = M_PI / 180;
	int distaz( double olat, double olon, double tlat, double tlon, double *del, double *az, double *baz );

	distaz( (double)lat0, (double)lon0, (double)lat1, (double)lon1, &distkm, &az, &baz );
	distdeg = distkm / 111.11;
	return( (float) distdeg * deg2rad );
}

float vonMises1D_pdf( float kappa, float dist_rads, float fac )
{
	float vM_pdf, r0 = 0;
	float bessi0( float x );
	float twopi = 2.0*M_PI, halfpi = M_PI/2.0;

	vM_pdf = exp( kappa * cos( dist_rads - r0 ) ) / ( twopi * bessi0(kappa) );
	return vM_pdf*fac;
}

float vonMisesFisher3d_pdf( float kappa, float dist_rads, float fac )
{
	float Cp3, vMF_pdf;
	float twopi = 2.0*M_PI, halfpi = M_PI/2.0;

	Cp3 = ( ( kappa ) / ( twopi * (exp(kappa)-exp(-kappa)) ) );
	vMF_pdf = Cp3 * exp( kappa * ( halfpi - dist_rads ) );
	return vMF_pdf*fac;
}

/* bessi0.c */
/* Returns the modified Bessel function I0(x) for any real x. */

float bessi0(float x)
{
	float ax,ans;
	double y;

	if ((ax=fabs(x)) < 3.75) {
		y=x/3.75;
		y*=y;
		ans=1.0+y*(3.5156229+y*(3.0899424+y*(1.2067492
			+y*(0.2659732+y*(0.360768e-1+y*0.45813e-2)))));
	} else {
		y=3.75/ax;
		ans=(exp(ax)/sqrt(ax))*(0.39894228+y*(0.1328592e-1
			+y*(0.225319e-2+y*(-0.157565e-2+y*(0.916281e-2
			+y*(-0.2057706e-1+y*(0.2635537e-1+y*(-0.1647633e-1
			+y*0.392377e-2))))))));
	}
	return ans;
}
