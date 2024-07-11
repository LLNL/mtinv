#include <random>
using namespace std;

/***
 https://en.wikipedia.org/wiki/Mersenne_Twister - A pseudo random number generator (PRNG)
   a high quality random number generator

See: https://channel9.msdn.com/Events/GoingNative/2013/rand-Considered-Harmful
     https://stackoverflow.com/questions/27759891/how-to-run-mersenne-twister-inside-a-function
***/

extern "C" float getRandFloat( const float A, const float B );

float getRandFloat( const float A, const float B )
{
	static std::random_device randDev;
	static std::mt19937 twister(randDev());
	static std::uniform_real_distribution<float> dist;

	dist.param( std::uniform_real_distribution<float>::param_type(A,B));
	return dist(twister);
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
