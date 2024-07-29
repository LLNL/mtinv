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

/*** SEISMIC ANALYSIS CODE is Copyright 2019 LLNL/LLNS under Contract DE-AC52-07NA27344 ***/

typedef struct {
	float delta,  depmin, depmax, scale,  odelta;
	float b,      e,      o,      a,      fmt;
	float t0,     t1,     t2,     t3,     t4;
	float t5,     t6,     t7,     t8,     t9;
	float f,      resp0,  resp1,  resp2,  resp3;
	float resp4,  resp5,  resp6,  resp7,  resp8;
	float resp9,  stla,   stlo,   stel,   stdp;
	float evla,   evlo,   evel,   evdp,   mag;
	float user0,  user1,  user2,  user3,  user4;
	float user5,  user6,  user7,  user8,  user9;
	float dist,   az,     baz,    gcarc,  sb;
	float sdelta, depmen, cmpaz,  cmpinc, xmin;
	float xmax,   ymin,   ymax,   adjtim, fhdr65;
	float fhdr66, fhdr67, fhdr68, fhdr69, fhdr70;
	int  nzyear, nzjday, nzhour, nzmin,  nzsec;
	int  nzmsec, nvhdr,  norid,  nevid,  npts;
	int  nsnpts, nwfid,  nxsize, nysize, nhdr15;
	int  iftype, idep,   iztype, ihdr4,  iinst;
	int  istreg, ievreg, ievtyp, iqual,  isynth;
	int  imagtyp,imagsrc,ihdr13, ihdr14, ihdr15;
	int  ihdr16, ihdr17, ihdr18, ihdr19, ihdr20;
	int  leven,  lpspol, lovrok, lcalda, lhdr5;
	char  kstnm[8],  kevnm[16], khole[8],  ko[8],     ka[8];
	char  kt0[8],    kt1[8],    kt2[8],    kt3[8],    kt4[8];
	char  kt5[8],    kt6[8],    kt7[8],    kt8[8],    kt9[8];
	char  kf[8],     kuser0[8], kuser1[8], kuser2[8];
	char  kcmpnm[8], knetwk[8], kdatrd[8], kinst[8];
} Sac_Header;

static Sac_Header sac_null = {
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345., -12345., -12345., -12345., -12345.,
-12345, -12345, -12345, -12345, -12345,
-12345, -12345, -12345, -12345, -12345,
-12345, -12345, -12345, -12345, -12345,
-12345, -12345, -12345, -12345, -12345,
-12345, -12345, -12345, -12345, -12345,
-12345, -12345, -12345, -12345, -12345,
-12345, -12345, -12345, -12345, -12345,
-12345, -12345, -12345, -12345, -12345,
{ '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
{ '-','1','2','3','4','5',' ',' ' }
};

/* defines for logical data types */
#define TRUE    1
#define FALSE   0

/* defines for enumerated data types */
#define IREAL   0 
#define ITIME   1 
#define IRLIM   2 
#define IAMPH   3 
#define IXY     4 
#define IUNKN   5 
#define IDISP   6 
#define IVEL    7 
#define IACC    8 
#define IB      9 
#define IDAY   10 
#define IO     11 
#define IA     12 
#define IT0    13 
#define IT1    14 
#define IT2    15 
#define IT3    16 
#define IT4    17 
#define IT5    18 
#define IT6    19 
#define IT7    20 
#define IT8    21 
#define IT9    22 
#define IRADNV 23 
#define ITANNV 24 
#define IRADEV 25 
#define ITANEV 26 
#define INORTH 27 
#define IEAST  28 
#define IHORZA 29 
#define IDOWN  30 
#define IUP    31 
#define ILLLBB 32 
#define IWWSN1 33 
#define IWWSN2 34 
#define IHGLP  35 
#define ISRO   36 
#define INUCL  37 
#define IPREN  38 
#define IPOSTN 39 
#define IQUAKE 40 
#define IPREQ  41 
#define IPOSTQ 42 
#define ICHEM  43 
#define IOTHER 44 
#define IGOOD  45 
#define IGLCH  46 
#define IDROP  47 
#define ILOWSN 48 
#define IRLDTA 49 
#define IVOLTS 50 
#define INIV51 51 
#define INIV52 52 
#define INIV53 53 
#define INIV54 54 
#define INIV55 55 
#define INIV56 56 
#define INIV57 57 
#define INIV58 58 
#define INIV59 59 
#define INIV60 60

#define TEST "%12.5f%12.5f%12.5f%12.5f%12.5f\n"
#define FCS "%15.7f%15.7f%15.7f%15.7f%15.7f\n"
#define ICS "%10d%10d%10d%10d%10d\n"
#define CCS1 "%-8.8s%-8.8s%-8.8s\n"
#define CCS2 "%-8.8s%-16.16s\n"
