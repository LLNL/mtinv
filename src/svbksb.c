#include "../include/nrutil.h"

/*** from numerical recipes (https://numerical.recipes) ***/
/*** does singlular value decomposition back-substition for linear inversions ***/

void svbksb( float **u, float *w, float **v, int m, int n, float *b, float *x )
{
	int jj,j,i;
	float s,*tmp;
	float *vector( int, int );
	void free_vector( float *, int, int );

	tmp=vector(1,n);
	for (j=1;j<=n;j++) 	
	{
		s=0.0;
		if (w[j]) 
		{
			for (i=1;i<=m;i++) 
				s += u[i][j]*b[i];
			s /= w[j];
		}
		tmp[j]=s;
	}

	for (j=1;j<=n;j++) 	
	{
		s=0.0;
		for (jj=1;jj<=n;jj++) 
			s += v[j][jj]*tmp[jj];
		x[j]=s;
	}
	free_vector(tmp,1,n);
}
