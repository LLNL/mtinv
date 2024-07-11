/******************************************************************/
/*	GSTRK.C   Implements the stroke Postscript routine, and   */
/*	          resets the Imagraph line parameters		  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/

/*	Modified on 5/13/92 by WEG for use on NeXT's
								  */
								  
#include "cglobals.h" 

#define TRUE 1

extern FILE *cgstream;

/************************** Laserwriter version *****************************/

int cg_stroke(void)
{
	fprintf(cgstream," stroke\tnd\n");
	return(0);
}

