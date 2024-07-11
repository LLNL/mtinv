/******************************************************************/
/*	GINIT.C   ----------Initialization Module-------------    */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/
/* GINIT.C  -- Initialization Module ---------------------------- */
/*  history:
1989-03-08 Geoff Ghose
	Modified for integration with BGI graphics devices
1992-05-15 William Grosso
	Additional modifications
1994 ~ 1996-08-28 Izumi Ohzawa
	Lots of changes for NEXTSTEP.
1996-11-20 Izumi Ohzawa
	cg_use_stdout() to set a flag to send output to stdout.
	
*/

#include "version.h"
#include "cglobals.h"
#include "cgfont.h"
#include "cgaxes.h"
#include "cgraph.h"
#include <string.h>
#include <strings.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>

int markloaded [20];
static int filename_set = 0;
static int counter = 0;
static int hasBBox = 0;
static int hasTitle = 0;
static char *defprolog = "%!PS-Adobe-2.0 EPSF-2.0\n";
static char *tailor_rsc = "%%BeginResource: procset (Tailor group constructs)\n\
/TailorGroupBegin {pop} def /TailorGroupEnd {} def\n\
/pdfmark where\n\
{pop} {userdict /pdfmark /cleartomark load put} ifelse\n\
%%EndResource\n\n";


/* Make this global so this could be overridden via call to cg_setprolog() */
char *psprolog = NULL;
char *pstitle = NULL;
char *bboxstr = NULL;
char *topcomments = NULL;
char *tailorcomments = NULL;
char *progname_argv0 = NULL;	/* this should get argv[0] in main() */

/* static float xlimit, ylimit;  */	/* INCH RANGE of x and y */
/****************************************************************************/


int lw_init(int rot)
{	
uid_t myuid;
struct passwd *pw;
long timenow;
char timestr[32], username[32];
extern uid_t getuid(void);

	time(&timenow); strcpy(timestr, ctime(&timenow)); timestr[24] = '\0';
	myuid = getuid();
	pw = getpwuid(myuid);
	strncpy(username, pw->pw_name, 30);

	lw_laser_open();			/* open a stream to LaserWriter */

	if(psprolog == NULL) {
	    psprolog = (char *)malloc(strlen(defprolog) + 2);
	    strcpy(psprolog, defprolog);
	}
	fprintf(cgstream,"%s", psprolog);		/* output prolog */

	if (rot != 1) {
		if( !hasBBox) {
		    if(bboxstr)
		        fprintf(cgstream,"%%%%BoundingBox: %s\n", bboxstr);
		    else
		        fprintf(cgstream,"%%%%BoundingBox: 0 0 612 792\n");
		}
		if( !hasTitle) {
		    if(pstitle)
			fprintf(cgstream,"%%%%Title: %s\n", pstitle);
		    else
			fprintf(cgstream,"%%%%Title: %s (version %s.%s)\n", versmsg, versmajor, versminor);
		}
		fprintf(cgstream,"%%%%Orientation: Landscape\n");
		fprintf(cgstream,"%%%%Pages: 0 0\n%%%%EndComments\n\n");

		fprintf(cgstream,"0 792 moveto\ncurrentpoint translate\n");
		fprintf(cgstream,"270 rotate\n\n");
	}
	else {
		if(!hasBBox) {
		    if(bboxstr)
		        fprintf(cgstream,"%%%%BoundingBox: %s\n", bboxstr);
		    else
		        fprintf(cgstream,"%%%%BoundingBox: 0 0 612 792\n");
		}
		if( !hasTitle) {
		    if(pstitle)
			fprintf(cgstream,"%%%%Title: %s\n", pstitle);
		    else
			fprintf(cgstream,"%%%%Title: %s (version %s.%s)\n", versmsg, versmajor, versminor);
		}
		/* Other comments */
		if(progname_argv0 != NULL)
		    fprintf(cgstream, "%%%%Creator: %s\n", progname_argv0);
		fprintf(cgstream, "%%%%CreationDate: %s\n", timestr);
	        fprintf(cgstream, "%%%%For: %s\n", username);
		fprintf(cgstream, "%%%%Pages: 0 0\n%%%%EndComments\n\n");
	}

	if(topcomments != NULL)
	    fprintf(cgstream, "%s", topcomments);
				
	cgyorig= 0;
	cgxorig= 0;
	
	/* initialize strings for marker gray/color */
	cg_markergray(0.0, 1.0);	/* black stroke, white fill */

	/* DEFINE ALL OF THE SPACE-SAVING ABBREVIATIONS */
	fprintf(cgstream,"\n\n%%%%BeginResource: (%s; version %s.%s)\n\n", versmsg, versmajor, versminor);
	fprintf(cgstream,
	  "%% @@@@ Define 'true' to enable color [works only if cg_grayrgbcolor() is used].\n");
	switch(flex_graycolor) {
	    case 0:  /* no flex color, force B/W */
		fprintf(cgstream,"/__UseColor false def\n");
		break;
	    case 1:  /* no flex color, force color */
		fprintf(cgstream,"/__UseColor true def\n");
		break;
	    default:
	    case 2:  /* Flex color, force color */
		fprintf(cgstream,"%% /__UseColor false def\n");
		break;
	}
	fprintf(cgstream, "\n%% Let the use of color or B/W dependent on imaging device\n");
	fprintf(cgstream, "%% by checking if the device supports color or is B/W.\n");
	if(flex_graycolor < 2)
	    fprintf(cgstream,"%% ");	/* if flexible color is OFF, comment out def */
	fprintf(cgstream,
	"/__UseColor {statusdict begin /processcolors where {pop processcolors}{1} ifelse end 2 ge} def\n\n");

	fprintf(cgstream,
	"%% @@@@ Define 'true' to enable opaque background. See cg_opaqueBG().\n");

	if(opaque_BG) fprintf(cgstream,"/__OpaqueBG true def\n\n");
	else  fprintf(cgstream,"/__OpaqueBG false def\n\n");

	fprintf(cgstream,"/xs {72 mul} def\n/ys {72 mul} def\n");
	fprintf(cgstream,"/cxs {cvr xs} def\n/cys {cvr ys} def\n");
	fprintf(cgstream,"/cp {closepath} def\n");
	fprintf(cgstream,"/gs {gsave} def\n");
	fprintf(cgstream,"/gr {grestore} def\n");
	fprintf(cgstream,"/ff {findfont} def\n");
	fprintf(cgstream,"/stw {stringwidth} def\n");
	fprintf(cgstream,"/ssf {scalefont setfont} def\n");
	fprintf(cgstream,"/cpt {currentpoint translate} def\n");
	fprintf(cgstream,"/rlto {rlineto} def\n");
	fprintf(cgstream,"/rmto {rmoveto} def\n");
	fprintf(cgstream,"/lto {lineto} def\n");
	fprintf(cgstream,"/mto {moveto} def\n");
	fprintf(cgstream,"/setg {setgray} def\n");
	fprintf(cgstream,"/slw {setlinewidth} def\n");
	fprintf(cgstream,"/savlw {/clw currentlinewidth def} def\n");
	fprintf(cgstream,"/sl {cvr ys exch cvr xs exch lto} def\n");
	fprintf(cgstream,"/sm {cvr ys exch cvr xs exch mto} def\n");
/*	fprintf(cgstream,"/srectfill {cvr ys exch cvr xs exch rectifill} def\n"); */
	fprintf(cgstream,"/nd {[] 0 setdash} def\n");
	fprintf(cgstream,"/charheight {newpath 0 0 mto (I) false\n");
	fprintf(cgstream,"	charpath flattenpath pathbbox\n");
	fprintf(cgstream,"	exch pop exch sub exch pop newpath} def\n");
	fprintf(cgstream,"__UseColor { /setgrayrgbcolor { setrgbcolor pop } def }\n");
	fprintf(cgstream,"    { /setgrayrgbcolor { pop pop pop setgray } def } ifelse\n");
	fprintf(cgstream,"%%%%EndResource\n\n");

	fprintf(cgstream,"%s", tailor_rsc);

	fprintf(cgstream,"%%%%EndProlog\n\n");

	fprintf(cgstream, "(%% Generated by: %s\n", versmsg);
	if(tailorcomments != NULL)
	    fprintf(cgstream, "%s", tailorcomments);
	fprintf(cgstream,") TailorGroupBegin\n\n");

	/* Insert topcomments as an annotation pdfmark.  Comments will then become
	   an annotation in the Acrobat (PDF) file once the file is distilled by
	   an Acrobat Distiller.  It has no effect as long as the file remains in PS.
	*/
	if(tailorcomments != NULL) {
	    fprintf(cgstream, "%% Annotation for Acrobat (PDF) distiller\n");
	    fprintf(cgstream, "[ /Rect [0 0 300 150]\n");
	    fprintf(cgstream, "/Contents (");
	    fprintf(cgstream, "%s)\n", tailorcomments);
	    fprintf(cgstream, "/Title (Cgraph plot on %s)\n", timestr);
	    fprintf(cgstream, "/Open false\n");		/* initialize for iconized appearance */
	    fprintf(cgstream, "/ANN pdfmark\n\n");
	}

	fprintf(cgstream, "%% Executable PS code begins here.\n\n");
	fprintf(cgstream, "%% Background drawn here if enabled above. Args:(gray r g b)\n");
	fprintf(cgstream,"__OpaqueBG { %g %g %g %g setgrayrgbcolor clippath fill } if \n\n",
		bg_gray, bg_r, bg_g, bg_b);

	fprintf(cgstream, "0 0 0 0 setgrayrgbcolor 1 slw\n");
	fprintf(cgstream, "1 setlinecap 0 setlinejoin\n");
	fprintf(cgstream,"%% End of stuff generated by cg_init()\n\n");
	return(0);
}


/* User must copy the string to retain it */
char *cg_get_output_filename(void)
{
   return filnam;
}

/* this must be called before each call to cg_init() */
void cg_set_output_filename(char *file)
{
    filnam=address;
    strncpy(filnam, file, 1022);
    filename_set  = 1;
}

void cg_launch_preview(int flag)
{
     launchPreview = flag;
}

/***************************************************************************/
void cg_use_stdout(int flag)
{
    useStandardOutput = flag;	/* send output to stdout if non-zero */
}

/*----------------Set up to send to the laserwriter------------------------*/
void lw_laser_open(void)
{
extern int getpid(void);
	char pps[]=".eps";
	filnam=address;
	if(!filename_set) {
	    strcpy(filnam, "/tmp/.cg");
	    
	    append_int(getpid());
	    strcat(filnam, "-");
	    append_int(counter);
	    counter++;
	    filnam=strcat(filnam, pps);
	}
	if(useStandardOutput)
	    cgstream = stdout;			/* output to stdout */
	else
	    cgstream = fopen(filnam, "a");	/* open for append */
	if(cgstream == NULL) {
		fprintf(stderr, "Cannot open %s for PS output\n", filnam);
		return;
	}
	return;			/* successful file/device open */
	
}

/* used in showpage() to flush/close network i/o */
void lw_laser_close(void)
{
	fclose(cgstream);
	filename_set = 0;	
}

/***************************************************************************/
/* -- lw_message(str):  Prints a string to stdio of LaserWriter --- */
/*    For status, extra information display */

void lw_message(char *msgstring)
{
	fprintf(cgstream,"(%s) print flush\n", msgstring); /* version message */
}



/*
 Returns a line in outbuf upto '\n' from inbuf, keeps current position.
 Similar to fgets(), except it will read from another multi-line string.
 If no more lines, returns NULL.
 BUGS: must reset inbuf to something else before use.
*/
static  char *reset1 = "reset1";

char *sgets(char *outbuf, char *inbuf)
{
static char *lastinbuf=NULL;
static char *cptr;
static char *endbuf;	// points to NULL char at the end of str.
    if(lastinbuf != inbuf) {
	// input string changed, so update current positions.
	cptr = inbuf;
	lastinbuf = inbuf;
	endbuf = inbuf + strlen(inbuf);
    }
    if(cptr == NULL || cptr >= endbuf) {
	// Reached end of inbuf.
	return(NULL);
    }
    else {
	// return next line.
	sscanf(cptr, "%[^\n]", outbuf);	// no '\n' in outbuf
	strcat(outbuf, "\n");		// add '\n' at the end
    	cptr = index(cptr, '\n');	// position cptr on next '\n'.
	if(cptr != NULL) cptr++;	// point to the next line.
	return(outbuf);
    }
}

/* Set %%Creator: comment */
int cg_setcreator(char *ptext)
{
     if(progname_argv0) {
	free(progname_argv0);
	progname_argv0 = NULL;
     }
     progname_argv0 = (char *)malloc(strlen(ptext)+2);
     strcpy(progname_argv0, ptext);
     return(0);
}

/* Set %%Creator: comment */
int cg_settitle(char *ptext)
{
     if(pstitle) {
	free(pstitle);
	pstitle = NULL;
     }
     pstitle = (char *)malloc(strlen(ptext)+2);
     strcpy(pstitle, ptext);
     return(0);
}

/* Set %%BoundingBox: comment */
int cg_setboundingbox(char *ptext)
{
     if(bboxstr) {
	free(bboxstr);
	bboxstr = NULL;
     }
     bboxstr = (char *)malloc(strlen(ptext)+2);
     strcpy(bboxstr, ptext);
     return(0);
}



/* Set desired PS prolog.  This should be called before *cg_init()* call.
   Only true PS prolog comments should go here, not regular comments.
   Regular comments should be inserted by function cg_comments(char *);

   %%BoundingBox: may be included here, or use cg_setboundingbox() above.
   %%Title: may be included here, or use cg_settitle() above.
*/
int cg_setprolog(char *ptext)
{
char linebuf[256];
     if(psprolog) free(psprolog);	/* release memory for the old one */
     psprolog = (char *)malloc(strlen(ptext)+2);
     strcpy(psprolog, ptext);	

     sgets(linebuf, reset1);	// need to reset this function...
     hasBBox = 0;
     while( sgets(linebuf, psprolog) != NULL ) {
	if( strncmp(linebuf,"%%BoundingBox:", 14) == 0 ) {
	    hasBBox = 1;
	}
     }

     sgets(linebuf, reset1);	// need to reset this function...
     hasTitle = 0;
     while( sgets(linebuf, psprolog) != NULL ) {
	if( strncmp(linebuf,"%%Title:", 8) == 0 ) {
	    hasTitle = 1;
	}
     }

     return 0;
}

/* Comments that go after prolog, but before other definitions.
   This should be called *before* cg_init() call.
*/
int cg_set_top_comments(char *ptext)
{
char *ptr, *ptr2, ch;
int  escapecount;
    if(topcomments) free(topcomments);	/* release memory for the old one */
    topcomments = (char *)malloc(strlen(ptext)+2);
    strcpy(topcomments, ptext);

    if(tailorcomments) free(tailorcomments);		/* tailor comments escapes ( and ) */
    escapecount = 0;
    ptr = topcomments;
    while( (ch = *ptr++) != '\0')
	if( (ch == '(') || (ch == ')') || (ch == '\\') ) escapecount++;


    tailorcomments = (char *)malloc((size_t)(strlen(topcomments) + escapecount*2 + 16));
    ptr = topcomments;
    ptr2 = tailorcomments;
    while( (ch = *ptr++) != '\0') {
	if( (ch == '\\') || (ch == '(') || (ch == ')') ) {
	    *ptr2++ = '\\';
	    *ptr2++ = ch;
	}
	else
	    *ptr2++ = ch;
    }
    *ptr2 = '\0';

    return 0;
}

/* Other comments that can go anywhere in PS stream */
int cg_comments(char *comments) 
{
	if(comments != NULL)
	    fprintf(cgstream,"%s", comments);
	return(0);
}


/* --------------------------------------------------------------------------- */

int cg_init(int rot,int expand,float scale)
{
	int i;
	
	/* --- These initializations moved from gstart.c --- */


	cgSCALE = scale * POINTSPINCH;	  /* # of points per current length unit */
	for (i=0;i<20;i++)
		markloaded[i] = 0;	/* clear marker-loaded flags */
        cgMARKAX=  	1;  
        cgTEXTAX=  	0;  
        cgispolar=	0;  
        cgxcp=		0;  
        cgycp=		0;  
   	cgprecision=	4;
    	cglogticlen=	5;
    	cglinticlen=	5;
	cglinnumdist=	2;
	cglintnposition=2;
	cglognumdist=	2;
	cglogtnposition=2;
	cglognumsel=	3;
	cglogticsel=	2047;
    	cglinnumoff=	0;

	lw_init(rot);

	cg_font( 12.0, 10);	/* 12 point Times-Roman (ROMAND.XSH) as default */
	return(0);
}

void append_int(int inumber)
{
char tnumbuf[64];
	sprintf(tnumbuf, "%d", inumber);
	strncat(filnam, tnumbuf, 60);
}	

