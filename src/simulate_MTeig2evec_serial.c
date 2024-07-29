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
#include <unistd.h>

#include "../include/mteig.h"  /** global datatype and structure declarations **/

extern char progname[128];

void simulate_MTeig2evec_serial(
		int nsim_eig,
		int nsim_evec,
		Results *rbest, 
                int FixMyiz,
                Solution *sol,
                int nsta,
                EventInfo *ev,
                float **a_matrix,
                float *b_vector,
                int rows,
                int cols,
                int idump,
                float max_vred,
                float Mo,
                int seed,
                int Detail_Output,
                int verbose )
{
	int j = 0;

/*** functional prototypes ***/

	void simulate_MT_lite( 
		int nsim_evec, 
		Results *rbest, /*** &(rbest[irbest]) just one row ***/
		int nsta,
		EventInfo *ev,
		float **a_matrix,
		float *b_vector,
		int rows,
		int cols,
		float Mo,
		int seed,
		int verbose );

	void print_results( Results *r, FILE *fp );

	void print_results_all( Results *r, int nsim_eig, char *filename );

/*** start subroutine ***/
/*** eigenvalues: rbest[j].e0, e1, e2 ***/
/*** precomputed: rbest[j].lune_lat, rbest[j].lune_lon ***/
/*** call subroutine simulate_MT_lite() to get random eigenvectors rbest.e[3].v[3] rbest[j].MT.ij ***/
/*** calls forward calc MT -> synthetics and %VR ***/

	for( j = 0; j < nsim_eig; j++ )
	{
		nsim_evec = rbest[j].nevec;
		simulate_MT_lite( 
			nsim_evec,
			&(rbest[j]),
			nsta,
			ev,
			a_matrix,
			b_vector,
			rows,
			cols,
			Mo,
			seed,
 			verbose );

		if(j%100) fprintf( stderr, "." );

	} /*** loop over j-th eigen-values ***/

	fprintf(stderr, "\n" );

	print_results_all( rbest, nsim_eig, "best.all.out" );
}

/***********************************************************************************/
/** serial version forward computing MT solutions for given eigveals and eigvecs ***/
/***********************************************************************************/

void simulate_MT_serial_OLD(
                int iteration, 
		Results *r,
		int ir,
		Results *rbest,
		int irbest,
                int FixMyiz,
		Solution *sol,
		int nsta,
		EventInfo *ev, 
                float **a_matrix,
		float *b_vector,
		int rows,
		int cols,
                int idump,
		float max_vred,
		float Mo,
		int seed, 
                int Detail_Output,
		FILE *fpall,
		int verbose )
{
/*** local variables ***/
	int i, iz; 
	float x_vector[7];
	Tensor M;
	MomentTensor Ma, Mn;
	int mtdegfree = 6;

/*** eigenvalues ***/
	Eigenvalue e[3] = { { 0, { 0, 0, 0 }, 0, 0 },
                            { 0, { 0, 0, 0 }, 0, 0 },
                            { 0, { 0, 0, 0 }, 0, 0 } };
/*** sorted eig ***/
        Eigenvalue e_sort[3] = { { 0, { 0, 0, 0 }, 0, 0 },
                                 { 0, { 0, 0, 0 }, 0, 0 },
                                 { 0, { 0, 0, 0 }, 0, 0 } };	
/*** subroutines ***/

	void randomEigVec( Eigenvalue *e, int seed, int verbose );
        void Eig2MT( Eigenvalue *e, Tensor *M );
        void eig_sort3( Eigenvalue *e, Eigenvalue *e_sort, int verbose );
	void transfer_solution( Results *r, int k, Solution *sol, int iz, Eigenvalue *e, Eigenvalue *e_sort );

        void search_best_results( Results *r, int nsim_evec, Results *rbest, int irow_best );
	void print_results( Results *r, FILE *fp );

	void set_moment_tensor( MomentTensor *Ma, float *x, int idf, int verbose );
        void normalize_moment_tensor( MomentTensor *Ma, MomentTensor *Mn, float Mtotal, int verbose );
        void set_solution( Solution *sol, MomentTensor Ma, MomentTensor Mn );
        void mt2eig( MomentTensor Mn, Solution *sol, int iz, int verbose );
        void eig2iso( Solution *sol, int iz, int verbose );
        void Eig2MajorDC( Solution *sol, int iz, int verbose );
        void eig2lune_4mtinv( Solution *sol, int iz, int verbose );
        void eig2lune( float *eig, float *latitude, float *longitude, int verbose );

	float var_red;
	float forward_calc_from_MT(
			int iteration,
                        int nsta,
                        EventInfo *ev,
                        float *x_vector,
                        float **a_vector,
                        float *b_vector,
                        int rows,
                        int cols,
                        int idump,
                        float max_vred,
                        int verbose );


/*************************************************************************/
/*** begin subroutine ***/
/*************************************************************************/

	e[0].u = rbest[irbest].e0;
        e[1].u = rbest[irbest].e1;
        e[2].u = rbest[irbest].e2;

	randomEigVec( e, seed, verbose );

	for( i = 0; i < 3; i++ ) e[i].u *= Mo;
	Eig2MT( e, &M );

/*********************************************************/
/*** forward calc matrix multiply m * A = syn          ***/
/*** compute variance reduction  d-syn                 ***/
/*** model vector x_vector[7] is the moment tensor Mij ***/
/*********************************************************/

	x_vector[1] = M.xx;
	x_vector[2] = M.yy;
	x_vector[3] = M.xy;
	x_vector[4] = M.xz;
	x_vector[5] = M.yz;
	x_vector[6] = M.zz;

	var_red = forward_calc_from_MT( iteration, nsta, ev,
            x_vector, a_matrix, b_vector, rows, cols, idump, max_vred, verbose );

	if(verbose)
		fprintf( stderr, "%s: var_red=%g\n", progname, var_red );

/*********************************************************************************************/
/***** To save compute time, we should move these to end of search_best_results          *****/
/*****          and only run then instead after each iteration                           *****/
/*********************************************************************************************/

	iz = FixMyiz;
	
	if( Detail_Output )
	{
		/*** set and normalize the moment tensor ***/
		set_moment_tensor( &Ma, x_vector, mtdegfree, verbose );

	/*******************************************************************/
	/*** decompose the moment tensor                                 ***/
	/***    mtinv_subs.c: mt2eig( Ma, sol, iz, verbose );            ***/
	/*** we don't need to do this since we are already starting with ***/
	/***  the eigenvalues and eigenvectors. Transfer below           ***/
	/*******************************************************************/

		sol[iz].FullMT.eig[1].val = e[0].u / Mo;
                sol[iz].FullMT.eig[2].val = e[1].u / Mo;
                sol[iz].FullMT.eig[3].val = e[2].u / Mo;

                sol[iz].FullMT.eig[1].vec[1] = e[0].v[0];
                sol[iz].FullMT.eig[1].vec[2] = e[0].v[1];
                sol[iz].FullMT.eig[1].vec[3] = e[0].v[2];

                sol[iz].FullMT.eig[2].vec[1] = e[1].v[0];
                sol[iz].FullMT.eig[2].vec[2] = e[1].v[1];
                sol[iz].FullMT.eig[2].vec[3] = e[1].v[2];

                sol[iz].FullMT.eig[3].vec[1] = e[2].v[0];
                sol[iz].FullMT.eig[3].vec[2] = e[2].v[1];
                sol[iz].FullMT.eig[3].vec[3] = e[2].v[2];

	/*************************************************************************/
        /*** calculate the isotropic component, deviatoric MT and source types ***/
        /*** mtinv_subs.c: eig2iso( sol, iz, verbose );                        ***/
        /*************************************************************************/
                /* eig2iso_mteig_wrapper_();*/

                /*** normalize all moment tensor and eigenvalues, sol[iz].Mtotal previously set above ***/
                normalize_moment_tensor( &Ma, &Mn, sol[iz].Mtotal, verbose );

                set_solution( &(sol[iz]), Ma, Mn );

                Eig2MajorDC( sol, iz, verbose );

                /* Eig2MinorDC( sol, iz, verbose ); */
                eig2lune_4mtinv( sol, iz, verbose );
	}
	else
	{
		sol[iz].FullMT.eig[1].val = e[0].u / Mo;
                sol[iz].FullMT.eig[2].val = e[1].u / Mo;
                sol[iz].FullMT.eig[3].val = e[2].u / Mo;
                
                sol[iz].FullMT.eig[1].vec[1] = e[0].v[0];
                sol[iz].FullMT.eig[1].vec[2] = e[0].v[1];
                sol[iz].FullMT.eig[1].vec[3] = e[0].v[2];
                
                sol[iz].FullMT.eig[2].vec[1] = e[1].v[0];
                sol[iz].FullMT.eig[2].vec[2] = e[1].v[1];
                sol[iz].FullMT.eig[2].vec[3] = e[1].v[2];
                
                sol[iz].FullMT.eig[3].vec[1] = e[2].v[0];
                sol[iz].FullMT.eig[3].vec[2] = e[2].v[1];
                sol[iz].FullMT.eig[3].vec[3] = e[2].v[2];

		sol[iz].Maj.T.az = 0;
                sol[iz].Maj.T.pl = 0;
                sol[iz].Maj.B.az = 0;
                sol[iz].Maj.B.pl = 0;
                sol[iz].Maj.P.az = 0;
                sol[iz].Maj.P.pl = 0;

                sol[iz].Maj.P1.s = 0;
                sol[iz].Maj.P1.d = 0;
                sol[iz].Maj.P1.r = 0;

                sol[iz].Maj.P2.s = 0;
		sol[iz].Maj.P2.d = 0;
                sol[iz].Maj.P2.r = 0;
	}
	
/*** set some variables ***/

        sol[iz].lune_lat = rbest[irbest].lune_lat;
        sol[iz].lune_lon = rbest[irbest].lune_lon;
        sol[iz].k        = rbest[irbest].k;
        sol[iz].T        = rbest[irbest].T;
        sol[iz].Mtotal   = rbest[irbest].Mtotal;

        sol[iz].FullMT.eig[1].val = e[0].u / Mo;
        sol[iz].FullMT.eig[2].val = e[1].u / Mo;
        sol[iz].FullMT.eig[3].val = e[2].u / Mo;

        sol[iz].mxx = M.xx/Mo;
        sol[iz].myy = M.yy/Mo;
        sol[iz].mzz = M.zz/Mo;
        sol[iz].mxy = M.xy/Mo;
        sol[iz].mxz = M.xz/Mo;
        sol[iz].myz = M.yz/Mo;
	
	eig_sort3( e, e_sort, verbose );

/*** transfer MT solution/source type pars to results structure ***/

	transfer_solution( r, ir, sol, iz, e, e_sort );

/**** Print Results - eigenvectors ****/

	if( Detail_Output ) print_results( &(r[ir]), fpall );

} /*** end of simulate_MT_serial_OLD() ***/
