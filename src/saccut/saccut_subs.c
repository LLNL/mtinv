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
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "../../include/mt.h"

/*** sac time markers ***/
#define O_MARKER 0
#define A_MARKER 1
#define T0_MARKER 2
#define T1_MARKER 3
#define T2_MARKER 4
#define T3_MARKER 5
#define T4_MARKER 6
#define T5_MARKER 7
#define T6_MARKER 8
#define T7_MARKER 9
#define T8_MARKER 10
#define T9_MARKER 11
#define UNKNOWN_MARKER -99

/****************************************************************/
/*** cut a sac file by twincut relative to kmarker time maker ***/
/****************************************************************/
/***
	bugs: 
	8/4/2006 can the code handle negative tbeg and nbeg?   
	bug fixed 6/14/2007 adjust reference time with b.hdr value
	to get the new begin time of the first time point in sac file
***/
/**************************************************************************/

extern char progname[128];

float *cut_sac( Sac_File *sacfile, float precut, float twincut, char *kmarker, int verbose )
{
	float *x, tbeg, sum;
	int i, jj, kk, nbeg, ncut;
	int imarker;

	int set_marker( char * );
	void sac2time( MyTime *, Sac_Header * );
	void time2sac( MyTime *, Sac_Header * );
	void sac_minmax( float *, int, float *, float *, float * );
	void parsestring( MyTime *, char * );
	int julday( int, int, int );

/***********************************/
/*** check for valid time marker ***/
/***********************************/
	if( (imarker = set_marker( kmarker )) == UNKNOWN_MARKER )
	{
		printf("%s: cut_sac(): Unknown SAC Time Marker:\n\t imarker=%d kmarker=%s\n", 
			progname, imarker, kmarker );
		exit(-1);
	}

	if(verbose)printf("%s: cut_sac(): \t precut=%g twincut=%g ", progname, precut, twincut );

	if(verbose && sacfile->s.iztype != -12345 )
		printf("%s: reference time marker is %d", progname, sacfile->s.iztype );

	if(verbose)printf("\n");

/**************************************************************************/
/*** determine the begining point for cut in time and points set        ***/
/*** the reference time in the sac file by this time                    ***/
/**************************************************************************/
	switch(imarker)
	{
		case O_MARKER:
			if( sacfile->s.o == -12345. ) 
			{
				printf("ERROR %s time marker not set in file %s\n", kmarker, sacfile->filename );
				exit(-1);
			}
			tbeg = sacfile->s.b + ( sacfile->ot.epoch - sacfile->beg.epoch ) - precut;

		/**************************************************************************/
		/*** reset the reference time marker offset to begining of file ***/
		/**************************************************************************/
			sacfile->beg.epoch = sacfile->ot.epoch - precut;
			epoch2time( &(sacfile->beg), sacfile->beg.epoch );

			if( verbose )
			{
				fprintf(stdout, "%s: ot=%15.2f b=%15.2f precut=%15.2f tbeg=%g\n",
					progname, sacfile->ot.epoch, sacfile->beg.epoch, precut, tbeg );
				fflush(stdout);

				fprintf(stdout, "%s: ot =", progname );
				WriteMyTime2STDOUT( &(sacfile->ot) );
				fflush(stdout);

				fprintf(stdout, "%s: beg=", progname );
				WriteMyTime2STDOUT( &(sacfile->beg) );
				fflush(stdout);
			}

			time2sac( &(sacfile->beg), &(sacfile->s) );
			sacfile->s.o = precut;
			break;

		case A_MARKER:
			if( sacfile->s.a == -12345. )
			{
				printf("ERROR %s time marker not set in file %s\n", kmarker, sacfile->filename );
				exit(-1);
			}
			tbeg = sacfile->a.epoch - sacfile->beg.epoch - precut;

			sacfile->beg.epoch = sacfile->a.epoch - precut;
			epoch2time( &(sacfile->beg), sacfile->beg.epoch );

			time2sac( &(sacfile->beg), &(sacfile->s) );
			sacfile->s.a = precut;
			break;

		case T0_MARKER:
			if( sacfile->s.t0 == -12345. )
			{
				printf("ERROR %s time marker not set in file %s\n", kmarker, sacfile->filename );
				exit(-1);
			}
			tbeg = sacfile->t0.epoch - sacfile->beg.epoch - precut;

			sacfile->beg.epoch = sacfile->t0.epoch - precut;
			epoch2time( &(sacfile->beg), sacfile->beg.epoch );

			time2sac( &(sacfile->beg), &(sacfile->s) );
			sacfile->s.t0 = precut;
			break;

		case T1_MARKER:
			if( sacfile->s.t1 == -12345. )
			{
				printf("ERROR %s time marker not set in file %s\n", kmarker, sacfile->filename );
				exit(-1);
			}
			tbeg = ( sacfile->t1.epoch - sacfile->beg.epoch ) - precut;

			sacfile->beg.epoch = sacfile->t1.epoch - precut;
			epoch2time( &(sacfile->beg), sacfile->beg.epoch );

			time2sac( &(sacfile->beg), &(sacfile->s) );
			sacfile->s.t1 = precut;
			break;

		case T2_MARKER:
			if( sacfile->s.t2 == -12345. )
			{
				printf("ERROR %s time marker not set in file %s\n", kmarker, sacfile->filename );
				exit(-1);
			}
			tbeg = ( sacfile->t2.epoch - sacfile->beg.epoch ) - precut;

			sacfile->beg.epoch = sacfile->t2.epoch - precut;
			epoch2time( &(sacfile->beg), sacfile->beg.epoch );

			time2sac( &(sacfile->beg), &(sacfile->s) );
			sacfile->s.t2 = precut;
			break;

		case T3_MARKER:
			if( sacfile->s.t3 == -12345. )
			{
				printf("ERROR %s time marker not set in file %s\n", kmarker, sacfile->filename );
				exit(-1);
			}
			tbeg = ( sacfile->t3.epoch - sacfile->beg.epoch ) - precut;

			sacfile->t3.epoch = sacfile->t3.epoch - precut;
			epoch2time( &(sacfile->t3), sacfile->t3.epoch );
			time2sac( &(sacfile->t3), &(sacfile->s) );
			sacfile->s.t3 = precut;
			break;

		case T4_MARKER:
			if( sacfile->s.t4 == -12345. )
                        {
                                printf("ERROR %s time marker not set in file %s\n", kmarker, sacfile->filename );
                                exit(-1);
                        }
                        tbeg = ( sacfile->t4.epoch - sacfile->beg.epoch ) - precut;
			sacfile->t4.epoch = sacfile->t4.epoch - precut;
			epoch2time( &(sacfile->t4), sacfile->t4.epoch );
                        time2sac( &(sacfile->t4), &(sacfile->s) );
                        sacfile->s.t4 = precut;
			break;

		case T5_MARKER:
			if( sacfile->s.t5 == -12345. )
                        {
                                printf("ERROR %s time marker not set in file %s\n", kmarker, sacfile->filename );
                                exit(-1);
                        }
                        tbeg = ( sacfile->t5.epoch - sacfile->beg.epoch ) - precut;
			sacfile->t5.epoch = sacfile->t5.epoch - precut;
			epoch2time( &(sacfile->t5), sacfile->t5.epoch );
                        time2sac( &(sacfile->t5), &(sacfile->s) );
                        sacfile->s.t5 = precut;
			break;

		case T6_MARKER:
			if( sacfile->s.t6 == -12345. )
                        {
                                printf("ERROR %s time marker not set in file %s\n", kmarker, sacfile->filename );
                                exit(-1);
                        }
                        tbeg = ( sacfile->t6.epoch - sacfile->beg.epoch ) - precut;
			sacfile->t6.epoch = sacfile->t6.epoch - precut;
			epoch2time( &(sacfile->t6), sacfile->t6.epoch );
                        time2sac( &(sacfile->t6), &(sacfile->s) );
                        sacfile->s.t6 = precut;
			break;

		case T7_MARKER:
			if( sacfile->s.t7 == -12345. )
                        {
                                printf("ERROR %s time marker not set in file %s\n", kmarker, sacfile->filename );
                                exit(-1);
                        }
                        tbeg = ( sacfile->t7.epoch - sacfile->beg.epoch ) - precut;
			sacfile->t7.epoch = sacfile->t7.epoch - precut;
			epoch2time( &(sacfile->t7), sacfile->t7.epoch );
                        time2sac( &(sacfile->t7), &(sacfile->s) );
                        sacfile->s.t7 = precut;
			break;

		case T8_MARKER:	
			if( sacfile->s.t8 == -12345. )
                        {
                                printf("ERROR %s time marker not set in file %s\n", kmarker, sacfile->filename );
                                exit(-1);
                        }
                        tbeg = ( sacfile->t8.epoch - sacfile->beg.epoch ) - precut;
			sacfile->t8.epoch = sacfile->t8.epoch - precut;
			epoch2time( &(sacfile->t8), sacfile->t8.epoch );
                        time2sac( &(sacfile->t8), &(sacfile->s) );
                        sacfile->s.t8 = precut;
			break;

		case T9_MARKER:
			if( sacfile->s.t9 == -12345. )
                        {
                                printf("ERROR %s time marker not set in file %s\n", kmarker, sacfile->filename );
                                exit(-1);
                        }
                        tbeg = ( sacfile->t9.epoch - sacfile->beg.epoch ) - precut;
			sacfile->t9.epoch = sacfile->t9.epoch - precut;
			epoch2time( &(sacfile->t9), sacfile->t9.epoch );
                        time2sac( &(sacfile->t9), &(sacfile->s) );
                        sacfile->s.t9 = precut;
			break;
	}

/***************************************************/
/*** if more than 1 day then something is wrong  ***/
/***************************************************/
	if( fabs(tbeg) > 86400 )
	{
		fprintf(stderr, "*************************************************************************\n");
		fprintf(stderr, "ERROR problem with window cut time.\n");
		fprintf(stderr, "Please check your origin time!\n");
		exit(-1);
	}

/***************************************************/
/**** sun4 gcc version 3.4 does not have rintf() ***/
/*** no rintf()  in libm.so.1                    ***/
/***************************************************/
/*
	ncut = (int)rintf( (twincut+precut) / sacfile->s.delta );
	nbeg = (int)rintf( tbeg/sacfile->s.delta );
*/
/****************************************************/
/*** replaced with rint()                         ***/
/****************************************************/
	ncut = (int)rint( (twincut+precut) / sacfile->s.delta );
	nbeg = (int)rint( tbeg/sacfile->s.delta );

	if( verbose )
	{
	  fprintf(stdout, 
	    "%s: sac_cut(): %s marker twincut=%g precut=%g tbeg(+precut)=%g nbeg(+precut)=%d ncut(+precut)=%d twincut=%g\n", 
		  progname, kmarker, twincut, precut, 
		  tbeg, nbeg, ncut, twincut );
	}

/***********************************************************/
/*** do the cutting of the data points into a temp space ***/
/***********************************************************/

/****************************************************************************************/
/*** KLUDGE...                                                                        ***/
/*** on sun4/solaris the memmory allocation seems messy so we preallocate the memmory ***/
/*** set all to zero and release. seems to solve the problems 11/7/2006 gai.          ***/
/****************************************************************************************/
	x = (float *)calloc( 1000000, sizeof(float) );
/**
	free(x);
	x = (float *)malloc( sizeof(float) );
**/

/*********************************************/
/*** gene_ichinose@urscorp.com 8/4/2006   ****/
/*** fix to handle negative tbeg and nbeg ****/
/*** pad zeros to begin of file           ****/
/*********************************************/
	for(kk=0, i=0; i < sacfile->s.npts; i++ )
	{
		jj = nbeg + i;
		/* printf("i=%d kk=%d jj=%d npts=%d\n", i, kk, jj, sacfile->s.npts ); */

		x = (float *)realloc(x,(kk+1)*sizeof(float));

		if( jj > 0 )
		{
			if( jj < sacfile->s.npts && jj < abs(nbeg)+ncut )
			{
				x[kk] = sacfile->data[jj];
				kk++;
			}
			else
			{
				break;
			}
		}
		else /*** nbeg and tbeg was negative so pad with zeros ****/
		{
			x[kk] = 0.0;
			kk++;
		}
	}
	if( verbose ) 
	{
		fprintf(stdout, "%s: kk=%d ncut=%d npts=%d\n", 
			progname, kk, ncut, sacfile->s.npts );
		fflush(stdout);
	}

/**************************************************************************/
/*** swap the data back into the sacfile structure ***/
/**************************************************************************/
	/* fprintf( stdout, " old npts=%d \n", sacfile->s.npts ); */
	sacfile->s.npts = kk;
	/* fprintf( stdout, " new npts=%d \n", sacfile->s.npts ); */

	/* free(sacfile->data); */

	/* sacfile->data = (float *)calloc( sacfile->s.npts, sizeof(float)); */

	sacfile->data = (float *)realloc( sacfile->data, sacfile->s.npts * sizeof(float));

	for( i=0; i < sacfile->s.npts; i++ )
	{
		sacfile->data[i] = x[i];
	}

	free(x);

/**************************************************************************/
/*** shift all the other time marker offsets ***/
/**************************************************************************/
	if( imarker != O_MARKER  && sacfile->s.o  != -12345. ) sacfile->s.o  = sacfile->s.o  - tbeg;
	if( imarker != A_MARKER  && sacfile->s.a  != -12345. ) sacfile->s.a  = sacfile->s.a  - tbeg;
	if( imarker != T0_MARKER && sacfile->s.t0 != -12345. ) sacfile->s.t0 = sacfile->s.t0 - tbeg;
	if( imarker != T1_MARKER && sacfile->s.t1 != -12345. ) sacfile->s.t1 = sacfile->s.t1 - tbeg;
	if( imarker != T2_MARKER && sacfile->s.t2 != -12345. ) sacfile->s.t2 = sacfile->s.t2 - tbeg;
	if( imarker != T3_MARKER && sacfile->s.t3 != -12345. ) sacfile->s.t3 = sacfile->s.t3 - tbeg;
	if( imarker != T4_MARKER && sacfile->s.t4 != -12345. ) sacfile->s.t4 = sacfile->s.t4 - tbeg;
	if( imarker != T5_MARKER && sacfile->s.t4 != -12345. ) sacfile->s.t5 = sacfile->s.t5 - tbeg;
	if( imarker != T6_MARKER && sacfile->s.t4 != -12345. ) sacfile->s.t6 = sacfile->s.t6 - tbeg;
	if( imarker != T7_MARKER && sacfile->s.t4 != -12345. ) sacfile->s.t7 = sacfile->s.t7 - tbeg;
	if( imarker != T8_MARKER && sacfile->s.t4 != -12345. ) sacfile->s.t8 = sacfile->s.t8 - tbeg;
	if( imarker != T9_MARKER && sacfile->s.t4 != -12345. ) sacfile->s.t9 = sacfile->s.t9 - tbeg;

/*************************************/
/*** reset SAC data extrema values ***/
/*************************************/
	sac_minmax( sacfile->data, sacfile->s.npts, 
		&(sacfile->s.depmax), &(sacfile->s.depmin), &(sacfile->s.depmen) );

	sum=0.0;
	for(i=0;i<sacfile->s.npts;i++)
		sum=sum+sacfile->data[i];

	if(verbose)
	{
		fprintf(stdout, "%s: sac_cut(): max=%g min=%g mean=%g sum=%g\n", 
		  progname, 
		  sacfile->s.depmax, sacfile->s.depmin, sacfile->s.depmen, sum );
	}

	return (float *)sacfile->data;
}

/***********************************/
/*** set_sac_time_marker()       ***/
/*** mode = 0  by time string    ***/
/*** mode = 1  by time structure ***/
/***********************************/

void set_sac_time_marker( int mode,
			Sac_File *sacfile,
			char *timestring, 
			MyTime *t,
			char *kmarker,
			int verbose )
{
	int imarker;
	int set_marker( char * );
	void sac2time( MyTime *, Sac_Header * );
	void time2sac( MyTime *, Sac_Header * );
	void parsestring( MyTime *t, char *str );
	void clone_mytime( MyTime *t, MyTime *a );
	void initialize_mytime( MyTime * );

/***********************************/
/*** check for valid marker      ***/
/***********************************/
	if( (imarker = set_marker( kmarker )) == UNKNOWN_MARKER )
	{
	  printf("set_sac_time_marker(): ");
	  printf(" Unknown SAC Time Marker:\n\t imarker=%d kmarker=%s\n", 
		imarker, kmarker );
	  exit(-1);
	}
	if( verbose ) 
	{
	  fprintf( stdout, "%s: set_sac_time_marker(): mode=%d verbose=%d kmarker=%s imarker=%d\n", 
		progname, mode, verbose, kmarker, imarker );
	}

/********************************************************************************************/
/*** the kzdate and kztime nzyear, nzjday,nzhour, nzmin, nzsec, nzmsec are only reference ***/
/*** times NOT the time of the begin point of file.  Reference time is the ZERO time in   ***/
/*** the file.  Must take into account the beg hdr value to get the REAL begin time in the***/
/*** file, that is the absolute time of the first sample, which could be negative and     ***/
/*** nonzero check the IZTYPE to see what the reference tie marker is...                  ***/
/********************************************************************************************/
/*** get the SAC reference time      ***/
/*** beg is a MyTime structure       ***/
/*** sacfile->s is the sac structure ***/
/*** nzyear,nzjday,...-> MyTime      ***/
/***************************************/

	sac2time( &(sacfile->ref), &(sacfile->s) );

/*************************************************/
/*** get the actual begin time of first sample ***/
/*************************************************/

	sacfile->beg.offset = 0;
	epoch2time( &(sacfile->beg), (sacfile->ref.epoch + sacfile->s.b) );

	if( sacfile->s.o != -12345 )
	{
		sacfile->ot.offset = 0;
		epoch2time( &(sacfile->ot), (sacfile->ref.epoch + sacfile->s.o) );
		if(verbose)
		  printf("%s: set_sac_time_marker(): getting origin time from sac file\n",
			progname );
	}
	else
	{
		if(verbose)
		  printf("%s: set_sac_time_marker(): origin time is undefined in sac file\n",
			progname );
	}

	if(verbose)
	{
		printf( "%s: set_sac_time_marker(): sac reference time=", progname );
		WriteMyTime2STDOUT( &(sacfile->ref) );

		printf( "%s: set_sac_time_marker(): sac begin time=    ", progname );
		WriteMyTime2STDOUT( &(sacfile->beg) );
	}
 
/*************************************************/
/*** get the origin time from the command line ***/
/*** string and set it in the sac header       ***/
/*************************************************/

	switch(imarker)
	{
		case O_MARKER:
			if( mode == 0 ) parsestring( &(sacfile->ot), timestring );
			if( mode == 1 )	clone_mytime( t, &(sacfile->ot) );
			sacfile->s.o  = (float)(sacfile->ot.epoch - sacfile->beg.epoch);
			sprintf( sacfile->s.ko, "OT" );
			if( verbose )
			{
				printf( "%s: set_sac_time_marker(): sac ot    time= ", progname );
				WriteMyTime2STDOUT( &(sacfile->ot) );
				printf( "%s: set_sac_time_marker(): ot offset=%g\n", progname, sacfile->s.o );
			}
			break;

		case A_MARKER:
			if( mode == 0 ) parsestring( &(sacfile->a), timestring );
			if( mode == 1 ) clone_mytime( t, &(sacfile->a) );
			sacfile->s.a  = (float)(sacfile->a.epoch - sacfile->beg.epoch);
			sprintf( sacfile->s.ka, "A" );
			if( verbose )
			{
				printf( "%s: set_sac_time_marker(): sac a     time= ", progname );
				WriteMyTime2STDOUT( &(sacfile->a) );
				printf( "%s: a  offset=%g\n", progname, sacfile->s.a );
			}
			break;

		case T0_MARKER:

			if( mode == 0 ) parsestring( &(sacfile->t0), timestring );
			if( mode == 1 ) clone_mytime( t, &(sacfile->t0) );
			sacfile->s.t0 = (float)(sacfile->t0.epoch - sacfile->beg.epoch);
			sprintf( sacfile->s.kt0, "T0" );
			if( verbose )
			{
				printf( "%s: set_sac_time_marker(): sac t0    time= ", progname );
				WriteMyTime2STDOUT( &(sacfile->t0) );
				printf( "%s: t0  offset=%g\n", progname, sacfile->s.t0 );
			}
			break;

		case T1_MARKER:

			if( mode == 0 ) parsestring( &(sacfile->t1), timestring );
			if( mode == 1 ) clone_mytime( t, &(sacfile->t1) );
			sacfile->s.t1  = (float)(sacfile->t1.epoch - sacfile->beg.epoch);
			sprintf( sacfile->s.kt1, "T1" );
			if( verbose )
			{
				printf( "%s: set_sac_time_marker(): sac t1    time= ", progname );
				WriteMyTime2STDOUT( &(sacfile->t1) );
				printf( "%s: t1 offset=%g\n", progname, sacfile->s.t1 );
			}
			break;

		case T2_MARKER:
			if( mode == 0 ) parsestring( &(sacfile->t2), timestring );
			if( mode == 1 ) clone_mytime( t, &(sacfile->t2) );
                        sacfile->s.t2  = (float)(sacfile->t2.epoch - sacfile->beg.epoch);
			break;
	
		case T3_MARKER:
			if( mode == 0 ) parsestring( &(sacfile->t3), timestring );
			if( mode == 1 ) clone_mytime( t, &(sacfile->t3) );
                        sacfile->s.t3  = (float)(sacfile->t3.epoch - sacfile->beg.epoch);
			break;

		case T4_MARKER:
			if( mode == 0 ) parsestring( &(sacfile->t4), timestring );
			if( mode == 1 ) clone_mytime( t, &(sacfile->t4) );
                        sacfile->s.t4  = (float)(sacfile->t4.epoch - sacfile->beg.epoch);
			break;

		case T5_MARKER:
                        if( mode == 0 ) parsestring( &(sacfile->t5), timestring );
			if( mode == 1 ) clone_mytime( t, &(sacfile->t5) );
                        sacfile->s.t5  = (float)(sacfile->t5.epoch - sacfile->beg.epoch);
                        break;
                                                                                                                                                          
		case T6_MARKER:
                        if( mode == 0 ) parsestring( &(sacfile->t6), timestring );
			if( mode == 1 ) clone_mytime( t, &(sacfile->t6) );
                        sacfile->s.t6  = (float)(sacfile->t6.epoch - sacfile->beg.epoch);
                        break;
                                                                                                                                                          
		case T7_MARKER:
                        if( mode == 0 ) parsestring( &(sacfile->t7), timestring );
			if( mode == 1 ) clone_mytime( t, &(sacfile->t7) );
                        sacfile->s.t7  = (float)(sacfile->t7.epoch - sacfile->beg.epoch);
                        break;
                                                                                                                                                          
		case T8_MARKER:
                        if( mode == 0 ) parsestring( &(sacfile->t8), timestring );
			if( mode == 1 ) clone_mytime( t, &(sacfile->t8) );
                        sacfile->s.t8  = (float)(sacfile->t8.epoch - sacfile->beg.epoch);
                        break;
                                                                                                                                                          
		case T9_MARKER:
                        if( mode == 0 ) parsestring( &(sacfile->t9), timestring );
			if( mode == 1 ) clone_mytime( t, &(sacfile->t9) );
                        sacfile->s.t9  = (float)(sacfile->t9.epoch - sacfile->beg.epoch);
                        break;
	}
	if( verbose )
	{
		printf("%s: set_sac_time_marker(): sac Reference time=", progname );
		WriteMyTime2STDOUT( &(sacfile->ref) );
		printf("%s: set_sac_time_marker(): sac Begin     time=", progname );
		WriteMyTime2STDOUT( &(sacfile->beg) );
		printf("%s: set_sac_time_marker(): sac Origin    time=", progname );
		WriteMyTime2STDOUT( &(sacfile->ot) );
	}
}

int set_marker( char *kmarker )
{
	int i, imarker=UNKNOWN_MARKER;
	for( i=0; i<3; i++ ) kmarker[i] = toupper( kmarker[i] );
	if( strncmp( kmarker, "O",  1 )== 0 ) imarker=O_MARKER;
	if( strncmp( kmarker, "A",  1 )== 0 ) imarker=A_MARKER;
	if( strncmp( kmarker, "T0", 2 )== 0 ) imarker=T0_MARKER;
	if( strncmp( kmarker, "T1", 2 )== 0 ) imarker=T1_MARKER;
	if( strncmp( kmarker, "T2", 2 )== 0 ) imarker=T2_MARKER;
	if( strncmp( kmarker, "T3", 2 )== 0 ) imarker=T3_MARKER;
	if( strncmp( kmarker, "T4", 2 )== 0 ) imarker=T4_MARKER;
	if( strncmp( kmarker, "T5", 2 )== 0 ) imarker=T5_MARKER;
	if( strncmp( kmarker, "T6", 2 )== 0 ) imarker=T6_MARKER;
	if( strncmp( kmarker, "T7", 2 )== 0 ) imarker=T7_MARKER;
	if( strncmp( kmarker, "T8", 2 )== 0 ) imarker=T8_MARKER;
	if( strncmp( kmarker, "T9", 2 )== 0 ) imarker=T9_MARKER;
	return imarker;
}

void time2sac( MyTime *t, Sac_Header *s )
{
	s->nzyear = t->year;
	s->nzjday = t->jday;
	s->nzhour = t->hour;
	s->nzmin  = t->min;
	s->nzsec  = t->isec;
	s->nzmsec = t->msec;
}

void sac2time( MyTime *t, Sac_Header *s )
{
	void complete( MyTime *t );
	void initialize_mytime( MyTime *t );
/*** start subroutine ***/
	initialize_mytime( t );
	t->year = s->nzyear;
	t->jday = s->nzjday;
	t->hour = s->nzhour;
	t->min  = s->nzmin;
	t->isec = s->nzsec;
	t->msec = s->nzmsec;
	t->offset = 0;
	complete( t );
}
