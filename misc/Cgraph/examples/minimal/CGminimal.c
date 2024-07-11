/* CGminimal.c

To get the output to stdout, invoke the program with an argument '-', i.e.,
    CGminimal -  > CGminimal.eps

(For other examples, see Makefile)

This file shows by example how to retain information on the data source and
analysis parameters in your EPS plot files as comments.


It also shows new color functions for specifying gray and color at
the same time for later use for slides.  You can flip B/W and Color
by editing just one line in the EPS file generated.  See functions:

 For Non-Marker objects:
    void cg_grayrgbcolor(float gray, float r, float g, float b);

 For Markers and Pointers:
    void cg_markergraycolor (float peri_gray, float peri_r, float peri_g, float peri_b,
		     float inside_gray, float inside_r, float inside_g, float inside_b);


Single arch compile with stripped binary:
cc -O -Wall -s -object -o CGminimal CGminimal.c -lcgraph

FAT compile:
cc -O -Wall -arch i386 -arch m68k -s -object -o CGminimal CGminimal.c -lcgraph

For use with GDB debugger:
cc -Wall -g -o CGminimal CGminimal.c -lcgraph

*/


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <cgraph.h>

#define PI	3.141592654

/* Override the BoundingBox numbers below to generate EPS files with
   appropriate cropping rectangles.  If your program generates a correct
   %%BoundingBox, it will save you time when importing EPS files into a
   drawing application.
*/

static char *prog_version = "CGminimal.c [V1.01, 1996-08-28, Izumi Ohzawa]";



void plot_something(char *progname, int use_stdout)
{
int 	rot= 1;			/* Portrait=1, Landscape=0 */
int 	expand= 1;		/* left over from PC version, not used in NeXT Cgraph */
float	scale= 1.0;		/* scaling factor */
float   radius1, radius2;	/* analysis or plot parameters */
char comment[4096];	/* reserve sufficient space for header comments for EPS file */
char strbuff[128];
int i;
float x, y, angle;

	if(use_stdout)
	   cg_use_stdout(use_stdout);	/* decide where to send output */
	cg_useflexcolor(2);		/* let the device capability dictate color/monochrome */

	cg_setboundingbox("0 0 216 216");
	cg_settitle(prog_version);
	cg_setcreator(progname);

	/* Prepare comment block for EPS for data source tracing */
	radius1= 0.8; radius2= 1.2;
	/* PostScript comment lines must start with a % character. */
	sprintf(comment, "%% %s\n", prog_version);
	sprintf(comment+strlen(comment), "%% Original data file: %s\n", progname);
	sprintf(comment+strlen(comment), "%% Analysis parameter-1: %g\n", radius1);
	sprintf(comment+strlen(comment), "%% Analysis parameter-1: %g\n", radius2);
	 /* ... more stuff to record as needed .. */
	cg_set_top_comments(comment); 	/* set the comments for EPS header */

	cg_init(rot, expand, scale);	/* expand is not used in cg_init() but for compatibility */
	cg_aorigin(1.5, 1.5);		/* move origin to center of BoundingBox */


	/* interesting shape */
	sprintf(strbuff, "\n\n%% Radius %g\n", radius1);
	cg_postscript(strbuff);		/* enter comment into file for possible later editing */

	for(i=0; i<=200; i++) {
	    angle = PI*(float)i/100.0;
	    x = radius2 * cos(3.0*angle);
	    y = radius2 * sin(5.0*angle);
	    if(i) cg_line(x, y);
	    else  cg_move(x, y);		/* first point */
	}
	cg_closepath();

/*
	cg_postscript("\n\n%% Demo of falling back on injection of raw PostScript code.\n");
	cg_postscript("\n gsave 0.6 1.0 0.6 0.6 setgrayrgbcolor eofill grestore\n");
*/
	/* 4 lines below will do the same thing as the above raw PS code. */
	cg_gsave();
	cg_grayrgbcolor(0.6, 1.0, 0.6, 0.6);
	cg_eofill();
	cg_grestore();

/* void cg_grayrgbcolor(float gray, float r, float g, float b); */
	cg_grayrgbcolor(0.0,  0.0, 0.0, 1.0);	/* black for gray, and blue for color */
	cg_stroke();	/* strokes the two circles */
	cg_postscript("\n%% End of my plot.\n");

	cg_showpage();
}


void main(int argc, char **argv)
{
int use_stdout=0;
    if(argc >= 2  && strncmp(argv[1], "-", 1) == 0)	/* if arg is "-", output to stdout */
	use_stdout = 1;

    plot_something(argv[0], use_stdout);
}

