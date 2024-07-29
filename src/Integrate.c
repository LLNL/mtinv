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

/***
 Performs integration using the trapezoidal rule
 This command uses the trapezoidal integration method.  The first
 output data point is set to zero and the number of points is
 reduced by one.  The data does not have to be evenly spaced.

 Tested against SAC for accuracy.
****/

#include <stdio.h>
#include <stdlib.h>

void integrate( float *x, float dt, int *nt, float *beg )
{
	float sum = 0, step;
	int i, npts;
	float *y;

	npts = *nt - 1;
	step = dt/2;
	y = calloc( npts, sizeof(float) );

	for( i = 0; i < npts; i++ )
	{
		sum += step * ( x[i+1] + x[i] );
		y[i] = sum;
	}

	for( i = 0; i < npts; i++ ) x[i] = y[i];
	y[*nt] = 0;
	x[*nt] = 0;

	*beg = *beg + (dt/2.0);
        *nt = npts;

	free(y);
	return;
}
