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

/***
Keys:	Primary	mtid
Foreign	fpid, orid, commid
Data:	Descriptive	mt_type, srcfunc, algorithm, epi_fixed, time_fixed, depth_fixed
Measurement	mxx, myy, mzz, mxy, mxz, myz, smxx, smyy, smzz, smxy, smxz, smyz, mo, smo, momag, piso, pclvd, pdc, kiso, kclvd, epsilon, f_factor, var_red, residual, variance, nslpb, nrlpb, tmnlpb, nsmw, nrmw, tmnmw, nssurf, nrsurf, tmnsurf, half_duration
Administrative	auth, lddate
***/

typedef struct {
	long mtid;  /*** moment tensor identifier  NA=Null not allowed ***/
	long fpid;  /*** focal plane identifier  NA = -1           ***/
	long orid;  /*** origin identifier  NA = -1           ***/
	double mxx,myy,mzz,mxy,mxz,myz;  /*** moment tensor elements in cartesian coordinates NA Value = Not Allowed ***/
	double smxx,smyy,smzz,smxy,smxz,smyz;   /*** moment tensor element errors NA=-1 ***/
	char mt_type[16];      /*** moment tensor type = DEV, DC, FULL, ISO  NA='-' ***/
	double mo;             /*** scalar seismic moment N*m  NA=-1 ***/
	double smo;            /*** moment error NA=-1 ***/
	float momag;           /*** moment magnitude Mw = log10(mo)/1.5 - 10.73  NA=-999 ***/
	float piso;            /***  percent isotropic  NA = -1 ***/
	float pclvd;           /*** percent compensated linear vector dipole NA = -1 ***/
	float pdc;             /*** percent double couple NA = -1 ***/
	float kiso;            /*** source type isotropic   Mo(iso) / ( Mo(iso) + Mo(dev) )  NA = -999 ***/
	float kclvd;           /*** source type clvd (see Dr. Howard Patton, LLNL)    NA = -999 ***/
	float epsilon;         /***  percent CLVD/2 see Hudson et al. (1989) JGR  NA = -999  ***/
	float f_factor;        /*** F-factor for tectonic release. It is related to the ratio of the double ***/
			       /*** couple moment to the isotropic moment (Toksoz and Kehrer, 1972) NA = -999 ***/
	double var_red;        /*** percent variance reduction NA = 999 ***/
	double residual;       /*** total residual using L2 norm NA = -1  ***/
	double variance;       /*** variance or residual   NA = -1  ***/
	int nslpb;             /*** number of stations with lp body waves NA = -1  ***/
	int nrlpb;             /*** number of records with lp body waves NA = -1 ***/
	float tmnlpb;          /*** minimum period for body wave inversion NA = -1 ***/
	int nsmw;              /*** number of stations with mantle waves NA = -1 ***/
	int nrmw;              /*** number of records with mantle waves NA = -1 ***/
	float tmnmw;           /*** minimum periods for mantle wave inversion NA = -1 ***/
	int nssurf,nrsurf;     /*** number of stations and records with surface waves NA = -1 ***/
	float tmnsurf;         /*** minimum periods for surface waves NA = -1 ***/
	float half_duration;   /*** source time function half-duration NA = -1 ***/
        char srcfunc[24];      /*** source time function type srcfunc IN {triangle, boxcar, impluse} NA = '-'  ***/
	char algorithm[24];    /*** algorithm used NA = '-' ***/
        char epi_fixed[8];     /*** epicenter fixed flag epi_fixed in {f,r,g} NA = '-' ***/
	char time_fixed[8];    /*** Centroid time fixed in moment tensor inversion - f=free, r=restrained by program, g=restrained by geophysicist NA = '-' ***/
	char depth_fixed[8];   /*** Depth fixed in moment tensor inversion - f=free, r=restrained by program, g=restrained by geophysicist NA = '-' ***/
        char auth[48];         /*** author of source NA = '-' ***/
        long commid;           /*** comment identification NA = -1 ***/
	char lddate_stamp[24]; /*** string version of lddate YYYY/mm/DD,HH:MM:SS.SS ***/
	MyTime lddate;
} Moment;


typedef struct {
	long fpid;      /*** focal plane identifier NA Value not allowed***/
	long orid;      /*** origin id NA Value = -1 ***/
	char fp_type[16];  /*** focal plane type FM MT PT, '-' ***/
	char decomp_type[16];   /*** decomposition type BESTDC, MAJOR, MINOR, PUERDC, FULLMT, MAJDC, MINDC, INTDC, MAJCLVD, MINCLVD, INTCLVD, '-' ***/
	float strike1,dip1,rake1;   /*** strike, dip and rake of nodal plane 1 NA Value=-999 ***/
	float strike2,dip2,rake2;   /*** strike, dip and rake of nodal plane 2 NA Value=-999 ***/
	float sstrike,sdip,srake;   /*** strike, dip and rake error NA Value=-1 ***/
        int primary_plane;           /* 1,2 NA=-1 */
        double eval1;  /* largest positive Eigenvalue NA = 0 N*m **/
        float az1, pl1;             /* azimuth and plunge for largest positive eigenvalue NA=-999*/
	double eval2;  /* largest negative Eigenvalue NA = 0 N*m **/
	float az2, pl2;		    /* azimuth and plunge for largest negative eigenvalue NA=-999*/
	double eval3;  /* smallest Eigenvalue NA = 0 N*m **/
	float az3, pl3;             /* azimuth and plunge for largest eigenvalue NA=-999*/
        double dcmo;    /* pure double couple moment N*m NA=-1 */
	double isomo;   /* isotropic moment  N*m NA=-1 */
	double devmo;   /* deviatoric moment N*m NA=-1 */
        char auth[24];  /* author, source of data */
        long commid;       /*** comment identification NA = -1 ***/
        char lddate_stamp[24];   /*** string version of lddate YYYY/mm/DD,HH:MM:SS.SS ***/
	MyTime lddate;
} Focal_Plane;

/***
Keys:	Primary	fpid
Foreign	orid, commid
Data:	Descriptive	fp_type, decomp_type
Measurement	strike1, dip1, rake1, strike2, dip2, rake2, sstrike, sdip, srake, primary_plane, eval1, az1, pl1, eval2, az2, pl2, eval3, az3, pl3, dcmo, devmo, isomo
Administrative	auth, lddate
***/


/***
The moment_version table

#	COLUMN	STORAGE TYPE	EXT FMT	CHAR POS	UNIT	NA VALUE	DESCRIPTION
1	evid	number(9)	i9	1-9	-	Not Allowed	event identifier
2	pref_mtid	number(9)	i9	11-19	-	Not Allowed	preferred moment tensor
3	pref_fpid	number(9)	i9	21-29	-	-1	preferred focal plane
4	auth	varchar2(20)	a20	31-50	-	-	author, source of data
5	commid	number(9)	i9	52-60	-	-1	comment identifier
6	lddate	date	a17:YY/MM/DD HH24:MI:SS	62-78	-	Not Allowed	load date

Keys:	Foreign	pref_mtid, pref_fpid, commid
Data:	Administrative	auth, lddate
***/

typedef struct {
	long evid;         /*** event identification ***/
	long perf_mtid;     /*** perferred mtid ***/
	long perf_fpid;     /*** perferred fpid ***/
	char auth[32];      /*** author, agency source of solution ***/
	long commid;       /*** comment identification NA = -1 ***/
        char lddate_stamp[24];   /*** string version of lddate YYYY/mm/DD,HH:MM:SS.SS ***/
        MyTime lddate;
} Moment_Version;


typedef struct {
	long mtdataid;
	long mtid;
	char sta[16];
	char net[8];
	float distkm, azimuth;
	char used[8];      /*** defining = 'D'   not-defining = 'N'   NA Value  = '-' ***/
	char quality[64];  /*** comment ***/
	char auth[48];        /*** author of source NA = '-' ***/
        long commid;          /*** comment identification NA = -1 ***/
        char lddate_stamp[24];  /*** string version of lddate YYYY/mm/DD,HH:MM:SS.SS ***/
        MyTime lddate;
} MT_Data;


typedef struct {
	long mtwfsegid;
	long mtdataid;
	char wfsegtype[16]; /*** Waveform segment type = FULL, RegPnl, TeleP, TeleS, Rayl, Love NA Value = '-'***/
	char sta[16];
	char net[8];
	char chan[8];
	long chanid;      
	double start_time;  /*** epoch time for start time of window NA Value = not allowed ***/
	double end_time;    /*** epoch time for end time of window NA Value = not allowed ***/
	long mtfilterid;
	long mtvmodid;
	float redvel;       /*** reduction velocity in km/sec  NA = -999 ***/
	float tshift;       /*** time shift used in seconds  NA = -999 ***/
	float ampweight;        /*** NA = -1 ***/
	char distnorm[8];      /*** y = yes    n = no NA = '-' ***/
	float r0;              /*** NA = -1 ***/
	float rayparam;        /*** NA = -1 ***/
	float distdeg;         
	float distkm;
	float az;
	float baz;
	float interpsamprate;     /*** NA = -1 ***/
	char used[8];      /*** defining = 'D'   not-defining = 'N'   NA Value  = '-' ***/
	char quality[64];  /*** comment ***/
	char algor[32];    /*** method used for synthetics  FK, FD, FE, RT, UNK   NA = '-' ***/
	char lddate_stamp[24];  /*** string version of lddate YYYY/mm/DD,HH:MM:SS.SS ***/
        MyTime lddate;
} MT_Waveform_Segment;

typedef struct {
	long mtfilterid;        /*** NA Value = not null ***/
	char filtertype[16];    /*** BP,BR,LP,HP   NA Value = not null  ***/
	char filterdesign[32];  /*** Butterworth, Bessel, Chebyshev1, Chebyshev2 NA Value = not null ***/
	int npole;              /*** number of poles NA Value = not null ***/
	float lcorner;          /*** low frequency corner NA Value = -1 ***/
	float hcorner;          /*** high frequency corner NA Value = -1 ***/
	int npass;              /*** number of passes  { 1, 2 }  NA Value = not null ***/
	float tranbw;           /*** Transition bandwidth  NA Value = -1  ***/
	float attenfactor;      /*** attenuation factor NA Value = -1 ***/
} MT_Filter;


typedef struct {
	float lat, lon, depth;
	double time;
	long orid, evid;
	int jdate, nass, ndef, ndp, grn, srn;
	char etype[8];
	float depdp;
	char dtype[8];
	float mb;
	long mbid;
	float ms;
	long msid;
	float ml;
	long mlid;
	char algorithm[32];
	long commid;
	char lddate_stamp[24];  /*** string version of lddate YYYY/mm/DD,HH:MM:SS.SS ***/
	MyTime lddate;
	MyTime ot;
} Origin;
