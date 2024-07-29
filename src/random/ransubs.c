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

float gasdev(int *idum)
{
	static int iset=0;
	static float gset;
	float fac, rsq, v1, v2;
	float ran1(int *idum);

	if( *idum < 0 ) iset = 0; /*** reinitialize ***/

	if( iset == 0 )  /*** we dont have an extra deviate handy ***/
	{
		do {
			/*** pick 2 uniform numbers in the sq extending from -1 to +1 in
				each direction, see if they are in the unit circle ***/
			v1=2.0*ran1(idum)-1.0;
			v2=2.0*ran1(idum)-1.0;
			rsq = v1*v1 + v2*v2;
		}
		while ( rsq >= 1.0  || rsq == 0.0 );
				/*** try again if they are no in unit circle ***/

		/*** now make the box-muller transform to get two normal deviates 
			return one and save the other for next time ***/

		fac=sqrt(-2.0*log(rsq)/rsq);
		gset=v1*fac;
		iset=1;        /*** set flag ***/
		return v2*fac;
	} else {
		iset=0;
		return gset;
	}
}

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB) 
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

/*** minimal random number generator of Park and Miller with Bays
 Durham shuffle and added safe guards, returns a uniform random 
    deviate between 0 and 1 ***/

float ran1( int *idum )
{
	int j;
	long k;
	static long iy = 0;
	static long iv[NTAB];
	float temp;

	if ( *idum < 0 || !iy ) /*** initialize ***/
	{
	/*** Be sure to prevent idum = 0. ***/
		if( -(*idum) < 1 )
			*idum = 1;
		else
			*idum = -(*idum);

	/*** Load the shuffle table (after 8 warm-ups). ***/
		for( j = NTAB+7; j >= 0; j-- ) 
		{
			k = (*idum)/IQ;
			*idum = IA*(*idum-k*IQ)-IR*k;
			if( *idum < 0 ) *idum += IM;
			if (j < NTAB) iv[j] = *idum;
		}
		iy=iv[0];
	}

/*** Start here when not initializing. Compute idum=(IA*idum) % IM without over- ***/
/*** flows by Schrage’s method. Will be in the range 0..NTAB-1. Output previously ***/
/*** stored value and refill the  shuffle table. Because users don’t expect endpoint values. ***/
	
	k = (*idum)/IQ;
	*idum = IA*(*idum-k*IQ)-IR*k;
	if( *idum < 0 ) *idum += IM;
	j     = iy / NDIV;
	iy    = iv[j];
	iv[j] = *idum;
	if( ( temp = AM*iy ) > RNMX ) 
		return RNMX; 
	else
		return temp;
}

#undef IA
#undef IM
#undef AM
#undef IQ
#undef IR
#undef NTAB
#undef NDIV
#undef EPS
#undef RNMX
