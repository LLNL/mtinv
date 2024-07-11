#include "cglobals.h"

int cg_coord_select(int marks,int text) 	/* 0 = inches, 1 = axis coordinates */
{
	cgMARKAX = marks;
	cgTEXTAX = text;
	return(0);
}
