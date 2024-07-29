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
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "../include/mt.h"         /** global datatype and structure declarations **/
#include "../include/nrutil.h"     /** numerical recipes **/

extern char progname[128];

#define MAXDATAPTS 1000000

void sacdata2inv_loaddata( 
	EventInfo *ev,
        int nsta,
        char *sacdatadir,
        char *respdir,
        char *currentdir,
	int verbose )
{

/*** local variable declarations ***/
	int    ista;
	int    ifiles, number_files, n3cmp, found;
	char   sacfiles[256][2048];
	int    errno;

/*** local function prototypes ***/
	
	/*** sacdata2inv_subs.c ***/
	void getsacfiles(
                char *stnm,
                char *net,
		char *loc, 
                char *pathname,
                char sacfiles[256][2048],
                int *nfiles,
                int verbose );

        /*** sacdata2inv_subs.c ***/
        float *getsacdata(
                char *cmp,
                float *x,
                Sac_Header *sp,
                char *sacfile,
                char *filename,
                int *ifound,
                int verbose );

        /*** getrespfile_sub.c ***/
        void  getrespfile(
                char *pathname,
                char *sta,
                char *net,
		char *loc,
                char *cmp,
                char *khole,
                int verbose,
                char respfile[256] );

	/*** sacdata2inv_subs.c ***/
        void  writesacfile( EventInfo *ev );

	/*** transfer/sactransfer.c ***/
        void  transfer_response(
                float *data,
                int npts,
                float delta,
                char *sacpzfile,
                int verbose );

/*** start subroutine ***/

	for( ista = 0; ista < nsta; ista++ )
	{

/*********************************************************/
/*** given a directory path, get a file list of SAC    ***/
/*** files written from RDSEED                         ***/
/*********************************************************/
		if( verbose )
		{
       			fprintf( stdout,
			  "%s: sacdata2inv_loaddata(): calling getsacfiles()\n", 
				progname );
		}

		getsacfiles(
               		ev[ista].stnm,
               		ev[ista].net,
			ev[ista].loc,
               		sacdatadir,
               		sacfiles,
               		&number_files,
               		verbose );

       		if( verbose )
       		{
                	fprintf(stdout,
                   	"%s: sacdata2inv_loadata(): number of SAC files=%d for station=%s network=%s location=%s\n",
                        	progname, number_files, ev[ista].stnm, ev[ista].net, ev[ista].loc );

                	for( ifiles=0; ifiles<number_files; ifiles++ )
                	{
                        	fprintf( stdout, "%s: sacdata2inv_loadata(): ifiles=%d sacfile=%s\n",
                                	progname, ifiles, sacfiles[ifiles] );
                	}

        	}

/************************************************/
/*** go to the working directory              ***/
/************************************************/
        	if( (errno = chdir( sacdatadir )) != 0 )
        	{
                	fprintf( stderr, "%s: chdir errno=%d\n", progname, errno );
                	if( errno==-1 )
                	{
                        	fprintf(stderr, "%s: No Directory %s\n\n",
                                	progname, sacdatadir );
                	}
                	fprintf(stderr, "%s: Error in chdir %s\n",
                        	progname, sacdatadir );
        	}
        	if( verbose )
          		fprintf( stdout, "%s: sacdata2inv_loadata(): calling directory %s\n",
                		progname, sacdatadir );

/************************************************/
/*** load sac files into memory, figure out   ***/
/*** the component orientations               ***/
/************************************************/

        	if(verbose)
          		fprintf( stdout, "%s: sacdata2inv_loadata(): allocating memory to load data\n", progname );

        	ev[ista].z.data  = (float *)calloc(MAXDATAPTS,sizeof(float));
        	ev[ista].ns.data = (float *)calloc(MAXDATAPTS,sizeof(float));
        	ev[ista].ew.data = (float *)calloc(MAXDATAPTS,sizeof(float));


		if( strcmp( ev[ista].wavetype, "Surf/Pnl" ) == 0 )
		{
		  fprintf( stdout, "%s: %s: %s: loading %s data\n", progname, __FILE__, __func__, ev[ista].wavetype );

		  n3cmp = 0;
		  for( ifiles=0; ifiles < number_files; ifiles++ )
		  {
			found = 0;

			if( n3cmp >= 3 ) break;   /*** if more than 3 SAC files do not read the rest ***/

			ev[ista].ew.data = (float *)getsacdata( "EW",  ev[ista].ew.data, &(ev[ista].ew.s), sacfiles[ifiles], ev[ista].ew.filename, &found, verbose );
                         
			if(found) { n3cmp++; continue; }
       
			ev[ista].ns.data = (float *)getsacdata( "NS",  ev[ista].ns.data, &(ev[ista].ns.s), sacfiles[ifiles], ev[ista].ns.filename, &found, verbose );
			
			if(found) { n3cmp++; continue; }

			ev[ista].z.data  = (float *)getsacdata( "VER", ev[ista].z.data,  &(ev[ista].z.s), sacfiles[ifiles], ev[ista].z.filename, &found, verbose );
                                        
			if(found) { n3cmp++; continue; }

		  } /*** loop over files ***/

		  if(verbose)
		    fprintf(stdout, "%s: %s: %s: done! only n3cmp=%d will be read\n", progname, __FILE__, __func__, n3cmp );

              	  if( n3cmp < 3 )
               	  {
			fprintf( stderr, "%s: %s: %s: STDERR: FATAL ERROR only n3cmp=%d read 3 needed. Quitting.\n", progname, __FILE__, __func__, n3cmp );
                        fprintf( stdout, "%s: %s: %s: STDOUT: FATAL ERROR only n3cmp=%d read 3 needed. Quitting.\n", progname, __FILE__, __func__, n3cmp );
                       	exit(-1);
               	  }

		} /**** if the wavetype is translational seismogram "Surf/Pnl" *******/

/***********************************************************************************/
/*** G.Ichinose Nov  7 2019  - Added support for rotational GFs w1, w2, w3       ***/
/*** Rotational component about EW         x1-axis U -> w1 -> z -> z             ***/
/*** Rotational component about NS         x2-axis V -> w2 -> r -> ns            ***/
/*** Rotational component about z-vertical x3-axis W -> w3 -> t -> ew            ***/
/*** for now we will shoe-horn in the rotational into the ew,ns,z                ***/
/***********************************************************************************/
/***
U -> w1  z z
V -> w2  r ns
W -> w3  t ew
***/
		if( strcmp( ev[ista].wavetype, "Rotational" ) == 0 )
		{

		  fprintf( stdout, "%s: %s: %s: loading %s data\n", progname, __FILE__, __func__, ev[ista].wavetype );

		  n3cmp = 0;

		  for( ifiles=0; ifiles < number_files; ifiles++ )
		  {
			found = 0;

			if( n3cmp >= 3 ) break;

			ev[ista].z.data = (float *)getsacdata( "W1", ev[ista].z.data, &(ev[ista].z.s), sacfiles[ifiles], ev[ista].z.filename, &found, verbose );
			if(found) { if(verbose) fprintf(stdout, "%s: %s: %s: found cmp=%s filename=%s\n", progname, __FILE__, __func__, "W1", ev[ista].z.filename ); }
			if(found) { n3cmp++; continue; }

			ev[ista].ns.data = (float *)getsacdata( "W2", ev[ista].ns.data, &(ev[ista].ns.s), sacfiles[ifiles], ev[ista].ns.filename, &found, verbose );
			if(found) { if(verbose) fprintf(stdout, "%s: %s: %s: found cmp=%s filename=%s\n", progname, __FILE__, __func__, "W2", ev[ista].ns.filename ); }
			if(found) { n3cmp++; continue; }

			ev[ista].ew.data  = (float *)getsacdata( "W3", ev[ista].ew.data,  &(ev[ista].ew.s),  sacfiles[ifiles], ev[ista].ew.filename,  &found, verbose );
			if(found) { if(verbose) fprintf(stdout, "%s: %s: %s: found cmp=%s filename=%s\n", progname, __FILE__, __func__, "W3", ev[ista].ew.filename ); }
			if(found) { n3cmp++; continue; }

		  } /*** loop over ifiles ***/

		  if(verbose) 
			fprintf(stdout, "%s: %s: %s: done! only n3cmp=%d will be read\n", progname, __FILE__, __func__, n3cmp );

		  if( n3cmp < 3 )
		  {
			fprintf( stderr, "%s: %s: %s: STDERR: FATAL ERROR only n3cmp=%d rotational read 3 needed. Quitting.\n", progname, __FILE__, __func__, n3cmp );
                        fprintf( stdout, "%s: %s: %s: STDOUT: FATAL ERROR only n3cmp=%d rotational read 3 needed. Quitting.\n", progname, __FILE__, __func__, n3cmp );
                        exit(-1);
		  }

		}   /**** if the wavetype is rotational seismogram "Rotational" *******/

/********************************************************************************/
/*** give a warning that more than 3 sac files exist for this station.network ***/
/*** the files are alphabetically sorted so typical the first 3 are data      ***/
/*** while the others are later arriving packets (see mergesac.c in misc)     ***/
/********************************************************************************/

        	if( n3cmp > 3 )
        	{
          	  fprintf(stdout, 
            	  "%s: %s: %s: WARNING! more than 3 components found for station=%s.%s(ista=%d).\n",
                	progname, __FILE__, __func__, ev[ista].stnm, ev[ista].net, ista );
        	}

/********************************************************/
/*** find the SAC_PZs files in the Response directory ***/
/********************************************************/

		chdir( currentdir );

		if(verbose)
               	  fprintf( stdout, "%s: %s: %s: cwd=%s\n\t looking for SAC_PZs files in dir=%s\n",
                        progname, __FILE__, __func__, currentdir, respdir );

		sprintf( ev[ista].ew.sacpzfile, " " );
                sprintf( ev[ista].ns.sacpzfile, " " );
                sprintf( ev[ista].z.sacpzfile, " " );

		getrespfile(
                	respdir,
                	ev[ista].stnm,
                	ev[ista].net,
			ev[ista].loc,
                	ev[ista].ew.s.kcmpnm,
                	ev[ista].ew.s.khole,
                	verbose,
                	ev[ista].ew.sacpzfile );
	
		getrespfile(
                	respdir,
                	ev[ista].stnm,
                	ev[ista].net,
			ev[ista].loc,
                	ev[ista].ns.s.kcmpnm,
                	ev[ista].ns.s.khole,
                	verbose,
                	ev[ista].ns.sacpzfile );
	
		getrespfile(
                	respdir,
                	ev[ista].stnm,
                	ev[ista].net,
			ev[ista].loc,
                	ev[ista].z.s.kcmpnm,
                	ev[ista].z.s.khole,
                	verbose,
                	ev[ista].z.sacpzfile );

		fprintf( stdout,
          		"%s: %s: %s: Response files =\n \tew=(%s)\n \tns=(%s)\n \t z=(%s)\n",
                	progname, __FILE__, __func__,
                	ev[ista].ew.sacpzfile,
                	ev[ista].ns.sacpzfile,
                	ev[ista].z.sacpzfile );
		
	/*** reset the current directory for writting output files ***/

        	chdir( currentdir );

	/*** check the sampling rate, otherwise there will be some bad stuff in interpolate_fft()  ***/
	
		if( ev[ista].dt < ev[ista].z.s.delta ) 
		{
			fprintf( stderr, "\n\n%s: %s: %s:\n", progname, __FILE__, __func__ );
			fprintf( stderr, "%s: %s: %s: STDERR: FATAL ERROR: %s.%s.%s the sampling rate %g (sec/samp) of the SAC data is greater than requested in mtinv.par file dt=%g (sec/samp)\n",
				progname, __FILE__, __func__, ev[ista].net, ev[ista].stnm, ev[ista].loc, ev[ista].z.s.delta, ev[ista].dt );
			fprintf( stderr, "%s: %s: %s:\n\n\n", progname, __FILE__, __func__ );
			fflush(stderr);

			fprintf( stdout, "\n\n%s: %s: %s:\n", progname, __FILE__, __func__ );
                        fprintf( stdout, "%s: %s: %s: STDOUT: FATAL ERROR: %s.%s.%s the sampling rate %g (sec/samp) of the SAC data is greater than requested in mtinv.par file dt=%g (sec/samp)\n",
                                progname, __FILE__, __func__, ev[ista].net, ev[ista].stnm, ev[ista].loc, ev[ista].z.s.delta, ev[ista].dt );
                        fprintf( stdout, "%s: %s: %s:\n\n\n", progname, __FILE__, __func__ );
                        fflush(stdout);

			exit(-1);
		}

	} /*** loop over ista ***/

	fprintf( stdout, "%s: %s: %s: done loading\n\n", progname, __FILE__, __func__ );
}
