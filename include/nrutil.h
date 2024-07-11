float *vector( int nl, int nh );
float **matrix( int nrl, int nrh, int ncl, int nch );
void free_vector( float *v, int nl, int nh );
void free_matrix( float **m, int nrl, int nrh, int ncl, int nch );
void nrerror( char *error_text );

/*
float **convert_matrix( );
double *dvector();
double **dmatrix();
int *ivector();
int **imatrix();
float **submatrix();
void free_dvector();
void free_ivector();
void free_dmatrix();
void free_imatrix();
void free_submatrix();
void free_convert_matrix();
float ***f3tensor();
void free_f3tensor();
*/
