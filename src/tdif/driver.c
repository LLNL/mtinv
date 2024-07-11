#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../include/mt.h"
                                                                                                                         
int main( int ac, char **av )
{
        Sac_File sacfile;
        int verbose=1;
        int operator=2;
        FILE *fp;
        void differentiate( float *, int, float, int );
                                                                                                                         
        setpar(ac,av);
        mstpar( "f", "s", &(sacfile.filename) );
        getpar( "verbose", "b", &verbose );
        getpar( "op", "d", &operator );
        endpar();
                                                                                                                         
        fp = fopen( sacfile.filename, "rb" );
        fread( &(sacfile.s), sizeof(Sac_Header), 1, fp );
        sacfile.data = (float *)calloc( sacfile.s.npts, sizeof(float));
        fread( &sacfile.data[0], sacfile.s.npts*sizeof(float), 1, fp );
        fclose(fp);
                                                                                                                         
        differentiate( &(sacfile.data[0]), sacfile.s.npts, sacfile.s.delta, operator );
                                                                                                                         
        fp = fopen( "out.sac", "wb" );
        fwrite( &(sacfile.s), sizeof(Sac_Header), 1, fp );
        fwrite( &(sacfile.data[0]), sacfile.s.npts*sizeof(float), 1, fp );
        fclose(fp);
                                                                                                                         
        free(sacfile.data);
        exit(0);
}
