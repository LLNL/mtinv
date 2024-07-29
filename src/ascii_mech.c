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
#include <string.h>
#include <math.h>

#define DIMX 60
#define DIMY 60
#define EX 1.8
typedef struct plane {
	double stk,dip,rak;
} Plane;
typedef struct xyz {
	double x,y,z;
} Point;

/**** testing ***/
/***
main(ac,av)
int ac;
char **av;
{
	float stk1,dip1,rak1,stk2,dip2,rak2;
	float pplg,pazi,tazi,tplg;
	int nx,ny,nr;
	nx=12;
	ny=12;
	nr=12;
	stk1=309;
	dip1=59;
	rak1=173;
	stk2=43;
	dip2=84;
	rak2=31;
	pplg = 17;
	pazi = 172;
	tazi = 270;
	tplg = 26;
	ascii_mech( nx, ny, nr, stk1, dip1, rak1, stk2, dip2, rak2, pplg, pazi, tplg, tazi );
}
***/



/*** this creates a text representation of the MT solution for email messages ***/

void ascii_mech( FILE *fp, int x, int y, int r,
	float stk1, float dip1, float rak1, 
	float stk2, float dip2, float rak2, 
	float pplg, float pazi, float tplg, float tazi )
{
	Plane np1, np2;
	Plane T_az_pl, P_az_pl;
	Point Taxis, Paxis;
	double pi;
	char sArea[DIMX][DIMY];

	Plane np1_to_np2( Plane );
	Plane xyz_to_plane( Point);
	Point np1_to_P( Plane, Plane );
	Point np1_to_T( Plane, Plane );
	void circle( int, int, int, char, char [DIMX][DIMY] );
	void paint_the_ball( int, int, int, Plane, Plane, char, double, char [DIMX][DIMY] );
	void plot_axis( int, int, int, Point, char, char, char [DIMX][DIMY] );
	void print_sArea( FILE *, int, int, char [DIMX][DIMY] );

/*** initialize variables ***/
	pi = M_PI;
	memset( sArea, ' ', DIMX*DIMY );

	np1.stk = (double)stk1;
	np1.dip = (double)dip1;
	np1.rak = (double)rak1;
	np2.stk = (double)stk2;
	np2.dip = (double)dip2;
	np2.rak = (double)rak2;

	P_az_pl.dip = pplg;
	P_az_pl.stk = pazi;
	T_az_pl.dip = tplg;
	T_az_pl.stk = tazi;

/*** calculate second nodal plane and P-T Axis azimuth and plunge from nodal plane 1 ***/
/***
	np2   = np1_to_np2( np1 );
****/
	Paxis = np1_to_P( np1, np2 );
	Taxis = np1_to_T( np1, np2 );
/****
	T_az_pl = xyz_to_plane( Taxis );
	P_az_pl = xyz_to_plane( Paxis );
****/

/*** print output ***/
/***
	fprintf(fp, " np1: %4.0f %4.0f %4.0f\n", np1.stk, np1.dip, np1.rak );
	fprintf(fp, " np2: %4.0f %4.0f %4.0f\n", np2.stk, np2.dip, np2.rak );
	fprintf(fp, " P  plg:%4.0f az:%4.0f \n", P_az_pl.dip, P_az_pl.stk );
	fprintf(fp, " T  plg:%4.0f az:%4.0f \n", T_az_pl.dip, T_az_pl.stk );
***/
/*** print a filled circle ***/
	circle( x, y, r, '-', sArea );

/*** print the nodal planes ***/
/*
	if( np1.dip != 0 ) plplan( x, y, r, np1.stk, np1.dip, np1.rak, '.', sArea );
	if( np2.dip != 0 ) plplan( x, y, r, np2.stk, np2.dip, np2.rak, '.', sArea );
*/
/*** fill the compressional quadrants (T-axis) ****/
	paint_the_ball( x, y, r, np1, np2, '#', T_az_pl.stk, sArea );

/*** plot the P and T axis ****/
	plot_axis( x, y, r, Paxis, 'P', ' ', sArea);
	plot_axis( x, y, r, Taxis, 'T', ' ', sArea);

/*** send to output ***/
	print_sArea( fp, 2 * x + 1, DIMY, sArea );
}

/*** hardwired to fill circle ****/
void circle( int x1, int y1, int R, char cX, char sArea[DIMX][DIMY] )
{
	double alfa, dr, xx, yy;
	int xp, yp;
	int conv_x( int, int, double, double, double );
	int conv_y( int, int, double, double, double );
	void moveto( int, int, char, char [DIMX][DIMY] );

	for( dr = 0; dr <= R; dr += (double)R/20. ) 
	{
		for( alfa=0.0; alfa<360.0; alfa += 1.0  ) 
		{
			xx = x1 + dr * cos( alfa );
			yy = y1 + dr * sin( alfa );
			xp = conv_x( 0, (int)(2*R*EX), xx, 0., (double)(2.0*R) );
			yp = conv_y( 0,           2*R, yy, 0., (double)(2.0*R) );
			moveto( xp+1, yp, cX, sArea );
		}
	}
}

void moveto( int x, int y, char cX, char sArea[DIMX][DIMY] )
{
	if( ( x >=0 && x < DIMX) && ( y >=0 && y < DIMY ) )
        sArea[x][y] = cX;
}

int conv_x( int x1, int x2, double xx, double xmin, double xmax )
{
    if ( xmax != xmin )
        return((int)(((xx-xmin)*(double)(x2-x1)/(xmax-xmin))+(double)x1));
    else return 1;
}

int conv_y( int x1, int x2, double xx, double xmin, double xmax )
{
    if ( xmax != xmin )
        return((int)(((xx-xmin)*((double)(x1-x2))/(xmax-xmin))+(double)x2));
    else return 1;
}

void print_sArea( FILE *fp, int nX, int nY, char sArea[DIMX][DIMY] )
{
	int i, k;
	for ( i= 0; i < nX; i++)
	{
		for ( k= 0; k < nY; k++)
		{
			fprintf( fp, "%c", sArea[k][i] );
		}
		fprintf( fp, "\n" );
	}
}

void paint_the_ball( int X, int Y, int R, Plane NP1, Plane NP2, char cX, double fi_T, char sArea[DIMX][DIMY] )
{
	double pi, rad, fi, dInc;
	int IM;
	int sign( double );
	void psom( int, int, int, double, double, double, 
		double, double, double, int, char, char [DIMX][DIMY] );

	pi = M_PI;
	rad = pi/180.;
	IM = sign(NP1.rak);

	for ( dInc= 0.0; dInc <= 90.0; dInc += 1.0 )
	{
		for ( fi = 0.0; fi <= 360.0; fi += 1.0 )
		{
			psom(X,Y,R,
				NP1.dip * rad,  NP2.dip * rad,
				NP1.stk * rad , NP2.stk * rad,
				dInc * rad , fi* rad,
				IM, cX, sArea);
		}
	}
}

void plplan( int X, int Y, int R, double strk, double dip, double slip, char cX, char sArea[DIMX][DIMY] )
{
	double pi,xx,yy,t,rad,fi,d,l,x,y;
	int xp, yp;
	int conv_x( int, int, double, double, double );
        int conv_y( int, int, double, double, double );
	void moveto( int, int, char, char [DIMX][DIMY] );

	pi = M_PI;
	rad = pi/180.;
	fi = strk * rad;
	d  = dip  * rad;
	l  = slip * rad;

	for( t=0.0; t <= 180.0; t += 2.0 ) 
	{
		x = cos(t * rad);
		y = cos(d) * 0.85 * sin(t * rad);
		xx =  X + R * (sin(fi) * x + cos(fi) * y );
		yy =  Y + R * (cos(fi) * x - sin(fi) * y );
		xp = conv_x( 0, 2*X, xx * EX, 0., 2.0*R);
		yp = conv_y( 0, 2*Y, yy, 0.0, 2.0*R);
		moveto(xp+1, yp, cX,sArea);
	}
}

int sign( double x ) 
{
	int y;
	if (x==0.) { y=0; }
	if (x>0.) { y=1; }
	if (x<0.) { y=-1; }
	return (y);
}

/* compute the sign of radiation function at the point fi, dInc)
   thanks to Hugues Dufumier for the psom function
*/
void psom( int X, int Y, int R, 
	double d1, double d2, double fi1, double fi2, double dInc, double fi,
	int IM, char cX, char sArea[DIMX][DIMY] )
{
	double RR, XX, YY;
	double P1, P2;
	int xp, yp;
	int conv_x( int, int, double, double, double );
        int conv_y( int, int, double, double, double );

	P1= sin(fi1 - fi) * sin(d1) * sin (dInc) +  cos(d1) *cos(dInc);
	P2= sin(fi2 - fi) * sin(d2) * sin (dInc) +  cos(d2) *cos(dInc);
	if (P1*P2*(double)IM >0.)
	{
		RR = R * sin(dInc / 2.)*sqrt(2.);
		XX = X + RR * sin(fi);
		YY = Y + RR * cos(fi);
		xp = conv_x( 0, (int)(2*X), XX*EX, 0.0, 2.0*R);
		yp = conv_y( 0, (int)(2*Y),    YY, 0.0, 2.0*R);
		moveto(xp +1, yp, cX, sArea);
	}
}

Plane np1_to_np2( Plane np1 )
{
	Plane np2;
	double dstk=0;
	double pi;

	pi = M_PI;
	double r=pi/180;
	np2.stk=0;
	np2.dip=0;
	np2.rak=0;
	dstk = (1/r) * atan( -cos( r * np1.rak ) / (cos(r*np1.dip)*sin(r * np1.rak) ) );
	np2.stk = np1.stk - dstk;
	np2.stk -= 180;
	if( np2.stk < 0 ) np2.stk += 360;

	np2.dip = (1./r) * atan(cos(r*np1.dip) / ( sin(r*np1.dip)*cos(r*dstk) ) );
	np2.rak = acos( sin(r*np1.dip)*cos(r*np1.rak)/sin(r*np2.dip) )/r;
	np2.rak -= 180;
	if( np1.rak > 0 ) np2.rak *= -1;
	return np2;
}

Plane xyz_to_plane( Point m )
{
	Plane z;
	double pi;
	pi = M_PI;

	if( m.z == 0 )
		z.dip = 90;
	else
		z.dip = 180/pi * atan2( m.z, sqrt( m.x * m.x + m.y * m.y ) );
	if( m.x == 0 ) 
		z.stk = 90;
	else
		z.stk = (180/pi) * atan2( m.y, m.x );
	z.rak = 0;
	if( z.dip < 0 ) 
	{
		z.dip = fabs(z.dip);
		z.stk -= 180;
	}
	if( z.stk < 0 ) z.stk += 360;
	return z;
}

Point np1_to_P( Plane np1, Plane np2 )
{
	Point t;
	double stk,dip,rak;
	double pi;

	pi = M_PI;
	stk = np1.stk * pi/180;
        dip = np1.dip * pi/180;
	rak = np1.rak * pi/180;
	t.x = -1/sqrt(2) * ( cos(rak)*cos(stk) + (sin(rak)*cos(dip) + sin(dip))*sin(stk));
	t.y = -1/sqrt(2) * ( cos(rak)*sin(stk) - (sin(rak)*cos(dip) + sin(dip))*cos(stk));
	t.z =  1/sqrt(2) * ( sin(rak)*sin(dip) - cos(dip));
	t.x *= -1;
	t.y *= -1;
	t.z *= -1;
	return t;
}

Point np1_to_T( Plane np1, Plane np2 )
{
	Point t;
	double stk,dip,rak;
	double pi;

	pi = M_PI;
	stk = np1.stk * pi/180;
	dip = np1.dip * pi/180;
	rak = np1.rak * pi/180;
	t.x =  1/sqrt(2) * ( cos(rak)*cos(stk) + (sin(rak)*cos(dip)-sin(dip))*sin(stk));
	t.y =  1/sqrt(2) * ( cos(rak)*sin(stk) - (sin(rak)*cos(dip)-sin(dip))*cos(stk));
	t.z = -1/sqrt(2) * ( sin(rak)*sin(dip) + cos(dip) );
	t.x *= -1;
	t.y *= -1;
	t.z *= -1;
	return t;
}

void plot_axis( int X, int Y, int R, Point axis, char cX1, char cX2, char sArea[DIMX][DIMY] )
{
	int xp, yp;
	int conv_x( int, int, double, double, double );
        int conv_y( int, int, double, double, double );
        void moveto( int, int, char, char [DIMX][DIMY] );

	xp = conv_x( 0, 2*R, axis.y, -1.0, 1.0 );
	yp = conv_y( 0, 2*R, axis.x, -1.0, 1.0 );

	moveto((int)(xp *EX),yp,cX1, sArea);
	moveto((int)((xp) *EX)-1,yp,  cX2, sArea);
	moveto((int)((xp) *EX)-1,yp -1,cX2, sArea);
	moveto((int)((xp) *EX)-1,yp +1,cX2, sArea);
	moveto((int)(xp *EX),yp-1,cX2, sArea);
	moveto((int)(xp *EX),yp+1,cX2, sArea);
	moveto((int)(xp *EX) +1,yp  ,cX2, sArea);
	moveto((int)(xp *EX) +1,yp-1,cX2, sArea);
	moveto((int)(xp *EX) +1,yp+1,cX2, sArea);
}
