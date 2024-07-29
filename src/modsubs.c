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

extern char progname[128];

/*** various subroutines for creating, manipulating, and printing 1D Earth velocity models ***/

void clone_model( VelMod *vm0, VelMod *vm1 )
{
	int i;
	vm1->nlay = vm0->nlay;
	vm1->maxlay = vm0->maxlay;
	strcpy( vm1->modpath, vm0->modpath );
	strcpy( vm1->modfile, vm0->modfile );
	for( i=0; i<vm0->nlay; i++ )
	{
		vm1->thick[i]	= vm0->thick[i];
		vm1->ztop[i]	= vm0->ztop[i];
		vm1->vp[i]	= vm0->vp[i];
		vm1->qa[i]	= vm0->qa[i];
		vm1->vs[i]	= vm0->vs[i];
		vm1->qb[i]	= vm0->qb[i];
		vm1->rho[i]	= vm0->rho[i];
		vm1->sigma[i]	= vm0->sigma[i];
	}
}

void create_gmt_script( VelMod *vm )
{
	FILE *fp;
	char filename[256];
	sprintf( filename, "%s.gmt.csh", vm->modfile );
	fp = fopen( filename, "w" );
	fprintf( stderr, "%s: %s: %s: writting %s\n",
		progname, __FILE__, __func__, filename );
	fprintf( fp, "#!/bin/csh\n" );	
	fprintf( fp, "set PS=%s.ps\n", vm->modfile );
	fprintf( fp, "pltmod gmt modeldb=%s velmod=%s | \\\n", vm->modpath, vm->modfile );
        fprintf( fp, " gmt psxy -R0/10/-100/0 -JX5i/5i -W -P -Bxf0.1a1+l\"Velocity km/sec\" -Byf5a10+l\"Depth km\" -BNSEW+t\"1D Earth Model\" >! ${PS}\n" );
	fprintf( fp, "gmt psconvert  -A -Tj -E300 ${PS}\n" );
	fclose(fp);
}

void print_mod0( VelMod *vm )
{
        int i;
	
	printf("%s: \n", progname );
        printf("%s: number of layers=%d file=%s/%s.mod\n", progname, vm->nlay, vm->modpath, vm->modfile);
        printf("%s:\t\t Layer Thick   Ztop    Vp     Qa     Vs     Qb       Rho     Sigma\n", progname );
	printf("%s:\t\t       (km)   (km)  (km/s)         (km/s)           (g/cc)        \n", progname );

        for( i=0; i< vm->nlay; i++)
        {
                printf("%s:\t\t %03d %7.2f %6.2f %6.2f %8.2f %6.2f %8.2f %6.2f %6.2f\n",
			progname,
                        i,
                        vm->thick[i],
                        vm->ztop[i],
                        vm->vp[i],
                        vm->qa[i],
                        vm->vs[i],
                        vm->qb[i],
                        vm->rho[i],
                        vm->sigma[i]
                );
        }
	printf("%s: \n", progname );

 /***                                                                                                                                
        printf("#### ========================================================================\n");
        printf("nlay=%d\n",vm->nlay);
        for( i=0; i< vm->nlay; i++)
        {
                printf("lay%02d=%07.2f,%06.2f,%08.2f,%06.2f,%08.2f,%06.2f\n",
                        i+1, vm->thick[i], vm->vp[i], vm->qa[i], vm->vs[i], vm->qb[i], vm->rho[i] );
        }
        printf("#### ========================================================================\n");
 ****/
}
 
void write_csv( VelMod *vm )
{
	FILE *fp;
	char filename[128];
	int i;

	sprintf( filename, "%s.csv", vm->modfile );
	fp = fopen( filename, "w" );
	fprintf(fp, "thickness_km,depth_to_top_km,Vp_kps,Qp,Vs_kps,Qs,Density_gpcc,sigma\n" );
	for( i=0; i< vm->nlay; i++)
	{
		fprintf( fp, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
			vm->thick[i],
                        vm->ztop[i],
                        vm->vp[i],
                        vm->qa[i],
                        vm->vs[i],
                        vm->qb[i],
                        vm->rho[i],
                        vm->sigma[i] );
	}
	fclose(fp);
}

void write_mod1( FILE *fp, VelMod *vm )
{
	int i;
	fprintf(fp, "###\n" );
	fprintf(fp, "### path = %s\n", vm->modpath );
	fprintf(fp, "### file = %s\n", vm->modfile );
	fprintf(fp, "### number of layers = %d\n", vm->nlay );
	fprintf(fp, "###\n" );
	for( i = 0; i < vm->nlay; i++ )
	{
		fprintf( fp, "%7.2f %6.2f %8.2f %6.2f %8.2f %6.2f\n",
			vm->thick[i],
			vm->vp[i],
			vm->qa[i],
			vm->vs[i],
			vm->qb[i],
			vm->rho[i]
               	);

	}
}

void print_mod1( FILE *fp, VelMod *vm )
{
        int i;
	fprintf(fp, "###\n" );
        fprintf(fp, "### path = %s\n", vm->modpath );
        fprintf(fp, "### file = %s\n", vm->modfile );
        fprintf(fp, "### number of layers = %d\n", vm->nlay );
        fprintf(fp, "###\n" );
        for( i=0; i< vm->nlay; i++)
        {
                fprintf(fp, "%7.2f %6.2f %8.2f %6.2f %8.2f %6.2f\n",
                        vm->thick[i],
                        vm->vp[i],
                        vm->qa[i],
                        vm->vs[i],
                        vm->qb[i],
                        vm->rho[i]
                );
        }
        printf("\n\n");
}

void create_mod( VelMod *vm )
{
	char rec[128], filename[256];
        int i;
        FILE *fp;
        float u,d;
	int verbose = 0;

	vm->maxlay = MAX_MODEL_LAYERS;
	strcpy( filename, vm->modpath );
	strcat( filename, "/" );
	strcat( filename, vm->modfile );
	strcat( filename, ".mod" );

        if( (fp = fopen(filename,"r")) == NULL )
	{
		printf("%s: create_mod: FATAL ERROR cannot open file %s\n", 
			progname, filename );
		exit(-1);
	}
	if(verbose)
	{
		printf("%s: create_mod: reading velocity model from file %s\n", 
			progname, filename );
	}
	i=0;
	vm->ztop[0] = 0;
	while( fgets( rec, 128, fp ) != NULL )
	{
		if( rec[0] != '#' )
		{
			/* thickness vp  qa  vs  qb rho */
        		sscanf(rec, "%f %f %f %f %f %f",
				&(vm->thick[i]),
				&(vm->vp[i]),
				&(vm->qa[i]),
				&(vm->vs[i]), 
				&(vm->qb[i]),
				&(vm->rho[i])
			);
			vm->ztop[i+1]=vm->ztop[i]+vm->thick[i];

		/*** compute the Poisson ratio ****/
			u = vm->vp[i]*vm->vp[i] - 2.0 * (vm->vs[i]*vm->vs[i]) ;
			d = 2.0* ((vm->vp[i]*vm->vp[i]) - (vm->vs[i]*vm->vs[i])) ;
			vm->sigma[i] = u/d;
			i++;
		}
		if( i > vm->maxlay )
		{
			printf("%s: create_mod: increase maxlay=%d\n", 
				progname, vm->maxlay );
			exit(-1);
		}
	}
	vm->nlay = i;
	fclose(fp);
}

/*** reinitialize the S-vel & Den to a Poissonian Solid based on P-vel ***/

void reinit_mod( VelMod *vm )
{
	int i;
	float km2m = 1000;

	for( i = 0; i < vm->nlay; i++ )
	{

	  /*** assume poisson solid ***/
		vm->sigma[i]=0.25;

		vm->vs[i] = sqrt( ((vm->vp[i]*vm->vp[i])*(2.*vm->sigma[i]-1.)) /
                                 ( 2.*(vm->sigma[i]-1.) ) );

	  /*** Berteussen 1977 ***/
		vm->rho[i] = 0.32 * vm->vp[i] + 0.77;

          /*** Mancilla 2001 ***/
		vm->qb[i] = ( km2m * vm->vs[i] ) * 0.1;
		vm->qa[i] = 2.25 * vm->qb[i];
		vm->qa[i] = rintf( vm->qa[i] );
		vm->qb[i] = rintf( vm->qb[i] );

        }

}

void reinit_mod2( VelMod *vm, int fixbottom )
{
	int i;
        float km2m = 1000;
	if( (vm->nlay - fixbottom) <=0 ) return;
        for( i = 0; i < (vm->nlay - fixbottom); i++ )
        {
                /*** assume poisson solid ***/
                vm->sigma[i]=0.25;
                vm->vs[i] = sqrt( ((vm->vp[i]*vm->vp[i])*(2.*vm->sigma[i]-1.)) /
                                 ( 2.*(vm->sigma[i]-1.) ) );

                /*** Berteussen 1977 ***/
                vm->rho[i] = 0.32 * vm->vp[i] + 0.77;

                /*** Mancilla 2001 ***/
                vm->qb[i] = ( km2m * vm->vs[i] ) * 0.1;
                vm->qa[i] = 2.25 * vm->qb[i];
                vm->qa[i] = rintf( vm->qa[i] );
                vm->qb[i] = rintf( vm->qb[i] );
        }
}

void plot_gmt_mod( VelMod *vm, int ioutput, int doP, int doS, char *pline, char *sline )
{
        FILE *fp;
        float z[10000],x1[10000],x2[10000], qa[10000], qb[10000], rho[10000];
        int i, ilin;

/*** P-wave ***/
        for(ilin=0, i=0;i<=2*(vm->nlay);i++)
        {
                x1[ilin]=vm->vs[i];
                x1[ilin+1]=vm->vs[i];
                z[ilin]=vm->ztop[i];
                z[ilin+1]=vm->ztop[i+1];
                ilin+=2;
        }

/*** S-wave ***/
        for(ilin=0, i=0;i<=2*(vm->nlay);i++)
        {
                x2[ilin]	= vm->vp[i];
                x2[ilin+1]	= vm->vp[i];
                z[ilin]		= vm->ztop[i];
                z[ilin+1]	= vm->ztop[i+1];
                qa[ilin]	= vm->qa[i];
                qa[ilin+1]	= vm->qa[i];
		qb[ilin]	= vm->qb[i];
		qb[ilin+1]	= vm->qb[i];
                rho[ilin]	= vm->rho[i];
                rho[ilin+1]	= vm->rho[i];
                ilin+=2;
        }

/***  print model params for GMT psxy ***/
	if( ioutput == 2 )
	{
		if( (fp = fopen("mod_gmt.xy","w")) == NULL ) 
			fprintf(stderr, "warning cannot open file\n");
        	fprintf(fp, "> P-wave\n");
        	for(i=0;i<2*(vm->nlay);i++)
        	{
                	fprintf(fp, "%g %g %g %g\n", 
				x1[i], -z[i], qb[i], rho[i] );
        	}
        	fprintf(fp, "> S-wave \n");
        	for(i=0;i<2*(vm->nlay);i++)
        	{
                	fprintf(fp, "%g %g %g %g\n", 
				x2[i], -z[i], qa[i], rho[i] );
        	}
		fclose(fp);
	}
	else if( ioutput == 1 )
	{
/*** changed to GMT v5.x Nov21,2017 ***/
		if( doS )
		{
		 fprintf( stdout, "> %s S-wave\n", sline );
		 z[0] = 0;
		 for( i = 0; i < vm->nlay; i++ )
		 {
			z[i+1] = z[i] + vm->thick[i];
			fprintf( stdout, "%g %g %g %g\n",
				vm->vs[i], -z[i], vm->qb[i], vm->rho[i] );
			fprintf( stdout, "%g %g %g %g\n",
				vm->vs[i], -z[i+1], vm->qb[i], vm->rho[i] );
		 }
		}

		if( doP )
		{
		 fprintf( stdout, "> %s P-wave\n", pline );
		 z[0] = 0;
		 for( i = 0; i < vm->nlay; i++ )
		 {
			z[i+1] = z[i] + vm->thick[i];
			fprintf( stdout, "%g %g %g %g\n",
				vm->vp[i], -z[i], vm->qa[i], vm->rho[i] );
			fprintf( stdout, "%g %g %g %g\n",
                                vm->vp[i], -z[i+1], vm->qa[i], vm->rho[i] );
		 }
		}
	}
}

/***
	major bug fix March2014 G. Ichinose
	original:   if( g->evdp > zlay0 && g->evdp < zlay1 )
	changed:    if( g->evdp > zlay0 && g->evdp <= zlay1 )
	evdp could be equal exactly to zlay1 or zlay0 would be skipped
	set to initial value so if still rigidity = 0 
	compute_rigidity exits on error
***/

void compute_rigidity( Greens *g, int verbose )
{
        int i;
        float zlay1, zlay0;

/*** always set to some initial value ***/
	g->rigidity = 0;

        for( i=0; i< g->v.nlay; i++ )
        {
                zlay0 = g->v.ztop[i];
                zlay1 = g->v.ztop[i] + g->v.thick[i];

		if(verbose)
		{
		  fprintf( stdout, 
		    "%s: compute_rigidity(): zlay0=%g zlay1=%g Vs=%g rho=%g i=%d\n",
		    	progname, zlay0, zlay1, g->v.vs[i], g->v.rho[i], i );
		}

                if( g->evdp > zlay0 && g->evdp <= zlay1 )
                {
		  g->rigidity = (g->v.vs[i] * 1.0E+05)*(g->v.vs[i] * 1.0E+05)*g->v.rho[i] ;
                }
        }

	if( g->rigidity <= 0 ) 
	{
		fprintf( stdout, "%s: ERROR: compute_rigidity(): rigidity not set!\n", progname );
		exit(-1);
	}

        if( verbose ) 
	{
		fprintf( stdout, "%s: compute_rigidity(): rigidity=%g z=%g\n", 
			progname, g->rigidity, g->evdp );
	}
}
