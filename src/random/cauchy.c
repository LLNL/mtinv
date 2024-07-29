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

int main( int ac, char **av )
{
	float y=0, x = 0, dx = 0.1, x0 = -10, x1 = 10;
	float median = 0; /*** mode or median ***/
	float gamma = 1; /*** scale ***/
	float yscale = 1;
	int icdf = 0;
	int ipdf = 1;

	float cauchy_pdf( float median, float gamma, float x );
	float cauchy_cdf( float median, float gamma, float x );

	int setpar(int , char **),mstpar(),getpar();
	void endpar();

	setpar(ac,av);
	mstpar("median", "f", &median);
	mstpar("gamma", "f", &gamma);
	mstpar("dx", "f", &dx);
	mstpar("x0", "f", &x0);
	mstpar("x1", "f", &x1);
	mstpar("yscale", "f", &yscale );
	getpar("pdf", "b", &ipdf );
	getpar("cdf", "b", &icdf );
	endpar();

	fprintf( stdout,
	  "> cauchy distribution median=%g gamma=%g dx=%g x0=%g x1=%g ipdf=%d icdf=%d\n",
		median, gamma, dx, x0, x1, ipdf, icdf );
		
	for( x = x0; x <= x1; x += dx )
	{
		if(ipdf) y = yscale * cauchy_pdf( median, gamma, x );
		if(icdf) y = yscale * cauchy_cdf( median, gamma, x );
		fprintf( stdout, "%g %g\n", x, y );
	}
	return 0;
}

float cauchy_pdf( float median, float gamma, float x )
{
	float arg0, arg1;
	arg0 = ( ( x - median ) / gamma );
	arg1 = M_PI * gamma * ( 1 + (arg0*arg0) );
	return (1.0/arg1);
}

float cauchy_cdf( float median, float gamma, float x )
{
	float arg0, arg1;
	arg0 = ( ( x - median ) / gamma );
	arg1 = ( 1.0 / M_PI ) * atan( arg0 ) + 0.5;
	return arg1;
}
