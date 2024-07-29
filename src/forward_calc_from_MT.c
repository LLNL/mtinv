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

#include "../include/nrutil.h"     /** numerical recipes **/
#include "../include/mt.h"         /** global datatype and structure declarations **/

extern char progname[128];

/***
  used by mteig.c for computing moment tensor variance reductions (fits) for 
  network sensitivity solution lune plots
***/

void create_Amatrix( Greens **grn,
                        EventInfo *ev,
                        int nsta,
                        int FixMyiz,
                        float **a_matrix,
                        float *b_vector,
                        int rows,
                        int cols,
                        int mtdegfree,
                        int Distance_Normalize,
                        float DistNormR0,
                        int verbose )
{
	int i, j;

	/*** subroutines ***/
	void make_amatrix(
                EventInfo *ev,
                Greens **grn,
                int nsta,
                int iz,
                float **a_matrix,
                float *b_vector,
                int mtdegfree,
                int Distance_Normalize,
                float DistNormR0 );

/*******************************************************/
/*** begin subroutine: initialize matrix and vectors ***/
/*******************************************************/

        for( i=1; i<=rows; i++ ) { b_vector[i] = 0; }
        for( j=1; j<=cols; j++ ) {
                for( i=1; i<=rows; i++ ) { a_matrix[i][j] = 0; }
	}

	make_amatrix( ev, grn, nsta, FixMyiz, a_matrix, b_vector, 
		mtdegfree, Distance_Normalize, DistNormR0 );

} /*** end of subroutine create_Amatrix() ***/




/***
  used by mteig.c for computing moment tensor variance reductions (fits) for
  network sensitivity solution lune plots
***/

/********
 see include/mt.h Mo = math.pow( 10.0, 1.5*(Mw+10.73) ) = 1.2445146117713818e+16; Reference Mw = 0.0
**********/

float forward_calc_from_MT(
			int iteration,
			int nsta,
			EventInfo *ev,
                        float *x_vector,
			float **a_matrix,
			float *b_vector,
			int rows,
			int cols,
			int idump,		
			float max_vred,
			int verbose )
{
	int i, j;
	int ista;
	float iz;
        float *s_vector;	/*** synthetic                s[1..rows]          ***/
	float var_red;

/*** idump = set this to dump the multiplex data and synthetic vector as SAC files ***/

/********************/
/*** subroutines ***/
/********************/
	float **matrix( int start_row, int stop_row, int start_col, int stop_col );
	float *vector( int start, int end );

        void free_matrix(float **m, int nrl, int nrh, int ncl, int nch );
        void free_vector(float *v, int nl, int nh );

	float variance_reduction( float *data, float *synthetic, int nstart, int nend );

	void matmul( int conj, float **bb, int nx, float *x, int ny, float *y );

	char sacfilename[256];
	void wrtnewsac( char *FO, float dt, int ns, float *ar, float b );

	EventInfo *evlocal;
	
/************************************************************************************************/
/*** initalize memory ***/
/************************************************************************************************/
	if(verbose)
	{
		fprintf( stdout, "%s: %s: %s: Allocating memory for evlocal nsta=%d\n", 
			progname, __FILE__, __func__, nsta );
	}
	evlocal = (EventInfo *) malloc( nsta*sizeof(EventInfo) );

/******************************************************************/
/*** form the moment tensor from solution vector x              ***/
/*** col1-Mxx, col2-Myy, col3-Mxy, col4-Mxz, col5-Myz, col6-Mzz ***/
/*** store the original moment tensor to solution structure     ***/
/******************************************************************/

	for( ista = 0; ista < nsta; ista++ )
	{
		/***normalized Mij ***/
		evlocal[ista].Mo = ev[ista].Mo;
		evlocal[ista].Mxx = x_vector[1] / evlocal[ista].Mo;
                evlocal[ista].Myy = x_vector[2] / evlocal[ista].Mo;
                evlocal[ista].Mxy = x_vector[3] / evlocal[ista].Mo;
                evlocal[ista].Mxz = x_vector[4] / evlocal[ista].Mo;
                evlocal[ista].Myz = x_vector[5] / evlocal[ista].Mo;
                evlocal[ista].Mzz = x_vector[6] / evlocal[ista].Mo;
		strcpy( evlocal[ista].stnm, ev[ista].stnm );
		strcpy( evlocal[ista].net, ev[ista].net );
		evlocal[ista].my_z = ev[ista].my_z;
		evlocal[ista].Mw = ev[ista].Mw;

	/*** these are meaningless here read from EV line in mtinv.par file ***/
        /*** ev[ista].evdp, ev[ista].str, ev[ista].dip, ev[ista].rak, ***/

		if(verbose)
		{
		  fprintf( stdout, "%s: %s: %s: ista=%d evlocal Mo=%g Mij=(%g, %g, %g, %g, %g, %g) stnm=%s net=%s my_z=%g Mw=%g\n",
			progname, __FILE__, __func__,
			ista, 
			evlocal[ista].Mo,
			evlocal[ista].Mxx,
			evlocal[ista].Myy,
			evlocal[ista].Mxy,
			evlocal[ista].Mxz,
			evlocal[ista].Myz,
			evlocal[ista].Mzz,
			evlocal[ista].stnm,
			evlocal[ista].net,
			evlocal[ista].my_z,
			evlocal[ista].Mw );
		  fflush(stdout);
		}
	}

        if(verbose) 
	{
	  fprintf(stdout, "%s: %s: %s: allocating memory for s_vector[rows=%d]\n",
                progname, __FILE__, __func__, rows );
	  fflush(stdout);
	}

/************************************************************************************************/
/*** this is the selected depth from check_depth() ****/
/************************************************************************************************/
	/* iz = FixMyiz; */

/*** allocate memory and initialize to zeros ***/
        /* mt = matrix( 0, cols+1, 0, cols+1 ); */

	s_vector  = vector( 0, rows+1 );
	for( i=1; i<=rows; i++ ) s_vector[i] = 0;

/******************************************************************/
/*** form the moment tensor from solution vector x              ***/
/*** col1-Mxx, col2-Myy, col3-Mxy, col4-Mxz, col5-Myz, col6-Mzz ***/
/*** store the original moment tensor to solution structure     ***/
/******************************************************************/

	for( ista = 0; ista < nsta; ista++ )
	{
		if(verbose)
                {
		  fprintf( stdout,
	"%s: %s: %s: ista=%d %s.%s my_z=%g Mo=%e Mw=%g Mij=(%g,%g,%g,%g,%g,%g) Mo=%g\n",
                        progname,
                        __FILE__,
                        __func__,
                        ista,
                        evlocal[ista].stnm,
                        evlocal[ista].net,
                        evlocal[ista].my_z,
			evlocal[ista].Mo,
                        evlocal[ista].Mw,
                        evlocal[ista].Mxx, evlocal[ista].Mxy, evlocal[ista].Mxz,
                        evlocal[ista].Myy, evlocal[ista].Myz, evlocal[ista].Mzz, evlocal[ista].Mo );
		}

	} /*** ista - loop over all nsta ***/

/**********************************************************************************/
/*** assumes ev[ista].Mij is normalized to the total moment units of Dynes*cm   ***/
/*** normalize here by the base_moment relative to Green's functions of Mw=0    ***/
/*** for forward calculation                                                    ***/
/**********************************************************************************/

/*******************************************************************/
/*** for computing synthetics requires unnormalized abs MT dyncm ***/
/*******************************************************************/
	ista = 0;
/***
        sol[iz].moment_tensor[1][1] = ev[ista].Mxx * ev[ista].Mo;
        sol[iz].moment_tensor[1][2] = ev[ista].Mxy * ev[ista].Mo;
        sol[iz].moment_tensor[1][3] = ev[ista].Mxz * ev[ista].Mo;
        sol[iz].moment_tensor[2][1] = ev[ista].Mxy * ev[ista].Mo;
        sol[iz].moment_tensor[2][2] = ev[ista].Myy * ev[ista].Mo;
        sol[iz].moment_tensor[2][3] = ev[ista].Myz * ev[ista].Mo;
        sol[iz].moment_tensor[3][1] = ev[ista].Mxz * ev[ista].Mo;
        sol[iz].moment_tensor[3][2] = ev[ista].Myz * ev[ista].Mo;
        sol[iz].moment_tensor[3][3] = ev[ista].Mzz * ev[ista].Mo;
***/

/******************************************************************/
/*** form the moment tensor from solution vector x              ***/
/*** col1-Mxx, col2-Myy, col3-Mxy, col4-Mxz, col5-Myz, col6-Mzz ***/
/*** store the original moment tensor to solution structure     ***/
/******************************************************************/

	x_vector[1] = evlocal[ista].Mxx * (evlocal[ista].Mo/base_moment);
        x_vector[2] = evlocal[ista].Myy * (evlocal[ista].Mo/base_moment);
        x_vector[3] = evlocal[ista].Mxy * (evlocal[ista].Mo/base_moment);
        x_vector[4] = evlocal[ista].Mxz * (evlocal[ista].Mo/base_moment);
        x_vector[5] = evlocal[ista].Myz * (evlocal[ista].Mo/base_moment);
        x_vector[6] = evlocal[ista].Mzz * (evlocal[ista].Mo/base_moment);

/****************************************************/
/*** multiply solution x vector with a matrix or  ***/
/*** greens function matrix to get the s vector   ***/
/*** or synthetics vector then....                ***/
/*** compute the variance reduction and           ***/
/*** l2_norm error                                ***/
/****************************************************/

	matmul( 0, a_matrix, cols, x_vector, rows, s_vector );

	/* sol[iz].var_red = variance_reduction( b_vector, s_vector, 1, rows+1 ); */
	var_red = variance_reduction( b_vector, s_vector, 1, rows+1 );

	if(verbose)
	{
		fprintf( stdout, "%s: %s: %s: vred=%g\n",
                	progname, __FILE__, __func__, var_red );
	}

	if( ( idump == 1 ) && ( var_red > max_vred ) ) 
	{

	/*** solution vector, a-matrix * x-matrix ***/

		sprintf( sacfilename, "s_vector.%08.3f.%08d.sac", var_red, iteration );
		wrtnewsac( sacfilename, 1.0, rows, s_vector, 0 );

	/*** the data matrix, preshift? ***/

		sprintf( sacfilename, "b_vector.%08.3f.%08d.sac", var_red, iteration );
		wrtnewsac( sacfilename, 1.0, rows, b_vector, 0 );

		if(verbose)
		{
		  fprintf( stdout,
	"%s: %s: %s: iteration=%d idump=%d is on, writting files s_vector.sac b_vector_sac\n",
			progname, __FILE__, __func__, iteration, idump );
		}
	}

	free_vector( s_vector, 0, rows+1 );
	free( evlocal );

	return (float) var_red;

} /*** end of subroutine forward_calc_from_MT.c ***/
