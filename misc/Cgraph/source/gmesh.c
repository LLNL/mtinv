/******************************************************************/
/*	GMESH.C   Create a mesh for the graph 		   	  */
/*	Wayne Crawford, Univ. of California, Berkeley	       	  */
/*	   9-09-87  starting					  */					  
/*      Modified 3/14 Bruce McCready	  			  */
/*      Setting of the mesh color on the imagraph screen is now   */
/*      done in cg_mesh not in cg_color.      			  */
/******************************************************************/

/*	Modified on 5/13/92 by WEG to work on NeXT's		  */


#include "cglobals.h"
#include "cgaxes.h"


/****************************************************************************/

int cg_mesh(void)
{
	/* Save the linewidth and set the dash */
	fprintf(cgstream,"\n\n%% Mesh for plotting domain.\n");
	fprintf(cgstream,"savlw\t0.2 slw [1 2] 0 setdash\n");
	if (cglogx== TRUE) 
		lwlxmesh();
	else lwxmesh();
	if (cglogy== TRUE) 
		lwlymesh();
	else lwymesh();
	fprintf(cgstream,"clw slw nd\n");
	fprintf(cgstream,"%% END Mesh for plotting domain.\n");
	return(0);
}

/****************************************************************************/
/****************************** LINEAR MESHES *******************************/
/****************************************************************************/

int lwxmesh(void)
{
	fprintf(cgstream,"/x {%g cvr} def\n",cgxmi);
	fprintf(cgstream,"x xtix %g cvr {xs dup 0 mto\n",cgxma);
	fprintf(cgstream,"\tysize lto\n");
	fprintf(cgstream,"\tstroke xtix x add} for\n");
	return(0);
}

/****************************************************************************/

int lwymesh(void)
{
	fprintf(cgstream,"/y {%g cvr} def\n",cgymi);
	fprintf(cgstream,"y ytix %g cvr {ys dup 0 exch mto\n",cgyma);
	fprintf(cgstream,"\txsize exch lto\n");
	fprintf(cgstream,"\tstroke ytix y add} for\n");
	return(0);
 }

/****************************************************************************/
/*************************** LOGARITHMIC MESHES *****************************/
/****************************************************************************/


int lwlxmesh(void)
{
      double c;
      int intmin,intdec;
      int mask, i, result;

      intmin=cgxmi;
      intdec=cgldecx/10;
      if ((cgxtix<= 2047) && (cgxtix >= 0)) {
	    mask =  MINBIT;
	    if((result = (mask & cgxtix)) != 0)
            	lwxminmset(cgxmi);
            mask = MAXBIT;
	    if((result = (mask & cgxtix)) != 0)
            	lwxmset(cgxma);
            if (intmin != intdec)    /* if 1x10^n hasn't been done already */
                  lwxmset(cgldecx);
		mask = 1;
	    for(i = 1; i < 10; i++) {
			mask = mask <<1;
		    if((result = (mask & cgxtix)) != 0)
			    lwxmset(wc_xbottom(i));
		    }

      }
      else {	/* DO EVERY POSSIBLE TICK */

	/* Tick the lower decade */
            fprintf(cgstream,"/x %g cvr def\n",cgxmi);
            fprintf(cgstream,"x %g cvr %g cvr {xs dup 0 mto\n",cgldecx/10,cgldecx);
            fprintf(cgstream,"\tysize lto\n");
            fprintf(cgstream,"\tstroke} for\n");

	/* Tick the upper decade */
            fprintf(cgstream,"/x %g cvr def\n",cgudecx);
            fprintf(cgstream,"x %g cvr %g cvr {xs dup 0 mto\n",cgudecx,cgxma);
            fprintf(cgstream,"\tysize lto\n");
            fprintf(cgstream,"\tstroke} for\n");

            if (cgudecx>cgldecx) {	/* If ther are any decades in between... */
                for (c=10*cgldecx;c<=cgxma;c=c*10)   /* for each decade */
  		{
		  /* Tick one decade */	
                      fprintf(cgstream,"/x {%g cvr} def\n",c);
                      fprintf(cgstream,"x 10 div dup x {xs dup 0 mto\n");
                      fprintf(cgstream,"\tysize lto\n");
                      fprintf(cgstream,"\tstroke} for\n");
                }
            }
      }
      return(0);
}

/****************************************************************************/

int lwxmset(float min)   /* DO A MESH LINE AT min, min*10, ETC...*/
{
	float x;
	for (x=min;x<=cgxma;x *= 10) {
		fprintf(cgstream,"%g cvr xs 0 mto\n",x);
		fprintf(cgstream,"0 ysize rlto\n");
		fprintf(cgstream,"stroke\n");
	}
	return(0);
}


/****************************************************************************/

int lwxminmset(float min)   /* DO A MESH LINE AT MIN ONLY */
{
	float x;
	x=min;
		fprintf(cgstream,"%g cvr xs 0 mto\n",x);
		fprintf(cgstream,"0 ysize rlto\n");
		fprintf(cgstream,"stroke\n");
	return(0);
}


/***************************************************************************/


int lwlymesh(void)
{
      double c;
      int intmin,intdec;
      int mask, i, result;

      intmin=cgymi;
      intdec=cgldecy/10;
      if ((cgytix<= 2047) && (cgytix >= 0)) {   /* If we aren't ticking everything */
	    mask = MINBIT;
    	    if((result = (mask & cgytix)) != 0)
	      	lwyminmset(cgymi);
	    mask = MAXBIT;
	    if((result = (mask & cgytix)) != 0)
            	lwymset(cgyma);
            if (intmin != intdec)  /* If 1x10^n hasn't been done already */
                  lwymset(cgldecy);
		mask = 1;
	    for(i = 1; i < 10; i++) {
			mask = mask <<1;
		    if((result = (mask & cgytix)) != 0)
			    lwymset(wc_ybottom(i));

	    }
      }
      else {  /* Mesh EVERYTHING */

	/* Mesh the lower decade */
            fprintf(cgstream,"/y %g cvr def\n",cgymi);
            fprintf(cgstream,"y %g cvr %g cvr {ys dup 0 exch mto\n",cgldecy/10,cgldecy);
            fprintf(cgstream,"\txsize exch lto\n");
            fprintf(cgstream,"\tstroke} for\n");

	/* Mesh the upper decade */
            fprintf(cgstream,"/y %g cvr def\n",cgudecy);
            fprintf(cgstream,"y %g cvr %g cvr {ys dup 0 exch mto\n",cgudecy,cgyma);
            fprintf(cgstream,"\txsize exch lto\n");
            fprintf(cgstream,"\tstroke} for\n");

            if (cgudecy>cgldecy) {  /* If there are any decades in between */
                 for (c=10*cgldecy;c<=cgudecy;c=10*c)     /* For each decade */
                 {   
  		    /* Mesh one decade */    		
                      fprintf(cgstream,"/y {%g cvr} def\n",c);
                      fprintf(cgstream,"y 10 div dup y {ys dup 0 exch mto\n");
                      fprintf(cgstream,"\txsize exch lto\n");
                      fprintf(cgstream,"\tstroke } for\n");
                }       
            }
      }
      return(0);
}

/****************************************************************************/

int lwymset(float min) /* Mesh-line all multiples of ten of min on the graph */
{
	float y;
	for (y=min;y<=cgyma;y *= 10) {
		fprintf(cgstream,"0 %g cvr ys mto\n",y);
		fprintf(cgstream,"xsize 0 rlto\n");
		fprintf(cgstream,"stroke\n");
	}
	return(0);
}


/****************************************************************************/

int lwyminmset(float min) /* MESH MIN VALUE ONLY */
{
	float y;
	y=min;
		fprintf(cgstream,"0 %g cvr ys mto\n",y);
		fprintf(cgstream,"xsize 0 rlto\n");
		fprintf(cgstream,"stroke\n");
	return(0);
}


