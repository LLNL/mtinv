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

#include <math.h>

void sac_minmax( float *x, int n, float *max, float *min, float *mean )
{
        int i;
        float sum=0;
        *max = x[0];
        *min = x[0];
        *mean = 0;
        for( i=0; i<n; i++ )
        {
                if( x[i] > *max ) *max=x[i];
                if( x[i] < *min ) *min=x[i];
                sum = sum + x[i];
        }
        *mean = sum/n;
}

void sac_absmax( float *x, int n, float *absmax )
{
	int i;
	*absmax = fabs( x[0] );
	for( i=0; i<n; i++ )
	{
		if( fabs( x[i] ) > *absmax ) 
			*absmax = fabs( x[i] );
	}
}

/*** THESE WERE FOUND IN SACIO/SACIO.C ***/

/*** duplicate in sacextreama.c ***/
/*
void sac_absmax( float *x, int n, float *absmax )
{
        int i;
        *absmax = fabs( x[0] );
        for( i=0; i<n; i++ )
        {
                if( fabs( x[i] ) > *absmax )
                        *absmax = fabs( x[i] );
        }
}
*/

/*** duplicate in sacextreama.c ***/
/*
void sac_minmax( float *x, int n, float *max, float *min, float *mean )
{
        int i;
        float sum=0;
        *max = x[0];
        *min = x[0];
        *mean = 0;
        for( i=0; i<n; i++ )
        {
                if( x[i] > *max ) *max=x[i];
                if( x[i] < *min ) *min=x[i];
                sum = sum + x[i];
        }
        *mean = sum/n;
}
*/

