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
 *** merges waveform segments stored in multiple SAC files into one trace, overwrites first file or out.sac
 *** gaps written as zeros
 ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "version.h"
#include "sacfile.h"

char progname[128];

int main(int ac, char **av)
{
	SacFile *sf;
	int i, nfiles = 0;
	double mintime, maxtime, totaltime;
	
	float dt;
	float *data;
	int rank = 1;    /** this is the sac file that is the 1st (earliest start time) ***/
	int j, it, ndata = 0;
	char output_sac_filename[128];

	int verbose = 1; /*** for debug ***/

/*** function prototypes ***/

	void readsacfile( SacFile *sf, int verbose );
	void set_sac_minmax( Sac_Header *s, float *data );
	void WriteMyTime2STDOUT( MyTime *t );
	int cvt2time( double minepoch, double maxepoch, double myepoch, float dt );
	void remove_mean( float *, int );
	void wrtoldsac( char *FO, Sac_Header *s, float *data );

/*** start program ***/

	/* snprintf( progname, sizeof(progname), av[0] ); */
	/* sprintf( progname, av[0] ); */
	strcpy( progname, av[0] );

	fprintf( stderr, "%s: %s: %s: Version      = %s\n", progname, __FILE__, __func__, Version_Label );
        fprintf( stderr, "%s: %s: %s: Version Date = %s\n", progname, __FILE__, __func__, Version_Date );
        fprintf( stderr, "%s: %s: %s: Source       = %s\n", progname, __FILE__, __func__, Version_SrcDir );

	sf = malloc(sizeof(SacFile));
	maxtime = 0;
	mintime = 9e+25;

	for( i=1; i<ac; i++ )
	{
		sf = realloc( sf, (i+1)*sizeof(SacFile) );
		
		/* snprintf( sf[i].filename, sizeof(sf[i].filename), av[i] ); */
		/* sprintf( sf[i].filename, av[i] ); */
		strcpy( sf[i].filename, av[i] );

		if(verbose)	
		{
		  fprintf(stderr, "%s: %s: %s: opening file i=%d filename=%s\n",
			progname, __FILE__, __func__, i, sf[i].filename );
		}

		readsacfile( &sf[i], 0 /*verbose*/ );

		if( verbose )
		{
			fprintf( stdout, "%s: %s: %s: %s nt=%d dt=%g b=%g e=%g dur=%g \n",
				progname,
                                __FILE__,
                                __func__,
                                sf[i].filename,
                                sf[i].s.npts,
                                sf[i].s.delta,
                                sf[i].s.b,
                                sf[i].s.e, 
                                (sf[i].s.e - sf[i].s.b) );
                        
			fprintf( stdout, "%s: %s: %s: start=", progname, __FILE__, __func__ );
                        WriteMyTime2STDOUT( &(sf[i].beg) );

			fprintf( stdout, "%s: %s: %s:  stop=", progname, __FILE__, __func__ );
                        WriteMyTime2STDOUT( &(sf[i].end) );
                }

	/** rank: find the sac file with minimum(earliest) start time */

		if( sf[i].beg.epoch < mintime )
		{
			mintime = sf[i].beg.epoch;
			rank = i;
		}

		if( sf[i].end.epoch > maxtime )
		{
			maxtime = sf[i].end.epoch;
		}
		
		nfiles++;

	} /*** loop over all command line arguments ***/

	if( nfiles == 0 ) 
	{
		fprintf( stderr, "%s: %s: %s: Error no files read\n",
                        progname, __FILE__, __func__ );
		exit(-1);
	}

	dt = sf[1].s.delta;
	totaltime = maxtime - mintime;
	ndata = (int) (totaltime / dt);

	if(verbose)
        {
          fprintf( stderr, "%s: %s: %s: nfiles=%d total time = %lf sec / %lf hrs ndata=%d dt=%g rank=%d(ptr to first file header)\n",
                progname, __FILE__, __func__, nfiles, totaltime, totaltime/3600., ndata, dt, rank );
        }

	data = calloc( ndata, sizeof(float) );

	for( i=1; i<=nfiles; i++ )
	{
		j = cvt2time( mintime, maxtime, sf[i].beg.epoch, dt );

		if(verbose)
		{
		  fprintf(stderr, "%d %s mintime=%lf maxtime=%lf b=%g myepoch=%lf j=%d\n",
                        i,
                        sf[i].filename,
                        mintime,
                        maxtime,
                        sf[i].s.b,
                        sf[i].beg.epoch,
                        j );
		}

		/*** for helicorder version, remove the mean ***/
		/* remove_mean( &(sf[i].data[0]), sf[i].s.npts ); */

		for( it=0; it<sf[i].s.npts; it++ )
		{
			data[j] = sf[i].data[it];
			j++;
		}

	} /*** loop over each file ***/

	sf[rank].s.npts = ndata;
	sf[rank].s.b    = sf[rank].s.b;
	sf[rank].s.e    = rint( ndata * dt );

	/*** for helicorder version, don't remove the mean ***/
	if(verbose)fprintf(stderr, "%s: %s: %s: removing mean\n", progname, __FILE__, __func__ );
	remove_mean( &data[0], ndata );

	if(verbose)fprintf(stderr, "%s: %s: %s: setting SAC minmax\n", progname, __FILE__, __func__ );
	set_sac_minmax( &(sf[rank].s), &data[0] );

	if( strncmp( sf[rank].s.knetwk, "-12345", 6 ) == 0 || strcmp( sf[rank].s.knetwk, " " ) == 0 ) snprintf( sf[rank].s.knetwk, sizeof(sf[rank].s.knetwk), "NETWK" );
        if( strncmp( sf[rank].s.kstnm,  "-12345", 6 ) == 0 || strcmp( sf[rank].s.kstnm,  " " ) == 0 ) snprintf( sf[rank].s.kstnm,  sizeof(sf[rank].s.kstnm), "NOSTA" );
        if( strncmp( sf[rank].s.khole,  "-12345", 6 ) == 0 || strcmp( sf[rank].s.khole,  " " ) == 0 ) snprintf( sf[rank].s.khole,  sizeof(sf[rank].s.khole),  "" );
        if( strncmp( sf[rank].s.kcmpnm, "-12345", 6 ) == 0 || strcmp( sf[rank].s.kcmpnm, " " ) == 0 ) snprintf( sf[rank].s.kcmpnm, sizeof(sf[rank].s.kcmpnm), "CHAN" );

	if(verbose) fprintf( stderr, "%s: %s: %s: creating output filename\n", progname, __FILE__, __func__ );

        snprintf( output_sac_filename, sizeof(output_sac_filename), "%4d.%03d.%02d.%02d.%02d.%04d.%s.%s.%s.%s.D.SAC",
                sf[rank].s.nzyear,
                sf[rank].s.nzjday,
                sf[rank].s.nzhour,
                sf[rank].s.nzmin,
                sf[rank].s.nzsec,
                sf[rank].s.nzmsec,
                sf[rank].s.knetwk,
                sf[rank].s.kstnm,
                sf[rank].s.khole,
                sf[rank].s.kcmpnm );

	if(verbose) fprintf( stderr, "%s: %s: %s: output filename %s\n", progname, __FILE__, __func__, output_sac_filename );

/************************************/
/*** check if file already exists ***/
/************************************/

        if( access( output_sac_filename, F_OK ) == 0 )
        {
                fprintf( stderr, "%s: %s: %s: %s already exists, writting output to out.sac instead\n",
                        progname, __FILE__, __func__, output_sac_filename );
                snprintf( output_sac_filename, sizeof(output_sac_filename), "out.sac" );
        }

        fprintf( stderr, "%s: %s: %s: writting %s nt=%d dt=%g b=%g e=%g dur=%g min=%g max=%g mean=%g\n",
                progname,
                __FILE__,
                __func__,
                output_sac_filename,
                sf[rank].s.npts,
                sf[rank].s.delta,
                sf[rank].s.b,
                sf[rank].s.e,
                (sf[rank].s.e - sf[i].s.b),
                sf[rank].s.depmin,
                sf[rank].s.depmax,
                sf[rank].s.depmen );

        wrtoldsac( output_sac_filename, &(sf[rank].s), &data[0] );

        free(data);
        exit(0);

} /*** end of sacmerge.c: main() ***/

int cvt2time( double minepoch, double maxepoch, double myepoch, float dt )
{
        int ipt=0;
        ipt = (int)(( myepoch - minepoch )/dt);
        return ipt;
}
