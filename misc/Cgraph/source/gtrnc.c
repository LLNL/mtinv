/******************************************************************/
/*	GTRNC.C   Routine to reduce a floating point number to	  */
/*	          a one-significant-digit number   		  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/
#include  "cglobals.h"

#define DOWN 	0
#define UP 	1
#define EVEN 	2

/****************************************************************************/
float wc_trunc(float fvalue, int updown)
{
float decade,result;
double dublog;

      dublog= log10(fvalue);		/* fvalue = 10^dublog */
      dublog= floor(dublog);            /* how many decimal places */

                              	        /* AS AN EXAMPLE:                   */
      decade= pow(10.0, dublog);        /* If fvalue = 8765, decade = 1000  */
      result= fvalue/decade;            /*       and result = 8.765         */
      if (updown == UP) 
            result = ceil(result);      /*       result = 9 (round UP)      */
      else if (updown == DOWN)
            result = floor(result);     /*     result = 8 (round DOWN)      */
      else result= floor(result + 0.5); /*else round to whichever is closest*/

      result *= decade;                 /*       result = 8000 or 9000      */  
      return(result);
}

/**********************************************************************/
