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

#include "../include/mt.h"

char progname[128];

int main( int ac, char **av )
{
	VelMod vm;
	int init = 0, gmt = 0;
	int doP=1, doS=1;
	char pline[64], sline[64];

	void create_mod( VelMod *vm );
	void reinit_mod( VelMod *vm );
	void print_mod1( FILE *fp, VelMod *vm );
	void plot_gmt_mod( VelMod *vm, int ioutput, int doP, int doS, char *pline, char *sline );
	void print_mod0( VelMod *vm );
	void create_gmt_script( VelMod *vm );
	void write_csv( VelMod *vm );
	int csv_output = 0;

	int setpar( int, char ** );
	int mstpar( char *, char *, void * );
        int getpar( char *, char *, void * );
	void endpar();

/*** start main ***/

	strcpy( progname, av[0] );

	if( ac == 1 )
	{
	  fprintf( stderr, "USAGE:\n" );
	  fprintf( stderr, "\t %s modeldb=(string) velmod=(string) [no]init [no]gmt \n", progname );
	  fprintf( stderr, "\t          pline=\"-W1.2p,blue,5_2:0p\" sline=\"-W1.2p,red,5_2:0p\" \n" );
	  fprintf( stderr, "\n" );
	  fprintf( stderr, "REQUIRED PARAMETERS:\n" );
	  fprintf( stderr, "\t modeldb=(string) path directory to *.mod files\n" );
          fprintf( stderr, "\t velmod=(string) basename of *.mod file\n" );
	  fprintf( stderr, "\n" );
	  fprintf( stderr, "OPTIONAL PARAMETERS: \n" ); 
          fprintf( stderr, "\t [no]init reinitialize the S-vel & Den to a Poissonian Solid based on P-vel (default off)\n" );
	  fprintf( stderr, "\t [no]gmt sends output as GMT psxy output (default off, screen output)\n" );
	  fprintf( stderr, "\t pline=(string) P-wave GMT psxy -W specs \n" );
	  fprintf( stderr, "\t sline=(string) S-wave GMT psxy -W specs \n" );
	  fprintf( stderr, "\t csv=(boolean switch) output to csv file (default off)\n" );
	  fprintf( stderr, "\n" );
	  fprintf( stderr, "DESCRIPTION:\n" );
	  fprintf( stderr, "\t reads mod file and creates input for Generic Mapping Tools psxy\n" );
	  fprintf( stderr, "\n" );
	  exit(-1);
	}

	setpar(ac,av);
	mstpar( "modeldb", "s", &(vm.modpath) );
	mstpar( "velmod", "s", &(vm.modfile) );
	getpar( "csv", "b", &csv_output );
	getpar( "init", "b", &init );

	getpar( "gmt", "b", &gmt );
	if( gmt )
	{
		strcpy( pline, "-W1.2p,blue,5_2:0p" );
		strcpy( sline, "-W1.2p,red,5_2:0p" );
		getpar( "pline", "s", pline );
		getpar( "sline", "s", sline );
		getpar( "doP", "b", &doP );
		getpar( "doS", "b", &doS );
	}
	endpar();

	create_mod( &vm );

	if( init )
	{
		FILE *fp;
		reinit_mod( &vm );
		// print_mod0( &vm );
		fp = fopen("new.mod", "w");
		print_mod1( fp, &vm );
		fclose(fp);
	}

	if( gmt )
	{
		plot_gmt_mod( &vm, 1, doP, doS, pline, sline );
	}

	if( init == 0 && gmt == 0 ) print_mod0( &vm );

	if( csv_output ) write_csv( &vm );

	if( !gmt ) create_gmt_script( &vm );

	exit(-1);
}
