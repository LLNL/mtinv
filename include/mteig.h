#include "../include/nrutil.h"     /** numerical recipes **/
#include "../include/mt.h"         /** global datatype and structure declarations **/

#define DC        0
#define POS_CLVD  1
#define NEG_CLVD  2
#define POS_ISO   3
#define NEG_ISO   4
#define USER_MT   5
#define POS_CRACK 6
#define NEG_CRACK 7
#define POS_LVD   8
#define NEG_LVD   9

typedef struct {
        float u;
        float v[3];
        float lat, lon;
} Eigenvalue;

typedef struct {
	int nevec;
        float var_red;
        float lune_lat;
        float lune_lon;
        float k, T;
        float Mtotal;
        float pdc, pclvd, piso, pdev;

	float gcarc_dc;
	float gcarc_exp;

        float mxx, myy, mzz, mxy, mxz, myz;  /*** Results.mxx **/

	float e0, e1, e2; /*** from create lamb ***/

        Eigenvalue e[3]; /** fullMT **/

        Eigenvalue e_sort[3];  /***  e_sort[0].u/Mo, e_sort[0].lat, e_sort[0].lon ***/

        Tensor MT;       /* Results.MT.xx, MT.yy, ..., M.zz */

        MTdec FullMT;    /* Results.FullMT.eig[1].val, Results.FullMT.T.az */

        MTdec Maj;      /*** Results.Maj.eig[1].val, ***/
			/*** Results.Maj.T.az,  Results.Maj.P1.s ***/
			/*** Results.Maj.P1.d   Results.Maj.P1.r   ***/
} Results;
