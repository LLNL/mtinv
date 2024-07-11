/******************************************************************/
/*	GXAXS.C   Routines to generate the x-axis             	  */
/*	                                           		  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/

/*	Modifed on 5/13/92 by WEG		                  */
								  
#include "cglobals.h"
#include "cgaxes.h"
#include "cgfont.h"


/*****************************************************************************/
int cg_xaxis (float xsiz, float xmin, float xmax, float os, float xtix, int numsep)
{
    float x,xrang;
    int numdist, tt=0, tb=0, sigdec;
    char line[32];

    /* SET TO DEFAULT IF VALUE IS BEYOND RANGE */
    if ((numsep<1) || (numsep > 100)) numsep = 5;

    /* SET EVERYTHING UP */	
    os=os*cgSCALE;
    cglogx= FALSE;
    realxmax=xsiz;
    cgxma= xmax +.001*(xmax-xmin);
    cgxmi= xmin;
    cgxticks=xtix;
    xsiz=xsiz*cgSCALE;
    xrang= cgxma-cgxmi;
    cgxscale= xsiz/xrang;
    numdist= (cglinnumdist)*(cgfsizex/10.0)+1;

    fprintf(cgstream,"\n\n%% X axis (linear).\n");
    fprintf(cgstream,"/xs {%g cvr mul} def\n",cgxscale); 
    fprintf(cgstream,"/xtix {%g cvr} def\n",xtix); 
    fprintf(cgstream,"/xsize {%g cvr} def\n",xsiz); 
    fprintf(cgstream,"/xrange {%g cvr} def\n",xrang);
    fprintf(cgstream,"/xos {%g cvr sub} def\n\n",cgxmi);

    /* CREATE THE AXIS AND TICKS*/	
    if(cgAxisEnable) {
	if ((cglintnposition == 3) || (cglintnposition == 4))
	    tb= 0;
	else tb= -cglinticlen;
	if ((cglintnposition == 1) || (cglintnposition == 6))
	    tt= 0;
	else tt = cglinticlen;
	fprintf(cgstream,"/x 0 def\n");
		fprintf(cgstream,"0 %g mto\n%g 0 rlto\nstroke\n",os,xsiz);
	fprintf(cgstream,"x 0 xtix xrange {xs dup %g mto\n",os+tt);
	fprintf(cgstream,"\t%g lto\n",os+tb);
	fprintf(cgstream,"\tstroke xtix x add} for\n");
    }

    if(cgAxisNumberEnable) {
	sigdec= 0;
	/* Determine The Maximum Significant Decimal Place*/
	sigdec=findsigdec(cgxmi,xrang*1.01,numsep,cgxticks);
    
	/*NOW FOR THE REAL NUMBER PLACEMENT LOOP*/
	for (x=cglinnumoff*cgxticks; x<= xrang*1.01; x+= numsep*xtix)
	{
	    formaxnum(x+cgxmi,sigdec,line);
	    fprintf(cgstream,"\nnewpath 1000 1000 mto\n");
	    fprintf(cgstream,"(%s) stw pop 2 div\n",line);
	    fprintf(cgstream,"%g cvr exch sub ", x*cgxscale); 
	    if ((cglintnposition > 0) && (cglintnposition < 4)) 	/* Numbers BELOW Axis */
		fprintf(cgstream,"%g charheight sub ",os+tb-numdist);
    
	    else 					/* Numbers ABOVE Axis */
		fprintf(cgstream,"%g ",os+tt+numdist);
	    
	    fprintf(cgstream,"mto (%s) show\n", fix_string(line,1));
	}
    }

    leastY=os+tb-numdist-(1.4)*cgfontnum;
    fprintf(cgstream,"\n/xs {%g cvr sub %g cvr mul} def\n", cgxmi, cgxscale); 
    fprintf(cgstream,"%% END X axis (linear).\n");
    return(0);
}

