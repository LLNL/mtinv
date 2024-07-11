/******************************************************************/
/*	GFONT.C   Specifies the size and type of the font to      */
/*                be output                        		  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/

/*	Modified on 5/13/92 to work on NeXt's */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>								  
#include "cglobals.h"
#include "cgfont.h"
#include "PS_font_names.h"

#define SCRNSCALE  120
#define LOADFONTSIZE 21.
#define BUFSIZE 50

/*****************************************************************************
  Font type:  type = n*10 + m,  where

			n=
				1: Times font
				2: Courier font
				3: Helvetica font
			m= 
				0: Normal style
				1: Boldface style
				2: Italic style
				3: Boldface Italic style
*****************************************************************************/

int cg_font(float fsize,int type)
{
	int n,m;

	if (fsize < 0.)
		fsize = cgfsizex;
	else
		cgfsizex=fsize;
	if ((type >1) && (type<39))  	/* IF TYPE WITHIN RANGE */
	{
	    n=type/10;
	    m=type%10;
	    if (n ==1) {
		fprintf(cgstream,"/Times-");
		if (m ==1) fprintf(cgstream,"Bold");
		else if (m== 2) fprintf(cgstream,"Italic");
		else if (m== 3) fprintf(cgstream,"BoldItalic");
		else fprintf(cgstream,"Roman");
	    }
	    else {
		if (n ==2) fprintf(cgstream,"/Courier");
		else fprintf(cgstream,"/Helvetica");
		if (m ==1) fprintf(cgstream,"-Bold");
		else if (m== 2) fprintf(cgstream,"-Oblique");
		else if (m== 3) fprintf(cgstream,"-BoldOblique");
	    }
	    fprintf(cgstream," ff %g ssf\n", fsize);
	    cgfontnum=fsize;
	}
	return(0);
}

int cg_fontbyname(float fsize, char *fontname)
{
int i, fontOK= 0;
char *realfont;
	realfont = fontname;
	for(i=0; i<NUMPSFONTS; i++) {
	    if( strcmp(fontname, ps_fontnamelist[i]) == 0) {
	        fontOK = 1;
		break;
	    }
	}
	if(!fontOK) {
	    fprintf(stderr, "Bad font not in 35 PostScript fonts: %s\n", fontname);
	    fprintf(stderr, ">> substituting Times-Roman.\n");
	    realfont = "Times-Roman";
	}
	if (fsize < 0.)
		fsize = cgfsizex;
	else
		cgfsizex=fsize;

	fprintf(cgstream,"/%s ff %g ssf\n", realfont, fsize);
	cgfontnum=fsize;

	return(0);
}



