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

/*** called by mtinv.c - specs for automt.txt file used by mtbestfit.c ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <sys/stat.h>
#include <errno.h>

#include <sys/utsname.h>
#include <netdb.h>
#include <unistd.h>

#include "../include/nrutil.h"
#include "../include/mt.h"

extern char progname[128];

void write_results2(
	char *results_filename,
	Solution *sol,
	EventInfo *ev,
	Greens **grn,
	int iz,
	int nsta,
	int npages,
	int AutoAuth,
	int ishift,
	float cortol,
	float maxtimeshift,
	int iuse_snr,
	float minsnr,
	int igmt5,
	int sqlite3_db_write,
	int mysql_db_write,
	int oracle_db_write,
	float FixMyZ )
{
	int ipage = 0, ista = 0, ichan = 0, nsta_used = 0;
	char chan[8];
        char mt_type[24];
        int mtdegfree = 0;
        char used;
	char username[64];
	char myloc[8];

        MyTime *now;	
	char grd_mo_type[8];
	FILE *fp;
	float ot_tmp = 0;
	
	if( AutoAuth )
	{
		strcpy( username, "AutoAuth" );
		/* fprintf( stderr, "%s: %s: %s: username=%s\n", progname, __FILE__, __func__, username ); */
	}
        else
	{
		/*** getlogin() is a bad function/and does not work well with strcpy(), use getenv to stay consistent with rest of code ****/
		/* strcpy( username, getlogin() ); */

		strcpy( username, getenv( "USER" ) );
		/* fprintf( stderr, "%s: %s: %s: username=%s\n", progname, __FILE__, __func__, username ); */
	}

	ot_tmp = ev[0].ts0 * -1;

	if( sol[iz].mt_type == DEVIATORIC ) { mtdegfree = 5; strcpy( mt_type, "DEV" ); }
        if( sol[iz].mt_type == FULL_MT    ) { mtdegfree = 6; strcpy( mt_type, "FULL" ); }
        if( sol[iz].mt_type == EXPLOSION  ) { mtdegfree = 1; strcpy( mt_type, "ISO" ); }

/*** open file ***/

	if( (fp=fopen( results_filename, "a+")) == NULL )
        {
                fprintf(stderr, "%s: cannot write file %s\n",
			progname, results_filename );
                exit(-1);
        }

/*** move write_results2() till after psplot() and return ipage ***/
/*** function of depth and origin time ***/

	fprintf( fp, "BEGIN     %g %g %g %g %g %g %s %d %s  OT EVDP OT VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR\n",
		ot_tmp,
		grn[0][iz].evdp,
		sol[iz].ot,
		sol[iz].var_red,
		sol[iz].total_fitness1,
		sol[iz].PDC,
		mt_type,
		mtdegfree,
		username );

	fprintf( fp, "COM       %s\n", ev[0].comment );
	fprintf( fp, "EMAIL     email_T%05.1fsec_Z%05.1fkm_.txt\n", sol[iz].ot, grn[0][iz].evdp );

	fprintf( fp, "NPAGES    %d\n", npages ); 

	for( ipage = 0; ipage < npages; ipage++ ) 
	{
		fprintf( fp, "PSPLOT%02d  plot_T%05.1fsec_Z%05.1fkm_.p%02d.jpg\n",
			ipage+1, sol[iz].ot, grn[0][iz].evdp, ipage+1 );
	}

	fprintf( fp, "CMDLINE   %d %g %g %d %g %d %d %d %d %g\n",
		ishift,
		cortol,
		maxtimeshift,
		iuse_snr,
		minsnr,
		igmt5,
		sqlite3_db_write,
		mysql_db_write,
		oracle_db_write,
		FixMyZ ); 

	fprintf( fp, "M0_DYNCM  %e\n", sol[iz].dmoment );
	fprintf( fp, "M0_AB_EX  %g %d\n", sol[iz].abcassa, sol[iz].exponent );
	fprintf( fp, "MW        %g\n", sol[iz].mw );
	fprintf( fp, "PISO      %g\n", sol[iz].PISO );
	fprintf( fp, "PCLVD     %g\n", sol[iz].PCLVD );
	fprintf( fp, "PDC       %g\n", sol[iz].PDC );
	fprintf( fp, "KISO      %g\n", sol[iz].k );
	fprintf( fp, "EPS       %g\n", sol[iz].epsilon );
	fprintf( fp, "LUNELAT   %g\n", sol[iz].lune_lat );
	fprintf( fp, "LUNELON   %g\n", sol[iz].lune_lon );
	fprintf( fp, "FFAC      %g\n", sol[iz].f_factor );

	fprintf( fp, "MXX       %e\n",  sol[iz].mtt * pow( 10.0, sol[iz].exponent ) );
	fprintf( fp, "MYY       %e\n",  sol[iz].mff * pow( 10.0, sol[iz].exponent ) );
	fprintf( fp, "MZZ       %e\n",  sol[iz].mrr * pow( 10.0, sol[iz].exponent ) );
	fprintf( fp, "MXY       %e\n", -sol[iz].mtf * pow( 10.0, sol[iz].exponent ) );
	fprintf( fp, "MXZ       %e\n",  sol[iz].mrt * pow( 10.0, sol[iz].exponent ) );
	fprintf( fp, "MYZ       %e\n", -sol[iz].mrf * pow( 10.0, sol[iz].exponent ) );

	fprintf( fp, "VRED      %g\n", sol[iz].var_red );
	fprintf( fp, "TOTFIT    %g\n", sol[iz].total_fitness1 );
	fprintf( fp, "ALGOR     %s%s\n", progname, Version_Label );
	fprintf( fp, "MTDEGFREE %d %s\n", mtdegfree, mt_type );
	fprintf( fp, "USERNAME  %s\n", username );
	fprintf( fp, "P1        %g %g %g\n", sol[iz].Maj.P1.s, sol[iz].Maj.P1.d, sol[iz].Maj.P1.r );
	fprintf( fp, "P2        %g %g %g\n", sol[iz].Maj.P2.s, sol[iz].Maj.P2.d, sol[iz].Maj.P2.r );
	fprintf( fp, "TAXMAJ    %g %g %g\n", sol[iz].Maj.T.ev, sol[iz].Maj.T.az, sol[iz].Maj.T.pl );
	fprintf( fp, "PAXMAJ    %g %g %g\n", sol[iz].Maj.P.ev, sol[iz].Maj.P.az, sol[iz].Maj.P.pl );
	fprintf( fp, "BAXMAJ    %g %g %g\n", sol[iz].Maj.B.ev, sol[iz].Maj.B.az, sol[iz].Maj.B.pl );
	fprintf( fp, "MDEV      %e\n", ( sol[iz].Mdc + sol[iz].Mclvd ) );
	fprintf( fp, "MCLVD     %e\n", sol[iz].Mclvd );
	fprintf( fp, "MISO      %e\n", sol[iz].Miso );
	fprintf( fp, "MDC       %e\n", sol[iz].Mdc );
	fprintf( fp, "EIGT      %g\n", sol[iz].FullMT.T.ev );
	fprintf( fp, "EIGP      %g\n", sol[iz].FullMT.P.ev );
	fprintf( fp, "EIGB      %g\n", sol[iz].FullMT.B.ev ); 
	fprintf( fp, "LAT       %g\n", grn[0][iz].evla );
	fprintf( fp, "LON       %g\n", grn[0][iz].evlo );
	fprintf( fp, "TIME      %lf\n", ev[0].ot.epoch );
	fprintf( fp, "YR        %d\n", ev[0].ot.year );
	fprintf( fp, "MONTH     %d\n", ev[0].ot.month );
	fprintf( fp, "DAY       %d\n", ev[0].ot.mday );
	fprintf( fp, "HR        %d\n", ev[0].ot.hour );
	fprintf( fp, "MIN       %d\n", ev[0].ot.min );
	fprintf( fp, "SEC       %g\n", ev[0].ot.fsec );
	fprintf( fp, "JDAY      %d\n", ev[0].ot.jday );
	fprintf( fp, "MAXGAP    %g\n", sol[iz].maxgap );
	fprintf( fp, "DMINKM    %g\n", sol[iz].dminkm );

	/* ishift, cortol, maxtimeshift iuse_snr MINSNR  */

	nsta_used = 0;
        for( ista=0; ista<nsta; ista++ )
        {
                if( ev[ista].iused == 1 ) nsta_used++;
        }
	fprintf( fp, "NSTA_USED %d\n", nsta_used );
	fprintf( fp, "NSTA      %d\n", nsta );

	for( ista = 0; ista < nsta; ista++ )
	{
		if( ev[ista].grd_mo_type == VELOCITY )     sprintf( grd_mo_type, "V" );
		if( ev[ista].grd_mo_type == DISPLACEMENT ) sprintf( grd_mo_type, "D" );

		if( strcmp( ev[ista].loc, "" ) == 0 || strcmp( ev[ista].loc, "--" ) == 0 )
			strcpy( myloc, "\"\"");
		else
			strcpy( myloc, ev[ista].loc );


			/**         1  2  3  4  5 6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 **/
		fprintf( fp, "STA%03d %s %s %g %g %g %d %d %s %d %g %g %d %s %s %d %f %f %f %f %f %f %f %s\n",
			ista,
			grn[ista][iz].stnm,
			grn[ista][iz].net,
			grn[ista][iz].rdist,
			grn[ista][iz].az,    /* 5 */
			grn[ista][iz].baz,
			ev[ista].iused,
			ev[ista].grd_mo_type,
			grd_mo_type,
			ev[ista].npole,   /* 10 */
			ev[ista].lf,
			ev[ista].hf,
			ev[ista].npass, 
			grn[ista][iz].v.modfile, /* 14 */
			myloc,  /* 15 */   /** null="" else 00, 10, etc... **/
			ev[ista].nt,  /* 16 */
			ev[ista].dt, /* 17 */
			ev[ista].tr, /* 18 */
			ev[ista].tt, /* 19 */
			ev[ista].mul_factor, /* 20 */
			ev[ista].time_shift_all, /* 21 */
			ev[ista].time_shift_user, /* 22 */
			ev[ista].weight, /* 23 */
			ev[ista].wavetype  /* 24 */
		 );
	
		fprintf( fp, "NCHAN 3\n" );

		for( ichan = 0; ichan < 3; ichan++ )
		{
			if( ichan == 0 ) strcpy( chan, "BHZ" );
                	if( ichan == 1 ) strcpy( chan, "BHR" );
                	if( ichan == 2 ) strcpy( chan, "BHT" );

			/*                  1        2 3   4   5  6  7  8   9 10 11 */
			fprintf( fp, "STA%03d CHAN%02d %s %lf %lf %g %g %g %g %g %g ",
				ista,
				ichan,
				chan,
                                ev[ista].z.beg.epoch,
                                ev[ista].z.end.epoch, /* 5 */
	 			ev[ista].redv,
                                ev[ista].time_shift_all, /* 7 */
				ev[ista].time_shift_user, /* 8 */
                                ev[ista].weight,
                                grn[ista][iz].Prayparameter, /* 10 */
                                ev[ista].ew.s.delta );

	 		if( ichan == 0 )
                        {
                          fprintf( fp, " %g %g %g\n", ev[ista].z.P2P_snr, ev[ista].zxcor, ev[ista].ztlag );
                        }
                        if( ichan == 1 )
                        {
                          fprintf( fp, " %g %g %g\n", ev[ista].ns.P2P_snr, ev[ista].rxcor, ev[ista].rtlag );
                        }
                        if( ichan == 2 )
                        {
                          fprintf( fp, " %g %g %g\n", ev[ista].ew.P2P_snr, ev[ista].txcor, ev[ista].ttlag );
                        }
		}
	}

	fprintf( fp, "END\n" );
	fclose(fp);
}

/**** /Users/ichinose1/Work/automt_test/done/2016-10-26T214203/dev/automt.txt ****/
/***
	email_T004.0sec_Z006.0km_.txt
	plot_T004.0sec_Z006.0km_.p01.jpg
	run.csh : OT 2016/10/26,21:42:04.00
	db.txt
***/
/****
BEGIN     8 12 12.7266 0.612778 80.2312 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     7 3 13.6227 1.26643 90.2432 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     6 9 15.1009 5.2088 98.1009 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     5 3 26.0433 1.25291 80.2139 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     4 3 34.3663 2.41981 86.7979 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     3 9 25.3762 8.44489 97.9951 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     2 3 48.6016 12.0754 96.9751 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     1 6 48.4232 16.8423 98.1249 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     -0 6 50.8195 31.5226 99.3878 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     -1 6 49.5353 13.9903 97.4593 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     -2 27 33.3703 5.51271 94.9467 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     -3 33 29.1959 4.75305 94.8574 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     -4 12 16.7625 8.17104 98.9485 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     -5 15 11.0823 1.41818 93.1856 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     -6 18 8.61708 3.67997 98.6584 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     -7 3 14.2549 2.64885 95.6185 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     -8 6 12.9917 1.73167 93.4976 DEV 5 AutoAuth  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
BEGIN     -0 6 50.8195 31.5226 99.3878 DEV 5 ichinose1  OT EVDP VRED TOTFIT PDC MT_TYPE MTDEGFREE AUTHOR
****/
