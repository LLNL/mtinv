/*** from numerical recipes (https://numerical.recipes) ***/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
        Spherical Bessel Functions
For integer n, spherical Bessel functions are defined by 
 j^n(x) = integral π J^(n+(1/2))(x)
 y^n(x) = integral π Y^(n+(1/2))(x)
See equation (6.7.47) in NR

   They can be evaluated by a call to bessjy, and the derivatives can safely be found from the derivatives of equation (6.7.47).
 Note that in the continued fraction CF2 in (6.7.3) just the first term survives for ν = 1/2. Thus one can make a very simple
 algorithm for spherical Bessel functions along the lines of bessjy by always recursing jn down to n = 0, setting p and q from
 the first term in CF2, and then recursing yn up. No special series is required near x = 0. However, bessjy is already so
 efficient that we have not bothered to provide an independent routine for spherical Bessels.

*/

void sphbes( int n, float x, float *sj, float *sy, float *sjp, float *syp )
{
	static float RTPIO2 = 1.2533141;
	void bessjy(float x, float xnu, float *rj, float *ry, float *rjp, float *ryp);

	float factor,order,rj,rjp,ry,ryp;

	if (n < 0 || x <= 0.0) 
	{
		fprintf ( stderr, "%s: %s: bad arguments in sphbes\n", __FILE__, __func__ );
		exit(-1);
	}

	order=n+0.5;
	bessjy(x,order,&rj,&ry,&rjp,&ryp);
	factor=RTPIO2/sqrt(x);
	*sj=factor*rj;
	*sy=factor*ry;
	*sjp=factor*rjp-(*sj)/(2.0*x);
	*syp=factor*ryp-(*sy)/(2.0*x);
}
