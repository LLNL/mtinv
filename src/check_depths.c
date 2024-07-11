#include <stdio.h>
#include <stdlib.h>
#include "../include/mt.h"
extern char progname[128];

/********************************************************************************/
/*** used by mtinv.c, mteig.c, forward_calc_from_MT.c - checks user selected  ***/
/***    MT depth is in the Greens function library *.glib or *.ginv files     ***/
/********************************************************************************/

void check_depth( float FixMyZ, int *FixMyiz, float *z, int nz, int verbose )
{
	int is_my_z_valid( float, float *, int );

	if(verbose)
	{
		fprintf( stderr, "%s: %s: %s: searching for FixMyZ=%g nz=%d\n",
			progname, __FILE__, __func__, FixMyZ, nz );
	}

	if( ( *FixMyiz = is_my_z_valid( FixMyZ, z, nz )) == -999 )
	{
		fprintf( stderr,
			"%s: %s: %s: force_z=%g is ***NOT*** a valid depth in ginv files\n",
			progname, __FILE__, __func__, FixMyZ );
		exit(-1);
	}
	else
	{
		if(verbose)
		{
			fprintf( stderr, "%s: %s: %s: fixz=%g iz=%d is a valid depth in ginv files\n",
				progname, __FILE__, __func__, FixMyZ, *FixMyiz );
		}
	}
}

/*** macro only used above ***/

int is_my_z_valid( float myz, float *z, int nz )
{
        int iz, myiz;
        for( iz=0; iz<nz; iz++ )
        {
                if( myz == z[iz] )
                {
                        myiz = iz;
                        return myiz;
                }
        }
        return -999;
}
