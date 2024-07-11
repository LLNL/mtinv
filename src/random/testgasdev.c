#include <stdio.h>
#include <stdlib.h>

int main( int ac, char **av )
{
	int i;
	float x, y;

	int n = 100000;
	long iseed = 1;
	float mean = 1;	
	float stdev = 0.5;

	float gasdev( long *iseed );
	int setpar(), getpar(), mstpar();
	void endpar();

	setpar(ac,av);
	getpar( "n", "d", &n );
	getpar( "seed", "d", &iseed );
	getpar( "mean", "f", &mean );
	getpar( "stdev", "f", &stdev );
	endpar();

	for ( i = 0; i < n; i++ )
	{

	/* r = ((double) rand() / (RAND_MAX)); */

		x = gasdev( &iseed );
		y = ( x * stdev ) + mean;
		fprintf( stdout, "%d %g %g\n", i, y, x  );
	}
	exit(0);
}
