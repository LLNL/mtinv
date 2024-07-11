/******************************************************************/
/*	GPRIM.C   Implements the fill() and closepath() commands  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   6-16-88  starting					  */
/******************************************************************/

/*	Modified by WEG on 5/13/92
								  */
								  

#include "cglobals.h"

/****************************************************************************/

int cg_fill(void)
{
	fprintf(cgstream," fill ");
	return(0);
}

int cg_eofill(void)
{
	fprintf(cgstream," eofill ");
	return(0);
}

/****************************************************************************/

int cg_closepath(void)
{
	fprintf(cgstream," closepath ");
	return(0);
}


int cg_gsave(void)
{
	fprintf(cgstream," gs ");
	return(0);
}

int cg_grestore(void)
{
	fprintf(cgstream," gr ");
	return(0);
}


int cg_postscript(char *postcmd) 
{
	fprintf(cgstream,"%s", postcmd);
	return(0);
}



