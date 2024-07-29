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
 this is a helper function that autogenerates the run.csh and run2.csh files 
   reads *.glib files and determines which stations are used
   use after mkgrnlib or makeglib.csh 
***/

/*** gene.ichinose
 Mon Aug  3 16:01:58 PDT 2020
  
 todo list?

	add a feature to mv old version of run.csh to hidden directory
         rename old versions as run.csh.date_time.version_number

	make subroutine universal 

	int status = check_older_version_file( char *filename );
	1. check if file exists
	2. if yes then make a hidden directory .older_versions_parfiles
		if no then return with status = 0;
	3. mv the file to the hidden directory
	4. return with status = 1

	return back and write the new file version

 ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/utsname.h>

#include "../include/mt.h"

char progname[128];

#define MAX_STA 256

int main( int ac, char **av )
{
	Greens **grn;
	int nz, iz;
	float *z;
	int i, ix, ista;
	char comment[256];
	char dateid[256];
	FILE *fp;
	int it, nt, nsta;
	float min,max;
	int verbose=0;
	float *rdistkm;
	int *index;
	int indexx( int, float *, int * );
	float my_dt, lf=-1, hf=-1;
	float my_lf, my_hf;
	int my_nt;
	float minsnr = 3.0;
	float ctol = 0.85;
	float maxshift = 10.0;
	int igmt5 = 1;
	char gmtstring[8]; /* nogmt5 gmt5 */
	int ifullmt = 0;
	int first;
	float LOWEST_VELOCITY = 2.35;
	long evid=-1;
	char evidstr[32];

	int local_setup = 0;
	int irealtime = 0;
	int ioracle = 0;
	int imysql = 0;
	int isqlite = 1;
	char dbprog[8]; /* oracle mysql sqlite */
	char autoauth_string[12]; /*AutoAuth*/
	char RespDir[512], DataDir[512];
	char use;
	char com[3];
	char myloc[8];

	void select_dt_and_nt( float dist, float vel, int grn_npts, float grn_dt, float *dat_dt, int *dat_nt, int local_setup );
	void select_lf_and_hf( float dist, float dt, float lf, float hf, float *new_lf, float *new_hf, int local_setup );

	int setpar(int,char**);
	int mstpar( char *, char *, void * );
        int getpar( char *, char *, void * );
	void endpar();

	void Print_Usage(void);
	extern char progname[128];

	struct utsname my_uname;

/*** set enviromental variables : gmt_grd_filename, gmt_int_filename, gmt_cpt_filename, mtinv_db_filename ***/
/***                        MTINV_GMT_GRID_FILE, MTINV_GMT_INT_FILE, MTINV_GMT_CPT_FILE, MT_DATABASE_FILE ***/

        char *gmt_grd_filename, *gmt_int_filename, *gmt_cpt_filename, *mtinv_db_filename;

	int max_number_stations_invert = 8;
	float min_distance_define_km = 80;
	float max_distance_define_km = 800;

/*** start main ***/

	strcpy( progname, av[0] );
	fprintf( stderr, "%s: version=%s release date=%s\n",
		progname, Version_Label, Version_Date );

	if( uname(&my_uname) == -1 )
	{
		fprintf( stderr, "%s: uname() failed\n", progname );
	}
	else
	{
	/* Darwin Linux SunOS */

		fprintf( stdout, "%s: System name: %s\n", 
			progname, my_uname.sysname );

	/* ppc64 i386 i686 x86_64 amd64 ia64 sun4u */

		fprintf( stdout, "%s: Machine name: %s  Node Name: %s\n",
                        progname, my_uname.machine, my_uname.nodename );
	}

/*** defaults ***/
	strcpy( comment, "New Region" );
	/****required no default ***/
	/* strcpy( dateid, "2008/01/01,00:00:00.00" ); */ 
	strcpy( DataDir, "../Data" );
	strcpy( RespDir, "../Resp" );
	strcpy( evidstr, "-1" );

	if( ac <= 1 ) 
	{
		Print_Usage();
	}

	setpar(ac,av);
	mstpar( "date", "s", dateid );

	getpar( "com", "s", comment );
	getpar( "DataDir", "s", DataDir );
	getpar( "RespDir", "s", RespDir );

	getpar( "local", "b", &local_setup );
	getpar( "verbose", "b", &verbose );
	getpar( "minsnr", "f", &minsnr );
	getpar( "ctol", "f", &ctol );
	getpar( "maxshift", "f", &maxshift );
	getpar( "lf", "f", &lf );
	getpar( "hf", "f", &hf );
	getpar( "gmt5", "b", &igmt5 );
	getpar( "fullmt", "b", &ifullmt );
	getpar( "realtime", "b", &irealtime );

	getpar( "oracle", "b", &ioracle );
	getpar( "mysql", "b", &imysql );
	getpar( "sqlite", "b", &isqlite );

	getpar( "maxsta", "d", &max_number_stations_invert );
	getpar( "mindist", "f", &min_distance_define_km );
	getpar( "maxdist", "f", &max_distance_define_km );

	getpar( "evid", "s", evidstr );
	if( strcmp(evidstr,"-1") != 0 )
		evid = strtol( evidstr, NULL, 10 );

	endpar();

/*** set some flags ***/
	
	strcpy( gmtstring, "nogmt5" ); /*** default ***/
	if(igmt5) 
          strcpy( gmtstring, "gmt5" );
        else
          strcpy( gmtstring, "nogmt5" );

	strcpy( dbprog, "sqlite" ); /*** default ***/
        if(ioracle)
          strcpy( dbprog, "oracle" );
        else if(imysql)
          strcpy( dbprog, "mysql" );
        else if(isqlite)
          strcpy( dbprog, "sqlite" );

/*** set AutoAuth ***/
        if( irealtime ) 
                strcpy( autoauth_string, "AutoAuth" );
        else
                strcpy( autoauth_string, "" ); /**or noAutoAuth **/

/***************************/
/*** load the glib files ***/
/***************************/

	grn = (Greens **)malloc( MAX_STA*sizeof(Greens *) );
	for( ista=0, i=1; i<ac; i++ )
	{
		if( (fp=fopen(av[i],"r")) == NULL )
		{
			continue;
		}
		fread( &nz, sizeof(int), 1, fp );
		if(verbose) fprintf( stderr, "%s: i=%d ista=%d file=%s nz=%d\n", 
				progname, i, ista, av[i], nz );
		z = malloc( nz*sizeof(float) );
		fread( &z[0], nz*sizeof(float), 1, fp );
		grn[ista] = (Greens *)malloc( nz*sizeof(Greens) );
		for( iz=0; iz<nz; iz++ )
		{
			fread( &grn[ista][iz], sizeof(Greens), 1, fp );
			nt = grn[ista][iz].nt;
			min = grn[ista][iz].g.rss[0];
			max = grn[ista][iz].g.rss[0];
			for( it=1; it<nt; it++ )
			{
			  if( grn[ista][iz].g.rss[it] < min ) min =  grn[ista][iz].g.rss[it];
			  if( grn[ista][iz].g.rss[it] > max ) max =  grn[ista][iz].g.rss[it];
			}
			if(verbose)
			{
			  fprintf(stderr, 
			    "%s: iz=%d z=%g %s.%s.%s rdist=%.1f az=%.0f t0=%.2f dt=%.2f nt=%d min=%.2e max=%.2e fn=%s modf=%s\n",
				progname, iz, grn[ista][iz].evdp, 
				grn[ista][iz].net, grn[ista][iz].stnm, grn[ista][iz].loc, 
				grn[ista][iz].rdist, grn[ista][iz].az,
				grn[ista][iz].t0, grn[ista][iz].dt, grn[ista][iz].nt, min, max,
				grn[ista][iz].filename, grn[ista][iz].v.modfile );
			}
		}
		fclose(fp);
		ista++;
	}
	nsta= ista;
	if(verbose) fprintf(stderr, "nsta=%d\n", nsta );

/******************************************************************/
/*** sort the src-rec distance and print in increasing distance ***/
/******************************************************************/
	
	rdistkm = (float *)calloc( nsta+1, sizeof(float) );
	index = (int *)calloc( nsta+1, sizeof(int) );
	if(verbose) fprintf(stderr, "done allocating memory for rdistkm and index\n" );

	if( nsta == 1 )
	{
		rdistkm[1] = grn[0][0].rdist;
		index[1] = 1;
	}
	else
	{
		for( ista=0; ista<nsta; ista++ )
		{
			rdistkm[ista+1] = grn[ista][0].rdist;
		}
		indexx( nsta, rdistkm, index );
	}

/******************************************************************/
/*** begin writting run.csh file *****/
/******************************************************************/

	if( fopen( "run.csh", "r" ) != NULL )
	{
		fprintf( stderr, "%s: file run.csh already exists. ", progname );  
		fprintf( stderr, "I cannot overwrite. Please rename or delete the file run.csh\n\n" );
		exit(-1);
	}

	fprintf( stderr, "%s: writting output to file run.csh\n\n", progname );

	fp = fopen( "run.csh", "w" );
	fprintf( fp, "#!/bin/csh \n");

	if(irealtime) fprintf( fp, "#### Realtime version - set in makepar #### \n" );
	if(!irealtime) fprintf( fp, "#### Interactive version - set in makepar #### \n" );

	if( ifullmt )
	{
	   fprintf( fp, "\n### uncomment the one needed 1-isotropic mt  5-deviatoric mt  6-full mt\n" );
	   fprintf( fp, "set DEGFREE=6\n" );
	   fprintf( fp, "#set DEGFREE=1\n" );
	}
	else
	{
	   fprintf( fp, "set DEGFREE=5  # 1-isotropic_mt 5-deviatoric_mt 6-full_mt\n" );
	}
	fprintf( fp, "\n" );

	fprintf( fp, "cat >! mtinv.par << EOF\n" );
	fprintf( fp, "#### REGION COMMENT ############################\n" );
	fprintf( fp, "CM %s\n", comment );
	fprintf( fp, "#### Date and Origin Time ######################\n" );
	fprintf( fp, "OT %s\n", dateid );
	fprintf( fp, "#### Forward Calculations ######################\n" );
	fprintf( fp, "##    stk    dip    rak   Mw  evlo  evla   Z ##########\n" );
	fprintf( fp, "EV -999.0 -999.0 -999.0  0.0    %g    %g  15.0\n", grn[0][0].evlo, grn[0][0].evla );
	fprintf( fp, "#####################################################################################################\n" );
	fprintf( fp, "# sta net loc model  np pas lf  hf  nt  dt   tr  tt v/d  mulfac used(Y/N)  ts0  weight ###              #\n" );

	first = 0;
	for( ista=0; ista<nsta; ista++ )
	{
		strcpy( com, "" );

		ix = index[ista+1]-1;

		select_dt_and_nt( grn[ix][0].rdist, LOWEST_VELOCITY, grn[ix][0].nt, grn[ix][0].dt, &my_dt, &my_nt, local_setup );

		if( ( ( lf == -1 ) && ( hf == -1 ) ) || local_setup )
		{
		  select_lf_and_hf( grn[ix][0].rdist, my_dt, lf, hf, &my_lf, &my_hf, local_setup );
		}
		else
		{
		  my_lf=lf;
		  my_hf=hf;	  
		}

		if( my_dt < grn[ix][0].dt ) my_dt = grn[ix][0].dt;

	/*** only allow max_number_stations_invert stations to invert ****/

		if( ista < max_number_stations_invert )
			use = 'y';
		else
			use = 'n';

	/*** but when station distances in less than or greater than limits ***/

		if( grn[ix][0].rdist > max_distance_define_km ) use = 'n';
		if( grn[ix][0].rdist < min_distance_define_km ) use = 'n';

	/*** comment out station greater than distance and number limits ***/

		if( grn[ix][0].rdist > max_distance_define_km || ista > max_number_stations_invert - 1 ) 
		{
			strcpy( com, "#" );
			if( first == 0 )
			{
			  fprintf( fp, "#########################################################################################################\n" );
			  first = 1;
			}
		}

	/**** write out line ***/
	
		if( strcmp( grn[ix][0].loc, "" ) == 0 || strcmp( grn[ix][0].loc, "--" ) == 0 )
			strcpy( myloc, "\"\"" ); 
		else
			strcpy( myloc, grn[ix][0].loc );
	
		fprintf( fp, "%s%s\t%s\t%s\t%s 3 2 %.3f %.3f %5d %5.2f 0.0 0.0 d  1.0 %c  +0.00 +1.0 Surf/Pnl ### R=%.1f Az=%.0f\n",
			com,
			grn[ix][0].stnm,
			grn[ix][0].net,
			myloc,
			grn[ix][0].v.modfile,
			my_lf, 
			my_hf,
			my_nt, 
			my_dt,
			use, 
			grn[ix][0].rdist,
			grn[ix][0].az 
		);
	}
	fprintf( fp, "EOF\n" );
	fprintf( fp, "\n" );


/*** this section sets the interactive level of the csh-script allows for plotting ***/
        if(!irealtime)
        {
         fprintf( fp, "#######################################################################\n" );
         fprintf( fp, "if( $#argv == 1 ) then\n" );
         fprintf( fp, "  if( ${argv[1]} == \"i\" || ${argv[1]} == \"interactive\" ) then\n" );
         fprintf( fp, "     set interactive_level=1\n" );
         fprintf( fp, "  else if( ${argv[1]} == \"a\" || ${argv[1]} == \"automatic\" ) then\n" );
         fprintf( fp, "     set interactive_level=0\n" );
         fprintf( fp, "  else\n" );
         fprintf( fp, "    echo $0 requires option a or i : a=automatic or i=interactive\n" );
         fprintf( fp, "    exit(-1)\n" );
         fprintf( fp, "  endif\n" );
         fprintf( fp, "else\n" );
         fprintf( fp, "  echo $0 requires 1 argument got $#argv\n" );
         fprintf( fp, "  echo $0 requires option a or i : a=automatic or i=interactive\n" );
         fprintf( fp, "  exit(-1)\n" );
         fprintf( fp, "endif\n" );
         fprintf( fp, "#######################################################################\n" );
        }

	fprintf( fp, "### CLEAN UP ###\n" );
	fprintf( fp, "/bin/rm -f *.ginv *.data plot_T???.?sec_Z???.?km_.p??.ps email_T???.?sec_Z???.?km_.txt *.?.dat.xy *.?.syn.xy\n" );
	fprintf( fp, "/bin/rm -f plot_T???.?sec_Z???.?km_.p??.jpg plot_T???.?sec_Z???.?km_.p??.pdf *.sql\n" );
	fprintf( fp, "/bin/rm -f results.?.??? plotmech.??? plotz.??? gmtmap.??? mtinv.out multithread_mkgrnlib.out snr.out var_red.out\n" );

	if( irealtime ) fprintf( fp, "/bin/rm -f automt.txt\n" );

	fprintf( fp, "\n" );

	fprintf( fp, "### PROCESS GREENS FUNCTIONS ###\n" );
	fprintf( fp, "glib2inv par=mtinv.par noverbose parallel\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "### PROCESS DATA ### \n" );
	fprintf( fp, "sacdata2inv par=mtinv.par path=%s respdir=%s noverbose nodumpsac parallel\n", DataDir, RespDir );
	fprintf( fp, "\n" );

/*** forward modeling ****/
	if(!irealtime)
	{
	  fprintf( fp, "### FORWARD CALCULATION (Set Parameters in EV line of mtinv.par) ###\n" );
	  fprintf( fp, "### \n" );
	  fprintf( fp, "# mtinv fwd evid=%ld ts0=0 %s par=mtinv.par mtdegfree=${DEGFREE}\n", evid, gmtstring );
	  fprintf( fp, "\n" );
	}

	fprintf( fp, "# foreach ts0 ( -8 -7 -6 -5 -4 -3 -2 -1 -0.5 0 0.5 1 2 3 4 5 6 7 8 )\n" );
	fprintf( fp, "foreach ts0 ( -8 -7 -6 -5 -4 -3 -2 -1 0 1 2 3 4 5 6 7 8 )\n" );
	if( evid>0 )
	{
	  fprintf(fp,
	    " mtinv evid=%ld %s ts0=${ts0} par=mtinv.par %s mtdegfree=${DEGFREE} use_snr minsnr=%g shift ctol=%g maxshift=%g >> mtinv.out\n",
               	evid, autoauth_string, gmtstring, minsnr, ctol, maxshift );
	}
	else
	{
	  fprintf(fp, 
	    " mtinv %s ts0=${ts0} par=mtinv.par %s mtdegfree=${DEGFREE} use_snr minsnr=%g shift ctol=%g maxshift=%g >> mtinv.out\n",
		autoauth_string, gmtstring, minsnr, ctol, maxshift );
	}
	fprintf( fp, "end ### loop over ts0\n" );
	fprintf( fp, "\n" );

/***********************************/
/*** check origin time shift    ****/
/***********************************/
	fprintf( fp, "### CHECK ORIGIN TIME SHIFT ###\n" );
	fprintf( fp, "csh results.${DEGFREE}.csh\n" );

	if( !irealtime )
	{
	  if( strcmp( my_uname.sysname, "Darwin" ) == 0 )
	  {
		fprintf( fp, "if( ${interactive_level} == 1 ) then\n" );
		fprintf( fp, "open results.${DEGFREE}.jpg\n" );
		fprintf( fp, "endif\n" );
	  }
	  else if( strcmp( my_uname.sysname, "Linux" ) == 0 )
	  {
		fprintf( fp, "#eog results.${DEGFREE}.jpg\n" );
		fprintf( fp, "xdg-open results.${DEGFREE}.jpg\n" );
	  }
	  else if( strcmp( my_uname.sysname, "SunOS" ) == 0 )
	  {
		fprintf( fp, "xv results.${DEGFREE}.jpg\n" );
	  }
	  else
	  {
		fprintf( fp, "gs results.${DEGFREE}.ps\n" );
	  }
	}
	else /* don't look at if automatic realtime pipeline processing */
	{
	  fprintf( fp, "### uncomment for review\n" );
	  fprintf( fp, "# open results.?.jpg\n" );
	}
	fprintf( fp, "\n" );

/***********************************/
/*** generate waveform plots     ***/
/***********************************/
/*
	fprintf( fp, "### Use Ghostview to view PS files ###\n" );
	fprintf( fp, "#gs -dEPSCrop plot_T???.?sec_Z???.?km_.p??.ps\n" );
*/
	fprintf( fp, "### convert each postscript file to jpg \n" );
	fprintf( fp, "foreach ps ( plot_T???.?sec_Z???.?km_.p??.ps ) \n" );
	if(igmt5)
	{            /*** lower the resolution JPEG to just 120 pixesl per inch and remove PS ***/
		fprintf( fp, " gmt psconvert -Z -Tj -E120 ${ps}\n" ); /*** GMT 5.x.x ***/
	}
	else
	{
		fprintf( fp, "  ps2raster -Tj -E120 -A ${ps}\n" ); /*** GMT 4.x.x ***/
	}
	fprintf( fp, "end\n" );

	if( !irealtime )
	{
	  if( strcmp( my_uname.sysname, "Darwin" ) == 0 )
          {
		fprintf( fp, "if( ${interactive_level} == 1 ) then\n" );
                fprintf( fp, "  open plot_T???.?sec_Z???.?km_.p??.jpg\n" );
                fprintf( fp, "endif\n" );
          }
          else if( strcmp( my_uname.sysname, "Linux" ) == 0 )
          {
		fprintf( fp, "if( ${interactive_level} == 1 ) then\n" );
		fprintf( fp, "# eog plot_T???.?sec_Z???.?km_.p??.jpg\n" );
		fprintf( fp, "  xdg-open plot_T???.?sec_Z???.?km_.p??.jpg\n" );
		fprintf( fp, "endif\n" );
          }
          else if( strcmp( my_uname.sysname, "SunOS" ) == 0 )
          {
		fprintf( fp, "if( ${interactive_level} == 1 ) then\n" );
		fprintf( fp, "  xv plot_T???.?sec_Z???.?km_.p??.jpg\n" );
		fprintf( fp, "endif\n" );
          }
          else
          {
		fprintf( fp, "if( ${interactive_level} == 1 ) then\n" );
                fprintf( fp, "  gs -dEPSCrop plot_T???.?sec_Z???.?km_.p??.ps\n" );
		fprintf( fp, "endif\n" );
          }
	}
	else /** don't look at if automatic realtime pipeline processing  **/
	{
		fprintf( fp, "### uncomment \n" );
		fprintf( fp, "#open plot_T???.?sec_Z???.?km_.p??.jpg\n" );
	}
        fprintf( fp, "\n" );

/***********************************/
/*** make depth sensitivity plot ***/
/***********************************/

	fprintf( fp, "### MAKE DEPTH SENSITIVITY PLOT ###\n" );
	if( irealtime )
	{
		fprintf( fp, "# csh plotz.csh\n" );
	}
	else
	{
		fprintf( fp, "if( ${interactive_level} == 1 ) then\n" );
		fprintf( fp, " csh plotz.csh\n" );
		fprintf( fp, "endif\n" );
	}

	
	if( strcmp( my_uname.sysname, "Darwin" ) == 0 )
        {
		if( irealtime )
		{
			fprintf( fp, "# open plotz.jpg\n" );
		}
		else
		{
			fprintf( fp, "if( ${interactive_level} == 1 ) then\n" );
			fprintf( fp, " open plotz.jpg\n" );
			fprintf( fp, "endif\n" );
		}
        }
        else if( strcmp( my_uname.sysname, "Linux" ) == 0 )
        {
		if( irealtime )
		{
                	fprintf( fp, "#eog plotz.jpg\n" );
			fprintf( fp, "#xdg-open plotz.jpg\n" );
		}
		else
		{
			fprintf( fp, "if( ${interactive_level} == 1 ) then\n" );
			fprintf( fp, "# eog plotz.jpg\n" );
			fprintf( fp, "  xdg-open plotz.jpg\n" );
			fprintf( fp, "endif\n" );
		}
        }
        else if( strcmp( my_uname.sysname, "SunOS" ) == 0 )
        {
                fprintf( fp, "#xv plotz.jpg\n" );
        }
        else
        {
                fprintf( fp, "#gs plotz.ps\n" );
        }
        fprintf( fp, "\n" );

/**********************************************/
/*** MAKE DEPTH / OT-SHIFT SENSITIVITY PLOT ***/
/**********************************************/

	fprintf( fp, "### MAKE DEPTH / OT-SHIFT SENSITIVITY PLOT ###\n" );
        if( irealtime )
        {
		fprintf( fp, "# csh plotmech.csh\n" );
        }               
        else            
        {               
                fprintf( fp, "if( ${interactive_level} == 1 ) then\n" );
                fprintf( fp, " csh plotmech.csh\n" );
                fprintf( fp, "endif\n" );
        }

	if( strcmp( my_uname.sysname, "Darwin" ) == 0 )
        {
		if( irealtime )
		{
                	fprintf( fp, "# open plotmech.jpg\n" );
		}
		else
		{
			fprintf( fp, "if( ${interactive_level} == 1 ) then\n" );
			fprintf( fp, "  open plotmech.jpg\n" );
			fprintf( fp, "endif\n" );
		}
        }
        else if( strcmp( my_uname.sysname, "Linux" ) == 0 )
        {
		if( irealtime )
		{
                	fprintf( fp, "#eog plotmech.jpg\n" );
			fprintf( fp, "#xdg-open plotmech.jpg\n" );
		}
		else
		{
			fprintf( fp, "if( ${interactive_level} == 1 ) then\n" );
			fprintf( fp, "# eog plotmech.jpg\n" );
			fprintf( fp, "  xdg-open  plotmech.jpg\n" );
			fprintf( fp, "endif\n" );
		}
        }
        else if( strcmp( my_uname.sysname, "SunOS" ) == 0 )
        {
                fprintf( fp, "#xv plotmech.jpg\n" );
        }
        else
        {
                fprintf( fp, "#gs plotmech.ps\n" );
        }
        fprintf( fp, "\n" );

/*****************************************************************************************************/
/*** generate a map with mt solution, this is best done in run2.csh after best solution determined ***/
/*****************************************************************************************************/
        fprintf( fp, "#csh gmtmap.csh\n" );

        if( strcmp( my_uname.sysname, "Darwin" ) == 0 )
        {
                fprintf( fp, "#open gmtmap.jpg\n" );
        }
        else if( strcmp( my_uname.sysname, "Linux" ) == 0 )
        {
                fprintf( fp, "#eog gmtmap.jpg\n" );
		fprintf( fp, "#xdg-open gmtmap.jpg\n" );
        }
        else if( strcmp( my_uname.sysname, "SunOS" ) == 0 )
        {
                fprintf( fp, "#xv gmtmap.jpg\n" );
        }
        else
        {
                fprintf( fp, "#gs gmtmap.ps\n" );
        }
	fprintf( fp, "\n" );

/**** create a run2.csh file using automt.txt output ****/

	if(igmt5)
	{
	  fprintf( fp,
	    "mtbestfit gmt5 evid=%ld db pretty_plot noforce_best_vred mteig decimate_factor=2 DataDir=%s RespDir=%s\n", evid, DataDir, RespDir );
	}
	else
	{
	  fprintf( fp,
	    "mtbestfit nogmt5 evid=%ld db pretty_plot noforce_best_vred mteig decimate_factor=2 DataDir=%s RespDir=%s\n", evid, DataDir, RespDir );
	}
	fclose(fp);

	chmod( "run.csh", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );

/*************************************************************************/
/**** Write the CSHELL script to finalize the moment tensor solution ****/
/**** assumes the run.csh script was run and OT set to 0 offset      ****/
/*************************************************************************/

	if( fopen( "run2.csh", "r" ) != NULL )
        {
                fprintf( stderr, "%s: file run2.csh already exists. ", progname );
                fprintf( stderr, "I cannot overwrite. Please rename or delete the file run2.csh\n\n" );
                exit(-1);
        }
	else
	{
        	fprintf( stderr, "%s: writting output to file run2.csh\n\n", progname );
	}

        fp = fopen( "run2.csh", "w" );

        fprintf( fp, "#!/bin/csh \n");

/*** set enviromental variables : gmt_grd_filename, gmt_int_filename, gmt_cpt_filename, mtinv_db_filename ***/
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
	  fprintf( fp, "setenv MT_DATABASE_FILE %s\n", mtinv_db_filename );
	}

	fprintf( fp, "\n" );

        if( ifullmt )
        {
	   fprintf( fp, "###\n" );
           fprintf( fp, "### uncomment the one needed 1-isotropic mt  5-deviatoric mt  6-full mt\n" );
	   fprintf( fp, "###\n" );
           fprintf( fp, "set DEGFREE=6\n" );
           fprintf( fp, "#set DEGFREE=1\n" );
        }
        else
        {
	   fprintf( fp, "### 1-isotropic_mt  5-deviatoric_mt  6-full_mt\n" );
	   fprintf( fp, "###\n" );
           fprintf( fp, "set DEGFREE=5  # 1-isotropic_mt 5-deviatoric_mt 6-full_mt\n" );
        }
        fprintf( fp, "\n" );

	fprintf( fp, "###\n" );
	fprintf( fp, "### Clean Up\n" );
	fprintf( fp, "###\n" );

	fprintf( fp, "/bin/rm -f email_T???.?sec_Z???.?km_.txt plot_T???.?sec_Z???.?km_.p??.jpg *.ps\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "###\n" );
	fprintf( fp, "### MAKE GMT PLOT WITH LOCATION/STATION/SOLUTION ###\n" );
	fprintf( fp, "###\n" );

        fprintf( fp, "mtinv \\\n" );
	fprintf( fp, "\t evid=%ld       \\\n", evid );
        fprintf( fp, "\t ts0=0          \\\n" );
        fprintf( fp, "\t par=mtinv.par  \\\n" );
        fprintf( fp, "\t %s             \\\n", gmtstring );
        fprintf( fp, "\t mtdegfree=${DEGFREE} \\\n" );
        fprintf( fp, "\t gmtmap         \\\n" );
        fprintf( fp, "\t nodumpxy       \\\n" );
        fprintf( fp, "\t %s             \\\n", dbprog );
        fprintf( fp, "\t PltXcorLabel   \\\n" );
        fprintf( fp, "\t use_snr        \\\n" );
        fprintf( fp, "\t minsnr=%g      \\\n", minsnr );
        fprintf( fp, "\t ctol=%g        \\\n", ctol );
        fprintf( fp, "\t maxshift=%g >> mtinv.out\n", maxshift );
        fprintf( fp, "\n" );

	if(igmt5)
        {
		/*** final higher resolution JPEG 300 pixesl per inch and remove PS ***/
                fprintf( fp, "gmt psconvert -Z -Tj -E300 plot_T???.?sec_Z???.?km_.p??.ps\n" );
        }
        else
        {
                fprintf( fp, "  ps2raster -Tj -E300 plot_T???.?sec_Z???.?km_.p??.ps\n" );
        }

        fprintf( fp, "csh gmtmap.csh\n" );

        if( strcmp( my_uname.sysname, "Darwin" ) == 0 )
        {
                fprintf( fp, "open gmtmap.jpg\n" );
        }
        else if( strcmp( my_uname.sysname, "Linux" ) == 0 )
        {
                fprintf( fp, "#eog gmtmap.jpg\n" );
		fprintf( fp, "#xdg-open gmtmap.jpg\n" );
        }
        else if( strcmp( my_uname.sysname, "SunOS" ) == 0 )
        {
                fprintf( fp, "#xv gmtmap.jpg\n" );
        }
        else
        {
                fprintf( fp, "gs gmtmap.ps\n" );
        }
	fprintf( fp, "\n" );

	fprintf( fp, "### dumpxy option for publication quality GMT plots\n" );
	fprintf( fp, "#gmtwf.csh\n" );
	fprintf( fp, "\n" );

	if( isqlite )
	{
        	fprintf( fp, "\n" );
		fprintf( fp, "sqlite3 ${MT_DATABASE_FILE} << EOF\n" );
		fprintf( fp, ".read insert.sql\n" );
		fprintf( fp, ".quit\n" );
		fprintf( fp, "EOF\n" );

		fprintf( fp, "updateMTdb\n" );
		fprintf( fp, "\n" );
		fprintf( fp, "# list_MTdb.csh ${MT_DATABASE_FILE}\n" );
		fprintf( fp, "print_MTdb.csh > db.txt\n" );
		fprintf( fp, "# remove_MTdb.csh\n" );
	}
	fprintf( fp, "\n" );
	fclose(fp);

	chmod( "run2.csh", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );


	exit(0);

} /**** end of main() ***/

/**************************************************************************************************/
/*** Print Help Usage ***/
/**************************************************************************************************/

void Print_Usage()
{
	fprintf( stderr, "\n" );
	fprintf( stderr, "Usage: \n" );

	fprintf( stderr,
	  "\t %s date=\"yyy/mm/dd,hh:mm:ss.ss\" com=\"New Region Comment\" lf=(float) hf=(float)\n", progname );

	fprintf( stderr,
	  "\t\t minsnr=(float) ctol=(float) maxshift=(float) [no]gmt5 [no]fullmt DataDir=(string) RespDir=(string)\n" );

	fprintf( stderr,
	  "\t\t [no]oracle [no]mysql [no]sqlite [no]verbose *.glib\n" );

	fprintf( stderr, "\n\t REQUIRED PARAMETERS:\n" );
	fprintf( stderr, "\t\t date=(string) format: yyyy/mm/dd,hh:mm:ss.ss - Origin Time in exact format\n" );

	fprintf( stderr, "\n\t OPTIONAL PARAMETERS: (can be later changed in run.csh)\n" );
	fprintf( stderr, "\t\t com=(string)     A short comment that goes in the CM tag [default \"New Region\"]\n" );
	fprintf( stderr, "\t\t lf=(float)       Low frequency corner of the filter  [default 0.01 Hz] applies to all stations\n" );
        fprintf( stderr, "\t\t hf=(float)       High frequency corner of the filter  [default 0.1 Hz] applies to all stations\n" );
	fprintf( stderr, "\t\t minsnr=(float)   Minimum SNR allowed Peak-to-Peak amplitude in filtered band [default 3.0]\n" );
        fprintf( stderr, "\t\t ctol=(float)     Minimum Cross-Corrrelation needed to shift the waveforms [default 0.85]\n" );
        fprintf( stderr, "\t\t maxshift=(float) Maximum lag-time shift allowed when above ctol [default 10.0 sec]\n" );
        fprintf( stderr, "\t\t [no]gmt5         Make C-shell scripts compatible with GMT 5.x.x for plotting [default on]\n" );
        fprintf( stderr, "\t\t [no]fullmt       Do full moment tensor instead of deviatoric [default off]\n" );
	fprintf( stderr, "\t\t DataDir=(string) Directory where the *.SAC files are [default ../Data]\n" );
	fprintf( stderr, "\t\t RespDir=(string) Directory where the SAC_PZs_* files are [default ../Resp]\n" );
        fprintf( stderr, "\t\t [no]verbose      (boolean) turn on verbosity [default off]\n" );
	fprintf( stderr, "\t\t evid=(long)      EventID default -1 none\n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "\n\t OPTIONAL DATABASE OUTPUT PARAMETERS:\n" );
	fprintf( stderr, "\t\t [no]oracle=(boolean) Write SQL create & insert statements for ORACLE Database [default off]\n" );
	fprintf( stderr, "\t\t [no]mysql=(boolean)  Write SQL create & insert statements for MySQL Database [default off]\n" );
	fprintf( stderr, "\t\t [no]sqlite=(boolean) Write SQL create & insert statements for Sqlite3 Database [default on]\n" );
	fprintf( stderr, "\n" );
        fprintf( stderr, "\n\t OPTIONAL LOCAL-REGIONAL DISTANCE STATION TUNING PARAMETERS:\n" );
	fprintf( stderr, "\t\t maxsta=(integer)  Max_number_stations_invert [default 8]  default is nice because there is 8 panels per page\n" );
	fprintf( stderr, "\t\t mindist=(float)   Min_distance_define_km [default 80 km]   stations less than 80 km are only predicted\n" );
	fprintf( stderr, "\t\t maxdist=(float)   Max_distance_define_km [default 800 km]  stations greater than 800 km are only predicted\n" );
	fprintf( stderr, "\t\t [no]realtime      (boolean) sets up script to be automatic - default off\n");
	fprintf( stderr, "\t\t                     if realtime then no plot viewing and automt.txt file is setup for mtbestfit\n" );
	fprintf( stderr, "\n" );
	
	fprintf( stderr, "\tDESCRIPTION: \n" );
	fprintf( stderr, "\n\t This program auto-generates the run.csh C-shell script that allow\n" );
        fprintf( stderr, "\t\t for processing and inversion for Regional longperiod surface wave data\n" );
        fprintf( stderr, "\t NOTE! This program will not overwrite old run.csh and run2.csh scripts.  User must delete manually for safety.\n\n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "\n" );
	exit(0);
}


void select_dt_and_nt( float dist, float vel, int grn_npts, float grn_dt, float *dat_dt, int *dat_nt, int local_setup )
{
	float dt, slow_time;
	int nt;
	float roundoff( float, float );
	slow_time = dist/vel;

	nt = 1024;
	dt = 0.45;

	if( dist < 2000 )
	{
		nt = 1024;
		dt = slow_time/nt;
		dt = roundoff( dt, 100 );
	}

	if( dist < 100 )
	{
		nt = 1024;
		dt = slow_time/nt;
		dt = roundoff( dt, 100 );
	}


	if( dist <= 50 )
	{
		nt = 1024;
		dt = grn_dt + 0.02;
		dt = roundoff( dt, 100 );
	}

        if(local_setup)
        {
                if( dist < 250 ) dt = 0.1;
                if( dist < 180 ) dt = 0.08;
                if( dist < 160 ) dt = 0.07;
                if( dist < 130 ) dt = 0.06;
                if( dist < 100 ) dt = 0.05;
                if( dist <  35 ) dt = 0.05;
        }

	*dat_dt = dt;
	*dat_nt = nt;
}

float roundoff( float x, float base )
{
	return ((float)rint(x*base)/base);
}

void select_lf_and_hf( float dist, float dt, float lf, float hf, float *new_lf, float *new_hf, int local_setup )
{
	float fnyq;

	fnyq = 1.0/(2.0*dt);

	*new_lf = lf;
	*new_hf = hf;

	if( dist < 100 )
	{
		*new_lf = 0.020;
		*new_hf = 0.100;
	}

	if( *new_hf > fnyq ) *new_hf = fnyq;

	if(local_setup)
        {
                if( dist < 40 )
                {
                        *new_lf = 0.020;
                        *new_hf = 0.150;
                }
                else
                {
                        *new_lf = 0.020;
                        *new_hf = 0.100;
                }
	}
}
