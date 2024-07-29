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
#include "../../include/sac.h"
#include "../../include/complex.h"

int main( int ac, char **av )
{	
	Sac_Header sp;
	float *data;
	FILE *fp;
	char filename[128], sacpzfile[128];
	int verbose=0; /** default not verbosey **/
	int setpar(int,char **);
	int mstpar(), getpar();
	void endpar();
	void transfer_response( float *, int, float, char *, int );

	setpar(ac,av);
	mstpar("f","s",&filename);
	mstpar("pz", "s", &sacpzfile);
	getpar("verbose", "d", &verbose );
	endpar();

	fp = fopen(filename,"rb");
	fread(&sp, sizeof(Sac_Header), 1, fp);
	data = (float *)malloc(sizeof(float)*sp.npts);
	fread( &data[0], sizeof(float)*sp.npts, 1, fp );
	fclose(fp);

	transfer_response( data, sp.npts, sp.delta, sacpzfile, verbose );

	sprintf(filename,"%s.cor", filename);
	printf("writting file = %s\n", filename);
	fp = fopen(filename,"wb");
	fwrite(&sp, sizeof(Sac_Header), 1, fp);
	fwrite( &data[0], sizeof(float)*sp.npts, 1, fp );
	fclose(fp);

	free(data);
}
