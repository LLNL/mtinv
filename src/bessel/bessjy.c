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

/****
  bessjy.c 
  NR 
  Returns the Bessel functions rj = Jν , ry = Yν and their derivatives 
  rjp = Jν′ , ryp = Yν′, for positive x and for xnu = ν ≥ 0. The relative 
  accuracy is within one or two significant digits of EPS, except near a 
  zero of one of the functions, where EPS controls its absolute accuracy. 
  FPMIN is a number close to the machine’s smallest floating-point number. 
  All internal arithmetic is in double precision. To convert the entire 
  routine to double precision, change the REAL declaration above and 
  decrease EPS to 10−16. Also convert the subroutine beschb.
****/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*** newer version of nrutil.h ***/

static int imaxarg1,imaxarg2;
#define IMAX(a,b) (imaxarg1=(a),imaxarg2=(b),(imaxarg1) > (imaxarg2) ? (imaxarg1) : (imaxarg2))

static int iminarg1,iminarg2;
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ? (iminarg1) : (iminarg2))

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

#define EPS 1.0e-10
#define FPMIN 1.0e-30
#define MAXIT 10000
#define XMIN 2.0

void bessjy(float x, float xnu, float *rj, float *ry, float *rjp, float *ryp)
{
	void beschb(double x, double *gam1, double *gam2, double *gampl, double *gammi);

	int i,isign,l,nl;

	double a,b,br,bi,c,cr,ci,d,del,del1,den,di,dlr,dli,dr,e,f,fact,fact2,
		fact3,ff,gam,gam1,gam2,gammi,gampl,h,p,pimu,pimu2,q,r,rjl,
		rjl1,rjmu,rjp1,rjpl,rjtemp,ry1,rymu,rymup,rytemp,sum,sum1,
		temp,w,x2,xi,xi2,xmu,xmu2;

	if (x <= 0.0 || xnu < 0.0) 
	{ 
		fprintf( stderr, "%s: %s: bad arguments in bessjy\n", __FILE__, __func__ );
		exit(-1);
	}

	nl=(x < XMIN ? (int)(xnu+0.5) : IMAX(0,(int)(xnu-x+1.5)));
	xmu=xnu-nl;
	xmu2=xmu*xmu;
	xi=1.0/x;
	xi2=2.0*xi;
	w=xi2/M_PI;
	isign=1;
	h=xnu*xi;
	if (h < FPMIN) h=FPMIN;
	b=xi2*xnu;
	d=0.0;
	c=h;
	for (i=1;i<=MAXIT;i++) {
		b += xi2;
		d=b-d;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=b-1.0/c;
		if (fabs(c) < FPMIN) c=FPMIN;
		d=1.0/d;
		del=c*d;
		h=del*h;
		if (d < 0.0) isign = -isign;
		if (fabs(del-1.0) < EPS) break;
	}
	if (i > MAXIT) 
	{
		fprintf( stderr, "%s: %s: x too large in bessjy; try asymptotic expansion\n",
			__FILE__, __func__ );
		exit(-1);
	}
	rjl=isign*FPMIN;
	rjpl=h*rjl;
	rjl1=rjl;
	rjp1=rjpl;
	fact=xnu*xi;
	for (l=nl;l>=1;l--) {
		rjtemp=fact*rjl+rjpl;
		fact -= xi;
		rjpl=fact*rjtemp-rjl;
		rjl=rjtemp;
	}
	if (rjl == 0.0) rjl=EPS;
	f=rjpl/rjl;
	if (x < XMIN) {
		x2=0.5*x;
		pimu=M_PI*xmu;
		fact = (fabs(pimu) < EPS ? 1.0 : pimu/sin(pimu));
		d = -log(x2);
		e=xmu*d;
		fact2 = (fabs(e) < EPS ? 1.0 : sinh(e)/e);
		beschb(xmu,&gam1,&gam2,&gampl,&gammi);
		ff=2.0/M_PI*fact*(gam1*cosh(e)+gam2*fact2*d);
		e=exp(e);
		p=e/(gampl*M_PI);
		q=1.0/(e*M_PI*gammi);
		pimu2=0.5*pimu;
		fact3 = (fabs(pimu2) < EPS ? 1.0 : sin(pimu2)/pimu2);
		r=M_PI*pimu2*fact3*fact3;
		c=1.0;
		d = -x2*x2;
		sum=ff+r*q;
		sum1=p;
		for (i=1;i<=MAXIT;i++) {
			ff=(i*ff+p+q)/(i*i-xmu2);
			c *= (d/i);
			p /= (i-xmu);
			q /= (i+xmu);
			del=c*(ff+r*q);
			sum += del;
			del1=c*p-i*del;
			sum1 += del1;
			if (fabs(del) < (1.0+fabs(sum))*EPS) break;
		}
		if (i > MAXIT) 
		{
			fprintf( stderr, "%s: %s: bessy series failed to converge", __FILE__, __func__  );
			exit(-1);
		}
		rymu = -sum;
		ry1 = -sum1*xi2;
		rymup=xmu*xi*rymu-ry1;
		rjmu=w/(rymup-f*rymu);
	} else {
		a=0.25-xmu2;
		p = -0.5*xi;
		q=1.0;
		br=2.0*x;
		bi=2.0;
		fact=a*xi/(p*p+q*q);
		cr=br+q*fact;
		ci=bi+p*fact;
		den=br*br+bi*bi;
		dr=br/den;
		di = -bi/den;
		dlr=cr*dr-ci*di;
		dli=cr*di+ci*dr;
		temp=p*dlr-q*dli;
		q=p*dli+q*dlr;
		p=temp;
		for (i=2;i<=MAXIT;i++) {
			a += 2*(i-1);
			bi += 2.0;
			dr=a*dr+br;
			di=a*di+bi;
			if (fabs(dr)+fabs(di) < FPMIN) dr=FPMIN;
			fact=a/(cr*cr+ci*ci);
			cr=br+cr*fact;
			ci=bi-ci*fact;
			if (fabs(cr)+fabs(ci) < FPMIN) cr=FPMIN;
			den=dr*dr+di*di;
			dr /= den;
			di /= -den;
			dlr=cr*dr-ci*di;
			dli=cr*di+ci*dr;
			temp=p*dlr-q*dli;
			q=p*dli+q*dlr;
			p=temp;
			if (fabs(dlr-1.0)+fabs(dli) < EPS) break;
		}
		if (i > MAXIT) 
		{
			fprintf( stderr, "%s: %s: cf2 failed in bessjy\n", __FILE__, __func__ );
			exit(-1);
		}
		gam=(p-f)/q;
		rjmu=sqrt(w/((p-f)*gam+q));
		rjmu=SIGN(rjmu,rjl);
		rymu=rjmu*gam;
		rymup=rymu*(p+q/gam);
		ry1=xmu*xi*rymu-rymup;
	}
	fact=rjmu/rjl;
	*rj=rjl1*fact;
	*rjp=rjp1*fact;
	for (i=1;i<=nl;i++) {
		rytemp=(xmu+i)*xi2*ry1-rymu;
		rymu=ry1;
		ry1=rytemp;
	}
	*ry=rymu;
	*ryp=xnu*xi*rymu-ry1;
}
#undef EPS
#undef FPMIN
#undef MAXIT
#undef XMIN
