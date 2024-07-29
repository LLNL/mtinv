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

/************************************************************************************************/
/*** Tue Mar  5 14:09:54 PST 20244                                                            ***/
/***    sw46_to_grnlib.c:                                                                     ***/
/***      reads sac files, wildcards and constructs greens function grnlib file               ***/
/***                                                                                          ***/
/************************************************************************************************/

#include <stdio.h>      
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "../include/mt.h" /** global datatype and structure declarations, includes sac.h and mytime.h **/
char progname[128];

typedef struct {

/*** void parse_input_string( int icmp, char *input_string, gfCmpInfo *a, int verbose ); ***/

	int icmp; /* 1-10, skip 0 */
	char code[4];  /*** zss convert to uppercase -> ZSS ***/

				/*** sw4 x-ns y-ew z-vertical ***/
	char xyz_cmp[3][8];  /*** reads as x,y,z  but the order is x=2 y=1 z=0 in vector ***/
	float amp_scale[3];  /*** same order as above x=2 y=1 z=0 in vector ***/

	char sw4_gfs_directory[256]; /*** cus.sw4output.ZSS ***/
	char filename[3][128];  /*** T50A.z, T50A.x, T50A.y, sequential order z=0 x=2 y=1 in vector  ***/
	char fullpath[3][256];  /*** cus.sw4output.ZSS/T50A.z, cus.sw4output.ZSS/T50A.x, cus.sw4output.ZSS/T50A.y ***/

} gfCmpInfo;

int main( int ac, char **av )
{
	Greens g;
	char net[12], sta[12], loc[12];

/*** modsubs.c: get the velocity model ***/
	char modfile[128];
	char modpath[256];
	void create_mod( VelMod * );
	void print_mod0( VelMod * );

/*** GFs components ***/
	int ncmp = 10;
	int icmp = 0; /*** see cmp[] below ***/
		/***        0    1     2       3      4      5      6      7      8      9      10   ***/
        char cmp[11][8] = { "", "ZSS", "ZDS", "ZDD", "ZEX", "RSS", "RDS", "RDD", "REX", "TSS", "TDS" };
	gfCmpInfo a[11];
	char cmp_string[8];
	char input_string[256];
	int jcmp; /*** x,y,z particle motion vector not ffault gfs ***/
	void parse_input_string( int icmp, char *input_string, gfCmpInfo *a, int verbose );
	char *string_toupper(char *temp);

/*** adjust amplitudes according to the reference base moment Mo (Dyne*cm) ***/

	float RBH_basemoment = 1.0000000000E+20; /* Mw= 2.60 = 2/3 * math.log10( 1.0000000000E+20 ) - 10.73 */
	float YZ_basemoment  = 1.2445146117E+16; /* Mw= 0.0  = 2/3 * math.log10( 1.2445146117E+16 ) - 10.73 */
	float RBH_to_YZ_basemoment_ratio; /* 8035.261222317074 or 1.2445146117E-04 */

/*** stasubs.c:getsta() distaz.c:distaz() ***/
	char stadb_filename[256];
	double drdist,daz,dbaz;
	float evla, evlo;
	void getsta( char *stadb_filename, Greens *g, int *verbose );
	int distaz( double, double, double, double, double *, double *, double * );
/***depth - assumes sw4 only produces 1 depth per run ***/
	float evdp;
	int nz = 1;

/*** filtering not needed ***/
/*** processing filtering ***/
/***
        int bpfilter = 0;
        void bandpassFilterGreens( Greens *grn, float lf, float hf, int npole, int npass );
        int npole = 3, npass = 2;
        float lf = 0.01, hf = 0.2;
***/
/*** filter/filtersubs.c ***/
/*
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
*/

/*** rotate/rotate.c ***/
	int do_horizontal_component_rotation = 1, gcp = 1;
        float rotate_to_azimuth = -999; /*** if gcp, then copy baz? ***/

	float x_ew_cmp_azi = 0, y_ns_cmp_azi = 90;
	float x_ew_cmp_inc = 90, y_ns_cmp_inc = 90;

        void  rotate(   float *ns, int ns_nt, float *ns_cmpaz, float ns_cmpinc,
                        float *ew, int ew_nt, float *ew_cmpaz, float ew_cmpinc,
                        float angle, int verbose );

/*** tdif/Differentiates.c, not needed? ***/
	/* void  differentiate( float *x, int n, float dt, int op, int verbose ); */

/*** Integrate.c ***/
	void integrate( float *x, float dt, int *nt, float *beg );

/*** misc ***/
	int i, it;
	FILE *fpg; /*** greens function **/
	FILE *fps; /*** test sac file ***/

	float *readsac( Sac_Header *s, char *sac_filename, int verbose );
	Sac_Header *s[3]; /* z=0, ew(x)=1, ns(y)=2 */
	float *sacdata[3]; /* z=0, ew(x)=1, ns(y)=2 */

	void copy_vector( float *out, float *in, int n ); /*** see this file ***/

	void scale_vector( float *x, int n, float fac ); /*** from sacio/sacio.c ***/

	int verbose = 0;
	char pathname[128];
	char *shorten_path( char *pathname, char *exec_filename );

	int setpar( int, char ** );
        int getpar( char *, char *, void * );
        int mstpar( char *, char *, void * );
        void endpar();

/*****************************/
/*** start program main    ***/
/*****************************/
        strcpy( pathname, av[0] );

	shorten_path( pathname, progname );

	fprintf( stdout, "%s: %s: %s: STDOUT: Version=%s Release Date=%s exec full path=%s\n",
                progname,
                __FILE__,
                __func__,
                Version_Label,
                Version_Date,
                pathname );
	
/*** get cmdline args ***/

	setpar(ac,av);
	getpar("verbose", "b", &verbose );

/*****************************************************************/
/*** network_code.station_code.location_code ***/
/*****************************************************************/
	mstpar( "net", "s", net );
	mstpar( "sta", "s", sta );
	mstpar( "loc", "s", loc );

	mstpar( "stadb", "s", stadb_filename );
	/*** over ride stadb? ***/
	/*** getpar( "stla", "f", &stla ); ***/
	/*** getpar( "stlo", "f", &stlo ); ***/
	/*** getpar( "stel", "f", &stel ); ***/
	
/*****************************************************************/
/*** source event lat,lon,depth ***/
/*****************************************************************/
	mstpar( "evla", "f", &evla );
	mstpar( "evlo", "f", &evlo );
	mstpar( "evdp", "f", &evdp );

/*****************************************************************/
/***  velocity model name (w/o filename extension) and model directory path ***/
/*****************************************************************/
	mstpar( "velmod", "s", modfile );
	mstpar( "modeldb", "s", modpath );

/*****************************************************************/
/*** component information, and amplitude scaling and polarity ***/
/*****************************************************************/
	ncmp = 10;
	getpar( "ncmp", "d", &ncmp );
	for( icmp = 1; icmp <= ncmp; icmp++ )
	{
		sprintf( cmp_string, "cmp%02d", icmp );
		mstpar( cmp_string, "s", input_string );
		parse_input_string( icmp, input_string, &a[icmp], verbose );

		fprintf( stdout,
		  "%s: %s: %s: icmp=%d (%s) code=(%s) sw4_gfs_directory=(%s) ew_cmp=%s ns_cmp=%s z_cmp=%s scale=(%g,%g,%g)\n",
			progname,
			__FILE__,
			__func__,
			icmp,
			input_string,
			a[icmp].code,	
			a[icmp].sw4_gfs_directory,
			a[icmp].xyz_cmp[1], a[icmp].xyz_cmp[2], a[icmp].xyz_cmp[0], 
			a[icmp].amp_scale[1], a[icmp].amp_scale[2], a[icmp].amp_scale[0] );
	}

/*****************************************************************/
/*** rotate horizontal components N,E -> R,T or azimuth ***/
/*****************************************************************/
	getpar( "rotate", "b", &do_horizontal_component_rotation );
	if( do_horizontal_component_rotation )
	{
		getpar( "gcp", "b", &gcp );

		if( !gcp )
			mstpar( "az", "f", &rotate_to_azimuth );

		fprintf( stderr, "%s: %s: %s: Do rotate horizontal components do_horizontal_component_rotation = %d gcp = %d rotate_to_azimuth = %g\n",
			progname, __FILE__, __func__, do_horizontal_component_rotation, gcp, rotate_to_azimuth );
	}
	else
	{
		fprintf(stderr, "%s: %s: %s: Do **NOT** rotate horizontal components do_horizontal_component_rotation = %d gcp = %d rotate_to_azimuth = %g\n",
			progname, __FILE__, __func__, do_horizontal_component_rotation, gcp, rotate_to_azimuth );
	}

/*****************************************************************/
/*** integrate from velocity to displacement (default yes) sw4 synthetics are in velocity (m/s)***/
/*****************************************************************/
	int do_integrate = 1;
	getpar( "integrate", "b", &do_integrate );
	fprintf( stderr, "%s: %s: %s: integrate = %d\n", progname, __FILE__, __func__, do_integrate );

/*****************************************************************/
/*** apply base moment amplitude scaling ***/
/*****************************************************************/
	int do_base_moment_amplitude_scaling = 1, rbh_to_yz = 1;
	float base_moment_ratio = 1;
	getpar( "base_moment_scaling", "b", &do_base_moment_amplitude_scaling );
	if( do_base_moment_amplitude_scaling ) 
	{
		getpar( "rbh_to_yz", "b", &rbh_to_yz );
		if( rbh_to_yz )
		{
			RBH_to_YZ_basemoment_ratio = YZ_basemoment / RBH_basemoment;
			base_moment_ratio = RBH_to_YZ_basemoment_ratio;
		}
		else
		{
			getpar( "base_moment", "f", &base_moment_ratio );
		}
	}
	fprintf( stderr, "%s: %s: %s: do_base_moment_amplitude_scaling = %d rbh_to_yz = %d base_moment_ratio = %e (%e)\n",
		progname, __FILE__, __func__, do_base_moment_amplitude_scaling, rbh_to_yz, base_moment_ratio, (1.0/base_moment_ratio) );

	endpar();

/****************************************************************/
/*** done with command line args                             ***/
/*** copy input into the GLIB structure, see ../include/mt.h ***/
/***************************************************************/

	strcpy( g.net, net );
	strcpy( g.stnm, sta );
	strcpy( g.loc, loc );

/***********************************/
/*** load the velocity model? ***/
/***********************************/
	strcpy( g.v.modfile, modfile );
	strcpy( g.v.modpath, modpath );
	create_mod( &(g.v) );
	print_mod0( &(g.v) );

/***********************************/
/*** load the station information ***/
/***********************************/
	g.stel = 0;
	g.evla = evla;
	g.evlo = evlo;
	g.evdp = evdp;
	getsta( stadb_filename, &g, &verbose );
	fprintf( stdout, "%s: %s: %s: Net.Sta.Loc=(%s.%s.%s) stla=%g stlo=%g stel=%g evla=%g evlo=%g evdp=%g\n",
                progname, __FILE__, __func__,
                g.net, g.stnm, g.loc, g.stla, g.stlo, g.stel,
                g.evla, g.evlo, g.evdp ); 

	if( distaz( (double)g.evla, (double)g.evlo, (double)g.stla, (double)g.stlo, &drdist, &daz, &dbaz ) == 0 )
        {
                g.rdist = (float)drdist;
                g.az    = (float)daz;
                g.baz   = (float)dbaz;

                fprintf( stdout, "%s: %s: %s: distaz(r=%g, az=%g, baz=%g)\n",
                        progname, __FILE__, __func__,
                        g.rdist, g.az, g.baz );
        }
        else
        {
                fprintf( stdout, "%s: %s: %s: distaz: returned a fatal error\n",
                        progname, __FILE__, __func__ );
                exit(-1);
        }


/***********************************************************/
/*** read in sw4 gfs and write out glib format for mtinv ***/
/***********************************************************/

	sprintf( g.filename, "%s.%s.%s.%s.glib", g.net, g.stnm, g.loc, g.v.modfile );

	fprintf( stderr, "%s: %s: %s: writting glib file %s\n", 
		progname, __FILE__, __func__, g.filename );

	if( ( fpg = fopen( g.filename, "wb" ) ) == NULL )
	{
		fprintf( stderr, "%s: %s: %s: cannot open file %s for writting glib file\n", 
			progname, __FILE__, __func__, g.filename );
		exit(-1);
	}

	fwrite(&nz,sizeof(int),1,fpg);
	fwrite(&evdp,sizeof(float),1,fpg);
	fflush(fpg);

	for( icmp = 1; icmp <= ncmp; icmp++ )
	{
		fprintf( stderr, "\n" );
		for ( jcmp = 0; jcmp < 3; ++jcmp )
		{
			fprintf( stderr, "\n" );

			sprintf( a[icmp].filename[jcmp], "%s.%s", g.stnm, a[icmp].xyz_cmp[jcmp] );

			sprintf( a[icmp].fullpath[jcmp], "%s/%s", a[icmp].sw4_gfs_directory, a[icmp].filename[jcmp] );

			fprintf( stderr, "%s: %s: %s: opening files: cmp[%d]=%s jcmp=%d (%s)\n", 
				progname, __FILE__, __func__, 
				icmp, cmp[icmp], jcmp, 
				a[icmp].fullpath[jcmp] );
	
			if( ( fps = fopen( a[icmp].fullpath[jcmp], "rb" ) ) == NULL ) 
			{
				fprintf( stderr, "%s: %s: %s: error cannot open file %s\n", 
					progname, __FILE__, __func__, a[icmp].fullpath[jcmp] );
				exit(-1);
			}
			fclose(fps);

			sacdata[jcmp] = calloc( 8192, sizeof(float) );
			s[jcmp] = calloc( 1, sizeof(Sac_Header) );

			sacdata[jcmp] = readsac( s[jcmp], a[icmp].fullpath[jcmp], 0 /*verbose*/ );

			fprintf( stdout, "%s: %s: %s: sac file = %s npts = %d delta = %g b = %g kstnm=(%s) kcmpnm=(%s)\n",
				progname, __FILE__, __func__, 
				a[icmp].fullpath[jcmp], 
				s[jcmp]->npts, s[jcmp]->delta, s[jcmp]->b, s[jcmp]->kstnm, s[jcmp]->kcmpnm );

			if( s[jcmp]->npts > 4096 )
			{
				s[jcmp]->npts = 4096;
				fprintf(stderr, 
	"%s: %s: %s: WARNING npts exceeds Greens_Function storage space, truncating to 4096\n", progname, __FILE__, __func__ );
			}

			g.nt     = s[jcmp]->npts;
			g.dt     = s[jcmp]->delta;
			g.t0     = s[jcmp]->b;
			g.ts0    = s[jcmp]->b;

			g.redv   = -1;  /*** this is usually set to none ***/
			g.tstart = g.ts0;
			g.tend   =  g.ts0 + g.dt * g.nt;
       			g.twin   =  g.nt * g.dt;

		} /*** loop over xyz components ***/



/*** need to load x and y components and rotate to radial transverse for gfs RXX, TXX ***/
/*** need to convert velocity to displacement by integration ****/
/*** apply the moment scale adjustment RBH_to_YZ_basemoment_ratio ***/
/*** interpolate to new delta and npts ***/ 
        /*** SS components - zss,rss,tss   (x,y) -> rss,tss (z) -> zss ***/
        /*** DS components - zds,rds,tds   (x,y) -> rds,tds (z) -> zds ***/
        /*** DD components - zdd,rdd       (x,y) -> rdd,xxx (z) -> zdd ***/
        /*** EX components - zex,rex       (x,y) -> rex,xxx (z) -> zex  ***/
                        

	/*** RADIALS...    y-ns cmp ->    radial saved to sacdata[2] ***/
	/*** TRANSVERSE... x-ew cmp -> tranverse saved to sacdata[1] ***/
	/*** VERTICAL...   z    cmp ->  vertical saved to sacdata[0] ***/

		if( do_horizontal_component_rotation )
		{
			if( gcp )
			{
				rotate_to_azimuth = g.baz - 180;
				fprintf( stderr, "%s: %s: %s: Rotating to Great Circle Path az=%g baz=%g baz-180=%g rotate_to_azimuth=%g\n", 
					progname, __FILE__, __func__, g.az, g.baz, g.baz - 180, rotate_to_azimuth );
			}
			rotate( sacdata[2], s[2]->npts, &y_ns_cmp_azi, y_ns_cmp_inc, sacdata[1], s[1]->npts, &x_ew_cmp_azi, x_ew_cmp_inc, rotate_to_azimuth, verbose );
		}

		if( strncmp( cmp[icmp], "RSS", 3 ) == 0 )
		{
			fprintf( stdout, "%s: %s: %s: processing (%s, %s)\n", progname, __FILE__, __func__, cmp[icmp], a[icmp].code );

			copy_vector( g.g.rss, sacdata[2], s[2]->npts );

			if( do_base_moment_amplitude_scaling ) 
				scale_vector( g.g.rss, s[2]->npts, base_moment_ratio );

			if( do_integrate )
				integrate( g.g.rss, s[2]->delta, &(s[2]->npts), &(s[2]->b) );

			scale_vector( g.g.rss, s[2]->npts, a[icmp].amp_scale[2] );
		}

		if( strncmp( cmp[icmp], "RDS", 3 ) == 0 ) 
		{
			fprintf( stdout, "%s: %s: %s: processing (%s, %s)\n", progname, __FILE__, __func__, cmp[icmp], a[icmp].code );

			copy_vector( g.g.rds, sacdata[2], s[2]->npts );

			if( do_base_moment_amplitude_scaling ) 
				scale_vector( g.g.rds, s[2]->npts, base_moment_ratio );

			if( do_integrate ) 
				integrate( g.g.rds, s[2]->delta, &(s[2]->npts), &(s[2]->b) );

			scale_vector( g.g.rds, s[2]->npts, a[icmp].amp_scale[2] );
		}

		if( strncmp( cmp[icmp], "RDD", 3 ) == 0 )
		{
			fprintf( stdout, "%s: %s: %s: processing (%s, %s)\n", progname, __FILE__, __func__, cmp[icmp], a[icmp].code );

			copy_vector( g.g.rdd, sacdata[2], s[2]->npts );

			if( do_base_moment_amplitude_scaling )
				scale_vector( g.g.rdd, s[2]->npts, base_moment_ratio );

			if( do_integrate )
				integrate( g.g.rdd, s[2]->delta, &(s[2]->npts), &(s[2]->b) );

			scale_vector( g.g.rdd, s[2]->npts, a[icmp].amp_scale[2] );
		}
                       
		if( strncmp( cmp[icmp], "REX", 3 ) == 0 )
		{
			fprintf( stdout, "%s: %s: %s: processing (%s, %s)\n", progname, __FILE__, __func__, cmp[icmp], a[icmp].code );
			
			copy_vector( g.g.rep, sacdata[2], s[2]->npts );

			if( do_base_moment_amplitude_scaling )
				scale_vector( g.g.rep, s[2]->npts, base_moment_ratio );

			if( do_integrate ) 
				integrate( g.g.rep, s[2]->delta, &(s[2]->npts), &(s[2]->b) );

			scale_vector( g.g.rep, s[2]->npts, a[icmp].amp_scale[2] );
		}

		if( strncmp( cmp[icmp], "ZSS", 3 ) == 0 )
		{
			fprintf( stdout, "%s: %s: %s: processing (%s, %s)\n", progname, __FILE__, __func__, cmp[icmp], a[icmp].code );	

			copy_vector( g.g.zss, sacdata[0], s[0]->npts );

			if( do_base_moment_amplitude_scaling )
                        	scale_vector( g.g.zss, s[0]->npts, base_moment_ratio );

			if( do_integrate ) 
				integrate( g.g.zss, s[0]->delta, &(s[0]->npts), &(s[0]->b) );

			scale_vector( g.g.zss, s[0]->npts, a[icmp].amp_scale[0] );
		}

		if( strncmp( cmp[icmp], "ZDS", 3 ) == 0 )
		{
			fprintf( stdout, "%s: %s: %s: processing (%s, %s)\n", progname, __FILE__, __func__, cmp[icmp], a[icmp].code );

			copy_vector( g.g.zds, sacdata[0], s[0]->npts );

			if( do_base_moment_amplitude_scaling )
                        	scale_vector( g.g.zds, s[0]->npts, base_moment_ratio );

			if( do_integrate ) 
				integrate( g.g.zds, s[0]->delta, &(s[0]->npts), &(s[0]->b) );

			scale_vector( g.g.zds, s[0]->npts, a[icmp].amp_scale[0] );
		}

		if( strncmp( cmp[icmp], "ZDD", 3 ) == 0 )
		{
			fprintf( stdout, "%s: %s: %s: processing (%s, %s)\n", progname, __FILE__, __func__, cmp[icmp], a[icmp].code );

			copy_vector( g.g.zdd, sacdata[0], s[0]->npts );

			if( do_base_moment_amplitude_scaling )
                        	scale_vector( g.g.zdd, s[0]->npts, base_moment_ratio );

			if( do_integrate )
				integrate( g.g.zdd, s[0]->delta, &(s[0]->npts), &(s[0]->b) );

			scale_vector( g.g.zdd, s[0]->npts, a[icmp].amp_scale[0] );
		}

		if( strncmp( cmp[icmp], "ZEX", 3 ) == 0 )
		{
			fprintf( stdout, "%s: %s: %s: processing (%s, %s)\n", progname, __FILE__, __func__, cmp[icmp], a[icmp].code );

			copy_vector( g.g.zep, sacdata[0], s[0]->npts );

			if( do_base_moment_amplitude_scaling )
                        	scale_vector( g.g.zep, s[0]->npts, base_moment_ratio );
			
			if( do_integrate ) 
				integrate( g.g.zep, s[0]->delta, &(s[0]->npts), &(s[0]->b) );

			scale_vector( g.g.zep, s[0]->npts, a[icmp].amp_scale[0] );
		}

		if( strncmp( cmp[icmp], "TSS", 3 ) == 0 )
		{
			fprintf( stdout, "%s: %s: %s: processing (%s, %s)\n", progname, __FILE__, __func__, cmp[icmp], a[icmp].code );

			copy_vector( g.g.tss, sacdata[1], s[1]->npts );
			
			if( do_base_moment_amplitude_scaling )
                        	scale_vector( g.g.tss, s[1]->npts, base_moment_ratio );

			if( do_integrate )
				integrate( g.g.tss, s[1]->delta, &(s[1]->npts), &(s[1]->b) );

			scale_vector( g.g.tss, s[1]->npts, a[icmp].amp_scale[1] );
		}

		if( strncmp( cmp[icmp], "TDS", 3 ) == 0 )
		{
			fprintf( stdout, "%s: %s: %s: processing (%s, %s)\n", progname, __FILE__, __func__, cmp[icmp], a[icmp].code );

			copy_vector( g.g.tds, sacdata[1], s[1]->npts );

			if( do_base_moment_amplitude_scaling )
                        	scale_vector( g.g.tds, s[1]->npts, base_moment_ratio );

			if( do_integrate )
				integrate( g.g.tds, s[1]->delta, &(s[1]->npts), &(s[1]->b) );


			scale_vector( g.g.tds, s[1]->npts, a[icmp].amp_scale[1] );
		}

	} /*** loop over GFs components ***/

	fwrite( &g, sizeof(Greens), 1, fpg ); 

	fclose(fpg);
	fprintf( stdout, "%s: %s: %s: writting out file %s\n", progname, __FILE__, __func__, g.filename );
	fprintf( stdout, "%s: program finished. Bye-Bye!\n\n\n", progname );
}

void parse_input_string( int icmp, char *input_string, gfCmpInfo *a, int verbose )
{
	char tmp[1024], *ptr, tok[2] = { ',', '\0' } ;

	ptr = strtok( input_string, tok ); 
	strcpy( a->code, ptr );
	if(verbose)fprintf(stderr, "%s: %s: %s: ptr=%s a->code=%s\n", progname, __FILE__, __func__, ptr, a->code );
	
	ptr = strtok( NULL, tok );
	strcpy( a->sw4_gfs_directory, ptr );
	if(verbose)fprintf(stderr, "%s: %s: %s: ptr=%s a->sw4_gfs_directory=%s\n", progname, __FILE__, __func__, ptr, a->sw4_gfs_directory );

	ptr = strtok( NULL, tok );
	strcpy( a->xyz_cmp[1], ptr );
	if(verbose)fprintf(stderr, "%s: %s: %s: ptr=%s a->ew_cmp=%s\n", progname, __FILE__, __func__, ptr, a->xyz_cmp[1] );

	ptr = strtok( NULL, tok );
        strcpy( a->xyz_cmp[2], ptr );

	ptr = strtok( NULL, tok );
        strcpy( a->xyz_cmp[0], ptr );

	ptr = strtok( NULL, tok );
	a->amp_scale[1] = atof( ptr );

	ptr = strtok( NULL, tok );
        a->amp_scale[2] = atof( ptr );

	ptr = strtok( NULL, tok );
        a->amp_scale[0] = atof( ptr );
}

char *string_toupper(char *mystring)
{
        int i;
        char *temp;
        for( i = 0; i < 512; i++ )
        {
                if( mystring[i] == '\0' )
                {
                        temp[i]='\0';
                        break;
                }
                temp[i] = toupper(mystring[i]);
        }
        return (char *)temp;
}

void copy_vector( float *out, float *in, int n )
{
	int i;
	for ( i = 0; i < n; i++ ) out[i] = in[i];
}
