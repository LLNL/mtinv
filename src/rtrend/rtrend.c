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

extern char progname[128];

/* Now Local Variables ( float *a, float *b, float *sigmaa, float *sigmab, float *sigma, float *corcoef ) */

void rtrend( float x0, float dx, float *y, int n, int verbose )
{
	int i;
	float sumx, sumy, sumxy, sumx2, sumy2;
	float xi, yi;
	float sigma2, sigmaa2, sigmab2, den;
	float degfree;
	float ytmp, ypre;

	float a, b, sigmaa, sigmab, sigma, corcoef;

	sumx  = 0;
	sumy  = 0;
	sumxy = 0;
	sumx2 = 0;
	sumy2 = 0;
	degfree = (float)n - 2;

/*** loop over each data point and calculate sums ***/

	xi = x0;
	for( i = 0; i < n; i++ )
	{
		yi = y[i];
		sumx  += xi;
		sumy  += yi;
		sumxy += ( xi * yi );
		sumx2 += ( xi * xi );
		sumy2 += ( yi * yi );
		xi += dx;
	}

/* calculate linear fit */

	den = ( (float)n * sumx2 - sumx * sumx );
	b = ( sumx2 * sumy - sumx * sumxy ) / den;
	a = ( (float)n * sumxy - sumx * sumy ) / den;

/* estimate standard deviation */

	sigma2 = ( sumy2 + (float)n * b * b + a * a * sumx2 - 2 * b * sumy - 2 * a * sumxy + 2 * b * a * sumx ) / degfree;
	sigma = sqrt( sigma2 );

/* estimate erros in linear fit */

	sigmaa2 = (float)n * sigma2 / den;
	sigmab2 = sigma2 * sumx2 / den;
	sigmaa  = sqrt( sigmaa2 );
	sigmab  = sqrt( sigmab2 );

/* Calculate correlation coefficient between data and model. */

	corcoef = ( (float)n * sumxy - sumx * sumy ) / sqrt( den * ( (float)n * sumy2 - sumy*sumy ) );
	corcoef = fabs( corcoef );

/*** remove the mean and trend (y-intercept and slope) and return the new data vector **/

	xi = x0;
	for( i = 0; i < n; i++ )
	{
		ypre = b + a * xi;
		ytmp = y[i] - ypre;
		y[i] = ytmp;
		xi = xi + dx;
	}

	if(verbose)
	{
		fprintf( stdout, "%s: rtrend(): a=%g b=%g sigma=%g sigmaA=%g sigmaB=%g corcoef=%g\n",
			progname,
			a, 
			b, 
			sigma,
			sigmaa,
			sigmab,
			corcoef );
	}
}
