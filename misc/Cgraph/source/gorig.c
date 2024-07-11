/******************************************************************/
/*	GORIG.C   Set up the origin  				  */
/*                                                 		  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/

/*	Modified on 5/13/92 by WEG
								  */
								  
#include "cglobals.h"


/****************************************************************************/

int cg_rorigin(float xpos, float ypos)
{
	cgyorig += ypos*cgSCALE;
	cgxorig += xpos*cgSCALE;
	fprintf(cgstream,"\n\n%% Origin shift relative.\n");
	fprintf(cgstream,"/xs {%g mul} def /ys {%g mul} def\n", cgSCALE, cgSCALE);
	fprintf(cgstream, "%g %g mto  ", xpos*cgSCALE, ypos*cgSCALE);
	fprintf(cgstream,"cpt\n");
	fprintf(cgstream,"%% END Origin shift relative.\n");
	return(0);
}



/****************************************************************************/

int cg_aorigin(float xpos, float ypos)
{
	fprintf(cgstream,"\n\n%% Origin shift absolute.\n");
	fprintf(cgstream,"/xs {%g mul} def /ys {%g mul} def\n", cgSCALE, cgSCALE);

	/* Gotta do it via RELATIVE origin shifts */
	fprintf(cgstream,"%g %g mto  ", xpos*cgSCALE - cgxorig, ypos*cgSCALE - cgyorig);
	fprintf(cgstream,"cpt\n");

	cgyorig = ypos*cgSCALE;		/* new origin in absolute terms */
	cgxorig = xpos*cgSCALE;
	fprintf(cgstream,"%% END Origin shift absolute.\n");
	return(0);
}



/****************************************************************************/
