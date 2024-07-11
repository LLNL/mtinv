/*** this is for testing calls to taper_subs.c ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../include/mt.h"

#define UNKNOWN_TAPER_TYPE -99
#define BARTLETT        0       /** b **/
#define HANNING         1       /** h **/
#define PARZAN          2       /** p **/

int main( int ac, char **av )
{
	Sac_File sacfile;
	int verbose=0;
	char taper_type[3];
	float taper_frac_width;
	FILE *fp;
	void taper( float *, int, char *, float );

	setpar(ac,av);
	mstpar("f",	"s",	&(sacfile.filename));
	mstpar("type",	"s",	&taper_type );
	mstpar("width",	"f",	&taper_frac_width);
	getpar("verbose", "d",	&verbose );
	endpar();

	fp = fopen(sacfile.filename, "rb" );
	fread( &(sacfile.s), sizeof(Sac_Header), 1, fp );
	sacfile.data = (float *)calloc( sacfile.s.npts, sizeof(float) );
	fread( &sacfile.data[0], sacfile.s.npts*sizeof(float), 1, fp );
	fclose(fp);

	taper( &(sacfile.data[0]), sacfile.s.npts, taper_type, taper_frac_width );

/*** write the sac file with the o and ko marker ***/
	fp = fopen( "out.sac", "wb" );
	fwrite( &(sacfile.s), sizeof(Sac_Header), 1, fp );
	fwrite( &(sacfile.data[0]), sacfile.s.npts*sizeof(float), 1, fp );
	fclose(fp);

	free(sacfile.data);
}
