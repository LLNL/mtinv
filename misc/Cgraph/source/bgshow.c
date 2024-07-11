/******************************************************************/
/*	GSHOW.C   Necessary for the laserwriter to send a page    */
/*                through                          		  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/

/*	Modified on 5/13/92 by WEG (to use on NeXT's)		  */
/*	Added suggestion by G.M. Ghose 1998-08-29 for DISPLAYER */

#include <stdio.h>
#include <string.h>
#include "cglobals.h"

/* Adjust DISPLAYER command appropriately for your platform's PostScript
   viewer application.
*/

#ifdef NeXT
#define DISPLAYER "open -a Preview -temp %s"

#else

#define DISPLAYER "gs %s"
#endif


/****************************************************************************/

int cg_showpage(void) 
{
char strbuf[128];
int retv;

	fprintf(cgstream,"\n\n%% All drawing done.  Commit to imaging device.\n");
	fprintf(cgstream,"showpage\n");
	fprintf(cgstream,"TailorGroupEnd\n\n");
	fprintf(cgstream,"%%%%Trailer\n");	/* Don't put %%PageTrailer */
	lw_laser_close();

	/* Launch PS Preview.app with temp file so Preview deletes it when done. */
	/* ##### Modify these two lines to launch Ghostscript/GhostView on other systems */
	if(!useStandardOutput && launchPreview) {
	    sprintf(strbuf, DISPLAYER, filnam);
	    retv = system(strbuf);
	}
	return(0);
}

