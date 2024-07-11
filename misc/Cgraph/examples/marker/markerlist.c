/* markerlist.c -- draws a list of all available symbols and pointers for use
     with marking data points.

Single arch compile with stripped binary:
cc -O -Wall -s -object -o markerlist markerlist.c -lcgraph


Define USE_STDOUT to send output to stdout (see Makefile)

*/


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <cgraph.h>

#define NUMMARKERS	20

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
float markersize = 10.0;

#ifdef USE_STDOUT
	cg_use_stdout(1);		/* output to stdout */
#endif

	cg_setcreator(argv[0]);
	cg_settitle("Maker and Pointers for Cgraph library");
	cg_setboundingbox("0 0 460 100");
	cg_init(rot, expand, scale);	/* expand is not used in cg_init() but for compatibility */

/* void cg_grayrgbcolor(float gray, float r, float g, float b); */
	cg_grayrgbcolor(0.0,  0.0, 0.0, 1.0);	/* black for gray, and blue for color */

	/* draw dashed lines to define reference points at which markers are positioned. */
	y = 0.4;
	cg_move(0.1, y);
	cg_line(6.3, y);

	for(i=0; i< (NUMMARKERS + 4); i++) {
	    if(i < NUMMARKERS)
	        x = 0.25 * (float)i + 0.25;
	    else
	        x = 0.25 * (float)i + 0.35;
	    cg_move(x, y+0.35);
	    cg_line(x, y-0.35);
	}
	cg_dash(5, 0.04);
	cg_linewidth(0.12);
	cg_stroke();

	/* Plot markers and pointers at intersections defined by dashed lines */
	for(i=0; i< (NUMMARKERS + 4); i++) {
	    if(i < NUMMARKERS) {
	        x = 0.25 * (float)i + 0.25;
	        cg_marker(x, y, i, markersize);
	        sprintf(strbuf, "%d", i);
	    }
	    else {
	        x = 0.25 * (float)i + 0.35;
	        cg_pointer(x, y, i-NUMMARKERS, markersize);
	        sprintf(strbuf, "%d", i-NUMMARKERS);
	    }
	    cg_textalign( x, y+0.42, 0.0, strbuf, 1, 0, 0);
	}

	cg_text(0.25, 1.1, 0.0, "markersize = 10 pt");
	cg_text(2.5, 1.1, 0.0, "cg_marker()");
	cg_text(5.3, 1.1, 0.0, "cg_pointer()");

	cg_showpage();
}


