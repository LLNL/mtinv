/* ggraycolor.c

87-9-08  starting
	Wayne Crawford, Univ. of California, Berkeley
96-08-28
	Izumi Ohzawa,
*/

#include "cglobals.h"
#include "cgraph.h"
extern int markloaded[];


int cg_gray(float degree) 	

{
	if ((degree<0.) || (degree>1.))
		degree = 0.;
	fprintf(cgstream,"\n%g setgray\n",degree);
	return(0);
}

void cg_rgbcolor(float r, float g, float b)
{
	fprintf(cgstream, "\n%g %g %g setrgbcolor\n", r, g, b);
}

/* Specifies gray and color that will be selected depending on device.
   It uses gray on B/W devices, and uses r,g,b for device that supports color
   without using the built-in photometric rgb to gray conversion.
*/
void cg_grayrgbcolor(float gray, float r, float g, float b)
{
	fprintf(cgstream, "\n%g %g %g %g setgrayrgbcolor\n", gray, r, g, b);
}


/* ##################################################################### */
/* Color of Markers */
void cg_markergray (float newPerimeter, float newInside)
{
	int i;
	stroke_gray= newPerimeter;
	fill_gray= newInside;
	sprintf(stroke_graycolor, " %g setgray ", stroke_gray);
	sprintf(fill_graycolor, " %g setgray ", fill_gray);
	for (i=0;i<15;i++) markloaded[i]=0;
}

/* specify marker color attribute */
void cg_markercolor (float peri_r, float peri_g, float peri_b,
		     float inside_r, float inside_g, float inside_b)
{
	int i;
	/* no checking for bounds */
	stroke_r = peri_r; stroke_g = peri_g; stroke_b = peri_b;
	fill_r = inside_r; fill_g = inside_g; fill_b = inside_b;
	sprintf(stroke_graycolor, " %g %g %g setrgbcolor ", stroke_r, stroke_g, stroke_b);
	sprintf(fill_graycolor, " %g %g %g setrgbcolor ", fill_r, fill_g, fill_b);
	for (i=0;i<15;i++) markloaded[i]=0;
}

/* specify marker gray and color attribute */
/* Specifies gray and color that will be selected depending on device.
   It uses gray on B/W devices, and uses r,g,b for device that supports color
   without using the built-in photometric rgb to gray conversion.
*/
void cg_markergraycolor (float peri_gray, float peri_r, float peri_g, float peri_b,
		     float inside_gray, float inside_r, float inside_g, float inside_b)
{
	int i;
	/* no checking for bounds */
	stroke_gray = peri_gray; stroke_r = peri_r; stroke_g = peri_g; stroke_b = peri_b;
	fill_gray = inside_gray; fill_r = inside_r; fill_g = inside_g; fill_b = inside_b;
	sprintf(stroke_graycolor, " %g %g %g %g setgrayrgbcolor ",
			stroke_gray, stroke_r, stroke_g, stroke_b);
	sprintf(fill_graycolor, " %g %g %g %g setgrayrgbcolor ",
			fill_gray, fill_r, fill_g, fill_b);
	for (i=0;i<15;i++) markloaded[i]=0;
}

/* Flex color is enabled if flag >= 2
   Uses color if            flag == 1
   Uses B/W if              flag == 0
*/
void cg_useflexcolor(int flag)
{
     flex_graycolor = flag;
}

/* Enable opqeue background. Call before cg_init(). */
void cg_opaqueBG(float gray, float r, float g, float b)
{
	bg_gray = gray;
	bg_r = r; bg_g = g; bg_b = b;
	opaque_BG = 1;
}

