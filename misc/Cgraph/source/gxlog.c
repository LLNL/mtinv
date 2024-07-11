/*****************************************************************/
/*	GXLOG.C   Logarithmic x axis creation routines            */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/*  Updated 3/1/89 by Bruce McCready.  The updated program allows */
/*  the user to specify exactly what tics and numbers are to be   */
/*  printed by using a 11-bit bit map.				  */
/*  Bit 0 :   prints the minimum data value
 *  Bit 1 - 9:prints numbers ( or tics) for any value 1-9 where the
 *  corresponding bit is set and all powers of 10 for that integer
 *  <= the maximum data value.  
 *  Bit 10  :  prints the maximum data value
 *  The variables Numsel and Ticsel can be any value between 
 *  0 and (2^10) -1.  
 ******************************************************************/
 
 /*
	Modified on 5/13/92 by WEG to work on NeXT's
								  */
								  
#include "cglobals.h"
#include "cgaxes.h"
#include "cgfont.h"

#define EVEN 	2
#define UP 	1
#define DOWN 	0
											
/****************************************************************************/

int lw_xlog (float xsiz,float xmin,float xmax,float os) /* MAIN LASERWRITER LOG X-AXIS FUNCTION */
{
int    numdist;		/* point distance of numbers from end of tick */
int    tt,tb;		/* distance from axis to Top, Bottom of tickmark */
int    tt1, tb1;	/*  distance from axis to top, bottom for power of
			 * ten tick marks only */
double xrang,dubx;	/* temporary variables used for calculations */

	realxmax=xsiz;
	cgxma=wc_trunc(xmax*.99,UP);
	cgxmi=wc_trunc(xmin*1.01,DOWN);
	dubx= ceil( log10(cgxmi)-.0001 );
	cgldecx= pow(10.0,dubx);
	dubx= floor( log10(cgxma)+.0001 );
	cgudecx= pow(10.0,dubx);
	os=os*cgSCALE;
	cgispolar= FALSE;
	cglogx= TRUE;
	xsiz=xsiz*cgSCALE;
	xrang= log10(cgxma/cgxmi);
	cgxscale= xsiz/xrang;
	numdist= cglognumdist*(cgfsizex/10.0) + 1;

	fprintf(cgstream,"\n\n%% X axis (log).\n");

	fprintf(cgstream,"/xs {log %.5g cvr sub %.5g cvr mul} def\n",log10(cgxmi),cgxscale); 
	fprintf(cgstream,"/xsize {%.5g cvr} def\n",xsiz); 
	fprintf(cgstream,"/xrange {%.5g cvr} def\n",xrang);
	fprintf(cgstream,"/xos {%.5g cvr sub} def\n",os);

   	if(cgAxisEnable)
      	    fprintf(cgstream,"0 %.5g mto\n%.5g 0 rlto\nstroke\n",os,xsiz);

	if ((cglogtnposition == 3) || (cglogtnposition == 4))  {
		tb1 = 0;
		tb= 0;
	}
	else   {
		tb= -cglogticlen;
		tb1 = -cglogticlen10;
	}
	if ((cglogtnposition == 1) || (cglogtnposition == 6)) {
		tt1 = 0;
		tt= 0;
	}
	else   {
		 tt = cglogticlen;
		 tt1 = cglogticlen10;
	}
	leastY=os+tb-numdist-(1.4)*cgfontnum;
	lwxlinit(os,tt,tb,numdist,tt1,tb1);
        fprintf(cgstream,"%% END X axis (log).\n");
	return(0);
}

/****************************************************************************/
int lwxlinit(float os,int tt,int tb,int numdist,int tt1,int tb1) /* HANDLES TICKS AND NUMBERING OF X-AXIS */
{
int intmin,intdec;	/*Integer values of: x-minimum, 10s value of x-minimum*/
int mask;		/* mask holds a power of two that is ANDed with the*/
			/* variable cglogticsel. If true then the tics */
			/* for corresponding power of two are printed.  */
int i;			/* counting variable */
int result;
    cgxtix = cglogticsel;
    intmin=cgxmi;
    intdec=cgldecx/10;
    /* Check if bit pattern is in valid range */

    if(cgAxisEnable) {
	if ((cglogticsel<2048) && (cglogticsel>=0)) {
    
	    /* if low bit set then print ticline at minimum data value  */
		    mask = MINBIT;
		    if((result = (mask & cglogticsel)) != 0)
			lwxset(cgxmi,os+tt,os+tb);
    
	    /* if tenth bit set then print ticline maximum value   */
		    mask = MAXBIT;
		    if((result =(mask &cglogticsel)) != 0)
			lwxset(cgxma,os+tt,os+tb);
		    if (intmin != intdec)
			    lwxset(cgldecx,os+tt,os+tb);
	    /* Go through bit mask and for bits 1 - 9 check value.  If bit i = 1 */
	    /* then the corresponding ticline is printed on the graph.  i.e. if bit */
	    /* 2 is set then position 2 is marked on the graph, 2,20,200 ...<=max */
	    mask = 1;	
	    for(i = 1; i<10; i++)
	    {
		    mask = mask <<1;
		    if((result = (mask & cglogticsel)) != 0)
		    {
			    if(i == 1)
				{
				    lwxset(wc_xbottom(1),os+tt1,os+tb1);
				}
			    else 
				{   
			    	  lwxset(wc_xbottom(i),os+tt,os+tb);
				}
		   }
	    }
    
	}
	/* default is all tic marks set  */
	else lwallxtix(os+tb,os+tt);
    }

    if( ! cgAxisNumberEnable)
	return(0);
    /* repeat the process outlined for numbering the tics for printing the */
    /* numbers.   */
    if (cglognumsel <2048 && (cglognumsel >= 0)) {
	    mask = MINBIT;
	    if ((result =(mask & cglognumsel)) != 0)
		    lwlxmin(cgxmi,numdist,os,tt,tb);
	    mask = MAXBIT; 
	    if ((result = (mask & cglognumsel)) != 0) lwlxnum(cgxma,numdist,os,tt,tb);
	    mask = 1;
	    for(i = 1; i < 10; i++) {
		mask = (mask<<1);
		if(( result = (mask & cglognumsel)) != 0)
		lwlxnum(wc_xbottom(i),numdist,os,tt,tb);
	    }
    }
    /* Default value for numbering.  Min, Max, and 1,10,100,...<= max  */
    else {
	    lwlxmin(cgxmi,numdist,os,tt,tb);
	    lwlxnum(cgxma,numdist,os,tt,tb);
	    lwlxnum(wc_xbottom(1),numdist,os,tt,tb);
    }
    return(0);
}

/****************************************************************************/
int lwxset(float min,float top,float bot) 	/* LAY DOWN ALL OF THE TICKS FOR ONE SERIES */
				/* series of decades specified by min */
{
float x;
	for (x=min;x<=cgxma;x *= 10) {
		fprintf(cgstream,"%.5g cvr xs %.5g mto\n",x,bot);
		fprintf(cgstream,"0 %.5g rlto\n",top-bot);
		fprintf(cgstream,"stroke\n");
	}
	return(0);
}

/****************************************************************************/
int lwallxtix(float top,float bot) /* CREATE COMPLETE SET (10/DECADE) OF TICK MARKS */
{
float c;

	fprintf(cgstream,"/x %.5g cvr def\n",cgxmi);
	fprintf(cgstream,"x %.5g cvr %.5g cvr {xs dup %.5g mto\n",cgldecx/10,cgldecx,top);
	fprintf(cgstream,"		%.5g lto\n",bot);
	fprintf(cgstream,"		stroke} for\n");
	fprintf(cgstream,"/x %.5g cvr def\n",cgudecx);
	fprintf(cgstream,"x %.5g cvr %.5g cvr {xs dup %.5g mto\n",cgudecx,cgxma,top);
	fprintf(cgstream,"		%.5g lto\n",bot);
	fprintf(cgstream,"		stroke} for\n");
	if (cgudecx>cgldecx) {
		for (c=10*cgldecx;c<=cgudecx; c*= 10) {
			fprintf(cgstream,"/x %.7f def\n",c);
			fprintf(cgstream,"x 10 div dup x {xs dup %.5g mto\n",top);
			fprintf(cgstream,"		%.5g lto\n",bot);
			fprintf(cgstream,"		stroke} for\n");
		}
	}
	return(0);
}

/****************************************************************************/
int lwlxnum(float start,int numdist,float os,int tt,int tb) /* NUMBER ALL START*10^n WITHIN RANGE*/ 
{
float x;

	for (x=start;x<= cgxma;x=x*10) {
		x= wc_trunc(x,EVEN);
     	fprintf(cgstream,"(%.5g) stw pop 2 div\n",x);
		fprintf(cgstream,"%.5g exch sub ",log10(x/cgxmi)*cgxscale); 
		if ((cglogtnposition > 0) && (cglogtnposition < 4)) {
			fprintf(cgstream,"%.5g charheight sub\n",os+tb-numdist);
		}
	  	else fprintf(cgstream,"%.5g ",os+tt+numdist);
		fprintf(cgstream,"mto (%.5g) show\n",x);
	}
	return(0);
}


/****************************************************************************/
/*****   Special case for printing the Minimum value.  We just want to  *****/
/******  print the min value once, not all the powers of < max.   **********/
/******  This procedure is the lwlxnum with out the for loop.    *****/

int lwlxmin(float start,int numdist,float os,int tt,int tb) /* MARK MIN DATA VALUE ON GRAPH*/ 
{
float x;

	x=start;
	x= wc_trunc(x,EVEN);
     	fprintf(cgstream,"(%.5g) stw pop 2 div\n",x);
	fprintf(cgstream,"%.5g exch sub ",log10(x/cgxmi)*cgxscale); 
	if ((cglogtnposition > 0) && (cglogtnposition < 4)) {
	    fprintf(cgstream,"%.5g charheight sub\n",os+tb-numdist);
	}
	else
	    fprintf(cgstream,"%.5g ",os+tt+numdist);
	fprintf(cgstream,"mto (%.5g) show\n",x);
	return(0);
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
int cg_xlog(float size,float min,float max,float offset)  /* MAIN SPLITTER FUNCTION*/
{
	if (max <= 0.0) 
	{
		fprintf(stderr, "CG_XLOG: Max <= 0.0\n");
		return(0);
	}
	else if (min <= 0.0) 
	{
		fprintf(stderr, "CG_XLOG: Min <= 0.0\n");
		return(0);
	}
	if (max <= min)
	{
		fprintf(stderr, "CG_XLOG: Max <= Min\n");
		return(0);
	}
	if (size <= 0.0) 
	{
		fprintf(stderr, "CG_XLOG: Size <= 0.0\n");
		return(0);
	}
	lw_xlog(size,min,max,offset);
	return(0);
}

