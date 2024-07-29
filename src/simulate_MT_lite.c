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

/********************************************************************************************/
/*** this gets used by both parallel-pthreads and serial optional function calls  ***/
/*** calculates fwd calc mt solution usign A-matrix, data vector b_vector and computes ***/
/*** variance reduction ***/
/********************************************************************************************/

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
                int verbose )
{

/*** local variables ***/
	int mtdegfree = 6;
	int idump = 0;
	float max_vred = 0;

/*** store eigenvector results ***/
	Results *r;
	int ir = 0;
	int i, iz;

        float x_vector[7];
        Tensor M;

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

	void transfer_solution_v2( float var_red, 
				float Mo, Tensor M,
				Results *r, int ir, Results *rbest, 
				Eigenvalue *e, Eigenvalue *e_sort );
				
	float var_red;
	float forward_calc_from_MT(
                        int iteration_id,
                        int nsta,
                        EventInfo *ev,
                        float *x_vector,
                        float **a_matrix,
                        float *b_vector,
                        int rows,
                        int cols,
                        int idump,
                        float max_vred,
                        int verbose );

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

		if(verbose) writeTensor( &M, "MT" );

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

        	var_red = forward_calc_from_MT( ir, nsta, ev,
			x_vector, a_matrix, b_vector, rows, cols, idump, max_vred, verbose );

		eig_sort3( e, e_sort, verbose );

	/*** transfer MT solution/source type pars to results structure ***/
		/* transfer_solution( r, ir, sol, iz, e, e_sort ); */
		transfer_solution_v2( var_red, Mo, M, r, ir, rbest, e, e_sort );

	} /*** ir-th loop over [1,...,nsim_evec] eigenvectors ***/
	
	search_best_results_row( r, nsim_evec, rbest );

	free(r);

} /*** end of simulate_MT_lite() ***/
