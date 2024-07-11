/* new functions for Cgraph on the NeXT

	cg_xlabel
	cg_ylabel
	cg_centered
	cg_textalign
	
all written by WEG after request by A.Anzai and G.Ghose.

the idea is to get some nice text formatting functions into the Cgraph library 

	June 18, 1992
	
*/
		


#include "cglobals.h"
#include "cgfont.h"
#include "cgaxes.h"
#include "cgraph.h"
extern int markloaded[];


/*	prototypes will be stored in cgraph.h
	both cg_xlabel and cg_ylabel will be (essentially) routines that call
	cg_center.
	
	so we do cg_center first
	
*/

void cg_centerlabel (char *textString, float x, float y , float rotateBy, int coordSystem)
{
/*	brief description of function:
		cg_center takes a string and attempts to write it to the screen
		centered at (x,y) in  whichever coordsystem the user prefers.
		The user could choose coordSystem=0 (inches) or coordsystem=1
		(xmax, xmin,... system).
		
		then, we rotate the page, center the string, write the string, and
		unrotate the page.
		
		should give us the ability to write strings well.
		
*/

	fprintf(cgstream, "\n\n%% Centered text label\n");

	fprintf(cgstream, "gsave\n");
	if (coordSystem)
		fprintf(cgstream," %g %g sm\t ",x,y);
	else		
		fprintf(cgstream," %g %g mto\t ",x*cgSCALE,y*cgSCALE);
	fprintf(cgstream, " currentpoint translate ");
	fprintf(cgstream, "%g rotate ", rotateBy);
	fprintf(cgstream, "(%s) stringwidth ", textString);
	fprintf(cgstream, "2 div -1 mul exch 2 div \n -1 mul exch rmoveto ");
	fprintf(cgstream, "(%s) show ", textString);
	fprintf(cgstream, "grestore\n");
	fprintf(cgstream, "%% END Centered text label\n\n");

/*	I think that's it. We put the string on the paper and then returned to our
	normal coordinate system.
	Good deal, eh ?
	
*/

}

void cg_xlabel (char *textString)

{
	cg_centerlabel (textString, realxmax /2., (leastY-.4 * cgfontnum)/72.0
			 , 0., 0.);

}

void cg_ylabel (char *textString)
/*This isn't as simple as cg_xlabel because of the variable stringwidths of y-axis numbers. */
/*We can't just call cg_centerlabel :-( */
{
	fprintf(cgstream, "\n\n%% Centered Y label\n");

	fprintf(cgstream, "gsave\n");
	fprintf(cgstream," %g %g ", 0.5*realymax*cgSCALE,leastX);	
	fprintf(cgstream, " (%s) stringwidth ", xString);
	fprintf(cgstream, " pop sub ");
	fprintf(cgstream, " %g sub exch", 0.4*cgfontnum); 
	fprintf(cgstream, " moveto currentpoint translate ");
	fprintf(cgstream, " %g rotate ", 90.0);
	fprintf(cgstream, " (%s) stringwidth ", textString);
	fprintf(cgstream, " exch 2 div -1 mul exch -1 mul rmoveto ");
	fprintf(cgstream, "(%s) show ", textString);
	fprintf(cgstream, "grestore\n");
	fprintf(cgstream, "%% END Centered Y label\n\n");



}

void cg_textalign (float x, float y, float rotate, char *textString,
		   int xpos, int ypos, int coordSystem)

{
	fprintf(cgstream, "\n\n%% Aligned text label\n");
	fprintf(cgstream, "gsave\n");
	if (coordSystem)
		fprintf(cgstream," %g %g sm\t ",x,y);
	else		
		fprintf(cgstream," %g %g mto\t ",x*cgSCALE,y*cgSCALE);
	fprintf(cgstream, " currentpoint translate ");
	fprintf(cgstream, " %g rotate", rotate);
	fprintf(cgstream, " (%s) stringwidth pop 2 div %g mul " 
			,textString,(-1. *(float)xpos));
	fprintf(cgstream, " %g ",-1.0*(cgfontnum/2.7)*(float)ypos); 
	fprintf(cgstream, " rmoveto ");	
	fprintf(cgstream, " (%s) show ", textString);
	fprintf(cgstream, "grestore\n");
	fprintf(cgstream, "%% END Aligned text label\n\n");
	
}


