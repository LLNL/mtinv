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
#include <string.h>
#include <math.h>

/* GAUSSIAN 
#######################################
#                  (x - mu)^2          #
# f(x) = a exp( - ------------ ) + d  #
#                  2 sigma^2           #
#######################################
*/

int main( int ac, char **av )
{
	float a = 1, mu = 1, sig = 1, yoffset = 0;
	float dx = 0.1, x0 = -10, x1 = +10;
	float x, y;
	int  nx;
	int ixlog = 0;
	
	int setpar(int,char **),getpar();
	void endpar();

	setpar(ac,av);
	getpar( "a",   "f", &a );
	getpar( "mu",  "f", &mu );
	getpar( "sig", "f", &sig );
	getpar( "yoffset", "f", &yoffset );
	getpar( "dx",  "f", &dx );
	getpar( "x0",  "f", &x0 );
	getpar( "x1",  "f", &x1 );
	getpar( "xlog", "b", &ixlog );
	endpar();

	nx = (int)rint( ( x1 - x0 )/dx );

	fprintf( stdout, ">   %d %g %g %g %g %g %g %g\n", nx, a,mu,sig, yoffset, x0,x1,dx );

	for( x = x0; x <= x1; x += dx )
	{
		y = a * exp( -1 * pow( (x - mu), 2 ) / (2 * (sig*sig)) ) + yoffset;

		if(ixlog)
		  fprintf( stdout, "%g %g\n", log10(x), y );
		else
		  fprintf( stdout, "%g %g\n", x, y );
	}
	exit(0);
}
