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
#include <unistd.h>

/*** see mkdirp2.c - custom create dir recursevely ***/
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

/*** global datatype and structure declarations ***/
#include "../include/nrutil.h"
#include "../include/mt.h"

extern char progname[128];

/*************************************************************************************/
/*** load entire Greens function library (all depths) - saved in GLIB binary files ***/
/*** in regular 10 fundamental faulting format RSS,RDS,RDD,REX,....                ***/
/*** DepthVector *z; ./include/mt.h                                                ***/
/*************************************************************************************/
/*** also replaces                                                                 ***/
/*** mtinv_subs.c:load_greens() loads the *.ginv files/processed glibs             ***/
/*** added file_type == "ginv" or "glib"                                           ***/
/*************************************************************************************/

Greens **loadGlibAll( Greens **grn, EventInfo *ev, DepthVector *z, int nsta, char *file_type, int verbose )
{
	int iz, ista;
	FILE *fp;
	int debug = 0;
	void printGreensHdr( Greens *grn );
	char filename[256];

/*************************************************************/
/*** begin subroutine ***/
/*************************************************************/

	for( ista = 0; ista < nsta; ista++ )
	{
		if( strcmp( file_type, "glib" ) == 0 ) strcpy( filename, ev[ista].glib_filename );
		if( strcmp( file_type, "ginv" ) == 0 ) strcpy( filename, ev[ista].ginv_filename );

		if( (fp = fopen( filename, "rb" ) ) == NULL )
		{
			fprintf(stderr, "%s: %s: %s: STDERR: Fatal Error, cannot open file %s\n",
				progname, __FILE__, __func__, filename );
			exit(-1);
		}
		fprintf( stderr, "%s: %s: %s: STDERR: reading file %s\n", progname, __FILE__, __func__, filename );
		if(verbose)
                  fprintf( stdout, "%s: %s: %s: STDOUT: reading file %s\n", progname, __FILE__, __func__, filename );

	/***********************************************/
	/*** get the number of depths from glib file ***/
	/***********************************************/

                fread( &(z[ista].nz), sizeof(int), 1, fp );
	
	/***************************************************************/
        /*** alloc mem and get the depth vector(1:nz) from glib file ***/
        /***************************************************************/

                z[ista].z = (float *)calloc( z[ista].nz, sizeof(float) );

                fread( &(z[ista].z[0]), z[ista].nz * sizeof(float), 1, fp );

	/***************************************************************/
	/*** loop over depth and read in the Green's functions       ***/
	/*** allocate space greens-functions[num sta][num depths]    ***/
	/***************************************************************/

		grn[ista] = (Greens *)malloc( z[ista].nz * sizeof(Greens) );

		for( iz = 0; iz < z[ista].nz; iz++ )
		{
			fread( &(grn[ista][iz]), sizeof(Greens), 1, fp );

			if(verbose) printGreensHdr( &(grn[ista][iz]) );

			if(debug)
                        {
                                fprintf( stdout,
"ista=%d %s sta=%s net=%s stla=%g stlo=%g stel=%g evla=%g evlo=%g evdp=%g rdist=%g az=%g baz=%g t0=%g dt=%g twin=%g fmax=%g damp=%g eps=%g smin=%g rigidity=%g redv=%g ts0=%g tstart=%g tend=%g Ptakeoff=%g Pray=%g Ptime=%g Pray=%g kmax=%d nt=%d %s %s nlay=%d maxlay=%d rss=%g\n",
                                ista,
                                grn[ista][iz].filename,
                                grn[ista][iz].stnm,
                                grn[ista][iz].net,
                                grn[ista][iz].stla,
                                grn[ista][iz].stlo,
                                grn[ista][iz].stel,
                                grn[ista][iz].evla,
                                grn[ista][iz].evlo,
                                grn[ista][iz].evdp,
                                grn[ista][iz].rdist,
                                grn[ista][iz].az,
                                grn[ista][iz].baz,
                                grn[ista][iz].t0,
                                grn[ista][iz].dt,
                                grn[ista][iz].twin,
                                grn[ista][iz].fmax,
                                grn[ista][iz].damp,
                                grn[ista][iz].eps,
                                grn[ista][iz].smin,
                                grn[ista][iz].rigidity,
                                grn[ista][iz].redv,
                                grn[ista][iz].ts0,
                                grn[ista][iz].tstart,
                                grn[ista][iz].tend,
                                grn[ista][iz].Ptakeoff,
                                grn[ista][iz].Prayparameter,
                                grn[ista][iz].Pttime,
                                grn[ista][iz].Praybottom,
                                grn[ista][iz].kmax,
                                grn[ista][iz].nt,
                                grn[ista][iz].v.modfile,
                                grn[ista][iz].v.modpath,
                                grn[ista][iz].v.nlay,
                                grn[ista][iz].v.maxlay,
                                grn[ista][iz].g.rss[0] );
                        }

			if( verbose )
			{
                          fprintf( stdout, "%s: %s: %s: STDOUT: iz=%02d z=%3g %-8.8s rdist=%g az=%g ",
                                progname, __FILE__, __func__,
                                iz,
                                z[ista].z[iz],
                                grn[ista][iz].stnm,
                                grn[ista][iz].rdist,
                                grn[ista][iz].az );

			  fprintf( stdout, " evdp=%g t0=%g dt=%g nt=%d %s\n",
				grn[ista][iz].evdp,
				grn[ista][iz].t0,
				grn[ista][iz].dt,
				grn[ista][iz].nt,
				grn[ista][iz].filename );
			}

			if( ev[ista].nt > grn[ista][iz].nt )
			{
                          fprintf( stderr, "%s: %s: %s: STDERR: ERROR nt=%d of othe data greater than nt=%d ",
                                progname, __FILE__, __func__, ev[ista].nt, grn[ista][iz].nt );
                          fprintf( stderr, "of the Green's functions for ista=%d sta=%s.%s\n",
                                ista, ev[ista].stnm, ev[ista].net );
                          exit(-1);
			}

                	if( ev[ista].dt < grn[ista][iz].dt )
			{
                          fprintf( stderr, "%s: %s: %s: STDERR: ERROR dt=%g of the data is less than dt=%g ",
                                progname, __FILE__, __func__, ev[ista].dt, grn[ista][iz].dt );
                          fprintf( stderr, "of the Green's function for ista=%d sta=%s.%s\n",
                                ista, ev[ista].stnm, ev[ista].net );
                          exit(-1);
			}

		/*****************************************************************************************************/
		/*** if this is mtinv or mteig, then set the time shift to the Greens data structure and allocate 
			space for 3-C displacement/velocity particle motions synthetics ***/
		/*****************************************************************************************************/

			if( strcmp( file_type, "ginv" ) == 0 )
			{
				grn[ista][iz].ts0 = ev[ista].ts0;

				grn[ista][iz].tra = (float *)calloc(grn[ista][iz].nt, sizeof(float) );
				grn[ista][iz].rad = (float *)calloc(grn[ista][iz].nt, sizeof(float) );
				grn[ista][iz].ver = (float *)calloc(grn[ista][iz].nt, sizeof(float) );

			}

		} /*** loop over depth - iz ***/

		fclose(fp);

	} /*** loop over stations - ista  ***/

	return (Greens **)grn;
}

/*****************************************************************************************************/
/*** load just one depth from *.glib or *.ginv Greens function library ***/
/*****************************************************************************************************/

Greens *loadGlibZ( char *filename, float my_z, char *wavetype_string, int verbose )
{
        FILE *fp;

        int nz,iz;
        int ista = 0; /***fixed***/
        float *z;
        float ztol = 1.0E-05;
        int ifoundz = -1;
        Greens *grn;

        grn = calloc( 1, sizeof(Greens) );

        if( (fp = fopen( filename, "rb" ) ) == NULL )
        {
                fprintf(stderr, "%s: %s: %s: Fatal Error, cannot open file %s\n",
                        progname, __FILE__, __func__, filename );
                exit(-1);
        }

        fread(&nz,sizeof(int),1,fp);
        z = malloc(nz*sizeof(float));
        fread(&z[0],nz*sizeof(float),1,fp);

        if( verbose )
        {
                fprintf( stdout, "%s: %s: %s: nz=%d ", progname, __FILE__, __func__, nz );
                for(iz=0;iz<nz;iz++)
                        printf(" z[%d]=%g ", iz, z[iz] );
                printf("\n");
        }

        /*** search glib header for needed depth ***/
        for( iz=0; iz<nz; iz++ )
        {
                if( (my_z > z[iz]-ztol) && (my_z < z[iz]+ztol) )
                {
                        ifoundz = iz;
                        if(verbose)
                        {
                          fprintf( stdout, "%s: %s: %s: found iz=%d z=%g\n",
                                progname, __FILE__, __func__, ifoundz, z[ifoundz] );
                        }
                        break;
                }
        }
        if(ifoundz < 0)
        {
                fprintf( stderr, "%s: %s: %s: Fatal ERROR My_z=%g not found in z=",
                        progname, __FILE__, __func__, my_z );
        }

        for( iz=0; iz<nz; iz++ )
        {
                fread(grn,sizeof(Greens),1,fp);
                if(verbose)
                {
                  fprintf( stdout, "%s: %s: %s: iz=%d z=%g rdist=%g az=%g evdp=%g t0=%g dt=%g nt=%d %s\n",
                        progname, __FILE__, __func__,
                        iz, z[iz], grn->rdist, grn->az, grn->evdp, grn->t0,
                        grn->dt, grn->nt, grn->filename );
                }
                if( iz == ifoundz )
                {
                        break;
                }
        }
        return (Greens *)grn;
}


/***********************************************************************************************/
/*** print short header and peak values of Greens function library glib files for inspection ***/
/***********************************************************************************************/

void printGreensHdr( Greens *grn )
{
        int i;
        /* float *rss, *rds, *rdd, *rep, *zss, *zds, *zdd, *zep, *tss, *tds; */
        float *tmp;
        char cmp[][8]={ "RSS", "RDS", "RDD", "REX", "ZSS", "ZDS", "ZDD", "ZEX", "TSS", "TDS" };
        float peak;
        float find_abspeak_value_from_float_array( float *x, int nt );

        fprintf( stdout, "%s: %s: %s: NSL=(%s.%s.%s) Z=%g R=%g Az=%g RedV=%g t0=%g dt=%g nt=%d %s\n",
                progname,
                __FILE__,
                __func__,
                grn->net,
                grn->stnm,
                grn->loc,
                grn->evdp,
                grn->rdist,
                grn->az,
                grn->redv,
                grn->t0,
                grn->dt,
                grn->nt,
                grn->filename );

        for( i=0; i<10; i++ )
        {
                if(i==0)tmp = grn->g.rss;
                if(i==1)tmp = grn->g.rds;
                if(i==2)tmp = grn->g.rdd;
                if(i==3)tmp = grn->g.rep;
                if(i==4)tmp = grn->g.zss;
                if(i==5)tmp = grn->g.zds;
                if(i==6)tmp = grn->g.zdd;
                if(i==7)tmp = grn->g.zep;
                if(i==8)tmp = grn->g.tss;
                if(i==9)tmp = grn->g.tds;

                peak = find_abspeak_value_from_float_array( tmp, grn->nt );

                fprintf( stdout, "%s: %s: %s:\t\t %s abs-peak-amp = %6.3e\n",
                        progname, __FILE__, __func__,
                        cmp[i], peak * (1e+20/base_moment) );
        }
}

/*****************************************************************************************************/
/*** new, plot the greens functions using GMT commands in a csh script, GFs output in seperate dir  ***/
/*****************************************************************************************************/

void plotgrnlib_GMT5( Greens *g, int ista, char *wavetype, int make_output_dirs )
{
	int i;
	int gstart, gend;
	FILE *fp;
	char filename[256];
	char script_filename[12] = {"plotgrn.csh"};
	float yoffset;
	float begin, end;
	int first = 1, line_color_switch = 0;
	char line_color[][8] = { "red", "black" };
	char cmp[][5] = { "rss", "rds", "rdd", "rep",
                          "zss", "zds", "zdd", "zep",
                          "tss", "tds",
                          "w1ss", "w1ds", "w1dd", "w1ex",
                          "w2ss", "w2ds", "w2dd", "w2ex",
                          "w3ss", "w3ds", "w3dd", "w3ex" }; 
	char title_string[512];
	float peak;
	float find_abspeak_value_from_float_array( float *x, int nt );

/*** mkdirp ***/
        int mkdirp2( const char *, mode_t mode );
        char outDirectory[64];

/*************************/
/*** begin subroutine ***/
/*************************/

/*** option make_output_dirs assumes Greens is either already or will be written here, more neater to do this so files not all cluttered ***/

	if( make_output_dirs )
	{
		snprintf( outDirectory, 8, "%s", g->stnm );
		mkdirp2( outDirectory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
		chdir( outDirectory );

		fprintf( stderr, "%s: %s: %s: make_output_dirs=%d Writting output to directory outDirectory=%s\n",
                        progname, __FILE__, __func__, make_output_dirs, outDirectory );
	}

/*** get the start and stop pointers from glib file depending on wavetype ***/
	if( strcmp( wavetype, "Surf/Pnl" ) == 0 ) 
	{
		gstart = 0; 
		gend = 10;
	}
        if( strcmp( wavetype, "Rotational" ) == 0 ) 
	{ 
		gstart = 10;
		gend = 22;
	}

/*** open the GMT cshell script ***/

	if( (fp = fopen(script_filename, "w")) == NULL )
	{
		fprintf( stderr, "%s: %s: %s: cannot open file %s\n", 
			progname, __FILE__, __func__, script_filename );
		exit(-1);
	}
	chmod( script_filename, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );

	fprintf( fp, "#!/bin/csh\n" );
	fprintf( fp, "############################################################################\n" );
	fprintf( fp, "## This C-shell script was automatically generated                       ###\n" );
        fprintf( fp, "## and requires Generic Mapping Tools (GMT) http://gmt.soest.hawaii.edu/ ###\n" );
        fprintf( fp, "## SCRIPT ONLY WORKS WITH GMT Version 5.x.x                              ###\n" );
        fprintf( fp, "############################################################################\n" );
        fprintf( fp, "\n" );

	fprintf( fp, "gmt set PS_PAGE_ORIENTATION        portrait  \n" );
        fprintf( fp, "gmt set MAP_ANNOT_OFFSET_PRIMARY   2p         \n" );
        fprintf( fp, "gmt set MAP_ANNOT_OFFSET_SECONDARY 2p      \n" );
        fprintf( fp, "gmt set MAP_LABEL_OFFSET           0p       \n" );
        fprintf( fp, "gmt set PS_MEDIA                   letter  \n" );
        fprintf( fp, "gmt set FONT_ANNOT_PRIMARY          9p,Helvetica,black   \n" ); 
        fprintf( fp, "gmt set FONT_ANNOT_SECONDARY        9p,Helvetica,black  \n" );
        fprintf( fp, "gmt set FONT_LABEL                  9p,Palatino-Bold,black \n" );
        fprintf( fp, "gmt set FONT_LOGO                   9p,Helvetica,black     \n" );
        fprintf( fp, "gmt set FONT_TITLE                 12p,Times-Bold,black \n" );
	fprintf( fp, "\n" );

	/* fprintf( fp, "set R=\"-R0/100/-1/1\" \n" ); */
	begin = g->t0;
	end   = g->t0 + (g->nt * g->dt);

	fprintf( fp, "set R=\"-R%g/%g/-1/+1\" \n", begin, end );

	fprintf( fp, "set J=\"-JX6i/0.8i\" \n" );

	if( strcmp( wavetype, "Surf/Pnl" ) == 0 )
	{
	 fprintf( fp, "set  PS=%s.%s.%s.%s.%g.plotgrn.ps\n", g->net, g->stnm, g->loc, g->v.modfile, g->evdp );
	 fprintf( fp, "set JPG=%s.%s.%s.%s.%g.plotgrn.jpg\n", g->net, g->stnm, g->loc, g->v.modfile, g->evdp );
	}
	if( strcmp( wavetype, "Rotational" ) == 0 )
	{
	 fprintf( fp, "set  PS=%s.%s.%s.%s.%g.plotgrn.rot.ps\n", g->net, g->stnm, g->loc, g->v.modfile, g->evdp );
         fprintf( fp, "set JPG=%s.%s.%s.%s.%g.plotgrn.rot.jpg\n", g->net, g->stnm, g->loc, g->v.modfile, g->evdp );
	}

	fprintf( fp, "\n" );

	fprintf( fp, "gmt psbasemap $R $J -Bxf1a10+l\"seconds\" -Byf1a1 -BS -P -K >! ${PS}\n" );

        for( i=gstart; i<gend; i++ )
        {
		snprintf( filename, 256, "%s.%03d.%s.%g.grns", g->filename, ista, cmp[i], g->evdp );
		if( first == 1 ) {
			first = 0;
			yoffset = 0.0;
		}else{
			yoffset = 0.75;
		}
		if( (i%2) == 0 )
			line_color_switch = 0;
		else
			line_color_switch = 1;

		if( strcmp( wavetype, "Surf/Pnl" ) == 0 )
		{
		 if( i==0 ) peak = find_abspeak_value_from_float_array( &(g->g.rss[0]), g->nt );
                 if( i==1 ) peak = find_abspeak_value_from_float_array( &(g->g.rds[0]), g->nt );
                 if( i==2 ) peak = find_abspeak_value_from_float_array( &(g->g.rdd[0]), g->nt );
                 if( i==3 ) peak = find_abspeak_value_from_float_array( &(g->g.rep[0]), g->nt );
                 if( i==4 ) peak = find_abspeak_value_from_float_array( &(g->g.zss[0]), g->nt );
                 if( i==5 ) peak = find_abspeak_value_from_float_array( &(g->g.zds[0]), g->nt );
                 if( i==6 ) peak = find_abspeak_value_from_float_array( &(g->g.zdd[0]), g->nt );
                 if( i==7 ) peak = find_abspeak_value_from_float_array( &(g->g.zep[0]), g->nt );
                 if( i==8 ) peak = find_abspeak_value_from_float_array( &(g->g.tss[0]), g->nt );
                 if( i==9 ) peak = find_abspeak_value_from_float_array( &(g->g.tds[0]), g->nt );

		 fprintf( fp, "sac2xy norm < %s | gmt psxy $R $J -W0.5p,%-8s -Y%gi -K -O >> ${PS}\n",
                        filename, line_color[line_color_switch], yoffset );

                fprintf( fp, "echo %g 0 %s | gmt pstext $R $J -F+f11p,Helvetica-Bold,black+jMR -N -D0i/0.0i -O -K >> ${PS}\n",
                        begin, cmp[i] );

                fprintf( fp, "echo %g 0 %5.2e | gmt pstext $R $J -F+f11p,Helvetica-Bold,black+jML -N -D-0.50i/0.1i -O -K >> ${PS}\n",
                        end, peak );
		}

		if( strcmp( wavetype, "Rotational" ) == 0 )
		{
		  if( i==10 ) peak = find_abspeak_value_from_float_array( &(g->g.w1ss[0]), g->nt );
                  if( i==11 ) peak = find_abspeak_value_from_float_array( &(g->g.w1ds[0]), g->nt );
                  if( i==12 ) peak = find_abspeak_value_from_float_array( &(g->g.w1dd[0]), g->nt );
                  if( i==13 ) peak = find_abspeak_value_from_float_array( &(g->g.w1ex[0]), g->nt );

                  if( i==14 ) peak = find_abspeak_value_from_float_array( &(g->g.w2ss[0]), g->nt );
                  if( i==15 ) peak = find_abspeak_value_from_float_array( &(g->g.w2ds[0]), g->nt );
                  if( i==16 ) peak = find_abspeak_value_from_float_array( &(g->g.w2dd[0]), g->nt );
                  if( i==17 ) peak = find_abspeak_value_from_float_array( &(g->g.w2ex[0]), g->nt );

                  if( i==18 ) peak = find_abspeak_value_from_float_array( &(g->g.w3ss[0]), g->nt );
                  if( i==19 ) peak = find_abspeak_value_from_float_array( &(g->g.w3ds[0]), g->nt );
                  if( i==20 ) peak = find_abspeak_value_from_float_array( &(g->g.w3dd[0]), g->nt );
                  if( i==21 ) peak = find_abspeak_value_from_float_array( &(g->g.w3ex[0]), g->nt );

		fprintf( fp, "sac2xy norm < %s | gmt psxy $R $J -W0.5p,%-8s -Y%gi -K -O >> ${PS}\n",
			filename, line_color[line_color_switch], yoffset );

		fprintf( fp, "echo %g 0 %s | gmt pstext $R $J -F+f11p,Helvetica-Bold,black+jMR -N -D0i/0.0i -O -K >> ${PS}\n",
			begin, cmp[i] );

		fprintf( fp, "echo %g 0 %5.2e | gmt pstext $R $J -F+f11p,Helvetica-Bold,black+jML -N -D-0.50i/0.1i -O -K >> ${PS}\n",
			end, peak );
		}

		fprintf( fp, "\n" );
	}
	sprintf( title_string, "Greens functions %s net.sta.loc= %s.%s.%s model= %s depth= %g",
		wavetype, g->net, g->stnm, g->loc, g->v.modfile, g->evdp  );

	fprintf( fp, "gmt psbasemap $R $J -Bxf1a10+l\"seconds\" -Byf1a1 -BN+t\"%s\" -O  >> ${PS}\n", title_string );

	fprintf( fp, "gmt psconvert -A -Tj -E300 ${PS}\n" );

	fprintf( fp, "### cleanup and sys dep plotting\n" );
	fprintf( fp, "/bin/rm -f ${PS}\n" );
	fprintf( fp, "# open ${JPG}\n" );
	fclose(fp);

	system( "plotgrn.csh" );
	if( make_output_dirs ) chdir( ".." );
}	


/*****************************************************************************************************/
/*** write out Greens functions as inidividual SAC files ****/
/*****************************************************************************************************/

void wrtgrn2sac( Greens *g, int ista, char *wavetype, char *fmt, int make_output_dirs )
{
	/* int ngreen = 10 */
	int ngreen = 22;
        Sac_Header sp;
        FILE *fp;
        char filename[256];

	void sac_minmax( float *x, int n, float *max, float *min, float *mean );
	
	/***prevents the writting of empty files ***/
	void  write_it( Sac_Header *sp, float *x, int nt, char *filename );

 /*** old ***/
 /* char cmp[][3] = { "rss", "rds", "rdd", "rep", "zss", "zds", "zdd", "zep", "tss", "tds" }; */

/**** new ***/
/*** mtinv.v4.0.0 -  Green's function now include 3C rotational motions w1,w2,w3 ***/
	char cmp[][5] = { "rss", "rds", "rdd", "rep", 
                          "zss", "zds", "zdd", "zep", 
                          "tss", "tds", 
                          "w1ss", "w1ds", "w1dd", "w1ex", 
                          "w2ss", "w2ds", "w2dd", "w2ex", 
                          "w3ss", "w3ds", "w3dd", "w3ex" };
        int i;
	int verbose = 0;

/*** misc_tools/ampshift.c ***/

	int remove_offset = 0;

        void  ampshift( float *x, int n, int verbose );

/*** mkdirp ***/
	int mkdirp2( const char *, mode_t mode );
	char outDirectory[64];

/******************************************************/
/*** for shallow sources, this shifts the amplitude ***/
/*** of the first sample to zero, and mitigates the ***/
/*** artifact of the step offset sometimes from Grn ***/
/*** functions from shallow sources, before filter  ***/
/******************************************************/

/*** attempt to make the directory if it doesn't exist already ***/

	if( make_output_dirs )
	{
		snprintf( outDirectory, 8, "%s", g->stnm );
		mkdirp2( outDirectory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
		chdir( outDirectory );

		fprintf( stderr, "%s: %s: %s: make_output_dirs=%d Writting output to directory outDirectory=%s\n",
                        progname, __FILE__, __func__, make_output_dirs, outDirectory );
	}

	if(verbose) 
	{ 
		fprintf( stdout, "%s: %s: %s: ista=%d sta=%s wavetype=%s\n",
                        progname, __FILE__, __func__, ista, g->stnm, wavetype );
	}

	if( remove_offset )
	{
		ampshift( &(g->g.rss[0]), g->nt, verbose );
		ampshift( &(g->g.rds[0]), g->nt, verbose );
		ampshift( &(g->g.rdd[0]), g->nt, verbose );
		ampshift( &(g->g.rep[0]), g->nt, verbose );

		ampshift( &(g->g.zss[0]), g->nt, verbose );
		ampshift( &(g->g.zds[0]), g->nt, verbose );
		ampshift( &(g->g.zdd[0]), g->nt, verbose );
		ampshift( &(g->g.zep[0]), g->nt, verbose );

		ampshift( &(g->g.tss[0]), g->nt, verbose );
		ampshift( &(g->g.tds[0]), g->nt, verbose );

		if( strcmp( wavetype, "Rotational" ) == 0 )
		{
		 ampshift( &(g->g.w1ss[0]), g->nt, verbose );
		 ampshift( &(g->g.w1ds[0]), g->nt, verbose );
		 ampshift( &(g->g.w1dd[0]), g->nt, verbose );
		 ampshift( &(g->g.w1ex[0]), g->nt, verbose );

		 ampshift( &(g->g.w2ss[0]), g->nt, verbose );
		 ampshift( &(g->g.w2ds[0]), g->nt, verbose );
		 ampshift( &(g->g.w2dd[0]), g->nt, verbose );
		 ampshift( &(g->g.w2ex[0]), g->nt, verbose );

		 ampshift( &(g->g.w3ss[0]), g->nt, verbose );
		 ampshift( &(g->g.w3ds[0]), g->nt, verbose );
		 ampshift( &(g->g.w3dd[0]), g->nt, verbose );
		 ampshift( &(g->g.w3ex[0]), g->nt, verbose );
		}
	}

	if( strcmp( wavetype, "Surf/Pnl" ) == 0 ) ngreen = 10;
	if( strcmp( wavetype, "Rotational" ) == 0 ) ngreen = 22;

        for( i=0; i<ngreen; i++ )
        {
		if( strcmp(fmt,"\0") == 0 ) 
                  snprintf( filename, 256, 
			"%s.%03d.%s.%g.grns", 	
			g->filename, ista, cmp[i], g->evdp );
		else
		  snprintf( filename, 256,
			"%s.%s.%g.grns",
			fmt, cmp[i], g->evdp );

                sp              = sac_null;
                sp.b            = g->t0;
                sp.delta        = g->dt;
                sp.e            = g->t0 + (g->nt * g->dt);
                sp.stla         = g->stla;
                sp.stlo         = g->stlo;
                sp.evla         = g->evla;
                sp.evlo         = g->evlo;
                sp.evdp         = g->evdp;

                sp.nvhdr = 6;
                sp.norid = 0;
                sp.nevid = 0;
                sp.iftype = ITIME; /* data type: IRLIM spec re im | IAMPH amp ph | IXY general x,y  */
                sp.idep   = IUNKN; /* not disp vel acc or volts */
                sp.iztype = IB;    /* types: IUNKN,IB,IDAY,IO,IA,ITn  */
                sp.ievtyp = IUNKN; /* type of event IQUAKE - earthquake */
                sp.npts = g->nt;

                sp.leven  = TRUE;  /* is data evenly sampled  */
                sp.lpspol = FALSE; /* LLL sets it */
                sp.lcalda = TRUE;  /* should az,baz,gcarc,dist be calculated?  */
                sp.lhdr5  = FALSE; /* LLL sets it */

		sprintf( sp.knetwk, "%s", g->net );
		sprintf( sp.kstnm,  "%s", g->stnm );
		sprintf( sp.khole,  "%s", g->loc );
                sprintf( sp.kcmpnm, "%s", cmp[i] );

		if( strcmp( wavetype, "Surf/Pnl" ) == 0 )
		{
		  if( i==0 ) sac_minmax( &(g->g.rss[0]), g->nt, &(sp.depmax), &(sp.depmin), &(sp.depmen) ); 
                  if( i==1 ) sac_minmax( &(g->g.rds[0]), g->nt, &(sp.depmax), &(sp.depmin), &(sp.depmen) ); 
                  if( i==2 ) sac_minmax( &(g->g.rdd[0]), g->nt, &(sp.depmax), &(sp.depmin), &(sp.depmen) ); 
                  if( i==3 ) sac_minmax( &(g->g.rep[0]), g->nt, &(sp.depmax), &(sp.depmin), &(sp.depmen) ); 
                  if( i==4 ) sac_minmax( &(g->g.zss[0]), g->nt, &(sp.depmax), &(sp.depmin), &(sp.depmen) ); 
                  if( i==5 ) sac_minmax( &(g->g.zds[0]), g->nt, &(sp.depmax), &(sp.depmin), &(sp.depmen) ); 
                  if( i==6 ) sac_minmax( &(g->g.zdd[0]), g->nt, &(sp.depmax), &(sp.depmin), &(sp.depmen) ); 
                  if( i==7 ) sac_minmax( &(g->g.zep[0]), g->nt, &(sp.depmax), &(sp.depmin), &(sp.depmen) ); 
                  if( i==8 ) sac_minmax( &(g->g.tss[0]), g->nt, &(sp.depmax), &(sp.depmin), &(sp.depmen) ); 
                  if( i==9 ) sac_minmax( &(g->g.tds[0]), g->nt, &(sp.depmax), &(sp.depmin), &(sp.depmen) ); 
	
                  if( i==0 ) write_it( &sp, &(g->g.rss[0]), g->nt, filename );
                  if( i==1 ) write_it( &sp, &(g->g.rds[0]), g->nt, filename );
                  if( i==2 ) write_it( &sp, &(g->g.rdd[0]), g->nt, filename );
                  if( i==3 ) write_it( &sp, &(g->g.rep[0]), g->nt, filename );

                  if( i==4 ) write_it( &sp, &(g->g.zss[0]), g->nt, filename );
                  if( i==5 ) write_it( &sp, &(g->g.zds[0]), g->nt, filename );
                  if( i==6 ) write_it( &sp, &(g->g.zdd[0]), g->nt, filename );
                  if( i==7 ) write_it( &sp, &(g->g.zep[0]), g->nt, filename );

                  if( i==8 ) write_it( &sp, &(g->g.tss[0]), g->nt, filename );
                  if( i==9 ) write_it( &sp, &(g->g.tds[0]), g->nt, filename );
		}
		else if( strcmp( wavetype, "Rotational" ) == 0 )
		{
		  if( i==10 ) write_it( &sp, &(g->g.w1ss[0]), g->nt, filename );
                  if( i==11 ) write_it( &sp, &(g->g.w1ds[0]), g->nt, filename );
                  if( i==12 ) write_it( &sp, &(g->g.w1dd[0]), g->nt, filename );
                  if( i==13 ) write_it( &sp, &(g->g.w1ex[0]), g->nt, filename );

                  if( i==14 ) write_it( &sp, &(g->g.w2ss[0]), g->nt, filename );
                  if( i==15 ) write_it( &sp, &(g->g.w2ds[0]), g->nt, filename );
                  if( i==16 ) write_it( &sp, &(g->g.w2dd[0]), g->nt, filename );
                  if( i==17 ) write_it( &sp, &(g->g.w2ex[0]), g->nt, filename );

                  if( i==18 ) write_it( &sp, &(g->g.w3ss[0]), g->nt, filename );
                  if( i==19 ) write_it( &sp, &(g->g.w3ds[0]), g->nt, filename );
		  if( i==20 ) write_it( &sp, &(g->g.w3dd[0]), g->nt, filename );
                  if( i==21 ) write_it( &sp, &(g->g.w3ex[0]), g->nt, filename );

		} /**** if rotational wavetype ***/

        } /*** loop over i-th greens function ***/

	if( make_output_dirs ) chdir( ".." );

} /*** end of wrtgrn2sac.c ***/


/*****************************************************************************************************/
/**** subroutine needed by wrtgrn2sac()  ****/
/*****************************************************************************************************/

void  write_it( Sac_Header *sp, float *x, int nt, char *filename )
{
	FILE *fp;
	void sac_minmax( float *x, int n, float *max, float *min, float *mean );

	if( (fp = fopen(filename,"wb")) == NULL )
	{
		fprintf( stderr, "%s: %s: %s: ERROR! cannot open output file %s\n", 
			progname, __FILE__, __func__, filename );
		exit(-1);
	}

	sac_minmax( x, nt, &(sp->depmax), &(sp->depmin), &(sp->depmen) );

	fwrite( sp, sizeof(Sac_Header), 1, fp );
	fwrite( x, nt * sizeof(float), 1, fp );
	fclose(fp);
}

/*****************************************************************************************************/
/*** used here and in grnlib2sac.c:grn2disp2(), grnlib2sac.c:plotgrnlib_GMT5(),  and Greens_subs.c:printGreensHdr() ***/
/*****************************************************************************************************/

float find_abspeak_value_from_float_array( float *x, int nt )
{
        int i; 
        float peak;
        peak = 1.0E-29;
        for( i = 0; i < nt; i++ )
        {
                if( fabs(x[i]) > peak )
                        peak = fabs(x[i]);
        }
        return peak;
}

/*****************************************************************************************************/
/*** used below in Greens_subs.c: getGreensFuctionMinMax() - returns min,max and absolute peak values ***/
/*****************************************************************************************************/

void getVectorStats( float *x, int nt, float *min, float *max, float *abspeak )
{
        int i;
        float local_abspeak, local_min, local_max;

        local_abspeak = x[0];
        local_min = x[0];
        local_max = x[0];

        for( i = 0; i < nt; i++ )
        {
                if( x[i] > local_max ) local_max = x[i];
                if( x[i] < local_min ) local_min = x[i];
                if( fabs( x[i] ) > local_abspeak ) local_abspeak = fabs(x[i]);
        }
	*min = local_min;
	*max = local_max;
	*abspeak = local_abspeak;
}

/*****************************************************************************************************/
/*** this helps determine how to normalize each GF trace for plotting based on global peak values ***/
/*****************************************************************************************************/

void getGreensFuctionMinMax( int icmp, Greens *g1, Greens *g2, float *global_min, float *global_max, float *abspeak1, float *abspeak2 )
{
	float min,max; /**local vars**/
	void getVectorStats( float *x, int nt, float *min, float *max, float *abspeak );
	void checkit( float min, float max, float *global_min, float *global_max );

	*global_min = g1->g.rss[0];
	*global_max = g1->g.rss[0];

	if( icmp == 0 ) {
		getVectorStats( &(g1->g.rss[0]), g1->nt, &min, &max, abspeak1 ); checkit( min, max, global_min, global_max );
		getVectorStats( &(g2->g.rss[0]), g2->nt, &min, &max, abspeak2 ); checkit( min, max, global_min, global_max );

		/* fprintf( stdout, "%s: %s: %s: RSS icmp=%d min=%g max=%g abspeak1=%g abspeak2=%g\n",
			progname, __FILE__, __func__, icmp, min, max, *abspeak1, *abspeak2 ); */
	}
	else if( icmp == 1 ) {
		getVectorStats( &(g1->g.rds[0]), g1->nt, &min, &max, abspeak1 ); checkit( min, max, global_min, global_max );
		getVectorStats( &(g2->g.rds[0]), g2->nt, &min, &max, abspeak2 ); checkit( min, max, global_min, global_max );
	}
	else if( icmp == 2 ) {
		getVectorStats( &(g1->g.rdd[0]), g1->nt, &min, &max, abspeak1 ); checkit( min, max, global_min, global_max );
		getVectorStats( &(g2->g.rdd[0]), g2->nt, &min, &max, abspeak2 ); checkit( min, max, global_min, global_max );
	}
	else if( icmp == 3 ) {
		getVectorStats( &(g1->g.rep[0]), g1->nt, &min, &max, abspeak1 ); checkit( min, max, global_min, global_max );
		getVectorStats( &(g2->g.rep[0]), g2->nt, &min, &max, abspeak2 ); checkit( min, max, global_min, global_max );
	}
	else if( icmp == 4 ) {
		getVectorStats( &(g1->g.zss[0]), g1->nt, &min, &max, abspeak1 ); checkit( min, max, global_min, global_max );
		getVectorStats( &(g2->g.zss[0]), g2->nt, &min, &max, abspeak2 ); checkit( min, max, global_min, global_max );
	}
	else if( icmp == 5 ) {
		getVectorStats( &(g1->g.zds[0]), g1->nt, &min, &max, abspeak1 ); checkit( min, max, global_min, global_max );
		getVectorStats( &(g2->g.zds[0]), g2->nt, &min, &max, abspeak2 ); checkit( min, max, global_min, global_max );
	}
	else if( icmp == 6 ) {
		getVectorStats( &(g1->g.zdd[0]), g1->nt, &min, &max, abspeak1 ); checkit( min, max, global_min, global_max );
		getVectorStats( &(g2->g.zdd[0]), g2->nt, &min, &max, abspeak2 ); checkit( min, max, global_min, global_max );
	}
	else if( icmp == 7 ) {
		getVectorStats( &(g1->g.zep[0]), g1->nt, &min, &max, abspeak1 ); checkit( min, max, global_min, global_max );
		getVectorStats( &(g2->g.zep[0]), g2->nt, &min, &max, abspeak2 ); checkit( min, max, global_min, global_max );
	}	
	else if( icmp == 8 ) {
		getVectorStats( &(g1->g.tss[0]), g1->nt, &min, &max, abspeak1 ); checkit( min, max, global_min, global_max );
		getVectorStats( &(g2->g.tss[0]), g2->nt, &min, &max, abspeak2 ); checkit( min, max, global_min, global_max );
	}
	else if( icmp == 9 ) {
		getVectorStats( &(g1->g.tds[0]), g1->nt, &min, &max, abspeak1 ); checkit( min, max, global_min, global_max );
		getVectorStats( &(g2->g.tds[0]), g2->nt, &min, &max, abspeak2 ); checkit( min, max, global_min, global_max );
	}
	else { 
		fprintf(stderr, "%s: %s: %s: Error unknown icmp=%d\n", progname, __FILE__, __func__, icmp );
		exit(-1); 
	}
}

/*****************************************************************************************************/
/*** short macro for above function, updates global min,max ***/
/*****************************************************************************************************/

void checkit( float min, float max, float *global_min, float *global_max )
{
	if( min < *global_min ) *global_min = min;
	if( max > *global_max ) *global_max = max;
}
