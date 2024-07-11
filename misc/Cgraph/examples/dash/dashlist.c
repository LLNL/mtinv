/* 

Single arch compile with stripped binary:
cc -O -Wall -s -object -o dashlist dashlist.c -lcgraph


Define USE_STDOUT to send output to stdout (see Makefile)

*/


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <cgraph.h>

#define NUMDASHES 7

/* Override the BoundingBox numbers below to generate EPS files with
   appropriate cropping rectangles.  If your program generates a correct
   %%BoundingBox, it will save you time when importing EPS files into a
   drawing application.
*/



void main(int argc, char **argv)
{
int 	rot= 1;			/* Portrait=1, Landscape=0 */
int 	expand= 1;		/* left over from PC version, not used in NeXT Cgraph */
float	scale= 1.0;		/* scaling factor */
float x, y;
char strbuf[128];
int i;


#ifdef USE_STDOUT
	cg_use_stdout(1);		/* output to stdout */
#endif
	cg_setcreator(argv[0]);
	cg_settitle("Dash line types in Cgraph library");
	cg_setboundingbox("0 0 260 140");

	cg_init(rot, expand, scale);	/* expand is not used in cg_init() but for compatibility */

/* void cg_grayrgbcolor(float gray, float r, float g, float b); */
	cg_grayrgbcolor(0.0,  0.0, 0.0, 1.0);	/* black for gray, and blue for color */
	x = 0.2;
	for(i=0; i< NUMDASHES; i++) {
	    y = 0.2 * (float)i + 0.25;
	    cg_move(x, y);
	    cg_line(x + 3.0, y);
	    cg_dash(i, 0.4);
	    cg_stroke();
	    sprintf(strbuf, "%d", i);
	    cg_textalign( x + 3.1, y, 0.0, strbuf, 0, 1, 0);
	}

	cg_fontbyname(12.0, "Times-Roman");
	cg_text(x, y+0.2, 0.0, "dash period = 0.4 inches");

	cg_showpage();
}


