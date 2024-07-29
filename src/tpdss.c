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

/*** from numerical recipes (https://numerical.recipes) ***/

/*
   This takes the orientations of the tension and pressure
   axes, and determines the two nodal planes
   convert (P,T) to (dip,slip,stk) and (X,Y,Z).
      strike angle is measured from the north clockwise.
      plunge angle is measured from the horizon downward.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/mt.h"

/*** old interface ***/
/************
tpdss( dstkt, dplnt, dstkp, dplnp, dip0, stk0, slip0, dip1, stk1, slip1 )
float dstkt, dplnt, dstkp, dplnp;
float *dip0, *stk0, *slip0;
float *dip1, *stk1, *slip1;
************/

void tpdss( Solution *sol, int iz )
{
	float dstkt, dplnt, dstkp, dplnp;
	float dip0, stk0, slip0;
	float dip1, stk1, slip1;

	float stk, dip, slip;
	float stkp, plnp, stkt, plnt, deg, t1, t2, t3, p1, p2, p3;
	float f1, f2, f3, yy, xn1, xn2, xn3;
	float xx, dipp, stkk, slipp;

	dstkt = sol[iz].stkt;
	dplnt = sol[iz].plnt;
	dstkp = sol[iz].stkp;
	dplnp = sol[iz].plnp;

	deg=M_PI/180.0;

	if( sol[iz].stkt < -999.)
	{
		fprintf( stderr, "something went wrong in tpdss\n" );
		exit(-1);
	}

	stkp=dstkp*deg;
	plnp=dplnp*deg;
	stkt=dstkt*deg;
	plnt=dplnt*deg;

	t1=cos(plnt)*cos(stkt);
	t2=cos(plnt)*sin(stkt);
	t3=sin(plnt);
	p1=cos(plnp)*cos(stkp);
	p2=cos(plnp)*sin(stkp);
	p3=sin(plnp);
	f1=t1+p1;
	f2=t2+p2;
	f3=t3+p3;
	yy=sqrt(f1*f1+f2*f2+f3*f3);
	f1=f1/yy;
	f2=f2/yy;
	f3=f3/yy;
	xn1=t1-p1;
	xn2=t2-p2;
	xn3=t3-p3;
	yy=sqrt(xn1*xn1+xn2*xn2+xn3*xn3);
	xn1=xn1/yy;
	xn2=xn2/yy;
	xn3=xn3/yy;
	if( xn3 > 0 )
	{
		xn1=-xn1;
		xn2=-xn2;
		xn3=-xn3;
		f1=-f1;
		f2=-f2;
		f3=-f3;
	}

/* due to numerical inaccuracy we need: */
	if( fabs(xn3) <= 1.0)
		dip=acos(-xn3);
	else if ( xn3 >= 1.0 )
		dip = acos(-1.0);
	else if(xn3 <= -1.0) 
		dip = 0.0;

	stk=atan2(-xn1,xn2);
	xx=f1*xn2-f2*xn1;
	slip=atan2(-f3,xx);
	dip1  = dip/deg;
	slip1 = slip/deg;
	stk1  = stk/deg;
	
	if( slip1 < -180.) slip1 += 180.;
	if( slip1 >  180.) slip1 -= 360.;
	if( stk1 < 0.0)   stk1 += 360.0;
	if( stk1 < 0.01)  stk1 = 0.;
	if( fabs( dip1 ) < 0.01)  dip1=0.;
	if( fabs( slip1 ) < 0.01) slip1=0.;
	while( stk1 > 360. ) stk1 -= 360.;

	if(f3 > 0)
	{
		xn1=-xn1;
		xn2=-xn2;
		xn3=-xn3;
		f1=-f1;
		f2=-f2;
		f3=-f3;
	}

	if( fabs(f3) <= 1.0 )
		dipp=acos(-f3);
	else if( f3 >= 1.0 )
		dipp = acos(-1.0);
	else if( f3 <= -1.0 )
		dipp = 0.0;
	
	stkk=atan2(-f1,f2);
	xx=f2*xn1-f1*xn2;
	slipp=atan2(-xn3,xx);
	dip0=dipp/deg;
	slip0=slipp/deg;
	stk0=stkk/deg;
	
	if( slip0 < -180. ) slip0 += 360.;
	if( slip0 > 180. ) slip0 -= 360.;
	if( fabs( slip0 ) < 0.01) slip0 = 0.;

	while( stk0 > 360. ) stk0 -= 360.;
	while( stk0 < 0.0 ) stk0 += 360.;
	if( stk0 < 0.001 ) stk0 = 0.;

	if( fabs( dip0 ) < 0.01) dip0 = 0.;
	while( dip0 > 90. ) dip0 -= 90.;
	while( dip0 < 0. )  dip0 += 90.;

	sol[iz].dip0 = dip0;
	sol[iz].dip1 = dip1;
	sol[iz].stk0 = stk0;
	sol[iz].stk1 = stk1;
	sol[iz].rak0 = slip0;
	sol[iz].rak1 = slip1;
}
