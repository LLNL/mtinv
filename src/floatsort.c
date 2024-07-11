
/*** simple sorter, used by eig2lune.c, find_best_shift.c, mt2lune.c, mtinv_subs.c ***/

void floatsort( float *x, int n )
{
        int k,j,m;
        float hold;
        n=n+1;
        for( k=1; k <=n-2; k++ )
        {
                m = k;
                for( j = k + 1; j <= n - 1; j++ )
                {
                        if( x[j] < x[m] ) m = j;
                }
                hold = x[m];
                x[m] = x[k];
                x[k] = hold;
        }
        return;
}
