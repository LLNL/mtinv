#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern char progname[128];

/*******************************************************************/
/*******************************************************************
* ASSUMPTIONS:
* - "Normal" polarity is such that the second component leads
*   the first component by 90 degrees in a clockwise rotation.
*   if input is not in normal polarity then
* con12=-con12
* con22=-con22
*  if out is not in normal polarity then
*  con21=-con21
*  con22=-con22
*****************************************************************/
/*******************************************************************/

void rotate( float *ns, int ns_nt, float *ns_cmpaz, float ns_cmpinc, 
	     float *ew, int ew_nt, float *ew_cmpaz, float ew_cmpinc, 
	     float angle, int verbose )
{
        float deg2rad, cosa, sina, con11, con12, con21, con22;
        int i;
        float nstmp, ewtmp;
	int normal = 1;
	float angle_mod(float);

        if( ew_nt != ns_nt )
        {
		fprintf( stdout, "%s: %s: %s: error nt not same ns=%d ew=%d\n", 
			progname, __FILE__, __func__, ns_nt, ew_nt);
                exit(-1);
        }

	angle = angle_mod(angle);

	if(verbose)
	{
          fprintf( stdout, "%s: %s: %s: back angle=%g\n",
		progname, __FILE__, __func__, angle );
        }

        if( *ew_cmpaz == -12345 || *ns_cmpaz == -12345 || 
	  ( ew_cmpinc != 90 && ns_cmpinc != 90 ) )
        {
		printf("%s: rotate(): wrong or undef components given\n", 
			progname );
		printf("\t az=(ew=%g ns=%g) inc=(ew=%g ns=%g)\n",
			*ew_cmpaz, *ns_cmpaz, ew_cmpinc, ns_cmpinc);
		exit(-1);
        }
	if(verbose)
	{
		fprintf( stdout, "%s: %s: %s: rotate by angle=%g\n", 
			progname, __FILE__, __func__, angle );
	}
        deg2rad = M_PI/180;
        cosa = cos( deg2rad * angle );
        sina = sin( deg2rad * angle );
        con11 = cosa;
        con12 = sina;
        con21 = -sina;
        con22 = cosa;

	if( normal == 0 )
	{
		con12 = -con12;
		con22 = -con22;
	}

        for( i=0;i<ns_nt; i++ )
        {
                nstmp = con11 * ns[i] + con12 * ew[i];
                ewtmp = con21 * ns[i] + con22 * ew[i];
                ns[i] = nstmp;
                ew[i] = ewtmp;
        }
	*ns_cmpaz = angle;
	*ew_cmpaz = angle+90;
	if( *ew_cmpaz > 360 ) *ew_cmpaz = *ew_cmpaz - 360;
}

float angle_mod( float angle )
{
	if( angle >= 0 && angle < 360 ) 
	{
		return angle;
	}
	else
	{
		while( angle > 360 ) angle -= 360;
		while( angle < 0 ) angle += 360;
		return angle;
	}
}


