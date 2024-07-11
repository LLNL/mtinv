#include "sac.h"
#include "mytime.h"
                                                                                                                                       
typedef struct {
        Sac_Header s;
        float dt;
        int nt;
        MyTime beg;
        MyTime end;
        MyTime ot;
        char filename[256];
        float *data;
} SacFile;
