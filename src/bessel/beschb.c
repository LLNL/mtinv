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


/***
  Evaluates Γ1 and Γ2 by Chebyshev expansion for |x| ≤ 1/2. Also returns 1/Γ(1 + x) and
	1/Γ(1 − x). If converting to double precision, set NUSE1 = 7, NUSE2 = 8.
***/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void beschb(double x, double *gam1, double *gam2, double *gampl, double *gammi)
{
	float chebev(float a, float b, float c[], int m, float x);
	static int NUSE1 = 5;
	static int NUSE2 = 5;
	float xx;

	static float c1[] = {
		-1.142022680371168e0,6.5165112670737e-3,
		3.087090173086e-4,-3.4706269649e-6,6.9437664e-9,
		3.67795e-11,-1.356e-13};

	static float c2[] = {
		1.843740587300905e0,-7.68528408447867e-2,
		1.2719271366546e-3,-4.9717367042e-6,-3.31261198e-8,
		2.423096e-10,-1.702e-13,-1.49e-15};

	xx=8.0*x*x-1.0;
	*gam1=chebev(-1.0,1.0,c1,NUSE1,xx);
	*gam2=chebev(-1.0,1.0,c2,NUSE2,xx);
	*gampl= *gam2-x*(*gam1);
	*gammi= *gam2+x*(*gam1);
}

float chebev(float a, float b, float c[], int m, float x)
{
	float d=0.0,dd=0.0,sv,y,y2;
	int j;

	if ((x-a)*(x-b) > 0.0) 
	{
	  fprintf( stderr, "%s: %s: x not in range in routine chebev\n", __FILE__, __func__ );
	  exit(-1);
	}
	y2=2.0*(y=(2.0*x-a-b)/(b-a));
	for (j=m-1;j>=1;j--) {
		sv=d;
		d=y2*d-dd+c[j];
		dd=sv;
	}
	return y*d-dd+0.5*c[0];
}
