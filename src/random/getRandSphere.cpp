#include <random>
#include <cmath>
using namespace std;

/***
 https://en.wikipedia.org/wiki/Mersenne_Twister - A pseudo random number generator (PRNG)
   a high quality random number generator

See: https://channel9.msdn.com/Events/GoingNative/2013/rand-Considered-Harmful
     https://stackoverflow.com/questions/27759891/how-to-run-mersenne-twister-inside-a-function
***/

extern "C" void getRandSphere( float& e1, float& e2, float& e3 );

void getRandSphere( float& e1, float& e2, float& e3 )
{
	static float twopi = 6.283185307179586;
	float theta, u;

	static std::random_device randDev1;
	static std::mt19937 twister1(randDev1());

	static std::random_device randDev2;
        static std::mt19937 twister2(randDev2());

	static std::uniform_real_distribution<float> dist_theta;
	static std::uniform_real_distribution<float> dist_u;

	dist_theta.param( std::uniform_real_distribution<float>::param_type( 0.0, twopi ) );

	dist_u.param( std::uniform_real_distribution<float>::param_type( -1.0, +1.0 ) );

	theta = dist_theta(twister1);
	u     = dist_u(twister2);
	
	e1 = sqrt( 1 - (u*u)) * cos(theta);
	e2 = sqrt( 1 - (u*u)) * sin(theta);
	e3 = u;
}

/*
int main()
{
	float x;
	int i;
	for( int i = 0; i < 10; i++ )
	{
		x = getRand( 0.0, 1.0 );
		std::cout << x << " ";
	}
	std::cout << std::endl;
}
*/
