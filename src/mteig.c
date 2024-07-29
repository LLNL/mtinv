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

/*********************************************************************************************************/
/* 
mteig.c - G. Ichinose Thu Nov 21 19:10:59 PST 2019

 Creates MT Network Sensitivity Solution (NSS) based on Ford et al. First use mtinv inversion toolkit 
 to determine best fitting MT. NSS is max variance reduction for points in source type space Tape&Tape2012
 (Lune latitude,longitude) or Hudson et al. 1989 (k,T) eigenvalues and eigenvecters are sample randomly
 using spherical uniform distribution.  Best use 1000 to 2000 random eigenvalue samples and 2000 to
 +3000 random eigenvector samples   (2M - 16M total iterations)

Major Changes:

Fri Jul 24 21:26:01 PDT 2020
	Removed Solution *sol data structure from innner routines, because they threads were overwritting
        memory variables so used Results-data structure as storage, Same for EventInfo *ev created a
        evlocal variable for local reading and writting so to not x-contaminate other threads 
	Added plot_lune2.csh C-shell script GMT plotting
	Added user supplied eig/MT and special lune src-type points (ie. DC,+/-CLVD,+/-ISO, +/-crack)

Mon Jul 27 23:08:56 PDT 2020
        allow nsim_evec to vary as a function of (lat, or distance from DC) using vonMises-Fisher probability
	distribution (kappa=1.3 e.g., DC-8000evecs to (+/-)ISO 2000evecs)  given that the diversity of mechs 
        becomes less important as iso/clvd/dc % budget decreases away from origin (DC).

Wed Jul 29 23:44:09 PDT 2020
	added deviatoric disc and search for best fitting devmt solution

Tue Aug  4 00:17:38 PDT 2020
	added second GMT lune plot with normalized percent VR

To Do List ...
1.	add back the MajDC subroutine to compute PTB axes information on rbest array (low priority, no urgent need)
2.      vMF distribution or normal disribution random deviates of eigenvalues instead of uniform spherical

*/
/********************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include "../include/mteig.h" /** global datatype and structure declarations **/

char progname[128];
long evid;  /*** user supplied eventID, mtinv has this ***/

int main( int ac, char **av )
{
/************************/
/*** event info stuff ***/
/************************/
        EventInfo *ev;
        int ista, nsta;
        char evinfo_filename[128];

/************************/
/*** Greens stuff     ***/
/************************/
        Greens **grn;
        int nz,iz;
        float *z;  /*** for backwards compat... also access by z[ista].z ***/
	DepthVector *zvec;  /*** see ../include/mt.h ***/
	
	int Detail_Output = 0;   /*** Detail_Output = 0  :: do not compute TPB-ax and P1/P2: ***/
				/*** SDR for each evec iteration ***/
/****************/
/*** Solution ***/
/****************/
        Solution *sol;

/*** if idump==1 switch on then var_red > max_vred get vector dump of syn adnd data as SAC files ***/
	int idump = 0;
	float max_vred; 

	float ts0 = 0; /***assumes data already shifted to origin-time of zero shift ***/
	int FixMyiz;
	int check_on_status_ok;
	float FixMyZ;
	int Distance_Normalize = 0;
	float DistNormR0 = 1; /*** default is R0 = 1 km  in R/R0 ****/

/************************/
/*** local variables ***/
/************************/

	Results *r; /*** size is nsim_evec; this holds all results for single ***/
                    /*** createlambda() iteration randomEigVec() ***/

	Results *rbest; /*** size is nsim_eig; from all r[] this is the best ***/
			/*** fitting (highest var_red) for each createlambda() run ***/
	
	void print_results( Results *r, FILE *fp ); /*** just prints ptr to 1-row ***/

	void print_results_all( Results *r, int nrows, char *filename ); /*** prints all ***/

	void print_result_row_pretty_screen( Results *r, FILE *fp, char *label );

	Eigenvalue e[3] = { { 0, { 0, 0, 0 }, 0, 0 }, 
			    { 0, { 0, 0, 0 }, 0, 0 },
			    { 0, { 0, 0, 0 }, 0, 0 } };

/*** sorted eig ***/
	Eigenvalue e_sort[3] = { { 0, { 0, 0, 0 }, 0, 0 }, 
                                 { 0, { 0, 0, 0 }, 0, 0 },
                                 { 0, { 0, 0, 0 }, 0, 0 } };

	float Mo = 1.0;
	int i, j, k;
	int nsim_eig = 2000;
	int nsim_evec= 4000;  /*** obsolete due to eigvec_fac, still used as a place holder ***/
	int eigvec_fac = 17000; /*** von Mises-Fisher PDF default nsim_evec range 1000-8000 kappa=1.3 ***/
	int sum = 0;

/*** variable nsim_evec as a function of lunelat/lon vMF3d PDF distrib ***/
	int nevec;
	int iteration = 0;
	Tensor M;
	int seed = 1;   /*** default use random seed on each run ***/
	int verbose = 0;
	int parallel = 1; /*** default run in parallel mode, see nthreads=? and eigvec_fac=? nsim_eig and nsim_evec ***/
	int nthreads = 4;

	char pathname[128], currentdir[128];
	FILE *fpall, *fpbest, *fpeig;

/*** this adds the DC (0,0) and +iso(0,90) and -iso (0,-90) DEFAULT-off ***/

	int Add_DC_iso = 1;   /*** default is to add these lune points to search ***/

	int itag, Ntags=5;   /*5: 0=DC 1=+iso 2=-iso 3=+CLVD 4=-CLVD */
	void Add_DC_iso_2eigenvals( int itag, Results *rbest, int eigvec_fac, float Mo );

/*** adds a user supplied moment tensor ***/
	int Add_user_MT = 0;
	Tensor Musr;
/***
	void MT2eig( Tensor *Musr, float *pdc, float *pclvd, float *piso, float *pdev, 
		     float *lamb1, float *lamb2, float *lamb3, float *lune_lat, float *lune_lon, 
		     float *hudson_k, float *hundson_T );
***/

/*** adds a user supplied eigenvalues ***/
	int Add_user_Eig = 0;
	float e_user[3];

	void abs_sort( float *e, int verbose );

	void scalevec( float *x, float sc );

	void eig2eig( float *lamb1, float *lamb2, float *lamb3,
                float *percent_dc, float *percent_clvd, float *percent_iso, 
                float *lune_lat, float *lune_lon, float *hudson_k, float *hudson_T, 
		float *gcarc_dc, float *gcarc_exp, int *nevec, int eigvec_fac, int verbose );

/**********************************/
/*** Internal Local subroutines ***/
/**********************************/

        float pdc = 0, pclvd = 0, piso = 0;
        float e0 = 0, e1 = 0, e2 = 0;
        float lune_lat, lune_lon, hudson_k, hudson_T;
	float gcarc_dc = 0, gcarc_exp = 0;

        void createlamb(
                float *pdc,
		float *pclvd,
		float *piso,
                float *lamb1, float *lamb2, float *lamb3,
                float *lune_lat, float *lune_lon,
		float *hudson_k, float *hudson_T,
		float *gcarc_dc, float *gcarc_exp, 
                int seed,
		int *nevec,
		int eigvec_fac,
		int verbose );

	float dMo = 0.05;  /*** preset the spacing on the deviatoric disc ***/
	Results *create_clvd_eigs( Results *rbest, int *nsim_eig, int eigvec_fac, float Mo, float dMo, int verbose );

/*** these subroutines do all the forward calculations and outputs the %VR for each eigval-eigvec (lat,lon) set ***/

/*** serial version ***/
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
                int verbose );

/*** pthreads version ***/
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
                        int verbose );

/*** create a gmt plot of the results ***/
	int igmt5 = 1; /*** default yes, use GMT v5+ ***/

	void gmtplot( 
		Results *rbest,
		int nsim_eig,
		int color_or_gray,
		int doplt,
		char *title_text,
		int Add_user_Eig,
		int Add_user_MT,
		int Add_DC_iso,
		int do_norm_gmt_plot,
		int igmt5 );

	int doplt = 1; /*** default system call run C-shell GMT script and display using open jpg MacOSX ***/
	int do_norm_gmt_plot = 0;

	int color_or_gray = 1; /*** default do color, makecpt rainbow (=0 for grayscale) ***/

	int REDO_GMT_PLOT_ONLY = 0;

/*** load the output results and redo the plots ***/
	Results *read_results( char *filename, int *nsim_eig );

/********************************/
/*** Old External subroutines ***/
/********************************/

	int mtdegfree = 6;
	int rows = 0, cols = 0;
	float **a_matrix;	/*** A matrux with dimensions a[1..rows][1..cols] ***/
	float *b_vector;	/*** data                     b[1..rows]          ***/
		
	float **matrix( int start_row, int stop_row, int start_col, int stop_col );
        float *vector( int start, int end );

        void free_matrix(float **m, int nrl, int nrh, int ncl, int nch );
        void free_vector(float *v, int nl, int nh );

	void create_Amatrix( Greens **grn,
                        EventInfo *ev,
                        int nsta,
                        int FixMyiz,
                        float **a_matrix,
                        float *b_vector,
			int row, 
			int cols,
                        int mtdegfree,
                        int Distance_Normalize,
                        float DistNormR0,
                        int verbose );

/******************************/
/*** subroutines from mtinv ***/
/******************************/

/*** glib2inv_subs.c ***/
        EventInfo *glib2inv_get_input_parameters( char *filename, EventInfo *ev, int *n, int verbose );

/*** mtinv_subs.c ***/
        void load_the_data( EventInfo *ev, int nsta, float ts0, int verbose );

/*** mtinv_subs.c ***/
/*** deprecated ***/
        /* float *load_greens( EventInfo *ev, Greens **grn, int nsta, int *nz_tmp, int verbose ); */
/*** Greens_subs.c:loadGlibAll() ****/
	Greens **loadGlibAll( Greens **grn, EventInfo *ev, DepthVector *z, int nsta, char *file_type, int verbose );

/*** check_depths.c ***/
        void check_depth( float FixMyZ, int *FixMyiz, float *z, int nz, int verbose );

/*** shorten_path.c ***/
	char *shorten_path( char *pathname, char *filename );

/************************/
/*** misc             ***/
/************************/
        int setpar(int,char **);
	int mstpar( char *, char *, void * );
        int getpar( char *, char *, void * );
        void endpar();
	void Usage_Print( void );	
	char title_text[256];

/******************************/
/*** start program ***/
/******************************/
	strcpy( progname, av[0] );
	strcpy( pathname, progname );
	shorten_path( pathname, progname );
	getcwd( currentdir, 128 );

	if( verbose )
        {
          fprintf( stdout, "%s: %s: %s: STDOUT: Version=%s ReleaseDate=%s exec full path=%s\n",
                progname,
                __FILE__,
                __func__,
                Version_Label, Version_Date, pathname );
        }

        fprintf( stderr, "%s: %s: %s: STDERR: Version=%s ReleaseDate=%s exec full path=%s\n",
                progname,
                __FILE__,
                __func__,
                Version_Label, Version_Date, pathname );

/*************/
/*** USAGE ***/
/*************/
	if( ac <= 1 ) Usage_Print();

/*** get command line parameters ***/

	setpar(ac,av);

	getpar( "gmt_only", "b", &REDO_GMT_PLOT_ONLY );
	getpar( "gmt5",     "b", &igmt5 );

	getpar( "verbose",  "b", &verbose);
	getpar( "parallel", "b", &parallel );
	getpar( "nthreads", "d", &nthreads );
	getpar( "eigvec_fac", "d", &eigvec_fac );

/*** fractional moment for search points on deviatoric disc ***/
/*** override the spacing of search points on deviatoric disc ***/
	getpar( "dMo", "f", &dMo );
	if( dMo <= 0 ) dMo = 0.001;
	if( dMo >= 1.0 ) dMo = 0.5;

	getpar( "color", "b", &color_or_gray );
	getpar( "doplt", "b", &doplt );
	getpar( "do_norm_gmt_plot", "b", &do_norm_gmt_plot );

/*** this is the same as par file for mtinv version 4.0.0 ***/
	mstpar( "par", "s", &evinfo_filename );

/*** select the depth out of the Green's func library ***/
	mstpar( "fixz", "f", &FixMyZ );	 /*** required parameter ***/
	mstpar( "Mo", "f", &Mo );         /*** required parameter ***/
	mstpar( "nsim_eig", "d", &nsim_eig );
        getpar( "nsim_evec", "d", &nsim_evec );
        getpar( "seed", "d", &seed );

	strcpy( title_text, "Title Goes Here" );
	getpar( "title", "s", title_text );

	getpar( "norm", "b", &Distance_Normalize );
        if( Distance_Normalize )
        {
                mstpar( "R0", "f", &DistNormR0 );
        }

/*** write SAC output b_vector, s_vector for %VR > max_vred ***/
	getpar( "dump", "b", &idump );
	if( idump )
	{
		mstpar( "max_vred", "f", &max_vred );
	}

	getpar( "Add_DC_iso", "b", &Add_DC_iso );

	getpar( "Add_user_Eig", "b", &Add_user_Eig );
	if( Add_user_Eig )
	{ 	/*** required parameters if option selected ***/
		mstpar("e0", "f", &(e_user[0]) );
		mstpar("e1", "f", &(e_user[1]) );
		mstpar("e2", "f", &(e_user[2]) );
	}

	endpar();

/*** just read back in the results and redo the plots ***/

	if( REDO_GMT_PLOT_ONLY )
        {
		rbest = read_results( "best.all.out", &nsim_eig );

		fprintf( stderr, "%s: %s: %s: nsim_eig=%d\n", progname, __FILE__, __func__, nsim_eig );

		Add_user_Eig = 1;
		Add_user_MT = 0;
		Add_DC_iso = 1;
		doplt = 1;
		color_or_gray = 1;

                gmtplot(rbest, nsim_eig, color_or_gray,
			doplt, title_text, Add_user_Eig,
			Add_user_MT, Add_DC_iso, do_norm_gmt_plot, igmt5 );

                exit(0);
        }

	if( parallel ) idump=0; /*** turn off SAC file dumping/writting in pthreads version ***/

/**** start processing ***/

	fprintf( stderr, "%s: %s: %s: STDERR:\n", progname, __FILE__, __func__ );

	fprintf( stderr, "%s: %s: %s: STDERR: START! Mo=%e Zkm=%g nsim_eig=%d nsim_evec=%d total-iter=%d\n",
		progname, __FILE__, __func__, Mo, FixMyZ, nsim_eig, nsim_evec, (nsim_eig*nsim_evec) );

	fprintf( stderr, "%s: %s: %s: STDERR:          currentdir=%s\n", progname, 
		__FILE__, __func__, currentdir );

	fprintf( stderr, "%s: %s: %s: STDERR:\n", progname, __FILE__, __func__ );

	if(idump)
	{
	  fprintf( stderr,
	    "%s: %s: %s: STDERR: writting out data and synthetic vectors as SAC files when max_vred>%g\n",
		progname, __FILE__, __func__, max_vred );
	}
		
	fprintf( stderr, "%s: %s: %s: STDERR:\n", progname, __FILE__, __func__ );

/******************************************/
/*** allocate memory for parameter list ***/
/******************************************/
	if( verbose )
	{
		fprintf( stdout, "%s: %s: %s: allocating memory for data and event information\n", 
			progname, __FILE__, __func__ );
	}

	ev  = (EventInfo *)malloc(sizeof(EventInfo));
	ev  = (EventInfo *)glib2inv_get_input_parameters( evinfo_filename, ev, &nsta, verbose );

	if( verbose )
	{
		fprintf( stdout, "%s: %s: %s: reading data nsta=%d\n", 
			progname, __FILE__, __func__, nsta );
	}

	load_the_data( ev, nsta, ts0, verbose );

/******************************/
/*** load green's functions ***/
/******************************/

	if(verbose)
	  fprintf( stdout, "%s: %s: %s: allocating memory for Green's function\n",
                        progname, __FILE__, __func__ );

	zvec = (DepthVector *) malloc( nsta * sizeof(DepthVector) );

	grn = (Greens **)malloc( nsta*sizeof(Greens *) );

	/* z = (float *)load_greens( ev, grn, nsta, &nz, verbose ); */
	grn = loadGlibAll( grn, ev, zvec, nsta, "ginv", verbose );

	/*** for backwards compatibility ***/
	z = zvec[0].z;
	nz = zvec[0].nz;

/**************************************/
/*** check if fixing solution depth ***/
/*** iz_best get reset below        ***/
/**************************************/

	check_depth( FixMyZ, &FixMyiz, z, nz, verbose );
	
/*****************************************************/
/*** Error check if any any stations are turned on ***/
/*****************************************************/

        check_on_status_ok = 0;
        for( ista=0; ista<nsta; ista++ )
        {
                if( ev[ista].iused == 1 ) check_on_status_ok=1;
		
		/*** set these here before going into intense loop ***/
		ev[ista].Mo   = Mo;
		ev[ista].Mw   = log10( Mo )/1.5 - 10.73;;
		ev[ista].my_z = FixMyZ;
        }

        if(verbose)
	{
        	fprintf( stdout, "%s: check_on_status_ok=%d\n", 
			progname, check_on_status_ok );
	}

        if( !check_on_status_ok )
        {
                fprintf( stdout,
                  "%s: %s: %s: STDOUT: ERROR! no stations turned on in the par file %s\n",
                        progname, __FILE__, __func__, evinfo_filename );

                fprintf( stderr,
                  "%s: %s: %s: STDERR: ERROR! no stations turned on in the par file %s\n",
                        progname, __FILE__, __func__, evinfo_filename );
                exit(-1);
        }

/*********************************/
/*** set the type of inversion ***/
/*********************************/

	sol = (Solution *)malloc(nz*sizeof(Solution));
	for( iz=0; iz<nz; iz++ )
	{
        	sol[iz].mt_type  = FULL_MT;
	}

/***********************************************************************************/
/*** set inital random seed                                                      ***/
/***********************************************************************************/

	if(seed > 0)
		srandom(seed);
	else
		srand(abs(seed));

/***********************************************************************************/
/*** Load the Data and Greens functions                                          ***/
/*** create data vector "d" and Amatrix "A" in the forward calculaion: m * A = d ***/
/***********************************************************************************/

	cols = 6;
	rows = 1;
	iz = FixMyiz;  /*** this is the selected depth from check_depth() ****/
	sol[iz].evdp = grn[0][iz].evdp;

	for( ista = 0; ista < nsta; ista++ )
	{
		if( ev[ista].iused == 1 )
		{		
			rows += 3*grn[ista][iz].nt;
		}
	}

	fprintf( stderr,
	  "%s: %s: %s: STDERR: Allocating memory: a_matrix[rows=%d][cols=%d] and b_vector[rows=%d]\n",
                   progname, __FILE__, __func__, rows, cols, rows );

	a_matrix  = matrix( 0, rows+1, 0, cols+1 );
	b_vector  = vector( 0, rows+1 );

	create_Amatrix( grn, ev, nsta, FixMyiz, a_matrix, b_vector, rows, cols, mtdegfree, 
			Distance_Normalize, DistNormR0, verbose );

/***********************************************************************************/
/*** creat eigenvalues first and save lon,lat,k,T,e1,e2,e3 in results structure ****/
/***********************************************************************************/

	rbest = (Results *) calloc( nsim_eig, sizeof(Results) );

	for( j = 0; j < nsim_eig; j++ )
	{
		createlamb(
			&pdc, &pclvd, &piso, &e0, &e1, &e2, 
			&lune_lat, &lune_lon, &hudson_k, &hudson_T, 
			&gcarc_dc, &gcarc_exp, seed, &nevec, eigvec_fac, verbose );

		rbest[j].pdc      = pdc;
		rbest[j].pclvd    = pclvd;
		rbest[j].piso     = piso;
		rbest[j].e0       = e0;
		rbest[j].e1       = e1;
		rbest[j].e2       = e2;
		rbest[j].lune_lat = lune_lat;
                rbest[j].lune_lon = lune_lon;
		rbest[j].k        = hudson_k;
		rbest[j].T        = hudson_T;
		rbest[j].Mtotal   = Mo;
		rbest[j].FullMT.eig[1].val = e0;
		rbest[j].FullMT.eig[2].val = e1;
		rbest[j].FullMT.eig[3].val = e2;
		rbest[j].gcarc_dc  = gcarc_dc;
                rbest[j].gcarc_exp = gcarc_exp;
		rbest[j].nevec     = nevec;
	}

/*** add the deviatoric disk ***/

	fprintf( stderr, "%s: %s: %s: STDERR calling create_clvd_eigs() nsim_eig = %d\n",
		progname, __FILE__, __func__, nsim_eig );

/**** non-returning function create_clvd_eigs() was not reallocating memory ****/
	/* nclvd = (((Mclvd1 - (Mclvd0)) / dMo ) + 1); */
	/* rbest = realloc( rbest, ( nsim_eig + (((1 - (-1)) / dMo ) + 1) )*sizeof(Results) ); */

	rbest = create_clvd_eigs( rbest, &nsim_eig, eigvec_fac, Mo, dMo, verbose );

	fprintf( stderr, "%s: %s: %s: STDERR done with create_clvd_eigs() nsim_eig = %d increased\n",
		progname, __FILE__, __func__, nsim_eig );

/*** add the special lune verticies in source-type space ***/

	if( Add_DC_iso )
	{
		fprintf( stderr, "%s: %s: %s: STDERR Add_DC_iso=%d allocating memory for rbest(nsim_eig=%d Ntags=%d) and calling Add_DC_iso_2eigenvals()\n",
			progname, __FILE__, __func__, Add_DC_iso, nsim_eig, Ntags );

		rbest = realloc( rbest, (nsim_eig+Ntags)*sizeof(Results) );

		itag=0;
		for( j = nsim_eig; j < nsim_eig+Ntags; j++ )
		{
			Add_DC_iso_2eigenvals( itag, &(rbest[j]), eigvec_fac, Mo );
			itag++;
		}
		nsim_eig += Ntags;

		fprintf( stderr, "%s: %s: %s: STDERR Add_DC_iso=%d done with Add_DC_iso_2eigenvals()\n",
			progname, __FILE__, __func__, Add_DC_iso );
	}

/*** add user supplied special lat/lon lune point from inversion to test ***/

	if( Add_user_Eig )
	{
		fprintf( stderr, "%s: %s: %s: STDERR Add_user_Eig=%d allocating memory for rbest and calling eig2eig()\n",
                        progname, __FILE__, __func__, Add_user_Eig );

		rbest = realloc( rbest, (nsim_eig+2)*sizeof(Results) );
		
		eig2eig( &(e_user[0]), &(e_user[1]), &(e_user[2]), &pdc, &pclvd, &piso, 
                        &lune_lat, &lune_lon, &hudson_k, &hudson_T, 
			&gcarc_dc, &gcarc_exp, &nevec, eigvec_fac, verbose );

		j = nsim_eig;
		rbest[j].pdc      = pdc;
                rbest[j].pclvd    = pclvd;
                rbest[j].piso     = piso;
                rbest[j].e0       = e_user[0];
                rbest[j].e1       = e_user[1];
                rbest[j].e2       = e_user[2];
                rbest[j].lune_lat = lune_lat;
                rbest[j].lune_lon = lune_lon;
                rbest[j].k        = hudson_k;
                rbest[j].T        = hudson_T;
                rbest[j].Mtotal   = Mo;
                rbest[j].FullMT.eig[1].val = e_user[0];
                rbest[j].FullMT.eig[2].val = e_user[1];
                rbest[j].FullMT.eig[3].val = e_user[2];
		rbest[j].gcarc_dc  = gcarc_dc;
		rbest[j].gcarc_exp = gcarc_exp;
		rbest[j].nevec     = nevec;

		nsim_eig++;
	}

/**********************************************************************************/
/*** recount the number of eigenvectors ***/
/**********************************************************************************/

	for( sum = 0, j = 0; j < nsim_eig; j++ ) sum += rbest[j].nevec;

	fprintf( stderr, "%s: %s: %s: STDERR: Resized nsim_eig= %d total eigvecs=%d writting file eigvals.out\n", 
		progname, __FILE__, __func__, nsim_eig, sum );
	
	if(verbose) print_results_all( rbest, nsim_eig, "eigvals.out" );

/**********************************************************************************/
/*** loop over all eigenvalues stored in rbest[] and search random eigenvectors ***/
/**********************************************************************************/

	fprintf( stderr, "%s: %s: %s: STDERR: \n", progname, __FILE__, __func__ );

	if(!parallel)
	{
	  fprintf( stderr,
	    "%s: %s: %s: STDERR: SERIAL VERSION: parallel=%d (0-no/1-yes) calling simulate_MTeig2evec_serial(dot/100iter) ",
		progname, __FILE__, __func__, parallel );
		
		simulate_MTeig2evec_serial(
			nsim_eig,
			nsim_evec,
			rbest, 
			FixMyiz,
			sol,
			nsta,
			ev,
			a_matrix,
			b_vector,
			rows,
			cols,
			idump,
			max_vred,
			Mo,
			seed,
			Detail_Output,
			verbose );
	}
	else
	{
		fprintf( stderr,
            "%s: %s: %s: STDERR: PARALLEL VERSION: parallel=%d (0-no/1-yes) nthreads=%d calling simulate_MT_parallel(dot/100iter) ",
                progname, __FILE__, __func__, parallel, nthreads );

		simulate_MT_parallel( 
			nsim_eig,
			nsim_evec, 
			rbest,
			FixMyiz,
			sol,
			nsta,
			ev,
			a_matrix,
			b_vector,
			rows,
			cols,
			Mo,
			seed,
			Detail_Output,
			nthreads,
			verbose );
	}

/*** print out the best fitting in rbest ***/

	gmtplot( rbest, nsim_eig, color_or_gray,
		doplt, title_text, Add_user_Eig,
		Add_user_MT, Add_DC_iso, do_norm_gmt_plot, igmt5 );

/****************************/
/*** free allocated memory ***/
/****************************/

        free(rbest);
        free_matrix( a_matrix, 0, rows+1, 0, cols+1 );
        free_vector( b_vector, 0, rows+1 );

        fprintf( stderr, "%s: %s: %s: STDERR: END OF PROGRAM\n\n", progname, __FILE__, __func__ );

	exit(0);
  /****************************/
} /*** end of main() ***/
  /****************************/


/*****************************************************************************************/
/*** creates one or two gmt plots, 1-%VR on the lune, and 2-normalized VR on the lune ***/
/*** calls a lot of smaller subroutines                                               ***/
/*** SEE mteig_gmt.c                                                                  ***/
/*****************************************************************************************/

void gmtplot(
	Results *rbest,
	int nsim_eig,
	int color_or_gray,
	int doplt,
	char *title_text,
	int Add_user_Eig,
	int Add_user_MT,
	int Add_DC_iso,
	int do_norm_gmt_plot,
	int igmt5 )
{

/*** local variables ***/

	int ifullmt, idevmt;
	int idcmt, ipiso, iniso, iuser;

/*** functional prototypes ***/

	int search_largest_var_red( Results *rbest, int nsim_eig, char *sort, char *mt_type );

	void print_result_row_pretty_screen( Results *r, FILE *fp, char *label );

	
	void mteig_gmt5_plot( char *input_filename, char *gmt_script_filename, char *title_text, 
		Results *rbest, int ifullmt, int idevmt, int idcmt, int iuser, 
		int ipiso, int iniso, int Add_user_Eig, int Add_user_MT, int color_or_gray, int doplt );

	void mteig_gmt4_plot( char *input_filename, char *gmt_script_filename, char *title_text,
                Results *rbest, int ifullmt, int idevmt, int idcmt, int iuser,
                int ipiso, int iniso, int Add_user_Eig, int Add_user_MT, int color_or_gray, int doplt );

	void mteig_gmt_plot_lune_norm( char *input_filename, char *gmt_script_filename, char *title_text,
                Results *rbest, int ifullmt, int idevmt, int idcmt, int iuser,
                int ipiso, int iniso, int Add_user_Eig, int Add_user_MT, int color_or_gray, int doplt );

/*** begin function/subroutine ***/

	ifullmt = search_largest_var_red( rbest, nsim_eig, "sort", "FULLMT" );

	idevmt  = search_largest_var_red( rbest, nsim_eig, "nosort", "DEVMT" );

	fprintf( stderr, "%s: %s: %s: STDERR: \n", progname, __FILE__, __func__ );

	print_result_row_pretty_screen( &(rbest[ifullmt]), stdout, "bestfit - FULLMT" );
	
	fprintf( stderr, "%s: %s: %s: STDERR: \n", progname, __FILE__, __func__ );

	print_result_row_pretty_screen( &(rbest[idevmt]), stdout, "bestfit - DEVMT" );

	fprintf( stderr, "%s: %s: %s: STDERR: \n", progname, __FILE__, __func__ );
	
	print_result_row_pretty_screen( &(rbest[nsim_eig-1]), stdout, "user supplied" );

	fprintf( stderr, "%s: %s: %s: STDERR: \n", progname, __FILE__, __func__ );

/****************************/
/** create GMT plot ***/
/****************************/

	fprintf( stderr,
  "%s: %s: %s: STDERR: creating GMT C-shell script for plotting color_or_gray=%d doplt=%d title_text=%s see: best.all.out.lune.jpg\n",
		progname, __FILE__, __func__, color_or_gray, doplt, title_text );

	/****************************/
	/****see above ***/
	/****************************/
	/* ifullmt = search_largest_var_red() */
	/* idevmt = search_largest_var_red() */
	/****************************/

	if( Add_DC_iso )
	{
		idcmt  = nsim_eig-6;
		ipiso  = nsim_eig-5;
		iniso  = nsim_eig-4;
	}

	if( Add_user_Eig )
	{
		iuser  = nsim_eig-1;
	}

/****************************/
/** Network Sensitivity Solution on Eigvalue Lune Plot ***/
/** this version plots percent variance reduction on lune lat/lon ***/
/****************************/
	
	if(igmt5)
	{
	  mteig_gmt5_plot(
		"best.all.out",  /*** input_filename ***/
		"plot_lune2.csh",   /*** gmt_script_filename ***/
		title_text,
		rbest,
		ifullmt,
		idevmt,
		idcmt, 
		iuser,
		ipiso,
		iniso,
		Add_user_Eig,
		Add_user_MT,
		color_or_gray,
		doplt );
	}
	else
	{
	  mteig_gmt4_plot(
                "best.all.out",  /*** input_filename ***/
                "plot_lune2.csh",   /*** gmt_script_filename ***/
                title_text,
                rbest,
                ifullmt,
                idevmt,
                idcmt,
                iuser,
                ipiso,
                iniso,
                Add_user_Eig,
                Add_user_MT,
                color_or_gray,
                doplt ); 
	}

/** this version plots  normalized  variance reduction on lune lat/lon ***/

	if(do_norm_gmt_plot)
	{
	  mteig_gmt_plot_lune_norm(
		"best.all.out",  /*** input_filename ***/
		"plot_lune3.csh",   /*** gmt_script_filename ***/
		title_text,
		rbest,
		ifullmt,
		idevmt,
                idcmt,
                iuser,
                ipiso,
                iniso,
                Add_user_Eig,
                Add_user_MT,
                color_or_gray,
                doplt );
	}

} /*** end of subroutine gmtplot() ***/




/****************************/
/*** usage help print out ***/
/****************************/

void Usage_Print( void )
{
	fprintf( stderr, "\n" );
	fprintf( stderr, "USAGE:\n" );
	fprintf( stderr, "\t time %s par=mtinv.par nthreads=(integer) nsim_eig=2000 nsim_evec=4000 eigvec_fac=17000 \\ \n", progname );
	fprintf( stderr, "\t                    Mo=(float) fixz=14 color doplt seed=1 parallel [no]gmt5 title=\"put title here\" \\ \n" );
	fprintf( stderr, "\t                    Add_user_Eig e0=+1.69846 e1=+0.17199 e2=-1.29013\n" );

	fprintf( stderr, "\n" );
	fprintf( stderr, "Required: \n" );
	fprintf( stderr, "\t par       = MTINV parameter file. See mtinv.c \n" );
	fprintf( stderr, "\t nsim_eig  = #iter of createlamb() - creates random uniform distrib eigenvalues (best range 500-4000 use 2000)\n" );
	fprintf( stderr, "\t nsim_evec [DEPRECATED! use eigvec_fac] #iter of randomEigVec() - creates random uniform distrib of eigenvectors\n" );
	fprintf( stderr, "\t eigvec_fac = factor that scales the amount of eigenvectors for each random point on lune (best range 15000 to 20000 use 17000)\n" );
	fprintf( stderr, "\t seed      = seed > 0 random non-repeatable\n");
	fprintf( stderr, "\t             seed < 0 random repeatable (useful for testing requiring same numbers)\n" );
	fprintf( stderr, "\t Mo        = Seismic Scaler Moment (dyne cm) to scale unit eigenvalues\n" );
	fprintf( stderr, "\t fixz      = Source Depth to select from Green's function library, this code only handles one fixed depth at a time.\n" );
	fprintf( stderr, "\n" );

	fprintf( stderr, "Optional: \n" );
        fprintf( stderr, "\t [no]parallel [boolean] default off. Multithreading using posix pthreads \n" );
	fprintf( stderr, "\t nthreads=int [default 2] if parallel on then this is the number of pthreads used up to NUM_MAX_THREADS=16\n" );
	fprintf( stderr, "\t [no]color    [boolean] GMT lune plot %%VR uses rainbow color pallette or gray scale [default off: grayscale] \n" );
	fprintf( stderr, "\t [no]doplt    [boolean default yes] system call to execute GMT lune plot, otherwise just create the C-shell GMT script\n" );
	fprintf( stderr, "\t [no]gmt5     [boolean default yes] use GMT version 5+ for lune plot, otherwise uses GMT version 4.5.x (not avail for normalized lune)\n" );
	fprintf( stderr, "\t title=\"char *\"  text title in GMT Lune plot \n" );
	fprintf( stderr, "\t [no]domech    [boolean default yes] add psmeca in GMT C-shell script to plot best fitting %%VR MT focal mech\n" );
	fprintf( stderr, "\t [no]norm     [boolean] distance normalization default is off\n" );
	fprintf( stderr, "\t R0=float     normalize Green functions to distance of R/R0 [default R=1km]\n" );
	fprintf( stderr, "\t [no]dump     [boolean] Write out data and syn vecs as SAC files when %%VR > max_vred boolean [default off].\n" );
	fprintf( stderr, "\t max_vred=flt Maximum percent variance reduction to write data and syn as SAC files\n" ); 
	fprintf( stderr, "\t              range... -100 to 100%% see [no]dump option\n" );
	fprintf( stderr, "\t [no]Add_DC_iso    boolean default on - adds %%VR special points to Lune (DC,ISO,CLVD,crack,LVD)\n" );
	fprintf( stderr, "\t [no]Add_user_Eig  boolean default on - Adds a user supplied eigenvalues adds to Lune %%VR NSS plot\n" );
	fprintf( stderr, "\t        this option then requires Eigenvalues: e0={float} e1={float} e2={float} \n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "\t Future option Add_user_MT boolean default off, will require input Tensor Mxx=,,,, \n" );
	fprintf( stderr, "\n" );

	fprintf( stderr, "DESCRIPTION: \n" );
	fprintf( stderr, "  Creates MT Network Sensitivity Solution (NSS) based on Ford et al. (2010,2012)\n");
	fprintf( stderr, "  First use mtinv inversion toolkit to determine best fitting MT. \n" );
	fprintf( stderr, "  NSS is max variance reduction for points in source type space \n" );
	fprintf( stderr, "  Tape&Tape2012 (Lune latitude,longitude) or Hudson et al. 1989 (k,T)\n" );
	fprintf( stderr, "  eigenvalues and eigenvectros are sample randomly using spherical \n" );
	fprintf( stderr, "  uniform distribution. mteig uses the mtinv.par file as input and outputs a GMT script to create a plot\n" );
	
	fprintf( stderr, " use mtbestfit mteig ts0 to autogenerate mteig.csh file\n" );
	fprintf( stderr, "\n" );
}
