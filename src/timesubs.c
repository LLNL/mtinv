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

/*********************************************************************************/
/***** Gene A. Ichinose (1999)                                             *******/
/***** 100% Y2K COMPLIENT                                                  *******/
/***** ANSI C time subroutines -                                           *******/
/*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <strings.h>
#include "../include/sac.h"
#include "../include/mytime.h"

/*********************************************************************************
       returns a time structure with a complete local time
**********************************************************************************/
MyTime *mylocaltime( MyTime *t )
{
	struct tm *my_local_time;
	time_t local_time;
	char now[128];
	struct tm *localtime( const time_t *clock );
	void complete( MyTime *t );
	extern char *tzname[2];
	extern time_t timezone, altzone;

/* get the local time */
        /* my_local_time = (struct tm *)calloc(1,sizeof(struct tm)); */
        time(&local_time);
        my_local_time = localtime( &local_time );

/* swap of time values */
	t->year  = my_local_time->tm_year;
        t->month = my_local_time->tm_mon+1;/* unix month starts at 0 */
        t->mday  = my_local_time->tm_mday;
        t->hour  = my_local_time->tm_hour;
        t->min   = my_local_time->tm_min;
        t->isec  = my_local_time->tm_sec;
	t->fsec  = (float)my_local_time->tm_sec;
	t->dst   = my_local_time->tm_isdst;

/** set the DST flag from tm_isdst +1/0/-1 **/ 
	/* if(my_local_time->tm_isdst >= 0)t->dst=my_local_time->tm_isdst; */

	t->offset  = timezone/3600;
	strcpy( t->tzone, tzname[1] );

/* complete the rest of the fields */
	complete( t );
	/* free( my_local_time ); */
	return t;
}

/*********************************************************************************
       returns a time structure with complete UTC time
**********************************************************************************/
MyTime *myGMTtime( MyTime *t )
{
        struct tm *my_gmt_time;
        time_t local_time;
        struct tm *gmtime( const time_t *clock );

	void complete( MyTime *t );

/* get the local time */
        /* my_gmt_time = (struct tm *)calloc(1,sizeof(struct tm));  */
        time(&local_time);
        my_gmt_time = gmtime(&local_time);

/* swap of time values */
        t->year  = my_gmt_time->tm_year;
        t->month = my_gmt_time->tm_mon+1;/* unix month starts at 0 */
        t->mday  = my_gmt_time->tm_mday;
        t->hour  = my_gmt_time->tm_hour;
        t->min   = my_gmt_time->tm_min;
        t->isec  = my_gmt_time->tm_sec;
	t->fsec  = (float)my_gmt_time->tm_sec;
	t->dst   = my_gmt_time->tm_isdst;
	t->offset = 0;
	strcpy( t->tzone, "GMT" );

/* complete the rest of the fields */
        complete( t );

	/* free( my_gmt_time ); */
	return t;
}

/*********************************************************************************
Returns TRUE(0) or FALSE(1) if in DST and updates time structure fields.
    never call this from setmytime() subroutine 
**********************************************************************************
first Sunday in Apr. 2 AM in standard time. DST=+1; PST=+8 PDT=+7
last  Sunday in Oct. 2 AM in standard time. DST=+0; PST=+8
**********************************************************************************/
int setDST( MyTime *t )
{
	MyTime *start, *stop;
	int sday, eday;
	MyTime *setmytime2( MyTime *t, int year, int month, int day, int hour, int min, float sec, int offset );
	int day_of_month( int dayofweek, int weekoffset, int ref, int month, int year );
	void complete( MyTime *t );
	int after( MyTime *, MyTime * );
	int before( MyTime *, MyTime * );

	start = (MyTime *)calloc(1,sizeof(MyTime));
	stop  = (MyTime *)calloc(1,sizeof(MyTime));
	sday = day_of_month( SUNDAY, 1, BEGIN,   APRIL, t->year ); 
	eday = day_of_month( SUNDAY, 1,   END, OCTOBER, t->year );
	start = setmytime2( start, t->year,  4, sday, 2, 0, 0, 0. ); 
	stop  = setmytime2( stop,  t->year, 10, eday, 2, 0, 0, 0. );
	if( after( t, start ) && before( t, stop ) )
		t->dst = 1;
	else
		t->dst = 0;
	complete(t);
	if( t->dst == 1 ) return TRUE;
	if( t->dst == 0 ) return FALSE;
	return (int)t->dst;
}

int day_of_month( int dayofweek, int weekoffset, int ref, int month, int year )
{
	int leap, DaysSinceYr0, day, weekofmonth=1;
	int TotalNumDays;
	int TotalNumWeeks;
	int ThisDayOfMonth[32];
	int ThisDayOfWeek[32], ThisWeekOfMonth[32];
	int julday( int, int, int );

        leap=((year)<=1752?!((year)%4):(!((year)%4)&&((year)%100))||!((year)%400));
	TotalNumDays = day_tab[leap][month];
	for( day = 1; day <= TotalNumDays; day++) {
		ThisDayOfMonth[day] = day;
        	DaysSinceYr0 = (year-1)*365 +
		leap_years_since_year_1(year-1) + julday(year,month,day);
		ThisDayOfWeek[day] = 
		(((DaysSinceYr0 - 1 + SATURDAY) - NUMBER_MISSING_DAYS) % 7);
		if( ThisDayOfWeek[day] == 0 ) weekofmonth++;
		ThisWeekOfMonth[day] = weekofmonth;
	}

	TotalNumWeeks = ThisWeekOfMonth[TotalNumDays];

	if( ref == END )
	{
	  for( day = TotalNumDays; day >= (TotalNumDays-weekoffset*7); day-- )
	  {
		if ( dayofweek == ThisDayOfWeek[day] )
		{
			return ThisDayOfMonth[day];
		}
	  }
	}

	if( ref == BEGIN )
	{
		for( day = 1 ; day <= weekoffset*7; day++ )
		{
			if ( dayofweek == ThisDayOfWeek[day] )
			{
				return ThisDayOfMonth[day];
			}
		}
	}
	return ThisDayOfMonth[day];
}

/****************************************************************
	function just returns julian day
****************************************************************/
int julday(int yr, int mo, int da)
{
	int i, jd=0, leap;
	leap = ((yr)<=1752 ? !((yr)%4):(!((yr)%4)&&((yr)%100))||!((yr)%400));
	for(i=1; i < mo; i++) jd += day_tab[leap][i];
	return jd+da;
}

/*************************************************************************/
/** sets time structure given inputs *************************************/
/** use this for setDST()            *************************************/
/*************************************************************************/
MyTime *setmytime2(MyTime *t, int year, int month, int day, int hour, int min, float sec, int offset )
{
        void complete( MyTime *t );
        void initialize_mytime( MyTime *t );

        initialize_mytime(t);
        t->year  = year;
        t->month = month;
        t->mday  = day;
        t->hour  = hour;
        t->min   = min;
        t->fsec  = sec;
	t->offset = offset;
        complete( t );
        return t;
}

/****************************************************************
	 constructs a mytime structure given date and time
	     do not call setDST() from here
****************************************************************/
MyTime *setmytime(MyTime *t, int year, int month, int day, int hour, int min, int sec, int msec )
{
	void complete( MyTime *t );
	void fix_my_time( MyTime *t );
	int isMyTimeValid( MyTime *t );
	void initialize_mytime( MyTime *t );

	initialize_mytime(t);
	t->year  = year;
	t->month = month;
	t->mday  = day;
	t->hour  = hour;
	t->min   = min;
	t->isec  = sec;
	t->msec  = msec;
	if( isMyTimeValid(t) == FALSE ) {
		fprintf(stderr, "ERROR: invalid date given\n");
		exit(-1);
	}
	complete( t );
	fix_my_time( t );
	return t;
}

/****************************************************************
	 check time struct for valid date, time always valid
 ****************************************************************/
int isMyTimeValid( MyTime *t )
{
	int leap, yr, Valid=TRUE;
	yr=t->year;
	leap=((yr)<= 1752?!((yr)%4):(!((yr)%4)&&((yr)%100))|| !((yr)%400));
	if( t->year < 0 || t->year > 9999 )Valid=FALSE;
	if( t->jday > 366 && leap == 1 )Valid=FALSE;
	if( t->jday > 365 && leap == 0 )Valid=FALSE;
	if( t->month > 12 || t->month <= 0 )Valid=FALSE;
	if( t->mday > day_tab[leap][t->month] )Valid=FALSE;
	return Valid;
}

/****************************************************************
     initalizes the time structure 
****************************************************************/
void initialize_mytime( MyTime *t )
{
        t->year  = 0;
        t->month = 0;
        strcpy(t->cmonth, "\0");
        t->jday = 0;
        t->mday = 0;
        t->dayofweek=0;
        t->weekofyear=0;
        t->weekofmonth=0;
        strcpy(t->cday, "\0");
        t->hour=0;
        strcpy(t->ampm, "\0"); 
        t->min=0;
        t->fsec=0.;
        t->msec=0;
        t->isec=0;
        t->epoch=0;
        t->offset=0;
        t->dst=0;
        strcpy(t->tzone, "UTC\0");
        return;
}

/****************************************************************
    non intellegent way to complete missing time fields
    must give offset, year, month, (day or jday), hr,mn,sec,msec
 ****************************************************************/
void complete( MyTime *t )
{
	void jday_to_month( MyTime *t );
	void month_to_jday( MyTime *t );
	double time2epoch( MyTime *t );
	void fix_my_time( MyTime *t );
	void setDayOfWeek( MyTime *t );

/* fix Y2K problems */
	if( t->year < 1900 ) t->year = t->year + 1900;

/* if you know the month# but not jday */
/* else you know the jullian day and not month# */
	if( t->jday == 0 ) 
	{
		month_to_jday(t);
                jday_to_month(t);
        } 
	else
	{
                jday_to_month(t);
                month_to_jday(t);
        }

/* set day of week */
	setDayOfWeek( t );
/* set ampm flag */
	if( t->hour >= 0  && t->hour < 12 ) strcpy(t->ampm,"am");
	if( t->hour >= 12 && t->hour <= 23 ) strcpy(t->ampm,"pm");

/* set msec and isec */
	if( t->msec == 0 && t->isec == 0 ) {
		t->isec = (int)floor(t->fsec);
		t->msec = (int)floor( 1000*(t->fsec - t->isec) );
	}
/** set fsec **/
	if ( t->fsec == 0 )
		t->fsec = (float)t->isec + ( 0.001 * (float)t->msec );

/* set time zone given offset */
	if( t->offset == 0 ) strcpy(t->tzone,"UTC");
	if( t->offset == 1 && t->dst == 0 ) strcpy(t->tzone,"GMT+1");
	if( t->offset == 2 && t->dst == 0 ) strcpy(t->tzone,"GMT+2");
	if( t->offset == 3 && t->dst == 0 ) strcpy(t->tzone,"GMT+3");
	if( t->offset == 4 && t->dst == 0 ) strcpy(t->tzone,"GMT+4");
	if( t->offset == 5 && t->dst == 0 ) strcpy(t->tzone,"EST");
	if( t->offset == 5 && t->dst == 1 ) strcpy(t->tzone,"EDT");
	if( t->offset == 6 && t->dst == 0 ) strcpy(t->tzone,"CST");
	if( t->offset == 6 && t->dst == 1 ) strcpy(t->tzone,"CDT");
	if( t->offset == 7 && t->dst == 0 ) strcpy(t->tzone,"MST");
	if( t->offset == 7 && t->dst == 1 ) strcpy(t->tzone,"MDT");
        if( t->offset == 8 && t->dst == 0 ) strcpy(t->tzone,"PST");
        if( t->offset == 8 && t->dst == 1 ) strcpy(t->tzone,"PDT");
/* compute the epoch time */
	t->epoch = time2epoch( t );
	return;
}

/*************************************************************************************
return day of week for any date from 1 Jan. 1 to 31 Dec. 9999.
Assumes the Gregorian reformation eliminates
3 Sep. 1752 through 13 Sep. 1752.  Returns Thursday for all missing days.
**************************************************************************************/
void setDayOfWeek( MyTime *t )
{
	int DaysSinceYr0; /* number of days since year 0 */
	DaysSinceYr0=(t->year-1)*365+leap_years_since_year_1(t->year-1)+t->jday;
	if ( DaysSinceYr0 < FIRST_MISSING_DAY) {
	  t->dayofweek = ((DaysSinceYr0 - 1 + SATURDAY) % 7);
	} else if (DaysSinceYr0 >= (FIRST_MISSING_DAY + NUMBER_MISSING_DAYS)) {
	  t->dayofweek = (((DaysSinceYr0 - 1 + SATURDAY) - NUMBER_MISSING_DAYS) % 7);
	} else {
	  t->dayofweek = THURSDAY;
	}
	strcpy(t->cday, weekday[t->dayofweek]);
        return;
}

/****************************************************************
         fixes fields which are too long
 ****************************************************************/
void fix_my_time( MyTime *t )
{
	int leap = 0;
	leap = (((t->year%4 == 0) && (t->year%100 != 0)) || (t->year%400 == 0) ) ;

	while( t->fsec < 0 && t->isec < 0 )
	{
		t->fsec += 60.;
		t->isec += 60;
		t->min--;
	}

	while( t->fsec >= 60 && t->isec >= 60 ) 
	{
		t->fsec -= 60.;
		t->isec -= 60;
		t->min++;	
	}
/***
	while( t->msec >= 1000 )
	{
                t->msec -= 1000;
                t->isec++;
        }
***/
	if( t->msec >= 1000 ) 
	{
		fprintf(stderr, "Error in fix_my_time msec>1000\n");
		exit(-1);
	}
	if( t->min < 0 )
	{
		fprintf(stderr, "Error in fix_my_time min < 0\n" );
		exit(-1);
	}
        while( t->min >= 60 )
	{
                t->min -= 60;
                t->hour++;
        }

        while( t->hour > 23 ) {
                t->hour -= 24;
                t->jday++;
        }
	
        while( t->jday > 366 && leap == 1 ) {
                t->jday -= 366;
                t->year++;
        }
        while( t->jday > 365 && leap == 0 ) {
                t->jday -= 365;
                t->year++;
        }
	t->mday = 0;
	complete( t );
        return;
}

/****************************************************************************
       converts jullian day to month 
*****************************************************************************/
void jday_to_month( MyTime *t )
{
	int year=0, jday=0, i, leap=0;
	t->month = 0; 
	t->mday = 0;
	jday = t->jday;
        year = t->year;
        leap = (((year%4 == 0) && (year%100 != 0)) || (year%400 == 0) );
        for(i=1; jday > day_tab[leap][i];i++)
	{
		jday -= day_tab[leap][i];
	}
        t->month = i;
	strcpy( t->cmonth, mname[i] );
        t->mday = jday;
        return;
}

/****************************************************************************
       converts month to jullian day
*****************************************************************************/
void month_to_jday( MyTime *t )
{
	int i, leap;
        t->jday = 0;
        leap=(((t->year%4 == 0)&&(t->year%100 != 0))||(t->year%400 == 0)) ;

        for(i=1; i < t->month; i++)
	{
		t->jday += day_tab[leap][i];
	}
        t->jday += t->mday;
	return;
}

/*** converts epoch to character string ********************************/
/****************************************  0         1         2  ******/
/****************************************  0123456789012345678901 ******/
/****************************************  YYYY/MM/DD HH:MM:SS.SS ******/
/***********************************************************************/

char *Epoch_To_String_Time( double epoch, char *str )
{
	MyTime *t;
	void initialize_mytime( MyTime * );
	MyTime *epoch2time( MyTime *, double );

	t=(MyTime *)malloc(sizeof(MyTime));
	initialize_mytime( t );
	t = epoch2time( t, epoch );
	sprintf( str, "%04d/%02d/%02d %02d:%02d:%05.2f",
		t->year,
		t->month,
		t->mday,
		t->hour,
		t->min,
		t->fsec
	);
	free(t);
	return str;
}

/***************************************************************************
       converts epoch number to my time struct
****************************************************************************/
MyTime *epoch2time( MyTime *t, double epoch )
{
	int i=1970, leap = 0;
	double seconds;
	void complete( MyTime *t );
	void fix_my_time( MyTime *t );

	seconds = epoch;
	t->year  = 1970;
	t->month = 0;
	t->jday  = 1;
	t->mday  = 0;
	t->hour  = 0;
	t->min   = 0;
	t->isec  = 0;
	t->fsec  = 0;
	t->msec  = 0;
	t->offset = 0;

	while( seconds > (366*24*60*60) ) {
                leap = (((i%4 == 0) && (i%100 != 0)) || (i%400 == 0)) ;
                if( leap ) {
                        seconds -= (366*24*60*60);
			t->year++;
		}
                else {
                        seconds -= (365*24*60*60);
			t->year++;
		}
                i++;
        }

	while( seconds > (24*60*60) ) {
		seconds -= (24*60*60);
		t->jday++;
	}
	while( seconds > (60*60) ) {
		seconds  -= (60*60);
		t->hour++;
	}
	while( seconds > 60 ) {
		seconds  -= 60;
		t->min++;
	}
	t->fsec = seconds;

	complete( t );
	fix_my_time( t );
        return t;
}

/*****************************************************************************
       returns epoch number from a my time struct
      Currently converts to whatever time zone your in but 
      all epochs should only be referenced to UTC
******************************************************************************/
double time2epoch( MyTime *t )
{
	int i, leap=0;
	double epoch=0.;
/*
	if( t->offset != 0 ) 
		fprintf(stderr, "Warning: Epoch not referenced to UTC\n");
*/
	for( i=1970; i<t->year; i++) 
	{
		leap = (((i%4 == 0) && (i%100 != 0)) || (i%400 == 0)) ;
		if( leap ) 
			epoch += (366. * 24. * 60. * 60.);
		else
			epoch += (365. * 24. * 60. * 60.);
	}
	leap=(((t->year%4 == 0)&&(t->year%100 != 0))||(t->year%400 == 0));
	for(i=1; i < t->month; i++)
		epoch += (double)( day_tab[leap][i] * 24 * 60 * 60 );

	epoch += ( (t->mday-1) * 24 * 60 * 60 );
	epoch += ( t->hour * 60 * 60 );
	epoch += ( t->min  * 60 );
	epoch += t->fsec;
        return epoch;
}

/***************************************************************************
       IS time1 before time2?
****************************************************************************/
int before( MyTime *t1, MyTime *t2 )
{
	if( (t1->epoch - t1->offset) < (t2->epoch - t2->offset) ) 
		return TRUE;
	else
		return FALSE;
}

/****************************************************************************
       is time1 after time2?
*****************************************************************************/
int after( MyTime *t1, MyTime *t2 )
{
	if( (t1->epoch - t1->offset) > (t2->epoch - t2->offset) )
		return TRUE;
	else
        	return FALSE;
}

/***************************************************************************
       did time1 occur at the same time as time2?
****************************************************************************/
int IsTimeEq( MyTime *t1, MyTime *t2 )
{
	if( (t1->epoch - t1->offset) == (t2->epoch - t2->offset) )
		return TRUE;
	else
		 return FALSE;
}

/***************************************************************************
       clone_mytime() time t to time a
****************************************************************************/

void clone_mytime( MyTime *t, MyTime *a )
{
	/* fprintf( stderr, "timesubs.c: starting clone_mytime()\n" ); */

	a->year        = t->year;
	a->month       = t->month;
	a->jday        = t->jday;
	a->mday        = t->mday;
	a->dayofweek   = t->dayofweek;
	a->weekofyear  = t->weekofyear;
	a->weekofmonth = t->weekofmonth;
	a->hour        = t->hour;
	a->min         = t->min;
	a->fsec        = t->fsec;
	a->msec        = t->msec;
	a->isec        = t->isec;
	a->epoch       = t->epoch;
	a->offset      = t->offset;
	a->dst         = t->dst;

	/* strncpy( a->tzone,  t->tzone, 3 ); */
	sprintf( a->tzone, "%s", t->tzone );
	/* fprintf( stderr, "a->tzone=%s t->tzone=%s\n", a->tzone, t->tzone ); fflush(stderr); */

	/* strncpy( a->cday,   t->cday, 9 ); */
	sprintf( a->cday, "%s", t->cday );
	/* fprintf( stderr, "a->cday=%s t->cday=%s\n", a->cday, t->cday ); fflush(stderr); */

	/* strncpy( a->ampm,   t->ampm, 2 ); */
	sprintf( a->ampm, "%s", t->ampm );
	/* fprintf( stderr, "a->ampm=%s t->ampm=%s\n", a->ampm, t->ampm ); fflush(stderr); */

	/* strncpy( a->cmonth, t->cmonth, 3 ); */
	sprintf( a->cmonth, "%s", t->cmonth );
	/* fprintf( stderr, "a->cmonth=%s t->cmonth=%s\n", a->cmonth, t->cmonth ); fflush(stderr); */

	/*
	fprintf( stderr, "timesubs.c: leaving clone_mytime()\n" );
	fflush(stderr);
	*/
}

/****************************************************************************
 *     subtract time1 from time2 = time3                                    *
 ****************************************************************************/

void mydifftime( MyTime *t1, MyTime *t2, MyTime *t3 )
{
	MyTime *epoch2time( MyTime *, double );
	void fix_my_time( MyTime * );

	t3->epoch = t1->epoch - t2->epoch;
	epoch2time( t3, t3->epoch );
	fix_my_time( t3 );
        return;
}

/***************************************************************************
       add time1 to time2 = time 3
*****************************************************************************/
void myaddtime( MyTime *t1, MyTime *t2, MyTime *t3 )
{
	MyTime *epoch2time( MyTime *, double );
	void fix_my_time( MyTime * );

	t3->epoch = t1->epoch + t2->epoch;
	epoch2time( t3, t3->epoch );
	fix_my_time( t3 );
	return;
}

void WriteMyTime2STDOUT( MyTime *t )
{
        fprintf(stdout,
	"%-9s %-9s %2d, %4d (%03d) %s %4d-%02d-%02dT%02d:%02d:%07.4f (%03d) %2s %s+%d [%16.3lf]\n",
                t->cday,
                FullMonthName[t->month],
                t->mday,
                t->year,
                t->jday,
                t->cmonth,
                t->year,
                t->month,
                t->mday,
                t->hour,
                t->min,
                t->fsec,
                t->msec,
                t->ampm,
                t->tzone,
                t->offset,
                t->epoch );
        return;
}

void WriteMyTime2STDERR( MyTime *t )
{
	fprintf(stderr, 
	"%-9s %-9s %2d, %4d (%03d) %s %4d-%02d-%02dT%02d:%02d:%07.4f (%03d) %2s %s+%d [%16.3lf]\n",
                t->cday,
                FullMonthName[t->month],
                t->mday,
                t->year,
                t->jday,
                t->cmonth,
                t->year,
                t->month,
                t->mday,
                t->hour,
                t->min,
                t->fsec,
                t->msec,
                t->ampm,
                t->tzone,
                t->offset,
                t->epoch );
        return;
}

/********************************************************
         write a string fmt of time structure 
********************************************************/
char *MyTime2String( MyTime *t, char *str )
{
	sprintf(str, "%4d %-3.3s %02d(%03d) %02d:%02d:%06.3f %-3.3s",
		t->year,
		t->cmonth,
		t->mday,
		t->jday,
		t->hour,
		t->min,
		t->fsec,
		t->tzone
	);
        return str;
}

char *MyTime2ShortString( MyTime *t, char *str )
{
	sprintf( str, "%s-%02d,%02d:%02d:%02.0f-%s",
		t->cmonth,
		t->mday,
		t->hour,
		t->min,
		t->fsec,
		t->tzone );
	return str;
}

void parsestring( MyTime *t, char *str )
{
        void complete( MyTime *t );
        void fix_my_time( MyTime *t );
        int isMyTimeValid( MyTime *t );
        void initialize_mytime( MyTime *t );
        char dummy;
        initialize_mytime(t);

        sscanf(str, "%4d%c%2d%c%2d%c%2d%c%2d%c%5f",
                &t->year, &dummy,
                &t->month, &dummy,
                &t->mday, &dummy,
                &t->hour, &dummy,
                &t->min, &dummy,
                &t->fsec );

/*** assumes UTC ***/
	t->offset = 0;
	strcpy( t->tzone, "UTC" );
        complete( t );
        return;
}

MyTime *sac2mytime( MyTime *t, Sac_Header *sp )
{
	void complete( MyTime *t );
	void initialize_mytime( MyTime *t );
	
	initialize_mytime(t);
	t->year = sp->nzyear;
	t->jday = sp->nzjday;
	t->hour = sp->nzhour;
	t->min  = sp->nzmin;
	t->isec = sp->nzsec;
	t->msec = sp->nzmsec;
	t->fsec = (float)sp->nzsec + ((float)sp->nzmsec/1000);
	t->offset = 0;
	strcpy( t->tzone, "UTC" );
	complete( t );
	return t;
}

Sac_Header *mytime2sac( MyTime *t, Sac_Header *sp )
{
	sp->nzyear = t->year;
	sp->nzjday = t->jday;
	sp->nzhour = t->hour;
	sp->nzmin  = t->min;
	sp->nzsec  = t->isec;
	sp->nzmsec = t->msec;
	return sp;
}

double calcTimeAgo( MyTime *now, MyTime *evtime, char *return_type )
{
	double ago;

	if( strcmp( return_type, "WKS" ) == 0 )
	{
		ago = (now->epoch - evtime->epoch)/604800;
	}
	else if( strcmp( return_type, "DAYS" ) == 0 )
	{
		ago = (now->epoch - evtime->epoch)/86400;
	}
	else if( strcmp( return_type, "HRS" ) == 0 )
	{
		ago = (now->epoch - evtime->epoch)/3600;
	}	
	else if( strcmp( return_type, "MINS" ) == 0 )
	{
		ago = (now->epoch - evtime->epoch)/60;
	}
	else if( strcmp( return_type, "SECS" ) == 0 )
	{
		ago = (now->epoch - evtime->epoch);
	}
	else
	{
		/*** unknown type returns seconds ***/
		ago = (now->epoch - evtime->epoch);
	}
	return ago;
}
