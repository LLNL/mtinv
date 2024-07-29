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
#include "../include/mt.h"

/*** from numerical recipes (https://numerical.recipes) ***/

/*******************************************************************************
Eigenvalue solvers, tred2 and tqli, from "Numerical Recipes in C" (Cambridge
Univ. Press) by W.H. Press, S.A. Teukolsky, W.T. Vetterling, and B.P. Flannery
*******************************************************************************/

/***
trans1( stkt, plnt, stkp, plnp, stkn, plnn, evd, z, np, tol, nmt )
float *stkt, *plnt, *stkp, *plnp, *stkn, *plnn;
float *evd, **z;
int np, nmt;
float tol;
***/

void trans1( Solution *sol, int iz, int index[], int mode )
{
	float stkt, plnt, stkp, plnp, stkn, plnn;
	float evd[4], z[4][4];
	int np=3, nmt=3;
	float tol=1.0E-05;
	int i, j;
	float wmax=0.;
	float a11,a12,a13,a21,a22,a23,a31,a32,a33;
	float p1,p2,p3,t1,t2,t3;
	float xnorm, ynorm, znorm, tnorm, pnorm, tmp1, tmp2, tmp3, xy, z3, x1234;
	float plx, ply, plz, plt, plp;
	float delx, betx, dely, bety, delz, betz, delt, bett, delp, betp;
	void phase( float, float, float, float *, float * );

/*** added when converted interface to structure ***/
	for( i=1; i<=3; i++ )
	{
		if( mode == 0 )
		{
			evd[i] = sol[iz].Maj.eig[index[i]].val;
			for( j=1; j<=3; j++ )
				z[j][i] = sol[iz].Maj.eig[index[i]].vec[j];
		}
		if( mode == 1 )
		{
			evd[i] = sol[iz].Min.eig[index[i]].val;
			for( j=1; j<=3; j++ )
				z[j][i] = sol[iz].Min.eig[index[i]].vec[j];
		}
	}

	for(wmax=0, i=1;i<=nmt;i++)
		if( fabs( evd[i] ) > wmax )
			wmax=fabs( evd[i] );

	for(i=1; i<=nmt; i++)
	{
		if( fabs( evd[i] ) < tol*wmax ) 
		{
			a31=z[1][i];
			a32=z[2][i];
			a33=z[3][i];
		}
		if( evd[i] < -tol*wmax )
		{
			p1=z[1][i];
			p2=z[2][i];
			p3=z[3][i];
		}
		if( evd[i] > tol*wmax )
		{
			t1=z[1][i];
			t2=z[2][i];
			t3=z[3][i];
		}
	}
	a11=0.5*(t1+p1);
	a12=0.5*(t2+p2);
	a13=0.5*(t3+p3);
	a21=t1-a11;
	a22=t2-a12;
	a23=t3-a13;
	tnorm=1.0;
	if(t3 <= 0.0) tnorm=-tnorm;
	t1=t1/tnorm;
	t2=t2/tnorm;
	t3=t3/tnorm;
	pnorm=1.0;
	if(p3 <= 0.0) pnorm=-pnorm;
	p1=p1/pnorm;
	p2=p2/pnorm;
	p3=p3/pnorm;
	xnorm=sqrt(2.);
	if(a13 < 0.0) xnorm=-xnorm;
	ynorm=sqrt(2.);
	if(a23 < 0.0) ynorm=-xnorm;
	znorm=1.0;
	if(a33 < 0.0) znorm=-1.0;
	a11=a11/xnorm;
	a12=a12/xnorm;
	a13=a13/xnorm;
	a21=a21/ynorm;
	a22=a22/ynorm;
	a23=a23/ynorm;
	a31=a31/znorm;
	a32=a32/znorm;
	a33=a33/znorm;
                                                                                                                                   
/********************************************************************************************************
   now all vectors point into the lower hemisphere To get the correct orientations, we require
   that if the center of the focal sphere is a compression that the X,Y, Z axes form a right
   handed coordinate system in the lower hemisphere, otherwise it will form a left handed
   coordinate system
 *********************************************************************************************************/
                                                                                                                                   
	xy = a13*a23;           /* obtain P-wave polarity at the center of the focal sphere */
	z3 = a11*a22 - a12*a21; /* determine if right handed or left handed coordinate system */
                                                                                                                                   
/* make right handed coordinate system */
	if(z3 < 0.0)
	{
		tmp1=a11;
		tmp2=a12;
		tmp3=a13;
		a11=a21;
		a12=a22;
		a13=a23;
		a21=tmp1;
		a22=tmp2;
		a23=tmp3;
	}
	x1234 = 1.0;
                                                                                                                                   
/* x and y axis */
	phase(a11,a12,a13, &delx, &betx);
	phase(a21,a22,a23, &dely, &bety);
	plx = 90. - betx;
	ply = 90. - bety;
                                                                                                                                   
/*** Z-axis (NULL AXIS) ****/
	phase(a31,a32,a33, &delz, &betz);
	plz = 90. - betz;
	stkn = delz;
	plnn = plz;
                                                                                                                                   	
/*** T-axis ****/
	phase(t1,t2,t3, &delt, &bett);
	plt = 90. - bett;
	stkt = delt;
	plnt = 90.0 - bett;

/*** P-axis ****/
	phase(p1,p2,p3, &delp, &betp);
	plp = 90. - betp;
	stkp = delp;
	plnp = 90.0 - betp;

	sol[iz].stkt = stkt;
	sol[iz].plnt = plnt;
	sol[iz].stkp = stkp;
	sol[iz].plnp = plnp;
	sol[iz].stkn = stkn;
	sol[iz].plnn = plnn;

	/** no return needed, function protocol returns void ***/
}

/*********************************************************************
This expresses a three component vector in terms
of two angles in a spherical coordinate system
  x     x-coordinate of vector
  y     y-coordinate of vector
  z     z-coordinate of vector
  del   direction of projection of the vector on a
        horizontal plane, measured positively, clockwise
        from north - the trend of the vector.
  bet   angle that the 3-D vector makes with the
        downward vertical. This is just 90 - plunge
************************************************************************/
void phase( float x, float y, float z, float *del, float *bet )
{
	float degrad;
	degrad = M_PI/180;

	if( fabs(z) <= 1 )
		*bet = acos(z);
	else if ( z > 1 )
		*bet = 0;
	else if ( z < -1 )
		*bet = acos(-1);

	*bet = *bet / degrad;

	if( x < 0 )
	{
		*del = atan(y/x) / degrad+180;
		if( *del > 360) *del -= 360;
		if( *del < 0)  *del += 360;
	}
	else if( x == 0 )
	{
		if( y <  0 ) *del = 270;
		if( y == 0 ) *del = 0;
		if( y >  0 ) *del = 90;
	}
	else if( x > 0 )
	{
		if( y < 0 )
		{
			*del=atan(y/x)/degrad+360;
			if( *del > 360 ) *del -= 360;
			if( *del < 0 )   *del += 360;
		}
		if( y >= 0.0 )
		{
			*del=atan(y/x)/degrad;
			if( *del > 360) *del -= 360;
			if( *del < 0)   *del += 360;
		}
	}
}
