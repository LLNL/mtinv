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

#include "../include/mteig.h"  /** global datatype and structure declarations **/

extern char progname[128];

/********************************************************************************/
/********************************************************************************/
/*** loop over r[1]...r[nsim_evec] for highest var_red then put this entry    ***/ 
/*** into rbest[irow_best]                                                    ***/
/********************************************************************************/
/********************************************************************************/

/*** rbest - pass only row ptr of the array version (see below for whole array)***/

void search_best_results_row( Results *r, int nsim_evec, Results *rbest )
{
	int i;
	float best_var_red;
	int ibest = 0;
	void transfer_results( Results *in, Results *out );

	best_var_red = r[0].var_red;
	for( i = 0; i < nsim_evec; i++ )
	{
		if( r[i].var_red > best_var_red )
		{
			ibest=i;
			best_var_red=r[i].var_red;
		}
	}

/*** transfer results (in) ----> (out)  ***/
	transfer_results( &(r[ibest]), rbest );

}

/*************************************************************************************/
/*** rbest - pass the whole array version                                          ***/
/*** this version is for the Eigenvectors::Results *r array NOT! Eigenvalues rbest ***/
/*** SEE mteig_gmt.c for searching the rbest table                                 ***/
/*************************************************************************************/

void search_best_results( Results *r, int nsim_evec, Results *rbest, int irow_best )
{
	int i;
	float best_var_red;
	int ibest = 0;
	void transfer_results( Results *in, Results *out );

/*** start subroutine ***/

	best_var_red = r[0].var_red;

	for( i = 0; i < nsim_evec; i++ )
	{
		if( r[i].var_red > best_var_red )
		{
			ibest=i;
			best_var_red=r[i].var_red;
		}
	}

/*** transfer results (in) ----> (out)  ***/
	transfer_results( &(r[ibest]), &(rbest[irow_best]) );

/*** compute SDR amd P/T axes Maj DC  
    add to rbest; too time consuming to 
    do at each iteration ***/

/*** this is too much to do here, need to rewrite the 
    MTINV subroutines to be more generalized ***/
}

/********************************************************************************/
/********************************************************************************/
/*** transfers just one row in array to array                                ***/
/*** ***/
/*** SEE ../include/mteig.h for specifications typedef struct {...} Results; ****/
/********************************************************************************/
/********************************************************************************/

void transfer_results( Results *in, Results *out )
{
	int i, j, n=3;

/*** SEE ../include/mteig.h for specifications typedef struct {...} Results; ****/
	out->nevec    = in->nevec;

	out->var_red  = in->var_red;
	out->lune_lat = in->lune_lat;
	out->lune_lon = in->lune_lon;
	out->k        = in->k;
	out->T        = in->T;
	out->Mtotal   = in->Mtotal;
	out->pdc      = in->pdc;
	out->pclvd    = in->pclvd;
	out->piso     = in->piso;
	out->pdev     = in->pdev;

	out->gcarc_dc  = in->gcarc_dc;
	out->gcarc_exp = in->gcarc_exp;

 /*** Results.mxx **/
	out->mxx      = in->mxx;
	out->myy      = in->myy;
	out->mxy      = in->mxy;
	out->mxz      = in->mxz;
	out->myz      = in->myz;
	out->mzz      = in->mzz;

/*** float e0, e1, e2; from create lamb ***/
	out->e0       = in->e0;
	out->e1       = in->e1;
	out->e2       = in->e2;

/***  Eigenvalue e[3];  fullMT ****/
	for( i=0; i<n; i++ )
	{
		out->e[i].u = in->e[i].u;
		for( j=0; j<n; j++ )
		{
			out->e[i].v[j] = in->e[i].v[j];
		}
		out->e[i].lat = in->e[i].lat;
		out->e[i].lon = in->e[i].lon;
	}

/***   Eigenvalue e_sort[3];   e_sort[0].u/Mo, e_sort[0].lat, e_sort[0].lon ***/
	for( i=0; i<n; i++ )
        {
                out->e_sort[i].u = in->e_sort[i].u;
                for( j=0; j<n; j++ )
                {
                        out->e_sort[i].v[j] = in->e_sort[i].v[j];
                }
                out->e_sort[i].lat = in->e_sort[i].lat;
                out->e_sort[i].lon = in->e_sort[i].lon;
        }

/*  Tensor MT;  Results.MT.xx, MT.yy, ..., M.zz */
	out->MT.xx = in->MT.xx;
	out->MT.xy = in->MT.xy;
	out->MT.xz = in->MT.xz;
	out->MT.yx = in->MT.yx;
	out->MT.yy = in->MT.yy;
	out->MT.yz = in->MT.yz;
	out->MT.zx = in->MT.zx;
	out->MT.zy = in->MT.zy;
	out->MT.zz = in->MT.zz;

/* MTdec FullMT: Results.FullMT.eig[1].val, Results.FullMT.T.az  */
	out->FullMT.eig[1].val    = in->FullMT.eig[1].val;
	out->FullMT.eig[1].vec[1] = in->FullMT.eig[1].vec[1];
	out->FullMT.eig[1].vec[2] = in->FullMT.eig[1].vec[2];
	out->FullMT.eig[1].vec[3] = in->FullMT.eig[1].vec[3];

	out->FullMT.eig[2].val    = in->FullMT.eig[2].val;
        out->FullMT.eig[2].vec[1] = in->FullMT.eig[2].vec[1];
        out->FullMT.eig[2].vec[2] = in->FullMT.eig[2].vec[2];
        out->FullMT.eig[2].vec[3] = in->FullMT.eig[2].vec[3];

	out->FullMT.eig[3].val    = in->FullMT.eig[3].val;
        out->FullMT.eig[3].vec[1] = in->FullMT.eig[3].vec[1];
        out->FullMT.eig[3].vec[2] = in->FullMT.eig[3].vec[2];
        out->FullMT.eig[3].vec[3] = in->FullMT.eig[3].vec[3];

	/* out->FullMT.T.x   = in->FullMT.T.x;
	out->FullMT.T.y   = in->FullMT.T.y; */

	out->FullMT.T.az  = in->FullMT.T.az;
	out->FullMT.T.pl  = in->FullMT.T.pl;
	out->FullMT.T.ev  = in->FullMT.T.ev;
	out->FullMT.T.rad = in->FullMT.T.rad;
        out->FullMT.T.str = in->FullMT.T.str;
        out->FullMT.T.dip = in->FullMT.T.dip;

	/* out->FullMT.P.x   = in->FullMT.P.x;
        out->FullMT.P.y   = in->FullMT.P.y; */

        out->FullMT.P.az  = in->FullMT.P.az;
        out->FullMT.P.pl  = in->FullMT.P.pl;
        out->FullMT.P.ev  = in->FullMT.P.ev;
        out->FullMT.P.rad = in->FullMT.P.rad;
        out->FullMT.P.str = in->FullMT.P.str;
        out->FullMT.P.dip = in->FullMT.P.dip;

	/* out->FullMT.B.x   = in->FullMT.B.x;
        out->FullMT.B.y   = in->FullMT.B.y; */

        out->FullMT.B.az  = in->FullMT.B.az;
        out->FullMT.B.pl  = in->FullMT.B.pl;
        out->FullMT.B.ev  = in->FullMT.B.ev;
        out->FullMT.B.rad = in->FullMT.B.rad;
        out->FullMT.B.str = in->FullMT.B.str;
        out->FullMT.B.dip = in->FullMT.B.dip;
	out->FullMT.P1.s  = in->FullMT.P1.s;
	out->FullMT.P1.d  = in->FullMT.P1.d;
	out->FullMT.P1.r  = in->FullMT.P1.r;
	out->FullMT.P2.s  = in->FullMT.P2.s;
        out->FullMT.P2.d  = in->FullMT.P2.d;
        out->FullMT.P2.r  = in->FullMT.P2.r;

/*** MTdec Maj  :: Results.Maj.eig[1].val, ***/
              /*** Results.Maj.T.az,  Results.Maj.P1.s ***/
              /*** Results.Maj.P1.d   Results.Maj.P1.r   ***/

	out->Maj.eig[1].val    = in->Maj.eig[1].val;
        out->Maj.eig[1].vec[1] = in->Maj.eig[1].vec[1];
        out->Maj.eig[1].vec[2] = in->Maj.eig[1].vec[2];
        out->Maj.eig[1].vec[3] = in->Maj.eig[1].vec[3];

        out->Maj.eig[2].val    = in->Maj.eig[2].val;
        out->Maj.eig[2].vec[1] = in->Maj.eig[2].vec[1];
        out->Maj.eig[2].vec[2] = in->Maj.eig[2].vec[2];
        out->Maj.eig[2].vec[3] = in->Maj.eig[2].vec[3];

        out->Maj.eig[3].val    = in->Maj.eig[3].val;
        out->Maj.eig[3].vec[1] = in->Maj.eig[3].vec[1];
        out->Maj.eig[3].vec[2] = in->Maj.eig[3].vec[2];
        out->Maj.eig[3].vec[3] = in->Maj.eig[3].vec[3];

        /* out->Maj.T.x   = in->Maj.T.x;
        out->Maj.T.y   = in->Maj.T.y; */

        out->Maj.T.az  = in->Maj.T.az;
        out->Maj.T.pl  = in->Maj.T.pl;
        out->Maj.T.ev  = in->Maj.T.ev;
        out->Maj.T.rad = in->Maj.T.rad;
        out->Maj.T.str = in->Maj.T.str;
        out->Maj.T.dip = in->Maj.T.dip;

        /* out->Maj.P.x   = in->Maj.P.x;
        out->Maj.P.y   = in->Maj.P.y; */

        out->Maj.P.az  = in->Maj.P.az;
        out->Maj.P.pl  = in->Maj.P.pl;
        out->Maj.P.ev  = in->Maj.P.ev;
        out->Maj.P.rad = in->Maj.P.rad;
        out->Maj.P.str = in->Maj.P.str;
        out->Maj.P.dip = in->Maj.P.dip;

        /* out->Maj.B.x   = in->Maj.B.x;
        out->Maj.B.y   = in->Maj.B.y; */

        out->Maj.B.az  = in->Maj.B.az;
        out->Maj.B.pl  = in->Maj.B.pl;
        out->Maj.B.ev  = in->Maj.B.ev;
        out->Maj.B.rad = in->Maj.B.rad;
        out->Maj.B.str = in->Maj.B.str;
        out->Maj.B.dip = in->Maj.B.dip;
        out->Maj.P1.s  = in->Maj.P1.s;
        out->Maj.P1.d  = in->Maj.P1.d;
        out->Maj.P1.r  = in->Maj.P1.r;
        out->Maj.P2.s  = in->Maj.P2.s;
        out->Maj.P2.d  = in->Maj.P2.d;
        out->Maj.P2.r  = in->Maj.P2.r;

} /*** end of transfer_results() ****/

Results *read_results( char *filename, int *nsim_eig )
{
	FILE *fp;
	int ir = 0;
	char rec[512];
	char dum[8];
	Results *r;

/** testing ***/
	void print_results_all( Results *r, int nrows, char *filename );

	if( (fp = fopen( filename, "r" )) == NULL )
	{
		fprintf( stderr, "%s: %s: %s: ERROR cannot open file %s for reading \n",
			progname, __FILE__, __func__, 
			filename );
		exit(-1);
	}

	r = calloc( 9000, sizeof(Results) );

	while( fgets( rec, 512, fp ) != NULL )
	{
		r = realloc( r, (ir+1)*sizeof(Results) );

/*1   2      3      4     5     6     7     8     9     10    11    12    13  14    15    16    17  18    19    20   21    22    23    24    25    26    27    28     29    30    31    32    33    34      35    36    37    38    39    40        41        42    */
/*lon lat    vred   k     T     Mo    Taz   Tpl   Baz   Bpl   Paz   Ppl   P1: S     D     R     P2: S     D     R     Mij: Mxx   Myy   Mzz   Mxy   Mxz   Myz   Pev    Plat  Plon  Tev   Tlat  Tlon  Bev     Blat  Blon  pdc   pclvd piso  gcarc_dc  gcarc_exp nevec */
/* lon   lat   vred     k     T  Mtot   Taz   Tpl   Baz   Bpl   Paz   Ppl      stk1  dip1  rak1      stk2  dip2  rak2        Mxx   Myy   Mzz   Mxy   Mxz   Myz   eval1  lat   lon  eval2  lat   lon   eval3  lat    lon   pdc  pclvd piso    e0    e1    e2  dc   exp   nevec */
/*   1     2      3     4     5     6     7     8     9    10    11    12        13    14    15        16    17    18         19    20    21    22    23    24     25    26    27     28    29    30     31    32    33    34    35    36    37    38    39  40   41    42  */
/*
"%8.3f %7.3f %11.3f %5.2f %5.2f %5.2e %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f P1: %3.0f %2.0f %4.0f P2: %3.0f %2.0f %4.0f Mij: %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %+5.2f %3.0f %3.0f %+5.2f %3.0f %3.0f %+5.2f %3.0f %3.0f %5.2f %5.2f %5.2f %6.3f %6.3f %6.3f %8.3f %8.3f %9d\n",
*/

/*********************************************************************************************************************************************************************************************************************************************/
/***    1       2       3      4     5     6          7  8    9  10 11  12 13   14 15   16 17   18 19   20  21   22    23     24     25    26    27   28   29  30 31   32  33   34    35  36   37    38    39    40      41          42    ***/
/**** lon     lat      vr      k     T    Mo-total   taz tpl B   B   P   P       S  D    R       S  D    R       mxx   myy    mzz   mxy   mxz   myz   e0  lat lon e1   lat lon  e2   lat lon  pdc  pclvd piso   arcdc   arcexp    nevec  *****/
/* 15.040 -20.586      64.074 -0.22 -0.54 4.20e+22   0   0   0   0   0   0 P1:   0  0    0 P2:   0  0    0 Mij: -0.87  0.23 -0.01 -0.39  0.07  0.05 -1.00  84 333 -0.00   5 198 +0.36  84 333  0.36  0.42  0.22  25.3007 110.5857      4495  */
/*-20.174  41.108      76.438  0.39  0.70 4.20e+22   0   0   0   0   0   0 P1:   0  0    0 P2:   0  0    0 Mij:  0.02  0.94  0.20 -0.17  0.04 -0.14 -0.01  -5  10 +0.17  -5  10 +1.00  78  76  0.18  0.43  0.39  44.9891  48.8918      2875  */
/*********************************************************************************************************************************************************************************************************************************************/
			/*    1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 */
		sscanf( rec, "%f %f %f %f %f %f %f %f %f %f %f %f %s %f %f %f %s %f %f %f %s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d",
			&(r[ir].lune_lon),
			&(r[ir].lune_lat),
                	&(r[ir].var_red),
                	&(r[ir].k),
                	&(r[ir].T),
                	&(r[ir].Mtotal),
                	&(r[ir].Maj.T.az),
                	&(r[ir].Maj.T.pl),
                	&(r[ir].Maj.B.az),
                	&(r[ir].Maj.B.pl),
                	&(r[ir].Maj.P.az),
                	&(r[ir].Maj.P.pl),
			dum,
                	&(r[ir].Maj.P1.s),
                	&(r[ir].Maj.P1.d),
                	&(r[ir].Maj.P1.r),
			dum,
                	&(r[ir].Maj.P2.s),
                	&(r[ir].Maj.P2.d),
                	&(r[ir].Maj.P2.r),
			dum,
                	&(r[ir].mxx),
                	&(r[ir].myy),
                	&(r[ir].mzz),
                	&(r[ir].mxy),
                	&(r[ir].mxz),
                	&(r[ir].myz),
                	&(r[ir].e_sort[0].u), 
			&(r[ir].e_sort[0].lat), 
			&(r[ir].e_sort[0].lon),
                	&(r[ir].e_sort[1].u), 
			&(r[ir].e_sort[1].lat), 
			&(r[ir].e_sort[1].lon),
                	&(r[ir].e_sort[2].u), 
			&(r[ir].e_sort[2].lat), 
			&(r[ir].e_sort[2].lon),
                	&(r[ir].pdc),
                	&(r[ir].pclvd),
                	&(r[ir].piso),
                	&(r[ir].gcarc_dc),
                	&(r[ir].gcarc_exp),
                	&(r[ir].nevec) );
			
		ir++;
	}
	fclose(fp);
	*nsim_eig = ir;

	fprintf( stderr, "%s: %s: %s: nsim_eig = %d read from file = %s\n", progname, __FILE__, __func__, *nsim_eig, filename );

	/* print_results_all( r, ir, "test.out" ); */

	return((Results *)r);
}

void print_results_lune2mt( Results *r, int nrows, char *filename )
{
  FILE *fp; 
  int ir;
	float pvr = 0.0;

  fp = fopen( filename, "w" );
	
  fprintf( fp, "lune_lon,lune_lat,pvr,pdc,pclvd,piso,Mo,mxx,myy,mzz,mxy,mxz,myz,eig1,eig2,eig3\n" );

  for( ir = 0; ir < nrows; ir++ )
  {
	/*            1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16  */
	fprintf( fp, "%.3f,%.3f,%.2f,%.3f,%.3f,%.3f,%.3e,%.3e,%.3e,%.3e,%.3e,%.3e,%.3e,%.3e,%.3e,%.3e\n", 
		r[ir].lune_lon,
		r[ir].lune_lat,
		pvr,
		r[ir].pdc,
                r[ir].pclvd,
                r[ir].piso,
		r[ir].Mtotal,
		r[ir].mxx,
                r[ir].myy,
                r[ir].mzz,
                r[ir].mxy,
                r[ir].mxz,
                r[ir].myz,
		(r[ir].e_sort[0].u/r[ir].Mtotal), 
                (r[ir].e_sort[1].u/r[ir].Mtotal), 
                (r[ir].e_sort[2].u/r[ir].Mtotal) );
  }
}

void print_results_all( Results *r, int nrows, char *filename )
{
	FILE *fp;
	int ir;
	fp = fopen( filename, "w" );
	for( ir = 0; ir < nrows; ir++ )
	{
		fprintf( fp,
/*1   2      3      4     5     6     7     8     9     10    11    12    13  14    15    16    17  18    19    20   21    22    23    24    25    26    27    28     29    30    31    32    33    34      35    36    37    38    39   40   41    42 */
/*lon lat    vred   k     T     Mo    Taz   Tpl   Baz   Bpl   Paz   Ppl   P1: S     D     R     P2: S     D     R     Mij: Mxx   Myy   Mzz   Mxy   Mxz   Myz   Pev    Plat  Plon  Tev   Tlat  Tlon  Bev     Blat  Blon  pdc   pclvd piso  gcarc_dc  gcarc_exp nevec */
"%8.3f %7.3f %11.3f %5.2f %5.2f %5.2e %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f P1: %3.0f %2.0f %4.0f P2: %3.0f %2.0f %4.0f Mij: %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %+5.2f %3.0f %3.0f %+5.2f %3.0f %3.0f %+5.2f %3.0f %3.0f %5.2f %5.2f %5.2f %8.4f %8.4f %9d\n",
                r[ir].lune_lon,
                r[ir].lune_lat,
                r[ir].var_red,
                r[ir].k,
                r[ir].T,
                r[ir].Mtotal,
                r[ir].Maj.T.az,
                r[ir].Maj.T.pl,
                r[ir].Maj.B.az,
                r[ir].Maj.B.pl,
                r[ir].Maj.P.az,
                r[ir].Maj.P.pl,
                r[ir].Maj.P1.s,
                r[ir].Maj.P1.d,
                r[ir].Maj.P1.r,
                r[ir].Maj.P2.s,
                r[ir].Maj.P2.d,
                r[ir].Maj.P2.r,
                r[ir].mxx,
                r[ir].myy,
                r[ir].mzz,
                r[ir].mxy,
                r[ir].mxz,
                r[ir].myz,
                (r[ir].e_sort[0].u/r[ir].Mtotal), r[ir].e_sort[0].lat, r[ir].e_sort[0].lon,
                (r[ir].e_sort[1].u/r[ir].Mtotal), r[ir].e_sort[1].lat, r[ir].e_sort[1].lon,
                (r[ir].e_sort[2].u/r[ir].Mtotal), r[ir].e_sort[2].lat, r[ir].e_sort[2].lon,
                r[ir].pdc,
                r[ir].pclvd,
                r[ir].piso, 
		r[ir].gcarc_dc, 
		r[ir].gcarc_exp,
		r[ir].nevec );
	}
	fclose(fp);
}

/********************************************************************************/
/********************************************************************************/
/**** prints just one row ***/
/********************************************************************************/
/********************************************************************************/

void print_results( Results *r, FILE *fp )
{
	fprintf( fp,
/*1   2      3      4     5     6     7     8     9     10    11    12    13  14    15    16    17  18    19    20   21    22    23    24    25    26    27    28     29    30    31    32    33    34      35    36    37    38    39   40 41 42  43       44        45 */
/*lon lat    vred   k     T     Mo    Taz   Tpl   Baz   Bpl   Paz   Ppl   P1: S     D     R     P2: S     D     R     Mij: Mxx   Myy   Mzz   Mxy   Mxz   Myz   Pev    Plat  Plon  Tev   Tlat  Tlon  Bev     Blat  Blon  pdc   pclvd piso e0 e1 e2  gcarc_dc gcarc_exp nevec */
/******************************************************************************************************************************************************************************************************************************************************************************/
/* lon   lat   vred     k     T  Mtot   Taz   Tpl   Baz   Bpl   Paz   Ppl      stk1  dip1  rak1      stk2  dip2  rak2        Mxx   Myy   Mzz   Mxy   Mxz   Myz   eval1  lat   lon  eval2  lat   lon   eval3  lat    lon   pdc  pclvd piso    e0    e1    e2  dc   exp   nevec */
/*   1     2      3     4     5     6     7     8     9    10    11    12        13    14    15        16    17    18         19    20    21    22    23    24     25    26    27     28    29    30     31    32    33    34    35    36    37    38    39  40   41    42  */
"%8.3f %7.3f %11.3f %5.2f %5.2f %5.2e %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f P1: %3.0f %2.0f %4.0f P2: %3.0f %2.0f %4.0f Mij: %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %+5.2f %3.0f %3.0f %+5.2f %3.0f %3.0f %+5.2f %3.0f %3.0f %5.2f %5.2f %5.2f %6.3f %6.3f %6.3f %8.3f %8.3f %9d\n",
		r->lune_lon, /* 1 */
		r->lune_lat,
		r->var_red,
		r->k,	
		r->T,         /* 5 */
		r->Mtotal,
		/* r->FullMT.eig[1].val, r->FullMT.eig[2].val, r->FullMT.eig[3].val, */
		r->Maj.T.az,
		r->Maj.T.pl,
		r->Maj.B.az,
		r->Maj.B.pl,  /* 10 */
		r->Maj.P.az,
		r->Maj.P.pl,
		r->Maj.P1.s,   /* 13 */
		r->Maj.P1.d,
		r->Maj.P1.r,   /* 15 */
		r->Maj.P2.s,
		r->Maj.P2.d,
		r->Maj.P2.r,
		r->mxx,
		r->myy,         /* 20 */
		r->mzz,
                r->mxy,
		r->mxz,
                r->myz,           /* 24 */
		(r->e_sort[0].u/r->Mtotal), r->e_sort[0].lat, r->e_sort[0].lon, 
		(r->e_sort[1].u/r->Mtotal), r->e_sort[1].lat, r->e_sort[1].lon,
		(r->e_sort[2].u/r->Mtotal), r->e_sort[2].lat, r->e_sort[2].lon,
		r->pdc,            /* 34 */
		r->pclvd,         /* 35 */
		r->piso,
		r->e0,
		r->e1,
		r->e2,
		r->gcarc_dc,       /* 40 */
		r->gcarc_exp,
		r->nevec );       /* 42 */
	fflush(fp);
}

void transfer_solution_v2(
		float var_red, float Mo, Tensor M,
		Results *r, int ir,
		Results *rbest, Eigenvalue *e, Eigenvalue *e_sort )
{
	r[ir].nevec    = rbest->nevec;

	r[ir].var_red  = var_red;
        r[ir].lune_lat = rbest->lune_lat;
        r[ir].lune_lon = rbest->lune_lon;
        r[ir].k        = rbest->k;
        r[ir].T        = rbest->T;
        r[ir].Mtotal   = rbest->Mtotal;
        r[ir].pdc      = rbest->pdc;
        r[ir].pclvd    = rbest->pclvd;
        r[ir].piso     = rbest->piso;
        r[ir].pdev     = rbest->pdev;

	r[ir].gcarc_dc = rbest->gcarc_dc;
	r[ir].gcarc_exp= rbest->gcarc_exp;

        r[ir].mxx      = M.xx/Mo;
        r[ir].mxy      = M.xy/Mo;
        r[ir].mxz      = M.xz/Mo;
        r[ir].myy      = M.yy/Mo;
        r[ir].myz      = M.yz/Mo;
        r[ir].mzz      = M.zz/Mo;

	r[ir].MT.xx    = M.xx;
	r[ir].MT.xy    = M.xy;
	r[ir].MT.xz    = M.xz;
	r[ir].MT.yy    = M.yy;
	r[ir].MT.yz    = M.yz;
	r[ir].MT.zz    = M.zz;

	r[ir].e0       = rbest->e0;
	r[ir].e1       = rbest->e1;
	r[ir].e2       = rbest->e2;

	r[ir].FullMT.eig[1].val = e[0].u / Mo;
	r[ir].FullMT.eig[2].val = e[1].u / Mo;
	r[ir].FullMT.eig[3].val = e[2].u / Mo;
	r[ir].FullMT.eig[1].vec[1] = e[0].v[0];
	r[ir].FullMT.eig[1].vec[2] = e[0].v[1];
	r[ir].FullMT.eig[1].vec[3] = e[0].v[2];
	r[ir].FullMT.eig[2].vec[1] = e[1].v[0];
	r[ir].FullMT.eig[2].vec[2] = e[1].v[1];
	r[ir].FullMT.eig[2].vec[3] = e[1].v[2];
	r[ir].FullMT.eig[3].vec[1] = e[2].v[0];
	r[ir].FullMT.eig[3].vec[2] = e[2].v[1];
	r[ir].FullMT.eig[3].vec[3] = e[2].v[2];

	r[ir].Maj.T.ev = 0;
	r[ir].Maj.T.az = 0;
	r[ir].Maj.T.pl = 0;
	r[ir].Maj.P.ev = 0;
	r[ir].Maj.P.az = 0;
	r[ir].Maj.P.pl = 0;
	r[ir].Maj.B.ev = 0;
	r[ir].Maj.B.az = 0;
	r[ir].Maj.B.pl = 0;
	r[ir].Maj.P1.s = 0;
	r[ir].Maj.P1.d = 0;
	r[ir].Maj.P1.r = 0;
	r[ir].Maj.P2.s = 0;
	r[ir].Maj.P2.d = 0;
	r[ir].Maj.P2.r = 0;

	r[ir].e[0].u    = e[0].u;
	r[ir].e[0].v[0] = e[0].v[0];
	r[ir].e[0].v[1] = e[0].v[1];
	r[ir].e[0].v[2] = e[0].v[2];
	r[ir].e[0].lat  = e[0].lat;
	r[ir].e[0].lon  = e[0].lon;
	
	r[ir].e[1].u    = e[1].u;
	r[ir].e[1].v[0] = e[1].v[0];
	r[ir].e[1].v[1] = e[1].v[1];
	r[ir].e[1].v[2] = e[1].v[2];
	r[ir].e[1].lat  = e[1].lat;
	r[ir].e[1].lon  = e[1].lon;
	
	r[ir].e[2].u    = e[2].u;
	r[ir].e[2].v[0] = e[2].v[0];
	r[ir].e[2].v[1] = e[2].v[1];
	r[ir].e[2].v[2] = e[2].v[2];
	r[ir].e[2].lat  = e[2].lat;
	r[ir].e[2].lon  = e[2].lon;
	
	r[ir].e_sort[0].u    = e_sort[0].u;
	r[ir].e_sort[0].v[0] = e_sort[0].v[0];
	r[ir].e_sort[0].v[1] = e_sort[0].v[1];
	r[ir].e_sort[0].v[2] = e_sort[0].v[2];
	r[ir].e_sort[0].lat  = e_sort[0].lat;
	r[ir].e_sort[0].lon  = e_sort[0].lon;
	
	r[ir].e_sort[1].u    = e_sort[1].u;
	r[ir].e_sort[1].v[0] = e_sort[1].v[0];
	r[ir].e_sort[1].v[1] = e_sort[1].v[1];
	r[ir].e_sort[1].v[2] = e_sort[1].v[2];
	r[ir].e_sort[1].lat  = e_sort[1].lat;
	r[ir].e_sort[1].lon  = e_sort[1].lon;
	
	r[ir].e_sort[2].u    = e_sort[2].u;
	r[ir].e_sort[2].v[0] = e_sort[2].v[0];
	r[ir].e_sort[2].v[1] = e_sort[2].v[1];
	r[ir].e_sort[2].v[2] = e_sort[2].v[2];
	r[ir].e_sort[2].lat  = e_sort[2].lat;
	r[ir].e_sort[2].lon  = e_sort[2].lon;
}

/********************************************************************************/
/********************************************************************************/
/*** transfers mtinv Solution *sol data structure to Results *r data structure***/
/*** Solution data structure is an array as a function depths z[iz]           ***/
/********************************************************************************/
/********************************************************************************/

void transfer_solution( Results *r, int k, Solution *sol, int iz, Eigenvalue *e, Eigenvalue *e_sort )
{
	r[k].var_red  = sol[iz].var_red;
        r[k].lune_lat = sol[iz].lune_lat;
        r[k].lune_lon = sol[iz].lune_lon;
        r[k].k        = sol[iz].k;
        r[k].T        = sol[iz].T;
        r[k].Mtotal   = sol[iz].Mtotal;

	r[k].pdc      = sol[iz].PDC/100.;
        r[k].pclvd    = sol[iz].PCLVD/100.;
        r[k].piso     = sol[iz].PISO/100.;
	r[k].pdev     = sol[iz].PDEV/100.;

/*** set in mtinv_subs.c: eig2iso() ****/
        r[k].FullMT.eig[1].val = e[0].u / sol[iz].Mtotal;
        r[k].FullMT.eig[2].val = e[1].u / sol[iz].Mtotal;
        r[k].FullMT.eig[3].val = e[2].u / sol[iz].Mtotal;
	
        r[k].FullMT.eig[1].vec[1] = e[0].v[0];
        r[k].FullMT.eig[1].vec[2] = e[0].v[1];
        r[k].FullMT.eig[1].vec[3] = e[0].v[2];

        r[k].FullMT.eig[2].vec[1] = e[1].v[0];
        r[k].FullMT.eig[2].vec[2] = e[1].v[1];
        r[k].FullMT.eig[2].vec[3] = e[1].v[2];

        r[k].FullMT.eig[3].vec[1] = e[2].v[0];
        r[k].FullMT.eig[3].vec[2] = e[2].v[1];
        r[k].FullMT.eig[3].vec[3] = e[2].v[2];
	
	r[k].Maj.T.ev = sol[iz].Maj.T.ev;
	r[k].Maj.T.az = sol[iz].Maj.T.az; 
	r[k].Maj.T.pl = sol[iz].Maj.T.pl;

	r[k].Maj.P.ev = sol[iz].Maj.P.ev;
	r[k].Maj.P.az = sol[iz].Maj.P.az;
	r[k].Maj.P.pl = sol[iz].Maj.P.pl;

	r[k].Maj.B.ev = sol[iz].Maj.B.ev;
	r[k].Maj.B.az = sol[iz].Maj.B.az;
	r[k].Maj.B.pl = sol[iz].Maj.B.pl;
 
	r[k].Maj.P1.s = sol[iz].Maj.P1.s;
	r[k].Maj.P1.d = sol[iz].Maj.P1.d;
        r[k].Maj.P1.r = sol[iz].Maj.P1.r;

        r[k].Maj.P2.s = sol[iz].Maj.P2.s;
        r[k].Maj.P2.d = sol[iz].Maj.P2.d;
        r[k].Maj.P2.r = sol[iz].Maj.P2.r;

        r[k].mxx      = sol[iz].mxx;
        r[k].mxy      = sol[iz].mxy;
        r[k].mxz      = sol[iz].mxz;
        r[k].myy      = sol[iz].myy;
        r[k].myz      = sol[iz].myz;
        r[k].mzz      = sol[iz].mzz;

	r[k].e[0].u    = e[0].u;
        r[k].e[0].v[0] = e[0].v[0];
        r[k].e[0].v[1] = e[0].v[1];
        r[k].e[0].v[2] = e[0].v[2];
        r[k].e[0].lat  = e[0].lat;
        r[k].e[0].lon  = e[0].lon;

        r[k].e[1].u    = e[1].u;
        r[k].e[1].v[0] = e[1].v[0];
        r[k].e[1].v[1] = e[1].v[1];
        r[k].e[1].v[2] = e[1].v[2];
        r[k].e[1].lat  = e[1].lat;
        r[k].e[1].lon  = e[1].lon;

        r[k].e[2].u    = e[2].u;
        r[k].e[2].v[0] = e[2].v[0];
        r[k].e[2].v[1] = e[2].v[1];
        r[k].e[2].v[2] = e[2].v[2];
        r[k].e[2].lat  = e[2].lat;
        r[k].e[2].lon  = e[2].lon;

        r[k].e_sort[0].u    = e_sort[0].u;
        r[k].e_sort[0].v[0] = e_sort[0].v[0];
        r[k].e_sort[0].v[1] = e_sort[0].v[1];
        r[k].e_sort[0].v[2] = e_sort[0].v[2];
        r[k].e_sort[0].lat  = e_sort[0].lat;
        r[k].e_sort[0].lon  = e_sort[0].lon;

        r[k].e_sort[1].u    = e_sort[1].u;
        r[k].e_sort[1].v[0] = e_sort[1].v[0];
        r[k].e_sort[1].v[1] = e_sort[1].v[1];
        r[k].e_sort[1].v[2] = e_sort[1].v[2];
        r[k].e_sort[1].lat  = e_sort[1].lat;
        r[k].e_sort[1].lon  = e_sort[1].lon;

        r[k].e_sort[2].u    = e_sort[2].u;
        r[k].e_sort[2].v[0] = e_sort[2].v[0];
        r[k].e_sort[2].v[1] = e_sort[2].v[1];
        r[k].e_sort[2].v[2] = e_sort[2].v[2];
        r[k].e_sort[2].lat  = e_sort[2].lat;
        r[k].e_sort[2].lon  = e_sort[2].lon;
}

/********************************************************************************/
/********************************************************************************/
void Eig2MT( Eigenvalue *e, Tensor *M )
{
        M->xx = e[0].u * ( e[0].v[0] * e[0].v[0] ) + e[1].u * ( e[1].v[0] * e[1].v[0] ) + e[2].u * ( e[2].v[0] * e[2].v[0] );
        M->yy = e[0].u * ( e[0].v[1] * e[0].v[1] ) + e[1].u * ( e[1].v[1] * e[1].v[1] ) + e[2].u * ( e[2].v[1] * e[2].v[1] );
        M->zz = e[0].u * ( e[0].v[2] * e[0].v[2] ) + e[1].u * ( e[1].v[2] * e[1].v[2] ) + e[2].u * ( e[2].v[2] * e[2].v[2] );
        M->xy = e[0].u * ( e[0].v[0] * e[0].v[1] ) + e[1].u * ( e[1].v[0] * e[1].v[1] ) + e[2].u * ( e[2].v[0] * e[2].v[1] );
        M->xz = e[0].u * ( e[0].v[0] * e[0].v[2] ) + e[1].u * ( e[1].v[0] * e[1].v[2] ) + e[2].u * ( e[2].v[0] * e[2].v[2] );
        M->yz = e[0].u * ( e[0].v[1] * e[0].v[2] ) + e[1].u * ( e[1].v[1] * e[1].v[2] ) + e[2].u * ( e[2].v[1] * e[2].v[2] );
        M->yx = M->xy;
        M->zx = M->xz;
        M->zy = M->yz;
}

/********************************************************************************/
/********************************************************************************/
void scalevec( float *x, float sc )
{
        x[0] *= sc;
        x[1] *= sc;
        x[2] *= sc;
}

/********************************************************************************/
/********************************************************************************/
float normvec( float *x )
{
        float sum = 0;
        sum = sqrt( (x[0]*x[0]) + (x[1]*x[1]) + (x[2]*x[2]) );
        x[0] /= sum;
        x[1] /= sum;
        x[2] /= sum;
        return sum;
}


/********************************************************************************/
/********************************************************************************/
void set_solution( Solution *sol, MomentTensor Ma, MomentTensor Mn )
{
        sol->mw       = Ma.Mw;
        sol->exponent = Ma.expon;
        sol->abcassa  = Ma.abcassa;

        sol->FullMT.eig[1].val /= pow(10.0, sol->exponent );
        sol->FullMT.eig[2].val /= pow(10.0, sol->exponent );
        sol->FullMT.eig[3].val /= pow(10.0, sol->exponent );

        sol->Dev.eig[1].val /= pow(10.0, sol->exponent );
        sol->Dev.eig[2].val /= pow(10.0, sol->exponent );
        sol->Dev.eig[3].val /= pow(10.0, sol->exponent );

        sol->FullMT.T.ev /= pow(10.0, sol->exponent );
        sol->FullMT.B.ev /= pow(10.0, sol->exponent );
        sol->FullMT.P.ev /= pow(10.0, sol->exponent );

        sol->mrr = Mn.rr;
        sol->mtt = Mn.tt;
        sol->mff = Mn.ff;
        sol->mrt = Mn.rt;
        sol->mrf = Mn.rf;
        sol->mtf = Mn.tf;

        sol->mxx = Mn.xx;
        sol->mxy = Mn.xy;
        sol->mxz = Mn.xz;
        sol->myy = Mn.yy;
        sol->myz = Mn.yz;
        sol->mzz = Mn.zz;
}

/********************************************************************************/
/********************************************************************************/
void random_sphere( float *x, int seed )
{
	float theta, u;
	static float twopi = 6.283185307179586;
/*
	float random_float( int seed, float min, float max );
	theta = random_float( seed,  0.0, twopi );
	u     = random_float( seed, -1.0, +1.0 );
*/

	if(seed>0)
	{
		/*** srandomdev() is a BSD unix, not portable to Linux ***/
		/* srandomdev();  */
		theta = twopi * ( (float) random() / (RAND_MAX) );          /* 0 to 2pi */
		u     = 2.0 * ( ( (float) random() / (RAND_MAX) ) - 0.5 );  /* -1 to +1 */
	}
	else
	{
		/* sranddev(); */
		theta = twopi * ( (float) rand() / (RAND_MAX) );          /* 0 to 2pi */
                u     = 2.0 * ( ( (float) rand() / (RAND_MAX) ) - 0.5 );  /* -1 to +1 */
	}

	x[0]  = sqrt(1 - (u*u)) * cos(theta);
	x[1]  = sqrt(1 - (u*u)) * sin(theta);
	x[2]  = u;
}

/********************************************************************************/
/********************************************************************************/
void randomEigVec( Eigenvalue *e, int seed, int verbose )
{
	int i, n = 3;
	float x[3] = {0, 0, 0};
	float y[3] = {0, 0, 0};
	float z[3] = {0, 0, 0};

/*** subroutines ***/

	float R[3][3] = { {0,0,0}, {0,0,0}, {0,0,0} };
	void rand_rotation( float R[3][3], int seed, int verbose );

	float lon=0, lat=0, xtmp[3] = {0,0,0};
	void latlon2vec( float longitude_deg, float latitude_deg, float *x );
	void vec2latlon( float *longitude_deg, float *latitude_deg, float *x );

/*** start subroutine ***/

	rand_rotation( R, seed, verbose );

	x[0] = R[0][0];
	x[1] = R[0][1];
	x[2] = R[0][2];

	y[0] = R[1][0];
	y[1] = R[1][1];
	y[2] = R[1][2];

	z[0] = R[2][0];
	z[1] = R[2][1];
	z[2] = R[2][2];

/*** first eigenvalue ***/

	e[0].v[0] = x[0];
	e[0].v[1] = x[1];
	e[0].v[2] = x[2];
	
	vec2latlon( &lon, &lat, x );
	e[0].lat = lat;
	e[0].lon = lon;

	if(verbose)
	{
		latlon2vec( lon, lat, xtmp );
		fprintf( stdout, "%s: %s: %s: e=%+5.2e lon=%4.0f lat=%3.0f x=(%+5.2f,%+5.2f,%+5.2f) xtmp=(%+5.2f,%+5.2f,%+5.2f)\n",
                	progname, __FILE__, __func__, 
			e[0].u, lon, lat, 
			x[0], x[1], x[2],
			xtmp[0], xtmp[1], xtmp[2] );
		fflush(stdout);
	}

/*** second eigenvalue ***/

        e[1].v[0] = y[0];
        e[1].v[1] = y[1];
        e[1].v[2] = y[2];

	vec2latlon( &lon, &lat, y );
	e[1].lat = lat;
	e[1].lon = lon;

	if(verbose)
	{
		latlon2vec( lon, lat, xtmp );
                fprintf( stdout, "%s: %s: %s: e=%+5.2e lon=%4.0f lat=%3.0f y=(%+5.2f,%+5.2f,%+5.2f) xtmp=(%+5.2f,%+5.2f,%+5.2f)\n",
                        progname, __FILE__, __func__, 
			e[1].u, lon, lat,
                        y[0], y[1], y[2],
                        xtmp[0], xtmp[1], xtmp[2] );
		fflush(stdout);
	}

/*** third eigenvalue ***/

	e[2].v[0] = z[0];
        e[2].v[1] = z[1];
        e[2].v[2] = z[2];

	vec2latlon( &lon, &lat, z );
	e[2].lat = lat;
	e[2].lon = lon;

	if(verbose)
	{
        	latlon2vec( lon, lat, xtmp );
                fprintf( stdout, "%s: %s: %s: e=%+5.2e lon=%4.0f lat=%3.0f z=(%+5.2f,%+5.2f,%+5.2f) xtmp=(%+5.2f,%+5.2f,%+5.2f)\n",
                        progname, __FILE__, __func__, 
			e[2].u, lon, lat,
                        z[0], z[1], z[2],
                        xtmp[0], xtmp[1], xtmp[2] );
		fflush(stdout);
	}

	if(verbose)
	{
	  for( i = 0; i < n; i++ )
	  {
	     fprintf( stdout, "%s: %s: %s: i=%d u=%5.2f v1=%+5.2f v2=%+5.2f v3=%+5.2f\n",
		progname, __FILE__, __func__,
		i, e[i].u, e[i].v[0], e[i].v[1], e[i].v[2] );
	    fflush(stdout);
	  }
	}
}

/********************************************************************************
 Compute the Cross product of two vectors x and y 
 | i  j  k  |
 | x0 x1 x2 |   | x1 x2 |     | x0 x2 |     | x0 x1 |
 | y0 y1 y2 |   | y1 y2 |     | y0 y2 |     | y0 y1 |

                ( x1*y2 - y1*x2 )i - ( x0*y2 - y0*x2 )j + ( x0*y1 - x1*y0 )k
                ( x1*y2 - y1*x2 )i + ( y0*x2 - x0*y2 )j + ( x0*y1 - x1*y0 )k
 ********************************************************************************/

void cross_product( float *x, float *y, float *z )
{
	z[0] = ( x[1] * y[2] ) - ( x[2] * y[1] );
        z[1] = ( x[2] * y[0] ) - ( x[0] * y[2] );
        z[2] = ( x[0] * y[1] ) - ( x[1] * y[0] );
}

/********************************************************************************/
/********************************************************************************/
void scaleTensor( Tensor *M, float scale )
{
        M->xx *= scale;
        M->yy *= scale;
        M->zz *= scale;
        M->xy *= scale;
        M->xz *= scale;
        M->yz *= scale;
        M->yx = M->xy;
        M->zx = M->xz;
        M->zy = M->yz;
}

/********************************************************************************/
/********************************************************************************/
void writeTensor( Tensor *M, char *label )
{
        fprintf( stdout, "%s\n", label );
        fprintf( stdout, "\t %6.2e %6.2e %6.2e\n", M->xx, M->xy, M->xz );
        fprintf( stdout, "\t %6.2e %6.2e %6.2e\n", M->yx, M->yy, M->yz );
        fprintf( stdout, "\t %6.2e %6.2e %6.2e\n", M->zx, M->zy, M->zz );
        fprintf( stdout, "\n" );
	fflush(stdout);
}

/********************************************************************************/
/********************************************************************************/
void floorTensor( Tensor *M, float tol )
{
        if( fabs(M->xx) < tol ) M->xx = 0;
        if( fabs(M->xy) < tol ) M->xy = 0;
        if( fabs(M->xz) < tol ) M->xz = 0;
        if( fabs(M->yy) < tol ) M->yy = 0;
        if( fabs(M->yz) < tol ) M->yz = 0;
        if( fabs(M->zz) < tol ) M->zz = 0;
        M->yx = M->xy;
        M->zx = M->xz;
        M->zy = M->yz;
}

/********************************************************************************/
/*** given lat, lon of point on sphere, return the unit vector ***/
/********************************************************************************/
void latlon2vec( float longitude_deg, float latitude_deg, float *x )
{
	float latrad, lonrad;
	static float deg2rad = 0.017453292519943295; /* M_PI/180 */
	static float rad2deg = 57.29577951308232;    /* 180/M_PI; */

	latrad = latitude_deg  * deg2rad;
	lonrad = longitude_deg * deg2rad;
	x[0] = cos( latrad ) * cos( lonrad );
        x[1] = cos( latrad ) * sin( lonrad );
        x[2] = sin( latrad ); 
}

/********************************************************************************/
/*** given a unit vector return the point on sphere ***/
/********************************************************************************/
void vec2latlon( float *longitude_deg, float *latitude_deg, float *x )
{
	float latrad, lonrad; 
	float normvec( float *x );
	float dot( float *x, float *y );

	static float deg2rad = 0.017453292519943295; /* M_PI/180 */
        static float rad2deg = 57.29577951308232;    /* 180/M_PI; */

	normvec( x );

	lonrad = atan2( x[1] , x[0] );
	latrad = asin( x[2] );

	*longitude_deg = lonrad * rad2deg;
	if( *longitude_deg < 0 ) *longitude_deg += 360;

	*latitude_deg  = latrad * rad2deg;
}

/********************************************************************************/
/*** dot product of two vectors ***/
/********************************************************************************/
float dot( float *x, float *y )
{
	return ( x[0]*y[0] + x[1]*y[1] + x[2]*y[2] );
}

/********************************************************************************/
/*=========================================================================*
 *  R A N D _ R O T A T I O N      Author: Jim Arvo, 1991                  *
 *                                                                         *
 *  This routine maps three values (x[0], x[1], x[2]) in the range [0,1]   *
 *  into a 3x3 rotation matrix, M.  Uniformly distributed random variables *
 *  x0, x1, and x2 create uniformly distributed random rotation matrices.  *
 *  To create small uniformly distributed "perturbations", supply          *
 *  samples in the following ranges                                        *
 *                                                                         *
 *      x[0] in [ 0, d ]                                                   *
 *      x[1] in [ 0, 1 ]                                                   *
 *      x[2] in [ 0, d ]                                                   *
 *                                                                         *
 * where 0 < d < 1 controls the size of the perturbation.  Any of the      *
 * random variables may be stratified (or "jittered") for a slightly more  *
 * even distribution.                                                      *
 *                                                                         *
 *=========================================================================*/

void rand_rotation( float R[3][3], int seed, int verbose )
{
	float u[3];
	float theta, phi, z;
	float r, Vx, Vy, Vz;
	float st, ct, Sx, Sy;
	static float twopi = 6.283185307179586;
	float random_float( int seed, float min, float max );

	u[0] = random_float( seed, 0.0, 1.0 );
	u[1] = random_float( seed, 0.0, 1.0 );
	u[2] = random_float( seed, 0.0, 1.0 );
	
	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: u0=%g u1=%g u2=%g\n",
		progname, __FILE__, __func__, u[0], u[1], u[2] );
	  fflush(stdout);
	}

    /*==================================================================*/
    /* Compute a vector V used for distributing points over the sphere  */
    /* via the reflection I - V Transpose(V).  This formulation of V    */
    /* will guarantee that if x[1] and x[2] are uniformly distributed,  */
    /* the reflected points will be uniform on the sphere.  Note that V */
    /* has length sqrt(2) to eliminate the 2 in the Householder matrix. */
    /*==================================================================*/

	theta = u[0] * twopi;
	phi   = u[1] * twopi;
	z     = u[2] * 2;

    /*==================================================================*/
    /* Compute the row vector S = Transpose(V) * R, where R is a simple */
    /* rotation by theta about the z-axis.  No need to compute Sz since */
    /* it's just Vz.                                                    */
    /*==================================================================*/

	r = sqrt( z );
	Vx = sin( phi ) * r;
	Vy = cos( phi ) * r;
	Vz = sqrt( 2.0 - z );

	st = sin( theta );
	ct = cos( theta );
	Sx = Vx * ct - Vy * st;
	Sy = Vx * st + Vy * ct;

   /*==================================================================*/
   /* Construct the rotation matrix  ( V Transpose(V) - I ) R,         */
   /* which  is equivalent to V S - R.                                 */
   /*==================================================================*/

	R[0][0] = Vx * Sx - ct;
	R[0][1] = Vx * Sy - st;
	R[0][2] = Vx * Vz;

	R[1][0] = Vy * Sx + st;
	R[1][1] = Vy * Sy - ct;
	R[1][2] = Vy * Vz;
	
	R[2][0] = Vz * Sx;
	R[2][1] = Vz * Sy;
	R[2][2] = 1 - z;  /*** A.K.A ( Vz * Vz - 1 ) ***/

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: R: \n", progname, __FILE__, __func__ );
  fprintf( stdout, " R00=%+5.2f R01=%+5.2f R02=%+5.2f\n R10=%+5.2f R11=%+5.2f R12=%+5.2f\n R20=%+5.2f R21=%+5.2f R22=%+5.2f\n",
		R[0][0], R[0][1], R[0][2],
		R[1][0], R[1][1], R[1][2],
		R[2][0], R[2][1], R[2][2] );
	  fflush(stdout);
	}
}

/***
  *** This is like createlambda but user supplies eigenvalues ***
***/

void eig2eig( float *lamb1, float *lamb2, float *lamb3,
              float *percent_dc, float *percent_clvd, float *percent_iso, 
              float *lune_lat, float *lune_lon, float *hudson_k, float *hudson_T, 
		float *gcarc_dc, float *gcarc_exp, int *nevec, int eigvec_fac, int verbose )
{
	float pdc = 0, pclvd = 0, piso = 0;
	 int i, n = 3;
	float e[3] = { 0, 0, 0 };
	float sum = 0;

/*** acording to Julian et al., 1998; Hudson et al. 1989 ***/

        float k = 0, T = 0;
        float Mv = 0, Mclvd = 0, Mdc = 0;
        float dev[3]  = { 0, 0, 0 };
        void abs_sort( float *e, int verbose );
        int indx[4] = {1, 2, 3, 4 };
        void flt_sort( float *e, int *indx, char *sorttype, int verbose );
/*** eigenvalues on a sphere, fundamental lune, Tape&Tape,2012 ***/

        float gamma=0, beta=0, norm=0;
        float latitude=0, longitude=0;
        void  eig2lune( float *eig, float *latitude, float *longitude, int verbose );
        float random_float( int seed, float min, float max );
        float normvec( float *x );
        void scalevec( float *x, float sc );
        void random_sphere( float *x, int seed );

	float distrad;
	float kappa = 1.3;
	float distradians_gcp( float lat0, float lon0, float lat1, float lon1 );
        float vonMises1D_pdf( float kappa, float dist_rads, float eigvec_fac );
        float vonMisesFisher3d_pdf( float kappa, float dist_rads, float eigvec_fac );

	int distaz( double olat, double olon, double tlat, double tlon,
                    double *del, double *az, double *baz );
        double ddistdeg, daz, dbaz;

        static float deg2rad = 0.017453292519943295; /* M_PI/180 */
        static float rad2deg = 57.29577951308232;    /* 180/M_PI; */

/*** start subroutine ***/
	e[0] = *lamb1;
        e[1] = *lamb2;
        e[2] = *lamb3;
	abs_sort( e, verbose );

/*** compute source types hudson plot ***/
/*** find scaler to scale to seismic moment  = 1.0 ***/

        Mv    = (e[0] + e[1] + e[2])/3.0;
        for( i = 0; i < n; i++ ) dev[i] = e[i] - Mv;
        abs_sort( dev, verbose );
        Mclvd = -2.0 * dev[0];
        Mdc   = dev[0] - dev[1];

        sum = fabs(Mdc) + fabs(Mclvd) + fabs(Mv);
        scalevec( e, 1.0/sum );

/*** recalculate source types hudson plot with correct unit scaler moment ***/

        Mv    = (e[0] + e[1] + e[2])/3.0;
        for( i = 0; i < n; i++ ) dev[i] = e[i] - Mv;
        abs_sort( dev, verbose );
        Mclvd = -2.0 * dev[0];
        Mdc   = dev[0] - dev[1];

        k   = Mv / ( fabs(Mv) + fabs(dev[2]) );
        T   = 2.0 * ( -dev[0] / fabs(dev[2]) );

/*** return eigenvalues, hudson_et_al src types k,T and percent moments ***/
/*** Mdc, Mclvd, Mv correctly scaled so total Mo == 1.0 can be used as percent Mo ***/

        *percent_dc   = fabs(Mdc);
        *percent_clvd = fabs(Mclvd);
        *percent_iso  = fabs(Mv);

        *lamb1 = e[0];
        *lamb2 = e[1];
        *lamb3 = e[2];

        *hudson_k = k;
	*hudson_T = T;

        if(verbose)
        {
          fprintf( stdout, "%s: %s: %s: normalized and sorted by absval e[0]=%g e[1]=%g e[2]=%g\n",
                   progname, __FILE__, __func__, e[0], e[1], e[2] );
          fflush(stdout);
        }

/*** compute source types fundamental lune Tape&Tape ***/
/*** method requires descending reording of eigenvalues ***/

        flt_sort( e, indx, "descending", verbose );

/*** deviatoric MT only ***/
        /* gamma = atanf( ( sqrt(3.0) * dev[1] ) / ( dev[0] - dev[2] ) ); */

/*** full MT only ***/
        gamma = atanf( ( -e[0] + 2*e[1] -e[2] ) / ( sqrt(3.0)*(e[0] - e[2]) ) );
        longitude = gamma * rad2deg;

        norm  = sqrtf( e[0]*e[0] + e[1]*e[1] + e[2]*e[2] );
        beta  = acosf( (e[0] + e[1] + e[2]) / ( sqrt(3) * norm ) );
        latitude  = ( M_PI/2 - beta ) * rad2deg;

        *lune_lat = latitude;
        *lune_lon = longitude;

        /*** check to see if answers are the same ***/
        eig2lune( e, &latitude, &longitude, verbose );

	distaz( (double)0.0, (double)0.0, latitude, longitude, &ddistdeg, &daz, &dbaz );
        *gcarc_dc  = (float) ddistdeg/111.11;

	distrad = distradians_gcp( latitude, longitude, 0.0, 0.0 );
	*nevec = (int)rint( vonMisesFisher3d_pdf( kappa, distrad, eigvec_fac ) );

        distaz( (double)90.0, (double)0.0, latitude, longitude, &ddistdeg, &daz, &dbaz );
        *gcarc_exp = (float) ddistdeg/111.11;

/*** print results ***/
	if(verbose) 
	{
		fprintf( stdout,
	"%s: %s: %s: e=(%+6.3f,%+6.3f,%+6.3f) dev=(%+6.3f,%+6.3f,%+6.3f) Mv=%+6.3f Mclvd=%+6.3f Mdc=%+6.3f k=%+5.2f T=%+5.2f ",
			progname, __FILE__, __func__,
			e[0], e[1], e[2],
                        dev[0], dev[1], dev[2],
                        Mv, Mclvd, Mdc, k, T );

		fprintf( stdout, "lat=%.5f lon=%.5f lat=%.5f lon=%.5f gcarc_dc=%g gcarc_exp=%g\n",
			*lune_lat, *lune_lon, 
			latitude, longitude, 
			*gcarc_dc, *gcarc_exp );

		fflush(stdout);
	}
}

/***************************************************************************************/
/*** CREATELAMB()                                                                    ***/   
/*** inputs:  pdc, pclvd, piso ranges [-1, ..., 0, .., +1]   pdc can be [0, ..., +1] ***/
/*** outputs: eigenvalues | lamb1 | <= | lamb2 | <= | lamb3 |                        ***/
/***************************************************************************************/

void createlamb(
                float *percent_dc, float *percent_clvd, float *percent_iso, 
                float *lamb1, float *lamb2, float *lamb3,
                float *lune_lat, float *lune_lon, float *hudson_k, float *hudson_T,
		float *gcarc_dc, float *gcarc_exp, int seed, int *nevec, int eigvec_fac, int verbose )
{
	float pdc = 0, pclvd = 0, piso = 0;
	int i, n = 3;
	float e[3] = { 0, 0, 0 };
	float sum = 0;
	
/*** acording to Julian et al., 1998; Hudson et al. 1989 ***/

	float k = 0, T = 0;
	float Mv = 0, Mclvd = 0, Mdc = 0;
	float dev[3]  = { 0, 0, 0 };
	void abs_sort( float *e, int verbose );
	int indx[4] = {1, 2, 3, 4 };
	void flt_sort( float *e, int *indx, char *sorttype, int verbose );

/*** eigenvalues on a sphere, fundamental lune, Tape&Tape,2012 ***/

	float gamma=0, beta=0, norm=0;
	float latitude=0, longitude=0;
	void  eig2lune( float *eig, float *latitude, float *longitude, int verbose );
	float random_float( int seed, float min, float max );
	float normvec( float *x );
	void scalevec( float *x, float sc );
	void random_sphere( float *x, int seed );

	float distrad;
	float distradians_gcp( float lat0, float lon0, float lat1, float lon1 );
	float kappa = 1.3;
	float vonMises1D_pdf( float kappa, float dist_rads, float eigvec_fac );
	float vonMisesFisher3d_pdf( float kappa, float dist_rads, float eigvec_fac );

	int distaz( double olat, double olon, double tlat, double tlon,
                    double *del, double *az, double *baz );
        double ddistdeg, daz, dbaz;

	static float deg2rad = 0.017453292519943295; /* M_PI/180 */
        static float rad2deg = 57.29577951308232;    /* 180/M_PI; */

/*** start subroutine ***/
	
/*** eigenvalues on a sphere randomly uniform distribution ****/
	
	random_sphere( e, seed );

/*** sort the eigenvalues ***/

	abs_sort( e, verbose );

/*** compute source types hudson plot ***/
/*** find scaler to scale to seismic moment  = 1.0 ***/

	Mv    = (e[0] + e[1] + e[2])/3.0;
	for( i = 0; i < n; i++ ) dev[i] = e[i] - Mv;
	abs_sort( dev, verbose );
	Mclvd = -2.0 * dev[0];
	Mdc   = dev[0] - dev[1];

	sum = fabs(Mdc) + fabs(Mclvd) + fabs(Mv);
	scalevec( e, 1.0/sum );

/*** recalculate source types hudson plot with correct unit scaler moment ***/

	Mv    = (e[0] + e[1] + e[2])/3.0;
	for( i = 0; i < n; i++ ) dev[i] = e[i] - Mv;
	abs_sort( dev, verbose );
        Mclvd = -2.0 * dev[0];
        Mdc   = dev[0] - dev[1];

        k   = Mv / ( fabs(Mv) + fabs(dev[2]) );
        T   = 2.0 * ( -dev[0] / fabs(dev[2]) );

/*** return eigenvalues, hudson_et_al src types k,T and percent moments ***/
/*** Mdc, Mclvd, Mv correctly scaled so total Mo == 1.0 can be used as percent Mo ***/

	*percent_dc   = fabs(Mdc);
        *percent_clvd = fabs(Mclvd);
        *percent_iso  = fabs(Mv);
        *lamb1 = e[0];
        *lamb2 = e[1];
        *lamb3 = e[2];
	*hudson_k = k;
	*hudson_T = T;

        if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: normalized and sorted by absval e[0]=%g e[1]=%g e[2]=%g\n",
                   progname, __FILE__, __func__, e[0], e[1], e[2] );
	  fflush(stdout);
	}

/*** compute source types fundamental lune Tape&Tape ***/
/*** method requires descending reording of eigenvalues ***/

	flt_sort( e, indx, "descending", verbose );

/*** deviatoric MT only ***/
	/* gamma = atanf( ( sqrt(3.0) * dev[1] ) / ( dev[0] - dev[2] ) ); */

/*** full MT only ***/
	gamma = atanf( ( -e[0] + 2*e[1] -e[2] ) / ( sqrt(3.0)*(e[0] - e[2]) ) );
	longitude = gamma * rad2deg;

	norm  = sqrtf( e[0]*e[0] + e[1]*e[1] + e[2]*e[2] );
	beta  = acosf( (e[0] + e[1] + e[2]) / ( sqrt(3) * norm ) );
	latitude  = ( M_PI/2 - beta ) * rad2deg;

	*lune_lat = latitude;
        *lune_lon = longitude;
	
	/*** check to see if answers are the same ***/
	/* if(verbose) eig2lune( e, &latitude, &longitude, verbose ); */

	distaz( (double)0.0, (double)0.0, latitude, longitude, &ddistdeg, &daz, &dbaz );
	*gcarc_dc  = (float) ddistdeg/111.11;

	distrad = distradians_gcp( latitude, longitude, 0.0, 0.0 );
	*nevec = (int)rint( vonMisesFisher3d_pdf( kappa, distrad, eigvec_fac ) );

	distaz( (double)90.0, (double)0.0, latitude, longitude, &ddistdeg, &daz, &dbaz );
	*gcarc_exp = (float) ddistdeg/111.11;

/*** print results ***/
	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: e=(%+5.2f,%+5.2f,%+5.2f) dev=(%+5.2f,%+5.2f,%+5.2f) Mv=%+5.2f Mclvd=%+5.2f Mdc=%+5.2f k=%+5.2f T=%+5.2f lat=%.5f lon=%.5f\n",
		progname, __FILE__, __func__, e[0], e[1], e[2], dev[0], dev[1], dev[2], Mv, Mclvd, Mdc, k, T, latitude, longitude );
	  fflush(stdout);
	}
}

/**********************************************************************************/
/*** create a random repeatable or non-repeatable floating point number between ***/
/*** min and max ...  used by rand_rotation() and random_sphere()               ***/
/**********************************************************************************/

float random_float( int seed, float min, float max )
{
	float r=0;
        if(seed>0)    /**** non-repeatable random number uniformly distributed ***/
        {

		/*** srandomdev() is a BSD unix, not portable to Linux ***/
		/*  srandomdev(); */
		r = (max-min) * ( (float) random() / (RAND_MAX) ) + min;  /* min to max */
		/* fprintf( stdout, "%s: %s: %s: r=%g\n", progname, __FILE__, __func__, r ); */
        }
        else          /*** repeatable random number uniformly distributed ***/
        {
                /* sranddev(); */
		r = (max-min) * ( (float) rand() / (RAND_MAX) ) + min;  /* min to max */
		/* fprintf( stdout, "%s: %s: %s: r=%g\n", progname, __FILE__, __func__, r ); */
        }
	return r;
}

/********************************************************************************/
/*** sort by sorttype = descending or ascending order ***/
/********************************************************************************/

void flt_sort( float *e, int *indx, char *sorttype, int verbose )
{
	int i, n = 3;
	float a[4] = {0,0,0,0}; /*shift index by 1*/
	void indexx( int n, float *arrin, int *indx );

	for( i = 1; i <= n; i++ ) a[i] = e[i-1];
	indexx( n, a, indx );

	if( strcmp( sorttype, "descending" ) == 0 )
	{
		for( i = 1; i <= n; i++ )
		{
			e[i-1] = a[indx[n-(i-1)]];
		}
	}
	else if( strcmp( sorttype, "ascending" ) == 0 )
	{
		for( i = 1; i <= n; i++ ) 
		{
			e[i-1] = a[indx[i]];
		}
	}
	else
	{
		fprintf(stderr, "%s: %s: %s: Error unknown sorttype = %s\n", 
			progname, __FILE__, __func__, sorttype );
		exit(-1);
	}
	if(verbose) 
	{
		fprintf( stdout, "%s: %s: %s: sorttype=%s e[0]=%g e[1]=%g e[2]=%g\n",
			progname, __FILE__, __func__, sorttype, e[0], e[1], e[2] );
		fflush(stdout);
	}
}

/********************************************************************************/
/*** sort vector by absolute values, carry over signs +/- polarity            ***/
/********************************************************************************/

void abs_sort( float *e, int verbose )
{
	int i, n = 3;
	float a[4] = {0,0,0,0}; /*shift index by 1*/
	int s[4] = {+1,+1,+1,+1};
	int indx[4] = {1,2,3,4};
	void indexx( int n, float *arrin, int *indx );

/*** take abs value and save sign ***/

	for( i = 1; i <= n; i++ ) 
	{
		/* if(verbose)
		  fprintf( stdout, "%s: %s: %s: i=%d e=%g\n",
			progname, __FILE__, __func__, i, e[i-1] ); */

		if( e[i-1] < 0 )
			s[i] = -1;
		else
			s[i] = +1;

		/***shift by 1***/
		a[i] = fabs(e[i-1]);
	}

/* sort and return using original sign */

	indexx( n, a, indx );

	for( i = 1; i <= n; i++ ) 
	{
		/*** shift back by 1***/
		e[i-1] = s[indx[i]] * a[indx[i]];

		/* if(verbose)
		  fprintf( stdout, "%s: %s: %s: i=%d e=%g s=%d a=%g\n",
			  progname, __FILE__, __func__, i, e[i-1], s[indx[i]], a[indx[i]] ); */
	}
}

void eig_sort3( Eigenvalue *e, Eigenvalue *e_sort, int verbose )
{
	int i, j, n = 3; 
	int indx[4] = {1,2,3,4};
	float eig[3];
	void flt_sort( float *e, int *indx, char *sorttype, int verbose );
	
	for( i = 0; i < n; i++ ) eig[i] = e[i].u;
	flt_sort( eig, indx, "ascending", verbose );

	for( i = 0; i < n; i++ )
	{
		e_sort[i].u = eig[i];
		e_sort[i].lat = e[indx[i]-1].lat;
		e_sort[i].lon = e[indx[i]-1].lon;

		for( j = 0; j < n; j++ )
		{
			e_sort[i].v[j] = e[indx[i]-1].v[j];
		}
	}
}

void Add_DC_iso_2eigenvals( int itag, Results *rbest, int eigvec_fac, float Mo )
{
	float distrad;
        float distradians_gcp( float lat0, float lon0, float lat1, float lon1 );
        float kappa = 1.3;
        float vonMises1D_pdf( float kappa, float dist_rads, float eigvec_fac );
        float vonMisesFisher3d_pdf( float kappa, float dist_rads, float eigvec_fac );

	if(itag==0) /*DC*/
	{
		rbest->pdc      = 1;
                rbest->pclvd    = 0;
                rbest->piso     = 0;
		rbest->pdev     = 1;
                rbest->e0       = +1;
                rbest->e1       = 0;
                rbest->e2       = -1;
                rbest->lune_lat = 0;
                rbest->lune_lon = 0;
                rbest->k        = 0;
                rbest->T        = 0;
                rbest->Mtotal   = Mo;
                rbest->FullMT.eig[1].val = +1;
                rbest->FullMT.eig[2].val = 0;
                rbest->FullMT.eig[3].val = -1;
		rbest->gcarc_dc  = 0;
		rbest->gcarc_exp = 90;

		distrad = distradians_gcp( rbest->lune_lat, rbest->lune_lon, 0.0, 0.0 );
		rbest->nevec = (int) rint( vonMisesFisher3d_pdf( kappa, distrad, eigvec_fac ) );
	}
	else if( itag == 1 ) /* +iso */
	{
		rbest->pdc      = 0;
                rbest->pclvd    = 0;
                rbest->piso     = 1;
		rbest->pdev     = 0;
                rbest->e0       = +1;
                rbest->e1       = +1;
                rbest->e2       = +1;
                rbest->lune_lat = +90;
                rbest->lune_lon = 0;
                rbest->k        = +1;
                rbest->T        = 0;
                rbest->Mtotal   = Mo;
                rbest->FullMT.eig[1].val = +1;
                rbest->FullMT.eig[2].val = +1;
                rbest->FullMT.eig[3].val = +1;
		rbest->gcarc_dc  = 90;
                rbest->gcarc_exp = 0;

		distrad = distradians_gcp( rbest->lune_lat, rbest->lune_lon, 0.0, 0.0 );
		rbest->nevec = (int) rint( vonMisesFisher3d_pdf( kappa, distrad, eigvec_fac ) );
	}
	else if( itag == 2 ) /* -iso */
	{
		rbest->pdc      = 0;
                rbest->pclvd    = 0;
                rbest->piso     = 1;
		rbest->pdev     = 0;
                rbest->e0       = -1;
                rbest->e1       = -1;
                rbest->e2       = -1;
                rbest->lune_lat = -90;
                rbest->lune_lon = 0;
                rbest->k        = -1;
                rbest->T        = 0;
                rbest->Mtotal   = Mo;
                rbest->FullMT.eig[1].val = -1;
                rbest->FullMT.eig[2].val = -1;
                rbest->FullMT.eig[3].val = -1;
		rbest->gcarc_dc  = 0;
                rbest->gcarc_exp = 90;

		distrad = distradians_gcp( rbest->lune_lat, rbest->lune_lon, 0.0, 0.0 );
                rbest->nevec = (int) rint( vonMisesFisher3d_pdf( kappa, distrad, eigvec_fac ) );

	}
	else if( itag == 3 ) /* -CLVD */
	{
		rbest->pdc      = 0;
                rbest->pclvd    = 1;
                rbest->piso     = 0;
                rbest->pdev     = 1;
                rbest->e0       = +0.5;
                rbest->e1       = +0.5;
                rbest->e2       = -1;
                rbest->lune_lat = 0;
                rbest->lune_lon = +30;
                rbest->k        = 0;
                rbest->T        = -1;
                rbest->Mtotal   = Mo;
                rbest->FullMT.eig[1].val = +0.5;
                rbest->FullMT.eig[2].val = +0.5;
                rbest->FullMT.eig[3].val = -1;
		rbest->gcarc_dc  = 30;
                rbest->gcarc_exp = 90;

		distrad = distradians_gcp( rbest->lune_lat, rbest->lune_lon, 0.0, 0.0 );
                rbest->nevec = (int) rint( vonMisesFisher3d_pdf( kappa, distrad, eigvec_fac ) );
	}
	else if( itag == 4 ) /* +CLVD */
	{
		rbest->pdc      = 0;
                rbest->pclvd    = 1;
                rbest->piso     = 0;
                rbest->pdev     = 1;
                rbest->e0       = -0.5;
                rbest->e1       = -0.5;
                rbest->e2       = +1.0;
                rbest->lune_lat = 0;
                rbest->lune_lon = -30;
                rbest->k        = 0;
                rbest->T        = +1;
                rbest->Mtotal   = Mo;
                rbest->FullMT.eig[1].val = -0.5;
                rbest->FullMT.eig[2].val = -0.5;
                rbest->FullMT.eig[3].val = +1.0;
		rbest->gcarc_dc  = 30;
                rbest->gcarc_exp = 90;

		distrad = distradians_gcp( rbest->lune_lat, rbest->lune_lon, 0.0, 0.0 );
                rbest->nevec = (int) rint( vonMisesFisher3d_pdf( kappa, distrad, eigvec_fac ) );
	}
	else 
	{
		fprintf( stderr, "%s: %s: %s: FATAL ERROR unknown itag\n", 
			progname, __FILE__, __func__ );
		exit(-1);
	}
}

/******************************************************************************************/
/***
  Results *create_clvd_eigs():
     this code computes lune lat lon pairs for line across the lune "-CLVD/+CLVD" 
     sometimes referred to as the deviatoric disk. Calculation of eigenvalues performed 
     using raio of moments Mclvd=-1 to +1, Mdc=1-Mclvd and Miso=0 
     then function call to eig2eig to calculate the Lune lon/lat T,k percent dc,clvd,iso
     and number of eigenvectors to search for best fitting largest %VR based on
     von Mises-Fisher distribution as a function of radius from DC/origin  
 *** +CLVD lune_lon=-30 lune_lat=0  e0 = -0.5; e1 = -0.5; e2 = +1.0;
 *** DC    lune_lon=0   lune_lat=0  e0 = -1.0; e1 =  0.0; e2 = +1.0;
 *** -CLVD lune_lon=+30 lune_lat=0  e0 = +0.5; e1 = +0.5; e2 = -1.0;
 ***/
/*********************************************************************************************/

Results *create_clvd_eigs( Results *rbest, int *nsim_eig, int eigvec_fac, float Mo, float dMo, int verbose )
{
	int i = 0, j = 0, nclvd = 0;

	float Mclvd, Mdc, sgn;
	float Mtotal = 1, Mclvd0 = -1, Mclvd1 = +1;
/*** about 0.5degrees longitude on lune ***/
	/* float dMo = 0.02; */

/*** eig2eig() local variables ***/
	float e0, e1, e2;
	float pdc, pclvd, piso;
	float lune_lat, lune_lon, hudson_k, hudson_T;
	float gcarc_dc, gcarc_exp;
	int nevec;

	void eig2eig( float *lamb1, float *lamb2, float *lamb3,
              float *percent_dc, float *percent_clvd, float *percent_iso,
              float *lune_lat, float *lune_lon, float *hudson_k, float *hudson_T,
              float *gcarc_dc, float *gcarc_exp, int *nevec, int eigvec_fac, int verbose );

/************* start **************************/

	nclvd = (int)rint( ( Mclvd1 - Mclvd0 ) / dMo ) + 1;

	if(verbose)
	{
	  fprintf( stderr, "%s: %s: %s: STDERR: nclvd = %d\n", progname, __FILE__, __func__, nclvd );
	  fflush( stderr );
	}

	/** preallocate outside of function **/
	rbest = (Results *) realloc( rbest, (nclvd + *nsim_eig + 1)*sizeof(Results) );

	j = *nsim_eig;
	for( i = 0; i < nclvd; i++ )
	{
		Mclvd = Mclvd0 + (float)i * dMo;
		if( Mclvd < 0 ) sgn   = -1;
		else            sgn   = +1;
		Mclvd = fabs(Mclvd);
		Mdc   = Mtotal - Mclvd;
		e0    = -0.5 * fabs(Mclvd);
		e1    = e0 - fabs(Mdc);
		e2    = -( e0 + e1 );
		e0 *= sgn;
		e1 *= sgn;
		e2 *= sgn;

		eig2eig( &e0, &e1, &e2, &pdc, &pclvd, &piso,
			 &lune_lat, &lune_lon, &hudson_k, &hudson_T, 
			&gcarc_dc, &gcarc_exp, &nevec, eigvec_fac, verbose );

                rbest[j].pdc      = pdc;
                rbest[j].pclvd    = pclvd;
                rbest[j].piso     = piso;
                rbest[j].e0       = e0;
                rbest[j].e1       = e1;
                rbest[j].e2       = e2;

		/*** force zero ***/
                /* rbest[j].lune_lat = lune_lat; */
		rbest[j].lune_lat = 0.0;
                rbest[j].lune_lon = lune_lon;

		/*** force zero ***/
                /* rbest[j].k        = hudson_k; */	
		rbest[j].k        = 0.0;
                rbest[j].T        = hudson_T;

                rbest[j].Mtotal   = Mo;
                rbest[j].FullMT.eig[1].val = e0;
                rbest[j].FullMT.eig[2].val = e1;
                rbest[j].FullMT.eig[3].val = e2;
                rbest[j].gcarc_dc  = gcarc_dc;
                rbest[j].gcarc_exp = gcarc_exp;
                rbest[j].nevec     = nevec;
		j++;
	}
	*nsim_eig += nclvd;

	return (Results *) rbest;
}
 
void print_result_row_pretty_screen( Results *r, FILE *fp, char *label )
{
	fprintf( fp, "%s: %s: %s: %s: \n", progname, __FILE__, __func__, label );
	fflush(fp);
	
	fprintf( fp, "\t\t lon=%9.3f lat=%8.3f VR=%.3f %s\n", 
		r->lune_lon,
                r->lune_lat,
                r->var_red,
		label );
	fflush(fp);

	fprintf( fp, "\t\t arcDC=%5.1f(deg) arcExp=%5.1f(deg) DC=%3.0f%% CLVD=%3.0f%% ISO=%3.0f%% T=%5.2f k=%5.2f\n",
		r->gcarc_dc,
                r->gcarc_exp,
                r->pdc*100.0,
                r->pclvd*100.0,
                r->piso*100.0,
		r->T, r->k );
	fflush(fp);

	fprintf( fp, "\t\t M0=%5.2e eigvals=(%.3f, %.3f, %.3f) Mij:(xx,xy,xz,yy,yz,zz) %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f\n",
		r->Mtotal,
		r->e0, r->e1, r->e2,
                r->mxx,
                r->mxy,
                r->mxz,
                r->myy,
                r->myz,
                r->mzz );
	fflush(fp);
}
