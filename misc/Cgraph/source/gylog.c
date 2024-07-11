/******************************************************************/
/*	GYLOG.C   Imagraph 1024x1024x8 bit C interface module     */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   7-27-87  starting					  */
/******************************************************************/
#include "cglobals.h"
#include "cgaxes.h"
#include "cgfont.h"

#define EVEN 	2
#define UP 	1
#define DOWN 	0

/****************************************************************************/

int lw_ylog(float ysiz,float ymin,float ymax,float os)	/* MAIN LASERWRITER LOG Y-AXIS FUNCTION */
{
int numdist;		/* Pixel distance of numbers from end of tick */
int tr,tl;		/* Distance from axis to Right, Left of tick mark */
int tr1, tl1;		/* Distance from axis to right,left of tick mark
			 * for power of ten tic marks. */
double yrang,duby;	/* Temporary variables used for calculations */

	realymax=ysiz;
	cgyma= wc_trunc(ymax*.99,UP);
	cgymi= wc_trunc(ymin*1.01,DOWN);
	duby= ceil( log10(cgymi)-.0001 );
	cgldecy= pow(10.0,duby);
	duby= floor( log10(cgyma)+.0001 );
	cgudecy= pow(10.0,duby);
	os=os*cgSCALE;
	cgispolar= FALSE;
	cglogy= TRUE;
	ysiz=ysiz*cgSCALE;	  
	yrang= log10(cgyma/cgymi);
	cgyscale= ysiz/yrang;
	numdist= cglognumdist*(cgfsizex/10.0)+1;

	fprintf(cgstream,"\n\n%% Y axis (log).\n");

	fprintf(cgstream,"/ys {log %.5g cvr sub %.5g cvr mul} def\n",log10(cgymi),cgyscale);
	fprintf(cgstream,"/ysize {%.5g cvr} def\n",ysiz); 
	fprintf(cgstream,"/yrange {%.5g cvr} def\n",yrang);
	fprintf(cgstream,"/yos {%.5g cvr sub} def\n",os);

   	if(cgAxisEnable)
	    fprintf(cgstream,"%.5g 0 mto\t0 %.5g rlto\tstroke\n",os,ysiz);

	if ((cglogtnposition == 3) || (cglogtnposition == 4)) {
		tl1 = 0;
		tl= 0;
	}
	else   {
		tl= -cglogticlen;
		tl1 = -cglogticlen10;
	}
	if ((cglogtnposition == 1) || (cglogtnposition == 6)) {
		tr= 0;
		tr1 = 0;
	}
	else   {
		tr = cglogticlen;
		tr1 = cglogticlen10;
	}
	lwylinit(os,tr,tl,numdist,tr1, tl1);
	leastX=os+tl-numdist;
        fprintf(cgstream,"%% END Y axis (log).\n");

	return(0);
}

/****************************************************************************/
int lwylinit(float os,int tt,int tb,int numdist,int tt1,int tb1) /* HANDLES TICKS AND NUMBERING OF Y-AXIS */
{
int intmin,intdec;	/*Integer values of: x-minimum, 10s value of x-minimum*/
int mask;		/* mask holds a power of two that is ANDed with the*/
			/* variable cglogticsel. If true then the tics */
			/* for corresponding power of two are printed.  */
int i;			/* counting variable */
int result;
    cgytix = cglogticsel;
    intmin=cgymi;
    intdec=cgldecy/10;
    /* Check if bit pattern is in valid range */

    if(cgAxisEnable) {
	if ((cglogticsel<2048) && (cglogticsel>=0)) {

	    /* if low bit set then print ticline at minimum data value  */
	    mask = MINBIT;
	    if((result = (mask & cglogticsel)) != 0)
		lwysetmin(cgymi,os+tt,os+tb);

	    /* if tenth bit set then print ticline maximum value   */
	    mask = MAXBIT;
	    if((result =(mask &cglogticsel)) != 0)
		lwyset(cgyma,os+tt,os+tb);
	    if (intmin != intdec)
		    lwyset(cgldecy,os+tt,os+tb);

	    /* Go through bit mask and for bits 1 - 9 check value.  If bit i = 1 */
	    /* then the corresponding ticline is printed on the graph.  i.e. if bit */
	    /* 2 is set then position 2 is marked on the graph, 2,20,200 ...<=max */
	    mask = 1;
	    for(i = 1; i<10; i++)	
	    {
		    mask = mask << 1;
		    if((result = (mask & cglogticsel)) != 0)
			{
			    if(i == 1)
				{
				  lwyset(wc_ybottom(1),os+tt1,os+tb1);		    
				}
			    else
				{
			    	  lwyset(wc_ybottom(i),os+tt,os+tb);
				}
			}
	    }

	}
	/* default is all tic marks set  */
	else lwallytix(os+tb,os+tt);
    }

    /* repeat the process outlined for numbering the tics for printing the */
    /* numbers.   */

    if( ! cgAxisNumberEnable)
	return(0);

    if (cglognumsel <2048 && (cglognumsel >= 0)) {
	    mask = MINBIT;
	    if ((result =(mask & cglognumsel)) != 0)
		    lwlymin(cgymi,numdist,os,tt,tb);
	    mask = MAXBIT;
	    if ((result = (mask & cglognumsel)) != 0)
		    lwlynum(cgyma,numdist,os,tt,tb);
	    mask =1;
	    for(i = 1; i < 10; i++) {
		mask = mask << 1;
		if(( result = (mask & cglognumsel)) != 0)
		lwlynum(wc_ybottom(i),numdist,os,tt,tb);
	    }
    }
    /* Default value for numbering.  Min, Max, and 1,10,100,...<= max  */

    else {
	    lwlymin(cgymi,numdist,os,tt,tb);
	    lwlynum(cgyma,numdist,os,tt,tb);
	    lwlynum(wc_ybottom(1),numdist,os,tt,tb);
    }
    return(0);
}

/****************************************************************************/
int lwyset(float min,float right,float left) 	/* LAY DOWN ALL OF THE TICKS FOR ONE  */
				/* SERIES OF DECADES SPECIFIED BY MIN */
{
float y;
	for (y=min;y<=cgyma;y *= 10) {
		fprintf(cgstream,"%.5g %.5g cvr ys mto\n",right,y);
		fprintf(cgstream,"%.5g 0 rlto\n",left-right);
		fprintf(cgstream,"stroke\n");
	}
	return(0);
}



/****************************************************************************/
/* Same as lwysetmin without for loop.  Just want to lay down minimun value */
/* not all powers <= max.  **/

int lwysetmin(float min,float right,float left) 	/* LAY DOWN ALL OF THE TICKS FOR ONE  */
				/* SERIES OF DECADES SPECIFIED BY MIN */
{
float y;
	y=min;
		fprintf(cgstream,"%.5g %.5g cvr ys mto\n",right,y);
		fprintf(cgstream,"%.5g 0 rlto\n",left-right);
		fprintf(cgstream,"stroke\n");
		return(0);
}



/**********************************************************************/
int lwallytix(float left,float right) /*CREATE COMPLETE SET (10/DECADE) OF TICK MARKS*/
{
float c;

	fprintf(cgstream,"/y %g cvr def\n",cgymi);
	fprintf(cgstream,"y %.5g cvr %.5g cvr {ys dup %.5g exch mto\n",cgldecy/10,cgldecy,right);
	fprintf(cgstream,"\t%.5g exch lto\n",left);
	fprintf(cgstream,"\tstroke} for\n");
	fprintf(cgstream,"/y %.5g cvr def\n",cgudecy);
	fprintf(cgstream,"y %.5g cvr %.5g cvr {ys dup %.5g exch mto\n",cgudecy,cgyma,right);
	fprintf(cgstream,"\t%.5g exch lto\n",left);
	fprintf(cgstream,"\tstroke} for\n");
	if (cgudecy>cgldecy) {
		for (c=10*cgldecy; c<=cgudecy; c*= 10) {
			fprintf(cgstream,"/y %.5g def\n",c);
         	   fprintf(cgstream,"y 10 div dup y {ys dup %.5g exch mto\n",right);
			fprintf(cgstream,"\t%.5g exch lto\n",left);
			fprintf(cgstream,"\tstroke} for\n");
		}
	}
	return(0);
}

/****************************************************************************/
int lwlynum(float start,int numdist,float os,int tr,int tl) /* NUMBER ALL start*10^n WITHIN RANGE*/
{
float y;
char testString[40];

	strcpy(xString, "");
	for (y=start;y<= cgyma;y=y*10) {
		y=wc_trunc(y,EVEN);
		if ((cglogtnposition > 0) && (cglogtnposition < 4)) {
     			fprintf(cgstream,"%g (%g) ",os+tl-numdist,y);
			fprintf(cgstream,"stw pop sub\n");
			}
		else  {	      
     			fprintf(cgstream,"%g ",os+tr+numdist);
			}
		fprintf(cgstream,"%g ",cgyscale*log10(y/cgymi));
		fprintf(cgstream,"charheight 2 div sub mto ");
		fprintf(cgstream,"(%g) show\n",y);
		sprintf(testString,"%g", y);
		if (strlen(testString) > strlen(xString))
			sprintf(xString,"%g", y);

	}
	return(0);
}


/****************************************************************************/
int lwlymin(float start,int numdist,float os,int tr,int tl) /* NUMBER JUST MINIMUM VALUE */
{
float y;

	y=start;
	y=wc_trunc(y,EVEN);
	if ((cglogtnposition > 0) && (cglogtnposition < 4)) {
		fprintf(cgstream,"%g (%g) ",os+tl-numdist,y);
		fprintf(cgstream,"stw pop sub\n");
		}
	else  {	      
		fprintf(cgstream,"%g ",os+tr+numdist);
		}
	fprintf(cgstream,"%g ",cgyscale*log10(y/cgymi));
	fprintf(cgstream,"charheight 2 div sub mto ");
	fprintf(cgstream,"(%g) show\n",y);
	return(0);
}




/****************************************************************************/
int cg_ylog(float size,float min,float max,float offset)	/* MAIN SPLITTER FUNCTION */
{
	if (max <= 0.0) 
	{
		fprintf(stderr, "CG_YLOG: Max <= 0.0\n");
		return(0);
	}
	if (min <= 0.0) 
	{
		fprintf(stderr, "CG_YLOG: Min <= 0.0\n");
		return(0);
	}
	if (max <= min) 
	{
		fprintf(stderr, "CG_YLOG: Max <= Min\n");
		return(0);
	}
	if (size <= 0.0) 
	{
		fprintf(stderr, "CG_YLOG: Size <= 0.0\n");
		return(0);
	}
	lw_ylog(size,min,max,offset);
	return(0);
}

