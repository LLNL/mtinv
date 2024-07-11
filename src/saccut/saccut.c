#include <stdio.h>
#include <stdlib.h>
#include "../../include/mt.h"

char progname[256];

int main( int ac, char **av )
{
	Sac_File sacfile;
	MyTime t;
	FILE *fp;
	char timestring[24], kmarker[4];
	int verbose=0, mode;
	float precut, twincut;

	int setpar(int,char **),mstpar(),getpar();
	void endpar();
	float *cut_sac(Sac_File *, float, float, char *, int );
	void set_sac_time_marker( int, Sac_File *, char *, MyTime *, char *, int );
	void parsestring( MyTime *t, char *str );

/*** begin main ***/

	setpar(ac,av);
	mstpar("f",  "s", &(sacfile.filename));
	mstpar("ot", "s", timestring );
	mstpar("pre", "f", &precut );
	mstpar("twin", "f", &twincut );
	mstpar("mark", "s", &kmarker );
	getpar("verbose", "d", &verbose );
	endpar();

	if( verbose )
	{
	  printf("twin=%g kmarker=%s\n\tot=%s\n\topening file=%s\n", 
		twincut, kmarker, timestring, sacfile.filename);
	}

	if( (fp = fopen(sacfile.filename, "rb" )) == NULL )
	{
		fprintf(stderr, "cannot open file %s\n", sacfile.filename );
		exit(-1);
	}
	fread( &(sacfile.s), sizeof(Sac_Header), 1, fp );
	sacfile.data = (float *)calloc( sacfile.s.npts, sizeof(float) );
	fread( &sacfile.data[0], sacfile.s.npts*sizeof(float), 1, fp );
	fclose(fp);

	if( verbose )
	{
	  printf( "nt=%d dt=%g b=%g\n", sacfile.s.npts, sacfile.s.delta, sacfile.s.b );
	}

	parsestring( &t, timestring );
	mode = 0; /*** by time string    ***/
	mode = 1; /*** by time structure ***/
	set_sac_time_marker( mode, &sacfile, timestring, &t, kmarker, verbose );

/*** cut relative to kmarker and realloc the data memory ***/
	sacfile.data = (float *)cut_sac( &sacfile, precut, twincut, kmarker, verbose );
 
/*** write the sac file with the o and ko marker ***/
	fp = fopen( "out.sac", "wb" );
	fwrite( &(sacfile.s), sizeof(Sac_Header), 1, fp );
	fwrite( &(sacfile.data[0]), sacfile.s.npts*sizeof(float), 1, fp );
	fclose(fp);
        free(sacfile.data);
}
