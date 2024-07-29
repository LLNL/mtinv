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

#include "../../include/sac.h"
#include "../../include/complex.h"

int main(int ac, char **av)
{
	Sac_Header sp;
	float *data;
	FILE *fp;
	char filename[128];
	Dcomplex *samp;
	int npts;
	float *am, *ph;
	float df;
	void fft( int, Dcomplex *, double, double );
	int get_pow_2(int);
	int i;
	int setpar(),mstpar();
	void endpar();

/*** begin main ***/
	setpar(ac,av);
	mstpar("f","s",filename);
	endpar();

	fp = fopen(filename,"rb");
	fread(&sp, sizeof(Sac_Header), 1, fp);
	data = (float *)malloc(sizeof(float)*sp.npts);
	fread( &data[0], sizeof(float)*sp.npts, 1, fp );
	fclose(fp);
	printf("%s: npts=%d dt=%g\n", filename, sp.npts, sp.delta );

	npts = get_pow_2(sp.npts);
	df = 1.0 / ( sp.delta * npts );
	printf("npts=%d df=%g\n", npts, df );

	samp = (Dcomplex *)calloc(npts, sizeof(Dcomplex));
	for( i = 0; i < sp.npts; i++) 
	{
		samp[i].re = (double)data[i];
		samp[i].im = 0.0;
	}

	fft( npts, &samp[0], FORWARD, sp.delta );

	am = (float *)calloc(npts, sizeof(float));
	ph = (float *)calloc(npts, sizeof(float));

	for( i=0; i<npts; i++)
		am[i] = (float)sqrt( (samp[i].re*samp[i].re+samp[i].im*samp[i].im));

	for( i=0; i<npts; i++)
		ph[i] = atan( -samp[i].im / samp[i].re );

	sp.npts = npts;
        sp.delta = df;

	sprintf(filename,"%s.amp.sac", filename);
	printf("writting file = %s\n", filename);
        fp = fopen(filename,"wb");
        fwrite(&sp, sizeof(Sac_Header), 1, fp);
        fwrite( &am[0], sizeof(float)*sp.npts, 1, fp );
        fclose(fp);

	sprintf(filename,"%s.pha.sac", filename);
	printf("writting file = %s\n", filename);
        fp = fopen(filename,"wb");
        fwrite(&sp, sizeof(Sac_Header), 1, fp);
        fwrite( &ph[0], sizeof(float)*sp.npts, 1, fp );
        fclose(fp);
}
