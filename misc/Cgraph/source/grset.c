/******************************************************************/
/*	GRSET.C   Resets the screen or laserwriter to basic       */
/*	          origin, colors, and linetypes		  	  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/

/*	Modified by WEG on 5/13/92 to work on NeXT's
								  */
								  
#include "cglobals.h"

/****************************************************************************/

int cg_reset(void)
{
    lw_aorigin(-cgxorig,-cgyorig); /* Go to the bottom left corner of the page */
    lw_linewidth(1.0);
    lw_gray(0.0);
    lw_dash(0,0.);
    return(0);
}

