/* cgfont.h -- Font related global/external variables */

#if defined(MAINDEF)
    unsigned int ptarray[128];	/* array of pointers to character positions 
					in fontarray */
    unsigned int fontarray[3380]; /* coded array of loaded font */
				  /* This handles upto 7000 byte XSH file */
    int cgoldfont=	999;	/* which  font is currently loaded */
				/* Force loading 1-st time */	
    int cgfsizex=     	1;   	/* screen fontsize, x-direction	*/
    				/* (weg) seems to be in points...e.g it's the overall fontsize) */
    int cgfsizey=     	1;  	/* screen fontsize, y-direction	*/
    float cgfscalex=   	1.;   	/* amount of scaling needed in x-dir */
    float cgfscaley=   	1.;   	/* amount of scaling needed in y-dir */
    float cgfontnum=	12.;   	/* fontsize (in laserwriter terms) */
#else
    extern unsigned int ptarray[];
    extern unsigned int fontarray[];
    extern int cgoldfont;     	
    extern int cgfsizex;    
    extern int cgfsizey;    
    extern float cgfscalex;  
    extern float cgfscaley;  
    extern float cgfontnum;     
#endif

