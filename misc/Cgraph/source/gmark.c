/******************************************************************/
/*	GMARK.C   Print a marker or pointer of the specified type */
/*	          and size. The difference between a marker and a */
/*	          pointer is that a marker is centered on the     */
/*	          given coordinate, while a pointer has its tip   */
/*	          at that coordinate 				  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/
/*  Modification history -------------------------------------------


89-3-13 by GG
	For x and y scaling and floating point calc.
89-3-30 by IO
	Use of im_paint() eliminated.  Filled markers are now filled
	by dense line drawings.
92-5-13 by WEG
	Making it NeXT compatible.	

96-08-25 by Izumi
	Use of flexible color via cg_makergraycolor().

*------------------------------------------------------------------*/

#include "cglobals.h"

extern int markloaded[];


/****************************************************************************/

int cg_marker(float x, float y, int type, float size)
{
    /* fprintf(cgstream,"\n\n%% Marker symbol\n"); */
    fprintf(cgstream,"\ngs newpath nd\n");  /*Save user-defined linewidth */
    fprintf(cgstream,"/size %g def\n",size);
    if (cgMARKAX)	
    	fprintf(cgstream,"/x %g xs def\t/y %g ys def\n",x,y);  /* Set the marker spot */
    else	
    	fprintf(cgstream,"/x %g def\t/y %g def\n",x*cgSCALE,y*cgSCALE);  /* Set the marker spot (inches)*/

    type= type%20;   
    if (!markloaded[type])	/* if marker definition already loaded, load */
    {
	loadmark(type);
	markloaded[type] = 1;
    }	
    switch(type)  {
        case 0: /*OPEN CIRCLE*/
            fprintf(cgstream,"WCircle ");
            break;
        case 1: /*FILLED CIRCLE*/
            fprintf(cgstream,"BCircle ");
            break;
        case 2: /*OPEN TRIANGLE*/
            fprintf(cgstream,"Wtriangle ");
            break;
        case 3: /*FILLED TRIANGLE*/
            fprintf(cgstream,"Btriangle ");
            break;
        case 4: /* OPEN DIAMOND */
            fprintf(cgstream,"Wdiamond ");
            break;
        case 5: /* FILLED DIAMOND */
            fprintf(cgstream,"Bdiamond ");
            break;
        case 6: /* OPEN SQUARE */
            fprintf(cgstream,"Wsquare ");
            break;
        case 7: /* FILLED SQUARE */
            fprintf(cgstream,"Bsquare ");
            break;
        case 8: /*OPEN STAR*/               
            fprintf(cgstream,"Wstar ");
            break;
        case 9: /*FILLED STAR*/
            fprintf(cgstream,"Bstar ");
            break;
        case 10: /*OPEN PENTAGON*/
            fprintf(cgstream,"Wpent ");
            break;
        case 11: /*FILLED PENTAGON*/
            fprintf(cgstream,"Bpent ");
             break;
        case 12: /*OPEN INVERTED TRIANGLE*/
            fprintf(cgstream,"Wintri ");
            break;
        case 13: /*FILLED INVERTED TRIANGLE*/
            fprintf(cgstream,"Bintri ");
            break;
        case 14: /*BOWTIE*/
            fprintf(cgstream,"Bowtie ");
            break;
        case 15: /*JIGGER*/
            fprintf(cgstream,"Jigger ");
            break;
        case 16: /*ASTERISK*/
            fprintf(cgstream,"Asterisk ");
            break;
        case 17: /*CROSSHAIRS*/
            fprintf(cgstream,"Xhairs ");
            break;
        case 18: /*SPINNING WHEEL*/
            fprintf(cgstream,"Spinner ");
            break;
        case 19: /*CROSSHATCH*/
            fprintf(cgstream,"Xhatch ");
            break;
        default:
            break;
    }
    fprintf(cgstream," gr\n");   /* Restore non-marker GS */
    /* fprintf(cgstream,"%% END Marker symbol\n"); */
    return(0);
}


/***************************************************************************/

int loadmark(int marknum)    /* SEND OUT AN UNLOADED MARKER */
{
    if (marknum > 14)
    {
    	loadmark3(marknum);
	return(0);
    }
    else if (marknum > 7)
    {
    	loadmark2(marknum);
	return(0);
    }

    switch(marknum)
    {
	case 0:	/* OPEN CIRCLE */
		fprintf(cgstream,"/WCircle {\n");
		fprintf(cgstream,"	size 8 div slw\n");
		fprintf(cgstream,"	x y size .5 mul 0 360 arc closepath\n");
		fprintf(cgstream,"	gs %s fill gr\n", fill_graycolor);
		fprintf(cgstream,"	%s stroke} def\n\n", stroke_graycolor);
		break;
	case 1:	/* FILLED CIRCLE */
		fprintf(cgstream,"/BCircle {\n");
		fprintf(cgstream,"	size 8 div slw\n");
		fprintf(cgstream,"	x y size .5 mul 0 360 arc closepath\n");
		fprintf(cgstream,"	gs %s fill gr\n", stroke_graycolor);
		fprintf(cgstream,"	%s stroke} def\n\n", stroke_graycolor);
		break;
	case 2:	/* OPEN TRIANGLE */
		fprintf(cgstream,"/Wtriangle {\n");
		fprintf(cgstream,"	size 8 div slw\n");
		fprintf(cgstream,"	x .667 size mul y add mto\n");
		fprintf(cgstream,"	size neg 1.7321 div size neg rlto\n");
	      	fprintf(cgstream,"	size .8661 div 0 rlto\n");
		fprintf(cgstream,"	cp gs %s fill gr\n", fill_graycolor);
		fprintf(cgstream,"	%s stroke} def\n\n", stroke_graycolor);
		break;
	case 3: /* FILLED TRIANGLE */
		fprintf(cgstream,"/Btriangle {\n");
		fprintf(cgstream,"	size 8 div slw\n");
		fprintf(cgstream,"	x .667 size mul y add mto\n");
		fprintf(cgstream,"	size neg 1.7321 div size neg rlto\n");
	      	fprintf(cgstream,"	size .8661 div 0 rlto\n");
		fprintf(cgstream,"	cp gs %s fill gr\n", stroke_graycolor);
		fprintf(cgstream,"	%s stroke} def\n\n", stroke_graycolor);
		break;
	case 4:	/* OPEN DIAMOND */
		fprintf(cgstream,"/Wdiamond {\n");
		fprintf(cgstream,"	size 8 div slw\n");
		fprintf(cgstream,"	x size 1.38 div y add mto\n");
		fprintf(cgstream,"	size .546 mul size 1.38 div neg rlto\n");
		fprintf(cgstream,"	size .546 mul neg size 1.38 div neg rlto\n");
		fprintf(cgstream,"	size .546 mul neg size 1.38 div  rlto\n");
		fprintf(cgstream,"	cp gs %s fill gr\n", fill_graycolor);
		fprintf(cgstream,"	%s stroke} def\n\n", stroke_graycolor);
		break;
	case 5:	/* FILLED DIAMOND */
		fprintf(cgstream,"/Bdiamond {\n");
		fprintf(cgstream,"	size 8 div slw\n");
		fprintf(cgstream,"	x size 1.38 div y add mto\n");
		fprintf(cgstream,"	size .546 mul size 1.38 div neg rlto\n");
		fprintf(cgstream,"	size .546 mul neg size 1.38 div neg rlto\n");
		fprintf(cgstream,"	size .546 mul neg size 1.38 div  rlto\n");
		fprintf(cgstream,"	cp gs %s fill gr\n", stroke_graycolor);
		fprintf(cgstream,"	%s stroke} def\n\n", stroke_graycolor);
		break;
	case 6:	/* OPEN SQUARE */
		fprintf(cgstream,"/Wsquare {\n");
		fprintf(cgstream,"	size 8 div slw\n");
		fprintf(cgstream,"	size 2 div dup x add exch y\n");
		fprintf(cgstream,"	add mto 0 size neg rlto size\n"); 
		fprintf(cgstream,"	neg 0 rlto 0 size rlto cp gs\n");
		fprintf(cgstream,"	%s fill gr\n", fill_graycolor); 
		fprintf(cgstream,"	%s stroke} def\n\n", stroke_graycolor);
		break;
	case 7:	/* FILLED SQUARE */
		fprintf(cgstream,"/Bsquare {\n");
		fprintf(cgstream,"	size 8 div slw\n");
		fprintf(cgstream,"	size 2 div dup x add exch y\n");
		fprintf(cgstream,"	add mto 0 size neg rlto size\n"); 
		fprintf(cgstream,"	neg 0 rlto 0 size rlto cp gs\n");
		fprintf(cgstream,"	%s fill gr\n", stroke_graycolor); 
		fprintf(cgstream,"	%s stroke} def\n\n", stroke_graycolor);
		break;
	default: /*ERROR */
		fprintf(stderr, "There has been some error in loadmark()\n");
		break;
    }
    return(0);
}

/***************************************************************************/


int loadmark2(int marknum)    /* SEND OUT AN UNLOADED MARKER */	
{
    switch(marknum)
    {			
	case 8:	/* OPEN STAR: fill stroke must be reversed for this, and size 5 div */
		/* only half of width of perimeter stroke will show up. */
		fprintf(cgstream,"/Wstar {\n");
		fprintf(cgstream,"	size 4 div slw\n");
		fprintf(cgstream,"	.5257 size mul neg x add\n");
		fprintf(cgstream,"	.1708 size mul y add mto\n");
		fprintf(cgstream,"	1.0515 size mul 0 rlto\n");
		fprintf(cgstream,"	size .8507 mul neg size .6181 mul neg rlto\n");
		fprintf(cgstream,"	.3249 size mul size rlto\n");
		fprintf(cgstream,"	.3249 size mul size neg rlto\n");
		fprintf(cgstream,"	cp\n");
		fprintf(cgstream,"	gs %s stroke gr %s fill} def\n\n",
			stroke_graycolor, fill_graycolor);
		break;
	case 9:	/* FILLED STAR */
		fprintf(cgstream,"/Bstar {\n");
		fprintf(cgstream,"	size 4 div slw\n");
		fprintf(cgstream,"	.5257 size mul neg x add\n");
		fprintf(cgstream,"	.1708 size mul y add mto\n");
		fprintf(cgstream,"	1.0515 size mul 0 rlto\n");
		fprintf(cgstream,"	size .8507 mul neg size .6181 mul neg rlto\n");
		fprintf(cgstream,"	.3249 size mul size rlto\n");
		fprintf(cgstream,"	.3249 size mul size neg rlto\n");
		fprintf(cgstream,"	cp\n");
		fprintf(cgstream,"	gs %s stroke gr %s fill} def\n\n",
			stroke_graycolor, stroke_graycolor);
		break;
	case 10: /* OPEN PENTAGON */
		fprintf(cgstream,"/Wpent {\n");
		fprintf(cgstream,"	size 8 div slw\n");
		fprintf(cgstream,"	.5257 size mul neg x add\n");
		fprintf(cgstream,"	.1708 size mul y add mto\n");
		fprintf(cgstream,"	.5257 size mul .382 size mul rlto\n");
		fprintf(cgstream,"	.5257 size mul .382 size neg mul rlto\n");
		fprintf(cgstream,"	size neg dup .2008 mul exch .6181 mul rlto\n");
		fprintf(cgstream,"	size neg .6499 mul 0 rlto\n");
		fprintf(cgstream,"	cp gs %s fill gr\n", fill_graycolor);
		fprintf(cgstream,"	%s stroke} def\n\n", stroke_graycolor);
		break;
	case 11: /* FILLED PENTAGON */
		fprintf(cgstream,"/Bpent {\n");
		fprintf(cgstream,"	size 8 div slw\n");
		fprintf(cgstream,"	.5257 size mul neg x add\n");
		fprintf(cgstream,"	.1708 size mul y add mto\n");
		fprintf(cgstream,"	.5257 size mul .382 size mul rlto\n");
		fprintf(cgstream,"	.5257 size mul .382 size neg mul rlto\n");
		fprintf(cgstream,"	size neg dup .2008 mul exch .6181 mul rlto\n");
		fprintf(cgstream,"	size neg .6499 mul 0 rlto\n");
		fprintf(cgstream,"	cp gs %s fill gr\n", stroke_graycolor);
		fprintf(cgstream,"	%s stroke} def\n\n", stroke_graycolor);
		break;
	case 12: /* OPEN INVERTED TRIANGLE */
		fprintf(cgstream,"/Wintri {\n");
		fprintf(cgstream,"	size 8 div slw\n");
		fprintf(cgstream,"	x .667 size mul neg y add \n");
		fprintf(cgstream,"	mto size dup 1.7321 div exch rlto\n");
		fprintf(cgstream,"	size neg .8661 div 0 rlto\n");
		fprintf(cgstream,"	cp gs %s fill gr\n", fill_graycolor);
		fprintf(cgstream,"	%s stroke} def\n\n", stroke_graycolor);
		break;
	case 13: /* FILLED INVERTED TRIANGLE */
		fprintf(cgstream,"/Bintri {\n");
		fprintf(cgstream,"	size 8 div slw\n");
		fprintf(cgstream,"	x .667 size mul neg y add \n");
		fprintf(cgstream,"	mto size dup 1.7321 div exch rlto\n");
		fprintf(cgstream,"	size neg .8661 div 0 rlto\n");
		fprintf(cgstream,"	cp gs %s fill gr\n", stroke_graycolor);
		fprintf(cgstream,"	%s stroke} def\n\n", stroke_graycolor);
		break;
	case 14: /* BOWTIE */
		fprintf(cgstream,"/Bowtie {\n");
		fprintf(cgstream,"	size 8 div slw\n");
		fprintf(cgstream,"	size 2 div dup neg x add exch y\n");
		fprintf(cgstream,"	add mto size dup neg rlto 0 size \n");
		fprintf(cgstream,"	rlto size neg dup rlto cp gs\n");
		fprintf(cgstream,"	%s fill gr %s stroke} def\n\n",
			fill_graycolor, stroke_graycolor);
		break;
	default: /* ERROR */
		fprintf(stderr, "There has been some error in loadmark2()\n");
		break;
    }
    return(0);
}


/***************************************************************************/

int loadmark3(int marknum)    /* SEND OUT AN UNLOADED MARKER */	
{
    switch(marknum)
    {			
	case 15: /* Jigger */
		fprintf(cgstream,"/Jigger {\n");
		fprintf(cgstream,"	size 8 div slw\n");
		fprintf(cgstream,"	size 2 div dup neg x add exch y\n");
		fprintf(cgstream,"	add mto size dup neg rlto size neg\n");
		fprintf(cgstream,"	0 rlto size dup rlto cp gs\n");
		fprintf(cgstream,"	%s fill gr %s stroke} def\n\n",
					fill_graycolor, stroke_graycolor);
		break;
	case 16: /* ASTERISK */
		fprintf(cgstream,"/Asterisk {\n");
		fprintf(cgstream,"	size 8 div slw\n");
		fprintf(cgstream,"	x size 2 div y add mto\n");
		fprintf(cgstream,"	0 size neg rlto\n");
		fprintf(cgstream,"	size dup neg .433 mul exch 4 div rmto\n");
		fprintf(cgstream,"	size dup .866 mul exch 2 div rlto\n");
		fprintf(cgstream,"	0 size neg 2 div rmto\n");
		fprintf(cgstream,"	size dup neg .866 mul exch 2 div rlto\n");
		fprintf(cgstream,"	%s stroke} def\n\n", stroke_graycolor);
		break;
	case 17: /* CROSSHAIRS */
		fprintf(cgstream,"/Xhairs {\n");
		fprintf(cgstream,"	size 10 div slw\n");
		fprintf(cgstream,"	x y size 4 div 0 360 arc\n");
		fprintf(cgstream,"	gs %s fill gr %s\n", fill_graycolor, stroke_graycolor);
		fprintf(cgstream,"	stroke\n");
		fprintf(cgstream,"	x size 4 div y add mto\n");
		fprintf(cgstream,"	0 size 4 div rlto\n");
		fprintf(cgstream,"	size 2 div dup neg rmto\n");
		fprintf(cgstream,"	size neg 4 div 0 rlto\n");
		fprintf(cgstream,"	size neg 4 div dup rmto\n");
		fprintf(cgstream,"	0 size neg 4 div rlto\n");
		fprintf(cgstream,"	size 2 div dup neg exch rmto\n");
		fprintf(cgstream,"	size 4 div 0 rlto\n");
		fprintf(cgstream,"	0 size 2 div rmto\n");
		fprintf(cgstream,"	size 2 div 0 rlto\n");
		fprintf(cgstream,"	size 4 div dup neg rmto\n");
		fprintf(cgstream,"	0 size neg 2 div rlto\n");
		fprintf(cgstream,"	size neg 4 div dup rmto\n");
		fprintf(cgstream,"	size neg 2 div 0 rlto\n");
		fprintf(cgstream,"	size 4 div dup neg exch rmto\n");
		fprintf(cgstream,"	0 size 2 div rlto\n");
	   	fprintf(cgstream,"	%s stroke} def\n\n", stroke_graycolor);
		break;
	case 18: /* SPINNING WHEEL */
		fprintf(cgstream,"/Spinner {\n");
		fprintf(cgstream,"	size 10 div slw\n");
		fprintf(cgstream,"	size 4 div dup neg x add exch y\n");
		fprintf(cgstream,"	add mto size 2 div 0 rlto\n");
		fprintf(cgstream,"	0 size neg 2 div rlto\n");
		fprintf(cgstream,"	size neg 2 div 0 rlto cp\n");
		fprintf(cgstream,"	1 setg gs fill gr 0 setg\n");
		fprintf(cgstream,"	size 4 div dup neg exch rlto\n");
		fprintf(cgstream,"	size .75 mul dup 3 div neg rlto\n");
		fprintf(cgstream,"	size 4 div dup rlto\n");
		fprintf(cgstream,"	size neg 4 div dup 3 mul rlto\n");
		fprintf(cgstream,"	size 4 div dup neg rlto\n");
		fprintf(cgstream,"	size 4 div dup neg 3 mul exch rlto\n");
		fprintf(cgstream,"	size 4 div neg dup rlto\n");
		fprintf(cgstream,"	size 4 div dup 3 mul rlto %s stroke} def\n\n",
				stroke_graycolor);
		break;
	case 19: /* CROSSHATCH */
		fprintf(cgstream,"/Xhatch {\n");
		fprintf(cgstream,"	size 8 div slw\n");
		fprintf(cgstream,"	size dup neg 4 div x add exch 2\n");
		fprintf(cgstream,"	div y add mto size .75 mul dup\n");
		fprintf(cgstream,"	neg rlto size neg 4 div dup rmto\n");
		fprintf(cgstream,"	size .75 mul dup neg exch rlto\n");
		fprintf(cgstream,"	0 size neg 2 div rmto\n");
 		fprintf(cgstream,"	size .75 mul dup rlto\n");
		fprintf(cgstream,"	size 4 div dup neg rmto\n");
		fprintf(cgstream,"	size neg .75 mul dup rlto\n");
		fprintf(cgstream,"	%s stroke} def\n\n", stroke_graycolor);
		break;
	default: /*ERROR*/
		fprintf(stderr, "Error: loadmark3()\n");
		break;
    }
    return(0);
}




/***************************************************************************/


int cg_pointer(float x, float y, int type, float size)
{
char *pdir[] = {"R", "D", "L", "U", ""};

    fprintf(cgstream,"\n\n%% Pointer marker: %s\n\n", pdir[type]);
    fprintf(cgstream,"\ngs newpath nd\n");
    fprintf(cgstream,"/size %g def\n",size);
    fprintf(cgstream,"/x %g def\t/y %g def\n",x,y);
    type= type%4;
    switch(type)  {
        case 0: /*POINTER RIGHT*/
            fprintf(cgstream,"\tsize 8 div slw\tx xs y ys mto\n");
            fprintf(cgstream,"\tsize neg size 3. div rlto\t0 size 1.5 div neg rlto\n");
            fprintf(cgstream,"\tcp gs %s fill gr %s stroke\n",
					fill_graycolor, stroke_graycolor);
            break;
        case 1: /*POINTER DOWN*/
            fprintf(cgstream,"\tsize 8 div slw\tx xs y ys mto\n");
            fprintf(cgstream,"\tsize 3. div size rlto\tsize 1.5 div neg 0 rlto\n");
            fprintf(cgstream,"\tcp gs %s fill gr %s stroke\n",
					fill_graycolor, stroke_graycolor);
            break;
        case 2: /*POINTER LEFT*/
            fprintf(cgstream,"\tsize 8 div slw\tx xs y ys mto\n");
            fprintf(cgstream,"\tsize size 3. div rlto\t0 size 1.5 div neg rlto\n");
            fprintf(cgstream,"\tcp gs %s fill gr %s stroke\n",
					fill_graycolor, stroke_graycolor);
            break;
        case 3: /*POINTER UP*/
            fprintf(cgstream,"\tsize 8 div slw\tx xs y ys mto\n");
            fprintf(cgstream,"\tsize 3. div size neg rlto\tsize 1.5 div neg 0 rlto\n");
            fprintf(cgstream,"\tcp gs %s fill gr %s stroke\n",
					fill_graycolor, stroke_graycolor);
            break;
      default:
            break;
    }
    fprintf(cgstream," gr\n");    /* Restore non-marker GS */
    fprintf(cgstream,"%% END Pointer marker: %s\n\n", pdir[type]);
    return(0);
}

