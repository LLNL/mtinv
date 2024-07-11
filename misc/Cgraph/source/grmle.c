/******************************************************************/
/*	GRMLE.C   Relative move()s and line()s                    */
/*                                                 		  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/

/*	Modified by WEG on 5/13/92				  */

#include "cglobals.h"

/****************************************************************************/

int cg_rmove(float xpos, float ypos)
{
	if (cgMARKAX)
		fprintf(cgstream,"%g cxs %g cys rmto\t",xpos,ypos);
	else
		fprintf(cgstream,"%g cvr %g cvr rmto\t",xpos*cgSCALE,ypos*cgSCALE);
	return(0);
}

/****************************************************************************/

int cg_rline(float xpos, float ypos)
{
	if (cgMARKAX)
		fprintf(cgstream,"%g cxs %g cys rlto\n",xpos,ypos);
	else
		fprintf(cgstream,"%g cvr %g cvr rlto\t",xpos*cgSCALE,ypos*cgSCALE);
	return(0);
}

