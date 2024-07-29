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

/********************************************************************************/
/*** These functions generate the A-matrix used in the inverse problem d = Ax ***/
/***  Also creates the data and vector, see Mij convension for x-vector MT-sol***/
/*** this gets used by forward_calc_from_MT.c, grn2Mxy.c, mtinv.c             ***/
/*** there are many versions so becareful which you use!                      ***/
/********************************************************************************/

/***
G. Ichinose Wed Jan 10 17:22:21 PST 2018 
  Kludge to solve problems with random memory leak bug...

  changed all instances of ev[ista].ew.data[it] -> ev[ista].ew.data_safe[it]  
                           ev[ista].ns.data[it] -> ev[ista].ns.data_safe[it]
                           ev[ista].z.data[it]  -> ev[ista].z.data_safe[it]

         data[it] is dynamically allocated and still used important for sacdata2inv.c
         data_safe[it] is statically allocated with predefined size of 8192
***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/nrutil.h"
#include "../include/mt.h"

extern char progname[128];

static float half=0.5;
static float sixth=0.166666667;
static float third=0.333333333;


/*********************************************************************************/
/*** this version of make_amatrix2 uses the formaution of Jost&Herrmann (1989) ***/
/*** only deviatoric moment tensors not full mt                                ***/
/*********************************************************************************/

void make_amatrix2( Greens **grn, EventInfo *ev, int nsta, int iz, float **a_matrix, 
	float *b_vector, int mtdegfree, int Distance_Normalize, float DistNormR0 )
{
	int it, nt, ista, irow;
	float fi;
	static float d2r = 0.017453292519943295;
	static float r2d = 57.29577951308232;

	float *rss, *rds, *rdd, *rep, *zss, *zds, *zdd, *zep, *tss, *tds;
	float Rnorm = 1;
	float wt = 1;
	int verbose = 0;

/************************************************/
/*** set some constants                       ***/
/************************************************/

	if(verbose)fprintf(stderr, "make_amatrix2(iz=%d)\n", iz );

/************************************************/
/*** station loop                             ***/
/************************************************/

	irow = 1;
	for( ista = 0; ista < nsta; ista++ )
	{

	/*****************************************************/
	/*** skip comment out stations in the calculations ***/
	/*****************************************************/
		if( ev[ista].iused < 1 )
		{
			if(verbose)
			{
			  fprintf(stderr, 
			    "make_amatrix2(iz=%d): skiping %d %s %s\n",
				iz, ista, ev[ista].stnm, ev[ista].net );
			}
			continue;
		}

	/************************************************/
	/*** local variables                          ***/
	/************************************************/

		nt = grn[ista][iz].nt;
		fi = grn[ista][iz].az;
		wt = ev[ista].weight;

		if( Distance_Normalize )
		{
			Rnorm = grn[ista][iz].rdist/DistNormR0;
			if(verbose)
			{
			  fprintf(stdout, 
			    "make_amatrix2: sta=%s R0=%g dist=%g Rnorm=%g\n",
				ev[ista].stnm, DistNormR0, grn[ista][iz].rdist, Rnorm );
			}
		}

	/************************************************/
	/*** create pointers to the Green's functions ***/
	/************************************************/

		rss = grn[ista][iz].g.rss; 
		rds = grn[ista][iz].g.rds;
		rdd = grn[ista][iz].g.rdd;
		rep = grn[ista][iz].g.rep;
		zss = grn[ista][iz].g.zss;
		zds = grn[ista][iz].g.zds;
		zdd = grn[ista][iz].g.zdd;
		zep = grn[ista][iz].g.zep;
		tss = grn[ista][iz].g.tss;
		tds = grn[ista][iz].g.tds;

	/************************************************/
	/*** transverse/tangential component          ***/
	/************************************************/

		for( it = 0; it < nt; it++ )
		{
		  a_matrix[irow][1] = (  half * sin(2.0*fi) * tss[it] ) * Rnorm * wt;
		  a_matrix[irow][2] = ( -half * sin(2.0*fi) * tss[it] ) * Rnorm * wt;
		  a_matrix[irow][3] = (        -cos(2.0*fi) * tss[it] ) * Rnorm * wt;
		  a_matrix[irow][4] = (             sin(fi) * tds[it] ) * Rnorm * wt;
		  a_matrix[irow][5] = (            -cos(fi) * tds[it] ) * Rnorm * wt;
		  a_matrix[irow][6] = 0;
		  b_vector[irow]    = ev[ista].ew.data_safe[it] * Rnorm * wt;
		  irow++;
		}

	/************************************************/
	/*** radial components ***/
	/************************************************/

		for( it = 0; it < nt; it++ )
		{
		  a_matrix[irow][1] = ( +half*cos(2*fi) * rss[it] -half*rdd[it] ) * Rnorm * wt;
		  a_matrix[irow][2] = ( -half*cos(2*fi) * rss[it] -half*rdd[it] ) * Rnorm * wt;
		  a_matrix[irow][3] = (       sin(2*fi) * rss[it] ) * Rnorm * wt;
		  a_matrix[irow][4] = (         cos(fi) * rds[it] ) * Rnorm * wt;
		  a_matrix[irow][5] = (         sin(fi) * rds[it] ) * Rnorm * wt;
		  a_matrix[irow][6] = 0;
		  b_vector[irow]    = ev[ista].ns.data_safe[it] * Rnorm * wt;
		  irow++;
		}

	/************************************************/
	/*** vertical component ***/
	/************************************************/
		for( it = 0; it < nt; it++ )
		{
		  a_matrix[irow][1] = ( +half*cos(2*fi) * zss[it] -half*zdd[it] ) * Rnorm * wt;
		  a_matrix[irow][2] = ( -half*cos(2*fi) * zss[it] -half*zdd[it] ) * Rnorm * wt;
		  a_matrix[irow][3] = (       sin(2*fi) * zss[it] ) * Rnorm * wt;
		  a_matrix[irow][4] = (         cos(fi) * zds[it] ) * Rnorm * wt;
		  a_matrix[irow][5] = (         sin(fi) * zds[it] ) * Rnorm * wt;
		  a_matrix[irow][6] = 0;
		  b_vector[irow]    = ev[ista].z.data_safe[it] * Rnorm * wt;
		  irow++;
		}
	}
}

/***********************************************************************************/
/*** this version of make_amatrix uses the formaution of Herrmann and Hutchenson ***/
/*** (1993) and can be used for both deviatoric and full moment tensors          ***/
/***  not Jost&Herrmann(1989) - see Minson and Dreger (2008) GJI                 ***/
/***                                                                             ***/
/*** G.Ichinose Nov  7 2019  - Added support for rotational GFs w1, w2, w3       ***/
/*** Rotational component about EW         x1-axis U -> w1                       ***/
/*** Rotational component about NS         x2-axis V -> w2                       ***/
/*** Rotational component about z-vertical x3-axis W -> w3                       ***/
/***********************************************************************************/

void make_amatrix(
	EventInfo *ev,
	Greens **grn, 
	int nsta,
	int iz,
	float **a_matrix,
	float *b_vector,
	int mtdegfree,
	int Distance_Normalize,
	float DistNormR0 )
{
	int it = 0, nt = 0, ista = 0, irow = 0;
	float fi;

	static float d2r = 0.017453292519943295;
	static float r2d = 57.29577951308232;

	float *rss, *rds, *rdd, *rep;
	float *zss, *zds, *zdd, *zep;
	float *tss, *tds;

	float *w1ss, *w1ds, *w1dd, *w1ex;
	float *w2ss, *w2ds, *w2dd, *w2ex;
	float *w3ss, *w3ds, *w3dd, *w3ex;

        float Rnorm = 1.0;
	float wt = 1.0;
        int verbose = 0; /****** this is fixed and not a functional argument ************/

	void  writesacfile( EventInfo *ev );

	if(verbose)
	{
		if( mtdegfree == FORCE_EXPLOSION )
			fprintf( stdout, "%s: %s: %s: mtdegfree = FORCE_EXPLOSION\n", progname, __FILE__, __func__ );
		if( mtdegfree == DEVIATORIC_MOMENT_TENSOR ) 
			fprintf( stdout, "%s: %s: %s: mtdegfree = DEVIATORIC_MOMENT_TENSOR\n", progname, __FILE__, __func__ );
		if( mtdegfree == FULL_MOMENT_TENSOR )
			fprintf( stdout, "%s: %s: %s: mtdegfree = FULL_MOMENT_TENSOR\n", progname, __FILE__, __func__ );
	}

/************************************************/
/*** station loop                             ***/
/************************************************/
                                                                                                                                    
        irow = 1;
        for( ista = 0; ista < nsta; ista++ )
        {
        /*****************************************************/
        /*** skip comment out stations in the calculations ***/
        /*****************************************************/
                if( ev[ista].iused < 1 )
                {
                        if(verbose)
                        {
                          fprintf(stdout,
                            "%s: %s: %s: make_amatrix(iz=%d): skiping %d %s %s\n",
                                progname, __FILE__, __func__,
				iz, ista, ev[ista].stnm, ev[ista].net );
                        }
                        continue;
                }
                                                                                                                                    
        /************************************************/
        /*** local variables                          ***/
        /************************************************/

                nt = grn[ista][iz].nt;
                fi = grn[ista][iz].az * d2r;
		wt = ev[ista].weight;

                if( Distance_Normalize )
                {
                        Rnorm = grn[ista][iz].rdist / DistNormR0;

                        if(verbose)
                        {
                          fprintf(stdout,
                            "%s: %s: %s: sta=%s R0=%g dist=%g Rnorm=%g\n",
				progname, 
				__FILE__,
				__func__,
                                ev[ista].stnm,
				DistNormR0,
				grn[ista][iz].rdist,
				Rnorm );
                        }
                }

        /************************************************/
        /*** create pointers to the Green's functions ***/
        /************************************************/

                rss = grn[ista][iz].g.rss;
                rds = grn[ista][iz].g.rds;
                rdd = grn[ista][iz].g.rdd;
                rep = grn[ista][iz].g.rep;

                zss = grn[ista][iz].g.zss;
                zds = grn[ista][iz].g.zds;
                zdd = grn[ista][iz].g.zdd;
                zep = grn[ista][iz].g.zep;

                tss = grn[ista][iz].g.tss;
                tds = grn[ista][iz].g.tds;

		w1ss = grn[ista][iz].g.w1ss; 
		w1ds = grn[ista][iz].g.w1ds; 
		w1dd = grn[ista][iz].g.w1dd; 
		w1ex = grn[ista][iz].g.w1ex;

		w2ss = grn[ista][iz].g.w2ss;
		w2ds = grn[ista][iz].g.w2ds;
		w2dd = grn[ista][iz].g.w2dd;
		w2ex = grn[ista][iz].g.w2ex;

		w3ss = grn[ista][iz].g.w3ss; 
		w3ds = grn[ista][iz].g.w3ds;
		w3dd = grn[ista][iz].g.w3dd;
		w3ex = grn[ista][iz].g.w3ex;

		if(verbose)
		{
		  fprintf(stdout,
  "%s: %s: %s: sta(ista=%d)=%s z(iz=%d)=%g dist=%g Distance_Normalize=%d DistNormR0=%g Rnorm=%g iz=%d mtdegfree=%d nt=%d fi=%g wt=%g\n",
			progname, 
			__FILE__,
			__func__,
			ista,
			ev[ista].stnm,
			iz,
			grn[ista][iz].evdp,
			grn[ista][iz].rdist,
			Distance_Normalize,
			DistNormR0,
			Rnorm,
			iz,
			mtdegfree,
			nt,
			fi*(r2d),
			wt );
		}

        /****************************************************/
        /*** transverse/tangential component BHT BHE BHW w3**/
	/*** or w3 rotation about the vertical axis       ***/
        /****************************************************/
/***
U -> w1  z z
V -> w2  r ns
W -> w3  t ew
***/
		
                for( it = 0; it < nt; it++ )
                {
		  if( mtdegfree == FORCE_EXPLOSION )
		  {
			a_matrix[irow][1] = 0;
			a_matrix[irow][2] = 0;
			a_matrix[irow][3] = 0;
			a_matrix[irow][4] = 0;
			a_matrix[irow][5] = 0;
			a_matrix[irow][6] = 0;
		  }
		  else
		  {
		     if( strcmp( ev[ista].wavetype, "Surf/Pnl" ) == 0 )
		     {
                       a_matrix[irow][1] = (  half * sin(2*fi) * tss[it] ) * Rnorm * wt;  /* Mxx */
                       a_matrix[irow][2] = ( -half * sin(2*fi) * tss[it] ) * Rnorm * wt;  /* Myy */
                       a_matrix[irow][3] = (        -cos(2*fi) * tss[it] ) * Rnorm * wt;  /* Mxy */
                       a_matrix[irow][4] = (           sin(fi) * tds[it] ) * Rnorm * wt;  /* Mxz */
                       a_matrix[irow][5] = (          -cos(fi) * tds[it] ) * Rnorm * wt;  /* Myz */
                       a_matrix[irow][6] = 0;                                             /* Mzz */
		     }

		     if( strcmp( ev[ista].wavetype, "Rotational" ) == 0 )
		     {
		       a_matrix[irow][1] = -(  half * sin(2*fi) * w3ss[it] ) * Rnorm * wt; /* Mxx */
                       a_matrix[irow][2] = -( -half * sin(2*fi) * w3ss[it] ) * Rnorm * wt; /* Myy */
                       a_matrix[irow][3] = -(        -cos(2*fi) * w3ss[it] ) * Rnorm * wt; /* Mxy */
                       a_matrix[irow][4] = -(           sin(fi) * w3ds[it] ) * Rnorm * wt; /* Mxz */
                       a_matrix[irow][5] = -(          -cos(fi) * w3ds[it] ) * Rnorm * wt; /* Myz */
                       a_matrix[irow][6] = 0;
    		     }
		  }

                  b_vector[irow]    = ev[ista].ew.data_safe[it] * Rnorm * wt;

                  irow++;
		}

        /************************************************/
        /*** radial components BHR, HHR, BHN, BHV w2  ***/
	/*** or rotation w2 about north-south axis    ***/
        /************************************************/
/***
U -> w1  z z
V -> w2  r ns
W -> w3  t ew
***/

                for( it = 0; it < nt; it++ )
                {
		  if( mtdegfree == FORCE_EXPLOSION )
		  {
		    if( strcmp( ev[ista].wavetype, "Surf/Pnl" ) == 0 )
		    {
			a_matrix[irow][1] = ( third * rep[it] ) * Rnorm * wt;
			a_matrix[irow][2] = ( third * rep[it] ) * Rnorm * wt;
			a_matrix[irow][6] = ( third * rep[it] ) * Rnorm * wt;
		    }

		    if( strcmp( ev[ista].wavetype, "Rotational" ) == 0 )
		    {
			a_matrix[irow][1] = ( third * w2ex[it] ) * Rnorm * wt;
                        a_matrix[irow][2] = ( third * w2ex[it] ) * Rnorm * wt;
                        a_matrix[irow][6] = ( third * w2ex[it] ) * Rnorm * wt;
		    }

		    a_matrix[irow][3] = 0;
		    a_matrix[irow][4] = 0;
		    a_matrix[irow][5] = 0;
		  }
		  else if( mtdegfree == DEVIATORIC_MOMENT_TENSOR )
		  {

		    if( strcmp( ev[ista].wavetype, "Surf/Pnl" ) == 0 )
		    {
                      a_matrix[irow][1] = ( +half*cos(2*fi) * rss[it] -half*rdd[it] ) * Rnorm * wt; /* Mxx */
                      a_matrix[irow][2] = ( -half*cos(2*fi) * rss[it] -half*rdd[it] ) * Rnorm * wt; /* Myy */
                      a_matrix[irow][3] = (       sin(2*fi) * rss[it] ) * Rnorm * wt;               /* Mxy */
                      a_matrix[irow][4] = (         cos(fi) * rds[it] ) * Rnorm * wt;               /* Mxz */
                      a_matrix[irow][5] = (         sin(fi) * rds[it] ) * Rnorm * wt;               /* Myz */
                      a_matrix[irow][6] = 0;                                                        /* Mzz */
		    }

		    if( strcmp( ev[ista].wavetype, "Rotational" ) == 0 )
		    {
		      a_matrix[irow][1] = ( +half*cos(2*fi) * w2ss[it] -half*w2dd[it] ) * Rnorm * wt; /* Mxx */
                      a_matrix[irow][2] = ( -half*cos(2*fi) * w2ss[it] -half*w2dd[it] ) * Rnorm * wt; /* Myy */
                      a_matrix[irow][3] = (       sin(2*fi) * w2ss[it] ) * Rnorm * wt;                /* Mxy */
                      a_matrix[irow][4] = (         cos(fi) * w2ds[it] ) * Rnorm * wt;                /* Mxz */
                      a_matrix[irow][5] = (         sin(fi) * w2ds[it] ) * Rnorm * wt;                /* Myz */
                      a_matrix[irow][6] = 0;                                                          /* Mzz */
		    }

		  }
		  else if( mtdegfree == FULL_MOMENT_TENSOR )
		  {

		    if( strcmp( ev[ista].wavetype, "Surf/Pnl" ) == 0 )
		    {
		      a_matrix[irow][1] = ( +half*cos(2*fi) * rss[it] -sixth*rdd[it] + third*rep[it] ) * Rnorm * wt;
                      a_matrix[irow][2] = ( -half*cos(2*fi) * rss[it] -sixth*rdd[it] + third*rep[it] ) * Rnorm * wt;
                      a_matrix[irow][3] = (       sin(2*fi) * rss[it] ) * Rnorm * wt;
                      a_matrix[irow][4] = (         cos(fi) * rds[it] ) * Rnorm * wt;
                      a_matrix[irow][5] = (         sin(fi) * rds[it] ) * Rnorm * wt;
                      a_matrix[irow][6] = ( third * rdd[it] + third * rep[it] ) * Rnorm * wt;
		    }

		    if( strcmp( ev[ista].wavetype, "Rotational" ) == 0 )
		    {
			a_matrix[irow][1] = ( +half*cos(2*fi) * w2ss[it] -sixth*w2dd[it] + third*w2ex[it] ) * Rnorm * wt;
                        a_matrix[irow][2] = ( -half*cos(2*fi) * w2ss[it] -sixth*w2dd[it] + third*w2ex[it] ) * Rnorm * wt;
                        a_matrix[irow][3] = (       sin(2*fi) * w2ss[it] ) * Rnorm * wt;
                        a_matrix[irow][4] = (         cos(fi) * w2ds[it] ) * Rnorm * wt;
                        a_matrix[irow][5] = (         sin(fi) * w2ds[it] ) * Rnorm * wt;
                        a_matrix[irow][6] = ( third * w2dd[it] + third * w2ex[it] ) * Rnorm * wt;
		    }

		  }

                  b_vector[irow]    = ev[ista].ns.data_safe[it] * Rnorm * wt;

                  irow++;
                }

        /************************************************/
        /*** vertical component HHZ BHZ BHU w1        ***/
	/*** or w1 rotation about the east-west axis  ***/
        /************************************************/
/***
U -> w1  z z
V -> w2  r ns
W -> w3  t ew
***/

                for( it = 0; it < nt; it++ )
                {
		  if( mtdegfree == FORCE_EXPLOSION )
		  {
		    if( strcmp( ev[ista].wavetype, "Surf/Pnl" ) == 0 )	
		    {
                      a_matrix[irow][1] = ( third * zep[it] ) * Rnorm * wt;
                      a_matrix[irow][2] = ( third * zep[it] ) * Rnorm * wt;
		      a_matrix[irow][6] = ( third * zep[it] ) * Rnorm * wt;
		    }

		    if( strcmp( ev[ista].wavetype, "Rotational" ) == 0 )
		    {
		       a_matrix[irow][1] = ( third * w1ex[it] ) * Rnorm * wt;
                       a_matrix[irow][2] = ( third * w1ex[it] ) * Rnorm * wt;
                       a_matrix[irow][6] = ( third * w1ex[it] ) * Rnorm * wt;
		    }

                    a_matrix[irow][3] = 0;
                    a_matrix[irow][4] = 0;
                    a_matrix[irow][5] = 0;
                  }
                  else if( mtdegfree == DEVIATORIC_MOMENT_TENSOR )
                  {
		    if( strcmp( ev[ista].wavetype, "Surf/Pnl" ) == 0 )  
		    {
                      a_matrix[irow][1] = ( +half*cos(2*fi) * zss[it] -half*zdd[it] ) * Rnorm * wt; /* Mxx */
                      a_matrix[irow][2] = ( -half*cos(2*fi) * zss[it] -half*zdd[it] ) * Rnorm * wt; /* Myy */
                      a_matrix[irow][3] = (       sin(2*fi) * zss[it] ) * Rnorm * wt;               /* Mxy */
                      a_matrix[irow][4] = (         cos(fi) * zds[it] ) * Rnorm * wt;               /* Mxz */
                      a_matrix[irow][5] = (         sin(fi) * zds[it] ) * Rnorm * wt;               /* Myz */
                      a_matrix[irow][6] = 0;                                                        /* Mzz */
		    }

		    if( strcmp( ev[ista].wavetype, "Rotational" ) == 0 )
		    {
		      a_matrix[irow][1] = ( +half*cos(2*fi) * w1ss[it] -half*w1dd[it] ) * Rnorm * wt; /* Mxx */
                      a_matrix[irow][2] = ( -half*cos(2*fi) * w1ss[it] -half*w1dd[it] ) * Rnorm * wt; /* Myy */
                      a_matrix[irow][3] = (       sin(2*fi) * w1ss[it] ) * Rnorm * wt;               /* Mxy */
                      a_matrix[irow][4] = (         cos(fi) * w1ds[it] ) * Rnorm * wt;               /* Mxz */
                      a_matrix[irow][5] = (         sin(fi) * w1ds[it] ) * Rnorm * wt;               /* Myz */
                      a_matrix[irow][6] = 0;                                                        /* Mzz */
		    }

		  }
		  else if( mtdegfree == FULL_MOMENT_TENSOR )
                  {

		    if( strcmp( ev[ista].wavetype, "Surf/Pnl" ) == 0 ) 
		    {
                      a_matrix[irow][1] = ( +half*cos(2*fi) * zss[it] -sixth*zdd[it] + third*zep[it] ) * Rnorm * wt;
                      a_matrix[irow][2] = ( -half*cos(2*fi) * zss[it] -sixth*zdd[it] + third*zep[it] ) * Rnorm * wt;
                      a_matrix[irow][3] = (       sin(2*fi) * zss[it] ) * Rnorm * wt;
                      a_matrix[irow][4] = (         cos(fi) * zds[it] ) * Rnorm * wt;
                      a_matrix[irow][5] = (         sin(fi) * zds[it] ) * Rnorm * wt;
                      a_matrix[irow][6] = ( third * zdd[it] + third * zep[it] ) * Rnorm * wt;
		    }

		    if( strcmp( ev[ista].wavetype, "Rotational" ) == 0 )
		    {
		      a_matrix[irow][1] = ( +half*cos(2*fi) * w1ss[it] -sixth*w1dd[it] + third*w1ex[it] ) * Rnorm * wt;
                      a_matrix[irow][2] = ( -half*cos(2*fi) * w1ss[it] -sixth*w1dd[it] + third*w1ex[it] ) * Rnorm * wt;
                      a_matrix[irow][3] = (       sin(2*fi) * w1ss[it] ) * Rnorm * wt;
                      a_matrix[irow][4] = (         cos(fi) * w1ds[it] ) * Rnorm * wt;
                      a_matrix[irow][5] = (         sin(fi) * w1ds[it] ) * Rnorm * wt;
                      a_matrix[irow][6] = ( third * w1dd[it] + third * w1ex[it] ) * Rnorm * wt;
		    }

                  }

		  b_vector[irow]    = ev[ista].z.data_safe[it] * Rnorm * wt;

                  irow++;
                }
	}
}

/***********************************************************************************/
/*** void make_amatrix_special() - for use with 17 Mxy Greens functions not with ***/
/***                            10 fundamental faulting Greens functions RSS,RDS ***/
/*** this version of make_amatrix uses the formaution of Herrmann and Hutchenson ***/
/*** (1993) and can be used for both deviatoric and full moment tensors          ***/
/***********************************************************************************/

void make_amatrix_special( 
	EventInfo *ev,
	Greens **grn, 
	int nsta,
	int iz,
	float **a_matrix,
	float *b_vector,
	int mtdegfree,
	int Distance_Normalize,
	float DistNormR0 )
{
	int it = 0, nt = 0, ista = 0, irow;
	float fi;

	float *rss, *rds, *rdd, *rep, *zss, *zds, *zdd, *zep, *tss, *tds;
	float Rnorm = 1.0;
	float wt = 1.0;
	int verbose = 0;
	static float d2r = 0.017453292519943295;
	static float r2d = 57.29577951308232;

	void  writesacfile( EventInfo *ev );
	
        int ncmp = 17;
        int icmp = 0;        /* 0      1      2      3       4     5      6      7      8      9     10     11     12     13     14     15     16 ***/
        const char *cmp[] = { "rxx", "rxy", "rxz", "ryy", "ryz", "rzz", "txx", "txy", "txz", "tyy", "tyz", "zxx", "zxy", "zxz", "zyy", "zyz", "zzz" };

/***
	for( ista = 0; ista < nsta; ista++ )
		writesacfile( &ev[ista] );
***/

	if(verbose)
        {
                if( mtdegfree == FORCE_EXPLOSION )
                        fprintf(stdout, "%s: %s: %s: inversion = FORCE_EXPLOSION\n", progname, __FILE__, __func__ );
                if( mtdegfree == DEVIATORIC_MOMENT_TENSOR ) 
                        fprintf(stdout, "%s: %s: %s: inversion = DEVIATORIC_MOMENT_TENSOR\n", progname, __FILE__, __func__ );
                if( mtdegfree == FULL_MOMENT_TENSOR )
                        fprintf(stdout, "%s: %s: %s: inversion = FULL_MOMENT_TENSOR\n", progname, __FILE__, __func__ );
        }

/************************************************/
/*** station loop                             ***/
/************************************************/

	irow = 1;
	for( ista = 0; ista < nsta; ista++ )
	{

	/*****************************************************/
	/*** skip comment out stations in the calculations ***/
	/*****************************************************/

		if( ev[ista].iused < 1 )
		{
			if(verbose)
			{
				fprintf( stdout, 
					"%s: %s: %s: (iz=%d): skiping %d %s %s\n",
						progname, __FILE__, __func__,
						iz, ista, ev[ista].stnm, ev[ista].net );
			}
			fprintf( stdout, 
                                        "%s: %s: %s: (iz=%d): skiping %d %s %s\n",
                                                progname, __FILE__, __func__,
                                                iz, ista, ev[ista].stnm, ev[ista].net );

			continue;
		}

	/************************************************/
	/*** local variables                          ***/
	/************************************************/

		nt = grn[ista][iz].nt;
		fi = grn[ista][iz].az * d2r;
		wt = ev[ista].weight;

		if( Distance_Normalize )
		{
			Rnorm = grn[ista][iz].rdist / DistNormR0;
			
			if(verbose)
			{
				fprintf(stdout,
					"%s: %s: %s: sta=%s R0=%g dist=%g Rnorm=%g\n",
					progname,
					__FILE__,
					__func__,
					ev[ista].stnm,
					DistNormR0,
					grn[ista][iz].rdist,
					Rnorm );
			}
		}

/************************************************/
/*** create pointers to the Green's functions ***/
/************************************************/

		rss = grn[ista][iz].g.rss;
                rds = grn[ista][iz].g.rds;
                rdd = grn[ista][iz].g.rdd;
                rep = grn[ista][iz].g.rep;
                zss = grn[ista][iz].g.zss;
                zds = grn[ista][iz].g.zds;
                zdd = grn[ista][iz].g.zdd;
                zep = grn[ista][iz].g.zep;
                tss = grn[ista][iz].g.tss;
                tds = grn[ista][iz].g.tds;

                fprintf(stdout,
         "%s: %s: %s: sta(ista=%d)=%s z(iz=%d)=%g dist=%g Distance_Normalize=%d DistNormR0=%g Rnorm=%g iz=%d mtdegfree=%d nt=%d fi=%g wt=%g\n",
                        progname, 
                        __FILE__,
                        __func__,
                        ista,
                        ev[ista].stnm,
                        iz,
                        grn[ista][iz].evdp,
                        grn[ista][iz].rdist,
                        Distance_Normalize,
                        DistNormR0,
                        Rnorm,
                        iz,
                        mtdegfree,
                        nt,
                        fi*(r2d),
                        wt );

/************************************************/
/*** transverse/tangential component          ***/
/************************************************/

		for( it = 0; it < nt; it++ )
		{
		  if( mtdegfree == FORCE_EXPLOSION )
		  {
			a_matrix[irow][1] = 0;
                       	a_matrix[irow][2] = 0;
                       	a_matrix[irow][3] = 0;
                       	a_matrix[irow][4] = 0;
                       	a_matrix[irow][5] = 0;
                       	a_matrix[irow][6] = 0;
		  }
		  else
		  {
			a_matrix[irow][1] = ev[ista].rtzGxy[6][it] * Rnorm * wt;  /* Mxx */
			a_matrix[irow][2] = ev[ista].rtzGxy[9][it] * Rnorm * wt;  /* Myy */
			a_matrix[irow][3] = ev[ista].rtzGxy[7][it] * Rnorm * wt;  /* Mxy */
			a_matrix[irow][4] = ev[ista].rtzGxy[8][it] * Rnorm * wt;  /* Mxz */
			a_matrix[irow][5] = ev[ista].rtzGxy[10][it] * Rnorm * wt; /* Myz */
			a_matrix[irow][6] = 0;                                    /* Mzz */
		  }
		  b_vector[irow]    = ev[ista].ew.data_safe[it] * Rnorm * wt;
		  irow++;

		} /*** loop over it ***/

/************************************************/
/*** radial components                        ***/
/************************************************/

		for( it = 0; it < nt; it++ )
		{
		  if( mtdegfree == FORCE_EXPLOSION )
		  {
			a_matrix[irow][1] = ev[ista].rtzGxy[0][it] * Rnorm * wt;  /* Mxx */
                        a_matrix[irow][2] = ev[ista].rtzGxy[3][it] * Rnorm * wt;  /* Myy */
                        a_matrix[irow][3] = 0;
                        a_matrix[irow][4] = 0;
                        a_matrix[irow][5] = 0;
                        a_matrix[irow][6] = ev[ista].rtzGxy[5][it] * Rnorm * wt;  /* Mzz */
		  }
		  else if( mtdegfree == DEVIATORIC_MOMENT_TENSOR )
		  {
			a_matrix[irow][1] = ev[ista].rtzGxy[0][it] * Rnorm * wt;  /* Mxx */
			a_matrix[irow][2] = ev[ista].rtzGxy[3][it] * Rnorm * wt;  /* Myy */
			a_matrix[irow][3] = ev[ista].rtzGxy[1][it] * Rnorm * wt;  /* Mxy */
			a_matrix[irow][4] = ev[ista].rtzGxy[2][it] * Rnorm * wt;  /* Mxz */
			a_matrix[irow][5] = ev[ista].rtzGxy[4][it] * Rnorm * wt;  /* Myz */
			a_matrix[irow][6] = 0;                                    /* Mzz */
		  }
		  else if( mtdegfree == FULL_MOMENT_TENSOR )
		  {
			a_matrix[irow][1] = ev[ista].rtzGxy[0][it] * Rnorm * wt;  /* Mxx */
			a_matrix[irow][2] = ev[ista].rtzGxy[3][it] * Rnorm * wt;  /* Myy */
			a_matrix[irow][3] = ev[ista].rtzGxy[1][it] * Rnorm * wt;  /* Mxy */
			a_matrix[irow][4] = ev[ista].rtzGxy[2][it] * Rnorm * wt;  /* Mxz */
			a_matrix[irow][5] = ev[ista].rtzGxy[4][it] * Rnorm * wt;  /* Myz */
			a_matrix[irow][6] = ev[ista].rtzGxy[5][it] * Rnorm * wt;  /* Mzz */
		  }
		  b_vector[irow]    = ev[ista].ns.data_safe[it] * Rnorm * wt;
		  irow++;

		} /*** loop over it ***/

/************************************************/
/*** vertical component                       ***/
/************************************************/

		for( it = 0; it < nt; it++ )
		{
		  if( mtdegfree == FORCE_EXPLOSION )
		  {
			a_matrix[irow][1] = ev[ista].rtzGxy[11][it] * Rnorm * wt;  /* Mxx */
			a_matrix[irow][2] = ev[ista].rtzGxy[14][it] * Rnorm * wt;  /* Myy */
			a_matrix[irow][3] = 0;
			a_matrix[irow][4] = 0;
			a_matrix[irow][5] = 0;
			a_matrix[irow][6] = ev[ista].rtzGxy[16][it] * Rnorm * wt;  /* Mzz */
		  }
		  else if( mtdegfree == DEVIATORIC_MOMENT_TENSOR )
		  {
			a_matrix[irow][1] = ev[ista].rtzGxy[11][it] * Rnorm * wt;  /* Mxx */
                        a_matrix[irow][2] = ev[ista].rtzGxy[14][it] * Rnorm * wt;  /* Myy */
                        a_matrix[irow][3] = ev[ista].rtzGxy[12][it] * Rnorm * wt;  /* Mxy */
                        a_matrix[irow][4] = ev[ista].rtzGxy[13][it] * Rnorm * wt;  /* Mxz */
                        a_matrix[irow][5] = ev[ista].rtzGxy[15][it] * Rnorm * wt;  /* Myz */
                        a_matrix[irow][6] = 0;                                     /* Mzz */
		  }
		  else if( mtdegfree == FULL_MOMENT_TENSOR )
                  {
                        a_matrix[irow][1] = ev[ista].rtzGxy[11][it] * Rnorm * wt;  /* Mxx */
                        a_matrix[irow][2] = ev[ista].rtzGxy[14][it] * Rnorm * wt;  /* Myy */
                        a_matrix[irow][3] = ev[ista].rtzGxy[12][it] * Rnorm * wt;  /* Mxy */
                        a_matrix[irow][4] = ev[ista].rtzGxy[13][it] * Rnorm * wt;  /* Mxz */
                        a_matrix[irow][5] = ev[ista].rtzGxy[15][it] * Rnorm * wt;  /* Myz */
                        a_matrix[irow][6] = ev[ista].rtzGxy[16][it] * Rnorm * wt;  /* Mzz */
                  }
		  b_vector[irow]    = ev[ista].z.data_safe[it] * Rnorm * wt;
                  irow++;

                } /*** loop over it ***/

	} /*** loop over ista ***/

} /*** end of subroutine void make_amatrix_special() ***/
