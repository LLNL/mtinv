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

extern char progname[128];


/*** glib2inv processes Greens function library files *.glib for moment tensor inversion read by mtinv.c ***/
/*** glib2inv_serial() - single thread version, see glib2inv_parallel.c for pthreads version  ***/

void glib2inv_serial( Greens **grn, EventInfo *ev, DepthVector *z, int nsta, int idumpgrn, int verbose )
{

	int iz, ista;
	int DIFFoperator = 3;
	int mtdegfree = 5;

/********************/
/*** Greens stuff ***/
/********************/

	int ig;
	int ng; /*** depends on wavetype ****/
	int MAX_NUM_GRNS_FUNC = 23;
	int MAX_ARRAY_SIZE = 4096;
	float **garray;

	FILE *fpout;
	int old_nt;
	char taper_type[3];
	float taper_frac_width;

/***************************/
/*** function prototypes ***/
/***************************/

/*** tdif/Differentiates.c ***/
        void differentiate( float *x, int n, float dt, int op, int verbose );
                                                                                                                                                               
/*** source/source_subs.c ***/
        void source_time_function( float *data, int nt, float dt, float tr, float tt );
                                                                                                                                                               
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

/*** Ichinose Feb2010 ***/
/*** substitute interpolate_fft with interpolate_wiggins ***/
/*** interpolate/interpolate_subs.c ***/

        void interpolate_fft(   float *data,
                                int old_npts, float old_delta,
                                int *new_npts, float new_delta );
                                                                                                                                                              
/*** wrtgrn2sac.c ***/
        void wrtgrn2sac( Greens *g, int ista, char *wavetype, char *fmt, int make_output_dirs );
                                                                                                                                                               
/*** glib2inv_subs.c ***/
        void split2grn( Greens *g, float **garray );
	void split2grnRot( Greens *g, float **garray );

/*** glib2inv_subs.c ***/
        void array2grn( float **garray, Greens *g );
 	void array2grnRot( float **garray, Greens *g );
 
/*** misc_tools/ampshift.c ***/
        void  ampshift( float *x, int n, int verbose );

/***************************************************************************************************/
/**** begin program                                                                              ***/
/***************************************************************************************************/

/******************************************************************************/
/*** loop over stations process Green functions                             ***/
/******************************************************************************/
                                                                                                                                                               
/*******************************************************/
/*** allocate memory for greens function demultiplex ***/
/*******************************************************/
	garray = (float **)malloc( MAX_NUM_GRNS_FUNC * sizeof(float *) );
	for( ig=0; ig < MAX_NUM_GRNS_FUNC; ig++ )
		garray[ig] = (float *)calloc( MAX_ARRAY_SIZE, sizeof(float) );
	
	for( ista = 0; ista < nsta; ista++ )
	{
		for( iz = 0; iz < z[ista].nz; iz++ )
		{

        /******************************************************************/
        /*** demultiplex from structure to array of 10 greens functions ***/
        /*** and loop over the 10 fundamental faulting orientations     ***/
        /******************************************************************/
                         
			if( strcmp( ev[ista].wavetype, "Rotational" ) == 0 )
			{
			  split2grnRot( &grn[ista][iz], garray );
			  ng = 22;
			}
			else if( strcmp( ev[ista].wavetype, "Surf/Pnl" ) == 0 )
			{
                          split2grn( &grn[ista][iz], garray );
			  ng = 10;
                        }
                        else
			{
			  split2grn( &grn[ista][iz], garray );
			  ng = 10;
			}
     
			if(verbose)
			  fprintf( stdout, "%s: %s: %s: ng=%d ista=%d iz=%d\n", 
				progname, __FILE__, __func__, ng, ista, iz );

                        for( ig = 0 ; ig < ng; ig++ )
                        {

			/******************************************************/
			/*** for shallow sources, this shifts the amplitude ***/
			/*** of the first sample to zero, and mitigates the ***/
			/*** artifact of the step offset sometimes from Grn ***/
			/*** functions from shallow sources, before filter  ***/
			/******************************************************/

				ampshift( garray[ig], grn[ista][iz].nt, verbose );                                                                                                                      
                        /**********************************************************/
                        /*** ground motion type -> differentiate for velocity   ***/
                        /**********************************************************/
                                                                                                                                                               
                                if( ev[ista].grd_mo_type == VELOCITY )
                                {
                                        DIFFoperator = 3;
                                        differentiate(
                                                garray[ig],
                                                grn[ista][iz].nt,
                                                grn[ista][iz].dt,
                                                DIFFoperator,
                                                verbose );
                                }
                                                                                                                                                               
                        /***********************************/
                        /*** convolve trapazoid function ***/
                        /***********************************/
                                                                                                                                                               
                                source_time_function(
                                        garray[ig],
                                        grn[ista][iz].nt,
                                        grn[ista][iz].dt,
                                        ev[ista].tr,
                                        ev[ista].tt );
                                                                                                                                                               
                        /****************************************/
                        /*** bandpass filter greens functions ***/
                        /****************************************/
                                                                                                                                                               
                                iir_filter(
                                        garray[ig],
                                        grn[ista][iz].nt,
                                        "BU",
                                        ev[ista].trbndw,
                                        ev[ista].a,
                                        ev[ista].npole,
                                        "BP",
                                        ev[ista].lf,
                                        ev[ista].hf,
                                        grn[ista][iz].dt,
                                        ev[ista].npass );
                                                                                                                                                               
                        /*****************************************************/
                        /*** interpolate greens functions to new nt and dt ***/
                        /*****************************************************/

                                interpolate_fft(
                                        garray[ig],
                                        grn[ista][iz].nt,
                                        grn[ista][iz].dt,
                                        &old_nt,
                                        ev[ista].dt );

                        /*******************************************/
                        /*** taper with the new parameters       ***/
                        /*** do not need to taper the synthetics ***/
                        /*******************************************/
                        /***
                                strcpy( taper_type, "h\0" );
                                taper_frac_width = 0.40;
                                if( ev[ista].dt < 0.85 ) taper_frac_width = 0.25;
                                if( ev[ista].dt < 0.50 ) taper_frac_width = 0.10;
                                taper_frac_width = 0.05;
                                taper( garray[ig], ev[ista].nt,
                                        taper_type, taper_frac_width );
                        ***/
                                                                                                                                                               
                        } /*** loop over Green function - ig ***/
                                                                                                                                                               
                /***********************************************************/
                /*** reset nt and delta from interpolation or decimation ***/
                /***********************************************************/
                                                                                                                                                               
                        grn[ista][iz].nt = ev[ista].nt;
                        grn[ista][iz].dt = ev[ista].dt;
                                                                                                                                                               
                /********************************************/
                /*** convert back from array to structure ***/
                /********************************************/

			if( strcmp( ev[ista].wavetype, "Rotational" ) == 0 )
                          array2grnRot( garray, &grn[ista][iz] );
			else if( strcmp( ev[ista].wavetype, "Surf/Pnl" ) == 0 )
			  array2grn( garray, &grn[ista][iz] );
			else
			  array2grn( garray, &grn[ista][iz] );

                } /*** loop over depth - iz ***/

        } /*** loop over stations - ista  ***/
 
	free(garray);

/**************************************************/
/*** write out greens functions as seperate sac ***/
/***    files for inspection                    ***/
/**************************************************/

        if( idumpgrn )
        {
                for( ista = 0; ista < nsta; ista++ )
                {
                        for( iz = 0; iz < z[ista].nz; iz++ )
                        {
				fprintf( stdout, "%s: %s: %s: calling wrtgrn2sac() ista=%d iz=%d wavetype=%s\n",
					progname, __FILE__, __func__, ista, iz, ev[ista].wavetype );

                                wrtgrn2sac( &grn[ista][iz], ista, ev[ista].wavetype, "\0", 0 /* int make_output_dirs */ );
                        }
                }
        }

/******************************************************************************/
/*** loop over stations and write out green functions                       ***/
/******************************************************************************/
                                                                                                                                                               
        for( ista = 0; ista < nsta; ista++ )
        {
                if( (fpout = fopen( ev[ista].ginv_filename, "wb" )) == NULL )
                {
                        fprintf( stderr,
                          "%s: glib2inv.c: glib2inv(): STDERR: Fatal Error, cannot open file %s\n",
                                progname, ev[ista].ginv_filename );
                        exit(-1);
                }
                                                                                                                                                               
                fprintf( stderr,
                  "%s: %s: %s: STDERR: (%s.%s.%s) nz=%d nt=%d dt=%g writing file %s\n",
                        progname, __FILE__, __func__, 
			grn[ista][0].net,
                        grn[ista][0].stnm,
			grn[ista][0].loc,
                        z[ista].nz,
                        grn[ista][0].nt,
                        grn[ista][0].dt,
                        ev[ista].ginv_filename );
                                                                                                                                                               
                fwrite( &(z[ista].nz), sizeof(int), 1, fpout );
                fwrite( &(z[ista].z[0]), z[ista].nz * sizeof(float), 1, fpout );
                                                                                                                                                               
                for( iz = 0; iz < z[ista].nz; iz++ )
                {
                        fwrite( &grn[ista][iz], sizeof(Greens), 1, fpout );
                }
                                                                                                                                                               
                fclose(fpout);
        }
                                                                                                                                                               
} /*** end of glib2inv_serial.c ***/




/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/
/*** void glib2inv_special( ) ***/
/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/



void glib2inv_special( Greens **grn, EventInfo *ev, DepthVector *z, int nsta, int idumpgrn, int verbose )
{
	int iz, ista;
	int DIFFoperator = 3;
	int mtdegfree = 5;
	
	int ig, ng=10, MAX_ARRAY_SIZE=4096;
	float **garray;
	FILE *fpout;
	int old_nt;
	char taper_type[3];
	float taper_frac_width;

	int it, nt;
	float dt;

	/*** tdif/Differentiates.c ***/
        void differentiate( float *x, int n, float dt, int op, int verbose );

        /*** source/source_subs.c ***/
        void source_time_function( float *data, int nt, float dt, float tr, float tt );

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

        /*** Ichinose Feb2010 ***/
        /*** substitute interpolate_fft with interpolate_wiggins ***/
        /*** interpolate/interpolate_subs.c ***/

        void interpolate_fft(   float *data,
                                int old_npts, float old_delta,
                                int *new_npts, float new_delta );

        /*** wrtgrn2sac.c ***/
        void wrtgrn2sac( Greens *g, int ista, char *wavetype, char *fmt, int make_output_dirs );

        /*** glib2inv_subs.c ***/
        void split2grn( Greens *g, float **garray );

        /*** glib2inv_subs.c ***/
        void array2grn( float **garray, Greens *g );

        /*** misc_tools/ampshift.c ***/
        void  ampshift( float *x, int n, int verbose );

	int ncmp = 17;
	int icmp = 0;        /* 0      1      2      3       4     5      6      7      8      9     10     11     12     13     14     15     16 ***/
        const char *cmp[] = { "rxx", "rxy", "rxz", "ryy", "ryz", "rzz", "txx", "txy", "txz", "tyy", "tyz", "zxx", "zxy", "zxz", "zyy", "zyz", "zzz" };

	void write_Mxy_grns( EventInfo *ev, int nsta, int verbose );

/***************************************************************************************************/
/**** begin program                                                                              ***/
/***************************************************************************************************/

/******************************************************************************/
/*** loop over stations process Green functions                             ***/
/******************************************************************************/
/*******************************************************/
/*** allocate memory for greens function demultiplex ***/
/*******************************************************/

	ng = 17;
	garray = (float **)malloc( ng * sizeof(float *) );

	for( ig=0; ig<ng; ig++ )
	{
		garray[ig] = (float *)calloc( MAX_ARRAY_SIZE, sizeof(float) );
	}

	iz = 0;
	for( ista = 0; ista < nsta; ista++ )
	{
		/*** note: nt and dt from ev[ista] are the requested interpolated values
			   and nt and dt from grn[ista][0] are current values
			ev[ista].nt
			ev[ista].dt 
		    grn[ista][0].nt
		    grn[ista][0].dt
                    test_special options there is no Greens func library and
                    GFs are stored for single depth in SAC files 
		***/

		nt = grn[ista][0].nt;
		dt = grn[ista][0].dt;

	/************************/
	/*** split into array ***/
	/************************/
		for( icmp = 0; icmp < ncmp; icmp++ )
		{
			for( it = 0; it < nt; it++ )
			{
				garray[icmp][it] = ev[ista].rtzGxy[icmp][it];

			} /*** loop over it:nt ***/

		} /*** loop over icmp:ncmp ***/

		for( ig = 0; ig < ng; ig++ )
		{
			ampshift( garray[ig], nt, verbose );

			if( ev[ista].grd_mo_type == VELOCITY )
			{
				DIFFoperator = 3;
				differentiate( garray[ig], nt, dt, DIFFoperator, verbose );
			}

			source_time_function( garray[ig], nt, dt, ev[ista].tr, ev[ista].tt );
			
			iir_filter( garray[ig], nt, "BU", 
                                        ev[ista].trbndw,
                                        ev[ista].a,
                                        ev[ista].npole,
                                        "BP",
                                        ev[ista].lf,
                                        ev[ista].hf,
                                        dt,
                                        ev[ista].npass );

		/*********************************************************************************/
		/*** given old nt, dt -> resample with new ev[ista].dt the nt is already known ***/
		/*********************************************************************************/
			interpolate_fft( garray[ig], nt, dt, &old_nt, ev[ista].dt );
			
			grn[ista][iz].nt = ev[ista].nt;
			grn[ista][iz].dt = ev[ista].dt;

		} /*** loop over ig:ng ***/
		
	/*****************/
	/*** recombine ***/
	/*****************/
		nt = ev[ista].nt;
		for( icmp = 0; icmp < ncmp; icmp++ )
		{
                        for( it = 0; it < nt; it++ )
			{
         			ev[ista].rtzGxy[icmp][it] = garray[icmp][it];               

                        } /*** loop over it:nt ***/

                } /*** loop over icmp:ncmp ***/

	} /*** loop over ista:nsta ***/

	free(garray);

/******************************************************************************/
/*** loop over stations and write out Greens functions as sac files ***/
/******************************************************************************/

	write_Mxy_grns( ev, nsta, verbose );

} /*** glib2inv_serial.c:void glib2inv_special() ***/
	


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/*** void void write_Mxy_grns( EventInfo *ev, int nsta )                  *****/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void write_Mxy_grns( EventInfo *ev, int nsta, int verbose ) 
{
	Sac_Header sp;
	char sacfile[256];
	FILE *fp;

	float *txx, *txy, *txz, *tyy, *tyz;
        float *rxx, *rxy, *rxz, *ryy, *ryz, *rzz;
        float *zxx, *zxy, *zxz, *zyy, *zyz, *zzz;

	int mkdirp2( const char *, mode_t mode );
        char outDirectory[64];

	void set_sac_minmax( Sac_Header *s, float *data );

	int ncmp = 17;
        int icmp = 0;        /* 0      1      2      3       4     5      6      7      8      9     10     11     12     13     14     15     16 ***/
        const char *cmp[] = { "rxx", "rxy", "rxz", "ryy", "ryz", "rzz", "txx", "txy", "txz", "tyy", "tyz", "zxx", "zxy", "zxz", "zyy", "zyz", "zzz" };

	int ista;

	int nt, it;
	float twin, dt, t0, e;
	int bufsiz = 256;

/*** begin subroutine ***/

	if(verbose)
	  fprintf( stdout, "%s: %s: %s: \n", progname, __FILE__, __func__ );

	for( ista = 0; ista < nsta; ista++ )
	{

	  nt = ev[ista].nt;
	  dt = ev[ista].dt;
	  t0 = ev[ista].tstart;
	  twin = (float)nt * dt;
	  e = t0 + (nt*dt);

	  fprintf( stdout, "%s: %s: %s:  ista=%d sta=%s net=%s nt=%d dt=%g t0=%g twin=%g e=%g\n",
		progname, __FILE__, __func__,
		ista, ev[ista].stnm, ev[ista].net, ev[ista].nt, ev[ista].dt, t0, twin, e );

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

	  for( it = 0; it < nt; it++ )
	  {
		rxx[it] = ev[ista].rtzGxy[0][it];
		rxy[it] = ev[ista].rtzGxy[1][it];
		rxz[it] = ev[ista].rtzGxy[2][it];
		ryy[it] = ev[ista].rtzGxy[3][it];
		ryz[it] = ev[ista].rtzGxy[4][it];
		rzz[it] = ev[ista].rtzGxy[5][it];

		txx[it] = ev[ista].rtzGxy[6][it];
		txy[it] = ev[ista].rtzGxy[7][it];
		txz[it] = ev[ista].rtzGxy[8][it];
		tyy[it] = ev[ista].rtzGxy[9][it];
		tyz[it] = ev[ista].rtzGxy[10][it];

		zxx[it] = ev[ista].rtzGxy[11][it];
                zxy[it] = ev[ista].rtzGxy[12][it];
                zxz[it] = ev[ista].rtzGxy[13][it];
                zyy[it] = ev[ista].rtzGxy[14][it];
                zyz[it] = ev[ista].rtzGxy[15][it];
                zzz[it] = ev[ista].rtzGxy[16][it];
	  }
		
	/*** attempt to make the directory if it doesn't exist already ***/
          snprintf( outDirectory, 8, "%s", ev[ista].stnm );
	  mkdirp2( outDirectory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

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
	  sp.o = 0;
          strcpy( sp.ko, "OT" );

          strcpy( sp.kstnm, ev[ista].stnm );
          strcpy( sp.knetwk, ev[ista].net );

/**************************************************************************************************/
/*** set origin hypocenter and receiver locations ***/
/**************************************************************************************************/

        sp.evla = ev[ista].evla;
        sp.evlo = ev[ista].evlo;
        sp.evdp = ev[ista].evdp;
        sp.stla = ev[ista].stla;
        sp.stlo = ev[ista].stlo;
        sp.dist = ev[ista].rdistkm;
        sp.az   = ev[ista].az;
        sp.baz  = ev[ista].baz;
	sp.gcarc= ev[ista].rdist;
	sp.o    = 0;
	sprintf( sp.ko , "OT" );

/*** Reduction Velocity km/km/sec ***/
/*
        if( grn->redv > 0 )
        {
         snprintf( sp.kt1, 8, "%gkm/s", grn->redv );
         sp.t1 = grn->rdistkm/grn->redv;
        }
*/
	set_sac_minmax( &sp, txx );
        strcpy( sp.kcmpnm, "TXX" );

/*******
glib2inv_serial.c:692:9: warning: 'snprintf' will always overflow; destination buffer has size 256, but size argument is 273 [-Wbuiltin-memcpy-chk-size]
        snprintf( sacfile, 273, "%s/%s.txx.grn", sp.kstnm, ev[ista].glib_filename );
********/
			/*  8 1 256 8 = 273 */
        snprintf( sacfile, bufsiz, "%s/%s.txx.grn", sp.kstnm, ev[ista].glib_filename );
	fprintf(stdout, "%s: %s: %s: ista=%d sacfile=%s glib_filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
		progname, __FILE__, __func__, ista, sacfile, ev[ista].glib_filename,
		sp.kstnm, sp.knetwk, sp.kcmpnm,
		sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &txx[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

        set_sac_minmax( &sp, txy );
        strcpy( sp.kcmpnm, "TXY" );
        snprintf( sacfile, bufsiz, "%s/%s.txy.grn", sp.kstnm, ev[ista].glib_filename );
	fprintf(stdout, "%s: %s: %s: ista=%d sacfile=%s glib_filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, ista, sacfile, ev[ista].glib_filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &txy[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

        set_sac_minmax( &sp, txz );
        strcpy( sp.kcmpnm, "TXZ" );
        snprintf( sacfile, bufsiz, "%s/%s.txz.grn", sp.kstnm, ev[ista].glib_filename );
	fprintf(stdout, "%s: %s: %s: ista=%d sacfile=%s glib_filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, ista, sacfile, ev[ista].glib_filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &txz[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

        set_sac_minmax( &sp, tyy );
        strcpy( sp.kcmpnm, "TYY" );
        snprintf( sacfile, bufsiz, "%s/%s.tyy.grn", sp.kstnm, ev[ista].glib_filename );
	fprintf(stdout, "%s: %s: %s: ista=%d sacfile=%s glib_filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, ista, sacfile, ev[ista].glib_filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &tyy[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

        set_sac_minmax( &sp, tyz );
        strcpy( sp.kcmpnm, "TYZ" );
        snprintf( sacfile, bufsiz, "%s/%s.tyz.grn", sp.kstnm, ev[ista].glib_filename );
	fprintf(stdout, "%s: %s: %s: ista=%d sacfile=%s glib_filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, ista, sacfile, ev[ista].glib_filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &tyz[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

        set_sac_minmax( &sp, rxx );
        strcpy( sp.kcmpnm, "RXX" );
        snprintf( sacfile, bufsiz, "%s/%s.rxx.grn", sp.kstnm, ev[ista].glib_filename );
	fprintf(stdout, "%s: %s: %s: ista=%d sacfile=%s glib_filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, ista, sacfile, ev[ista].glib_filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &rxx[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

        set_sac_minmax( &sp, rxy );
        strcpy( sp.kcmpnm, "RXY" );
        snprintf( sacfile, bufsiz, "%s/%s.rxy.grn", sp.kstnm, ev[ista].glib_filename );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &rxy[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

        set_sac_minmax( &sp, rxz );
        strcpy( sp.kcmpnm, "RXZ" );
        snprintf( sacfile, bufsiz, "%s/%s.rxz.grn", sp.kstnm, ev[ista].glib_filename );
	fprintf(stdout, "%s: %s: %s: ista=%d sacfile=%s glib_filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, ista, sacfile, ev[ista].glib_filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &rxz[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

        set_sac_minmax( &sp, ryy );
        strcpy( sp.kcmpnm, "RYY" );
        snprintf( sacfile, bufsiz, "%s/%s.ryy.grn", sp.kstnm, ev[ista].glib_filename );
	fprintf(stdout, "%s: %s: %s: ista=%d sacfile=%s glib_filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, ista, sacfile, ev[ista].glib_filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &ryy[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

        set_sac_minmax( &sp, ryz );
        strcpy( sp.kcmpnm, "RYZ" );
        snprintf( sacfile, bufsiz, "%s/%s.ryz.grn", sp.kstnm, ev[ista].glib_filename );
	fprintf(stdout, "%s: %s: %s: ista=%d sacfile=%s glib_filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, ista, sacfile, ev[ista].glib_filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &ryz[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

        set_sac_minmax( &sp, rzz );
        strcpy( sp.kcmpnm, "RZZ" );
        snprintf( sacfile, bufsiz, "%s/%s.rzz.grn", sp.kstnm, ev[ista].glib_filename );
	fprintf(stdout, "%s: %s: %s: ista=%d sacfile=%s glib_filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, ista, sacfile, ev[ista].glib_filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &rzz[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

        set_sac_minmax( &sp, zxx );
        strcpy( sp.kcmpnm, "ZXX" );
        snprintf( sacfile, bufsiz, "%s/%s.zxx.grn", sp.kstnm, ev[ista].glib_filename );
	fprintf(stdout, "%s: %s: %s: ista=%d sacfile=%s glib_filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, ista, sacfile, ev[ista].glib_filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &zxx[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

        set_sac_minmax( &sp, zxy );
        strcpy( sp.kcmpnm, "ZXY" );
        snprintf( sacfile, bufsiz, "%s/%s.zxy.grn", sp.kstnm, ev[ista].glib_filename );
	fprintf(stdout, "%s: %s: %s: ista=%d sacfile=%s glib_filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, ista, sacfile, ev[ista].glib_filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &zxy[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

        set_sac_minmax( &sp, zxz );
        strcpy( sp.kcmpnm, "ZXZ" );
        snprintf( sacfile, bufsiz, "%s/%s.zxz.grn", sp.kstnm, ev[ista].glib_filename );
	fprintf(stdout, "%s: %s: %s: ista=%d sacfile=%s glib_filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, ista, sacfile, ev[ista].glib_filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &zxz[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

        set_sac_minmax( &sp, zyy );
        strcpy( sp.kcmpnm, "ZYY" );
        snprintf( sacfile, bufsiz, "%s/%s.zyy.grn", sp.kstnm, ev[ista].glib_filename );
	fprintf(stdout, "%s: %s: %s: ista=%d sacfile=%s glib_filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, ista, sacfile, ev[ista].glib_filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &zyy[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

        set_sac_minmax( &sp, zyz );
        strcpy( sp.kcmpnm, "ZYZ" );
        snprintf( sacfile, bufsiz, "%s/%s.zyz.grn", sp.kstnm, ev[ista].glib_filename );
	fprintf(stdout, "%s: %s: %s: ista=%d sacfile=%s glib_filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, ista, sacfile, ev[ista].glib_filename,
                sp.kstnm, sp.knetwk, sp.kcmpnm,
                sp.evla, sp.evlo, sp.evdp, sp.stla, sp.stlo );
        fp = fopen(sacfile,"w");
        fwrite( &sp, sizeof(Sac_Header), 1, fp );
        fwrite( &zyz[0], sp.npts*sizeof(float), 1, fp );
        fclose(fp);

        set_sac_minmax( &sp, zzz );
        strcpy( sp.kcmpnm, "ZZZ" );
        snprintf( sacfile, bufsiz,  "%s/%s.zzz.grn", sp.kstnm, ev[ista].glib_filename );
	fprintf(stdout, "%s: %s: %s: ista=%d sacfile=%s glib_filename=%s sta.net.chan=%s.%s.%s evla=%g evlo=%g evdp=%g stla=%g stlo=%g\n",
                progname, __FILE__, __func__, ista, sacfile, ev[ista].glib_filename,
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

	} /*** loop over nsta:ista ***/

} /*** glib2inv_serial.c: end write_Mxy_grns() ***/
