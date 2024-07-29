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

/*** this gets called by mtinv.c to plot the moment tensor - focal mechanims ***/
/*** non-GMT versions, uses ../misc/Cgraph/ ***/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../include/cgraph.h"
#include "../include/mt.h"

/*********************************************************/
/*** default line types                                ***/
/*********************************************************/
#define SOLID   0
#define DASHED  1
#define DOTTED  3
#define DASHDOT 4

/*********************************************************/
/*** default line widths                               ***/
/*********************************************************/
#define DATA_LINE_WIDTH 0.75
#define SYN_LINE_WIDTH 1.25

/*********************************************************/
/*** page orientations                                 ***/
/*********************************************************/
#define LANDSCAPE 0
#define PORTRAIT 1

/**************************************************************/
/*** line color structure and default red and black predefs ***/
/**************************************************************/
typedef struct 
{
        float gray;
	float red;
	float grn;
	float blu;
} Color;
                /**         R   G     B   **/
static Color Black = { 0.0, 0.0,  0.0,  0.0  };
static Color Red   = { 0.3, 1.0,  0.0,  0.0  };
static Color Green = { 0.3, 0.0,  1.0,  0.0  };
static Color Blue  = { 0.3, 0.0,  0.0,  1.0  };
static Color White = { 0.3, 1.0,  1.0,  1.0  };
static Color Gray  = { 0.3, 0.3,  0.3,  0.3  };
static Color Brown = { 0.3, 0.64, 0.16, 0.16 };
static Color Purple= { 0.3, 0.63, 0.13, 0.94 };
static Color Orange= { 0.3, 1.0,  0.64, 0.0  };
static Color Cyan  = { 0.3, 0.0,  1.0,  1.0  };
static Color Yellow= { 0.3, 1.0,  1.0,  0.0  };

extern char progname[128];

void plot_dc_clvd_mech( float xc, float yc, float rad, Solution *sol, EventInfo *ev,
	Greens **grn, int iz, int nsta, int iplanes, int ifill, int imode, int verbose )
{
	int ista, npts, k, j, i, kk;
	float fIso, fclvd, psi, dpsi, theta, sinsq;
	float taz, tdp, paz, pdp, evI, tev, bev, pev, baz, bdp, pevI, bevI, tevI;
	float str, dip, rpt, rp, rt, rb, xp, yp, xt, yt, xb, yb;
	float *xpt, *ypt, xpot, ypot;
	float daz, dp, ddp, az, plusmin, radius, offset=0.3, ampMax, zilch=0.01;
	int naz, ndp, kaz, kdp;
	float pt[4], ptnew[4];
	float ax1[4], ax2[4], ax3[4];

	float **TM;

	float *xcircle, *ycircle;
	int pendown;
	float toa, azi;
	float d2r;

	extern Color Red, Black, Blue, Green, White, Brown, Gray, Purple;
	extern Color Orange, Cyan, Yellow;

	void matxvec( float **, float *, float * );
	void strdvec( float *, float *, float * );
	void strdv( float *, float *, float * );
        void crossp( float *, float *, float * );
	void vecstrd( float, float, float * );
	void mtensor( float, float, float, float, float, float, float, float **, float *, int );
	float dotp( float *, float * );
	void draw_ps_line( float *, float *, int );
	void set_ps_color( Color );
	void draw_ps_marker( Color, float, float, int, float );
	float **matrix( int, int, int, int );
        void free_matrix( float **, int, int, int, int );

	d2r = M_PI/180;

/*** allocate memory ***/

	TM = matrix( 0, 4, 0, 4 );

/*** check imode ***/

	switch( imode )
	{
		case FULLMT :

			if(verbose)
			{
			  fprintf(stdout, 
				"%s: psmech.c: psmech(): plotting FULLMT\n", progname );
			  fflush(stdout);
			}
			
			pevI = sol[iz].FullMT.P.ev;
			bevI = sol[iz].FullMT.B.ev;
			tevI = sol[iz].FullMT.T.ev;
			taz  = sol[iz].Maj.T.az;
			tdp  = sol[iz].Maj.T.pl;
			baz  = sol[iz].Maj.B.az;
			bdp  = sol[iz].Maj.B.pl;
			paz  = sol[iz].Maj.P.az;
			pdp  = sol[iz].Maj.P.pl;
			break;

		case MAJORDC :

			if(verbose)
                        {
                          fprintf(stdout,
                                "%s: psmech.c: psmech(): plotting MAJORDC\n", progname );
                          fflush(stdout);
                        }

			pevI = sol[iz].Maj.P.ev;
                	bevI = sol[iz].Maj.B.ev;
                	tevI = sol[iz].Maj.T.ev;
                	taz  = sol[iz].Maj.T.az;
                	tdp  = sol[iz].Maj.T.pl;
                	baz  = sol[iz].Maj.B.az;
                	bdp  = sol[iz].Maj.B.pl;
                	paz  = sol[iz].Maj.P.az;
                	pdp  = sol[iz].Maj.P.pl;
			break;

		case MINORDC :

			if(verbose)
                        {
                          fprintf(stdout,
                                "%s: psmech.c: psmech(): plotting MINORDC\n", progname );
                          fflush(stdout);
                        }

			pevI = sol[iz].Min.P.ev;
                	bevI = sol[iz].Min.B.ev;
                	tevI = sol[iz].Min.T.ev;
                	taz  = sol[iz].Min.T.az;
                	tdp  = sol[iz].Min.T.pl;
                	baz  = sol[iz].Min.B.az;
                	bdp  = sol[iz].Min.B.pl;
                	paz  = sol[iz].Min.P.az;
                	pdp  = sol[iz].Min.P.pl;
			break;
	}

	evI = ( tevI + bevI + pevI )/3.0;
	tev = tevI - evI;
	bev = bevI - evI;
	pev = pevI - evI;

	if( verbose ) 
	{
		fprintf(stdout, "%s: psmech.c: psmech(): tevI=%g bevI=%g pevI=%g evI=%g ", 
			progname, tevI, bevI, pevI, evI );
		fflush(stdout);

		fprintf(stdout, "tev=%g bev=%g pev=%g\n", 
			tev, bev, pev );
		fflush(stdout);
	}

	if( fabs(tev) > fabs(pev) )
	{
		vecstrd( taz, tdp, ax1 );
		vecstrd( paz, pdp, ax3 );
		fclvd = fabs( bev/tev );
		fIso = evI/tev;
	}
	else
	{
		vecstrd( paz, pdp, ax1 );
		vecstrd( taz, tdp, ax3 );
		fclvd = fabs( bev/pev );
		fIso = evI/pev;
	}

	if(verbose) 
	{
		fprintf(stdout, "%s: psmech.c: psmech(): fIso=%g fclvd=%g\n", 	
			progname, fIso, fclvd );
		fflush(stdout);
	}

	crossp( ax1, ax3, ax2 );
	strdvec( ax2, &baz, &bdp );

	if( verbose ) 
	{
		fprintf( stdout, 
		  "%s: psmech.c: psmech(): baz=%g bdp=%g bev=%g\n", 
			progname, baz, bdp, bev );
		fflush(stdout);
	}


/************************************************************************/
/*** fill + and - in shaded ( T -axis quads )                         ***/
/************************************************************************/
	if( ifill )
	{
		if(verbose)
		{
			fprintf( stdout, 
		   "%s: psmech.c: psmech(): calling mtensor() taz=%g tdp=%g paz=%g pdp=%g tevI=%g bevI=%g pevI=%g\n", 
				progname, taz, tdp, paz, pdp, tevI, bevI, pevI );
			fflush( stdout );
		}

		mtensor( taz, tdp, paz, pdp, tevI, bevI, pevI, TM, &ampMax, verbose );

		if(verbose)
		{
	  	  fprintf( stdout, "%s: psmech.c: psmech(): back from mtensor() ampMax=%g\n", 
			progname, ampMax );
		  fflush(stdout);
		}

		daz = 15;
		dp  = 15;
		ddp = 15;
		ndp = 12;

		for( kdp = 1; kdp <= ndp; kdp++ )
		{
			daz = 15;
			naz = 24;
			if( dp > 45 )
			{
				daz = 30;
				naz = 12;
				if( dp > 60 )
				{
					daz = 40;
					naz = 9;
					if( dp > 75 )
					{
						daz = 45;
						naz = 8;
					}
				}
			}

			az = 0;
			for( kaz = 1; kaz <= naz; kaz++ )
			{
				vecstrd( az, dp, pt );
				matxvec( TM, pt, ptnew );
				plusmin = dotp( pt, ptnew );
				rpt = rad * sqrt(2) * sin(0.5*d2r*(90-dp));
				xpot = xc + rpt * sin( az * d2r );
				ypot = yc + rpt * cos( az * d2r );

				if( plusmin > (zilch*ampMax) )
				{
				  draw_ps_marker( Red, xpot, ypot, 16, 3.0 );
				}
				az = az + daz;
			}
			dp = dp + ddp;

		} /** loop over kdp ***/

	} /*** if fill is needed ***/

/********************************/
/*** plot the nodal plane     ***/
/********************************/
	xpt = (float *)malloc( 3*sizeof(float) );
	ypt = (float *)malloc( 3*sizeof(float) );

	if( iplanes )
	{
		cg_linewidth( 1.25 );
		dpsi = 0.5;
		npts = ceil( 360/dpsi );
		dpsi = dpsi * d2r;
		set_ps_color( Black );
		pendown = 0;

		for( psi=0, k=0; k <npts ; k++, psi += dpsi  )
		{
			theta = 0;
			sinsq = (2.+2.*fIso)/(3.+(1.-2.*fclvd)*cos(2.*psi));

			if( sinsq > 0.0 && sinsq <= 1.0 )
				theta=asin(sqrt(sinsq));
			else
				continue;

			pt[1] = cos(theta)*ax1[1]+sin(theta)*(sin(psi)*ax2[1]+cos(psi)*ax3[1]);
			pt[2] = cos(theta)*ax1[2]+sin(theta)*(sin(psi)*ax2[2]+cos(psi)*ax3[2]);
			pt[3] = cos(theta)*ax1[3]+sin(theta)*(sin(psi)*ax2[3]+cos(psi)*ax3[3]);

			strdv( pt, &str, &dip );
			rpt = rad * sqrt(2.0) * sin( 0.5 * d2r*(90.-dip));
		
			xpt[1] = xc + rpt * sin( str * d2r );
			ypt[1] = yc + rpt * cos( str * d2r );
			
			if( k > 0 && pt[3] < 0 )
			{
				if( pendown == 1 )
				{
					draw_ps_line( xpt, ypt, 2 );
				}
				pendown = 1;
			}
			else
			{
				pendown = 0;
			}
			xpt[0] = xpt[1];
			ypt[0] = ypt[1];
		}

	/*********************************/
	/*** plot the lower half       ***/
	/*********************************/

		set_ps_color( Black );
		pendown = 0;
		for( psi=0, k=0; k<=npts; k++, psi += dpsi )
		{
			theta = 0;
			sinsq = (2+2*fIso)/(3+(1-2*fclvd)*cos(2*psi));
			if( sinsq > 0.0 && sinsq <= 1.0 )
				theta=asin(sqrt(sinsq));
			else
				continue;
			pt[1] = -cos(theta)*ax1[1]+sin(theta)*(sin(psi)*ax2[1]+cos(psi)*ax3[1]);
			pt[2] = -cos(theta)*ax1[2]+sin(theta)*(sin(psi)*ax2[2]+cos(psi)*ax3[2]);
			pt[3] = -cos(theta)*ax1[3]+sin(theta)*(sin(psi)*ax2[3]+cos(psi)*ax3[3]);

			strdv( pt, &str, &dip );
			rpt = rad * sqrt(2) * sin( 0.5 * d2r * (90-dip) );
			xpt[1] = xc + rpt * sin( str * d2r );
			ypt[1] = yc + rpt * cos( str * d2r );

			if( k>0 && pt[3] < 0 )
			{
				if( pendown == 1 )
				{
					draw_ps_line( xpt, ypt, 2 );
				}
				pendown = 1;
			}
			else
			{
				pendown = 0;
			}
			xpt[0] = xpt[1];
			ypt[0] = ypt[1];
		}
		
		free(xpt);	
		free(ypt);

		if(verbose) 
		{
		  fprintf(stdout, "%s: psmech.c: psmech(): done drawing nodal planes\n", progname );
		  fflush(stdout);
		}

	/***********************************************/
	/**** draw the circle for the focal sphere ***/
	/***********************************************/
		xcircle = (float *)malloc( npts*sizeof(float) );
		ycircle = (float *)malloc( npts*sizeof(float) );
		psi = 0;
		for( k = 0; k < npts; k++ )
		{
			xcircle[k] = xc + rad * sin( psi );
			ycircle[k] = yc + rad * cos( psi );
			psi = psi + dpsi;
		}
		set_ps_color( Black );
		cg_dash( 0, 1.0 );
		cg_linewidth( 1.25 );
		draw_ps_line( xcircle, ycircle, npts );
		free( xcircle );
		free( ycircle );
	}

/********************************************/
/*** plot station take off angles and azi ***/
/********************************************/

	for( ista = 0 ; ista < nsta ; ista++ )
	{
		toa = grn[ista][iz].Ptakeoff;
		azi = grn[ista][iz].az;
		if( grn[ista][iz].Ptakeoff > 90.0 ) 
		{
			toa = 180 - grn[ista][iz].Ptakeoff;
			azi = grn[ista][iz].baz;
		}

		/***/
		/*** do not pass EventInfo structure storing 
			the EventInfo[ista].iused field
			put maybe in Greens or pass eventinfo? ***/
		/***/

		rp = rad * sqrt(2) * sin( 0.5 * d2r * toa );
		xp = xc + rp * sin( azi * d2r );
		yp = yc + rp * cos( azi * d2r );

		if( ev[ista].iused == 1 )
		{
			draw_ps_marker( Black, xp, yp, 0, 5.5 );
			draw_ps_marker( Black, xp, yp, 0, 2.0 ); 
		}
		else
		{
			draw_ps_marker( Black, xp, yp, 0, 5.5 );
			draw_ps_marker( White, xp, yp, 0, 2.0 );
		}
	}

/********************************************/
/*** plot P, T, and B axis                ***/
/********************************************/

	rp = rad * sqrt(2) * sin( 0.5 * d2r * ( 90 - pdp ) );
	rt = rad * sqrt(2) * sin( 0.5 * d2r * ( 90 - tdp ) );
	rb = rad * sqrt(2) * sin( 0.5 * d2r * ( 90 - bdp ) );

	xp = xc + rp * sin( paz * d2r ); 
	yp = yc + rp * cos( paz * d2r );
	draw_ps_marker( Red, xp, yp, 7, 5.0 );

	xt = xc + rt * sin( taz * d2r );
        yt = yc + rt * cos( taz * d2r );
        draw_ps_marker( Blue, xt, yt, 7, 5.0 );

	xb = xc + rb * sin( baz * d2r );
        yb = yc + rb * cos( baz * d2r );
        draw_ps_marker( Gray, xb, yb, 7, 5.0 );

	if(verbose)
	{
	  fprintf( stdout, "%s: %s: %s: freeing memory\n", 
		progname, __FILE__, __func__ );
	  fflush(stdout);
	}

	free_matrix( TM, 0, 4, 0, 4 );

} /*** end of plot_dc_clvd_mech() ***/

void draw_ps_marker( Color my_color, float x, float y, int type, float size )
{
	cg_markergraycolor( my_color.gray, my_color.red, my_color.grn, my_color.blu,
		my_color.gray, my_color.red, my_color.grn, my_color.blu );
	cg_marker( x, y, type, size );
	cg_stroke();
}

void set_ps_color( Color my_color )
{
	cg_grayrgbcolor( my_color.gray, my_color.red, my_color.grn, my_color.blu );
}

void draw_ps_line( float *x, float *y, int n )
{
	int i;
	cg_move( x[0], y[0] );
	for( i=0; i<n; i++ )
		cg_line( x[i], y[i] );
	cg_stroke();
}

/****************************************************
c given azimuth and dip of t-axis and p-axis in geographic
c  coordinates and normalized eigenvalues tev1,bev1 and pev1,
c  this finds moment tensor MT in geographic
c  coordinates, as well as rotation matrix RM and transpose
c  RMT to rotate from system where t=1-axis and p=2-axis.
c********************************************************
c Method: we will find matrix which rotates 1-axis into t,
c  and 2 axis into p.
****************************************************/

#define MAX(a,b) ((a) > (b) ? (a) : (b))

void mtensor( float taz, float tdp, float paz, float pdp, float tev1, 
		float bev1, float pev1, float **MT, float *ampMax, int verbose )
{
	float **RM, **RMT, *RMdata, *RMTdata;
	float *t, *p, *pnew, *pxt;
	float **SM1, **SMT1, **SM2, **SMT2, **SM3, **SMT3, **MPT;
	int i, j;
	int row=4, col=4;
	float pang;
	float d2r;

	void matxvec( float **, float *, float * );
	void strdvec( float *, float *, float * );
	void strdrv( float *, float *, float * );
	void crossp( float *, float *, float * );
	void vecstrd( float, float, float * );
	void matprod( float **, float **, float ** );
	void rotmat( int, float, float **, float ** );
	void matpr3( float **, float **, float **, float ** );

	float **matrix( int, int, int, int );
	void free_matrix( float **, int, int, int, int );

	d2r = M_PI/180;

	if(verbose)
	{
	   fprintf( stdout, "%s: psmech.c: mtensor(): allocating memory\n", progname );
	   fflush(stdout);
	} 

	RM   = matrix( 0, row, 0, col );
	RMT  = matrix( 0, row, 0, col );
	SM1  = matrix( 0, row, 0, col );
	SMT1 = matrix( 0, row, 0, col );
	SM2  = matrix( 0, row, 0, col );
	SMT2 = matrix( 0, row, 0, col );
	SM3  = matrix( 0, row, 0, col );
	SMT3 = matrix( 0, row, 0, col );
	MPT  = matrix( 0, row, 0, col );

	if(verbose)
	{
	  fprintf( stdout, "%s: psmech.c: mtensor(): done allocating matrix memory\n", progname );
          fflush(stdout);
	}
                                                                                                                                                                                                            
	t    = (float *)calloc(row,sizeof(float));
        p    = (float *)calloc(row,sizeof(float));
        pnew = (float *)calloc(row,sizeof(float));
        pxt  = (float *)calloc(row,sizeof(float));

/* set up moment tensor in t-p system */

	MPT[1][1]=tev1;
        MPT[2][2]=pev1;
        MPT[3][3]=bev1;

/* condition p-axis so that it really is perpendicular to t-axis */

        vecstrd(taz,tdp,t);
        vecstrd(paz,pdp,p);
        crossp(p,t,pxt);
        crossp(t,pxt,p);

/* Step 1: rotate 1-axis about 3-axis so that 1-axis lies along t azimuth */

        rotmat(3,(taz-90.),SM1,SMT1);
 
/* Step 2: rotate 1-axis about 2-axis so that 1-axis lies along t */

        rotmat(2, tdp, SM2, SMT2);
        matprod(SM2, SM1, SM3);
        matprod(SMT1, SMT2, SMT3);

/*  in this system, p should lie in 2-3 plane */

        matxvec( SM3, p, pnew );

/* Step 3: rotate about 1 axis so that 2 axis lies along p */

        pang = atan2( pnew[3], pnew[2] )/d2r;
        rotmat(1,-pang,SM1,SMT1);
        matprod(SM1,SM3,RM);
        matprod(SMT3,SMT1,RMT);

/*********************************************************/
/* Finally: find moment tensor in space coordinates.     */
/*  This since Mpt=RM*Mspace*RMT, thus Mspace=RMT*Mpt*RM */
/*********************************************************/
        matpr3(RMT,MPT,RM,MT);
                                                                                          
/*******************************************
 ampMax is absolute maximum amplitude
*******************************************/

        tev1 = fabs(tev1);
        bev1 = fabs(bev1);
        pev1 = fabs(pev1);
        *ampMax = MAX(tev1,bev1);
        *ampMax = MAX(*ampMax,pev1);

/************************/
/*** free all memory ***/
/***********************/

	if(verbose)
	{  
	  fprintf(stdout, "%s: psmech.c: mtensor() trying to free memory\n", progname );
	  fflush(stdout);
	}

	free(t);
	free(p);
	free(pxt);
	free(pnew);

	free_matrix( RM,   0, row, 0, col );
        free_matrix( RMT,  0, row, 0, col );
        free_matrix( SM1,  0, row, 0, col );
        free_matrix( SMT1, 0, row, 0, col );
        free_matrix( SM2,  0, row, 0, col );
        free_matrix( SMT2, 0, row, 0, col );
        free_matrix( SM3,  0, row, 0, col );
        free_matrix( SMT3, 0, row, 0, col );
        free_matrix( MPT,  0, row, 0, col );

	if(verbose)
	{
	   fprintf(stdout, "%s: psmech.c: mtensor(): done freeing memory\n", progname );
	   fflush(stdout);
	}
}

/*************************************************************/
/****** finds product P or three matrices A,B and C     ******/
/*************************************************************/

void matpr3( float **a, float **b, float **c, float **p )
{
        float **scr;
        int i,j, row=4, col=4;
        void matprod( float **, float **, float ** );
	float **matrix( int, int, int, int );
        void free_matrix( float **, int, int, int, int );

	scr = matrix( 0, row, 0, col );
        matprod(a,b,scr);
        matprod(scr,c,p);
	free_matrix( scr, 0, row, 0, col );
}

/*************************************************************/
/***** FINDS PRODUCT P=A TIMES B OF 3X3 MATRICES A & B *******/
/*************************************************************/

void matprod( float **a, float **b, float **p )
{
        int i, j, k;
        for( i=1; i<=3; i++ ) 
	{
                for( j=1; j<=3; j++ ) 
		{
                        p[i][j] =0.0;
                        for( k=1; k<=3; k++ ) 
			{
                                p[i][j] += a[i][k]*b[k][j];
                        }
                }
        }
}

/***********************************************************************/
/***** finds product vnew = A times v of 3x3 matrix a and 3 vector *****/
/***********************************************************************/

void matxvec( float **a, float *v, float *vnew )
{
        vnew[1] = a[1][1] * v[1] + a[1][2] * v[2] + a[1][3] * v[3];
        vnew[2] = a[2][1] * v[1] + a[2][2] * v[2] + a[2][3] * v[3];
        vnew[3] = a[3][1] * v[1] + a[3][2] * v[2] + a[3][3] * v[3];
}

/***********************************************************************/
/*** dot product                                                     ***/
/***********************************************************************/

float dotp( float *x, float *y )
{
        return  x[1]*y[1]+x[2]*y[2]+x[3]*y[3];
}
                                                                                          
/***********************************************************************/
/*** FINDS NORMALIZED CROSS PRODUCT Y X Z OF VECTORS Y, Z            ***/
/***********************************************************************/

void crossp( float *y, float *z, float *ycz )
{
        void xnorm( float * );
        ycz[1] = y[2] * z[3] - y[3] * z[2];
        ycz[2] = y[3] * z[1] - y[1] * z[3];
        ycz[3] = y[1] * z[2] - y[2] * z[1];
        xnorm(ycz);
}

void xnorm( float *x )
{
        float size;
	float dotp( float *, float * );

        size = sqrt( dotp(x,x) );
        if( size == 0.0 ) return;
        x[1] = x[1]/size;
        x[2] = x[2]/size;
        x[3] = x[3]/size;
}

void vecstrd( float str, float dip, float *r )
{
        float d2r;
        d2r = M_PI/180.0;
        r[1] = sin( d2r * str ) * cos( d2r * dip );
        r[2] = cos( d2r * str ) * cos( d2r * dip );
        r[3] = -sin( d2r * dip );
}

/*******************************************************************************/
/*** FINDS STRIKE AND DIP GIVEN COMPONENTS UNIT VECTOR; dip down is positive ***/
/*******************************************************************************/

void strdv( float *r, float *str, float *dip )
{
        float rs, d2r;
	d2r = M_PI/180;
        *str = atan2( r[1],r[2] )/d2r;
        rs  = sqrt( r[1]*r[1] + r[2]*r[2] );
        *dip = atan2( -r[3], rs )/d2r;
}

/*******************************************************************************/
/*** FINDS STRIKE AND DIP GIVEN COMPONENTS OF DOWNWARD POINTING UNIT VECTOR ***/
/*******************************************************************************/

void strdvec( float *r, float *str, float *dip )
{
        float rs,  d2r;
        d2r = M_PI/180.0;
        if( r[3] > 0 ) {
                r[1] = -r[1];
                r[2] = -r[2];
                r[3] = -r[3];
        }
        *str = atan2( r[1], r[2] )/d2r;
        if( *str < 0 ) *str+=360.0;
        rs = sqrt( r[1]*r[1] + r[2]*r[2] );
        *dip = atan2( -r[3], rs )/d2r;
}

/****************************************************************************/
/*** finds simple rotation matrix R and transpose RT for rotation         ***/
/***  of vector by angle theta about x=1, y=2 or z=3 axis. Alternatively, ***/
/***  it rotates axes angle minus theta                                   ***/
/*** TAKES VECTOR V AND ROTATES IT ANGLE THETA ABOUT AXIS X=1, Y=2, Z=3   ***/
/*** Alternatively, it rotates axis an angle minus theta                  ***/
/***  It also returns rotation matrix A and transpose AT                  ***/
/****************************************************************************/

void rotmat( int naxis, float theta, float **r, float **rt )
{
        float thetr;
        int i, j;
	float d2r;
	d2r = M_PI/180;

        thetr = theta * d2r;
        for( i=1; i<=3; i++ ) 
	{
                r[i][i] = cos( thetr );
                for( j=1; j<=3; j++ ) 
		{
                        if( i != j ) r[i][j] = 0.0;
                }
        }
/* NAXIS = X AXIS = 1 */
        if( naxis == 1 ) 
	{
                r[1][1] = 1.0;
                r[2][3] = -sin(thetr);
                r[3][2] =  sin(thetr);
        }
        else if( naxis == 2 ) 
	{
                r[2][2] = 1.0;
                r[1][3] = -sin(thetr);
                r[3][1] = sin(thetr);
        }
        else {
                r[3][3] = 1.0;
                r[1][2] = -sin(thetr);
                r[2][1] = sin(thetr);
        }

/* now find transpose */
        for( i=1; i<=3; i++ ) 
	{
                for( j=1; j<=3; j++ ) 
		{
			rt[i][j] = r[j][i];
		}
        }
}
