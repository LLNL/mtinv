/* CGRAPH Library private header file */
/* Function prototypes in this files are private to the library.  Do not use them. */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE 	1
#define FALSE 	0
#define PAGEWIDTH 8.5	/* define page size in inches */
#define SCRNSCALE 120
#define PAGEHEIGHT 11.0   /* define page height in inches */
#define POINTSPINCH 72.   /* define number of points per inch */

#if defined(MAINDEF)
    int printit=1;
    int saveit=0;		/* flag that let's the user determine whether or not to save */
				/* 0 means no querying */
    int launchPreview = 1;	/* launch a PostScript preview application on output file */
    int useStandardOutput = 0;	/* send output to stdout if non-zero */
    int cgAxisEnable = 1;		/* In non-zero actually plot axis */
    int cgAxisNumberEnable = 1;	/* In non-zero actually plot numbers on axis */
			
    float realymax, realxmax;				
    char *filnam;
    char address[1024];
    float leastX=10.0, leastY=10.0;	
    				/* globals to help out cg_xlabel and cg_ylabel */
    char xString[100];
    				/*The way leastX, leastY, and xString work is as follows:
				  leastX holds the place where the yaxis numbers are right 
				  justified to (e.g x coor of lower right corner of 
				  the "numberbox"). xString contains the string put at 
				  the highest tick on the y-axis. We'll use stringwidth to 
				  make the ylabel fit. 
				  leastY holds the y position of the x-axis numbers.
				 */
    /* color or gray specifications */
    int flex_graycolor = 2;	/* flexible gray and color specification */
    float stroke_gray = 0.0;	/* gray of storke for markers */
    float fill_gray = 1.0;	/* gray of fill for markers */
    				/* they are set in cg_markergray */
    float stroke_r = 0.0, stroke_g = 0.0, stroke_b = 0.0;	/* color of stroke */	
    float fill_r = 1.0, fill_g = 1.0, fill_b = 1.0;	/* color of fill */
    int opaque_BG = 0;		/* put an opaque background if non zero */
    float bg_gray=1.0, bg_r=0.0, bg_g=0.0, bg_b=0.5;	/* color of opaque background */
    char stroke_graycolor[80];	/* use this to cache gray or color spec for stroke of markers */
    char fill_graycolor[80];	/* use this to cache gray or color spec for fill of markers */

    int cgSCREEN=  	1;	/* Chooses between screen and printer	*/
    int cgMARKAX=  	1;	/* 0=inches, else= axis coordinates  */
    int cgTEXTAX=  	0;	/*     for markers and the text cg     */
    float cgxmi,cgymi;	   	/* Minimum graph values		     */
    float cgxorig,cgyorig;  	/* Position (in pixels) of origin    */
    float cgxscale,cgyscale;	/* Graph size/graph range	     */
    int cgispolar=	0;     	/* Is graph polar? (boolean)  	     */
    int cglogx,cglogy;	   	/* Is axis logarithmic? (boolean)    */
    int cgxfont, cgyfont;       /* maintain font size regardless of scaling*/
    float cglastmovex= 0.;	/* beginning of the current path */
    float cglastmovey= 0.;	/* beginning of the current path */
    float cgSCALE =     1.0;     /* scaling factor for cg_init */
    int cgxcp=		0;     	/* currentpoint			  */
    int cgycp=		0;     					 
    FILE *cgstream; 		/* Used to write to the laserwriter */
    float xscaling = 1.0;       /* used for scaling factor in cfont */
    float yscaling = 1.0;	/* make scaling factor independent 
				   			 * of imagraph's pixels/inch.
*/	
	
	
	  int lw_dash(int type,float mul);
	  int lw_init(int rot);
	  void lw_laser_open(void );
	  void lw_laser_close(void );
	  void lw_message(char *msgstring);
	  void append_int(int process);
	  int lw_show(void );
	  int findsigdec(float min,float range,int numsep,float ticks);
	  void formaxnum(float value,int sigdec,char *line);
	  float wc_ybottom(int multiplier);
	  float wc_xbottom(int multiplier);
	  int lw_font(float fsize,int type);
	  int lw_gray(float degree);
	  char *wc_linecomp(char *line,int Ept);
	  int lw_marker(float x,float y,int type,float size);
	  int loadmark(int marknum);
	  int loadmark2(int marknum);
	  int loadmark3(int marknum);
	  int lw_pointer(float x,float y,int type,float size);
	  int lw_mesh(void );
	  int lwxmesh(void );
	  int lwymesh(void );
	  int lwlxmesh(void );
	  int lwxmset(float min);
	  int lwxminmset(float min);
	  int lwlymesh(void );
	  int lwymset(float min);
	  int lwyminmset(float min);
	  int lw_move(float xpos,float ypos);
	  int lw_line(float xpos,float ypos);
	  int lw_rorigin(float xpos,float ypos);
	  int lw_aorigin(float xpos,float ypos);
	  int lw_fill(void );
	  int lw_closepath(void );
	  int lw_rmove(float xpos,float ypos);
	  int lw_rline(float xpos,float ypos);
	  int lw_reset(void );
	  int lw_stroke(void );
	  char *fix_string(char *text,int fix_minus);
	  int lw_text(float xpos,float ypos,float dir,char *text);
	  float wc_trunc(float fvalue,int updown);
	  int lw_linewidth(float linewidth);
	  int lw_xaxis(float xsiz,float xmin,float xmax,float os,float xtix,int numsep);
	  int lw_xlog(float xsiz,float xmin,float xmax,float os);
	  int lwxlinit(float os,int tt,int tb,int numdist,int tt1,int tb1);
	  int lwxset(float min,float top,float bot);
	  int lwallxtix(float top,float bot);
	  int lwlxnum(float start,int numdist,float os,int tt,int tb);
	  int lwlxmin(float start,int numdist,float os,int tt,int tb);
	  int lw_yaxis(float ysiz,float ymin,float ymax,float os,float ytix,int numsep);
	  int lw_ylog(float ysiz,float ymin,float ymax,float os);
	  int lwylinit(float os,int tt,int tb,int numdist,int tt1,int tb1);
	  int lwyset(float min,float right,float left);
	  int lwysetmin(float min,float right,float left);
	  int lwallytix(float left,float right);
	  int lwlynum(float start,int numdist,float os,int tr,int tl);
	  int lwlymin(float start,int numdist,float os,int tr,int tl);

#else
	extern  int lw_dash(int type,float mul);
	extern  int lw_init(int rot);
	extern  void lw_laser_open(void );
	extern  void lw_laser_close(void );
	extern  void lw_message(char *msgstring);
	extern  void append_int(int process);
	extern  int lw_show(void );
	extern  int findsigdec(float min,float range,int numsep,float ticks);
	extern  void formaxnum(float value,int sigdec,char *line);
	extern  float wc_ybottom(int multiplier);
	extern  float wc_xbottom(int multiplier);
	extern  int lw_font(float fsize,int type);
	extern  int lw_gray(float degree);
	extern  char *wc_linecomp(char *line,int Ept);
	extern  int lw_marker(float x,float y,int type,float size);
	extern  int loadmark(int marknum);
	extern  int loadmark2(int marknum);
	extern  int loadmark3(int marknum);
	extern  int lw_pointer(float x,float y,int type,float size);
	extern  int lw_mesh(void );
	extern  int lwxmesh(void );
	extern  int lwymesh(void );
	extern  int lwlxmesh(void );
	extern  int lwxmset(float min);
	extern  int lwxminmset(float min);
	extern  int lwlymesh(void );
	extern  int lwymset(float min);
	extern  int lwyminmset(float min);
	extern  int lw_move(float xpos,float ypos);
	extern  int lw_line(float xpos,float ypos);
	extern  int lw_rorigin(float xpos,float ypos);
	extern  int lw_aorigin(float xpos,float ypos);
	extern  int lw_fill(void );
	extern  int lw_closepath(void );
	extern  int lw_rmove(float xpos,float ypos);
	extern  int lw_rline(float xpos,float ypos);
	extern  int lw_reset(void );
	extern  int lw_stroke(void );
	extern  char *fix_string(char *text,int fix_minus);
	extern  int lw_text(float xpos,float ypos,float dir,char *text);
	extern  float wc_trunc(float fvalue,int updown);
	extern  int lw_linewidth(float linewidth);
	extern  int lw_xaxis(float xsiz,float xmin,float xmax,float os,float xtix,int numsep);
	extern  int lw_xlog(float xsiz,float xmin,float xmax,float os);
	extern  int lwxlinit(float os,int tt,int tb,int numdist,int tt1,int tb1);
	extern  int lwxset(float min,float top,float bot);
	extern  int lwallxtix(float top,float bot);
	extern  int lwlxnum(float start,int numdist,float os,int tt,int tb);
	extern  int lwlxmin(float start,int numdist,float os,int tt,int tb);
	extern  int lw_yaxis(float ysiz,float ymin,float ymax,float os,float ytix,int numsep);
	extern  int lw_ylog(float ysiz,float ymin,float ymax,float os);
	extern  int lwylinit(float os,int tt,int tb,int numdist,int tt1,int tb1);
	extern  int lwyset(float min,float right,float left);
	extern  int lwysetmin(float min,float right,float left);
	extern  int lwallytix(float left,float right);
	extern  int lwlynum(float start,int numdist,float os,int tr,int tl);
	extern  int lwlymin(float start,int numdist,float os,int tr,int tl);


/* variables */

    extern int launchPreview;	/* launch a PostScript preview application on output file */
    /* color or gray specifications */
    extern int flex_graycolor;	/* flexible gray and color specification */
    extern int useStandardOutput;	/* send output to stdout if non-zero */
    extern float stroke_gray;	/* gray of storke */
    extern float fill_gray;	/* gray of fill */
    				/* they are set in cg_markergray */
    extern float stroke_r, stroke_g, stroke_b;	/* color of stroke */	
    extern float fill_r, fill_g, fill_b;	/* color of fill */
    extern int opaque_BG;		/* put an opaque background if non zero */
    extern float bg_gray, bg_r, bg_g, bg_b;	/* color of opaque background */
    extern char stroke_graycolor[];	/* use this to cache gray or color spec for stroke */
    extern char fill_graycolor[];	/* use this to cache gray or color spec for fill */

    extern float leastX, leastY;	
    extern char xString[100];
    extern int printit;
    extern int saveit;
    extern int cgAxisEnable;		/* In non-zero actually plot axis */
    extern int cgAxisNumberEnable;	/* In non-zero actually plot numbers on axis */
    extern char *filnam;
    extern float realymax, realxmax;				
    extern char address[1024];
    extern int cgSCREEN; 	       
    extern int cgMARKAX;
    extern int cgTEXTAX;
    extern float cgxorig,cgyorig;  
    extern float cgSCALE;
    extern float cgxscale,cgyscale;
    extern float cgxmi,cgymi;     
    extern int cgispolar;	       
    extern int cglogx,cglogy;      
    extern float cglastmovex;
    extern int cgxfont;
    extern int cgyfont;
    extern float cglastmovey;

    extern int cgxcp;	       
    extern int cgycp;	       					 
    extern FILE *cgstream;       
    extern float xscaling;
    extern float yscaling;
#endif


