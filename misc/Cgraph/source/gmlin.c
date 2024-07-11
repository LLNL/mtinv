/******************************************************************/
/*	GMLIN.C   Implements the move() and line() commands       */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/

/*	Modified on 5/13/92 by WEG */

#include "cgraph.h"
#include "cglobals.h"

/****************************************************************************/


int cg_move(float xpos, float ypos)
{
	if (cgMARKAX)
		fprintf(cgstream,"%g %g sm\t",xpos,ypos);
	else	/* Inches */
		fprintf(cgstream,"%g %g mto\t",xpos*cgSCALE,ypos*cgSCALE);
	return(0);
}

/****************************************************************************/

int cg_line(float xpos, float ypos)
{
	if (cgMARKAX)
		fprintf(cgstream,"%g %g sl\n",xpos,ypos);
	else	/* Inches */
		fprintf(cgstream,"%g %g lto\t",xpos*cgSCALE,ypos*cgSCALE);
	return(0);
}

int cg_rectfill(float xpos, float ypos, float width, float height)
{
	cg_move( xpos, ypos);
	cg_line( xpos+width, ypos);
	cg_line( xpos+width, ypos+height);
	cg_line( xpos, ypos+height);
	cg_closepath();
	cg_fill();
	return(0);
}

