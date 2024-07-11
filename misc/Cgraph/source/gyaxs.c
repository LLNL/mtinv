/******************************************************************/
/*	GYAXS.C   Routines to generate the y-axis                 */
/*                                                 		  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/

/*	Modified on 5/13/92 by WEG to work on NeXT's
								  */
								  
								  
#include "cglobals.h"
#include "cgfont.h"
#include "cgaxes.h"
#include <string.h>


/*****************************************************************************/
int cg_yaxis (float ysiz, float ymin, float ymax, float os, float ytix, int numsep)
{
    float y,yrang;
    int numdist,tr=0,tl=0,sigdec;
    char line[32];

    /* SET TO DEFAULTS IF VALUES ARE BEYOND RANGE */
    if ((numsep<1) || (numsep > 100)) numsep = 5;

    /* SET EVERYTHING UP */	
    realymax=ysiz;
    cgyma= ymax + .001*(ymax-ymin);
    cgymi= ymin;
    os=os*cgSCALE;
    cglogy= FALSE;
    cgispolar= FALSE;
    ysiz=ysiz*cgSCALE;      
    yrang= ymax-ymin;
    cgyscale= ysiz/yrang;
    numdist= cglinnumdist+1;

    fprintf(cgstream,"\n\n%% Y axis (linear).\n");
    fprintf(cgstream,"/ys {%g cvr mul} def\n",cgyscale);
    fprintf(cgstream,"/ytix {%g cvr} def\n",ytix);
    fprintf(cgstream,"/ysize {%g cvr} def\n",ysiz); 
    fprintf(cgstream,"/yrange {%g cvr} def\n",yrang * 1.01);
    fprintf(cgstream,"/yos {%g cvr sub} def\n",ymin);


    /* CREATE THE AXIS AND TICKS*/	
    if(cgAxisEnable) {
	fprintf(cgstream,"%g 0 mto\n0 %g rlto\nstroke\n",os,ysiz);
	if ((cglintnposition == 3) || (cglintnposition == 4))
	    tl= 0;
	else tl= -cglinticlen;
	if ((cglintnposition == 1) || (cglintnposition == 6))
	    tr= 0;
	else tr = cglinticlen;
	fprintf(cgstream,"/y 0 def\n");
	fprintf(cgstream,"y 0 ytix yrange {ys dup %g exch mto\n",os+tr);
	fprintf(cgstream,"\t%g exch lto\n",os+tl);
	fprintf(cgstream,"\tstroke ytix y add} for\n");
    }

    /* Do the numbers on the axis */
    if(cgAxisNumberEnable) {
	sigdec= 0;
	/* This FOR Loop Merely Determines The Maximum Significant Decimal Place*/
	sigdec=findsigdec(cgymi,yrang*1.01,numsep,ytix);
	/*NOW FOR THE REAL NUMBER PLACEMENT LOOP*/
	for (y= cglinnumoff*ytix;y<= yrang*1.01;y += numsep*ytix) 
	{
	    formaxnum(y+cgymi,sigdec,line);
	    fprintf(cgstream,"newpath 1000 1000 mto\n");
	    if ((cglintnposition > 0) && (cglintnposition < 4))	/* Numbers LEFT OF AXIS */
		    {
		    fprintf(cgstream,"%g (%s) stw pop sub\n",os+tl-numdist,line);
		    leastX=os+tl-numdist;
		    }
	    else					/* Numbers RIGHT OF Axis */
		    {
		    fprintf(cgstream,"%g ",os+tr+numdist);
		    leastX=os+tr+numdist;
		    }
	    
	    fprintf(cgstream,"%g charheight ",cgyscale*y);
	    fprintf(cgstream,"2 div sub mto ");
	    fprintf(cgstream,"(%s) show\n", fix_string(line,1));
	    if (strlen(fix_string(line,1)) > strlen(xString))
		    sprintf(xString, "%s", fix_string(line,1));
	}
    }

    fprintf(cgstream,"\n/ys {%g cvr sub %g cvr mul} def\n",ymin,cgyscale);
    fprintf(cgstream,"%% END Y axis (linear).\n");
    return(0);
}

/*

int cg_yaxis(float size,float min,float max,float offset,float ticsep,int numsep)
{
    lw_yaxis(size,min,max,offset,ticsep,numsep);
    return(0);
}

*/
