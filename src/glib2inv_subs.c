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
#include <math.h>

#include "../include/mt.h"

extern char progname[128];


/*** glib2inv processes Greens function library files *.glib for moment tensor inversion read by mtinv.c ***/
/*** glib2inv_get_input_parameters subroutine for reading cmdline for glib2inv.c:main() ***/


EventInfo *glib2inv_get_input_parameters( char *filename, 
	EventInfo *ev, int *n, int verbose )
{
	FILE *fp;
	int MAX_RECORD_LENGTH = 512;
	char rec[MAX_RECORD_LENGTH];
	int ista, nitems;
	MyTime ot;
	char timestring[24], kdum[3], comment[256], database_info[256];
	char grd_mo_type, kused;

	float strike, dip, rake, Mw, evdp, evla, evlo;
	float Mxx, Mxy, Mxz, Myy, Myz, Mzz, Mo;
	float Nyquist_Frequency;

/*** timesubs.o ***/
	void parsestring( MyTime *, char * );
	void clone_mytime(  MyTime *, MyTime * );

/******************************/
/*** open glib2inv.par file ***/
/******************************/
	if( (fp = fopen( filename, "r" )) == NULL )
	{
		printf("%s: cannot open file %s\n", progname, filename );
		exit(-1);
	}

/********************/
/*** defaults     ***/
/********************/
	Mo  = 0;
	Mxx = 0;
	Mxy = 0;
	Mxz = 0;
	Myy = 0;
	Myz = 0;
	Mzz = 0;

	grd_mo_type='d';
	strike=-999; 
	dip=-999; 
	rake=-999;
	Mw=-999;
	ista = 0;

	while( fgets( rec, MAX_RECORD_LENGTH, fp ) != NULL )
	{

	/********************/
	/*** comment line ***/
	/********************/
		if( rec[0] == '#' ) continue;

	/***********************************************************/
	/*** AutoMT includes this database info line. The home   ***/
	/*** version skips it but we have it here to maintain    ***/
	/*** compatibility between operational and home versions ***/
	/***********************************************************/
		if( strncmp( rec, "DB ", 3 ) == 0 )
		{
			/*** just skip for now, G. Ichinose 2014 ***/
			/*
			sscanf( rec, "%s %[^\n]\n", kdum, database_info );
			if(verbose)printf("%s: glib2inv_subs(): database info=%s\n", 
				progname, database_info );
			*/
                        continue;
		}

	/*******************************************************/
	/*** event discription line (gets written into email)***/
	/*******************************************************/
		if( strncmp( rec, "CM ", 3 ) == 0 )
		{
			sscanf( rec, "%s %[^\n]\n", kdum, comment );
			if(verbose)
			{
			  fprintf( stdout, "%s: %s: %s: comment=%s\n",
				progname, __FILE__, __func__, comment );
			}
			continue;
		}

	/*******************************************************/
	/*** read the origin time                            ***/
	/*******************************************************/

		if( strncmp( rec, "OT ", 3 ) == 0 ) 
		{
			sscanf( rec, "%s %s", kdum, timestring );
			parsestring( &ot, timestring );
			if( verbose ) 
			{
			  fprintf( stdout, "%s: %s: %s: origin time=\t",
				progname, __FILE__, __func__ );
			  WriteMyTime2STDOUT( &ot );
			}
			continue;
		}

	/************************************************************************/
	/*** read the MT information ***/
	/************************************************************************/
		if( strncmp( rec, "MT ", 3 ) == 0 )
		{ 
				/*       1  2  3  4  5  6  7 */
			sscanf( rec, "%s %f %f %f %f %f %f %f",
				kdum, &Mxx, &Mxy, &Mxz, &Myy, &Myz, &Mzz, &Mo );

			if( verbose )
			{
			  fprintf( stdout,
			"%s: %s: %s: Mxx=%g Mxy=%g Mxz=%g Myy=%g Myz=%g Mzz=%g Mo=%g\n",
				progname, __FILE__, __func__,
				Mxx, Mxy, Mxz, Myy, Myz, Mzz, Mo );
			}
			continue;
		}

	/************************************************************************/
	/*** read the event information, only evlo, evla needed for inversion ***/
	/************************************************************************/

		if( strncmp(rec, "EV ", 3 ) == 0 )
		{
			sscanf( rec, "%s %f %f %f %f %f %f %f", 
				kdum, &strike, &dip, &rake, &Mw, &evlo, &evla, &evdp );

			if( verbose )
			{
			  fprintf( stdout,
			"%s: %s: %s: str=%g dip=%g rak=%g Mw=%g evla=%g evlo=%g evdp=%g\n",
				progname, __FILE__, __func__,
				strike, dip, rake, Mw, evla, evlo, evdp );
			}
			continue;
		}
	
	/************************************************************************/
	/*** read the station information                                     ***/
	/************************************************************************/

		ev = (EventInfo *)realloc( ev, (ista+1)*sizeof(EventInfo) );

		/****                  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 ***/
		nitems = sscanf( rec, "%s %s %s %s %d %d %f %f %d %f %f %f %c %f %c %f %f %s", 
			ev[ista].stnm, 			/*  1 */
			ev[ista].net,			/*  2 */
			ev[ista].loc,                   /*  3 */
			ev[ista].modfile,		/*  4 */
			&(ev[ista].npole),		/*  5 */
			&(ev[ista].npass),		/*  6 */
			&(ev[ista].lf),			/*  7 */
			&(ev[ista].hf),			/*  8 */
			&(ev[ista].nt),			/*  9 */
			&(ev[ista].dt),			/* 10 */
			&(ev[ista].tr),                 /* 11 */
			&(ev[ista].tt),			/* 12 */
			&grd_mo_type,			/* 13 */
			&(ev[ista].mul_factor),		/* 14 */
			&kused,				/* 15 */
			&(ev[ista].time_shift_all),     /* 16 */
			&(ev[ista].weight), 		/* 17 */
			ev[ista].wavetype               /* 18 Surf/Pnl Rotational */
		);

	/*** time_shift_all will get updated with the max cross-correlation ***/
		ev[ista].time_shift_user = ev[ista].time_shift_all;

		/* fprintf( stdout, "loc=(%s)\n", ev[ista].loc ); */

		if(  strcmp( ev[ista].loc, "\"\"" ) == 0 || strcmp( ev[ista].loc, "--" ) == 0 )
		{
			strcpy( ev[ista].loc, "" );
		}

	/***************************************/
	/*** G. Ichinose Nov 5, 2019 check wavetype options: "Surf/Pnl" or "Rotational" ***/
	/***************************************/

		if( strcmp( ev[ista].wavetype, "Surf/Pnl" ) == 0 || 
		    strcmp( ev[ista].wavetype, "Rotational" ) == 0 )
		{
			if(verbose)
			  fprintf( stdout, "%s: %s: %s: (%s.%s.%s) wavetype=(%s) ok\n",
				progname, __FILE__, __func__, 
				ev[ista].net, ev[ista].stnm, ev[ista].loc, ev[ista].wavetype );
		}
		else if( strcmp( ev[ista].wavetype, "" ) == 0 || strncmp( ev[ista].wavetype, "#", 1 ) == 0 )  /*** empty or next string is start of comment # ***/
		{
			strcpy( ev[ista].wavetype, "Surf/Pnl" );
			fprintf( stdout, "%s: %s: %s: ista=%d WARNING: NULL was entered so wavetype=(%s) assigned by default\n",
                                progname, __FILE__, __func__, ista, ev[ista].wavetype );
		}
		else
		{
			fprintf( stdout, "%s: %s: %s: STDOUT: FATAL ERROR: ista=%d unknown wavetype=(%s) options: \"Surf/Pnl\", \"Rotational\", or \"\"\n",
                                progname, __FILE__, __func__, ista, ev[ista].wavetype );
			fprintf( stderr, "%s: %s: %s: STDERR: FATAL ERROR: ista=%d unknown wavetype=(%s) options: \"Surf/Pnl\", \"Rotational\", or \"\"\n",
                                progname, __FILE__, __func__, ista, ev[ista].wavetype );
			exit(-1);
		}

	/***************************************/
	/*** check time number of items read ***/
	/***************************************/
		if( nitems < 17 )
		{
		  fprintf(stderr, "%s: not enough items=%d read from file %s ista=%d\n\n",
			progname, nitems, filename, ista );
		  fprintf(stderr, "%s: offending line=%s\n", progname, rec );
		  fprintf(stderr, "\n" );
		  fprintf(stderr, "%s: free-format, space seperated with 18 columns: \n", progname );
	          fprintf(stderr, "1   2   3   4   5     6     7    8    9   10   11 12 13   14   15 16  17     18       \n" );
		  fprintf(stderr, "sta net loc mod npole npass lf   hf   nt  dt   tr tt GM fmul used TS  weight wavetype \n" );
		  fprintf(stderr, "--- --- --- --- ----- ----- ---- ---- --- ---- -- -- -- ---- ---- --  ------ -------- \n");
		  fprintf(stderr, "PAS CI  ""  wus 3     2     0.02 0.05 512 0.15 0  0  d  1    y    0   1.0    Surf/Pnl or Rotational \n" );
		  fprintf(stderr, "\n" );
		  fprintf(stderr, "Column  1: Station Code\n" );
		  fprintf(stderr, "Column  2: Netwok Code\n" );
	          fprintf(stderr, "Column  3: Location Code = \"\", \"00\", \"10\", .... \n" );
		  fprintf(stderr, "Column  4: 1D Velocity Model Base File Name (without .mod extension)\n" );
    		  fprintf(stderr, "Column  5: Butterworth Filter number of poles\n" );
		  fprintf(stderr, "Column  6: Butterworth Filter number of passes\n" );
		  fprintf(stderr, "Column  7: Butterworth Filter lowpass corner (Hz)\n" );
		  fprintf(stderr, "Column  8: Butterworth Filter highpass corner (Hz)\n" );
		  fprintf(stderr, "Column  9: Decimated number of points\n" );
		  fprintf(stderr, "Column 10: Decimated Sampling Rate (sec/samp)\n" );
		  fprintf(stderr, "Column 11: Rise Time of Boxcar (sec)\n" );
		  fprintf(stderr, "Column 12: Duration of Boxcar (sec)\n" );
		  fprintf(stderr, "Column 13: Ground Motion (d=displacement v=velocity)\n" );
		  fprintf(stderr, "Column 14: Multuply all amplitudes of data by this value\n" );
		  fprintf(stderr, "Column 15: y=use in calculation and residual n=do not use only predict\n" );
		  fprintf(stderr, "Column 16: time shift all data by this value in (sec)\n" );
		  fprintf(stderr, "Column 17: Amplitude weight given to this station data for inversion\n" );
		  fprintf(stderr, "Column 18: Wave type: Surf/Pnl (ZRT) or Rotational-3C (UWV) [default empty is Surf/Pnl which is typical]\n" );
		  fprintf(stderr, "\n" );
		  exit(-1);
		}

		if( verbose ) 
		{
			fprintf(stdout, "%s: %s: %s: nitems read = %d for ista=%d from file %s\n",
				progname, __FILE__, __func__, nitems, ista, filename ); 
		}

	/***************************************/
	/*** check the npole range           ***/
	/***************************************/
		if( ev[ista].npole <= 0 || ev[ista].npole > 9 )
		{
			fprintf( stderr, "%s: glib2inv_subs(): npole=%d out of range [1,9]\n",
				progname, ev[ista].npole );
			exit(-1);
		}

	/***************************************/
	/*** check npass                     ***/
	/***************************************/
		if( ev[ista].npass < 1 || ev[ista].npass > 2 )
		{
			fprintf( stderr, "%s: glib2inv_subs(): npass=%d out of range [1,2]\n",
				progname, ev[ista].npass );
			exit(-1);
		}

	/***************************************/
	/*** check hf range below nyquist freq */
	/***************************************/
		Nyquist_Frequency = 1 / ( 2 * ev[ista].dt );
		if( ev[ista].hf > Nyquist_Frequency )
		{
			fprintf( stderr, 
			  "%s: glib2inv_subs(): hi-freq corner %g greater than Nyquist Frequency %g(Hz)\n",
				progname, ev[ista].hf, Nyquist_Frequency );

			fprintf( stderr,
			  "%s: glib2inv_subs(): lower hf for station %s.%s in file %s\n", 
				progname, ev[ista].stnm, ev[ista].net, filename );

			exit(-1);
		}

	/***************************************/
	/*** check if hf >= lf               ***/
	/***************************************/
		if( ev[ista].lf >= ev[ista].hf )
		{
			fprintf( stderr,
			  "%s: glib2inv_subs(): low-frequency corner %g greater than high-frequency corner %g\n",
				progname, ev[ista].lf, ev[ista].hf );

			fprintf( stderr,
                          "%s: glib2inv_subs(): increase hf or lower lf for station %s.%s in file %s\n",
                                progname, ev[ista].stnm, ev[ista].net, filename );
                        exit(-1);
		}

	/***************************************/
	/*** check mul factors               ***/
	/***************************************/
		if( ev[ista].mul_factor == 0 ) ev[ista].mul_factor = 1;

	/***********************************************************/
	/*** check if station is being used in the inversion of  ***/
	/*** is being carried along for prediction               ***/
	/***********************************************************/

		ev[ista].iused = 1; /** default catch all, station is being used **/
		if( kused == 'y' || kused == 'Y' || kused == '1' ) ev[ista].iused = 1;
		if( kused == 'n' || kused == 'N' || kused == '0' ) ev[ista].iused = 0;

		/*** envelope is deprecated gene.ichinose 07.01.2024 ***/
		ev[ista].ienvelope = 0;

	/***************************************************/
	/*** ground motion type velocity or displacement ***/
	/***************************************************/

		if( grd_mo_type == 'v' )
		{
			 ev[ista].grd_mo_type = VELOCITY;
		}
		else if( grd_mo_type == 'd' )
		{
			ev[ista].grd_mo_type = DISPLACEMENT;
		}
		else
		{
		  ev[ista].grd_mo_type = DISPLACEMENT;
		  fprintf(stderr, 
		    "%s: glib2inv_subs(): Warning unknown Ground motion type=%c, assuming displacement\n",
			progname, grd_mo_type );
		}

	/******************************************/
	/*** create file names for output files ***/
	/******************************************/

						/* 8 1 8 1 8 1 1 2 5 */
		snprintf( ev[ista].data_filename, 35, "%s.%s.%s.%c.%02d.data", 
			ev[ista].net, ev[ista].stnm, ev[ista].loc, 
			grd_mo_type, ista );

/***
clang -I../include -c glib2inv_subs.c -o glib2inv_subs.o
glib2inv_subs.c:361:3: warning: 'snprintf' will always overflow; destination buffer has size 256, but size argument is 288 [-Wbuiltin-memcpy-chk-size]
                snprintf( ev[ista].glib_filename, 288, "%s.%s.%s.%s.glib",
***/
						/* 8 1 8 1 8 1 256 5 = 288 */
		snprintf( ev[ista].glib_filename, 256, "%s.%s.%s.%s.glib",
			ev[ista].net, ev[ista].stnm, ev[ista].loc,
			ev[ista].modfile );

/***
glib2inv_subs.c:366:3: warning: 'snprintf' will always overflow; destination buffer has size 256, but size argument is 293 [-Wbuiltin-memcpy-chk-size]
                snprintf( ev[ista].ginv_filename, 293, "%s.%s.%s.%s.%c.%02d.ginv", 
***/
						/* 8 1 8 1 8 1 256 1 1 1 2 5 = 293 */
		snprintf( ev[ista].ginv_filename, 256, "%s.%s.%s.%s.%c.%02d.ginv", 
			ev[ista].net, ev[ista].stnm, ev[ista].loc,
			ev[ista].modfile, grd_mo_type, ista );

		if( verbose )
		{
		  printf( "%s: glib2inv_subs():           data file=%s\n", progname, ev[ista].data_filename );
		  printf( "%s: glib2inv_subs(): greens ftn lib file=%s\n", progname, ev[ista].glib_filename );
		  printf( "%s: glib2inv_subs(): greens ftn inv file=%s\n", progname, ev[ista].ginv_filename );
		}
	
	/***********************************************************************/
	/*** set some of the global data structures with the local variables ***/
	/***********************************************************************/

		clone_mytime( &ot, &(ev[ista].ot) );
		clone_mytime( &ot, &(ev[ista].ot_orig) );

		ev[ista].str  = strike;
		ev[ista].dip  = dip;
		ev[ista].rak  = rake;
		ev[ista].Mw   = Mw;

		ev[ista].my_z = evdp;
		ev[ista].evlo = evlo;
		ev[ista].evla = evla;
		ev[ista].evdp = evdp;
		
		ev[ista].Mo   = Mo;
		ev[ista].Mxx  = Mxx;
		ev[ista].Mxy  = Mxy;
		ev[ista].Mxz  = Mxz;
		ev[ista].Myy  = Myy;
		ev[ista].Myz  = Myz;
		ev[ista].Mzz  = Mzz;

		strcpy( ev[ista].comment, comment );

		if( verbose )
		{
		  fprintf( stdout, "%s: %s: %s: ista=%03d data=%s glib=%s ginv=%s npole=%d npass=%d\n",
			progname, __FILE__, __func__, 
			ista,
			ev[ista].data_filename,
			ev[ista].glib_filename,
			ev[ista].ginv_filename,
			ev[ista].npole, 
			ev[ista].npass );
		  fprintf( stdout, "%s: %s: %s: lf=%g hf=%g nt=%d dt=%g tr=%g tt=%g grd_mo_type=%d mul_factor=%g iused=%d\n",
			progname, __FILE__, __func__, 
			ev[ista].lf, ev[ista].hf, ev[ista].nt, 
			ev[ista].dt, ev[ista].tr, ev[ista].tt,
			ev[ista].grd_mo_type, ev[ista].mul_factor, ev[ista].iused );

		  fprintf( stdout, "%s: %s: %s: ista=%d str=%g dip=%g rak=%g Mw=%g my_z=%g evdp=%g\n",
			progname, __FILE__, __func__, 
			ista,
			ev[ista].str,
                        ev[ista].dip,
                        ev[ista].rak,
                        ev[ista].Mw,
                        ev[ista].my_z, ev[ista].evdp );
			
		}
		ista++;
	}
	*n = ista;
	return (EventInfo *)ev;
}

void array2grnRot( float **garray, Greens *g )
{       
        int it, nt;
        nt = g->nt;
        if( nt > 4096 ) printf("%s: array2grn: nt=%d > 4096\n", progname, nt );
        
        for( it=0; it<nt; it++ )
        {       
                g->g.rss[it] = 0;
                g->g.rds[it] = 0;
                g->g.rdd[it] = 0;
                g->g.rep[it] = 0;
                g->g.zss[it] = 0;
                g->g.zds[it] = 0;
                g->g.zdd[it] = 0;
                g->g.zep[it] = 0;
                g->g.tss[it] = 0;
                g->g.tds[it] = 0;

		g->g.w1ss[it] = 0;
                g->g.w1ds[it] = 0;
                g->g.w1dd[it] = 0;
                g->g.w1ex[it] = 0;

                g->g.w2ss[it] = 0;
                g->g.w2ds[it] = 0;
                g->g.w2dd[it] = 0;
                g->g.w2ex[it] = 0;

                g->g.w3ss[it] = 0;
                g->g.w3ds[it] = 0;
		g->g.w3dd[it] = 0;
                g->g.w3ex[it] = 0;
        }
        
        for( it=0; it<nt; it++ )
        {
                g->g.rss[it] = garray[0][it];
                g->g.rds[it] = garray[1][it];
                g->g.rdd[it] = garray[2][it];
                g->g.rep[it] = garray[3][it];
                g->g.zss[it] = garray[4][it];
                g->g.zds[it] = garray[5][it];
                g->g.zdd[it] = garray[6][it];
                g->g.zep[it] = garray[7][it];
                g->g.tss[it] = garray[8][it];
                g->g.tds[it] = garray[9][it];

		g->g.w1ss[it] = garray[10][it];
                g->g.w1ds[it] = garray[11][it];
                g->g.w1dd[it] = garray[12][it];
                g->g.w1ex[it] = garray[13][it];

                g->g.w2ss[it] = garray[14][it];
                g->g.w2ds[it] = garray[15][it];
                g->g.w2dd[it] = garray[16][it];
                g->g.w2ex[it] = garray[17][it];

                g->g.w3ss[it] = garray[18][it];
                g->g.w3ds[it] = garray[19][it];
                g->g.w3dd[it] = garray[20][it];
                g->g.w3ex[it] = garray[21][it];
        }
}

void array2grn( float **garray, Greens *g )
{
        int it, nt;
        nt = g->nt;
	if( nt > 4096 ) printf("%s: array2grn: nt=%d > 4096\n", progname, nt );

	for( it=0; it<nt; it++ )
	{
		g->g.rss[it] = 0;
                g->g.rds[it] = 0;
                g->g.rdd[it] = 0;
                g->g.rep[it] = 0;
                g->g.zss[it] = 0;
                g->g.zds[it] = 0;
                g->g.zdd[it] = 0;
                g->g.zep[it] = 0;
                g->g.tss[it] = 0;
                g->g.tds[it] = 0;
	}

        for( it=0; it<nt; it++ )
        {
                g->g.rss[it] = garray[0][it];
                g->g.rds[it] = garray[1][it];
                g->g.rdd[it] = garray[2][it];
                g->g.rep[it] = garray[3][it];
                g->g.zss[it] = garray[4][it];
                g->g.zds[it] = garray[5][it];
                g->g.zdd[it] = garray[6][it];
                g->g.zep[it] = garray[7][it];
                g->g.tss[it] = garray[8][it];
                g->g.tds[it] = garray[9][it];
        }
}

/*** incase ev->wavetype == Rotational then
     access additional rotational green's func ***/

void split2grnRot( Greens *g, float **garray )
{
        int it, nt;
        nt = g->nt;

        for( it=0; it<nt; it++ )
        {
                garray[0][it] = g->g.rss[it];
                garray[1][it] = g->g.rds[it];
                garray[2][it] = g->g.rdd[it];
                garray[3][it] = g->g.rep[it];
                garray[4][it] = g->g.zss[it];
                garray[5][it] = g->g.zds[it];
                garray[6][it] = g->g.zdd[it];
                garray[7][it] = g->g.zep[it];
                garray[8][it] = g->g.tss[it];
                garray[9][it] = g->g.tds[it];

		garray[10][it] = g->g.w1ss[it];
                garray[11][it] = g->g.w1ds[it];
                garray[12][it] = g->g.w1dd[it];
                garray[13][it] = g->g.w1ex[it];

                garray[14][it] = g->g.w2ss[it];
                garray[15][it] = g->g.w2ds[it];
                garray[16][it] = g->g.w2dd[it];
                garray[17][it] = g->g.w2ex[it];

                garray[18][it] = g->g.w3ss[it];
                garray[19][it] = g->g.w3ds[it];
		garray[20][it] = g->g.w3ss[it];
                garray[21][it] = g->g.w3ds[it];


        }
}

void split2grn( Greens *g, float **garray )
{
        int it, nt;
        nt = g->nt;
	
        for( it=0; it<nt; it++ )
        {
                garray[0][it] = g->g.rss[it];
                garray[1][it] = g->g.rds[it];
                garray[2][it] = g->g.rdd[it];
                garray[3][it] = g->g.rep[it];
                garray[4][it] = g->g.zss[it];
                garray[5][it] = g->g.zds[it];
                garray[6][it] = g->g.zdd[it];
                garray[7][it] = g->g.zep[it];
                garray[8][it] = g->g.tss[it];
                garray[9][it] = g->g.tds[it];
        }
}

void grn2disp( Greens *g, EventInfo *ev, int verbose, int mtdegfree )
{

/*** ten greens functions ***/
        float *rss, *rds, *rdd, *rep, *zss, *zds, *zdd, *zep, *tss, *tds;

/*** directional cosine coefficients ***/
        float a1, a2, a3, a4, a5;
        Tensor M; /* float Mxx, Myy, Mzz, Mxy, Mxz, Myz; */
        float strr, dipr, rakr;
	float half=0.5;
	float Mo;

        int it;
        float pi, d2r, dt, t0, e, tt, tr, fi;
        int nt;
	void source_time_function( float *, int, float, float, float );

/*** set some constants ***/
        pi  = M_PI;
        d2r = pi/180.;

        nt = g->nt;
        dt = g->dt;
        t0 = g->t0;
        e  = t0 + (nt*dt);
        tt = ev->tt;
        tr = ev->tr;

/**** set t0 from reduction velocity redv ****/
        t0 = g->tstart;
	e  = g->tend;

/*** assign greens function to local pointers ***/

        rss = g->g.rss;
        rds = g->g.rds;
        rdd = g->g.rdd;
        rep = g->g.rep;
        zss = g->g.zss;
        zds = g->g.zds;
        zdd = g->g.zdd;
        zep = g->g.zep;
        tss = g->g.tss;
        tds = g->g.tds;

/** convolve a source time function ***/

	if( tr == 0 && tt == 0 )
	{
	}
	else
	{
          source_time_function( rss, nt, dt, tr, tt );
          source_time_function( rds, nt, dt, tr, tt );
          source_time_function( rdd, nt, dt, tr, tt );
          source_time_function( rep, nt, dt, tr, tt );
          source_time_function( zss, nt, dt, tr, tt );
          source_time_function( zds, nt, dt, tr, tt );
          source_time_function( zdd, nt, dt, tr, tt );
          source_time_function( zep, nt, dt, tr, tt );
          source_time_function( tss, nt, dt, tr, tt );
          source_time_function( tds, nt, dt, tr, tt );
	}

/* w = transverse v = vertical u = radial */

	if( ( ev->str != -999 ) && ( ev->dip != -999 ) && ( ev->rak != -999 )  )
	{
          fi = g->az * d2r;
          ev->Mo = pow( 10.0, (1.5*( ev->Mw + 10.73)) );
	  Mo = ev->Mo / base_moment;

          strr = ev->str * d2r;
          dipr = ev->dip * d2r;
          rakr = ev->rak * d2r;

          M.xx = -( sin(dipr) * cos(rakr) * sin(2*strr) + sin(2*dipr) * sin(rakr) * sin(strr)*sin(strr) );
          M.yy =  ( sin(dipr) * cos(rakr) * sin(2*strr) - sin(2*dipr) * sin(rakr) * cos(strr)*cos(strr) );
          M.zz =  ( sin(2*dipr) * sin( rakr ) );
          M.xy =  ( sin(dipr) * cos(rakr) * cos(2*strr) + 0.5*sin(2*dipr) * sin(rakr) * sin(2*strr) );
          M.xz = -( cos(dipr) * cos(rakr) * cos(strr) + cos(2*dipr) * sin(rakr) * sin(strr) );
          M.yz = -( cos(dipr) * cos(rakr) * sin(strr) - cos(2*dipr) * sin(rakr) * cos(strr) );

          if( verbose )
	  {
		fprintf( stdout, "%s: %s: %s: using SDR to forward compute syn str=%g dip=%g rak=%g Mw=%g Mo=%g\n",
			progname, __FILE__, __func__, ev->str, ev->dip, ev->rak, ev->Mw, ev->Mo );
		fprintf( stdout, "%s: %s: %s: fi=%g strr=%g dipr=%g rakr=%g Mxx=%g Myy=%g Mzz=%g Mxy=%g Mxz=%g Myz=%g\n",
                	progname, __FILE__, __func__, fi, strr, dipr, rakr, M.xx, M.yy, M.zz, M.xy, M.xz, M.yz);
	  }
	}
	else
	{
	  fi = g->az * d2r;
	  Mo = ev->Mo / base_moment; /*** Mo gets applied below ***/
	  M.xx = ev->Mxx;
	  M.xy = ev->Mxy;
	  M.xz = ev->Mxz;
	  M.yy = ev->Myy;
	  M.yz = ev->Myz;
	  M.zz = ev->Mzz;

	  if( verbose )
	  {
            fprintf( stdout, "%s: %s: %s: using MT to forward compute syn fi=%g Mxx=%g Myy=%g Mzz=%g Mxy=%g Mxz=%g Myz=%g Mo=%g ev->Mo=%g base_moment=%g\n",
               	progname, __FILE__, __func__, fi, M.xx, M.yy, M.zz, M.xy, M.xz, M.yz,
		Mo, ev->Mo, base_moment );
	  }
	}


/*** switch to subroutine which uses a_matrix type approach calculating the 3-C displacements ***/

/*** This method (Aj) coefficients is only for DEVIATORIC compute the coefficients ****/

	a1 = half * ( M.xx - M.yy ) * cos( 2 * fi ) + M.xy * sin( 2 * fi );
        a2 = M.xz * cos( fi ) + M.yz * sin( fi );
        a3 = -half*( M.xx + M.yy );
        a4 = half * ( M.xx - M.yy ) * sin( 2 * fi ) - M.xy * cos( 2 * fi );
        a5 = -M.yz * cos( fi ) + M.xz * sin( fi );

        if( verbose ) 
	{
	   fprintf( stdout, "%s: %s: %s: a1=%g a2=%g a3=%g a4=%g a5=%g mtdegfree=%d\n", 
		progname, __FILE__, __func__, a1, a2, a3, a4, a5, mtdegfree );
	}

        for( it=0; it<nt; it++)
        {
		if( mtdegfree == 5 || mtdegfree == 6 )
		{
                  g->ver[it] = (a1*zss[it]+a2*zds[it]+a3*zdd[it])*Mo; /*** vertical   ***/
                  g->rad[it] = (a1*rss[it]+a2*rds[it]+a3*rdd[it])*Mo; /*** radial     ***/
                  g->tra[it] = (a4*tss[it]+a5*tds[it])*Mo;            /*** transverse ***/
		}
		else if( mtdegfree == 1 )
		{
		  g->ver[it] = zep[it] * Mo;
		  g->rad[it] = rep[it] * Mo;
		  g->tra[it] = 0;
		}
        }

	if( verbose )
	{
	  fprintf( stdout, "%s: %s: %s: done with forward calculation\n",
		progname, __FILE__, __func__ );
	}

        return;
}

void special_load( char *station_name, char *network_name, Greens *g )
{
	float **garray, *data;
	Sac_Header *s;	
	FILE *fp;
	int i, iext, it, num_ext = 10;
	char filename[128];                
		   	       /***            0    1      2     3      4     5       6      7       8     9     ***/
	char filename_extension[10][4] = { "rss", "rds", "rdd", "rex", "zss", "zds", "zdd", "zex", "tss", "tds" };
	int ig, ng=10;
	void array2grn( float **, Greens * );

/**************************/
/*** zero out the array ***/
/**************************/
	garray = (float **)malloc(ng*sizeof(float *));
	for( ig=0; ig<ng; ig++ )
		garray[ig] = (float *)malloc( 4096 * sizeof(float) );

	for( iext=0; iext<num_ext; iext++ )
	{	
		for( it=0; it < g->nt; it++ )
			garray[iext][it] = 0;
	}
	
/********************************************************/
/*** loop over all green's functions for this station ***/
/********************************************************/
	for( iext=0; iext<num_ext; iext++ )
	{

	/*************************************************************/
	/*** open the sac file for this Green's function component ***/
	/*************************************************************/
		sprintf( filename, "%s.%s.%s", 
		  station_name, network_name, filename_extension[iext] );
		fprintf( stdout, "%s: file=%s(%02d) ", progname, filename, iext );
		if( (fp=fopen(filename, "rb")) == NULL )
		{
			fprintf( stderr, "\n%s: Fatal Error, cannot open file %s\n",
				progname, filename );
			exit(-1);
		}

	/********************************************************/
	/*** load the sac header and data from file           ***/
	/*** copy the sac data into Greens structure          ***/
	/********************************************************/
		s = (Sac_Header *)malloc( sizeof(Sac_Header) );
		fread( s, sizeof(Sac_Header), 1, fp );
		data = (float *)calloc( s->npts, sizeof(float) );
		fread( &data[0], s->npts * sizeof(float), 1, fp );
		fclose(fp);

		g->nt  = s->npts;

	/********************************************************/
	/*** control overflow ***/
	/********************************************************/
		if( s->npts > 4096 ) g->nt = 4096;
		for( it = 0; it < g->nt; it++ ) 
			garray[iext][it] = data[it];

	/********************************************************/
	/*** copy the sac header into Greens structure ***/
	/********************************************************/
		strncpy( g->stnm, s->kstnm, 8 );
		strncpy( g->net, s->knetwk, 8 );
		strncpy( g->loc, s->khole, 8 );

		for( i=0; i<8; i++ )
		{
			if( g->stnm[i] == ' ' ) g->stnm[i]='\0';
			if( g->net[i]  == ' ' ) g->net[i]='\0';
		}
	
		g->dt		= s->delta;
		g->t0		= s->b;
		g->evdp 	= s->evdp;
		g->stla		= s->stla;
		g->stlo		= s->stlo;
		g->evla		= s->evla;
		g->evlo		= s->evlo;
		g->rdist	= s->dist;
		g->az		= s->az;
		g->baz		= s->baz;
		g->rigidity	= 3.3E+11;
		g->redv		= -1.0;
		g->ts0		= 0.0;
		g->tstart	= s->b;
		g->tend		= s->b + s->delta * s->npts;

		fprintf( stdout, " stla=%g stlo=%g evla=%g evlo=%g evdp=%g rdist=%g ",
			g->stla, g->stlo, g->evla, g->evlo, g->evdp, g->rdist );
		fprintf( stdout, " az=%g baz=%g dt=%g nt=%d b=%g sta=%s net=%s\n",
			g->az, g->baz, g->dt, g->nt, g->t0, g->stnm, g->net );

		free(s);
		free(data);
	}
	array2grn( garray, g );
}
