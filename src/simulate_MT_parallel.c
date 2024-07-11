#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include <pthread.h>    /* POSIX Threads */

#include "../include/mteig.h"  /** global datatype and structure declarations **/

extern char progname[128];

#define NUM_MAX_THREADS 16

typedef struct thread_data
{
	int nsim_evec;
	Results *rbest;
	int nsta;
	EventInfo *ev;
	float **a_matrix;
	float *b_vector;
	int rows;
	int cols;
	float Mo;
	int seed;
	int verbose;
} ThreadData;

void simulate_MT_parallel(
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
                float Mo,
                int seed,
                int Detail_Output,
		int nthreads,
                int verbose )
{
	pthread_t thread[NUM_MAX_THREADS];
	ThreadData *td;
	pthread_attr_t attr;

	int num_sim_eig_per_thread;
	int ir, i_rbest_row;
	int iter, ithread;
	int j;

	void *simulate_MT_lite_parallel_wrapper( void *ptr );
	void print_results_all( Results *r, int nsim_eig, char *filename );
        void print_results( Results *r, FILE *fp );

	int SLEEP_INTERVAL_TIME_SEC = 1; /*** unistd.h unsigned int sleep ( unsigned int seconds ) ***/

/*********************************************************************/
/*** start the pthreads                                            ***/
/*********************************************************************/

	if(verbose)
	{
		fprintf( stdout,
  "%s: %s: %s: nsim_eig=%d nsim_evec=%d FixMyiz=%d nsta=%d rows=%d cols=%d Mo=%e seed=%d Detail_Output=%d verbose=%d\n",
			progname,
			__FILE__,
			__func__,
			nsim_eig,
			nsim_evec,
			FixMyiz, nsta, rows, cols, 
			Mo,
			seed, Detail_Output, verbose );
	}

	td = malloc( nsim_eig * sizeof( ThreadData ) );

	for( ir = 0; ir < nsim_eig; ir++ )
	{
		/* td[ir].nsim_evec = nsim_evec; */
		td[ir].nsim_evec = rbest[ir].nevec;
		td[ir].rbest     = &(rbest[ir]);  /*** &(rbest[ir]) just one row or whole array? ***/
		td[ir].nsta      = nsta;
		td[ir].ev        = ev;
		td[ir].a_matrix  = a_matrix;
		td[ir].b_vector  = b_vector;
		td[ir].rows      = rows;
		td[ir].cols      = cols;
		td[ir].Mo        = Mo;
		td[ir].seed      = seed;
		td[ir].verbose   = verbose;

		if(verbose)
		{
			fprintf( stdout,
			"%s: %s: %s: td[ir=%d] nsim_evec=%d nsta=%d rows=%d cols=%d Mo=%e seed=%d\n", 
                		progname,
                		__FILE__,
                		__func__,
                		ir,
				td[ir].nsim_evec, 
				td[ir].nsta, 
				td[ir].rows, 
				td[ir].cols,
				td[ir].Mo, 
                		td[ir].seed );
			fflush( stdout );
		}

	}

	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );

	if( nthreads > NUM_MAX_THREADS ) nthreads = NUM_MAX_THREADS;

	num_sim_eig_per_thread = rint( nsim_eig / nthreads ) + 1;

	if(verbose)
	{
		fprintf( stdout, "%s: %s: %s: pthread_create(): num_sim_eig_per_thread = %d nsim_eig = %d nthreads = %d\n",
			progname, __FILE__, __func__, num_sim_eig_per_thread, nsim_eig, nthreads );
		fflush( stdout );
	}

	for( iter = 0; iter < num_sim_eig_per_thread; iter++ )
	{
		for( ithread = 0; ithread < nthreads; ithread++ )
		{
			i_rbest_row = iter + ithread * num_sim_eig_per_thread;

			if( i_rbest_row > nsim_eig-1 ) 
			{
				break;
			}
			else
			{
			  if(verbose)
			  {
			    fprintf( stdout,
			      "%s: %s: %s: pthread_create(): iter=%d ithread=%d i_rbest_row=%d\n",
				progname, __FILE__, __func__, iter, ithread, i_rbest_row );
			    fflush( stdout );
			  }
			}

			pthread_create(
				&thread[ithread], 
				&attr,
				simulate_MT_lite_parallel_wrapper,
				&(td[i_rbest_row]) );
		}

        /******************************/
        /*** JOIN the pthreads      ***/
        /******************************/

		for( ithread = 0; ithread < nthreads; ithread++ )
		{
			i_rbest_row = iter + ithread * num_sim_eig_per_thread;
			
			if( i_rbest_row > nsim_eig-1 )
			{
				break;
			}
			else
			{
			  if(verbose)
			  {
			    fprintf( stdout,
			      "%s: %s: %s: pthread_join(): iter=%d ithread=%d i_rbest_row=%d num_sim_eig_per_thread=%d\n",
                                progname, __FILE__, __func__, iter, ithread, i_rbest_row, num_sim_eig_per_thread );
			    fflush( stdout );
			  }
			}
			pthread_join( thread[ithread], NULL );

			/* sleep(SLEEP_INTERVAL_TIME_SEC); */
		}
		if(iter%100) fprintf( stderr, "." );
	}
	fprintf(stderr, "\n" );

/*** write output ***/
	fprintf( stderr, "%s: %s: %s: done with pthreads writting output, writting file best.all.out\n",
		progname, __FILE__, __func__ );
	fflush( stderr );

	print_results_all( rbest, nsim_eig, "best.all.out" );

	pthread_attr_destroy( &attr );
	free(td);
}

void *simulate_MT_lite_parallel_wrapper( void *ptr )
{

/*** external variables ***/
	ThreadData *td;

/*** local variables ***/
	int nsim_evec;
	Results *rbest;
	int nsta;
	EventInfo *ev;
	float **a_matrix;
	float *b_vector;
	int rows;
	int cols;
	float Mo;
	int seed;
	int verbose;

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

/***********************************************************************************************/
/*** start ***/
/***********************************************************************************************/

	td = (ThreadData *) ptr;

	nsim_evec = (int)         td->nsim_evec;
	rbest     = (Results *)   td->rbest;
	nsta      = (int)         td->nsta;
	ev        = (EventInfo *) td->ev;
	a_matrix  = (float **)    td->a_matrix;
	b_vector  = (float *)     td->b_vector;
	rows      = (int)         td->rows;
	cols      = (int)         td->cols;
	Mo        = (float)       td->Mo;
	seed      = (int)         td->seed;
	verbose   = (int)         td->verbose;

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: nsim_evec=%d nsta=%d rows=%d cols=%d Mo=%g seed=%d\n",
		progname,
		__FILE__,
		__func__,
		nsim_evec,
		nsta,
		rows,
		cols,
		Mo, 
		seed );
	  fflush(stdout);
	}

	simulate_MT_lite(
		nsim_evec,
		rbest,
		nsta,
		ev,
		a_matrix,
		b_vector,
		rows,
		cols,
		Mo,
		seed,
		verbose );

	return(void *)NULL;
}
