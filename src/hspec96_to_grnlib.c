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
/*** Sun Jan 14 18:42:43 PST 2024                                                             ***/
/*** hspec96_to_grnlib.c:                                                                     ***/
/***    reads sac files, wildcards and constructs greens function grnlib file                 ***/
/***                                                                                          ***/
/*** Usage:                                                                                   ***/
/*** hspec96_to_grnlib net=IU sta=WVT loc=00 mod=cus zrange=1,25,1  IU.WVT.00.cus.*.???.SAC   ***/
/***                                                                                          ***/
/***  RBH(basemom) / YZ(basemom) =                                                            ***/
/***  1.0E+20 / 1.2445146117e+16 = 8035.261222317074                                          ***/
/************************************************************************************************/

#include <stdio.h>      
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/mt.h" /** global datatype and structure declarations, includes sac.h and mytime.h **/

char progname[128];

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
	int icmp = 0;
        char cmp[][8] = { "ZDD", "RDD", "ZDS", "RDS", "TDS", "ZSS", "RSS", "TSS", "ZEX", "REX" };

/*** adjust amplitudes according to the reference base moment Mo (Dyne*cm) ***/

	float RBH_basemoment = 1.0000000000E+20; /* Mw= 2.60 = 2/3 * math.log10( 1.0000000000E+20 ) - 10.73 */
	float YZ_basemoment  = 1.2445146117E+16; /* Mw= 0.0  = 2/3 * math.log10( 1.2445146117E+16 ) - 10.73 */
	float RBH_to_YZ_basemoment_ratio; /* 8035.261222317074 or 1.2445146117E-04 */

/*** stasubs.c:getsta() distaz.c:distaz() ***/
	void getsta( char *filename, Greens *g, int *verbose );
	int distaz( double, double, double, double, double *, double *, double * );
	double drdist,daz,dbaz;
	char stadb_filename[256];
	float evla, evlo;

/***depth***/
	int iz;
	float *z;
	Depth_Info depth_info;  /*** mt.h: int type=0, z=single depth; type=1 zrange=zmin,zinc,zmax; type=2 multiple irregular **/
				/*** int nz; float zmin, zinc, zmax; float *zvec; ***/

        float single_src_depth_km = -99; /*** type=0 ***/
        int   nz_multiple_irregular = 0; /*** type=2 ***/
        /*** temp space for getpar() ***/
        float zvec[3];
        char  zstr[8];

/*** misc ***/
	int i, it;
	FILE *fpg; /*** greens function **/
	FILE *fps; /*** test sac file ***/

	float *readsac( Sac_Header *s, char *filename, int verbose );
	Sac_Header s;
	float *sacdata;

	int verbose = 0;
	char pathname[128];
	char *shorten_path( char *pathname, char *filename );

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

	/* if( ac <= 1 ) Usage_Print(); */
	
	RBH_to_YZ_basemoment_ratio = YZ_basemoment / RBH_basemoment;

	setpar(ac,av);
	getpar("verbose", "b", &verbose );
	mstpar( "net", "s", net );
	mstpar( "sta", "s", sta );
	mstpar( "loc", "s", loc );

	mstpar( "velmod", "s", modfile );
	mstpar( "modeldb", "s", modpath );

	mstpar( "stadb", "s", stadb_filename );
	mstpar( "evla", "f", &evla );
	mstpar( "evlo", "f", &evlo );

/*** get the depths ***/
	getpar( "z_depth_km", "f", &single_src_depth_km );
	getpar( "nz", "d", &nz_multiple_irregular );
	if( single_src_depth_km == -99 && nz_multiple_irregular == 0 )
        {
         mstpar("zrange", "vf[3]", &zvec );
         depth_info.type = 1;
         depth_info.zmin = zvec[0];
         depth_info.zinc = zvec[1];
         depth_info.zmax = zvec[2];
         depth_info.nz   = ((depth_info.zmax - depth_info.zmin)/depth_info.zinc) + 1;
        }
        else if( single_src_depth_km == -99 && nz_multiple_irregular > 0 )
        {
         depth_info.type = 2;
         depth_info.nz   = nz_multiple_irregular;
         depth_info.zvec = (float *)realloc( depth_info.zvec, nz_multiple_irregular * sizeof(float) );
         sprintf( zstr, "vf[%d]", nz_multiple_irregular );
         mstpar( "z_multiple", zstr, depth_info.zvec );
         for( iz = 0; iz < nz_multiple_irregular; iz++ )
         {
                fprintf( stdout,  "iz=%d z_multiple_irregular=%g\n", iz, depth_info.zvec[iz] );
         }
        }
        else if( single_src_depth_km != -99 && nz_multiple_irregular == 0 ) /* kludge for single src depth */
        {
         depth_info.type = 0;
         depth_info.nz   = 1;
         depth_info.zmin = single_src_depth_km;
         depth_info.zinc = single_src_depth_km*2;
         depth_info.zmax = single_src_depth_km;
        }
        else
        {
                fprintf(stderr,"%s: %s: %s: ERROR! in depth_info\n", progname, __FILE__, __func__ );
                exit(-1);
        }

	endpar();

/*** set the depth vector ***/

	if( depth_info.type == 0 || depth_info.type == 1 )
	{
		z = (float *) calloc( depth_info.nz, sizeof(float) );
		for( iz=0; iz < depth_info.nz; iz++ )
		{
			z[iz] = depth_info.zmin + ( iz * depth_info.zinc );
		}
	}
	else if( depth_info.type==2 )
	{
		z = (float *) calloc( depth_info.nz, sizeof(float) );
		for( iz=0; iz < depth_info.nz; iz++ )
		{
			z[iz] = depth_info.zvec[iz];
		}
	}

/***********************************/
/*** loop over command line args ***/
/***********************************/

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
	getsta( stadb_filename, &g, &verbose );
	fprintf( stdout, "%s: %s: %s: Net.Sta.Loc=(%s.%s.%s) stla=%g stlo=%g stel=%g evla=%g evlo=%g\n",
                progname, __FILE__, __func__,
                g.net, g.stnm, g.loc, g.stla, g.stlo, g.stel,
                g.evla, g.evlo ); 

	if( distaz( (double)g.evla, (double)g.evlo, (double)g.stla, (double)g.stlo, &drdist, &daz, &dbaz ) == 0 )
        {
                g.rdist = (float)drdist;
                g.az    = (float)daz;
                g.baz   = (float)dbaz;

                fprintf( stdout, "%s: %s: %s: distazs(r=%g, az=%g, baz=%g)\n",
                        progname, __FILE__, __func__,
                        g.rdist, g.az, g.baz );
        }
        else
        {
                fprintf( stdout, "%s: %s: %s: distaz: returned a fatal error\n",
                        progname, __FILE__, __func__ );
                exit(-1);
        }

/***********************************/
/*** write out ***/
/***********************************/
	sprintf( g.filename, "%s.%s.%s.%s.glib", g.net, g.stnm, g.loc, g.v.modfile );

	if( ( fpg = fopen( g.filename, "wb" ) ) == NULL )
	{
		exit(-1);
	}

	fwrite(&(depth_info.nz),sizeof(int),1,fpg);
	fwrite(&z[0],depth_info.nz*sizeof(float),1,fpg);
	fflush(fpg);

	for( iz=0; iz< depth_info.nz; iz++ )
	{
		for( icmp = 0; icmp < 10; icmp++ )
		{
			for ( i = 1; i < ac; i++ ) /*** skip the av[0] this is just the command name ***/
			{
				/*** check if this is a file or commandline arg ***/
				if( (fps=fopen(av[i],"rb")) == NULL ) 
				{
					continue;
				}
				fclose(fps);

				sacdata = readsac( &s, av[i], 0 /*verbose*/ );
	
				if( ( s.evdp == z[iz] ) && ( strncmp( cmp[icmp], s.kcmpnm, 3 ) == 0 ) )
				{
					fprintf( stdout, "%s: %s: %s: this is a match net=%s sta=%s loc=%s z=%g cmp=%s kcmpnm=%s sacfile=%s\n",
						progname, __FILE__, __func__, net, sta, loc, z[iz], cmp[icmp], s.kcmpnm, av[i] );

					g.evdp   = z[iz];
					g.nt     = s.npts;
					g.dt     = s.delta;
					g.t0     = s.b;
					g.ts0    = s.b;
					g.tstart = g.ts0;
					g.tend   =  g.ts0 + g.dt * g.nt;
        				g.redv   = 18;
        				g.twin   =  g.nt * g.dt;

					if( strncmp( cmp[icmp], "RSS", 3 ) == 0 ) {
						for( it = 0; it < s.npts; it++ ) g.g.rss[it] = sacdata[it] * RBH_to_YZ_basemoment_ratio;
					}
					if( strncmp( cmp[icmp], "RDS", 3 ) == 0 ) {
                                                for( it = 0; it < s.npts; it++ ) g.g.rds[it] = sacdata[it] * RBH_to_YZ_basemoment_ratio;
                                        }  
					if( strncmp( cmp[icmp], "RDD", 3 ) == 0 ) {
                                                for( it = 0; it < s.npts; it++ ) g.g.rdd[it] = sacdata[it] * RBH_to_YZ_basemoment_ratio;
                                        }  
					if( strncmp( cmp[icmp], "REX", 3 ) == 0 ) {
                                                for( it = 0; it < s.npts; it++ ) g.g.rep[it] = sacdata[it] * RBH_to_YZ_basemoment_ratio;
                                        }  

					if( strncmp( cmp[icmp], "ZSS", 3 ) == 0 ) {
                                                for( it = 0; it < s.npts; it++ ) g.g.zss[it] = sacdata[it] * RBH_to_YZ_basemoment_ratio;
                                        }  
					if( strncmp( cmp[icmp], "ZDS", 3 ) == 0 ) {
                                                for( it = 0; it < s.npts; it++ ) g.g.zds[it] = sacdata[it] * RBH_to_YZ_basemoment_ratio;
                                        }
					if( strncmp( cmp[icmp], "ZDD", 3 ) == 0 ) {
                                                for( it = 0; it < s.npts; it++ ) g.g.zdd[it] = sacdata[it] * RBH_to_YZ_basemoment_ratio;
                                        }  
					if( strncmp( cmp[icmp], "ZEX", 3 ) == 0 ) {
                                                for( it = 0; it < s.npts; it++ ) g.g.zep[it] = sacdata[it] * RBH_to_YZ_basemoment_ratio;
                                        }  
					

					if( strncmp( cmp[icmp], "TSS", 3 ) == 0 ) {
                                                for( it = 0; it < s.npts; it++ ) g.g.tss[it] = sacdata[it] * RBH_to_YZ_basemoment_ratio;
                                        }  
					if( strncmp( cmp[icmp], "TDS", 3 ) == 0 ) {
                                                for( it = 0; it < s.npts; it++ ) g.g.tds[it] = sacdata[it] * RBH_to_YZ_basemoment_ratio;
                                        }  
				}
				else
				{
					continue;
				} /*** if else check depth and component ***/

			} /*** loop over all sac files ***/

		} /*** loop over GFs components ***/

		fwrite( &g, sizeof(Greens), 1, fpg ); 

	} /*** loop over depths ***/

	fclose(fpg);
	fprintf( stdout, "%s: %s: %s: writting out file %s\n", progname, __FILE__, __func__, g.filename );
	fprintf( stdout, "%s: program finished. Bye-Bye!\n\n\n", progname );
}
