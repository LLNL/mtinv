/***********************************************************************************/
/*** Copyright 2024 Gene A. Ichinose (LLNL)                                      ***/
/***                                                                             ***/
/*** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” ***/
/*** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  ***/
/*** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE   ***/
/*** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         ***/
/*** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        ***/
/*** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    ***/
/*** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     ***/
/*** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     ***/
/*** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF      ***/
/*** THE POSSIBILITY OF SUCH DAMAGE.                                             ***/
/***                                                                             ***/
/*** Prepared by LLNL under Contract DE-AC52-07NA27344.                          ***/
/***********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strings.h>

#include "../include/mt.h"

/*** read a station file and search for station lat lon ***/

void getsta( char *filename, Greens *g, int *verbose )
{
        FILE *fp;
        char rec[256];
        int ista;
        Station *s;
	
	/* char dateid[37]; */
        /* int byr,bjday,bhr,bmin,bsec, eyr,ejday,ehr,emin,esec; */

 	extern char progname[128];
                                                                                                                             
        int stringlength( char * );
        int searchstring( char *, char * );
        void mytoken( char *, char, int, char * );
                                                                                                                              
        if( (fp = fopen(filename, "r"))==NULL )
        {
                printf("%s: ERROR could not open station database file %s\n",
                        progname, filename);
                exit(-1);
        }
        ista = 0;
        s = malloc( sizeof(Station) );
        while( fgets( rec, 256, fp ) != NULL )
        {
		/*** comments allowed ***/
		if( rec[0] == '#' ) continue;

                s = (Station *)realloc( s, (ista+1)*sizeof(Station) );
                sscanf(rec, "%s %s %f %f %f",
                        s[ista].id, s[ista].net,
                        &(s[ista].lat), &(s[ista].lon),
                        &(s[ista].elev)
                );

                /* mytoken( rec, '"', 2, &(s[ista].chan) ); */
                mytoken( rec, '"', 4, s[ista].description );

        /**** convert date part of record *****/
/******
                convs(rec, stringlength(rec)-36, stringlength(rec)-1, dateid );
                sscanf(dateid, "%4d,%3d,%2d:%2d:%2d %4d,%3d,%2d:%2d:%2d",
                        &byr,&bjday,&bhr,&bmin,&bsec, &eyr,&ejday,&ehr,&emin,&esec);
                setdate( &(s[ista].beg), byr,bjday,bhr,bmin,bsec );
                setdate( &(s[ista].end), eyr,ejday,ehr,emin,esec );
*****/
                if( strcmp( s[ista].id, g->stnm ) == 0 && strcmp( s[ista].net, g->net ) == 0 )
                {
			if(*verbose)
			{
                          printf("%-4.4s %-2.2s %12.5f %12.5f %12.5f %s\n",
                                s[ista].id,
                                s[ista].net,
                                s[ista].lat,
                                s[ista].lon,
                                s[ista].elev,
                                s[ista].description );
			}
                        g->stla = s[ista].lat;
                        g->stlo = s[ista].lon;
                        g->stel = s[ista].elev;

			if(*verbose)
			{
                         printf("%s: station %s.%s found from file %s\n",
                          progname, g->stnm, g->net, filename );
                        }
			
                        if(*verbose)
			{
                          printf("%s: \t\t%s net=%s lat=%g lon=%g elev=%g\n",
                            progname, g->stnm, g->net, g->stla, g->stlo, g->stel );
			}

                        free(s);
                        fclose(fp);
                        return;
                }
        }
        printf("%s: station %s not found from file %s.  EXITING! \n",
                progname, g->stnm, filename );
        free(s);
        fclose(fp);
        exit(-1);
}


int searchstring( char *str1, char *str2 ) /* find str2 in str1 */
{
        int sl1, sl2, i;
        char tmp1[512];

	int stringlength( char * );
        void convs( char *, int, int, char * );

        sl1 = stringlength( str1 )-1;
        sl2 = stringlength( str2 )-1;
        for(i=0;i<sl1-sl2;i++)
        {
                convs( str1, i, i+sl2, tmp1 );
                if( strncmp( tmp1, str2, sl2 ) == 0 )return 0;
        }
        return 1;
}

int stringlength( char *rec )
{
        int i;
        for(i=0;;i++)
        {
                if(rec[i]=='\n'||rec[i]=='\0')
                        break;
                else
                        i++;
        }
        return i;
}

void mytoken( char *rec, char delim, int field, char *mystring )
{
        int i,ntok,start,end,strlen;
	void convs( char *, int, int, char * );
        int stringlength( char * );

        strlen=stringlength(rec);
                                                                                                                                                      
        ntok=0;
        start=0;end=0;
        for(i=0;i<strlen;i++)
        {
                if( rec[i]==delim )
                {
                        ntok++;
                        end=i-1;
                        if(ntok==field){
                                convs(rec,start,end,mystring);
                                if( mystring[0] == '\0' || mystring[0] == ' ' )
                                {
                                        ntok=ntok-1;
                                        break;
                                }
                                else
                                {
        /* printf("field=%d ntok=%d (%s)\n", field, ntok, mystring); */
                                }
                        }
                        start=end+2;
                }
        }
}

void convs( char *in, int b, int e, char *tmp )
{
        int i,j;
        for (i=0,j=b; j<=e; j++) tmp[i++]= *(in+j);
        tmp[(e-b)+1] = '\0';
        return;
}

void setdate( MyTime *t, int yr, int jday, int hr, int mn, int sec )
{
        void complete( MyTime * );
        void initialize_mytime( MyTime * );

        initialize_mytime(t);
        t->year  = yr;
        t->jday = jday;
        t->hour  = hr;
        t->min   = mn;
        t->fsec  = sec;
        complete( t );
        return;
}
