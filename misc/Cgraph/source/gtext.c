/******************************************************************/
/*	GTEXT.C   Routines to print text on the screen or by the  */
/*	          laserwriter					  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/

/*	Modified by WEG on 5/13/92 to work on the NeXT machines
								  */
								  
#include "cglobals.h"

/****************************************************************************/
/* fix string for "(string) show" to properly handle PS escape chars        */
/* Used in: gtext.c, gxaxs.c, gyaxs.c, ginit.c where () show is used.	    */
/*          To find places where the fix may be necessary: grep show *.c    */
/* Add a line: char *fix_string(); in file "rtn.h"			    */
/****************************************************************************/

char *fix_string(char *text,int fix_minus)
{
char *sptr, *dptr;	/* source and destination pointer */
char ch;
static char out[128];	/* gotta exists after this function exits */
	sptr = text;
	dptr = out;
	while( (ch = *sptr++) != '\0')
	{
		if( ch == '-')
		{
			if(fix_minus)
			{
			   *dptr++ = '\\';	/* subst with octal 261 */
			   *dptr++ = '2';	/* "-" --> "\261" */
			   *dptr++ = '6';
			   *dptr++ = '1';
			}
		}
		else if( ch == '\\' || ch == '(' || ch == ')' )
		{
			*dptr++ = '\\';
			*dptr++ = ch;
		}
		else				/* normal characters */
			*dptr++ = ch;		/* just copy */
	}
	*dptr++ = '\0';		/* string terminator */
	return(out);	/* return ptr to the fixed string */
}


/* ***** print string  routine for PostScript device *************** */
int cg_text(float xpos, float ypos, float dir, char *text)
{
	fprintf(cgstream,"\n%% Text line.\n");
	if (cgTEXTAX)
		fprintf(cgstream,"%g %g sm\n",xpos,ypos);
	else
		fprintf(cgstream,"%g %g mto\n",xpos*cgSCALE,ypos*cgSCALE);
	if (dir == 0)
		fprintf(cgstream,"(%s) show\n", fix_string(text,1));
	else {
	  fprintf(cgstream," cpt\t%.2f rotate\t(%s) show\n",dir,fix_string(text,1));
	  fprintf(cgstream,"0 0 mto %.2f rotate\t",-dir);
	  if (cgTEXTAX)
	      fprintf(cgstream,"%g ys -1.0 mul %g ys -1.0 mul ",xpos,ypos);
	  else
	      fprintf(cgstream,"%g %g ",-xpos*cgSCALE,-ypos*cgSCALE);
	  fprintf(cgstream," mto cpt\n");
	}
	fprintf(cgstream,"%% END Text line.\n");
	return(0);
}

