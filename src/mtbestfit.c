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

/*** this is a helper function for mtinv work flow  ***/
/*** mtinv.c outputs results automt.txt ***/
/*** mtbestfit reads a file named automt.txt and autogenerates scripts to make plots for real-time version ***/
/*** autocreates run2.csh with best fitting MT solution and if 6-deg freedom MT then autocreates mteig.csh ***/
/*** run2.csh makes the final best fitting mt solution, and database output. mteig.csh & mteig.c creates  ***/
/*** the NetworkSensitivitySolution plot on the lune ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>
#include <errno.h>

#include <sys/utsname.h>
#include <netdb.h>
#include <unistd.h>

#include "../include/nrutil.h"
#include "../include/mt.h"

char progname[128];

typedef struct {
        char sta[8];
        char net[8];
        char loc[8];
        char modfile[256];
	float rdist, az, baz;
	int iused; /* 0 or 1 */
        int npole;
        int npass;
        float lf;
        float hf;
        int nt;
        float dt;
        float tr;
        float tt;
	int grd_mo_type; /* 0 or 1 */
        char grd_mo_type_v_or_d[8]; /* "v" or "d" */
        float mul_fac;
        char used_y_or_n[8]; /* "y" or "n" */
        float time_shift_all;
	float time_shift_user;
        float weight;
        char wavetype[32];
} mtinv_par_station_row;

typedef struct {
	float ot_shift;
	float z;
	float fsec;
	float vred;
	float fit;
	float pdc;
	float piso;
	float pclvd;
	float eigp,eigb,eigt;

	char mt_type[6];
	int mtdegfree;
	char author[32];
	char comment[128];
	char email_file[256];
	double originTimeEpoch;
	MyTime ot;
	float lat, lon;
	float momag, m0_dyne_cm, m0_newton_meters;
	float p1_str, p1_dip, p1_rak, p2_str, p2_dip, p2_rak;
	float mxx,myy,mzz,mxy,mxz,myz;

	int ishift;
	float cortol;
	float maxtimeshift;
	int iuse_snr;
	float minsnr;
	int igmt5;
	int sqlite3_db_write;
	int mysql_db_write;
	int oracle_db_write;

	float vred_max, fit_max;
	float fit_max_threshold, vred_diff_threshold;
	float vred_diff;

/*** fixed depth ***/
	int ifixz;
	float fixz;

	long evid;
	int npages;
	char **psplotfiles;

/*** add the station row ***/
	int nsta;
	mtinv_par_station_row *s;

} bestFitMT;

int main( int ac, char **av )
{
	FILE *fp;
	int i, j, i_best_vred, i_best_fit, nsol;
	int ista;
	bestFitMT *a;
	char tag[32], pstag[12], tmp[12];
	char rec[512];
	float vred_max, fit_max;
	long evid;
	char evidstr[32];
	int igmt5 = 1;  /*** default on ***/

	char filename[] = { "automt.txt" };
	float fit_max_threshold = 20;
	float vred_diff_threshold = 30;
	int force_best_vred = 1;  /*** default on ***/
	int use_ts0 = 1;

	int pretty_plot = 0;  /*** dumpxy and create pretty waveform plot for pubs ***/
	int idb = 1; /*** do the database load ***/
	char DataDir[256];
	char RespDir[256];

/*** functional prototypes ***/

	void write_mtbest( bestFitMT *a );
	void make_run( bestFitMT *a, char *fitType, int igmt5, long evid, int pretty_plot, int idb );
	void createResultsWebpage( bestFitMT *a );

	void make_mteig( bestFitMT *a, char *fitType, float decimate_factor, int use_ts0, int igmt5, char *DataDir, char *RespDir );
	int create_mteig = 1;
	float decimate_factor = 2; /* decimate_factor = [1,2,4,8] */

	void do_mtscreen( bestFitMT *a, char *python_script );

	void WriteMyTime2STDOUT( MyTime *t );
	MyTime *epoch2time( MyTime *t, double epoch );

	int help = 0;
	int debug = 0;

	int setpar(int ac, char **av);
	int mstpar( char *, char *, void * );
        int getpar( char *, char *, void * );
	void endpar();

/*** begin main(), open input file ***/

	strcpy( progname, av[0] );
	strcpy( DataDir, "../Data" );
	strcpy( RespDir, "../Resp" );

        fprintf( stderr, "%s: STDERR: Version=%s ReleaseDate=%s\n", progname, Version_Label, Version_Date );
	fprintf( stderr, "\n" );
	fprintf( stderr, " TYPE: \"%s help\" for options.\n", progname );
	fprintf( stderr, "\n" );
	fprintf( stderr, "\n" );

	setpar( ac, av );
	getpar( "force_best_vred", "b", &force_best_vred );
	strcpy( evidstr, "-1" );
	getpar( "evid", "s", evidstr );
	evid = strtol( evidstr, NULL, 10 );
	getpar( "gmt5", "b", &igmt5 );
	getpar( "pretty_plot", "b", &pretty_plot );
	getpar( "db", "b", &idb );
	getpar( "DataDir", "s", DataDir );
	getpar( "RespDir", "s", RespDir );

	getpar( "mteig", "b", &create_mteig );
	if(create_mteig)
	{
		getpar( "decimate_factor", "f", &decimate_factor );
		getpar( "use_ts0", "b", &use_ts0 );
	}

	getpar( "help", "b", &help );
	endpar();

	if( help )
	{
	  fprintf( stderr,
    "\nUsage: %s [no]force_best_vred evid={long} [no]gmt5 [no]pretty_plot [no]db [no]mteig decimate_factor={float} [no]use_ts0 DataDir=../Data RespDir=../Resp [no]help\n",
		progname );

	  fprintf( stderr, "\n" );

	  fprintf( stderr, "REQUIRED: \n" );
	  fprintf( stderr, "\t none, requires file named automt.txt\n" );
	  fprintf( stderr, "\n" );

	  fprintf( stderr, "OPTIONAL: \n" );
	  fprintf( stderr, "\t [no]force_best_vred forces (boolean) the use of max variance reduction instead of complex formula\n" );
	  fprintf( stderr, "\t evid={long}                carry along the EventID (default -1 off)\n" );
	  fprintf( stderr, "\t [no]gmt5                   (boolean) use gmt5+ instead of default gmt4\n" );
	  fprintf( stderr, "\t [no]pretty_plot            (boolean) creates a publication quality waveform plot using gmtwf.csh default off\n" );
	  fprintf( stderr, "\t [no]db                     (boolean) load the database insert.sql script default on\n" );
	  fprintf( stderr, "\t [no]help                   (boolean) prints usage default off\n" );
	  fprintf( stderr, "\t [no]mteig (boolean) creates mteig.csh script to do NSS lune plots default off (note only valid when mtdegfree=6)\n" );
	  fprintf( stderr, "\t decimate_factor={float}    decimation factor, only allows 1,2,4,8 default 2 so if nt=1024 then nt=512\n" );
	  fprintf( stderr, "\t [no]use_ts0                use station time-shifts, default true, otherwise reset to zero\n" );
	  fprintf( stderr, "\t DataDir                    (string) default ../Data\n" );
	  fprintf( stderr, "\t RespDir                    (string) default ../Resp\n" );
	  fprintf( stderr, "\n" );

	  fprintf( stderr, "DESCRIPTION:\n" );
	  fprintf( stderr, "reads a file named automt.txt and autogenerates scripts to make plots for real-time version\n" );
	  fprintf( stderr, "See mtbestfit.c fit_max_threshold = 20 vred_diff_threshold = 30 \n" );
	  fprintf( stderr, "\n" );

	  exit(0);
	}

	if( (fp = fopen( filename, "r" )) == NULL )
	{
		fprintf( stderr, "%s: %s: %s: FATAL ERROR: Cannot open file %s\n", 
			progname, __FILE__, __func__, filename );
		fprintf( stderr, "%s: %s: %s: Please delete any old automt.txt files and run mtinv using run.csh C-shell script\n",
			progname, __FILE__, __func__ );
		exit(-1);
	}

/*** read file and parse ***/

	a =calloc(1,sizeof(bestFitMT));
	i = 0;
	while( fgets( rec, 512, fp ) != NULL )
	{
		sscanf( rec, "%s", tag );

		if( strcmp( tag, "BEGIN" ) == 0 )
		{
			a = realloc( a, (i+1)*sizeof(bestFitMT));
		
			sscanf( rec, "%s %f %f %f %f %f %f %s %d %s",
				tag,
				&(a[i].ot_shift),
				&(a[i].z),
				&(a[i].fsec),
				&(a[i].vred),
				&(a[i].fit),
				&(a[i].pdc),
				a[i].mt_type,
				&(a[i].mtdegfree),
				a[i].author );
		}

		if( strcmp( tag, "CMDLINE" ) == 0 )
		{
			/*   CMDLINE   1 0.85 10 1 3 1 0 0 0 **/
			sscanf( rec, "%s %d %g %g %d %g %d %d %d %d %g",
				tag,
				&(a[i].ishift),
				&(a[i].cortol),
				&(a[i].maxtimeshift),
				&(a[i].iuse_snr),
				&(a[i].minsnr),
				&(a[i].igmt5),
				&(a[i].sqlite3_db_write),
				&(a[i].mysql_db_write),
				&(a[i].oracle_db_write),
				&(a[i].fixz) );	

			if( a[i].fixz == -99 )
				a[i].ifixz = 0; 
			else
				a[i].ifixz = 1;
		}

		if( strcmp( tag, "COM" ) == 0 )
			sscanf( rec, "%s %[^\n]", tag, a[i].comment );

		if( strcmp( tag, "EMAIL" ) == 0 )
			sscanf( rec, "%s %s", tag, a[i].email_file );

		if( strcmp( tag, "NPAGES" ) == 0 )
		{
			sscanf( rec, "%s %d", tag, &(a[i].npages) );

		/*** allocate memory for PS/JPEG plot files ***/

			a[i].psplotfiles = calloc( a[i].npages, sizeof(char *) );
			for( j = 0; j < a[i].npages; j++ )
			{
				a[i].psplotfiles[j] = calloc( 128, sizeof(char) );
			}

			for( j = 0; j < a[i].npages; j++ )
			{
				fgets( rec, 512, fp );
				sscanf( rec, "%s", tag );
				
				snprintf( pstag, 12, "PSPLOT%02d", j+1 );

				/*** Debug ***/
				/* fprintf( stdout, "tag=%s pstag=%s rec=%s\n", tag, pstag, rec ); */

				if( strcmp( tag, pstag ) == 0 ) 
				{
					sscanf( rec, "%s %s", pstag, a[i].psplotfiles[j] );
				}
			}

		} /*** reading NPAGES PS/JPEG plot files ***/

		if( strcmp( tag, "M0_DYNCM" ) == 0 )
		{
			sscanf( rec, "%s %f", tag, &(a[i].m0_dyne_cm) );
			a[i].m0_newton_meters = a[i].m0_dyne_cm * 1e-7;
		}

		if( strcmp( tag, "MW" ) == 0 )
			sscanf( rec, "%s %f", tag, &(a[i].momag) );
			
		if( strcmp( tag, "PISO" )  == 0 )
			sscanf( rec, "%s %f", tag, &(a[i].piso) );

		if( strcmp( tag, "PCLVD" ) == 0 )
			sscanf( rec, "%s %f", tag, &(a[i].pclvd) );

		if( strcmp( tag, "LAT" ) == 0 )
			sscanf( rec, "%s %f", tag, &(a[i].lat) );

		if( strcmp( tag, "LON" ) == 0 )
			sscanf( rec, "%s %f", tag, &(a[i].lon) );
	
		if( strcmp( tag, "EIGP" ) == 0 )
			sscanf( rec, "%s %f", tag, &(a[i].eigp) );

		if( strcmp( tag, "EIGB" ) == 0 )
			sscanf( rec, "%s %f", tag, &(a[i].eigb) );

		if( strcmp( tag, "EIGT" ) == 0 )
			sscanf( rec, "%s %f", tag, &(a[i].eigt) );
		
		if( strcmp( tag, "P1" ) == 0 )
			sscanf( rec, "%s %f %f %f", tag, &(a[i].p1_str), &(a[i].p1_dip), &(a[i].p1_rak) );

		if( strcmp( tag, "P2" ) == 0 )
                        sscanf( rec, "%s %f %f %f", tag, &(a[i].p2_str), &(a[i].p2_dip), &(a[i].p2_rak) );

		if( strcmp( tag, "MXX" ) == 0 )
			sscanf( rec, "%s %f", tag, &(a[i].mxx) );

		if( strcmp( tag, "MYY" ) == 0 )
			sscanf( rec, "%s %f", tag, &(a[i].myy) );

		if( strcmp( tag, "MZZ" ) == 0 )
			sscanf( rec, "%s %f", tag, &(a[i].mzz) );

		if( strcmp( tag, "MXY" ) == 0 )
			sscanf( rec, "%s %f", tag, &(a[i].mxy) );

		if( strcmp( tag, "MXZ" ) == 0 )
			sscanf( rec, "%s %f", tag, &(a[i].mxz) );

		if( strcmp( tag, "MYZ" ) == 0 )
			sscanf( rec, "%s %f", tag, &(a[i].myz) );

		if( strcmp( tag, "TIME" ) == 0 )
		{	
			sscanf( rec, "%s %lf", tag, &(a[i].originTimeEpoch) );
			epoch2time( &(a[i].ot), a[i].originTimeEpoch );
			/* WriteMyTime2STDOUT( &(a[i].ot) ); */
		}
	
		if( strcmp( tag, "NSTA" ) == 0 )
		{
			sscanf( rec, "%s %d", tag, &(a[i].nsta) );
			a[i].s = (mtinv_par_station_row *)calloc( a[i].nsta, sizeof(mtinv_par_station_row) );
			ista = 0;
		}
	

		snprintf( tmp, 8, "STA%03d", ista );
		if( strcmp( tag, tmp ) == 0 ) 
		{
			sscanf( rec,
		"%s %s %s %f %f %f %d %d %s %d %f %f %d %s %s %d %f %f %f %f %f %f %f %s",
				tag, 
				(a[i].s[ista].sta),
				(a[i].s[ista].net),
				&(a[i].s[ista].rdist),
				&(a[i].s[ista].az),
				&(a[i].s[ista].baz),
				&(a[i].s[ista].iused),
				&(a[i].s[ista].grd_mo_type),
				(a[i].s[ista].grd_mo_type_v_or_d),
				&(a[i].s[ista].npole),
				&(a[i].s[ista].lf),
				&(a[i].s[ista].hf),
				&(a[i].s[ista].npass),
				(a[i].s[ista].modfile),
				(a[i].s[ista].loc),
				&(a[i].s[ista].nt),
				&(a[i].s[ista].dt),
				&(a[i].s[ista].tr),
				&(a[i].s[ista].tt),
				&(a[i].s[ista].mul_fac),
				&(a[i].s[ista].time_shift_all),
				&(a[i].s[ista].time_shift_user),
				&(a[i].s[ista].weight),
				(a[i].s[ista].wavetype) );

			if(debug)
			{
				fprintf( stdout, 
			"%d %d %s %s %g %g %g %d %d %s %d %g %g %d %s (%s) %d %g %g %g %g %g %g %g %s\n",
					i,
					ista,
					(a[i].s[ista].sta),
                                	(a[i].s[ista].net),
                                	(a[i].s[ista].rdist),
                                	(a[i].s[ista].az),
                                	(a[i].s[ista].baz),
                                	(a[i].s[ista].iused),
                                	(a[i].s[ista].grd_mo_type),
                                	(a[i].s[ista].grd_mo_type_v_or_d),
                                	(a[i].s[ista].npole),
                                	(a[i].s[ista].lf),
                                	(a[i].s[ista].hf),
                                	(a[i].s[ista].npass),
                                	(a[i].s[ista].modfile),
                                	(a[i].s[ista].loc),
                                	(a[i].s[ista].nt),
                                	(a[i].s[ista].dt),
                                	(a[i].s[ista].tr),
                                	(a[i].s[ista].tt),
                                	(a[i].s[ista].mul_fac),
                                	(a[i].s[ista].time_shift_all),
					(a[i].s[ista].time_shift_user),
                                	(a[i].s[ista].weight),
                                	(a[i].s[ista].wavetype)
				);
			}
			ista++;				
		}

		if( strcmp( tag, "END" ) == 0 )
		{
			i++;
		}
	}
	nsol = i;
	/* rewind(fp); */
	fclose(fp);

/******************************************/
/*** search and find the best solution  ***/
/******************************************/

	i_best_fit  = 0;
	i_best_vred = 0;
	vred_max    = 0;
	fit_max     = 0;

	for( i = 0; i < nsol; i++ )
	{
		a[i].fit_max_threshold = fit_max_threshold;
		a[i].vred_diff_threshold = vred_diff_threshold;
		if( a[i].fit > fit_max )
		{
			fit_max = a[i].fit;
			i_best_fit = i;
		}

		if( a[i].vred > vred_max ) 
		{
			vred_max = a[i].vred;
			i_best_vred = i;
		}

		/* write_mtbest( &(a[i]) ); */
	}
	/* write_mtbest( &(a[i_best_vred]) ); */

/******************************************/
/*** determine which is better fit or vred ? ***/
/******************************************/

	a[i_best_vred].vred_max = vred_max;
	a[i_best_fit].vred_max  = vred_max;

	a[i_best_vred].fit_max  = fit_max;
	a[i_best_fit].fit_max   = fit_max;

        a[i_best_vred].vred_diff= fabs( a[i_best_vred].vred - a[i_best_fit].vred );
	a[i_best_fit].vred_diff = fabs( a[i_best_vred].vred - a[i_best_fit].vred );

	if( force_best_vred )
	{
		fprintf( stderr, "%s: %s: %s: Option1: force_best_vred=%d vred=%g i_best_vred=%d\n",
			progname, __FILE__, __func__, force_best_vred, a[i_best_vred].vred, i_best_vred );
 		 fflush(stderr); 

		make_run( &(a[i_best_vred]), "FORCE BEST VARIANCE-REDUCTION", igmt5, evid, pretty_plot, idb );

		fprintf( stderr, "%s: %s: calling createResultsWebpage()\n", __FILE__, __func__ );
		fflush(stderr);

                createResultsWebpage( &(a[i_best_vred]) );
	}
	else
	{
	  if(	( fit_max > fit_max_threshold ) && 
		( fabs( a[i_best_vred].vred - a[i_best_fit].vred ) > vred_diff_threshold ) )
	  {

		fprintf( stderr, "%s: %s: %s: Option2 fix_max=%g > fix_max_threshold=%g vred_diff=%g > vred_diff_threshold=%g\n",
                        progname, __FILE__, __func__, fit_max, fit_max_threshold, 
			 ( fabs( a[i_best_vred].vred - a[i_best_fit].vred ) ), vred_diff_threshold );
                 fflush(stderr);   

		make_run( &(a[i_best_fit]), "BEST FIT", igmt5, evid, pretty_plot, idb );

		fprintf( stderr, "%s: %s: calling createResultsWebpage()\n", __FILE__, __func__ );
                fflush(stderr);  

		createResultsWebpage( &(a[i_best_fit]) );
	  }
	  else
	  {
		 fprintf( stderr, "%s: %s: %s: Option3 force_best_vred=%d vred=%g i_best_vred=%d\n",
                        progname, __FILE__, __func__, force_best_vred, a[i_best_vred].vred, i_best_vred );
                 fflush(stderr);

		make_run( &(a[i_best_vred]), "BEST VARIANCE-REDUCTION", igmt5, evid, pretty_plot, idb );

		fprintf( stderr, "%s: %s: calling createResultsWebpage()\n", __FILE__, __func__ );
                fflush(stderr);  

		createResultsWebpage( &(a[i_best_vred]) );
	  }
	}
 
/*** create a mteig.csh file for percent variance-reduction NSS on lune ***/
/*** assumes all full-MT use best %VR not best-fit ***/

	if( create_mteig && strcmp( a[i_best_vred].mt_type, "FULL" ) == 0 )
	{
	  fprintf( stderr, "%s: %s: calling make_mteig()\n", __FILE__, __func__ );
          fflush(stderr);

	  make_mteig( &(a[i_best_vred]), "BEST VARIANCE-REDUCTION", decimate_factor, use_ts0, igmt5, DataDir, RespDir );
	}

	if( strcmp( a[i_best_vred].mt_type, "FULL" ) == 0 )
	{
	  fprintf( stderr, "%s: %s: calling do_mtscreen()\n", __FILE__, __func__ );
	  fflush(stderr);

	  do_mtscreen( &(a[i_best_vred]), "mtscreen.py" );
	}

	fprintf( stderr, "%s: %s: bye-bye\n", __FILE__, __func__ );
                fflush(stderr);
	exit(0);

} /*** END OF MAIN() ***/


void do_mtscreen( bestFitMT *a, char *python_script )
{
	FILE *fp;

	if( strcmp( a->mt_type, "FULL" ) == 0 )
	{
		fp = fopen("run2.csh","a+");
		fprintf(fp, "###\n" );
		fprintf(fp, "### run mtscreen.py ###\n" );
		fprintf(fp, "###\n" );
		fprintf(fp, "%s -mxx %.3f -mxy %.3f -mxz %.3f -myy %.3f -myz %.3f -mzz %.3f\n",
			python_script,
			a->mxx/a->m0_dyne_cm,
			a->mxy/a->m0_dyne_cm,
			a->mxz/a->m0_dyne_cm,
			a->myy/a->m0_dyne_cm,
			a->myz/a->m0_dyne_cm,
			a->mzz/a->m0_dyne_cm );
		fclose(fp);
	}
	else
	{
		
	}
}

void make_mteig( bestFitMT *a, char *fitType, float decimate_factor, int use_ts0, int igmt5, char *DataDir, char *RespDir )
{
	FILE *fp;
	static char *script_filename = {"mteig.csh"};
	static char *executable_filename = {"mteig"};
	char rec[256];
	int nt;
	float dt, ts0;
	char gmt_opt[8]; /*** gmt5 or nogmt5 ***/
	int ista;
	void write_mtbest( bestFitMT *a );
	void string2lower( char *str, int n );

/*** begin subroutine ***/

	fprintf( stderr, "%s: %s: fitType=%s decimate_factor=%g use_ts0=%d script_filename=%s executable_filename=%s\n",
		__FILE__, __func__, fitType, decimate_factor, use_ts0,
		script_filename, executable_filename );
	fflush(stderr);

	/* write_mtbest(a); */

	fp = fopen( script_filename, "w" );
	fprintf( fp, "#!/bin/csh\n" );
	fprintf( fp, "#####################################################################\n" );
	fprintf( fp, "### this is an autogenerated script for mteig                     ###\n" );
	fprintf( fp, "### mteig compute %%VR NSS on the lune following Ford et al., 2012 ###\n" );
	fprintf( fp, "#####################################################################\n" );

	fprintf( fp, "###\n" );
        fprintf( fp, "### Created by %s fitType=%s\n", progname, fitType );
        fprintf( fp, "### ot=%g fsec=%g z=%g vred=%g fit=%g pdc=%g piso=%g pclvd=%g\n",
                a->ot_shift, a->fsec, a->z, a->vred, a->fit,
                roundf(a->pdc), roundf(a->piso), roundf(a->pclvd) );
        
        fprintf( fp, "### fit_max=%g fit_max_threshold=%g vred_diff=%g vred_diff_threshold=%g\n",
                        a->fit_max, 
                        a->fit_max_threshold,
                        a->vred_diff, 
                        a->vred_diff_threshold );
          
        fprintf( fp, "###\n" );

	fprintf( fp, "\n" );
	fprintf( fp, "#set DEGFREE=6  ### mteig assumes degree-of-freedom is 6 for full-MT to compute %%VR on lune\n" );
	fprintf( fp, "\n" );
	fprintf( fp, "cat >! mtinv.par << EOF\n" );
	fprintf( fp, "#### REGION COMMENT ############################\n" );
	fprintf( fp, "CM %s\n", a->comment );
        fprintf( fp, "#### Date and Origin Time ######################\n" );
        fprintf( fp, "OT %4d-%02d-%02dT%02d:%02d:%06.3f\n",
		a->ot.year, a->ot.month, a->ot.mday, a->ot.hour, a->ot.min, a->ot.fsec );
        fprintf( fp, "#### Forward Calculations ######################\n" );
        fprintf( fp, "##    stk    dip    rak   Mw  evlo  evla   Z ##########\n" );
        fprintf( fp, "EV   %.0f   %.0f   %.0f %.2f    %g    %g  %g\n",
			a->p1_str, a->p1_dip, a->p1_rak, a->momag, a->lon, a->lat, a->z );

        fprintf( fp, "#####################################################################################################\n" );
        fprintf( fp, "# sta net loc model  np pas lf  hf  nt  dt   tr  tt v/d  mulfac used(Y/N)  ts0  weight ###              #\n" );
	for( ista = 0; ista < a->nsta; ista++ )
	{
		string2lower( a->s[ista].grd_mo_type_v_or_d, 8 );

		if( a->s[ista].iused == 1 )
			strcpy( a->s[ista].used_y_or_n, "y" );
		else if( a->s[ista].iused == 0 )
			strcpy( a->s[ista].used_y_or_n, "n" );
		else
			strcpy( a->s[ista].used_y_or_n, "n" );
		

		/*************************************************************************/
		/* decimate_factor = 2;  nt = 1024 to 512; dt = 0.10 to 0.20 fnyq=2.5Hz */
		/* decimate_factor = 4;  nt = 1024 to 254; dt = 0.10 to 0.40 fnyq=1.25Hz */
		/* decimate_factor = 8;  nt = 1024 to 128; dt = 0.10 to 0.80 fnyq=0.625Hz */
		/*************************************************************************/

		if( decimate_factor == 8 || decimate_factor == 4 || decimate_factor == 2 )
		{
			nt = a->s[ista].nt / decimate_factor;
			dt = a->s[ista].dt * decimate_factor;
		}
		else if( decimate_factor == 1 )
		{
			nt = a->s[ista].nt;
			dt = a->s[ista].dt;
		}
		else
		{
			fprintf( stderr,
			  "%s: %s: %s: WARNING decimate_factor=%g out of range [2,4,8]\n",
				progname, __FILE__, __func__, decimate_factor );
			nt = a->s[ista].nt;
			dt = a->s[ista].dt;
		}
		
	/*** don't use the automatic time-shift assumes user adjusted already so fix 0 ***/
		if( use_ts0 )
			ts0 = a->s[ista].time_shift_user;
		else
			ts0 = a->s[ista].time_shift_all;

		snprintf( rec, 128, "%-8s %-4s %-4s %s %2d %2d %.3f %.3f %4d %g %g %g %s %g %s %+.3f %g %s ## R=%.1f Az=%.0f",
			a->s[ista].sta,
			a->s[ista].net,
			a->s[ista].loc,
			a->s[ista].modfile,
			a->s[ista].npole,
			a->s[ista].npass,
			a->s[ista].lf,
			a->s[ista].hf,
			nt,
			dt, 
			a->s[ista].tr,
			a->s[ista].tt,
			a->s[ista].grd_mo_type_v_or_d,
			a->s[ista].mul_fac,
			a->s[ista].used_y_or_n,
			ts0, 
			a->s[ista].weight,
			a->s[ista].wavetype,
			a->s[ista].rdist,
			a->s[ista].az );

		if( a->s[ista].iused == 1 )
			fprintf( fp, "%s\n", rec );
		else if( a->s[ista].iused == 0 )
			fprintf( fp, "#%s\n", rec );
	}

	fprintf( fp, "### NOTE! stas not used, are commented out and not loaded by mteig ###\n" );
	fprintf( fp, "### because there is no prediction only millions of forward calcs ###\n" );
	fprintf( fp, "EOF\n" );
	fprintf( fp, "\n" );
	
	fprintf( fp, "### PROCESS GREENS FUNCTIONS ###\n");
	fprintf( fp, "glib2inv par=mtinv.par noverbose parallel\n");
	fprintf( fp, "\n" );

	fprintf( fp, "### PROCESS DATA ###\n");
	fprintf( fp, "sacdata2inv par=mtinv.par path=%s respdir=%s noverbose nodumpsac parallel\n", DataDir, RespDir );
	fprintf( fp, "\n" );

	fprintf( fp, "### \n" );
	fprintf( fp, "### changed nsim_eig=2000 to 500 to speed up runtime\n" );
	fprintf( fp, "### \n" );

	if(igmt5)
		sprintf( gmt_opt, "gmt5" );
	else
		sprintf( gmt_opt, "nogmt5" );

	fprintf( fp, "time mteig par=mtinv.par nthreads=8 \\\n" );
	fprintf( fp, "                         nsim_eig=500 nsim_evec=4000 eigvec_fac=17000 \\\n" );
	fprintf( fp, "                         Mo=%e fixz=%g \\\n", a->m0_dyne_cm, a->z );
	fprintf( fp, "                         color doplt seed=1 %s parallel \\\n", gmt_opt );

	fprintf( fp, "                         title=\"%04d-%02d-%02d ",
		a->ot.year, a->ot.month, a->ot.mday );
	fprintf( fp, "%s", a->comment );
	fprintf( fp, "\" " );

/*** list stas ***/
/***
	for( ista=0; ista < a->nsta; ista++ )
	{
        	if( a->s[ista].iused == 1 )
		{
			fprintf( fp, " %s", a->s[ista].sta );
		}
	}
***/

/* sol[iz].FullMT.T.ev, sol[iz].FullMT.B.ev, sol[iz].FullMT.P.ev */
	fprintf( fp, "Add_user_Eig e0=%+g e1=%+g e2=%+g\n", a->eigt, a->eigb, a->eigp );
	fprintf( fp, "\n" );

	fclose(fp);
	chmod( "mteig.csh", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );

} /*** end of make_mteig() ***/

void string2lower( char *str, int n )
{
	int i;
	char ctmp;
	for( i = 0; i < n; i++ )
	{
		ctmp = tolower(str[i]);
		str[i] = ctmp;
	}
}

/******************************/
/**** void make_run() *********/
/******************************/

void make_run( bestFitMT *a, char *fitType, int igmt5, long evid, int pretty_plot, int idb )
{
	int i;
	FILE *fp;
	char gmtstring[8];
	char dbprog[8];

/*** set enviromental variables : gmt_grd_filename, gmt_int_filename, gmt_cpt_filename, mtinv_db_filename ***/
/***                        MTINV_GMT_GRID_FILE, MTINV_GMT_INT_FILE, MTINV_GMT_CPT_FILE, MT_DATABASE_FILE ***/

	char *gmt_grd_filename, *gmt_int_filename, *gmt_cpt_filename, *mtinv_db_filename;

/*** set some flags ***/

	a->igmt5 = igmt5;

        if(a->igmt5)
          strcpy( gmtstring, "gmt5" );
        else
          strcpy( gmtstring, "nogmt5" );

/*** use this one for automt ***/

	a->sqlite3_db_write = 1;
        strcpy( dbprog, "sqlite3" ); /*** default ***/

        if(a->oracle_db_write)
          strcpy( dbprog, "oracle" );
        else if(a->mysql_db_write)
          strcpy( dbprog, "mysql" );
        else if(a->sqlite3_db_write)
          strcpy( dbprog, "sqlite" );

/*** begin write run2.csh file ***/

	fp = fopen( "run2.csh", "w" );

	fprintf( fp, "#!/bin/csh \n");

	fprintf( fp, "###\n" );
	fprintf( fp, "### Created by %s fitType=%s\n", progname, fitType );
	fprintf( fp, "### ot=%g fsec=%g z=%g vred=%g fit=%g pdc=%g piso=%g pclvd=%g\n",
		a->ot_shift, a->fsec, a->z, a->vred, a->fit,
		roundf(a->pdc), roundf(a->piso), roundf(a->pclvd) );

	fprintf( fp, "### fit_max=%g fit_max_threshold=%g vred_diff=%g vred_diff_threshold=%g\n", 
			a->fit_max, 
			a->fit_max_threshold, 
			a->vred_diff, 
			a->vred_diff_threshold );

	fprintf( fp, "###\n" );

/*** set enviromental variables : gmt_grd_filename, gmt_grd_filename, gmt_cpt_filename, mtinv_db_filename ***/
/***                        MTINV_GMT_GRID_FILE, MTINV_GMT_INT_FILE, MTINV_GMT_CPT_FILE, MT_DATABASE_FILE ***/

	if( (gmt_grd_filename = getenv( "MTINV_GMT_GRID_FILE" )) == NULL )
	{
	  fprintf( fp, "setenv MTINV_GMT_GRID_FILE /Users/ichinose1/Work/topogmt/etopo5.grd\n" );
	}
	else
	{
	  fprintf( stderr, "%s: %s: %s: enviromental variable MTINV_GMT_GRID_FILE=%s\n",
		progname, __FILE__, __func__, gmt_grd_filename );
	  fprintf( fp, "setenv MTINV_GMT_GRID_FILE %s\n", gmt_grd_filename );
	}

	if( (gmt_int_filename = getenv( "MTINV_GMT_INT_FILE" )) == NULL )
	{
	  fprintf( fp, "setenv MTINV_GMT_INT_FILE  /Users/ichinose1/Work/topogmt/etopo5.int\n" );
	}
	else
	{
	  fprintf( stderr, "%s: %s: %s: enviromental variable MTINV_GMT_INT_FILE=%s\n",
                progname, __FILE__, __func__, gmt_int_filename );
	  fprintf( fp, "setenv MTINV_GMT_INT_FILE %s\n", gmt_int_filename );
	}

	if( (gmt_cpt_filename = getenv( "MTINV_GMT_CPT_FILE" )) == NULL )
	{
	  fprintf( fp, "setenv MTINV_GMT_CPT_FILE  /Users/ichinose1/Work/topogmt/etopo5.cpt\n" );
	}
	else
	{
	  fprintf( stderr, "%s: %s: %s: enviromental variable MTINV_GMT_CPT_FILE=%s\n",
                progname, __FILE__, __func__, gmt_cpt_filename );
	  fprintf( fp, "setenv MTINV_GMT_CPT_FILE %s\n", gmt_cpt_filename );
	}

	if( (mtinv_db_filename = getenv( "MT_DATABASE_FILE" )) == NULL )
	{
	  fprintf( fp, "setenv MT_DATABASE_FILE  /Users/ichinose1/Work/mtinv.v3.0.6/data/mt.db\n" );
	}
	else
	{
	  fprintf( stderr, "%s: %s: %s: enviromental variable MTINV_DATABASE_FILE=%s\n",
                progname, __FILE__, __func__, mtinv_db_filename );
	  fprintf( fp, "setenv MTINV_DATABASE_FILE %s\n", mtinv_db_filename );
	}
	fprintf( fp, "\n" );

        if( strcmp( a->mt_type, "FULL" ) == 0 )
        {
           fprintf( fp, "set DEGFREE=6  # 1-isotropic_mt 5-deviatoric_mt 6-full_mt\n" );
        }
        else if( strcmp( a->mt_type, "DEV" ) == 0 )
        {
           fprintf( fp, "set DEGFREE=5  # 1-isotropic_mt 5-deviatoric_mt 6-full_mt\n" );
        }
	else if( strcmp( a->mt_type, "ISO" ) == 0 )
	{
		fprintf( fp, "set DEGFREE=1  # 1-isotropic_mt 5-deviatoric_mt 6-full_mt\n" );
	}
	else
	{
		fprintf( fp, "set DEGFREE=5  # 1-isotropic_mt 5-deviatoric_mt 6-full_mt\n" );
	}
        fprintf( fp, "\n" );

	fprintf( fp, "###\n" );
        fprintf( fp, "### Clean Up\n" );
        fprintf( fp, "###\n" );

        fprintf( fp, "/bin/rm -f email_T???.?sec_Z???.?km_.txt plot_T???.?sec_Z???.?km_.p??.jpg *.ps\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "### MAKE GMT PLOT WITH LOCATION/STATION/SOLUTION ###\n" );
        fprintf( fp, "###\n" );

        fprintf( fp, "mtinv \\\n" );
	if( a->ifixz == 1 ) 
	fprintf( fp, "\t fixz=%g	\\\n", a->fixz );
	fprintf( fp, "\t evid=%ld       \\\n", evid );
        fprintf( fp, "\t ts0=%g         \\\n", -1 * a->ot_shift );
        fprintf( fp, "\t par=mtinv.par  \\\n" );
        fprintf( fp, "\t %s             \\\n", gmtstring );
        fprintf( fp, "\t mtdegfree=${DEGFREE} \\\n" );
        fprintf( fp, "\t gmtmap           \\\n" );
	fprintf( fp, "\t write_emails     \\\n" );

        if(pretty_plot) 
		fprintf( fp, "\t dumpxy nodumpsac print_gmtwf_mt_info \\\n" );
	else 
		fprintf( fp, "\t nodumpxy nodumpsac noprint_gmtwf_mt_info \\\n" );

        fprintf( fp, "\t %s             \\\n", dbprog );
        fprintf( fp, "\t PltXcorLabel   \\\n" );
        fprintf( fp, "\t use_snr        \\\n" );
        fprintf( fp, "\t minsnr=%g      \\\n", a->minsnr );
        fprintf( fp, "\t ctol=%g shift  \\\n", a->cortol );
        fprintf( fp, "\t maxshift=%g >> mtinv.out\n", a->maxtimeshift );
        fprintf( fp, "\n" );

/******* run2.csh : only need one command for all ****/
	if(a->igmt5)
	 fprintf( fp, "gmt psconvert -Z -Tj -E300 plot_T???.?sec_Z???.?km_.p??.ps\n" );
	else
	 fprintf( fp, "ps2raster -Tj -E300 plot_T???.?sec_Z???.?km_.p??.ps\n" );

/*****
	for( i = 0; i < a->npages; i++ )
	{
	  if(a->igmt5)
          {
                fprintf( fp, "gmt psconvert -Z -Tj -E300 %s\n", a->psplotfiles[i] );
          }
          else
          {
                fprintf( fp, "ps2raster -Tj -E300 %s\n", a->psplotfiles[i] );
          }
	}
*******/

/*** middle part of run2.csh, create gmtmap.csh plot ***/

        fprintf( fp, "csh gmtmap.csh\n" );
	fprintf( fp, "#open gmtmap.jpg\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "### dumpxy option for publication quality GMT plots\n" );
	if(pretty_plot)
         fprintf( fp, "gmtwf.csh\n" );
	else
	 fprintf( fp, "#gmtwf.csh\n" );
        fprintf( fp, "\n" );

/*** depending on type of database, write out results.sql file for loading ***/

	if( a->sqlite3_db_write )
        {
		if(idb)
		{
                 fprintf( fp, "\n" );
                 fprintf( fp, "sqlite3 ${MT_DATABASE_FILE} << EOF\n" );
                 fprintf( fp, ".read insert.sql\n" );
                 fprintf( fp, ".quit\n" );
                 fprintf( fp, "EOF\n" );
		 fprintf( fp, "\n" );
                 fprintf( fp, "# updatedb conflicts in unix/Linux, changed name to updateMTdb \n" );
		 fprintf( fp, "updateMTdb\n" );
                 fprintf( fp, "\n" );


		 if( (mtinv_db_filename = getenv( "MT_DATABASE_FILE" )) != NULL )
		 {
			fprintf( fp, "### %s : current MT_DATABASE_FILE=%s\n", 
				progname, mtinv_db_filename );
			fprintf( fp, "# list_MTdb.csh\n" );
                   	fprintf( fp, "print_MTdb.csh > db.txt\n" );
                   	fprintf( fp, "# remove_MTdb.csh\n" );
		 }
		 else
		 {
                   fprintf( fp, "# list_MTdb.csh\n" );
                   fprintf( fp, "print_MTdb.csh > db.txt\n" );
                   fprintf( fp, "# remove_MTdb.csh\n" );
		 }
		}
		else
		{
                 fprintf( fp, "#\n" );
                 fprintf( fp, "# sqlite3 ${MT_DATABASE_FILE} << EOF\n" );
                 fprintf( fp, "# .read insert.sql\n" );
                 fprintf( fp, "# .quit\n" );
                 fprintf( fp, "# EOF\n" );
                 fprintf( fp, "#\n" );

                 fprintf( fp, "### updatedb conflicts in unix/Linux, changed name to updateMTdb \n" );
                 fprintf( fp, "# updateMTdb\n" );
	
                 fprintf( fp, "#\n" );
                 fprintf( fp, "### list_MTdb.csh ${MT_DATABASE_FILE}\n" );
                 fprintf( fp, "# print_MTdb.csh > db.txt\n" );
                 fprintf( fp, "### remove_MTdb.csh\n" );
		}
        }
        fclose(fp);

        chmod( "run2.csh", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );


	
} /*** end of make_run() ****/

void write_mtbest( bestFitMT *a )
{
	int j, ista;

	fprintf( stdout, "%f %f %f %f %f %f %s %d %s %s %s %d : \n",
		a->ot_shift,
		a->z,
		a->fsec,
		a->vred,
		a->fit,
		a->pdc,
		a->mt_type,
		a->mtdegfree,
		a->author,
		a->comment,
		a->email_file,
		a->npages );

	for( j = 0; j < a->npages; j++ )
	{
		fprintf( stdout, "\t(%s)\n", a->psplotfiles[j] );
	}

	fprintf( stdout, "nsta=%d\n", a->nsta );

	for( ista = 0; ista < a->nsta; ista++ )
	{
		fprintf( stdout,
"ista=%d sta=%s net=%s R=%g az=%g baz=%g iused=%d grdmo=%d v_or_d=%s np=%d lf=%g hf=%g pass=%d modfile=%s loc=(%s) nt=%d dt=%g tr=%g tt=%g mulfac=%g ts_xcor=%g ts_user=%g wt=%g wvt=%s\n",
			ista, 
                        a->s[ista].sta,
                        a->s[ista].net,
                        a->s[ista].rdist,
                        a->s[ista].az,
                        a->s[ista].baz,
                        a->s[ista].iused,
                        a->s[ista].grd_mo_type,
                        a->s[ista].grd_mo_type_v_or_d,
                        a->s[ista].npole,
                        a->s[ista].lf,
                        a->s[ista].hf,
                        a->s[ista].npass,
                        a->s[ista].modfile,
                        a->s[ista].loc,
                        a->s[ista].nt,
                        a->s[ista].dt,
                        a->s[ista].tr,
                        a->s[ista].tt,
                        a->s[ista].mul_fac,
                        a->s[ista].time_shift_all,
			a->s[ista].time_shift_user,
                        a->s[ista].weight,
                        a->s[ista].wavetype
		);
 	}                       
}

void createResultsWebpage( bestFitMT *a )
{
        FILE *fp;
	int i = 0;

        fp = fopen( "index.html", "w" );
        fprintf( fp, "<!DOCTYPE html>\n" );
        fprintf( fp, "<HTML>\n" );
        fprintf( fp, "<HEAD>\n" );
        fprintf( fp, "<TITLE>\n" );

	fprintf( fp, "%4d-%02d-%02d (%03d) %02d:%02d:%05.2f %.4f %.4f %s\n", 
		a->ot.year,
		a->ot.month,
		a->ot.mday,
		a->ot.jday,
		a->ot.hour,
		a->ot.min,
		a->ot.fsec,
		a->lat,
		a->lon,
		a->comment );

        fprintf( fp, "</TITLE>\n" );
        fprintf( fp, "</HEAD>\n" );

/*** email file ***/

	fprintf( fp, "<H1>%4d-%02d-%02d (%03d) %02d:%02d:%05.2f %.4f %.4f %s</H1>\n", 
                a->ot.year,
                a->ot.month,
                a->ot.mday,
                a->ot.jday,
                a->ot.hour,
                a->ot.min,
                a->ot.fsec,
                a->lat,
                a->lon,
                a->comment );

        fprintf( fp, "<P><A HREF=\"%s\">%s</A>\n", a->email_file, a->email_file );
        fprintf( fp, "<DIV ID=\"list\">\n" );
        fprintf( fp, "<P>\n" );
	fprintf( fp, "<IFRAME SRC=\"%s\" WIDTH=1200 HEIGHT=300 FRAMEBORDER=1></IFRAME>\n",
			a->email_file );
        fprintf( fp, "</P></DIV>\n" );
	fprintf( fp, "<P><IMG src=\"gmtmap.jpg\" style=\"width: 60%%; height: auto\" ></P>\n" );
	fprintf( fp, "<P>\n" );
	for( i = 0; i < a->npages; i++ )
	{
		fprintf( fp, "<IMG SRC=\"%s\"style=\"width: 60%%; height: auto\" >\n", 
				a->psplotfiles[i] );
	}
	fprintf( fp, "</P>\n" );

	if( strcmp( a->mt_type, "FULL" ) == 0 )
        {
		fprintf( fp, "<P><IMG SRC=\"results.6.jpg\" style=\"width: 60%%; height: auto\"/></P>\n" );
        }
        else if( strcmp( a->mt_type, "DEV" ) == 0 )
        {
		fprintf( fp, "<P><IMG SRC=\"results.5.jpg\" style=\"width: 60%%; height: auto\"/></P>\n" );
        }
        else if( strcmp( a->mt_type, "ISO" ) == 0 )
        {
                fprintf( fp, "<P><IMG SRC=\"results.1.jpg\" style=\"width: 60%%; height: auto\"/></P>\n" );
        }
        else
        {
                fprintf( fp, "<P><IMG SRC=\"results.5.jpg\" style=\"width: 60%%; height: auto\"/></P>\n" );
        }


	fprintf( fp, "<P><A HREF=\"plotmech.jpg\">plotmech.jpg</A></P>\n" );
	fprintf( fp, "<P><A HREF=\"plotz.jpg\">plotz.jpg</A></P>\n" );
	fprintf( fp, "<P><A HREF=\"db.txt\">db.txt</A>\n" );
	fprintf( fp, "<DIV ID=\"list\">\n" );
	fprintf( fp, "<P>\n" );
	fprintf( fp, "<IFRAME SRC=\"db.txt\" width=1200 height=300 frameborder=2 ></IFRAME>\n" );
	fprintf( fp, "</P>\n" );
	fprintf( fp, "</DIV>\n" );
	fprintf( fp, "</BODY>\n" );
        fprintf( fp, "</HTML>\n" );
        fclose(fp);
}

