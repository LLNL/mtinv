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

/*******************************************************************************
Eigenvalue solvers, tred2 and tqli, from "Numerical Recipes in C" (Cambridge
Univ. Press) by W.H. Press, S.A. Teukolsky, W.T. Vetterling, and B.P. Flannery
*******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define SIGN(A,B)      ((B) < 0 ? -fabs(A) : fabs(A))

void tqli(float d[], float e[], int n, float **z)
{
        int m,l,iter,i,k;
        float s,r,p,g,f,dd,c,b;

        for (i=2;i<=n;i++) e[i-1]=e[i];
        e[n]=0.0;
        for (l=1;l<=n;l++) {
                iter=0;
                do {
                        for (m=l;m<=n-1;m++) {
                                dd=fabs(d[m])+fabs(d[m+1]);
                                if (fabs(e[m])+dd == dd) break;
                        }
                        if (m != l) {
                                if (iter++ == 30)
				{
                                        fprintf(stderr, "%s: %s: Too many iterations in TQLI\n", __FILE__, __func__ );
					fprintf(stdout, "%s: %s: Too many iterations in TQLI\n", __FILE__, __func__ );

                                        exit(-1);
                                }
                                g=(d[l+1]-d[l])/(2.0*e[l]);
                                r=sqrt((g*g)+1.0);
                                g=d[m]-d[l]+e[l]/(g+SIGN(r,g));
                                s=c=1.0;
                                p=0.0;
                                for (i=m-1;i>=l;i--) {
                                        f=s*e[i];
                                        b=c*e[i];
                                        if (fabs(f) >= fabs(g)) {
                                                c=g/f;
                                                r=sqrt((c*c)+1.0);
                                                e[i+1]=f*r;
                                                c *= (s=1.0/r);
                                        } else {
                                                s=f/g;
                                                r=sqrt((s*s)+1.0);
                                                e[i+1]=g*r;
                                                s *= (c=1.0/r);
                                        }
                                        g=d[i+1]-p;
                                        r=(d[i]-g)*s+2.0*c*b;
                                        p=s*r;
                                        d[i+1]=g+p;
                                        g=c*r-b;
                                        /* Next loop can be omitted if eigenvectors not wanted */
                                        for (k=1;k<=n;k++) {
                                                f=z[k][i+1];
                                                z[k][i+1]=s*z[k][i]+c*f;
                                                z[k][i]=c*z[k][i]-s*f;
                                        }
                                }
                                d[l]=d[l]-p;
                                e[l]=g;
                                e[m]=0.0;
                        }
                } while (m != l);
        }
}
