/******************************************************************/
/*	GSTYL.C   Defines the "style" of the x and y axes         */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/

/*	Modified on 5/13/92 by WEg to work on NeXT's
								  */
								  
#include "cgaxes.h"
#include "cglobals.h"
/**********************************************************************/

int cg_axis_enable(int axisEnable, int numberEnable)
{
	cgAxisEnable = axisEnable;
	cgAxisNumberEnable = numberEnable;
	return(0);
}

int cg_logax_style(int numsel,int ticsel,int numdist,
	int tnposition,int ticlen,int ticlen10,int numformat)
{

	if ((numsel >= 0) && (numsel <= 2047)) 
		cglognumsel = numsel;
	if ((ticsel >= 0) && (ticsel <= 2047)) 
		cglogticsel = ticsel;
	if ((numdist >= 0) && (numdist <= 100))
		cglognumdist = numdist;
	if ((tnposition > 0) && (tnposition <= 6)) 
		cglogtnposition = tnposition;
	if ((ticlen>0) && (ticlen < 100))
		cglogticlen = ticlen;
	if ((ticlen10 > 0) && (ticlen10 < 100))
		cglogticlen10 = ticlen10;
	else cglogticlen10 = cglogticlen;
	if (numformat == 1)
		cgnumformat = 1;
	return(0);
}


int cg_linax_style(int precision,int numoff,int numdist,int tnposition,int ticlen)
{

	if ((precision >= 0) && (precision < 11)) 
		cgprecision = precision;
	if ((numoff >= 0) && (numoff < 20)) 
		cglinnumoff = numoff;
	if ((numdist >= 0) && (numdist <= 100))
		cglinnumdist = numdist;
	if ((tnposition > 0) && (tnposition <= 6)) 
		cglintnposition = tnposition;
	if ((ticlen>0) && (ticlen < 100))
		cglinticlen = ticlen;
	return(0);
}
