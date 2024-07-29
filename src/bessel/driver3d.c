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

/*** performs tests on various subroutines, to be used with mteig.c mteig_subs.c ***/

/*** these are the 3D versions, 1D see driver.c ***/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int ac, char **av)
{
	float lon, lat;
	float lat_u = 0, lon_u = 0;
	float lon0 = -30, lon1 = +30, lat0 = -90, lat1 = 90, dxy = 2;
	int ix, iy, nx, ny;
	int nfac = 10000;
	float vM_pdf, vMF_pdf, kappa = 1;
	int vMF3d = 1;
	float distrad;

	float distradians_gcp( float lat0, float lon0, float lat1, float lon1 );
	float vonMises1D_pdf( float kappa, float dist_rads, float fac );
	float vonMisesFisher3d_pdf( float kappa, float dist_rads, float fac );
	int setpar(), mstpar(), getpar();
	void endpar();

	setpar( ac, av );
	mstpar( "kappa", "f", &kappa );
	getpar( "dxy", "f", &dxy );
	getpar( "vMF3d", "b", &vMF3d );
	getpar( "nfac", "d", &nfac );
	endpar();

	nx = rint( ( lon1 - lon0 ) / dxy ) + 1;
	ny = rint( ( lat1 - lat0 ) / dxy ) + 1;
	
	fprintf( stderr, "nx=%d ny=%d dxy=%g lon0/1=%g/%g lat0/1=%g/%g \n",
		nx, ny, dxy, lon0, lon1, lat0, lat1 );

	for( ix = 0; ix < nx; ix++ )
	{
		lon = lon0 + (float)ix * dxy;

		for( iy = 0; iy < ny; iy++ )
		{
			lat = lat0 + (float)iy * dxy;

			distrad = distradians_gcp( lat, lon, lat_u, lon_u );

			vM_pdf  = vonMises1D_pdf(kappa,distrad,nfac);
			vMF_pdf = vonMisesFisher3d_pdf(kappa,distrad,nfac);

			if( vMF3d )
			  fprintf( stdout, "%g %g %g %g\n", lon, lat, vMF_pdf, distrad );
			else
			  fprintf( stdout, "%g %g %g %g\n", lon, lat, vM_pdf, distrad );
		}
	}
}
