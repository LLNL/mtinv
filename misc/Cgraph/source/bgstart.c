/* ########## OBSOLETE but needed for MAINDEF define ######################## */

/******************************************************************/
/*	GSTART.C   Imagraph 1024x1024x8 bit C interface module    */
/*	Izumi Ohzawa, Univ. of California, Berkeley		  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-3-87  starting					  */
/******************************************************************/
#define MAINDEF	1

#include "cglobals.h"
#include "cgaxes.h"
#include "cgfont.h"

/****************************************************************************/
int cg_setup(int choice)
{
	if (choice==0) printit = 2;
	return (0);
}

