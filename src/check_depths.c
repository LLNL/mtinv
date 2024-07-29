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
#include "../include/mt.h"
extern char progname[128];

/********************************************************************************/
/*** used by mtinv.c, mteig.c, forward_calc_from_MT.c - checks user selected  ***/
/***    MT depth is in the Greens function library *.glib or *.ginv files     ***/
/********************************************************************************/

void check_depth( float FixMyZ, int *FixMyiz, float *z, int nz, int verbose )
{
	int is_my_z_valid( float, float *, int );

	if(verbose)
	{
		fprintf( stderr, "%s: %s: %s: searching for FixMyZ=%g nz=%d\n",
			progname, __FILE__, __func__, FixMyZ, nz );
	}

	if( ( *FixMyiz = is_my_z_valid( FixMyZ, z, nz )) == -999 )
	{
		fprintf( stderr,
			"%s: %s: %s: force_z=%g is ***NOT*** a valid depth in ginv files\n",
			progname, __FILE__, __func__, FixMyZ );
		exit(-1);
	}
	else
	{
		if(verbose)
		{
			fprintf( stderr, "%s: %s: %s: fixz=%g iz=%d is a valid depth in ginv files\n",
				progname, __FILE__, __func__, FixMyZ, *FixMyiz );
		}
	}
}

/*** macro only used above ***/

int is_my_z_valid( float myz, float *z, int nz )
{
        int iz, myiz;
        for( iz=0; iz<nz; iz++ )
        {
                if( myz == z[iz] )
                {
                        myiz = iz;
                        return myiz;
                }
        }
        return -999;
}
