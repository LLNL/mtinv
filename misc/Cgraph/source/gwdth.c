/******************************************************************/
/*	GWDTH.C   Set the linewidth for output on the             */
/*	               laserwriter                 		  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/

/*	Modified on 5/13/92 by WEG to work on NeXt's
								  */
								  
#include <stdio.h>
#include "cglobals.h"
#define FALSE 0

extern FILE *cgstream;
/****************************************************************************/

int cg_linewidth(float linewidth)
{
	fprintf(cgstream,"%8.5f slw\n",linewidth);
	return(0);
}


