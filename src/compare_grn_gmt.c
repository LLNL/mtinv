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
compare_grn_gmt.c: 
creates GMT script to compare Greens functions from two *.glib files
a function of depth z=1,2, components zss,zds,...,tds 
/Users/ichinose1/Work/mtinv.v4.0.1/src
Thu Feb 29 08:26:28 PST 2024
g.ichinose
****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>

#include <sys/stat.h>
#include <errno.h>
#include "../include/nrutil.h"
#include "../include/mt.h"

char progname[128];

int main( int ac, char **av )
{
	Greens *grn1, *grn2;
	Greens *loadGlibZ( char *filename, float my_z, char *wavetype_string, int verbose );
	char filename1[256], filename2[256];
	void printGreensHdr( Greens *grn );
/*** wavetype_string is populated with either "Surf/Pnl" or "Rotational" ***/
	char wavetype_string[32];
	float my_z;

	int verbose = 0;
	int ista = 0;
	int make_output_dirs = 0;
	float tstart = -999, tstop = -999;  /*** user defined start and stop plotting ***/

	void plot_compare_grnlib_GMT5( Greens *g1, Greens *g2, int ista, char *wavetype_string,
                 int make_output_dirs, float tstart, float tstop, char *filename1, char *filename2, 
		 float lf, float hf, int clean_out_tmp_files );


/*** misc see ../lib/libget.a ***/
        int setpar( int ac, char **av );
        int mstpar( char *, char *, void * );
        int getpar( char *, char *, void * );
        void endpar();

/*** processing filtering ***/
	int bpfilter = 1;
	void bandpassFilterGreens( Greens *grn, float lf, float hf, int npole, int npass );
	int npole = 3, npass = 2;
	float lf = 0.01, hf = 0.2;

	/*** filter/filtersubs.c ***/           
        void iir_filter( float *data,           
                        int nsamps,
                        char *filter_type,
                        float trbndw,
                        float a,
                        int iord,
                        char *operation_type,
                        float flo,
                        float fhi,      
                        float ts,       
                        int passes );  

	int clean_out_tmp_files = 1; /*** True remove *.grn files ***/

	void Usage(int ac, char **av);

/**********************/
/**** begin program ***/
/**********************/
        strcpy( progname, av[0] );
	sprintf( wavetype_string, "Surf/Pnl" );
        fprintf( stderr, "%s: Version=%s Release Date=%s\n", progname, Version_Label, Version_Date );
        fprintf( stdout, "%s: Version=%s Release Date=%s\n", progname, Version_Label, Version_Date );

	if ( ac < 4 ) Usage(ac,av);

	setpar(ac,av);
	mstpar("glib1",    "s", filename1 );
	mstpar("glib2",    "s", filename2 );
	mstpar("z",       "f", &my_z );
        getpar("verbose", "b", &verbose );
	getpar( "tstart",  "f",   &tstart );
	getpar( "tstop",   "f",   &tstop );

	getpar( "bpfilter", "b", &bpfilter );
	if(bpfilter)
	{
		getpar( "lf", "f", &lf );
		getpar( "hf", "f", &hf );
		getpar( "npass", "d", &npass );
		getpar( "npole", "d", &npole );
	}
	getpar( "clean", "b", &clean_out_tmp_files );
	endpar();

	grn1 = calloc(1,sizeof(Greens));
	grn1 = loadGlibZ( filename1, my_z, wavetype_string, verbose );
	if(verbose) printGreensHdr(grn1);

	grn2 = calloc(1,sizeof(Greens));
	grn2 = loadGlibZ( filename2, my_z, wavetype_string, verbose );
	if(verbose) printGreensHdr(grn2);

	if(bpfilter)
	{
		bandpassFilterGreens( grn1, lf, hf, npole, npass );
		bandpassFilterGreens( grn2, lf, hf, npole, npass );
	}

	plot_compare_grnlib_GMT5( grn1, grn2, ista, wavetype_string, make_output_dirs, 
			tstart, tstop, filename1, filename2, lf, hf, clean_out_tmp_files );

  /***********************/
} /*** end of main()   ***/
  /***********************/

void bandpassFilterGreens( Greens *grn, float lf, float hf, int npole, int npass )
{
        char filter_type[] = { "BU" };
        float a = 1, trbndw = 1;
        char operation_type[] = { "BP" };
 	float dt;
	int nt;
       
/*** filter/filtersubs.c ***/
        void iir_filter( float *data,
                        int nsamps,
                        char *filter_type,
                        float trbndw,
                        float a,
                        int iord,
                        char *operation_type,
                        float flo,
                        float fhi,
                        float ts,
                        int passes );
	dt = grn->dt;	
	nt = grn->nt;

	iir_filter( grn->g.rss, nt, filter_type, trbndw, a, npole, operation_type, lf, hf, dt, npass );
	iir_filter( grn->g.rds, nt, filter_type, trbndw, a, npole, operation_type, lf, hf, dt, npass );  
	iir_filter( grn->g.rdd, nt, filter_type, trbndw, a, npole, operation_type, lf, hf, dt, npass );  
	iir_filter( grn->g.rep, nt, filter_type, trbndw, a, npole, operation_type, lf, hf, dt, npass );  
	iir_filter( grn->g.zss, nt, filter_type, trbndw, a, npole, operation_type, lf, hf, dt, npass );  
	iir_filter( grn->g.zds, nt, filter_type, trbndw, a, npole, operation_type, lf, hf, dt, npass );  
	iir_filter( grn->g.zdd, nt, filter_type, trbndw, a, npole, operation_type, lf, hf, dt, npass );  
	iir_filter( grn->g.zep, nt, filter_type, trbndw, a, npole, operation_type, lf, hf, dt, npass );  
	iir_filter( grn->g.tss, nt, filter_type, trbndw, a, npole, operation_type, lf, hf, dt, npass );  
	iir_filter( grn->g.tds, nt, filter_type, trbndw, a, npole, operation_type, lf, hf, dt, npass );  
}


/**** allow user to set the start and stop of time axes range ****/

void plot_compare_grnlib_GMT5( Greens *g1, Greens *g2, int ista, char *wavetype_string, 
	int make_output_dirs, float tstart, float tstop, 
	char *glib_filename1, char *glib_filename2, float lf, float hf, int clean_out_tmp_files )
{
	int i;
	int gstart, gend;
	FILE *fp;
	char filename1[256], filename2[256];
	char script_filename[12] = {"plotgrn.csh"};
	float yoffset;
	float begin, end;
	int first = 1, line_color_switch = 0;
	char line_color[][8] = { "red", "black", "blue", "green" };

	char cmp[][5] = { "rss", "rds", "rdd", "rep",
                          "zss", "zds", "zdd", "zep",
                          "tss", "tds",
                          "w1ss", "w1ds", "w1dd", "w1ex",
                          "w2ss", "w2ds", "w2dd", "w2ex",
                          "w3ss", "w3ds", "w3dd", "w3ex" };

        char title_string[512];

	char *string_toupper(char *temp);

        float min, max, peak1, peak2, ratio;

	void getGreensFuctionMinMax( int icmp, Greens *g1, Greens *g2, float *global_min, float *global_max, float *abspeak1, float *abspeak2 );

	void wrtgrn2sac( Greens *g, int ista, char *wavetype, char *fmt, int make_output_dirs );

/*** mkdirp ***/
        int mkdirp2( const char *, mode_t mode );
        char outDirectory[64];

	float MoRatio;
	MoRatio = 1.0E+20 / base_moment;

/***************************************************************************************************************/
/*** check the GMT version for -B+s"subtitle" compatibility (GMT version 6.2.0+)                             ***/
/*** https://docs.generic-mapping-tools.org/latest/gmt.html#b-full                                           ***/
/*** -B[axes][+b][+gfill][+i[val]][+n][+olon/lat][+ssubtitle][+ttitle][+w[pen]][+xfill][+yfill][+zfill]      ***/
/***************************************************************************************************************/
	char subtitle_string[512];
	int check_gmt_version( int mymajor, int myminor );

/*************************/
/*** begin subroutine ***/
/*************************/

/*** option make_output_dirs assumes Greens is either already or will be written here, more neater to do this so files not all cluttered ***/

        if( make_output_dirs )
        {
                snprintf( outDirectory, 8, "%s", g1->stnm );
                mkdirp2( outDirectory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
                chdir( outDirectory );

                fprintf( stderr, "%s: %s: %s: make_output_dirs=%d Writting output to directory outDirectory=%s\n",
                        progname, __FILE__, __func__, make_output_dirs, outDirectory );
        }

/*** get the start and stop pointers from glib file depending on wavetype ***/
/*** the number of greens functions ***/
        if( strcmp( wavetype_string, "Surf/Pnl" ) == 0 )
        {
                gstart = 0;
                gend = 10;
        }
        if( strcmp( wavetype_string, "Rotational" ) == 0 )
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
        fprintf( fp, "gmt set PS_MEDIA                   letter  \n" );
        fprintf( fp, "gmt set FONT_ANNOT_PRIMARY          9p,Helvetica,black   \n" );
        fprintf( fp, "gmt set FONT_ANNOT_SECONDARY        9p,Helvetica,black  \n" );
        fprintf( fp, "gmt set FONT_LABEL                  9p,Palatino-Bold,black \n" );
        fprintf( fp, "gmt set FONT_LOGO                   9p,Helvetica,black     \n" );
	fprintf( fp, "gmt set FONT_TITLE                 14p,Times-Bold,black \n" );

	fprintf( fp, "gmt set MAP_TITLE_OFFSET 22p \n" ); 
	fprintf( fp, "gmt set MAP_LABEL_OFFSET 10p \n" );
	fprintf( fp, "gmt set MAP_HEADING_OFFSET 18p \n" );

	/**** subtitle new psbasemap -B+s feature of GMT version 6.2+ not compatible with GMT version 6.1 ***/
	if( check_gmt_version( 6, 2 ) )
	  fprintf( fp, "gmt set FONT_SUBTITLE              12p,Palatino-Bold,black \n" );

        fprintf( fp, "\n" );


        begin = g1->t0;
        end   = g1->t0 + (g1->nt * g1->dt);

	/***user defined start and stop ***/
        if( tstart != -999 )begin = tstart;
        if(  tstop != -999 ) end = tstop;

        /* fprintf( stdout, "tstart=%g tstop=%g begin=%g end=%g\n",
                tstart, tstop, begin, end ); */

	fprintf( fp, "set R=\"-R%g/%g/-1/+1\" \n", begin, end );
	fprintf( fp, "set J=\"-JX6i/0.8i\" \n" );

	if( strcmp( wavetype_string, "Surf/Pnl" ) == 0 )
        {
         fprintf( fp, "set  PS=%s.%s.%s.%s.%g.plotgrn.ps\n", g1->net, g1->stnm, g1->loc, g1->v.modfile, g1->evdp );
         fprintf( fp, "set JPG=%s.%s.%s.%s.%g.plotgrn.jpg\n", g1->net, g1->stnm, g1->loc, g1->v.modfile, g1->evdp );
        }
        if( strcmp( wavetype_string, "Rotational" ) == 0 )
        {
         fprintf( fp, "set  PS=%s.%s.%s.%s.%g.plotgrn.rot.ps\n", g1->net, g1->stnm, g1->loc, g1->v.modfile, g1->evdp );
         fprintf( fp, "set JPG=%s.%s.%s.%s.%g.plotgrn.rot.jpg\n", g1->net, g1->stnm, g1->loc, g1->v.modfile, g1->evdp );
        }

	fprintf( fp, "\n" );

	fprintf( fp, "gmt psbasemap $R $J -Bxf1a10+l\"seconds\" -Byf1a1 -BS -P -K >! ${PS}\n" );

	wrtgrn2sac( g1, ista, wavetype_string, "g1", make_output_dirs );
	wrtgrn2sac( g2, ista, wavetype_string, "g2", make_output_dirs );


	for( i=gstart; i<gend; i++ )
	{
		snprintf( filename1, 256, "g1.%s.%g.grns", cmp[i], g1->evdp );
		snprintf( filename2, 256, "g2.%s.%g.grns", cmp[i], g2->evdp );

		if( first == 1 ) {
			first = 0;
			yoffset = 0.0;
		}else{
			yoffset = 0.8;
		}

		line_color_switch = 0;

		getGreensFuctionMinMax( i, g1, g2, &min, &max, &peak1, &peak2 );
		ratio = peak1 / peak2 ;

		fprintf( stdout, "%s: %s: %s: cmp[i]=%s min=%g max=%g peak1=%g peak2=%g ratio=%g\n",
			progname, __FILE__, __func__, cmp[i], min, max, peak1, peak2, ratio );

		fprintf( fp, "set R=\"-R%g/%g/%g/%g\" \n", begin, end, min, max );

	/*** make this line thicker -W1p ***/

		fprintf( fp, "sac2xy < %s | gmt psxy $R $J -W1.25p,%-s -Y%gi -K -O >> ${PS}\n",
                        filename1, line_color[0], yoffset );

	/*** this is overlay line -W0.5p ***/

		fprintf( fp, "sac2xy < %s | gmt psxy $R $J -W0.75p,%-s -K -O >> ${PS}\n",
			filename2, line_color[1] );

	/*** component name ***/

		fprintf( fp, "echo %g 0 %s | gmt pstext $R $J -F+f11p,Helvetica-Bold,black+jMR -N -D0i/0.0i -O -K >> ${PS}\n",
                        begin, string_toupper(cmp[i]) );
	
	/*** label, ratio, peak-amplitudes ***/
		fprintf( fp, "echo %g 0 ratio=%.3f | gmt pstext $R $J -F+f8p,Helvetica,black+jML -N -D-1i/+0.1i -O -K >> ${PS}\n",
                        end, ratio );
		
		fprintf( fp, "echo %g 0 PeakAmp= %5.2e, %5.2e | gmt pstext $R $J -F+f6p,Helvetica,black+jML -N -D-1.2i/-0.1i -O -K >> ${PS}\n",
                        end, (peak1 * MoRatio), (peak2 * MoRatio) );

	}

	/*** line_color[0] = "red" line_color[1] = "black" ***/
	/* g1->net, g1->stnm, g1->loc, g2->net, g2->stnm, g2->loc, g1->v.modfile, g2->v.modfile, */

	/*** title and subtitle strings ***/
	sprintf( title_string, "Greens Function Comparisons depth=(%g/%g) filter=%g-%g(Hz)", g1->evdp, g2->evdp, lf, hf  );
	sprintf( subtitle_string, "@;red;%s@;; vs. @;black;%s@;;", glib_filename1, glib_filename2 );

	/***** subtitles was not compatible with GMT version 6.1, it is a new feature of GMT 6.2+ ****/
	/***     note this is also the last gmt command in the script so close with -O             ***/

	if( check_gmt_version( 6, 2 ) )
	{
		fprintf( fp, "gmt psbasemap $R $J -Bxf1a10+l\"seconds\" -Byf1a1 -BN+s\"%s\"+t\"%s\"  -O  >> ${PS}\n", 
			subtitle_string, title_string );		
	}
	else
	{
		fprintf( fp, "gmt psbasemap $R $J -Bxf1a10+l\"seconds\" -Byf1a1 -BN+t\"%s\"  -O -K >> ${PS}\n", 
			title_string );

		/*** subtitle, just add as string, wont be neat or centered ***/

		fprintf( fp, "gmt pstext $R $J -N -F+f10p,Times-BoldItalic,black+jML -D1.6i/1i -O >> ${PS} << EOF\n" );
		fprintf( fp, "0 0 %s\n", subtitle_string );
		fprintf( fp, "EOF\n" );
	}

        fprintf( fp, "gmt psconvert -A -Tj -E300 ${PS}\n" );

        fprintf( fp, "### cleanup and sys dep plotting\n" );
	fprintf( fp, "##\n" );
        fprintf( fp, "/bin/rm -f ${PS}\n" );

	if( clean_out_tmp_files ) fprintf( fp, "/bin/rm -f g?.???.*.grns\n" );

        fprintf( fp, "# open ${JPG}\n" );
        fclose(fp);

        system( "plotgrn.csh" );
	if( make_output_dirs ) chdir( ".." );
}

/********************************************************************************/
/*** this converts to uppercase and returns new string, leaves original alone ***/
/*** g.ichinose 7/17/2024 - fixed bug, forgot to allocate string space        ***/
/********************************************************************************/

char *string_toupper(char *mystring)
{
	char *temp;
	int i, n;
	n = strlen( mystring );
	temp = calloc(n,sizeof(char));
	for( i = 0; i < n; i++ )
		temp[i] = toupper(mystring[i]);
	return (char *)temp;
}

/*** g.ichinose 7/17/2024 tested and changes committed ***/


void Usage(int ac, char **av)
{
	/* fprintf( stderr, "commandline arguments = %d\n", ac ); */
	fprintf( stderr, "\n" );
	fprintf( stderr, "Usage: %s (required) glib1={string} glib2={string} z={float} \n", av[0] );
	fprintf( stderr, "\t\t (optional) [no]verbose [no]clean tstart={float} tstop={float} \n" );
	fprintf( stderr, "\t\t [no]bpfilter lf={float} hf={float} npass={int} npole={int} \n" );
	fprintf( stderr, "\n" );

	fprintf( stderr, "REQUIRED:\n" );
	fprintf( stderr, "\t glib1={string} filename of first  *.glib Greens function file in grnlib format\n" );
	fprintf( stderr, "\t glib1={string} filename of second *.glib Greens function file in grnlib format\n" );
	fprintf( stderr, "\t z={float}      depth of Greens function in km\n" );
	fprintf( stderr, "\n" );

	fprintf( stderr, "OPTIONAL:\n" );
	fprintf( stderr, "\t verbose or noverbose {boolean} verbose output default noverbose\n" );
	fprintf( stderr, "\t clean or noclean {boolean} delete temporary GFs SAC files after plotting default clean\n" );
	fprintf( stderr, "\t tstart={float} tstop={float} time window in seconds (default uses absolute times relative to origin-time)\n" );
	fprintf( stderr, "\t bpfilter / nobpfilter {boolean} default bpfilter\n" );
	fprintf( stderr, "\t\t if bpfilter then lf={float} hf={float} filter low and hi-freq corner in Hz (default lf=0.01 hf=0.2)\n" );
	fprintf( stderr, "\t\t if bpfilter then npole=3 and npass=2 3 pole, 2-pass acausal Butterworth bandpass filter\n" );
}
