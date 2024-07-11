/* cgaxes.h -- Global/external variable used in axis routines */

#include <string.h>

#define MINBIT 1
#define MAXBIT 2048 

#if defined(MAINDEF)
    float cgxma,cgyma;	   	/* maximum logarithmic graph values	*/
    float cgxticks,cgyticks;	/* tick spacing for linear graphs	*/
    double cgldecy,cgldecx; 	/* the upper end of the lowest decade of */
				/*   a log graph */
    double cgudecy,cgudecx; 	/* the lower end of the highest decade */
				/*  of a log graph */
    int cgxtix,cgytix;	   	/* used for logarithmic tick marks	*/
    int cgprecision=	4;  	/* max # digits on screen graphs	*/
    int cglogticlen=	5;	/* length of log tick marks in points */
    int cglinticlen=	5;	/* length of lin axis tick marks in points */
    int cglinnumdist=	2;	/* gap between end of tick to numbers */
				/*   on linear axis */
    int cglogticlen10 = 8;	/* length of log tick marks at 10's */
    int cgnumformat =	0;	/* numbering format flag for log axis */
    int cglintnposition=2;	/* tick position selector for linear axes */
    int cglognumdist=	2;	/* gep between end of tick to numbers */
				/*  on log axis */
    int cglogtnposition=2;	/* tick position selector for log axis */
    int cglognumsel=	1;	/* numbering selector for log axis */
    int cglogticsel=	0;	/* tick selector mask for log axis */
    int cglinnumoff=	0;	/*  */
#else
    extern float cgxma,cgyma;     
    extern float cgxticks,cgyticks;
    extern double cgldecy,cgldecx; 
    extern double cgudecy,cgudecx; 
    extern int cgxtix,cgytix;      
    extern int cgprecision;
    extern int cglogticlen;
    extern int cglinticlen;
    extern int cglogticlen10;
    extern int cgnumformat;
    extern int cglintnmask;
    extern int cglogtnmask;
    extern int cglogspmask;
    extern int cglinnumdist;
    extern int cglintnposition;
    extern int cglognumdist;
    extern int cglogtnposition;
    extern int cglognumsel;
    extern int cglogticsel;
    extern int cglinnumoff;
#endif
