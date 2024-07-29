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

void source_time_function( float *data, int nt, float dt, float tr, float tt )
{
	int nrise, ntop;
	float *data0, *data1, *data2;
	void boxconv( int, int, float *, float * );
	void shiftfwd( int, float *, float * );
	void shiftback( int, float *, float * );

	if( tr == 0 && tt == 0 ) return;

	nrise = (int)rint(tr/dt);
	ntop  = (int)rint(tt/dt);
	/* fprintf(stdout, "source_time_function: nrise=%d ntop=%d\n", nrise, ntop); */

	data0 = (float *) calloc(2*nt, sizeof(float));
	data1 = (float *) calloc(2*nt, sizeof(float));
	data2 = (float *) calloc(2*nt, sizeof(float));

	shiftfwd( nt, data, data0 );
	boxconv( nrise, nt, data0, data1 );
	boxconv( nrise+ntop, nt, data1, data2 );
	shiftback( nt, data2, data );

	free(data0);
	free(data1);
	free(data2);
}

/****************************************************
BOXCONV()
inputs : xx(1:nx) - data
         nb = boxcar length    nb = twin/dt
outputs: y(1,nx+nb-1)
*****************************************************/
void boxconv( int nb, int nx, float *xx, float *yy )
{
	int ny, i;
	float *bb;
	bb = (float *) calloc( nx+nb+1, sizeof(float) );
	
	if( nb<1 || nb>nx )
	{
		printf("boxcar: nb < 1 or nb > nx nb=%d nx=%d\n", nb, nx );
		exit(-1);
	}
	ny = nx+nb-1;
	bb[1]=xx[1];
	bb[1]=xx[1];
	for(i=2; i<=nx; i++)
		bb[i] = bb[i-1] + xx[i]; /*** make B(Z) = X(Z)/(1-Z) ***/
	for( i=nx+1; i<=ny; i++ )
		bb[i] = bb[i-1];
	for( i=1; i<=nb; i++ )
		yy[i] = bb[i];
	for( i=nb+1; i<=ny; i++ )
		yy[i] = bb[i] - bb[i-nb];  /***  make Y(Z) = B(Z) * (1-Z^nb) ***/
	for( i=1; i<=ny; i++ ) yy[i] = yy[i]/nb;
	free(bb);
}

/*** convert from uu[0:nt-1]  ==> uu[1:nt] ***/
void shiftfwd( int nt, float *uu, float *bb )
{
        int i;
        for(i=0; i<=nt; i++) bb[i+1] = uu[i];
}

/*** convert from uu[1:nt]    ==> uu[0:nt-1] ***/
void shiftback( int nt, float *bb, float *uu )
{
        int i;
        for(i=1; i<=nt; i++) uu[i-1] = bb[i];
}
