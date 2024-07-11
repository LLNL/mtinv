


#include "cglobals.h"

int cg_save(int flag)
{
/*	To set the saving switch. */
	if ((flag==0) || (flag==1))
	saveit = flag;
	return(0);
}
