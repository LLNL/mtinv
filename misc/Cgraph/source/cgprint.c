


#include "cglobals.h"

int cg_print(int flag)
{
/*	To set the saving switch. */
	if ((flag==0) || (flag==1) || (flag==2))
	printit = flag;
	return(0);
}
