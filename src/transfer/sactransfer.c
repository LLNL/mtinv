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
