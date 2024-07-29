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

/**********************************************************************/
/***** Gene A. Ichinose (1999)                                  *******/
/***** 100% Y2K COMPLIENT                                       *******/
/***** header file for C time subroutines                       *******/
/**********************************************************************/
#ifndef TRUE
#define TRUE 0
#endif

#ifndef FALSE
#define FALSE 1
#endif

#define BEGIN	0
#define END	1
#define FIRST_MISSING_DAY     639787 /* 3 Sep 1752 */
#define NUMBER_MISSING_DAYS   11     /* 11 day correction */
#define SUNDAY     0
#define MONDAY     1
#define TUESDAY    2
#define WEDNESDAY  3
#define THURSDAY   4  /* for reformation */
#define FRIDAY     5
#define SATURDAY   6  /* 1 Jan 1 was a Saturday */
#define JANUARY    1
#define FEBURARY   2
#define MARCH      3
#define APRIL      4
#define MAY        5
#define JUNE       6
#define JULY       7
#define AUGUST     8
#define SEPTEMBER  9
#define OCTOBER   10
#define NOVEMBER  11
#define DECEMBER  12

/* leap year -- account for gregorian reformation in 1752 */
#define leap_year(yr) \
  ((yr) <= 1752 ? !((yr) % 4) : \
  (!((yr) % 4) && ((yr) % 100)) || !((yr) % 400))

/* number of centuries since 1700, not inclusive */
#define centuries_since_1700(yr) \
  ((yr) > 1700 ? (yr) / 100 - 17 : 0)

/* number of centuries since 1700 whose modulo of 400 is 0 */
#define quad_centuries_since_1700(yr) \
  ((yr) > 1600 ? ((yr) - 1600) / 400 : 0)

/* number of leap years between year 1 and this year, not inclusive */
#define leap_years_since_year_1(yr) \
  ((yr) / 4 - centuries_since_1700(yr) + quad_centuries_since_1700(yr))

typedef struct {
	int year;         /* 0-9999 */
	int month;        /* month of year  (01-12)        */
	char cmonth[4];   /* month JAN,FEB,MAR,APR,MAY.... */
	int jday;         /* day of year  (001-366)        */
	int mday;         /* day of month (01-31)          */
	int dayofweek;    /* day of week ( 0-06 )          */
	int weekofyear;   /* week of year 0-52             */
	int weekofmonth;  /* 0-5 or -1 + 1                 */
	/* 123456 123456 1234567 123456789 12345678 123456 12345678 */
	/* Sunday Monday Tuesday Wednesday Thursday Friday Saturday */
	char cday[10]; 
	int hour;       /* hour (00-23)            */
	char ampm[3];   /* am or pm                */
	int min;        /* minute (00-59)          */
	float fsec;     /* seconds (00.000-59.999) */
	int msec;       /* milliseconds (000-999)  */
	int isec;       /* seconds (00-59)         */
	double epoch;   /* seconds since 1/1/1960  */
	int offset;     /* hours from GMT          */
	int dst;        /* PST or PDT 0 +1 hours   */
	char tzone[8];  /* UTC PST MST CST EST     */  
} MyTime;

static MyTime mytime_null = {
	0, 0, { ' ',' ',' ' }, 0, 0, 0, 0, 0, 
	{ ' ',' ',' ',' ',' ',' ',' ',' ',' ' },
	0, { 'a','m' }, 0, 0., 0, 0, 0., 0, 0, { 'U','T','C' }
};

static int day_tab[2][13] = {
	{0,31,28,31,30,31,30,31,31,30,31,30,31},
	{0,31,29,31,30,31,30,31,31,30,31,30,31}
};

static char mname[][4] = { "0", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

static char FullMonthName[][12] = { "0", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

static char weekday[][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

/********* get time from system **********/
MyTime *mylocaltime( MyTime *t );
MyTime *myGMTtime( MyTime *t );

/********** get time from user **********/
MyTime *setmytime(  MyTime *t, int year, int month, int day, int hour, int min, int sec, int msec );
MyTime *setmytime2( MyTime *t, int year, int month, int day, int hour, int min, float sec, int offset );

/***********************************************  
  checks for empty or invalid 
  time fields and returns a (true/false) 
**********************************************/
int isMyTimeValid( MyTime *t );

/*************************************************** 
  complete - set fields 
  not set by user or system 
***************************************************/
void complete( MyTime *t );          
void fix_my_time( MyTime *t );       
void jday_to_month( MyTime *t );     
void month_to_jday( MyTime *t );     
MyTime *epoch2time( MyTime *t, double ); 
double time2epoch( MyTime *t );        
int day_of_month( int dayofweek, int weekoffset, int ref, int month, int year );
int setDST( MyTime *t );

/*** manipulate the time or reference other times ********/
int before( MyTime *, MyTime * );
int after( MyTime *, MyTime * );              
int IsTimeEq( MyTime *, MyTime * );
void clone_mytime( MyTime *t, MyTime *a );        
void mydifftime( MyTime *t1, MyTime *t2, MyTime *t3 );   
void myaddtime( MyTime *t1, MyTime *t2, MyTime *t3 );

/********** output or display time to user **********/
void WriteMyTime2STDOUT( MyTime * );
void WriteMyTime2STDERR( MyTime * );
char *MyTime2String( MyTime *t, char *str );
