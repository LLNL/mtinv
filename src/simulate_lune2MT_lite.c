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


/*** this test code only computes the MT values given a set of lune params ***/
/*** there are no forward MT calc ***/

void create_lune2mt_grid(
                int nsim_eig,
                int nsim_evec,
                Results *rbest,
                float Mo,
                int seed,
                int verbose )
{
        int j = 0;

/*** functional prototypes ***/
/*** &(rbest[irbest]) just one row ***/

        void simulate_lune2MT_lite(
                int nsim_evec,
                Results *rbest,
                float Mo,
                int seed,
                int verbose );

	void print_results_lune2mt( Results *r, int nrows, char *filename );

/*** start subroutine ***/
	
	for( j = 0; j < nsim_eig; j++ )
        {
                nsim_evec = rbest[j].nevec;
                simulate_lune2MT_lite( nsim_evec, &(rbest[j]), Mo, seed, verbose );
        }

        print_results_lune2mt( rbest, nsim_eig, "lune2mt.csv" );

} /* end of create_lune2mt_grid() ***/



/*** this test code only computes the MT values given a set of lune params ***/
/*** there are no forward MT calc, see simulate_MT_lite.c instead ***/
/*** &(rbest[irbest]) just one row ***/

void simulate_lune2MT_lite( int nsim_evec, Results *rbest, float Mo, int seed, int verbose )
{
/*** store eigenvector results ***/
        Results *r;
        int ir = 0;
        int i;
	int iz;

	float var_red;
        Tensor M;
	float x_vector[7];
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

        void writeTensor( Tensor *M, char *label );

        void eig_sort3( Eigenvalue *e, Eigenvalue *e_sort, int verbose );

	/*** we are trying to not use Solution structure ***/
	/* 
	void transfer_solution(
			Results *r,
			int k,
			Solution *sol,
			int iz,
			Eigenvalue *e,
			Eigenvalue *e_sort );
	*/

	/*** this version does not transfer the MT elements because it is too slow ***/
        void transfer_solution_v2(
			float var_red,
			float Mo,
			Tensor M,
			Results *r, 
			int ir,
			Results *rbest,
			Eigenvalue *e,
			Eigenvalue *e_sort );

	/*** this version transfers the MT elements ***/
	/*** for lune2mt.c does not compute fwd MT calc ***/

/*** rbest is array ***/
        void search_best_results( Results *r, int nsim_evec, Results *rbest, int irow_best );

/*** rbest is ptr to array row ***/
        void search_best_results_row( Results *r, int nsim_evec, Results *rbest );

        void print_results( Results *r, FILE *fp );

/*************************************************************************/
/*** begin subroutine ***/
/*************************************************************************/

/*** allocate memory to store MT forward calculation results for each eigenvector set ***/

        r = (Results *) calloc( nsim_evec, sizeof(Results) );

        for( ir = 0; ir < nsim_evec; ir++ )
        {

        /*** set the precomputed eigenvalues ****/
                e[0].u = rbest->e0;
                e[1].u = rbest->e1;
                e[2].u = rbest->e2;

        /**** generate random uniform spherical distributed ***/
                randomEigVec( e, seed, verbose );

        /*** scale eigenvalues by Mo ***/
                for( i = 0; i < 3; i++ )
                        e[i].u *= Mo;

        /*** compute Moment Tensor M using eigenval/vectors e ***/
                Eig2MT( e, &M );

                if(verbose)writeTensor( &M, "MT" );

	/*** this code doesn't calculate variance reduction ***/
        	/*** var_red = forward_calc_from_MT( ) ***/

        /*******************************************************************/
        /*** decompose the moment tensor                                 ***/
        /***    mtinv_subs.c: mt2eig( Ma, sol, iz, verbose );            ***/
        /*** we don't need to do this since we are already starting with ***/
        /***  the eigenvalues and eigenvectors. Transfer below           ***/
        /*******************************************************************/

	/***
		set_moment_tensor( &Ma, x_vector, mtdegfree, verbose );
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
	***/

	/*************************************************************************/
        /*** calculate the isotropic component, deviatoric MT and source types ***/
        /*** mtinv_subs.c: eig2iso( sol, iz, verbose );                        ***/
        /*************************************************************************/
                /* eig2iso_mteig_wrapper_();*/

	/*** normalize all moment tensor and eigenvalues, sol[iz].Mtotal previously set above ***/
	/***
		normalize_moment_tensor( &Ma, &Mn, sol[iz].Mtotal, verbose );
		set_solution( &(sol[iz]), Ma, Mn );
		Eig2MajorDC( sol, iz, verbose );
		eig2lune_4mtinv( sol, iz, verbose );
	***/

	/*** set some variables ***/
	/***
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
	***/

	        eig_sort3( e, e_sort, verbose );

        /*** transfer MT solution/source type pars to results structure ***/

		/*** transfer_solution( r, ir, sol, iz, e, e_sort ); ***/

		var_red = 100;
                transfer_solution_v2( var_red, Mo, M, r, ir, rbest, e, e_sort );

        } /*** ir-th loop over [1,...,nsim_evec] eigenvectors ***/

	search_best_results_row( r, nsim_evec, rbest );

        free(r);

} /*** end of simulate_lune2MT_lite() ***/
