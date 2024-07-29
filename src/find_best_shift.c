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

#include "../include/nrutil.h"     /** numerical recipes **/
#include "../include/mt.h"         /** global datatype and structure declarations **/

extern char progname[128];

#define NUM_COMP 4

/*** this is used by mtinv.c - computes the correlation-coefficient and lagtime for each 3-C ***/

void find_best_shift( EventInfo *ev, float cortol, float maxtimeshift, float *time_shift_all )
{
	int i;
	float xcor[NUM_COMP];
	float tlag[NUM_COMP];
	int ixcor[NUM_COMP];

	int ncmp = 3;
	int debug = 0;

	void floatsort( float *x, int n );
	void indexx( int n, float *arrin, int *indx );

	ixcor[1] = 1;
	ixcor[2] = 2;
	ixcor[3] = 3;

	xcor[1]  = ev->zxcor;
	xcor[2]  = ev->rxcor;
	xcor[3]  = ev->txcor;

	tlag[1]  = ev->ztlag;
	tlag[2]  = ev->rtlag;
	tlag[3]  = ev->ttlag;

	if(debug)
	{
	 for( i=1; i<=ncmp; i++ )
	 {
	  fprintf ( stdout, "i=%d ixcor[i]=%d xcor=%g tlag=%g\n",
                i, ixcor[i], xcor[ixcor[i]], tlag[ixcor[i]] );
	 }
	}

	indexx( ncmp, xcor, ixcor );

	if( debug )
	{
	 for( i=1; i<=ncmp; i++ )
	 {
	  fprintf ( stdout, "i=%d ixcor[i]=%d xcor=%g tlag=%g\n", 
		i, ixcor[i], xcor[ixcor[i]], tlag[ixcor[i]] );
	 }
	}

	if( xcor[ixcor[3]] > cortol && fabs(tlag[ixcor[3]]) <= fabs(maxtimeshift) )
	{
		*time_shift_all = tlag[ixcor[3]];
	}
	else
	{
		*time_shift_all = 0;
	}
}
