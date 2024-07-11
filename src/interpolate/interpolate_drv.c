#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include "../../include/sac.h"

typedef struct {
	Sac_Header hdr;
	float *data;
	char filename[128];
} Sac_File;

int main( int ac, char **av )
{
	Sac_File *sf;
	FILE *fp;
	int i, j, nfiles=0, old_npts, new_npts;
	float old_delta, new_delta;
	void interpolate_fft( float *, int, float, int *, float );
	int setpar(),mstpar();
	void endpar();

/*** begin main ***/
	setpar(ac,av);
	mstpar("dt", "f", &new_delta );
	endpar();

	fprintf(stderr, "%s: dt=%g\n", av[0], new_delta );
	sf = (Sac_File *)malloc(ac*sizeof(Sac_File));
	for( j=1; j<ac; j++ )
	{
		if( (fp=fopen(av[j],"r"))==NULL )continue;
/* allocate array space read data */
		fprintf(stderr, "%s: %s\n", av[0], av[j] );
		strcpy( sf[nfiles].filename, av[j] );
		fread(&sf[nfiles].hdr, sizeof(Sac_Header), 1, fp);
		sf[nfiles].data = (float *)calloc( sf[nfiles].hdr.npts, sizeof(float) );
		fread( &sf[nfiles].data[0], sf[nfiles].hdr.npts * sizeof(float), 1, fp );
		fclose(fp);

		old_npts = sf[nfiles].hdr.npts;
		old_delta = sf[nfiles].hdr.delta;
		interpolate_fft( &sf[nfiles].data[0], old_npts, old_delta, &new_npts, new_delta );

/* change appropriate header values */
		sf[nfiles].hdr.npts = new_npts;
		sf[nfiles].hdr.delta = new_delta;
		sf[nfiles].hdr.e = sf[nfiles].hdr.b + sf[nfiles].hdr.npts*sf[nfiles].hdr.delta;

/* write header and data */
		sprintf( sf[nfiles].filename, "%s.int", sf[nfiles].filename );
		fp = fopen( sf[nfiles].filename, "wb");
		fwrite( &sf[nfiles].hdr, sizeof(Sac_Header), 1, fp);
		fwrite( &sf[nfiles].data[0], sf[nfiles].hdr.npts*sizeof(Sac_Header), 1, fp);
		fclose(fp);

/* free array space */
		nfiles++;
	}
	if( nfiles > 0 ) free(sf);
}
