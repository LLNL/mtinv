/******************************************************************/
/*	GBOTT.C   						  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/
#include "cglobals.h"
#include "cgaxes.h"

/****************************************************************************/

float wc_ybottom(int multiplier) /*finds the lowest multiple of 10 of multiplier*/
{
	float bottom;

	bottom= cgldecy*((float)multiplier)/10.;
	while (bottom < cgymi) bottom *= 10.;
	return(bottom);
}

/****************************************************************************/

float wc_xbottom(int multiplier) /*finds the lowest multiple of 10 of*/
{
	float bottom;

	bottom= cgldecx*((float)multiplier)/10.;
	while (bottom < cgxmi) bottom *= 10.;
	return(bottom);
}

/***************************************************************************/
