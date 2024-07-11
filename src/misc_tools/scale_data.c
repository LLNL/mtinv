/**** 
* the name of the original version fmul() conflicts 
* with some intrinsic C, compiler call maybe 
****/

/**** renamed to scale_data(), returns vector multiplied by scaler factor ****/

void scale_data( float *x, int n, float a )
{
	int i;
	for( i = 0; i < n; i++ ) x[i] *= a;
}
