/* Program to plot scatter diagram of L and R Opt TF determined by Spect Amplitude */
/* Izumi Ohzawa 94-10-10 */
/*
cc -O -Wall -s -object -o TFopt-lrLog TFopt-lrLog.c -lcgraph

Define USE_STDOUT to send output to stdout (see Makefile)

*/


char *comment= "% TFopt-lrLog.c [94-10-13, Izumi Ohzawa]\n\
% Comparison of L-R optimal TF [Log axes] from XT spectrum peaks.\n";

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <streams/streams.h>
#include <cgraph.h>

#define PI	3.141592654
#define GREEN	130
#define RED	250
#define WHITE	255
#define GRAY	254


void main(int argc, char *argv[])
{
float vLmin = 0.5;
float vLmax = 10.0;
float vRmin = 0.5;
float vRmax = 10.0;
float	ratio, slopeH, slopeL, yH, yL;
int ninside;

int rot, expand;
int marktype;
int ntotal;
float	scale;
float markersize = 8.0;
/* char filename[128]; */
char *filename = "TFampspect.dat";
char linebuf[256];
FILE *fpin, *fopen();
float tfL, tfR;


	ratio = 1.5;
	slopeH = ratio;
	slopeL = 1.0/ratio;

	fpin = fopen(filename, "r");
	if( fpin == NULL )
	{
		printf("Can't open data file: %s\n", filename);
		exit(2);
	}
	/* OK, now plot the whole thing */	
	rot = 1;		/* portrait */
	expand = 1;		/* expand to fill screen */
	scale = 1.0;		/* scaling factor */


#ifdef USE_STDOUT
	cg_use_stdout(1);		/* output to stdout */
#endif
	cg_setcreator(argv[0]);
	cg_settitle("Log-log scatter plot from XY data file");
	cg_setboundingbox("0 0 270 270");
	cg_set_top_comments(comment); 	/* Comments that go directly after prolog. */
					/* These should describe the data source and plotting */
					/* parameters in detail. */
	cg_init(rot, expand, scale);	/* expand is not used in cg_init() but for compatibility */


	cg_aorigin(0.5, 0.5);
	cg_font(18.0, 30);

/* ## Do X and Y axes ## */
/* ---  Specify both gray and color so that later editing of 1 line in the EPS can flip
	   from B/W to color (for slides). */
/*	cg_grayrgbcolor(gray, red, grn, blu); */
	cg_grayrgbcolor(0.0,  1.0, 1.0, 0.4);	/* black for B/W, and light yellow for color */

/* ---	cg_linax_style(precision, numoff, numdist, tnposition, ticlen);
	cg_linax_style(3, 0, 5, 3, 8);
*/
/* ---	int cg_logax_style(int numsel,int ticsel,int numdist,int tnposition,int ticlen, int ticlen10, int numformat); */
/*	cg_logax_style(0x27, 0x7ff, 4,  3, 5, 8, 0);	*//* 1 2 5 numbers */
	cg_logax_style(0xb, 0x7ff, 4,  3, 4, 8, 0);	/* 1 3 numbers */

/* --- 	cg_yaxis( ysize, ymin, ymax, yoffset, ticsep, numsep); 
	cg_yaxis( 3.0, 0.0, 0.8001, 0.0, 0.2, 2);
	cg_xaxis( 3.0, 0.0, 0.8001, 0.0, 0.2, 2);
*/
/* ---	cg_xlog(xsize, min, max, offset);   */
	cg_xlog( 3.0, 0.5, 10.0, 0.0);
	cg_ylog( 3.0, 0.5, 10.0, 0.0);


	cg_mesh();
	cg_font(22.0, 30);

	/* Draw exact correspondence, and 0.0 horizontal line */
	cg_grayrgbcolor(0.0,  0.7, 0.7, 0.7);	/* black for B/W, and gray for color */
	cg_linewidth(0.6);
	cg_move(0.5, 0.5);
	cg_line(10.0, 10.0);
	cg_stroke();

	/* ratio lines in dash */
	cg_dash(6, 0.1364);
	cg_move(vLmin*ratio, vRmin);
	cg_line(vLmax, vRmax/ratio);
	cg_move(vLmin, vRmin*ratio);
	cg_line(vLmax/ratio, vRmax);
	cg_stroke();
	cg_dash(0, 1.0);

	ninside = 0;
	ntotal = 0;	/* reset counter */
	cg_linewidth(1.0);

/* void cg_markergraycolor (float peri_gray, float peri_r, float peri_g, float peri_b,
		     float inside_gray, float inside_r, float inside_g, float inside_b); */
	/* Marker color is specified by a different function. */
	cg_markergraycolor(0.0, 1.0, 1.0, 1.0,
			   1.0, 1.0, 0.0, 0.0);	/* white in black, or red in white */
	marktype = 0;	/* open circle */

	while( fgets(linebuf, 128, fpin) != NULL) {
	    if(linebuf[0] == '#' || linebuf[0] == '%' || linebuf[0] == ';'
		|| strlen(linebuf) == 0) continue;

	    sscanf(linebuf,"%f %f", &tfL, &tfR);
	    ntotal++;
	    cg_marker(tfL, tfR, marktype, markersize);
	    yH = slopeH*tfL;
	    yL = slopeL*tfL;
	    if( tfR <= yH && tfR >= yL)
		ninside++;
	} /* end of while( fgets() ) loop */

	fclose(fpin);	/* close file */
	printf("%d of %d points are within a factor of %6.2f\n",
		ninside, ntotal, ratio);


	/* showpage() is always necessary, or nothing will come out. */
	cg_showpage();
}

