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

#include <stdlib.h>
#include <math.h>

/*** from numerical recipes (https://numerical.recipes) ***/

/*******************************************************************************
Eigenvalue solvers, tred2 and tqli, from "Numerical Recipes in C" (Cambridge
Univ. Press) by W.H. Press, S.A. Teukolsky, W.T. Vetterling, and B.P. Flannery
*******************************************************************************/

void tred2( float **a, int n, float d[], float e[] )
{
        int l,k,j,i;
        float scale,hh,h,g,f;

        for (i=n;i>=2;i--) 
	{
                l=i-1;
                h=scale=0.0;
                if (l > 1) {
                        for (k=1;k<=l;k++)
                                scale += fabs(a[i][k]);
                        if (scale == 0.0)
                                e[i]=a[i][l];
                        else {
                                for (k=1;k<=l;k++) {
                                        a[i][k] /= scale;
                                        h += a[i][k]*a[i][k];
                                }
                                f=a[i][l];
                                g = f>0 ? -sqrt(h) : sqrt(h);
                                e[i]=scale*g;
                                h -= f*g;
                                a[i][l]=f-g;
                                f=0.0;
                                for (j=1;j<=l;j++) {
                                /* Next statement can be omitted if eigenvectors not wanted */
                                        a[j][i]=a[i][j]/h;
                                        g=0.0;
                                        for (k=1;k<=j;k++)
                                                g += a[j][k]*a[i][k];
                                        for (k=j+1;k<=l;k++)
                                                g += a[k][j]*a[i][k];
                                        e[j]=g/h;
                                        f += e[j]*a[i][j];
                                }
                                hh=f/(h+h);
                                for (j=1;j<=l;j++) {
                                        f=a[i][j];
                                        e[j]=g=e[j]-hh*f;
                                        for (k=1;k<=j;k++)
                                                a[j][k] -= (f*e[k]+g*a[i][k]);
                                }
                        }
                } else
                        e[i]=a[i][l];
                d[i]=h;
        }
        /* Next statement can be omitted if eigenvectors not wanted */
        d[1]=0.0;
        e[1]=0.0;
        /* Contents of this loop can be omitted if eigenvectors not
                        wanted except for statement d[i]=a[i][i]; */
        for (i=1;i<=n;i++) {
                l=i-1;
                if (d[i]) {
                        for (j=1;j<=l;j++) {
                                g=0.0;
                                for (k=1;k<=l;k++)
                                        g += a[i][k]*a[k][j];
                                for (k=1;k<=l;k++)
                                        a[k][j] -= g*a[k][i];
                        }
                }
                d[i]=a[i][i];
                a[i][i]=1.0;
                for (j=1;j<=l;j++) a[j][i]=a[i][j]=0.0;
        }
}
