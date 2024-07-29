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

#include "../include/mt_version.h"
#include "../include/sac.h"
#include "../include/mytime.h"
#include "../include/peak2peak.h"

/****************************************************************************************/
/*** in ../include/mt.h : static float base_moment = 1.2445146117713818e+16;          ***/
/* Mo = math.pow( 10.0, 1.5*(Mw+10.73) ) = 1.2445146117713818e+16; Reference Mw = 0.0 ***/
/* Typically base_moment is 10^20 dyne*cm  so conversion factor is 8035.2612218561935 ***/
/****************************************************************************************/
	static float base_moment = 1.2445146117713818e+16;

typedef struct { float x, y; } Vector;
typedef struct { float s, d, r; } Plane;
typedef struct { 
	float xx,xy,xz;
	float yx,yy,yz;
	float zx,zy,zz;
} Tensor;

typedef struct {
	Vector pt;
	int type;
	char ktype[3];
	float az,pl,ev;
	float rad,str,dip;
} Poles;

typedef struct {
	float val;
	float vec[4];
	float x,y,z;
} Vector3;

typedef struct {
	float xx,xy,xz;
	float yx,yy,yz;
	float zx,zy,zz;
	Tensor T;
	float moment, Mw, abcassa;
	int expon;
	float rr,tt,ff,rt,rf,tf;
	float mt[4][4];
} MomentTensor;

typedef struct {
	Vector3 eig[4];
	Poles T, P, B;
	Plane P1, P2;
	Tensor mt;
} MTdec;

typedef struct { float r,s,d; } Polar;

typedef struct {
	int np;
	Vector *p;
	Polar *v;
	int first;
	int inside;
	int Taxis;
	int Paxis;
	int Baxis;
} NodalPlane;

#define MAX_MODEL_LAYERS 1024
typedef struct {
        char modfile[256], modpath[256];
        int nlay;
	int maxlay;
        float thick[MAX_MODEL_LAYERS];
	float  ztop[MAX_MODEL_LAYERS];
	float    vp[MAX_MODEL_LAYERS];
	float    vs[MAX_MODEL_LAYERS];
	float    qa[MAX_MODEL_LAYERS];
	float    qb[MAX_MODEL_LAYERS];
	float   rho[MAX_MODEL_LAYERS];
	float sigma[MAX_MODEL_LAYERS];
} VelMod;

/**************************************************************************************************/
/*** change array size from 2048 to 4096 ***/
/*** dont forget to change greensf.f     ***/
/*** same as fkstrain.h/fkstrain.c       ***/
/**************************************************************************************************/

typedef struct {

	/*** the original 3-fundamental faulting orientations + 1-isotropic Gf ***/
	/*** 3-component displacement r-radial z-vertical t-transverse ***/

        float rss[4096], rds[4096], rdd[4096], rep[4096];
	float zss[4096], zds[4096], zdd[4096], zep[4096];
	float tss[4096], tds[4096];  /*** tdd=0 and tep=0 ***/

	/*** the 3-fundamental faulting orientations + 1-isotropic Gf ***/
	/*** 3 components of rotation w3-rotation about vertical axes, ***/
	/***      w1-rot about E-W, w2-rot about N-S axis ***/

	float w3ss[4096], w3ds[4096], w3dd[4096], w3ex[4096];
        float w2ss[4096], w2ds[4096], w2dd[4096], w2ex[4096];
        float w1ss[4096], w1ds[4096], w1dd[4096], w1ex[4096];

} Greens_Function;

/*******************************************************************************************************/
/*** new GLIB filename format net.sta.loc.model.glib             ***/
/*** new data filename format net.sta.loc.[v/d].ista#.data       ***/
/*** new ginv filename format net.sta.loc.model.[v/d].#ista.ginv ***/

typedef struct {
				   /*sta=stnm*/
	char filename[256], net[8], stnm[8], loc[8];
	float stla, stlo, stel, evla, evlo, evdp;
	float rdist, az, baz;
        float t0, dt, twin, fmax, damp, eps, smin, rigidity;
	float redv, ts0, tstart, tend;

/** newly added 2010/11/27 G. Ichinose see rayp_subs.c ***/

	float Ptakeoff, Prayparameter, Pttime, Praybottom;

	int kmax, nt;
        VelMod v;
        Greens_Function g;
	float *rad, *tra, *ver;
} Greens;

/*******************************************************************************************************/
/*** wrapper for sac header and floating point waveform data with some additional info ***/
/*******************************************************************************************************/

typedef struct {
	int id;
	char filename[256];
	char sacpzfile[256];

        MyTime ot,a,t0,t1,t2,t3,t4,t5,t6,t7,t8,t9;
        MyTime ref, beg, end;

	Phase pha[2];
        float P2P_snr;

	float data_safe[8192];

	Sac_Header s;
        float  *data; /*** cannot use fixed array because it is used to load raw data in sacdata2inv.c ***/
} Sac_File;

/*******************************************************************************************************/
/***
	EventInfo Data Structure - holds event-origin, source MT, station-inversion fit information 
		Storage of the SAC formated waveform and meta-data, raw, processed and temp storage for plotting synthetics
***/
/*******************************************************************************************************/

/*** new GLIB filename format net.sta.loc.model.glib             ***/
/*** new data filename format net.sta.loc.[v/d].ista#.data       ***/
/*** new ginv filename format net.sta.loc.model.[v/d].#ista.ginv ***/

typedef struct {
        char net[8], stnm[8], loc[8], modfile[256];
        char data_filename[256], glib_filename[256], ginv_filename[256];
	char comment[256];

/*** holds space for future use:  ***/
	/*** wavetype:  "" or "Surf/Pnl" (typical 3C ZRT waveform data, default) ***/
	/*** wavetype: "Rotational"  (new case, not typical, U,W,V 3-C rotational data Array-Derived or 3C BB rot-sensor ***/
                        
	char wavetype[32];  

	long evid,orid;
	char dbuser[32],dbsid[32];
	float mb, ms, mbmle, msvmax, mwcoda;
	int ndef;
	double time;
	float lat, lon, depth;
	int grn;
	char grname[128];
	char grname_directory[128];

        float my_z;
        float lf,hf,dt,tr,tt;
        int npole,npass;
        int nt, iused, ienvelope;
        float trbndw,a;
	float ot_shift, ts0, tstart, tend, redv;
        float str,dip,rak,Mw,Mo;
	float Mxx, Mxy, Mxz, Myy, Myz, Mzz;
        float stla,stlo,evla,evlo,evdp;
	float rdistkm, az, baz;

/*** every station will have a total response of all comp in GS ***/
	float vred,xcor,tlag;
	int ilag;

/*** added for variance reduction by each station and each components ***/
/*** see sol[iz].var_red for event values of variance reduction ***/
	float vred_sta, vred_zcmp, vred_rcmp, vred_tcmp;

/*** added for cross_correlation.c ***/
	float zxcor,rxcor,txcor;
	float ztlag, rtlag, ttlag;
	int izlag, irlag, itlag;

	int grd_mo_type; /*** 0=disp 1=vel ***/
	float mul_factor;  /*** on-the-fly scaling of all the data ***/
	float weight;
	float time_shift_all; /*** this gets updated with new cross correlation shift ***/
	float time_shift_user;  /*** this is read in and does not get updated ***/

	/*** mini Greens ***/
	float rdist;

	MyTime ot, ot_orig;

	/*** putting Greens functions in Mxy format here ***/
        float rtzGxy[24][4096];

	/* Sac_File r,t; */
	Sac_File ns, ew, z;            /*** these store translational (NS,EW,Z) data ***/
	Sac_File syn_r, syn_t, syn_z;  /*** these store synthetics ***/

	/** we are going to shoe-horn in the rotational with the translational for now ***/
	/* Sac_File w1, w2, w3; */           /*** rotational data (U,V,W) ***/

} EventInfo;

typedef struct {
        char id[8];
        float lat, lon, elev;
        char chan[256];
        char net[8];
        char description[512];
        MyTime beg;
        MyTime end;
        float dist, az, baz;
} Station;

typedef struct {
	float moment_tensor[4][4];
	float var_red, l2norm_error, total_fitness1, total_fitness2;
	float mw, mo;
	float abcassa;
	int exponent;
	/*** normalized moment tensor in cartesian cooridinates ***/
	float mxx, myy, mzz, mxy, mxz, myz;
	double dmoment;
	float stk0,dip0,rak0;
	float stk1,dip1,rak1;
	float evlo, evla, evdp, ot;
	int mt_type;
	float epsilon;
	float k, T;
	float fIso, fiso, fclvd;
	float f_factor;
	float lune_lat, lune_lon;

	/*** normalized moment tensor in spherical coordinates ***/
	float mrr, mtt,mff,mrt,mrf,mtf;
	float dmrr,dmtt,dmff,dmrt,dmrf,dmtf;
	float smxx,smyy,smzz,smxy,smxz,smyz;
	float Mdc,Mclvd,Miso,Mdev,Mtotal;
	float PDC,PCLVD,PISO,PDEV;
	int nsta;
	float maxgap;
	float dminkm;
	float taz,tdp,tev,paz,pdp,pev,baz,bdp,bev;
	float stkt,stkp,stkn,plnt,plnp,plnn;
	Tensor MT;
	MomentTensor M;
	MTdec FullMT;
	MTdec Dev;
	MTdec Maj;
	MTdec Min;
} Solution;

/****************************************************/
/*** Dziewonski et al., 1983                      ***/
/*** Moment tensor components R=rho T=theta F=phi ***/
/*** MXX MXY MXZ     MTT -MTF  MTR                ***/
/*** MYX MYY MYZ -> -MFT  MFF -MFR                ***/
/*** MZX MZY MZZ     MRT -MRF  MRR                ***/
/****************************************************/

#define DISPLACEMENT 0
#define VELOCITY 1
#define DEVIATORIC 0
#define FULL_MT 1
#define EXPLOSION 3
#define FORCE_EXPLOSION 1
#define DEVIATORIC_MOMENT_TENSOR 5
#define FULL_MOMENT_TENSOR 6

#define FULLMT  0
#define MAJORDC 1
#define MINORDC 2

#ifndef PORTRAIT
#define PORTRAIT 1
#endif

#ifndef LANDSCAPE
#define LANDSCAPE 0
#endif

#define SCREEN_PRINT 0 /*** gs -dEPSCrop , ps2pdf -dEPSCrop , ps2raster -A ***/
#define PAPER_PRINT  1  /*** ps2pdf or lpr ***/

/*** these for grid serach routines (GS) ***/
typedef struct {
	float s,d,r;
	int id, iz;
	float Mo,Mw,z;
	Tensor M;
	float vred,xcor;
	float Mdc,Mclvd,Miso,Mtotal;
	float pdc,pclvd,piso;
} SDRvector;

typedef struct {
	float str0, str1, dstr;
	float dip0, dip1, ddip;
	float rak0, rak1, drak;
	float wdc0, wdc1, dwdc;
	float Mw0,  Mw1,  dMw;
	int nstr,ndip,nrak,nwdc,nMw;
} SDRGrid;

/*** added 08/07/2008 to swap information about the Greens function depths ***/
typedef struct {
	int type; /* type=0 z=single depth, type=1 zrange=zmin,zinc,zmax, type=2 multiple irregular */
	int nz;
	float zmin, zinc, zmax;
	float *zvec;
} Depth_Info;

/*** needed by Greens_subs.c and glib2inv.c ***/

typedef struct {
        int ista;
        int nz;
        float *z;
} DepthVector;
