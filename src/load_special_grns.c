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
#include <sys/stat.h>
#include <errno.h>
#include "../include/nrutil.h"
#include "../include/mt.h"

extern char progname[128];

/*** load_special_grns.c - load_special_grns() - loads Greens functions saved in Mij format (Not fundamental-faulting) ***/
/*** use grn2Mxy to convert/create these kinds of  Greens functions ***/
/*** this is called by glib2inv.c. grn2Mxy.c, and mtinv.c ***/

/*****************************************************************************************************************/
/*** load green's functions (see email note below)                                                             ***/
/*** saved in 17 SAC files formated in Zxx, Zxy, Zxz, ..., Rxx, Rxy, ... Txx, Txy, ... Tyz (Tzz = 0)           ***/
/*****************************************************************************************************************/
/*** To load green's functions - saved in GLIB binary files in regular 10 fundamental faulting format          ***/
/*** RSS,RDS,RDD,REX, ZSS,ZDS,ZDD,ZEP, TSS,TDS                                                                 ***/
/***                                                                                                           ***/
/*** Greens_subs.c:     mtinv_subs.c:load_greens() loads the *.ginv files/processed glibs                      ***/
/*****************************************************************************************************************/

/**
I add the option now but need to test the code changes which requires writing something to provide test input.

With a special command line flag for mtinv, the code will read 17 SAC files for the Green’s functions.
Six (rxx, rxy, rxz, ryy, ryz, rzz) are for the radial component.
Six (zxx, zxy, zxz, zyy, zyz, zzz) are for the vertical components.
There is only 5 (txx, txy, txz, tyy, tyz) for the transverse component because tzz is zero.
The files need to be in subdirectories named by station and the filename format is station.network.model.glib.rxx.grn as listed in the mtinv.par file.

Should take me a few days to test. For this to work properly, the data and GFs need to be processed in the same exact way (i.e., resampling and filtering).
**/


float *load_special_grns( EventInfo *ev, Greens **grn, int nsta, int *nz_tmp, int verbose )
{
        int it = 0, nt = 0, ista = 0, iz = 0, nz = 0;
        float *z;
        float min=0, max=0;
        float mydepth = 0;

/*** Sac Grn Func ***/
        Sac_Header s;
        float *data;
        int ncmp = 17;
        int icmp = 0;        /* 0      1      2      3       4     5      6      7      8      9     10     11     12     13     14     15     16 ***/
        const char *cmp[] = { "rxx", "rxy", "rxz", "ryy", "ryz", "rzz", "txx", "txy", "txz", "tyy", "tyz", "zxx", "zxy", "zxz", "zyy", "zyz", "zzz" };
        char sacfilename[400];

        float *readsac( Sac_Header *s, char *filename, int verbose );
        void duplicate_vector( float *x, float *y, int n );
        int noverbose = 0;

/*** start routine ***/
        *nz_tmp = 1;
        nz = *nz_tmp;

        for( ista = 0; ista < nsta; ista++ )
        {
                for( icmp = 0; icmp < ncmp; icmp++ )
		{
                                /* 128    +      8 + 1 + 256 + 3 + 4 */
                        snprintf( sacfilename, 400, "%s/%s.%s.grn",
                                ev[ista].stnm, 
				ev[ista].glib_filename, 
				cmp[icmp] );

                        if(verbose)
                        {
                                fprintf( stdout, "%s: %s: %s: reading file %s ista=%d icmp=%d cmp=%s\n",
                                        progname, __FILE__, __func__, sacfilename, ista, icmp, cmp[icmp] );
                        }
                        if( (data = readsac( &s, sacfilename, noverbose )) == NULL )
			{
				fprintf( stdout, "%s: %s: %s: ERROR reading file %s\n",
                                        progname, __FILE__, __func__, sacfilename );
                                exit(-1);
                        }

                        fprintf( stdout, "%s: %s: %s: %s %s.%s.%s nt=%d dt=%g evdp=%g min=%g max=%g\n",
                                progname, __FILE__, __func__,
                                sacfilename,
                                s.kstnm, s.knetwk, s.kcmpnm,
                                s.npts, s.delta,
                                s.evdp,
                                s.depmin, s.depmax );

		/*** see mt.h for EventInfo.rtzGxy[icmp][nt] memory storage for greens functions ***/

                        duplicate_vector( data, &(ev[ista].rtzGxy[icmp][0]), s.npts );

                } /*** cmp loop ***/

		ev[ista].evla = s.evla;
		ev[ista].evlo = s.evlo;
		ev[ista].stla = s.stla;
		ev[ista].stlo = s.stlo;
		ev[ista].evdp = s.evdp;
		ev[ista].rdistkm = s.dist; 
		ev[ista].az    = s.az;
		ev[ista].baz   = s.baz;
		ev[ista].rdist = s.gcarc;
		ev[ista].tstart = s.b;
		ev[ista].tend   = s.e;
		
                mydepth = s.evdp;

                grn[ista] = (Greens *)malloc( (nz+1)*sizeof(Greens));

                grn[ista][iz].nt    = s.npts;
		grn[ista][iz].dt    = s.delta;
                grn[ista][iz].t0    = s.b;

		grn[ista][iz].tstart = s.b;
		grn[ista][iz].tend   = s.e;
		if( s.user0 != -12345. ) grn[ista][iz].redv = s.user0;

                grn[ista][iz].az    = s.az;
                grn[ista][iz].rdist = s.dist;
		grn[ista][iz].baz   = s.baz;

		grn[ista][iz].evla = s.evla;
		grn[ista][iz].evlo = s.evlo;
		grn[ista][iz].stla = s.stla;
		grn[ista][iz].stlo = s.stlo;
		grn[ista][iz].evdp = s.evdp;
		grn[ista][iz].stel = 0;

		strcpy( grn[ista][iz].stnm, s.kstnm );
		strcpy( grn[ista][iz].net, s.knetwk );
		
        } /*** station loop ***/

        fprintf( stdout, "%s: %s: %s: finished reading greens functions\n",
                progname, __FILE__, __func__ );
        fflush(stdout);

        z = malloc( (*nz_tmp) * sizeof(float) );
        z[0] = mydepth;
        return (float *)z;
}
