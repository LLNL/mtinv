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

#include "../include/nrutil.h"
#include "../include/mt.h"

char progname[128];

/*** glib2inv processes Greens function library files *.glib for moment tensor inversion read by mtinv.c ***/
/*** see support by glib2inv_parallel.c glib2inv_serial.c glib2inv_subs.c ***/

int main( int ac, char **av )
{

/************************/
/*** depth vector     ***/
/************************/

	int iz;
	DepthVector *z;  /*** see ../include/mt.h ***/

/************************/
/*** event info stuff ***/
/************************/

	EventInfo *ev;
	EventInfo *glib2inv_get_input_parameters( char *, EventInfo *, int *, int );
	int ista, nsta;
	char evinfo_filename[256];

/********************/
/*** Greens stuff ***/
/********************/
	
	Greens **grn;
	FILE *fpin;

/*******************/
/*** local stuff ***/
/*******************/

	int iparallel = 0;
	int debug = 0;
	int verbose = 0;
	int idumpgrn = 0;
	int test_special = 0;
	int DIFFoperator = 3;
	char pathname[128];

/***************************/
/*** function prototypes ***/
/***************************/

/*** Greens_subs.c: Greens **loadGlibAll() ***/

	Greens **loadGlibAll( Greens **grn, EventInfo *ev, DepthVector *z, int nsta, char *file_type, int verbose );

/*** glib2inv_serial.c ***/

	void glib2inv_serial( Greens **grn, EventInfo *ev, DepthVector *z, int nsta, int idumpgrn, int verbose );

/*** glib2inv_parallel.c ***/

	void glib2inv_parallel( Greens **grn, EventInfo *ev, DepthVector *z, int nsta, int idumpgrn, int verbose );

/*** shorten_path.c ***/

	char *shorten_path( char *pathname, char *filename );

/*** load_special_grns.c ***/

	float *load_special_grns( EventInfo *ev, Greens **grn, int nsta, int *nz_tmp, int verbose );

/*** glib2inv_serial.c ***/

	void glib2inv_special( Greens **grn, EventInfo *ev, DepthVector *z, int nsta, int idumpgrn, int verbose );

/*** misc ../lib/libget.a: ../misc/getpar/ ***/
	int setpar(int ac, char **av);
	int mstpar( char *, char *, void * );
        int getpar( char *, char *, void * );
	void endpar(void);

/*** usage-> this file? ***/
	void Usage_Print();

/***************************************************************************************************/
/**** begin program                                                                              ***/
/***************************************************************************************************/

/*****************************************************/
/*** get the input parameters foreach each station ***/
/*****************************************************/

	strcpy( pathname, av[0] );
	shorten_path( pathname, progname );

	fprintf( stderr, "%s: STDERR: Version=%s ReleaseDate=%s exec full path=%s\n",
		progname, Version_Label, Version_Date, pathname );

/*****************************************************/
/*** usage                                         ***/
/*****************************************************/

	if( ac <= 1 ) Usage_Print();

/******************************************************************************/
/*** command line arguments                                                 ***/
/******************************************************************************/

	setpar( ac, av );
	mstpar( "par",      "s", &evinfo_filename );
	getpar( "verbose",  "b", &verbose );
	getpar( "dumpgrn",  "b", &idumpgrn );
	getpar( "parallel", "b", &iparallel );
	getpar( "test_special", "b", &test_special );
	endpar();

	if( test_special ) iparallel = 0;

	if(verbose)
	{
	  fprintf( stdout, "%s: STDOUT: Version=%s ReleaseDate=%s exec full path=%s\n",
                progname, Version_Label, Version_Date, pathname );
	}

/***********************************/
/*** load in par file parameters ***/
/***********************************/

	ev  = (EventInfo *)malloc(sizeof(EventInfo));
	ev  = (EventInfo *)
		glib2inv_get_input_parameters( evinfo_filename, ev, &nsta, verbose );

	if( verbose )
	{
		fprintf( stdout, "%s: %s: %s: STDOUT: nsta=%d\n", 
			progname, __FILE__, __func__, nsta );
	
		for( ista = 0; ista < nsta; ista++ )
		{
		  fprintf( stdout, "%s: %s: %s: ista=%03d data=%s glib=%s ginv=%s npole=%d npass=%d lf=%g ",
			progname, __FILE__, __func__, ista, ev[ista].data_filename, ev[ista].glib_filename,
			ev[ista].ginv_filename, ev[ista].npole, ev[ista].npass, ev[ista].lf );

		  fprintf( stdout, "hf=%g nt=%d dt=%g tr=%g tt=%g velordisp=%d mulfac=%g iused=%d\n",
			ev[ista].hf, ev[ista].nt, ev[ista].dt, ev[ista].tr, ev[ista].tt,
			ev[ista].grd_mo_type, ev[ista].mul_factor, ev[ista].iused );
		}

		for( ista = 0; ista < nsta; ista++ )
		{
			fprintf( stdout, "%s: \t", progname );
			WriteMyTime2STDOUT( &(ev[ista].ot) );
		}
	}

/******************************************************************************/
/*** loop over stations and just read in the Green functions                ***/
/******************************************************************************/

	/*** special formated greens functions, using Mij format ***/
	if(test_special)
	{
		float *z_tmp;
		int nz = 1;
		grn = (Greens **) malloc( nsta * sizeof(Greens *) );

		z_tmp = (float *) load_special_grns( ev, grn, nsta, &nz, verbose );

		z = (DepthVector *) malloc( nsta * sizeof(DepthVector) );

		for( ista = 0; ista < nsta; ista++ )
		{
			z[ista].nz = 1;
			z[ista].ista = ista;
			z[ista].z = (float *) calloc(z[ista].nz, sizeof(float));
			z[ista].z = z_tmp;
		}	

		/**************************************/
		/*** process the GFs                ***/
		/**************************************/
		glib2inv_special( grn, ev, z, nsta, idumpgrn, verbose );

	} /*** end test_special option true ***/

	else   /*** this is the normal glib file from mkgrnlib.c ***/

	{
		grn = (Greens **) malloc( nsta * sizeof(Greens *) );

		z = (DepthVector *) malloc( nsta * sizeof(DepthVector) );

		grn = loadGlibAll( grn, ev, z, nsta, "glib", verbose );

	/**************************************************/
	/*** do the processing                          ***/
	/**************************************************/

	  if( iparallel )
	  {
		glib2inv_parallel( grn, ev, z, nsta, idumpgrn, verbose );
	  }
	  else
	  {
		glib2inv_serial( grn, ev, z, nsta, idumpgrn, verbose );
	  }

	  fprintf( stdout, "%s: %s: %s: STDOUT: freeing memory\n", progname, __FILE__, __func__ );
	  fprintf( stderr, "%s: %s: %s: STDERR: freeing memory\n", progname, __FILE__, __func__ );

	  free(z);

	}

	free(ev);
	free(grn);

	fprintf( stderr, "%s: %s: %s: STDERR: Finished Program. Bye-Bye! \n\n\n", progname, __FILE__, __func__ );
	fprintf( stdout, "%s: %s: %s: STDOUT: Finished Program. Bye-Bye! \n\n\n", progname, __FILE__, __func__ );

	exit(0);
}

void Usage_Print()
{
        fprintf(stderr,
          "\n USAGE: %s par=mtinv.par [no]verbose [no]dumpgrn [no]parallel [no]test_special\n",
          progname );

        fprintf(stderr, "\n" );
        fprintf(stderr, "\t REQUIRED PARAMETERS: \n" );
        fprintf(stderr, "\t par=mtinv.par    station parameter file\n" );
        fprintf(stderr, "\n" );

        fprintf(stderr, "\t OPTIONAL PARAMETERS: \n" );
        fprintf(stderr, "\t [no]verbose           be verbosy [boolean DEFAULT off]\n" );
        fprintf(stderr, "\t [no]dumpgrn           write out the Green functions as SAC formatted files [boolean DEFAULT off]\n" );

        fprintf(stderr, "\t [no]parallel          process Greens functions using pThreads/multithreading [boolean DEFAULT on] \n" );

        fprintf(stderr, "\t [no]test_special      read in Greens functions from SAC files in Mij(r,t,z) format [boolean DEFAULT off]\n" );
        fprintf(stderr, "\t                       test_special option requires serial option (noparallel) automatically unset\n" );
        fprintf(stderr, "\n" );

	fprintf(stderr, "\t DESCRIPTION:\n");
	fprintf(stderr, "\t Reads station, filters, models, from PAR file mtinv.par and processes Green's function libraries.\n" );         
	fprintf(stderr, "\t Outputs processed Green's function libraries for input into mtinv. For processing data see sacdata2inv.\n" );
	fprintf(stderr, "\n" );
}
