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
#include "../../include/mt.h"

#define UNKNOWN_TAPER_TYPE -99
#define BARTLETT        0       /** b **/
#define HANNING         1       /** h **/
#define PARZAN          2       /** p **/

/*** char taper_type[3]; ***/

void taper( float *data, int nt, char *taper_type, float taper_frac_width )
{
	float taper, x;
	int j, itype, taper_length;

	if( taper_frac_width < 0.0 || taper_frac_width > 1.0 )
	{
	  printf("mytaper(): taper_frac_width=%g must be between 0 and 1\n", 
		taper_frac_width);
	  exit(-1);
	}

	itype=UNKNOWN_TAPER_TYPE;
	if( strncmp( taper_type, "b", 1 ) == 0 ) itype=BARTLETT;
        if( strncmp( taper_type, "h", 1 ) == 0 ) itype=HANNING;
        if( strncmp( taper_type, "p", 1 ) == 0 ) itype=PARZAN;

	taper_length = nt * taper_frac_width;
	switch( itype )
	{
		case BARTLETT:
			for(j=0; j<taper_length; j++)
			{
				taper = 1.0*(float)j/(float)(taper_length);
				data[j] *= taper;
				data[(nt-1)-j] *= taper;
			}
			break;

		case PARZAN:
			for(j=0; j<taper_length; j++)
			{
				x=1.0*(float)j/(float)(taper_length+1);
				if(x-0.5 <= 0)
					taper=6.0*x*x-6.0*x*x*x;
				else
					taper=1.0-2.0*pow((double)(1.0-x),(double)3.0);
				data[j] *= taper;
				data[(nt-1)-j] *= taper;
			}
			break;
	
		case HANNING:
			for(j=0; j<taper_length; j++)
			{
				taper = 0.5-0.5*cos(M_PI*(float)(j)/(float)(taper_length+1));
				data[j] *= taper;
				/**** for MTINV do not taper the backend ***/
				/* data[(nt-1)-j] *= taper; */
			}
			break;

		case UNKNOWN_TAPER_TYPE:
			printf("mytaper(): Unknwon Taper Type\n");
			break;
	}
	
}
