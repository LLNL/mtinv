/* 

5-14-92	Taken over by WEG

*/



#include "cglobals.h"
#include "cgaxes.h"

#define TRUE 1
#define FALSE 0


int findsigdec(float min, float range, int numsep, float ticks)
{
	float var;
	int expon,count,decpt,sigdec;
	char line[30];
	
	sigdec=0;
	
    for (var= cglinnumoff*ticks;var<=range;var += numsep*ticks) {
	if((var +min) < pow(10.0,(float)cgprecision))
		sprintf(line, "%*g", cgprecision, var+min);
	else sprintf(line, "%*e", cgprecision, var+min);
	
	/* convert number to string form */
        expon= FALSE;
        count=0;
        decpt=0;
        while (line[count] != '\0') {
            if (line[++count] == 'e' ) {	/* search for exponent marker */
                expon=TRUE;
            }
            if (line[count] == '.' )   /* locate decimal point */
                    decpt=count;
        }
	if (decpt == 0) decpt = count - 1;	/* decpt is end of number */
        if (expon== FALSE) 
            sigdec=((count-decpt-1)>sigdec)?(count-decpt-1):sigdec;
     } /* end for() */
     return(sigdec);
}

void formaxnum(float value, int sigdec,char *line)
{
	
int expon, count, decpt, ept=0;
	/* gcvt(x+cgxmi,cgprecision,line); */
	/* write number with trailing zeros */
	sprintf(line, "%#*f", cgprecision, value);
	/* convert number to string form */
        expon= FALSE;
        count=0;
        decpt=0;
        while (line[count] != '\0') {
            if (line[++count] == 'e' ) {
                expon=TRUE;
                ept=count;
            }
            if (line[count] == '.' ) 
                decpt=count;
        }
        if (expon == TRUE) {  /* need to clean up gcvt's exp notation */ 
	    wc_linecomp(line,ept);
            count= -1;
            while (line[++count] != '\0'); 
        }
	if(decpt) {
		if (sigdec) line[sigdec+decpt+1]='\0';
		else line[decpt]='\0';
	}
        
}
