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

#include "sac.h"

char progname[128];

int main( int ac, char **av )
{
	char SacFileName[256];
	Sac_Header s;
	float *data;
	float dt, beg;
	int nt;
	int verbose = 1;

	float *readsac( Sac_Header *s, char *filename, int verbose );
	void set_sac_minmax( Sac_Header *s, float *data );
	void wrtoldsac( char *FO, Sac_Header *s, float *data );

	void remove_mean( float *x, int n );
	void rtrend_sac( float *x, int n, float dt, float beg, int verbose );

	void rtrend( float x0, float dx, float *y, int n, int verbose );
	int setpar(),mstpar();
	void endpar();

/*** begin main ***/
	strcpy( progname, av[0] );

	setpar(ac,av);
	mstpar( "sacf",      "s",     SacFileName );
	endpar();

	data = calloc( 24*3600*200, sizeof(float) );
	data = readsac( &s, SacFileName, verbose );

	dt = s.delta;
	nt = s.npts;
	beg = s.b;

	/* remove_mean( data, nt );  */
	/* rtrend_sac( data, nt, dt, beg, verbose ); */

/*** this is sac routine, also does rmean ***/
	rtrend( beg, dt, data, nt, verbose );

	set_sac_minmax( &s, data );
	fprintf( stdout, "%s: min=%g max=%g mean=%g\n", 
		progname, s.depmin, s.depmax, s.depmen );

	wrtoldsac( "rtrend.sac", &s, data );

	free(data);
	return 0;
}
