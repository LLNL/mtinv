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

/*** mkdirp ***/
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "../include/nrutil.h"     /** numerical recipes **/
#include "../include/mt.h"         /** global datatype and structure declarations **/

char progname[128];

#define MT_INPUT        0
#define SDR_MO_INPUT    1
#define SDR_MISO_INPUT  2
#define ISO_CLVD_INPUT  3

/*********************************************************************************************************************/
/*********************************************************************************************************************
* Reads Green's function libraries from mkgrnlib using the 3 fundamental faulting orientations:                      *
*  SS,DS,DD+Isotropic format of Helmberger and Langston (H&L 1975) and outputs Gfs in Mij format.                    *
*  Writes output to new directories: (e.g.), \"INCN/IU.INCN.00.mdj2.glib.rxx.grn\" where                             *
*  extensions *.{cmp}(i}{j}.grn are Mij i={x,y,z} j={x,y,z}, and component cmp={z,r,t}                               *
*                                                                                                                    *
*    Use glib2inv test_special and mtinv special argument options:                                                   *
*    The storage structure here is local but uses mt.h: float EventInfo.rtzGxy[24][4096] in glib2inv.c and mtinv.c   *
*    See glib2inv.c calls load_special_grns.c:load_special_grns() and glib2inv_serial.c:glib2inv_special()           *
*    See mtinv.c calls load_special_grns.c:load_special_grns() and make_amatrix.c:void make_amatrix_special()        *
**********************************************************************************************************************/
/*********************************************************************************************************************/

int main( int ac, char **av )
{
	Greens grn;
	int nz, iz;
	FILE *fp;
	char filename[256];
	float *z, my_z, ztol;
	MyTime ot;
	MyTime beg;
	
	int verbose = 0;
	int mtdegfree = 5;  /* 1 ISO, 5 DEV, or 6 FULL */
	int ifoundz = 0;
	int input_type; 
	int iwrtdir = 1;

/******************************************************************************/
/*** timesubs.o ***/
/******************************************************************************/
	char dateid[256];
	void parsestring( MyTime *t, char *str );
	void clone_mytime(  MyTime *t2, MyTime *t1 );
	void WriteMyTime2STDOUT( MyTime * );

	int setpar(int ac, char **av);
	int mstpar( char *, char *, void * );
        int getpar( char *, char *, void * );
	void endpar();

	void conv2mxy( Greens *grn, int mtdegfree, int iwrtdir, int verbose );
	void Usage(int ac);

/**********************/
/**** begin program ***/
/**********************/
	strcpy( progname, av[0] );
        fprintf( stderr, "\n\n%s: %s: %s: Version=%s Release Date=%s\n",
                progname, __FILE__, __func__, Version_Label, Version_Date );

	fprintf( stdout, "\n\n%s: %s: %s: Version=%s Release Date=%s\n",
                progname, __FILE__, __func__, Version_Label, Version_Date );

	if( ac < 3 ) Usage(ac);

	setpar(ac,av);
	mstpar("glib",    "s", filename );
	mstpar("z",       "f", &my_z );
	getpar( "mtdegfree", "d", &mtdegfree );
	getpar( "wrtdir", "b", &iwrtdir );
	getpar("verbose", "b", &verbose );

/*** no implemented yet ***/
	strcpy( dateid, "2008/01/01,00:00:00\0" );
	getpar( "date", "s", dateid );
	endpar();

/*** set the time ***/
/*
	parsestring( &ot, dateid );
	clone_mytime( &ot, &(ot) );
	clone_mytime( &ot, &(beg) );
*/

/**********************************************/
/*** open green's function file and read in ***/
/**********************************************/
	if( (fp = fopen( filename, "rb" ) ) == NULL )
        {
                fprintf(stderr, "%s: Fatal Error, cannot open file %s\n",
                        progname, filename );
                exit(-1);
        }

	fread(&nz,sizeof(int),1,fp);
        z = malloc(nz*sizeof(float));
        fread(&z[0],nz*sizeof(float),1,fp);

/**************************************************************************/
/*** search Green function library for the requested depth              ***/
/**************************************************************************/
	ifoundz = -1;
        ztol = 1.0E-05;
        for( iz=0; iz<nz; iz++ )
        {
                if( (my_z > z[iz]-ztol) && (my_z < z[iz]+ztol) )
                {
                        ifoundz = iz;
                        if(verbose)
                          fprintf( stdout, "%s: %s: %s: found iz=%d z=%g\n",
                            progname, __FILE__, __func__, ifoundz, z[ifoundz] );
                        break;
                }
        }

        if( ifoundz < 0 )
        {
                fprintf( stderr, "%s: %s: %s: Fatal ERROR My_z=%g not found in z=",
                        progname, __FILE__, __func__, my_z );
        }
        else
        {
                if(verbose)
                  fprintf(stderr, "%s: %s: %s: found my_z=%g ifoundz=%d iz=%d z=%g\n",
                        progname, __FILE__, __func__, 
			my_z, ifoundz, iz, z[ifoundz] );
        }

	for( iz=0; iz<nz; iz++ )
	{
		fread(&grn,sizeof(Greens),1,fp);

		if( iz == ifoundz )
		{
			conv2mxy( &grn, mtdegfree, iwrtdir, verbose );
			break;
		}
	}
	fclose(fp);
	free(z);

	exit(0);

} /*** end of main.c ***/

void conv2mxy( Greens *grn, int mtdegfree, int iwrtdir, int verbose )
{
	Sac_Header sp;
	char sacfile[264]; /* 256 + 8 */
	FILE *fp;
	int it;

	float *rss, *rds, *rdd, *rep, *zss, *zds, *zdd, *zep, *tss, *tds;
	float *txx, *txy, *txz, *tyy, *tyz;
        float *rxx, *rxy, *rxz, *ryy, *ryz, *rzz;
        float *zxx, *zxy, *zxz, *zyy, *zyz, *zzz;

	float third = 0.333333333333333333;
	float sixth = 0.166666666666666667;
	float half  = 0.500000000000000000;
	int nt;
	float dt, t0, fi, e;
	/* float twin = 0; */

	int mkdirp2( const char *, mode_t mode );
	char outDirectory[64];
	
	void set_sac_minmax( Sac_Header *s, float *data );

/*** begin subroutine ***/
	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: mtdegfree = %d ",
		progname, __FILE__, __func__, mtdegfree );
	  if( mtdegfree == FORCE_EXPLOSION )          fprintf( stdout, " FORCE_EXPLOSION\n" );
	  if( mtdegfree == DEVIATORIC_MOMENT_TENSOR ) fprintf( stdout, " DEVIATORIC_MOMENT_TENSOR\n" );
	  if( mtdegfree == FULL_MOMENT_TENSOR )       fprintf( stdout, " FULL_MOMENT_TENSOR\n" );
	}

	fi = grn->az * (M_PI/180);
	nt = grn->nt;
	dt = grn->dt;
	t0 = grn->t0;
	/* twin = (float)nt * dt; */
	e = t0 + (nt*dt);

/*** 
 mt.h:float EventInfo.rtzGxy[24][4096] 

             int icmp  0      1      2      3       4     5      6      7      8      9     10     11     12     13     14     15     16 
const char *cmp[] = { "rxx", "rxy", "rxz", "ryy", "ryz", "rzz", "txx", "txy", "txz", "tyy", "tyz", "zxx", "zxy", "zxz", "zyy", "zyz", "zzz" };
***/


/**************************************************************************************************/
/*** allocate memory for green func ***/
/**************************************************************************************************/
	txx = calloc( nt, sizeof(float) );
	txy = calloc( nt, sizeof(float) );
	txz = calloc( nt, sizeof(float) );
	tyy = calloc( nt, sizeof(float) );
	tyz = calloc( nt, sizeof(float) );
	
	rxx = calloc( nt, sizeof(float) );
	rxy = calloc( nt, sizeof(float) );
	rxz = calloc( nt, sizeof(float) );
	ryy = calloc( nt, sizeof(float) );
	ryz = calloc( nt, sizeof(float) );
	rzz = calloc( nt, sizeof(float) );

	zxx = calloc( nt, sizeof(float) );
	zxy = calloc( nt, sizeof(float) );
	zxz = calloc( nt, sizeof(float) );
	zyy = calloc( nt, sizeof(float) );
	zyz = calloc( nt, sizeof(float) );
	zzz = calloc( nt, sizeof(float) );

/**************************************************************************************************/
/*** simplify by assigning local pointers to structure ***/
/**************************************************************************************************/
        rss = grn->g.rss;
        rds = grn->g.rds;
        rdd = grn->g.rdd;
        rep = grn->g.rep;
        zss = grn->g.zss;
        zds = grn->g.zds;
        zdd = grn->g.zdd;
        zep = grn->g.zep;
        tss = grn->g.tss;
        tds = grn->g.tds;

	/* scale? = 1.0E+20 / base_moment; */

	for( it = 0; it < nt; it++ )
	{

/*** transverse components ***/

          if( mtdegfree == FORCE_EXPLOSION )
	  {
		txx[it] = 0;
          	txy[it] = 0;
          	txz[it] = 0;
          	tyy[it] = 0;
          	tyz[it] = 0;
	  }
	  else
	  {
		txx[it] = ( half * sin(2*fi) * tss[it] );
		txy[it] = (       -cos(2*fi) * tss[it] );
		txz[it] = (        sin(fi)   * tds[it] );
		tyy[it] = (-half * sin(2*fi) * tss[it] );
		tyz[it] = (       -cos(fi)   * tds[it] );
  	  }
	
/*** radial components ***/

	  if( mtdegfree == FORCE_EXPLOSION )
	  {
		rxx[it] = ( third * rep[it] );
		rxy[it] = 0;
		rxz[it] = 0;
		ryy[it] = ( third * rep[it] );
		ryz[it] = 0;
		rzz[it] = ( third * rep[it] );
	  } 
	  else if( mtdegfree == DEVIATORIC_MOMENT_TENSOR )
	  {
		rxx[it] = ( +half*cos(2*fi) * rss[it] -half*rdd[it] );
		rxy[it] = (                     sin(2*fi) * rss[it] );
		rxz[it] = (                       cos(fi) * rds[it] );
		ryy[it] = ( -half*cos(2*fi) * rss[it] -half*rdd[it] );	
		ryz[it] = (                       sin(fi) * rds[it] );
		rzz[it] = 0;
	  } 
	  else if( mtdegfree == FULL_MOMENT_TENSOR )
	  {
		rxx[it] = ( +half*cos(2*fi) * rss[it] - sixth*rdd[it] + third*rep[it] );
		rxy[it] = ( sin(2*fi) * rss[it] );
		rxz[it] = (   cos(fi) * rds[it] );
		ryy[it] = ( -half*cos(2*fi) * rss[it] - sixth*rdd[it] + third*rep[it] );
		ryz[it] = (   sin(fi) * rds[it] );
		rzz[it] = ( third * rdd[it] + third * rep[it] );
	  }

/*** vertical components ***/

	  if( mtdegfree == FORCE_EXPLOSION )
	  {
		zxx[it] = ( third * zep[it] );
                zxy[it] = 0;
                zxz[it] = 0;
                zyy[it] = ( third * zep[it] );
                zyz[it] = 0;
                zzz[it] = ( third * zep[it] );
	  }
	  else if( mtdegfree == DEVIATORIC_MOMENT_TENSOR )
	  {
		zxx[it] = ( +half*cos(2*fi) * zss[it] -half*zdd[it] );
                zxy[it] = (       sin(2*fi) * zss[it] );
                zxz[it] = (         cos(fi) * zds[it] );
                zyy[it] = ( -half*cos(2*fi) * zss[it] -half*zdd[it] );
                zyz[it] = (         sin(fi) * zds[it] );
                zzz[it] = 0;
	  }
	  else if( mtdegfree == FULL_MOMENT_TENSOR )
	  {
		zxx[it] = ( +half*cos(2*fi) * zss[it] - sixth*zdd[it] + third*zep[it] );
		zxy[it] = (   sin(2*fi) * zss[it] );
		zxz[it] = (   cos(fi)   * zds[it] );
		zyy[it] = ( -half*cos(2*fi) * zss[it] - sixth*zdd[it] + third*zep[it] );
		zyz[it] = (   sin(fi)   * zds[it] );
		zzz[it] = ( third * zdd[it] + third * zep[it] );
	  }

	} /*** loop over it:nt ***/

	if( iwrtdir )
	{
		sprintf( outDirectory, "%s", grn->stnm );
		mkdirp2( outDirectory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
		chdir( outDirectory );
	}

/*****************************************/
/**** SAC HEADER                     *****/
/*****************************************/
	sp = sac_null;	
	sp.nvhdr = 6;
        sp.norid = 0;
        sp.nevid = 0;
        sp.iftype = ITIME;
        sp.idep = IUNKN;
        sp.iztype = IO;
        sp.ievtyp = IQUAKE;
        sp.leven = TRUE;
        sp.lpspol = FALSE;
        sp.lcalda = TRUE;

	sp.npts = nt;
        sp.delta = dt;
        sp.b = t0;
        sp.e = e;

	strcpy( sp.kstnm, grn->stnm );
	strcpy( sp.knetwk, grn->net );
	
/**************************************************************************************************/
/*** set origin time ***/
/**************************************************************************************************/
/*
        sp.nzyear = ot.year;
        sp.nzjday = ot.jday;
        sp.nzhour = ot.hour;
        sp.nzmin  = ot.min;
        sp.nzsec  = ot.isec;
        sp.nzmsec = ot.msec;
*/
        sp.o      = 0;
        strcpy( sp.ko, "OT" );
/**************************************************************************************************/
/*** set origin hypocenter and receiver locations ***/
/**************************************************************************************************/

        sp.evla = grn->evla;
        sp.evlo = grn->evlo;
        sp.evdp = grn->evdp;
        sp.stla = grn->stla;
        sp.stlo = grn->stlo;
        sp.stel = grn->stel;

	sp.dist = grn->rdist;
	sp.az   = grn->az;
	sp.baz  = grn->baz;

	sp.user0 = grn->redv;
	strcpy( sp.kuser0, "redv" );

        if( grn->redv > 0 )
        {
         sprintf( sp.kt1, "%.1fkms", grn->redv );
         sp.t1 = grn->rdist/grn->redv;  /*** Reduction Velocity km/km/sec ***/
        }

	set_sac_minmax( &sp, txx );
	strcpy( sp.kcmpnm, "TXX" );
	snprintf( sacfile, 264, "%s.txx.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
	fp = fopen(sacfile,"w");
	fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &txx[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	set_sac_minmax( &sp, txy );
	strcpy( sp.kcmpnm, "TXY" );
        snprintf( sacfile, 264, "%s.txy.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &txy[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	set_sac_minmax( &sp, txz );
	strcpy( sp.kcmpnm, "TXZ" );
        snprintf( sacfile, 264, "%s.txz.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &txz[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	set_sac_minmax( &sp, tyy );
	strcpy( sp.kcmpnm, "TYY" );
        snprintf( sacfile, 264, "%s.tyy.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &tyy[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	set_sac_minmax( &sp, tyz );
	strcpy( sp.kcmpnm, "TYZ" );
        snprintf( sacfile, 264, "%s.tyz.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &tyz[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	set_sac_minmax( &sp, rxx );
	strcpy( sp.kcmpnm, "RXX" );
        snprintf( sacfile, 264, "%s.rxx.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &rxx[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	set_sac_minmax( &sp, rxy );
	strcpy( sp.kcmpnm, "RXY" );
        snprintf( sacfile, 264, "%s.rxy.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &rxy[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	set_sac_minmax( &sp, rxz );
	strcpy( sp.kcmpnm, "RXZ" );
        snprintf( sacfile, 264, "%s.rxz.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &rxz[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	set_sac_minmax( &sp, ryy );
	strcpy( sp.kcmpnm, "RYY" );
        snprintf( sacfile, 264, "%s.ryy.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &ryy[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	set_sac_minmax( &sp, ryz );
	strcpy( sp.kcmpnm, "RYZ" );
        snprintf( sacfile, 264, "%s.ryz.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &ryz[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	set_sac_minmax( &sp, rzz );
	strcpy( sp.kcmpnm, "RZZ" );
        snprintf( sacfile, 264, "%s.rzz.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &rzz[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	set_sac_minmax( &sp, zxx );
	strcpy( sp.kcmpnm, "ZXX" );
        snprintf( sacfile, 264, "%s.zxx.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &zxx[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	set_sac_minmax( &sp, zxy );
	strcpy( sp.kcmpnm, "ZXY" );
        snprintf( sacfile, 264, "%s.zxy.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &zxy[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	set_sac_minmax( &sp, zxz );
	strcpy( sp.kcmpnm, "ZXZ" );
        snprintf( sacfile, 264, "%s.zxz.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &zxz[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	set_sac_minmax( &sp, zyy );
	strcpy( sp.kcmpnm, "ZYY" );
        snprintf( sacfile, 264, "%s.zyy.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &zyy[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	set_sac_minmax( &sp, zyz );
	strcpy( sp.kcmpnm, "ZYZ" );
        snprintf( sacfile, 264, "%s.zyz.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &zyz[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	set_sac_minmax( &sp, zzz );
	strcpy( sp.kcmpnm, "ZZZ" );
        snprintf( sacfile, 264, "%s.zzz.grn", grn->filename );
	fprintf(stdout, "%s: %s: %s: sacfile=%s filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, sacfile, grn->filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &zzz[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

	free(txx);
	free(txy);
	free(txz);
	free(tyy);
	free(tyz);
	
	free(rxx);
        free(rxy);
        free(rxz);
        free(ryy);
        free(ryz);
	free(rzz);

	free(zxx);
        free(zxy);
        free(zxz);
        free(zyy);
        free(zyz);
	free(zzz);
}	


void Usage(int ac)
{
	fprintf( stderr, "Usage: %s glib=%%s z=%%f mtdegfree=%%d [no]verbose [no]wrtdir \n", progname );
	fprintf( stderr, "\n" );

	fprintf( stderr, "REQUIRED: \n" );
	fprintf( stderr, "\t glib=string       - Greens function library file ending in *.glib extension [required]\n" );
	fprintf( stderr, "\t z=float           - The hypocenter depth [required]\n" );
	fprintf( stderr, "\n" );

	fprintf( stderr, "OPTIONAL: \n" );
	fprintf( stderr, "\t mtdegfree=integer - 1=force isotropic, 5=deviatoric_moment_tensor, 6=full_moment_tensor [default 5]\n" );
	fprintf( stderr, "\t [no]verbose       - verbosity boolean default off\n" );
	fprintf( stderr, "\t [no]wrtdir        - write output to new directory by station name [boolean default on]\n" );
	fprintf( stderr, "\n" );

	fprintf( stderr, "DESCRIPTION: \n" );
	fprintf( stderr, "\t Reads Green's function libraries from mkgrnlib using the 3 fundamental faulting orientations:\n" );
	fprintf( stderr, "\t SS,DS,DD+Isotropic format of Helmberger and Langston (H&L 1975) and outputs Gfs in Mij format.\n" );
	fprintf( stderr, "\t Writes output to new directories: (e.g.), \"INCN/IU.INCN.00.mdj2.glib.rxx.grn\" where \n" );
	fprintf( stderr, "\t extensions *.{cmp}(i}{j}.grn are Mij i={x,y,z} j={x,y,z}, and component cmp={z,r,t} \n" );

	fprintf( stderr, "\n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "\t Use glib2inv test_special and mtinv special argument options: \n" );
	fprintf( stderr, "\t The storage structure here is local but uses mt.h: float EventInfo.rtzGxy[24][4096] in glib2inv.c and mtinv.c \n" );
	fprintf( stderr, "\t See glib2inv.c calls load_special_grns.c:load_special_grns() and glib2inv_serial.c:glib2inv_special() \n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "\t See mtinv.c calls load_special_grns.c:load_special_grns() and make_amatrix.c:void make_amatrix_special() \n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "\n" );

	exit(0);
}
