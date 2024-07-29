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

/**** differnetiation : computes f(t)=x dx/dt ***/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../include/mt.h"

/* void differentiate( float *x, int n, float dt, int op ) */

void  differentiate( float *x, int n, float dt, int op, int verbose )
{
	int i;
	float *z;
	float half=0.5;
	float twothird    = 0.66666666667;
	float onetwelveth = 0.08333333333;	
	z = (float *) calloc( n, sizeof(float));
	if( op == 2 ) /** 2 point operator **/
	{
		for( i=0; i<n-1; i++ )
			z[i] = (x[i+1]-x[i])/dt;
		z[n-1]=0;
	}
	if( op == 3 )
	{
		for( i=1; i<n-1; i++ )
			z[i] = half*((x[i+1]-x[i])/dt);
		z[0] = 0;
		z[n-1] = 0;
	}
	if( op == 5 )
	{
		for( i=2; i<n-2; i++ )
		{
			z[i] = twothird*((x[i+1]-x[i-1])/dt) + 
				onetwelveth* ((x[i+2]-x[i-2])/dt);
		}
		z[0] = 0;
		z[1] = 0;
		z[n-2] = 0;
		z[n-1] = 0;
	}
	for( i=0; i<n; i++ ) x[i]=z[i];
	free(z);
	return;
}
