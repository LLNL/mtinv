/******************************************************************/
/*	GLCMP.C   Revises an exponential numeric output string    */
/*		  to look crisper and cleaner          		  */
/*	Wayne Crawford, Univ. of California, Berkeley		  */
/*	   9-09-87  starting					  */
/******************************************************************/
#include "cglobals.h"



/****************************************************************************/

char *wc_linecomp(char *line, int Ept)
{
int count;
    if (line[Ept-1] == '.')    	/* ERASE THE DECIMAL POINT */
    {
        count=Ept-1;
        --Ept;
        while (line[count] != '\0')  /* Shift the string back by one */
 	{
            line[count]= line[count+1];
            ++count; 
        }
    }    
    if (line[Ept+1] == '+')     /* ERASE the PLUS sign AFTER the E */
    {
        count=Ept+1;
        --Ept;
        while (line[count] != '\0')  /* Shift the string back by one */
	{
            line[count]= line[count+1];
            ++count; 
        }
    }    
    while (line[Ept+2] == '0')     /* ERASE ANY UNNECESSARY ZEROES */
    {	
        count=Ept+2;
        while (line[count] != '\0')  /* Shift the string back by one */
	{
            line[count]= line[count+1];
            ++count;
        }
    }
    return( line );
}

/****************************************************************************/
