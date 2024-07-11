/******************************************************************/
/*	GDASH.C   Create one of seven dashes (including a solid   */
/*                line)                            		  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/
/* Modified for BGI Interface 3/9/89, GG 

	Additional modifications for NeXT system : WEG on 5/13/92 
	IM stuff (pc screen outputs) removed.
	Some housecleaning.
	
								  */
#include "cglobals.h"

/****************************************************************************/
/*   	[array] offset setdash -
		sets the dash pattern parameter.  If array is empty
		(its length = 0) then stroke produces a normal, unbroken
		line. If array is not empty, stroke produces dashed lines
		according to the pattern given in the array. The contents
		of the array are used cyclically.  The first element gives 
		length of the dash, the next the length of the space between,
		and so on.  Therefore, dash type 2 below would create a dash
		(scaled by the appropriate multiplication factor) with 6 on,
		2 off, 1 on, 2 off, 1 on, 2 off, 6 on, two off, etc. Offset
		is the initial offset of the line and is unimportant.  The
		pattern RAM settings in the screen function above create
		approximately the same type dashes, but it's easier to 
		understand what they look like from the PostScript code     */

int cg_dash(int type, float mul) 
{
	fprintf(cgstream,"\n\n%% Setting dash pattern\n");
	if ((mul < 0.01) || (mul > 20.0)) {
	    fprintf(stderr, "Dash period %g is out of range (0.01 .. 20). Using 0.2\n", mul);
	    mul= 0.2;
	}
	switch(type) {
	    /* Keep sum of all dash components to be 72 points */
	    case 0: fprintf(cgstream,"nd\n");	       /* no dash procedure */
		    break;
	    case 1: fprintf(cgstream,"[%.5g %.5g] 0 setdash\n",62.0*mul,10.0*mul);
		    break; 
	    case 2: fprintf(cgstream,"[%.5g %.5g %.5g %.5g",
					30.0*mul, 10.0*mul, 6.0*mul, 10.0*mul);
		    fprintf(cgstream," %.5g %.5g] 0 setdash\n", 6.0*mul, 10.0*mul);
		    break; 
	    case 3: fprintf(cgstream,"[%.5g %.5g %.5g %.5g] 0 setdash\n",
			    36.0*mul, 12.0*mul, 12.0*mul, 12.0*mul);
		    break; 
	    case 4: fprintf(cgstream,"[%.5g %.5g] 0 setdash\n", 54.0*mul, 18.0*mul);
		    break; 
	    case 5: fprintf(cgstream,"[%.5g %.5g] 0 setdash\n", 18.0*mul, 54.0*mul);
		    break; 
	    case 6: fprintf(cgstream,"[%.5g %.5g] 0 setdash\n", 36.0*mul, 36.0*mul);
		    break; 
	    default: break;
    	}
	fprintf(cgstream,"%% END Setting dash pattern\n\n");
	return(0);
}

