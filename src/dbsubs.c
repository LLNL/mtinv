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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <errno.h>

#include "../include/nrutil.h"
#include "../include/mt.h"
#include "../include/NNSA_schemas.h"

extern char progname[128];

void db_sqlite3_create( void )
{
	FILE *fp;
	extern long evid;

        char moment_table_name[256];
        char focal_plane_table_name[256];
        char origin_table_name[256];
        char mt_data_table_name[256];
        char mt_waveform_segment_table_name[256];
	char mt_phase_data_table_name[256];
        char mt_filter_table_name[256];
        char mt_earthmodel_table_name[256];
        char mt_earthmodel_node_table_name[256];

        sprintf( moment_table_name, "MOMENT_STAGE" );
        sprintf( focal_plane_table_name, "FOCAL_PLANE_STAGE" );
        sprintf( origin_table_name, "MT_ORIGIN_STAGE" );
        sprintf( mt_data_table_name, "MT_DATA_STAGE" );
        sprintf( mt_waveform_segment_table_name, "MT_WAVEFORM_SEGMENT_STAGE" );
	sprintf( mt_phase_data_table_name, "MT_PHASE_DATA_STAGE" );
        sprintf( mt_filter_table_name, "MT_FILTER_STAGE" );
        sprintf( mt_earthmodel_table_name, "MT_EARTHMODEL_STAGE" );
        sprintf( mt_earthmodel_node_table_name, "MT_EARTHMODEL_NODE_STAGE" );

	if( (fp = fopen( "create.sql", "w" )) == NULL )
	{
		fprintf( stderr, "ERROR cannot open file create.sql\n" );
		exit(-1);
	}

	fprintf( fp, "DROP TABLE %s;\n", moment_table_name );
        fprintf( fp, "DROP TABLE %s;\n", focal_plane_table_name );
        fprintf( fp, "DROP TABLE %s;\n", origin_table_name );
        fprintf( fp, "DROP TABLE %s;\n", mt_data_table_name );
        fprintf( fp, "DROP TABLE %s;\n", mt_waveform_segment_table_name );
	fprintf( fp, "DROP TABLE %s;\n", mt_phase_data_table_name );
        fprintf( fp, "DROP TABLE %s;\n", mt_filter_table_name );
        fprintf( fp, "DROP TABLE %s;\n", mt_earthmodel_table_name );
        fprintf( fp, "DROP TABLE %s;\n", mt_earthmodel_node_table_name );

        fprintf( fp, "\n\n" );

        fprintf( fp, "DROP TABLE mtid_seq;\n" );
        fprintf( fp, "DROP TABLE fpid_seq;\n" );
        fprintf( fp, "DROP TABLE mtorid_seq;\n" );
        fprintf( fp, "DROP TABLE mtdataid_seq;\n" );
        fprintf( fp, "DROP TABLE mtwfsegid_seq;\n" );
	fprintf( fp, "DROP TABLE phaid_seq;\n" );
        fprintf( fp, "DROP TABLE mtfilterid_seq;\n" );
        fprintf( fp, "DROP TABLE mtvmodid_seq;\n" );
        fprintf( fp, "DROP TABLE nodeid_seq;\n" );

        fprintf( fp, "\n\n" );
	
fprintf(fp, "CREATE TABLE       mtid_seq (      mtid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);\n");
fprintf(fp, "CREATE TABLE       fpid_seq (      fpid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);\n");
fprintf(fp, "CREATE TABLE     mtorid_seq (    mtorid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);\n");
fprintf(fp, "CREATE TABLE   mtdataid_seq (  mtdataid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);\n");
fprintf(fp, "CREATE TABLE  mtwfsegid_seq ( mtwfsegid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);\n");
fprintf(fp, "CREATE TABLE      phaid_seq (     phaid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);\n");
fprintf(fp, "CREATE TABLE mtfilterid_seq (mtfilterid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);\n");
fprintf(fp, "CREATE TABLE   mtvmodid_seq (  mtvmodid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);\n");
fprintf(fp, "CREATE TABLE     nodeid_seq (    nodeid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);\n");

	fprintf( fp, "\n\n" );

	fprintf( fp, "INSERT into       mtid_seq (       mtid, lddate) VALUES (  1000, CURRENT_TIMESTAMP );\n" );
	fprintf( fp, "INSERT into       fpid_seq (       fpid, lddate) VALUES (   100, CURRENT_TIMESTAMP );\n" );
	fprintf( fp, "INSERT into     mtorid_seq (     mtorid, lddate) VALUES ( 10000, CURRENT_TIMESTAMP );\n" );
	fprintf( fp, "INSERT into   mtdataid_seq (   mtdataid, lddate) VALUES (   100, CURRENT_TIMESTAMP );\n" );
        fprintf( fp, "INSERT into  mtwfsegid_seq (  mtwfsegid, lddate) VALUES (    10, CURRENT_TIMESTAMP );\n" );
	fprintf( fp, "INSERT into      phaid_seq (      phaid, lddate) VALUES (     1, CURRENT_TIMESTAMP );\n" );
        fprintf( fp, "INSERT into mtfilterid_seq ( mtfilterid, lddate) VALUES (     1, CURRENT_TIMESTAMP );\n" );
	fprintf( fp, "INSERT into   mtvmodid_seq (   mtvmodid, lddate) VALUES (     1, CURRENT_TIMESTAMP );\n" );
        fprintf( fp, "INSERT into     nodeid_seq (     nodeid, lddate) VALUES (     1, CURRENT_TIMESTAMP );\n" );

/*** create table for moment ***/

        fprintf( fp, "CREATE TABLE %s ( \n", moment_table_name );
        fprintf( fp, " mtid          NUMBER(9)    not null,\n" );
        fprintf( fp, " fpid          NUMBER(9)    default  -1,\n");
        fprintf( fp, " orid          NUMBER(9)    not null, \n");
        fprintf( fp, " mxx           FLOAT(53)    not null, \n");
        fprintf( fp, " myy           FLOAT(53)    not null, \n");
        fprintf( fp, " mzz           FLOAT(53)    not null, \n");
        fprintf( fp, " mxy           FLOAT(53)    not null, \n");
        fprintf( fp, " mxz           FLOAT(53)    not null, \n");
        fprintf( fp, " myz           FLOAT(53)    not null, \n");
        fprintf( fp, " smxx          FLOAT(53)    default   -1, \n");
        fprintf( fp, " smyy          FLOAT(53)    default   -1, \n");
        fprintf( fp, " smzz          FLOAT(53)    default   -1, \n");
        fprintf( fp, " smxy          FLOAT(53)    default   -1, \n");
        fprintf( fp, " smxz          FLOAT(53)    default   -1, \n");
        fprintf( fp, " smyz          FLOAT(53)    default   -1, \n");
        fprintf( fp, " mt_type       VARCHAR2(6)  default  '-', \n");
        fprintf( fp, " mo            FLOAT(53)    default   -1, \n");
        fprintf( fp, " smo           FLOAT(53)    default   -1, \n");
        fprintf( fp, " momag         FLOAT(24)    default -999, \n");
        fprintf( fp, " piso          FLOAT(24)    default   -1, \n");
        fprintf( fp, " pclvd         FLOAT(24)    default   -1, \n");
        fprintf( fp, " pdc           FLOAT(24)    default   -1, \n");
        fprintf( fp, " kiso          FLOAT(24)    default -999, \n");
        fprintf( fp, " kclvd         FLOAT(24)    default -999, \n");
        fprintf( fp, " epsilon       FLOAT(24)    default -999, \n");
        fprintf( fp, " f_factor      FLOAT(24)    default -999, \n");
        fprintf( fp, " var_red       FLOAT(53)    default  999, \n");
        fprintf( fp, " residual      FLOAT(53)    default   -1, \n");
        fprintf( fp, " variance      FLOAT(53)    default   -1, \n");
        fprintf( fp, " nslpb         NUMBER(5)    default   -1, \n");
        fprintf( fp, " nrlpb         NUMBER(5)    default   -1, \n");
        fprintf( fp, " tmnlpb        FLOAT(24)    default   -1, \n");
        fprintf( fp, " nsmw          NUMBER(5)    default   -1, \n");
	fprintf( fp, " nrmw          NUMBER(5)    default   -1, \n");
        fprintf( fp, " tmnmw         FLOAT(24)    default   -1, \n");
        fprintf( fp, " nssurf        NUMBER(5)    default   -1, \n");
        fprintf( fp, " nrsurf        NUMBER(5)    default   -1, \n");
        fprintf( fp, " tmnsurf       FLOAT(24)    default   -1, \n");
        fprintf( fp, " half_duration FLOAT(24)    default -1, \n");
        fprintf( fp, " srcfunc       VARCHAR2(12) default '-', \n");
        fprintf( fp, " algorithm     VARCHAR2(15) default '-', \n");
        fprintf( fp, " epi_fixed     CHAR(1)      default '-', \n");
        fprintf( fp, " time_fixed    CHAR(1)      default '-', \n");
        fprintf( fp, " depth_fixed   CHAR(1)      default '-', \n");
        fprintf( fp, " auth          VARCHAR2(20) default '-', \n");
        fprintf( fp, " commid        NUMBER(9)    default -1, \n");
        fprintf( fp, " lddate        DATETIME     default CURRENT_TIMESTAMP ); \n");
        fprintf( fp, "\n\n" );
 
/*** create table for focal_plane ***/
	
	fprintf( fp, "CREATE TABLE %s ( \n", focal_plane_table_name );
        fprintf( fp, " fpid          NUMBER(9)    not null, \n" );
        fprintf( fp, " orid          NUMBER(9)    not null, \n" );
        fprintf( fp, " fp_type       VARCHAR2(6)  default  '-', \n" );
        fprintf( fp, " decomp_type   VARCHAR2(8)  default  '-', \n" );
        fprintf( fp, " strike1       FLOAT(24)    default -999, \n" );
        fprintf( fp, " dip1          FLOAT(24)    default -999, \n" );
        fprintf( fp, " rake1         FLOAT(24)    default -999, \n" );
        fprintf( fp, " strike2       FLOAT(24)    default -999, \n" );
        fprintf( fp, " dip2          FLOAT(24)    default -999, \n" );
        fprintf( fp, " rake2         FLOAT(24)    default -999, \n" );
        fprintf( fp, " sstrike       FLOAT(24)    default -999, \n" );
        fprintf( fp, " sdip          FLOAT(24)    default -999, \n" );
        fprintf( fp, " srake         FLOAT(24)    default -999, \n" );
        fprintf( fp, " primary_plane NUMBER(2)    default   -1, \n" );
        fprintf( fp, " eval1         FLOAT(53)    default    0, \n" );
        fprintf( fp, " az1           FLOAT(24)    default -999, \n" );
        fprintf( fp, " pl1           FLOAT(24)    default -999, \n" );
        fprintf( fp, " eval2         FLOAT(53)    default    0, \n" );
        fprintf( fp, " az2           FLOAT(24)    default -999, \n" );
        fprintf( fp, " pl2           FLOAT(24)    default -999, \n" );
        fprintf( fp, " eval3         FLOAT(53)    default    0, \n" );
        fprintf( fp, " az3           FLOAT(24)    default -999, \n" );
        fprintf( fp, " pl3           FLOAT(24)    default -999, \n" );
        fprintf( fp, " dcmo          FLOAT(53)    default   -1, \n" );
        fprintf( fp, " devmo         FLOAT(53)    default   -1, \n" );
        fprintf( fp, " isomo         FLOAT(53)    default   -1, \n" );
        fprintf( fp, " auth          VARCHAR2(20) default  '-', \n" );
        fprintf( fp, " commid        NUMBER(9)    default   -1, \n" );
        fprintf( fp, " lddate        DATETIME     default CURRENT_TIMESTAMP );   \n" );
        fprintf( fp, "\n\n" );

/*** create table for origin ***/

	fprintf( fp, "CREATE TABLE %s ( \n", origin_table_name );
        fprintf( fp, " lat       FLOAT(53)    default           -999, \n" );
        fprintf( fp, " lon       FLOAT(53)    default           -999, \n" );
        fprintf( fp, " depth     FLOAT(24)    default           -999, \n" );
        fprintf( fp, " time      FLOAT(53)    default 9999999999.999, \n" );
        fprintf( fp, " orid      NUMBER(9)    not null              , \n" );
        fprintf( fp, " evid      NUMBER(9)    default             -1, \n" );
        fprintf( fp, " jdate     NUMBER(8)    default             -1, \n" );
        fprintf( fp, " nass      NUMBER(4)    default             -1, \n" );
        fprintf( fp, " ndef      NUMBER(4)    default             -1, \n" );
        fprintf( fp, " ndp       NUMBER(4)    default             -1, \n" );
        fprintf( fp, " grn       NUMBER(8)    default             -1, \n" );
        fprintf( fp, " srn       NUMBER(8)    default             -1, \n" );
        fprintf( fp, " etype     VARCHAR2(7)  default            '-', \n" );
        fprintf( fp, " depdp     FLOAT(24)    default           -999, \n" );
        fprintf( fp, " dtype     VARCHAR2(1)  default            '-', \n" );
        fprintf( fp, " mb        FLOAT(24)    default           -999, \n" );
        fprintf( fp, " mbid      NUMBER(9)    default             -1, \n" );
        fprintf( fp, " ms        FLOAT(24)    default           -999, \n" );
        fprintf( fp, " msid      NUMBER(9)    default             -1, \n" );
        fprintf( fp, " ml        FLOAT(24)    default           -999, \n" );
        fprintf( fp, " mlid      NUMBER(9)    default             -1, \n" );
        fprintf( fp, " algorithm VARCHAR2(15) default            '-', \n" );
        fprintf( fp, " auth      VARCHAR2(20) default            '-', \n" );
        fprintf( fp, " commid    NUMBER(9)    default             -1, \n" );
        fprintf( fp, " lddate    DATETIME     default CURRENT_TIMESTAMP, \n" );
        fprintf( fp, " unique ( lat, lon, depth, time, algorithm, auth ) ); \n" );
        fprintf( fp, "\n\n" );

/*** create table for mt_data ***/

	fprintf( fp, "CREATE TABLE %s ( \n", mt_data_table_name );
        fprintf( fp, " mtdataid NUMBER(9)    not null,\n" );
        fprintf( fp, " mtid     NUMBER(9)    not null,\n" );
        fprintf( fp, " sta      VARCHAR2(8)  default          '-', \n" );
        fprintf( fp, " net      VARCHAR2(8)  default          '-', \n" );
        fprintf( fp, " distkm   FLOAT(24)    default         -999, \n" );
        fprintf( fp, " azimuth  FLOAT(24)    default         -999, \n" );
        fprintf( fp, " used     VARCHAR2(1)  default          '-', \n" );
        fprintf( fp, " quality  VARCHAR2(32) default          '-', \n" );
        fprintf( fp, " auth     VARCHAR2(20) default          '-', \n" );
        fprintf( fp, " commid   NUMBER(9)    default           -1, \n" );
        fprintf( fp, " lddate   DATETIME     default CURRENT_TIMESTAMP ); \n" );
        fprintf( fp, "\n\n" );

/*** create table for mt_wfseg  ***/

        fprintf( fp, "CREATE TABLE %s ( \n", mt_waveform_segment_table_name );
        fprintf( fp, " mtwfsegid      NUMBER(9)    not null             , \n" );
        fprintf( fp, " mtdataid       NUMBER(9)    not null             , \n" );
        fprintf( fp, " wfsegtype      VARCHAR2(16) default            -1, \n" );
        fprintf( fp, " sta            VARCHAR2(8)  default           '-', \n" );
        fprintf( fp, " chan           VARCHAR2(8)  default           '-', \n" );
        fprintf( fp, " chanid         NUMBER(9)    default            -1, \n" );
        fprintf( fp, " start_time     FLOAT(53)    default          -999, \n" );
        fprintf( fp, " end_time       FLOAT(53)    default   -9999999999, \n" );
        fprintf( fp, " mtfilterid     NUMBER(9)    default            -1, \n" );
        fprintf( fp, " mtvmodid       NUMBER(9)    default            -1, \n" );
        fprintf( fp, " redvel         FLOAT(24)    default          -999, \n" );
        fprintf( fp, " tshift         FLOAT(24)    default          -999, \n" );
        fprintf( fp, " ampweight      FLOAT(24)    default            -1, \n" );
        fprintf( fp, " distnorm       VARCHAR2(1)  default           '-', \n" );
        fprintf( fp, " r0             FLOAT(24)    default            -1, \n" );
        fprintf( fp, " rayparam       FLOAT(24)    default            -1, \n" );
        fprintf( fp, " distdeg        FLOAT(24)    default          -999, \n" );
        fprintf( fp, " distkm         FLOAT(24)    default          -999, \n" );
        fprintf( fp, " az             FLOAT(24)    default          -999, \n" );
        fprintf( fp, " baz            FLOAT(24)    default          -999, \n" );
        fprintf( fp, " interpsamprate FLOAT(24)    default            -1, \n" );
        fprintf( fp, " used           VARCHAR2(1)  default           '-', \n" );
        fprintf( fp, " quality        VARCHAR2(64) default           '-', \n" );
	fprintf( fp, " snr            FLOAT(24)    default            -1, \n" );
	fprintf( fp, " xcor           FLOAT(24)    default          -999, \n" );
	fprintf( fp, " lagtime        FLOAT(24)    default          -999, \n" );
        fprintf( fp, " algor          VARCHAR2(32) default           '-', \n" );
        fprintf( fp, " lddate         DATETIME     default CURRENT_TIMESTAMP); \n" );
        fprintf( fp, "\n\n" );
 
/*** create table for mt_phase_data_table_name */
	
	fprintf( fp, "CREATE TABLE %s ( \n", mt_phase_data_table_name );
	fprintf( fp, " phaid          NUMBER(9)    not null,              \n" );
	fprintf( fp, " mtwfsegid      NUMBER(9)    not null,              \n" );
	fprintf( fp, " phasetype      VARCHAR2(8)  default           '-', \n" );
	fprintf( fp, " sta            VARCHAR2(8)  default           '-', \n" );
	fprintf( fp, " chan           VARCHAR2(8)  default           '-', \n" );
	fprintf( fp, " gvlo           FLOAT(24)    default            -1, \n" );
	fprintf( fp, " gvhi           FLOAT(24)    default            -1, \n" );
	fprintf( fp, " amp            FLOAT(24)    default          -999, \n" );
	fprintf( fp, " time           FLOAT(53)    default   -9999999999, \n" );
	fprintf( fp, " dur            FLOAT(24)    default            -1, \n" );
	fprintf( fp, " freq           FLOAT(24)    default            -1, \n" );
	fprintf( fp, " per            FLOAT(24)    default            -1, \n" );
	fprintf( fp, " lddate         DATETIME     default CURRENT_TIMESTAMP); \n" );
	fprintf( fp, "\n\n" );

/*** create table for mt_filter ***/ 

        fprintf( fp, "CREATE TABLE %s ( \n", mt_filter_table_name );
        fprintf( fp, " mtfilterid   NUMBER(9)    not null, \n" );
        fprintf( fp, " filtertype   VARCHAR2(2)  not null, \n" );
        fprintf( fp, " filterdesign VARCHAR2(16) not null, \n" );
        fprintf( fp, " npole        NUMBER(10)   default   -1, \n" );
        fprintf( fp, " lcorner      FLOAT(24)    default  999, \n" );
        fprintf( fp, " hcorner      FLOAT(24)    default  999, \n" );
        fprintf( fp, " npass        NUMBER(2)    default   -1, \n" );
        fprintf( fp, " tranbw       FLOAT(24)    default   -1, \n" );
        fprintf( fp, " attenfactor  FLOAT(24)    default   -1 );\n" );
        fprintf( fp, "\n\n" );
 
/*** create table for mt_earthmodel ***/

        fprintf( fp, "CREATE TABLE %s ( \n", mt_earthmodel_table_name );
        fprintf( fp, " mtvmodid  NUMBER(9)    not null, \n" );
        fprintf( fp, " dimension VARCHAR2(16) not null, \n" );
        fprintf( fp, " nlon      NUMBER(9)    not null, \n" );
        fprintf( fp, " nlat      NUMBER(9)    not null, \n" );
        fprintf( fp, " ndep      NUMBER(9)    not null, \n" );
        fprintf( fp, " modelname VARCHAR2(64) default   '-', \n" );
        fprintf( fp, " auth      VARCHAR2(32) default   '-', \n" );
        fprintf( fp, " lddate    DATETIME     default CURRENT_TIMESTAMP);\n" );
        fprintf( fp, "\n\n" );
 
/*** create table for mt_earthmodel_node ****/

        fprintf( fp, "CREATE TABLE %s ( \n", mt_earthmodel_node_table_name );
        fprintf( fp, " nodeid      NUMBER(9)    not null primary key, \n" );
        fprintf( fp, " mtvmodid    NUMBER(9)    default     -1, \n" );
        fprintf( fp, " lon         FLOAT(24)    default   -999, \n" );
        fprintf( fp, " lat         FLOAT(24)    default   -999, \n" );
        fprintf( fp, " interpstyle VARCHAR2(16) default    '-', \n" );
        fprintf( fp, " z           FLOAT(24)    default   -999, \n" );
        fprintf( fp, " ztop        FLOAT(24)    default   -999, \n" );
        fprintf( fp, " thickness   FLOAT(24)    default   -999, \n" );
        fprintf( fp, " vp          FLOAT(24)    default   -999, \n" );
        fprintf( fp, " qp          FLOAT(24)    default   -999, \n" );
        fprintf( fp, " vs          FLOAT(24)    default   -999, \n" );
        fprintf( fp, " qs          FLOAT(24)    default   -999, \n" );
        fprintf( fp, " rho         FLOAT(24)    default   -999, \n" );
        fprintf( fp, " poissonratio FLOAT(24)   default   -999);\n" );
        fprintf( fp, "\n\n" );
 
/*** show the tables ***/
/***
        fprintf( fp, "desc %s;\n", moment_table_name );
        fprintf( fp, "desc %s;\n", focal_plane_table_name );
        fprintf( fp, "desc %s;\n", origin_table_name );
        fprintf( fp, "desc %s;\n", mt_data_table_name );
        fprintf( fp, "desc %s;\n", mt_waveform_segment_table_name );
        fprintf( fp, "desc %s;\n", mt_filter_table_name );
        fprintf( fp, "desc %s;\n", mt_earthmodel_table_name );
        fprintf( fp, "desc %s;\n", mt_earthmodel_node_table_name );
        fprintf( fp, "source insert.sql \n" );
        fprintf( fp, "\n\n" );
***/

	fclose(fp);

} /*** db_sqlite3_create() ***/



/****************************************************************************************************************************/
/****************************************************************************************************************************/
/********
# orid    (link to origin table for origin time, lat, lon, depth) NA not allowed
# mt_type (moment tensor type = DEV, DC, FULL, ISO, NA='-')
# momag    moment magnitude (NA=-999)
# M0      scalar seismic moment (N*m)  NA=-1
# %DC     percent isotropic  NA = -1
# %CLVD   percent compensated linear vector dipole NA = -1
# %ISO    percent double couple NA = -1
# %VR     (percent variance reduction  NA = 999)
# Mij     (Mxx,Myy,Mzz,Mxy,Mxz,Myz in Nm) NA not allowed
# author  (LLNL OUN is good)  NA = '-'
#
#orid     mt_type momag   m0       pdc  pclvd piso var_red  mxx       myy         mzz         mxy         mxz        myz        author     depth
#8109218  FULL    4.72    1.49e+16  12  36    53   86.41    -1.29e+15  -5.21e+14  -5.47e+14    4.15e+13   4.29e+14    2.75e+12  ichinose1  1.0
***/
/****************************************************************************************************************************/
/****************************************************************************************************************************/

void llnl_db_write(  EventInfo *ev, Solution *sol, Greens **grn, int nsta, int iz, int AutoAuth, int verbose )
{
	MyTime *now;
	FILE *fp;
        char *username;
        long orid = 10000001;
	extern long evid;
	char mt_type[24];
	int mtdegfree;
	float mxx, myy, mzz, mxy, mxz, myz;
	float DyneCm_to_Netwon_M = 1.0E-07;

	char now_string[32];
	char ot_string[32];
	
/*** get the username from shell environment ***/

        username = calloc(32,sizeof(char));

        if( AutoAuth )
                sprintf( username, "AutoMT:%s", ev[0].dbuser );
        else
                username = getenv( "USER" );

        username[15] = '\0';

/*** get the system time now ***/

        now = (MyTime *)calloc( 1, sizeof( MyTime ) );
        now = myGMTtime( now );

/*** get mt type ***/

	if( sol[iz].mt_type == EXPLOSION  ) sprintf( mt_type, "ISO" );
        if( sol[iz].mt_type == DEVIATORIC ) sprintf( mt_type, "DEV" );
        if( sol[iz].mt_type == FULL_MT    ) sprintf( mt_type, "FULL" );

/*** convert mt coordinate system to X-Y and units to Nm ***/

/**** this is for llnl db loader ****/
/**** this is not correct coversion from cartesian to spherical coordinate systems need to fix or use below ****/
/******
	mxx =  sol[iz].mrr * pow( 10.0, sol[iz].exponent ) * DyneCm_to_Netwon_M;
        myy =  sol[iz].mtt * pow( 10.0, sol[iz].exponent ) * DyneCm_to_Netwon_M;
        mzz =  sol[iz].mff * pow( 10.0, sol[iz].exponent ) * DyneCm_to_Netwon_M;
        mxy =  sol[iz].mrt * pow( 10.0, sol[iz].exponent ) * DyneCm_to_Netwon_M;
        mxz = -sol[iz].mrf * pow( 10.0, sol[iz].exponent ) * DyneCm_to_Netwon_M;
        myz = -sol[iz].mtf * pow( 10.0, sol[iz].exponent ) * DyneCm_to_Netwon_M;
*****/
	mxx  = sol[iz].moment_tensor[1][1] * DyneCm_to_Netwon_M;
	myy  = sol[iz].moment_tensor[2][2] * DyneCm_to_Netwon_M;
	mzz  = sol[iz].moment_tensor[3][3] * DyneCm_to_Netwon_M;

	mxy  = sol[iz].moment_tensor[1][2] * DyneCm_to_Netwon_M;
	mxz  = sol[iz].moment_tensor[1][3] * DyneCm_to_Netwon_M;
	myz  = sol[iz].moment_tensor[2][3] * DyneCm_to_Netwon_M;

/*** write out the solution ***/
	
	fp = fopen( "llnl_db.load.txt", "a"  );

	sprintf( ot_string, "%04d-%02d-%02dT%02d:%02d:%06.3f",
		ev[0].ot.year,  ev[0].ot.month, ev[0].ot.mday, ev[0].ot.hour, ev[0].ot.min, ev[0].ot.fsec );
	sprintf( now_string, "%04d-%02d-%02dT%02d:%02d:%06.3f",
		now->year, now->month, now->mday, now->hour, now->min, now->fsec );

	fprintf( fp, "orid     mt_type momag m0        pdc  pclvd piso var_red   mxx        myy        mzz        mxy        mxz        myz        author       depth\n" );
        fprintf( fp, "%ld %s %8.2f %8.2e  %3.0f %3.0f   %3.0f  %8.2f   %+6.2e  %+6.2e  %+6.2e  %+6.2e  %+6.2e  %+6.2e  %s %8.2f %s %s_%s_%s\n",
		orid,
                mt_type,
		sol[iz].mw,
                sol[iz].dmoment * DyneCm_to_Netwon_M,
		sol[iz].PDC,
                sol[iz].PCLVD,
		sol[iz].PISO,
		sol[iz].var_red,
		mxx, myy, mzz, mxy, mxz, myz,
		username,
		grn[0][iz].evdp,
		ot_string,
		now_string,
		progname,
		Version_Label );

	fclose(fp);
	free(now);
}

void db_sqlite3_write( EventInfo *ev, Solution *sol, Greens **grn, int nsta, int iz, int AutoAuth, int verbose )
{
	Moment *moment;
	Focal_Plane *focal_plane;
	MT_Data *mt_data;
	MT_Filter *mt_filter;
	Origin *origin;
	extern long evid;

	float mxx, myy, mzz, mxy, mxz, myz;
        float DyneCm_to_Netwon_M = 1.0E-07;

	int ista, ichan, ilay;
	char chan[8];
	char mt_type[24];
	char grd_mo_type;
	int mtdegfree;
	char used;
	FILE *fp;
	char *username;
	MyTime *now;
	long orid, fpid, mtid, mtdataid, mtwfsegid, mtfilterid, mtvmodid, nodeid;
	int jdate;
	
	char moment_table_name[256];
	char focal_plane_table_name[256];
	char origin_table_name[256];
	char mt_data_table_name[256];
	char mt_waveform_segment_table_name[256];
	char mt_phase_data_table_name[256];
	char mt_filter_table_name[256];
	char mt_earthmodel_table_name[256];
	char mt_earthmodel_node_table_name[256];

/*** get the username from shell environment ***/

	username = calloc(32,sizeof(char));

	if( AutoAuth )
		sprintf( username, "AutoMT:%s", ev[0].dbuser );
	else
		username = getenv( "USER" );

	username[15] = '\0';

/*** get the system time now ***/
	
	now = (MyTime *)calloc( 1, sizeof( MyTime ) );
	now = myGMTtime( now );

/*** set some defaults test id's ***/
/*** sql insert statements use nextval or currval from oracle sequence table ***/

	orid = 10001;
	fpid = 101;
	mtid = 11;
	mtdataid = 2;
	mtwfsegid = 1;
	mtfilterid = 3;
	mtvmodid = 100;
	nodeid = 1;

/*********** set moment tensor degrees of freedom here ****************************/

	if( sol[iz].mt_type == EXPLOSION  ) mtdegfree = 1;
        if( sol[iz].mt_type == DEVIATORIC ) mtdegfree = 5;
        if( sol[iz].mt_type == FULL_MT    ) mtdegfree = 6;

        if( mtdegfree == 1 ) sprintf( mt_type, "ISOMT" );
        if( mtdegfree == 5 ) sprintf( mt_type, "DEVMT" );
        if( mtdegfree == 6 ) sprintf( mt_type, "FULLMT" );

/*** set table names here ***/

        sprintf( moment_table_name, "MOMENT_STAGE" );
        sprintf( focal_plane_table_name, "FOCAL_PLANE_STAGE" );
        sprintf( origin_table_name, "MT_ORIGIN_STAGE" );
        sprintf( mt_data_table_name, "MT_DATA_STAGE" );
        sprintf( mt_waveform_segment_table_name, "MT_WAVEFORM_SEGMENT_STAGE" );
	sprintf( mt_phase_data_table_name, "MT_PHASE_DATA_STAGE" );
        sprintf( mt_filter_table_name, "MT_FILTER_STAGE" );
        sprintf( mt_earthmodel_table_name, "MT_EARTHMODEL_STAGE" );
        sprintf( mt_earthmodel_node_table_name, "MT_EARTHMODEL_NODE_STAGE" );

	if( ( fp = fopen( "insert.sql", "w" ) ) == NULL )
	{
		fprintf( stderr, "ERROR: cannot open file insert.sql\n" );
		exit(-1);
	}

	fprintf( fp, "INSERT into   mtid_seq ( lddate ) VALUES ( current_timestamp );\n" );
	fprintf( fp, "INSERT into   fpid_seq ( lddate ) VALUES ( current_timestamp );\n" );
	fprintf( fp, "INSERT into mtorid_seq ( lddate ) VALUES ( current_timestamp );\n" );
	fprintf( fp, "\n" );
	fprintf( fp, "INSERT into %s\n", moment_table_name );
	fprintf( fp, "  ( mtid, fpid, orid, mxx, myy, mzz, mxy, mxz, myz, mt_type, mo, momag, \n" );
	fprintf( fp, "  piso, pclvd, pdc, kiso, kclvd, epsilon, f_factor, var_red, residual, \n" );
        fprintf( fp, "  algorithm, epi_fixed, time_fixed, depth_fixed, auth, lddate ) \n" );
	fprintf( fp, "VALUES ( \n" );
	fprintf( fp, " (SELECT   max(mtid) FROM mtid_seq), \n" );
	fprintf( fp, " (SELECT   max(fpid) FROM fpid_seq), \n" );
	fprintf( fp, " (SELECT max(mtorid) FROM mtorid_seq), \n" );

/*** this is output for sqlite3 *******/
/**** this is not correct coversion from cartesian to spherical coordinate systems need to fix or use below ****/
/***
                sol[iz].mrr * pow( 10.0, sol[iz].exponent ),
                sol[iz].mtt * pow( 10.0, sol[iz].exponent ),
                sol[iz].mff * pow( 10.0, sol[iz].exponent ),
                sol[iz].mrt * pow( 10.0, sol[iz].exponent ),
                -sol[iz].mrf * pow( 10.0, sol[iz].exponent ),
                -sol[iz].mtf * pow( 10.0, sol[iz].exponent )
****/
	mxx  = sol[iz].moment_tensor[1][1] * DyneCm_to_Netwon_M;
        myy  = sol[iz].moment_tensor[2][2] * DyneCm_to_Netwon_M;
        mxy  = sol[iz].moment_tensor[1][2] * DyneCm_to_Netwon_M;
        mxz  = sol[iz].moment_tensor[1][3] * DyneCm_to_Netwon_M;
        myz  = sol[iz].moment_tensor[2][3] * DyneCm_to_Netwon_M;
        mzz  = sol[iz].moment_tensor[3][3] * DyneCm_to_Netwon_M;

        fprintf( fp, "    %6.2e, %6.2e, %6.2e, %6.2e, %6.2e, %6.2e, \n", 
				mxx, myy, mzz, mxy, mxz, myz );

        /***   type  Mo     Mw     PDC   PCLVD   PISO   kiso   kclvd  eps    f_fac */
        fprintf( fp, " '%s', %8.2e, %8.2f, %3.0f, %3.0f, %3.0f, %8.2f, %7.2f, %8.3f, %9.4f, \n",
                mt_type,
                sol[iz].dmoment,
                sol[iz].mw,
                sol[iz].PISO,
                sol[iz].PCLVD,
                sol[iz].PDC,
                sol[iz].k,
                -999.0,
                sol[iz].epsilon,
                sol[iz].f_factor );

 	/***           var_red l2norm progname version label, username ****/
        fprintf( fp, " %6.2f, %5.2e, '%s%s:%d', 'Y', 'N', 'N', '%s', CURRENT_TIMESTAMP ); \n",
                sol[iz].var_red,
                sol[iz].l2norm_error,
                progname,
                Version_Label,
		mtdegfree,
                username );
	
	fprintf( fp, "\n" );

/********************************************/
/*** DEVIATORIC   major double couple     ***/
/********************************************/

	if( sol[iz].mt_type == DEVIATORIC || mtdegfree == 5 ) 
        {
          fprintf( fp, "INSERT into %s \n", focal_plane_table_name );

	  fprintf( fp, " ( fpid, orid, fp_type, decomp_type, strike1, dip1, rake1, strike2, dip2, rake2, \n" );
          fprintf( fp, "   eval1, az1, pl1, eval2, az2, pl2, eval3, az3, pl3, \n" );
          fprintf( fp, "   auth, devmo, isomo, dcmo, lddate ) \n" );
	  fprintf( fp, "VALUES \n" );
	  fprintf( fp, "( (SELECT   max(fpid) FROM fpid_seq), \n" );
	  fprintf( fp, "  (SELECT max(mtorid) FROM mtorid_seq), \n" );

          fprintf( fp, " 'MT', 'MAJORDC',   %3.0f, %2.0f, %4.0f,   %3.0f, %2.0f, %4.0f, \n",
                sol[iz].Maj.P1.s, sol[iz].Maj.P1.d, sol[iz].Maj.P1.r,
                sol[iz].Maj.P2.s, sol[iz].Maj.P2.d, sol[iz].Maj.P2.r );
 
          fprintf( fp, " %5.2f, %.0f, %.0f,  %5.2f, %.0f, %.0f,  %5.2f, %.0f, %.0f, \n",
                sol[iz].Maj.T.ev, sol[iz].Maj.T.az, sol[iz].Maj.T.pl,
                sol[iz].Maj.P.ev, sol[iz].Maj.P.az, sol[iz].Maj.P.pl,
                sol[iz].Maj.B.ev, sol[iz].Maj.B.az, sol[iz].Maj.B.pl );
 
          fprintf( fp, " '%s', %g, %g, %g, CURRENT_TIMESTAMP ); \n",
                username,
                sol[iz].Mdc + sol[iz].Mclvd,
                sol[iz].Miso,
                sol[iz].Mdc );
        
          fprintf( fp, "\n" );

        } /*** mt_type == DEVIATORIC ***/

/**************************************************/
/*** FULL_MT           full moment tensor       ***/
/*** EXPLOSION     isotropic only moment tensor ***/
/**************************************************/


	if( ( sol[iz].mt_type == FULL_MT    || mtdegfree == 6 ) || 
            ( sol[iz].mt_type == EXPLOSION  || mtdegfree == 1 ) )
        {

         fprintf( fp, "INSERT into %s \n", focal_plane_table_name );

         fprintf( fp, " ( fpid, orid, fp_type, decomp_type, strike1, dip1, rake1, strike2, dip2, rake2, \n" );
         fprintf( fp, " eval1, az1, pl1, eval2, az2, pl2, eval3, az3, pl3, auth, devmo, isomo, dcmo, lddate )\n" );
	 fprintf( fp, "VALUES\n" );
	 fprintf( fp, "( (SELECT   max(fpid) FROM fpid_seq), \n" );
         fprintf( fp, "  (SELECT max(mtorid) FROM mtorid_seq), \n" );

         fprintf( fp, " 'MT', 'FULLMT',   %3.0f, %2.0f, %4.0f,   %3.0f, %2.0f, %4.0f, \n",
                sol[iz].Maj.P1.s, sol[iz].Maj.P1.d, sol[iz].Maj.P1.r,
                sol[iz].Maj.P2.s, sol[iz].Maj.P2.d, sol[iz].Maj.P2.r );

         fprintf( fp, " %5.2f, %.0f, %.0f,  %5.2f, %.0f, %.0f,  %5.2f, %.0f, %.0f, \n",
                sol[iz].FullMT.T.ev, sol[iz].Maj.T.az, sol[iz].Maj.T.pl,
                sol[iz].FullMT.P.ev, sol[iz].Maj.P.az, sol[iz].Maj.P.pl,
                sol[iz].FullMT.B.ev, sol[iz].Maj.B.az, sol[iz].Maj.B.pl );

         fprintf( fp, " '%s', %g, %g, %g, CURRENT_TIMESTAMP ); \n",
                username,
                sol[iz].Mdc + sol[iz].Mclvd,
                sol[iz].Miso,
                sol[iz].Mdc );

        }  /*** mt_type == FULL_MT     ***/
           /*** mt_type  ==  EXPLOSION ***/

        fprintf( fp, "\n" );

/***
	MT_ORIGIN STAGE table
***/
	jdate = ev[0].ot.year * 1000 + ev[0].ot.jday;
	
        fprintf( fp, "INSERT into %s \n", origin_table_name );
	fprintf( fp, " ( lat, lon, depth, time, orid, evid, jdate, algorithm, auth, lddate )\n" );
	fprintf( fp, "VALUES\n" );

            /***        lat lon dep time ***/    
        fprintf( fp, " ( %g, %g, %g, %lf, \n", 
                grn[0][iz].evla,
                grn[0][iz].evlo,
                grn[0][iz].evdp,
                ev[0].ot.epoch );

		/*** orid ***/
	fprintf( fp, "  (SELECT max(mtorid) FROM mtorid_seq), \n" );

	/***             evid jdate algor auth lddate ***/
	fprintf( fp, "  %ld, %d, '%s%s:%d', '%s', CURRENT_TIMESTAMP ); \n",
                evid,
                jdate,
                progname,
                Version_Label,
		mtdegfree,
                username );

        fprintf( fp, "\n" );

	fclose(fp);

/******************************************************************************************/
/*** for each station and channel write out database records to                         ***/
/*** MT_DATA, MT_WAVEFORM_SEGMENT MT_FILTER MT_EARTHMODEL and MT_EARTHMODEL_NODE tables ***/
/******************************************************************************************/

	if( ( fp = fopen( "insert.sql", "a" ) ) == NULL )
        {
                fprintf( stderr, "ERROR: cannot open file insert.sql\n" );
                exit(-1);
        }

	for( ista = 0; ista < nsta; ista++ )
	{
		used = '-';
		if( ev[ista].iused == 1 ) used = 'Y';
		if( ev[ista].iused  < 1 ) used = 'N';
	
		if( ev[ista].grd_mo_type == DISPLACEMENT )
			grd_mo_type = 'D';
                else if( ev[ista].grd_mo_type == VELOCITY )
                        grd_mo_type = 'V';
                else
                        grd_mo_type = '-';
 
/***
        insert into mt_data ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate )
***/
		fprintf( fp, "\n" );
		fprintf( fp, "INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );\n" );

		fprintf( fp, "\n" );
		fprintf( fp, "INSERT into %s\n", mt_data_table_name );
		fprintf( fp, " ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) \n" );
		fprintf( fp, "VALUES\n" );
		fprintf( fp, " ( ( SELECT max(mtdataid) FROM mtdataid_seq ), \n" );
		fprintf( fp, "   ( SELECT     max(mtid) FROM mtid_seq ), \n" );
		
		fprintf( fp, "   '%s', '%s', %.1f, %.0f, '%c', '%s', '%s', CURRENT_TIMESTAMP ); \n",
			grn[ista][iz].stnm,
                        grn[ista][iz].net,
                        grn[ista][iz].rdist,
                        grn[ista][iz].az,
                        used,
                        "-",
                        username 
                );

		fprintf( fp, "\n" );

/***

insert into mt_waveform_segment ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time,
  end_time, mtfilterid, redvel, tshift, distnorm, r0, distdeg, distkm, az, baz, interpsamprate, used, algor, lddate );

****/
		for( ichan = 0; ichan < 3; ichan++ )
                {
                        if( ichan == 0 ) strcpy( chan, "BHZ" );
                        if( ichan == 1 ) strcpy( chan, "BHR" );
                        if( ichan == 2 ) strcpy( chan, "BHT" );

			fprintf( fp, "\n" );
			fprintf( fp, "INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );\n" );

			if( ichan == 0 ) 
			{
				fprintf( fp, "INSERT into mtfilterid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );\n" );
				fprintf( fp, "INSERT into   mtvmodid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );\n" );
			}

			fprintf( fp, "\n" );
			fprintf( fp, "INSERT into %s\n", mt_waveform_segment_table_name );
			fprintf( fp, "  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, \n" );
                        fprintf( fp, "  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, \n" );
                        fprintf( fp, "  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) \n" );
			fprintf( fp, "VALUES\n" );

                        fprintf( fp, " ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), \n" );
			fprintf( fp, "   (SELECT  max(mtdataid) FROM mtdataid_seq), \n" );

			fprintf( fp, "    '%s-%c', '%s', '%s', %lf, %lf, ",
				"FULL",
				grd_mo_type, 
				grn[ista][iz].stnm,
				chan,
				ev[ista].z.beg.epoch,
				ev[ista].z.end.epoch );

			fprintf( fp, "\n" );

			fprintf( fp, "  (SELECT max(mtfilterid) FROM mtfilterid_seq), \n");
			fprintf( fp, "  (SELECT max(mtvmodid) FROM mtvmodid_seq), \n" );
	
		/***          redv tshift ampweight      r0=-1 RayP distdeg                         ***/
		/***                                distnorm='N'        rdistkm az  baz delta used  ***/
			fprintf( fp, "  %g, %g, %g, '%c', -1, %g, %.2f, %.1f, %.0f, %.0f, %g, '%c', ",  
                                ev[ista].redv,
                                ev[ista].time_shift_all,
                                ev[ista].weight,
                                'N',
                                grn[ista][iz].Prayparameter,
                                grn[ista][iz].rdist/111.13,
                                grn[ista][iz].rdist,
                                grn[ista][iz].az,
                                grn[ista][iz].baz,
                                ev[ista].ew.s.delta,
                                used );

			if( ichan == 0 )
			{
			  fprintf( fp, " %g, %g, %g, '%s',  CURRENT_TIMESTAMP ); \n",
                                ev[ista].z.P2P_snr,
                                ev[ista].zxcor,
                                ev[ista].ztlag,
                                "FK" );
			}
			if( ichan == 1 )
			{
			  fprintf( fp, " %g, %g, %g, '%s',  CURRENT_TIMESTAMP ); \n",
                                ev[ista].ns.P2P_snr,
                                ev[ista].rxcor,
                                ev[ista].rtlag,
                                "FK" );
			}
			if( ichan == 2 )
			{
			  fprintf( fp, " %g, %g, %g, '%s',  CURRENT_TIMESTAMP ); \n", 
				ev[ista].ew.P2P_snr,
				ev[ista].txcor,
				ev[ista].ttlag,
                                "FK" );
			}

			fprintf( fp, "\n" );

			if( ichan == 0 )
			{
				fprintf( fp, "\n" );
				fprintf( fp, "INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );\n" );

				fprintf( fp, "INSERT into %s\n", mt_phase_data_table_name );
				fprintf( fp, " ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )\n" );
				fprintf( fp, "VALUES\n" );
				fprintf( fp, " ( (SELECT     max(phaid) FROM     phaid_seq), \n" );
				fprintf( fp, "   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), \n" );
				fprintf( fp, "   '%s', '%s', '%s', %g, %g, %g, %g, %g, %g, %g );\n",
					"NOISE", 
					grn[ista][iz].stnm,
                                	chan,
					ev[ista].z.pha[NOISE].gvlo,
					ev[ista].z.pha[NOISE].gvhi,
					ev[ista].z.pha[NOISE].amp,
					ev[ista].z.pha[NOISE].time,
					ev[ista].z.pha[NOISE].duration,
					ev[ista].z.pha[NOISE].frequency,
					ev[ista].z.pha[NOISE].period );

				fprintf( fp, "\n" );
				fprintf( fp, "INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );\n" );

				fprintf( fp, "INSERT into %s\n", mt_phase_data_table_name );
                                fprintf( fp, " ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )\n" );
                                fprintf( fp, "VALUES\n" );
                                fprintf( fp, " ( (SELECT     max(phaid) FROM     phaid_seq), \n" );
				fprintf( fp, "   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), \n" );
                                fprintf( fp, "   '%s', '%s', '%s', %g, %g, %g, %g, %g, %g, %g );\n",
                                        "SIGNAL",
                                        grn[ista][iz].stnm,
                                        chan,
                                        ev[ista].z.pha[SIGNAL].gvlo,
                                        ev[ista].z.pha[SIGNAL].gvhi,
                                        ev[ista].z.pha[SIGNAL].amp,
                                        ev[ista].z.pha[SIGNAL].time,
                                        ev[ista].z.pha[SIGNAL].duration,
                                        ev[ista].z.pha[SIGNAL].frequency,
                                        ev[ista].z.pha[SIGNAL].period );

				fprintf( fp, "\n" );
			} /*** ichan == 0 .or. BHZ ***/

			if( ichan == 1 )
			{
				fprintf( fp, "\n" );
                                fprintf( fp, "INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );\n" );

                                fprintf( fp, "INSERT into %s\n", mt_phase_data_table_name );
                                fprintf( fp, " ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )\n" );
                                fprintf( fp, "VALUES\n" );
                                fprintf( fp, " ( (SELECT     max(phaid) FROM     phaid_seq), \n" );
                                fprintf( fp, "   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), \n" );
                                fprintf( fp, "   '%s', '%s', '%s', %g, %g, %g, %g, %g, %g, %g );\n",
                                        "NOISE",
                                        grn[ista][iz].stnm,
                                        chan,
                                        ev[ista].ns.pha[NOISE].gvlo,
                                        ev[ista].ns.pha[NOISE].gvhi,
                                        ev[ista].ns.pha[NOISE].amp,
                                        ev[ista].ns.pha[NOISE].time,
                                        ev[ista].ns.pha[NOISE].duration,
                                        ev[ista].ns.pha[NOISE].frequency,
                                        ev[ista].ns.pha[NOISE].period );

                                fprintf( fp, "\n" );
                                fprintf( fp, "INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );\n" );

                                fprintf( fp, "INSERT into %s\n", mt_phase_data_table_name );
                                fprintf( fp, " ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )\n" );
                                fprintf( fp, "VALUES\n" );
                                fprintf( fp, " ( (SELECT     max(phaid) FROM     phaid_seq), \n" );
                                fprintf( fp, "   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), \n" );
                                fprintf( fp, "   '%s', '%s', '%s', %g, %g, %g, %g, %g, %g, %g );\n",
                                        "SIGNAL",
                                        grn[ista][iz].stnm,
                                        chan,
                                        ev[ista].ns.pha[SIGNAL].gvlo,
                                        ev[ista].ns.pha[SIGNAL].gvhi,
                                        ev[ista].ns.pha[SIGNAL].amp,
                                        ev[ista].ns.pha[SIGNAL].time,
                                        ev[ista].ns.pha[SIGNAL].duration,
                                        ev[ista].ns.pha[SIGNAL].frequency,
                                        ev[ista].ns.pha[SIGNAL].period );
                                fprintf( fp, "\n" );

			} /*** ichan == 1 .or. ns BHR ****/

			if( ichan == 2 )
			{
				fprintf( fp, "\n" ); 
                                fprintf( fp, "INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );\n" );

                                fprintf( fp, "INSERT into %s\n", mt_phase_data_table_name );
                                fprintf( fp, " ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )\n" );
                                fprintf( fp, "VALUES\n" );
                                fprintf( fp, " ( (SELECT     max(phaid) FROM     phaid_seq), \n" );
                                fprintf( fp, "   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), \n" );
                                fprintf( fp, "   '%s', '%s', '%s', %g, %g, %g, %g, %g, %g, %g );\n",
                                        "NOISE",
                                        grn[ista][iz].stnm,
                                        chan,
                                        ev[ista].ew.pha[NOISE].gvlo,
                                        ev[ista].ew.pha[NOISE].gvhi,
                                        ev[ista].ew.pha[NOISE].amp,
                                        ev[ista].ew.pha[NOISE].time,
                                        ev[ista].ew.pha[NOISE].duration,
                                        ev[ista].ew.pha[NOISE].frequency,
                                        ev[ista].ew.pha[NOISE].period );

                                fprintf( fp, "\n" );
                                fprintf( fp, "INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );\n" );

                                fprintf( fp, "INSERT into %s\n", mt_phase_data_table_name );
                                fprintf( fp, " ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )\n" );
                                fprintf( fp, "VALUES\n" );
                                fprintf( fp, " ( (SELECT     max(phaid) FROM     phaid_seq), \n" );
                                fprintf( fp, "   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), \n" );
                                fprintf( fp, "   '%s', '%s', '%s', %g, %g, %g, %g, %g, %g, %g );\n",
                                        "SIGNAL",
                                        grn[ista][iz].stnm,
                                        chan,
                                        ev[ista].ew.pha[SIGNAL].gvlo,
                                        ev[ista].ew.pha[SIGNAL].gvhi,
                                        ev[ista].ew.pha[SIGNAL].amp,
                                        ev[ista].ew.pha[SIGNAL].time,
                                        ev[ista].ew.pha[SIGNAL].duration,
                                        ev[ista].ew.pha[SIGNAL].frequency,
                                        ev[ista].ew.pha[SIGNAL].period );
				fprintf( fp, "\n" );

			} /*** ichan == 2 .or. BHT ***/

/****
        insert into mt_filter ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass )	
		ALL Channels use the same filter 
****/
			if( ichan == 0 )
			{
				fprintf( fp, "INSERT into %s\n", mt_filter_table_name );
				fprintf( fp, "  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) \n" );
				fprintf( fp, "VALUES\n" );
				fprintf( fp, "  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), '%s', '%s', %d, %g, %g, %d );\n",
					"BP",
					"BUTTERWORTH",
					ev[ista].npole,
					ev[ista].lf,
					ev[ista].hf,
					ev[ista].npass );
			}

			fprintf( fp, "\n" );

/****
        insert into mt_earthmodel ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate )
		ALL Channels use same model 
****/
			if( ichan == 0 )
			{
			  fprintf( fp, "INSERT into %s\n", mt_earthmodel_table_name );
			  fprintf( fp, " ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) \n" );
			  fprintf( fp, "VALUES\n" );
			  fprintf( fp, "  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq), " );
			  fprintf( fp, "  '%s', %d, %d, %d, '%s', '%s', CURRENT_TIMESTAMP );\n",
                                "1-D", 1, 1, grn[ista][iz].v.nlay, grn[ista][iz].v.modfile, username );
			}

/*** 
  skip
  insert into mt_earthmodel_node ( nodeid, mtvmodid, ztop, thickness, vp, qp, vs, qs, rho, poissonratio ) VALUES
***/
		} /*** loop over ichan channel ***/

	} /*** loop over ista station ***/

	fprintf( fp, ".quit\n" );
        fclose(fp);
}

void db_oracle_write( EventInfo *ev, Solution *sol, Greens **grn, int nsta, int iz, int AutoAuth, int verbose )
{
	Moment *moment;
	Focal_Plane *focal_plane;
	MT_Data *mt_data;
	MT_Filter *mt_filter;
	Origin *origin;
	extern long evid;

	int ista, ichan, ilay;
	char chan[8];
	char mt_type[24];
	char grd_mo_type;
	int mtdegfree;
	char used;
	FILE *fp;
	char *username;
	MyTime *now;
	long orid,fpid,mtid,mtdataid,mtwfsegid,mtfilterid,mtvmodid,nodeid;
	int jdate;

	float mxx, myy, mzz, mxy, mxz, myz;
        float DyneCm_to_Netwon_M = 1.0E-07;

	char moment_table_name[256];
        char focal_plane_table_name[256];
        char origin_table_name[256];
        char mt_data_table_name[256];
        char mt_waveform_segment_table_name[256];
        char mt_filter_table_name[256];
        char mt_earthmodel_table_name[256];
        char mt_earthmodel_node_table_name[256];

/*** get the username from shell environment ***/
	username = calloc(32,sizeof(char));

	if( AutoAuth )
		sprintf( username, "AutoMT:%s", ev[0].dbuser );
	else
		username = getenv( "USER" );
	username[15] = '\0';

/*** get the system time now ***/

	now = (MyTime *)calloc( 1, sizeof(MyTime) );
	now = myGMTtime( now );

/*** set some defaults test id's ***/
/*** sql insert statements use nextval or currval from oracle sequence table ***/

        orid = 10001;
        fpid =   101;
        mtid =    11;
        mtdataid = 2;
        mtwfsegid = 1;
        mtfilterid = 3;
        mtvmodid  = 100;
        nodeid = 1;

/*********** set moment tensor degrees of freedom here ****************************/

	if( sol[iz].mt_type == EXPLOSION  ) mtdegfree = 1;
        if( sol[iz].mt_type == DEVIATORIC ) mtdegfree = 5;
        if( sol[iz].mt_type == FULL_MT    ) mtdegfree = 6;

        if( mtdegfree == 1 ) sprintf( mt_type, "ISOMT" );
        if( mtdegfree == 5 ) sprintf( mt_type, "DEVMT" );
        if( mtdegfree == 6 ) sprintf( mt_type, "FULLMT" );

/*** set table names here ***/

	sprintf( moment_table_name, "MOMENT_STAGE" );
	sprintf( focal_plane_table_name, "FOCAL_PLANE_STAGE" );
	sprintf( origin_table_name, "MT_ORIGIN_STAGE" );
	sprintf( mt_data_table_name, "MT_DATA_STAGE" );
	sprintf( mt_waveform_segment_table_name, "MT_WAVEFORM_SEGMENT_STAGE" );
	sprintf( mt_filter_table_name, "MT_FILTER_STAGE" );
	sprintf( mt_earthmodel_table_name, "MT_EARTHMODEL_STAGE" );
	sprintf( mt_earthmodel_node_table_name, "MT_EARTHMODEL_NODE_STAGE" );

/**********************************************************************************************************
        insert into moment ( mtid, fpid, orid, mxx, myy, mzz, mxy, mxz, myz,
                mt_type, mo, momag, piso, pclvd, pdc, kiso, kclvd, epsilon, f_factor,
                var_red, residual, nslpb, nrlpb, nssurf, nrsurf, tmnsurf,
                half_duration, srcfunc, algorithm, epi_fixed, time_fixed, depth_fixed, auth, lddate );
***********************************************************************************************************/

	fp = fopen( "insert.sql", "w");

	fprintf( fp, "REM\n" );
        fprintf( fp, "REM These are autogenerated statements from %s table_name=%s\n", 
		progname, moment_table_name );
        fprintf( fp, "REM\n" );
	fprintf( fp, "\n\n" );

	fprintf( fp, "insert into %s ( mtid, fpid, orid, mxx, myy, mzz, mxy, mxz, myz, \n", moment_table_name );
        fprintf( fp, "  mt_type, mo, momag, piso, pclvd, pdc, kiso, kclvd, epsilon, f_factor, \n" );
        fprintf( fp, " var_red, residual, \n" );
        fprintf( fp, " algorithm, epi_fixed, time_fixed, depth_fixed, auth, lddate ) VALUES \n" );
	fprintf( fp, " ( mtid_seq.nextval, fpid_seq.nextval, mtorid_seq.nextval, " );


	mxx  = sol[iz].moment_tensor[1][1] * DyneCm_to_Netwon_M;
        myy  = sol[iz].moment_tensor[2][2] * DyneCm_to_Netwon_M;
        mxy  = sol[iz].moment_tensor[1][2] * DyneCm_to_Netwon_M;
        mxz  = sol[iz].moment_tensor[1][3] * DyneCm_to_Netwon_M;
        myz  = sol[iz].moment_tensor[2][3] * DyneCm_to_Netwon_M;
        mzz  = sol[iz].moment_tensor[3][3] * DyneCm_to_Netwon_M;

	fprintf( fp, "    %6.2e, %6.2e, %6.2e, %6.2e, %6.2e, %6.2e, \n",
		mxx, myy, mzz, mxy, mxz, myz );

/**** this is for Oracle ****/
/**** this is not correct coversion from cartesian to spherical coordinate systems need to fix or use below ****/
/****
	fprintf( fp, "    %6.2e, %6.2e, %6.2e, %6.2e, %6.2e, %6.2e, \n",
                sol[iz].mrr * pow( 10.0, sol[iz].exponent ),
                sol[iz].mtt * pow( 10.0, sol[iz].exponent ),
                sol[iz].mff * pow( 10.0, sol[iz].exponent ),
                sol[iz].mrt * pow( 10.0, sol[iz].exponent ),
                -sol[iz].mrf * pow( 10.0, sol[iz].exponent ),
                -sol[iz].mtf * pow( 10.0, sol[iz].exponent )
         );
****/

	/***   type  Mo     Mw     PDC   PCLVD   PISO   kiso   kclvd  eps    f_fac */
        fprintf( fp, " '%s', %8.2e, %8.2f, %3.0f, %3.0f, %3.0f, %8.2f, %7.2f, %8.3f, %9.4f, \n",
                mt_type,
                sol[iz].dmoment,
                sol[iz].mw,
                sol[iz].PDC,
                sol[iz].PCLVD,
                sol[iz].PISO,
                sol[iz].k,
                -999.0,
                sol[iz].epsilon,
                sol[iz].f_factor );
 
 /* fprintf( fp, " %6.2f, %5.2e, '%s%s', 'Y', 'N', 'N', '%s', '%4d-%02d-%02d %02d:%02d:%02d' ); \n", */
	fprintf( fp, " %6.2f, %5.2e, '%s%s', 'Y', 'N', 'N', '%s', sysdate ); \n",
                sol[iz].var_red,
                sol[iz].l2norm_error,
                progname,
                Version_Label,
                username );

        fprintf( fp, "\n" );
        fclose(fp);

/***********************************************************************************************
        insert into focal_plane ( fpid, orid, fp_type, decomp_type, strike1, dip1, rake1,
                strike2, dip2, rake2, eval1, az1, pl1, eval2, az2, pl2, eval3, az3, pl3,
                auth, devmo, isomo, dcmo, ldate );
*************************************************************************************************/

	fp = fopen( "insert.sql", "a+");

	fprintf( fp, "REM\n" );
	fprintf( fp, "REM These are autogenerated statements from %s for table %s\n", 
		progname, focal_plane_table_name );
	fprintf( fp, "REM\n" );

/********************************************/
/*** DEVIATORIC   major double couple     ***/
/********************************************/

	if( sol[iz].mt_type == DEVIATORIC || mtdegfree == 5 ) 
	{
	  fprintf( fp, "insert into %s ( fpid, orid, fp_type, decomp_type,  \n", focal_plane_table_name );
          fprintf( fp, " strike1, dip1, rake1, strike2, dip2, rake2, \n" );
          fprintf( fp, " eval1, az1, pl1, eval2, az2, pl2, eval3, az3, pl3, \n" );
          fprintf( fp, " auth, devmo, isomo, dcmo, lddate ) VALUES \n" );

	  fprintf( fp, " ( fpid_seq.currval, mtorid_seq.currval, " );
	  fprintf( fp, " 'MT', 'MAJORDC',   %3.0f, %2.0f, %4.0f,   %3.0f, %2.0f, %4.0f, \n",
                sol[iz].Maj.P1.s, sol[iz].Maj.P1.d, sol[iz].Maj.P1.r,
                sol[iz].Maj.P2.s, sol[iz].Maj.P2.d, sol[iz].Maj.P2.r );
 
          fprintf( fp, " %5.2f, %.0f, %.0f,  %5.2f, %.0f, %.0f,  %5.2f, %.0f, %.0f, \n",
                sol[iz].Maj.T.ev, sol[iz].Maj.T.az, sol[iz].Maj.T.pl,
                sol[iz].Maj.P.ev, sol[iz].Maj.P.az, sol[iz].Maj.P.pl,
                sol[iz].Maj.B.ev, sol[iz].Maj.B.az, sol[iz].Maj.B.pl );
 
          fprintf( fp, " '%s', %g, %g, %g, sysdate ); \n",
                username,
                sol[iz].Mdc + sol[iz].Mclvd,
                sol[iz].Miso,
                sol[iz].Mdc );
	
	  fprintf( fp, "\n" );

	} /*** mt_type == DEVIATORIC ***/

/**************************************************/
/*** FULL_MT           full moment tensor       ***/
/*** EXPLOSION     isotropic only moment tensor ***/
/**************************************************/

	if( ( sol[iz].mt_type == FULL_MT    || mtdegfree == 6 ) || 
	    ( sol[iz].mt_type == EXPLOSION  || mtdegfree == 1 ) )
	{

	 fprintf( fp, "insert into %s ( fpid, orid, fp_type, decomp_type,  \n", focal_plane_table_name );
         fprintf( fp, " strike1, dip1, rake1, strike2, dip2, rake2, \n" );
         fprintf( fp, " eval1, az1, pl1, eval2, az2, pl2, eval3, az3, pl3, \n" );
         fprintf( fp, " auth, devmo, isomo, dcmo, lddate ) VALUES \n" );

	 fprintf( fp, " ( fpid_seq.currval, mtorid_seq.currval, " );
	 fprintf( fp, " 'MT', 'FULLMT',   %3.0f, %2.0f, %4.0f,   %3.0f, %2.0f, %4.0f, \n",
                sol[iz].Maj.P1.s, sol[iz].Maj.P1.d, sol[iz].Maj.P1.r,
                sol[iz].Maj.P2.s, sol[iz].Maj.P2.d, sol[iz].Maj.P2.r );

	 fprintf( fp, " %5.2f, %.0f, %.0f,  %5.2f, %.0f, %.0f,  %5.2f, %.0f, %.0f, \n",
                sol[iz].FullMT.T.ev, sol[iz].Maj.T.az, sol[iz].Maj.T.pl,
                sol[iz].FullMT.P.ev, sol[iz].Maj.P.az, sol[iz].Maj.P.pl,
                sol[iz].FullMT.B.ev, sol[iz].Maj.B.az, sol[iz].Maj.B.pl );

         fprintf( fp, " '%s', %g, %g, %g, sysdate ); \n",
                username,
                sol[iz].Mdc + sol[iz].Mclvd,
                sol[iz].Miso,
                sol[iz].Mdc );

	}  /*** mt_type == FULL_MT     ***/
	   /*** mt_type  ==  EXPLOSION ***/

	fprintf( fp, "\n" );

	fclose(fp);

/***********************************************************************************************/
/***
        insert into origin ( lat, lon, depth, time, orid, evid, jdate, nass, ndef,
                ndp, grn, srn, etype, depdp, dtype, mb, mbid, ms, msid, ml, mlid,
                algorithm, auth, lddate );
***/

	fp = fopen( "insert.sql", "a+");

	fprintf( fp, "\n" );

	fprintf( fp, "insert into %s ( lat, lon, depth, time, orid, evid, jdate, \n",origin_table_name );
	fprintf( fp, " algorithm, auth, lddate ) VALUES \n" );

	jdate = ev[0].ot.year * 1000 + ev[0].ot.jday;

	    /***    lat  lon dep time orid evid jdate algor auth lddate ***/
        fprintf( fp, " ( %g, %g, %g, %lf, mtorid_seq.currval, %ld, %d, '%s%s', '%s', sysdate ); \n",
                grn[0][iz].evla,
                grn[0][iz].evlo,
                grn[0][iz].evdp,
                ev[0].ot.epoch,
                ev[0].evid,
                jdate,
                progname,
                Version_Label,
                username );

	fprintf( fp, "\n" );

	fclose(fp);

/******************************************************************************************/
/*** for each station and channel write out database records to                         ***/
/*** MT_DATA, MT_WAVEFORM_SEGMENT MT_FILTER MT_EARTHMODEL and MT_EARTHMODEL_NODE tables ***/
/******************************************************************************************/

	fp = fopen( "insert.sql", "a+" );

	for( ista = 0; ista < nsta; ista++ )
	{
		used = '-';
		if( ev[ista].iused == 1 ) used = 'Y';
		if( ev[ista].iused  < 1 ) used = 'N';

		if( ev[ista].grd_mo_type == DISPLACEMENT )
                        grd_mo_type = 'D';
                else if( ev[ista].grd_mo_type == VELOCITY )
                        grd_mo_type = 'V';
                else
                        grd_mo_type = '-';
 
/***
        insert into mt_data ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate )
***/

		fprintf( fp, "\n" );

		fprintf( fp,
		  "insert into %s ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) VALUES\n",
			mt_data_table_name );
		
		fprintf( fp, " ( mtdataid_seq.nextval, mtid_seq.currval, '%s', '%s', %.1f, %.0f, '%c', '%s', '%s', sysdate ); \n",
			/* mtdataid + ista */
			/* mtid, */
			grn[ista][iz].stnm,
			grn[ista][iz].net,
			grn[ista][iz].rdist,
			grn[ista][iz].az,
			used,
			"-",
			username 
		);

		fprintf( fp, "\n" );

		for( ichan = 0; ichan < 3; ichan++ )
		{
                	if( ichan == 0 ) strcpy( chan, "BHZ" );
                	if( ichan == 1 ) strcpy( chan, "BHR" );
                	if( ichan == 2 ) strcpy( chan, "BHT" );

/***
        insert into mt_waveform_segment ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time,
        end_time, mtfilterid, redvel, tshift, distnorm, r0, distdeg, distkm, az, baz, interpsamprate, used, algor, lddate );
****/
	
			fprintf( fp, "\n" );
			
			fprintf( fp, 
				"insert into %s ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, \n", 
				mt_waveform_segment_table_name );

			fprintf( fp, " end_time, mtfilterid, mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, \n " );
			fprintf( fp, " distkm, az, baz, interpsamprate, used, algor, lddate ) VALUES \n" );
			fprintf( fp, " ( mtwfsegid_seq.nextval, mtdataid_seq.currval, '%s-%c', '%s', '%s', %lf, %lf, ",
				"FULL", grd_mo_type, grn[ista][iz].stnm, chan, ev[ista].z.beg.epoch, ev[ista].z.end.epoch );

			if( ichan == 0 )
				fprintf( fp, "mtfilterid_seq.nextval, mtvmodid_seq.nextval, " );
			else
				fprintf( fp, "mtfilterid_seq.currval, mtvmodid_seq.currval, " );

			fprintf( fp, " %g, %g, %g, '%c', -1, %g, %.2f, %.1f, %.0f, %.0f, %g, '%c', '%s', sysdate ); \n",
				ev[ista].redv,
				ev[ista].time_shift_all,
				ev[ista].weight,
				'N',
				grn[ista][iz].Prayparameter,
				grn[ista][iz].rdist/111.13,
				grn[ista][iz].rdist,
				grn[ista][iz].az,
				grn[ista][iz].baz,
				ev[ista].ew.s.delta,
				used,
				"FK" 
			);
	
			fprintf( fp, "\n" );
/****
        insert into mt_filter ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass )
****/
			if( ichan == 0 )
			{
			  fprintf( fp, 
			    "insert into %s ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) VALUES \n",
				mt_filter_table_name );
			  fprintf( fp, "( mtfilterid_seq.currval, '%s', '%s', %d, %g, %g, %d ); \n",
				"BP", "BUTTERWORTH", ev[ista].npole, ev[ista].lf, ev[ista].hf, ev[ista].npass );
			}

/****
        insert into mt_earthmodel ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate )
****/

			if( ichan == 0 )
			{
			  fprintf( fp, 
				"insert into %s ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) VALUES \n",
				mt_earthmodel_table_name );

			  fprintf( fp, "( mtvmodid_seq.currval, '%s', %d, %d, %d, '%s', '%s', sysdate ); \n",
				"1-D", 1, 1, grn[ista][iz].v.nlay, grn[ista][iz].v.modfile, username );
			}

/***
        insert into mt_earthmodel_node ( nodeid, mtvmodid, ztop, thickness, vp, qp, vs, qs, rho, poissonratio ) VALUES \n",
***/

			if( ichan == 0 )
			{
			  for( ilay = 0; ilay < grn[ista][iz].v.nlay; ilay++ )
			  {
				fprintf( fp, 
					"insert into %s ( nodeid, mtvmodid, ztop, \n", 
					mt_earthmodel_node_table_name );

				fprintf( fp, " thickness, vp, qp, vs, qs, rho, poissonratio ) VALUES \n" );

				fprintf( fp, "( nodeid_seq.nextval, mtvmodid_seq.currval, %g, %g, %g, %g, %g, %g, %g, %g ); \n",
					grn[ista][iz].v.ztop[ilay],
                                	grn[ista][iz].v.thick[ilay],
                                	grn[ista][iz].v.vp[ilay],
                                	grn[ista][iz].v.qa[ilay],
                                	grn[ista][iz].v.vs[ilay],
                                	grn[ista][iz].v.qb[ilay],
                                	grn[ista][iz].v.rho[ilay],
                                	grn[ista][iz].v.sigma[ilay] );

			  } /*** loop over layer ***/
			}


		} /*** loop over ichan channel ***/

	} /*** loop over ista station ***/

	fprintf( fp, "quit\n" );
	fclose(fp);

/*** show rows ***/
	
/*** 
	fp = fopen( "insert.sql", "a+" );
	fprintf( fp, "set linesize 800\n" );

	fprintf( fp, "select * from %s;\n", moment_table_name);
	fprintf( fp, "select * from %s;\n", focal_plane_table_name );
	fprintf( fp, "select * from %s;\n", origin_table_name );
	fprintf( fp, "select * from %s;\n", mt_data_table_name );
	fprintf( fp, "select * from %s;\n", mt_waveform_segment_table_name );
	fprintf( fp, "select * from %s;\n", mt_filter_table_name );
	fprintf( fp, "select * from %s;\n", mt_earthmodel_table_name );
	fprintf( fp, "select * from %s;\n", mt_earthmodel_node_table_name );
	fprintf( fp, "quit\n" );
	fclose(fp);
***/

}

void db_mysql_write( EventInfo *ev, Solution *sol, Greens **grn, int nsta, int iz, int AutoAuth, int verbose )
{
	Moment *moment;
	Focal_Plane *focal_plane;
	MT_Data *mt_data;
	MT_Filter *mt_filter;
	Origin *origin;
	extern long evid;

	float mxx, myy, mzz, mxy, mxz, myz;
        float DyneCm_to_Netwon_M = 1.0E-07;

	int ista, ichan, ilay;
	char chan[8];
	char mt_type[24];
	char grd_mo_type;
	int mtdegfree;
	char used;
	FILE *fp;
        char *username;
        MyTime *now;
        long orid,fpid,mtid,mtdataid,mtwfsegid,mtfilterid,mtvmodid,nodeid;
        int jdate;

	char moment_table_name[256];
	char focal_plane_table_name[256];
	char origin_table_name[256];
	char mt_data_table_name[256];
        char mt_waveform_segment_table_name[256];
        char mt_filter_table_name[256];
	char mt_earthmodel_table_name[256];
	char mt_earthmodel_node_table_name[256];

/*** get the username from shell environment ***/
	username = calloc(32,sizeof(char));
        username = getenv( "USER" );

/*** get the system time now ***/
        now = (MyTime *)calloc( 1, sizeof(MyTime) );
        now = myGMTtime( now );

	orid = 10001;
	fpid =   101;
	mtid =    11;
	mtdataid = 2;
	mtwfsegid = 1;
	mtfilterid = 3;
	mtvmodid  = 100;
	nodeid = 1;

	sprintf( moment_table_name, "MOMENT_STAGE" );
	sprintf( focal_plane_table_name, "FOCAL_PLANE_STAGE" );
	sprintf( origin_table_name, "MT_ORIGIN_STAGE" );
	sprintf( mt_data_table_name, "MT_DATA_STAGE" );
        sprintf( mt_waveform_segment_table_name, "MT_WAVEFORM_SEGMENT_STAGE" );
        sprintf( mt_filter_table_name, "MT_FILTER_STAGE" );
	sprintf( mt_earthmodel_table_name, "MT_EARTHMODEL_STAGE" );
	sprintf( mt_earthmodel_node_table_name, "MT_EARTHMODEL_NODE_STAGE" );

/**********************************************************************************************************
	insert into moment ( mtid, fpid, orid, mxx, myy, mzz, mxy, mxz, myz, 
		mt_type, mo, momag, piso, pclvd, pdc, kiso, kclvd, epsilon, f_factor,
		var_red, residual, nslpb, nrlpb, nssurf, nrsurf, tmnsurf, 
		half_duration, srcfunc, algorithm, epi_fixed, time_fixed, depth_fixed, auth, lddate );
***********************************************************************************************************/

	fp = fopen( "insert.sql", "w");

	fprintf( fp, "\n\n" );

	fprintf( fp, "insert into %s ( mtid, fpid, orid, mxx, myy, mzz, mxy, mxz, myz, \n", moment_table_name );
	fprintf( fp, "  mt_type, mo, momag, piso, pclvd, pdc, kiso, kclvd, epsilon, f_factor, \n" );
	fprintf( fp, " var_red, residual, \n" );
	fprintf( fp, " algorithm, epi_fixed, time_fixed, depth_fixed, auth, lddate ) VALUES \n" );

	/* fprintf( fp, " ( %ld, %ld, %ld, %6.2e, %6.2e, %6.2e, %6.2e, %6.2e, %6.2e, \n", */
		/* mtid, fpid, orid,  */

	mxx  = sol[iz].moment_tensor[1][1] * DyneCm_to_Netwon_M;
        myy  = sol[iz].moment_tensor[2][2] * DyneCm_to_Netwon_M;
        mxy  = sol[iz].moment_tensor[1][2] * DyneCm_to_Netwon_M;
        mxz  = sol[iz].moment_tensor[1][3] * DyneCm_to_Netwon_M;
        myz  = sol[iz].moment_tensor[2][3] * DyneCm_to_Netwon_M;
        mzz  = sol[iz].moment_tensor[3][3] * DyneCm_to_Netwon_M;

	fprintf( fp, " ( NULL, NULL, NULL, %6.2e, %6.2e, %6.2e, %6.2e, %6.2e, %6.2e, \n",
		 mxx, myy, mzz, mxy, mxz, myz );

/*** this is for mysql ****/
/**** this is not correct coversion from cartesian to spherical coordinate systems need to fix or use below ****/
/****
	fprintf( fp, " ( NULL, NULL, NULL, %6.2e, %6.2e, %6.2e, %6.2e, %6.2e, %6.2e, \n",
		sol[iz].mrr * pow( 10.0, sol[iz].exponent ),
                sol[iz].mtt * pow( 10.0, sol[iz].exponent ),
                sol[iz].mff * pow( 10.0, sol[iz].exponent ),
                sol[iz].mrt * pow( 10.0, sol[iz].exponent ),
                -sol[iz].mrf * pow( 10.0, sol[iz].exponent ),
                -sol[iz].mtf * pow( 10.0, sol[iz].exponent )
	 );
****/

	if( sol[iz].mt_type == EXPLOSION  ) mtdegfree = 1;
        if( sol[iz].mt_type == DEVIATORIC ) mtdegfree = 5;
        if( sol[iz].mt_type == FULL_MT    ) mtdegfree = 6;

	if( mtdegfree == 1 ) sprintf( mt_type, "ISOMT" );
	if( mtdegfree == 5 ) sprintf( mt_type, "DEVMT" );
	if( mtdegfree == 6 ) sprintf( mt_type, "FULLMT" );

		/***   type  Mo     Mw     PDC   PCLVD   PISO   kiso   kclvd  eps    f_fac */
	fprintf( fp, " '%s', %8.2e, %8.2f, %3.0f, %3.0f, %3.0f, %8.2f, %7.2f, %8.3f, %9.4f, \n",
		mt_type,
		sol[iz].dmoment,
		sol[iz].mw,
		sol[iz].PDC,
                sol[iz].PCLVD,
                sol[iz].PISO,
	 	sol[iz].k, 
		-999.0, 
		sol[iz].epsilon, 
		sol[iz].f_factor );

	fprintf( fp, " %6.2f, %5.2e, '%s%s', 'Y', 'N', 'N', '%s', '%4d-%02d-%02d %02d:%02d:%02d' ); \n", 
		sol[iz].var_red,
		sol[iz].l2norm_error, 
		progname, 
		Version_Label, 
		username, 
		now->year, now->month, now->mday, now->hour, now->min, now->isec );
	fprintf( fp, "\n" );
	fclose(fp);
	
/***********************************************************************************************
	insert into focal_plane ( fpid, orid, fp_type, decomp_type, strike1, dip1, rake1, 
		strike2, dip2, rake2, eval1, az1, pl1, eval2, az2, pl2, eval3, az3, pl3,
		auth, devmo, isomo, dcmo, ldate );
*************************************************************************************************/
        fp = fopen( "insert.sql", "a+");

/*** major double couple ***/

	fprintf( fp, "\n" );
        fprintf( fp, "insert into %s ( fpid, orid, fp_type, decomp_type,  \n", focal_plane_table_name );
        fprintf( fp, " strike1, dip1, rake1, strike2, dip2, rake2, \n" );
	fprintf( fp, " eval1, az1, pl1, eval2, az2, pl2, eval3, az3, pl3, \n" );
	fprintf( fp, " auth, devmo, isomo, dcmo, lddate ) VALUES \n" );

        fprintf( fp, " ( %ld, %ld, 'MT', 'MAJORDC',   %3.0f, %2.0f, %4.0f,   %3.0f, %2.0f, %4.0f, \n",
		fpid, 
		orid, 
		sol[iz].Maj.P1.s, sol[iz].Maj.P1.d, sol[iz].Maj.P1.r,
		sol[iz].Maj.P2.s, sol[iz].Maj.P2.d, sol[iz].Maj.P2.r );

	fprintf( fp, " %5.2f, %.0f, %.0f,  %5.2f, %.0f, %.0f,  %5.2f, %.0f, %.0f, \n", 
		sol[iz].Maj.T.ev, sol[iz].Maj.T.az, sol[iz].Maj.T.pl, 
		sol[iz].Maj.P.ev, sol[iz].Maj.P.az, sol[iz].Maj.P.pl, 
		sol[iz].Maj.B.ev, sol[iz].Maj.B.az, sol[iz].Maj.B.pl ); 

	fprintf( fp, " '%s', %g, %g, %g, '%4d-%02d-%02d %02d:%02d:%02d' ); \n",
		username, 
		sol[iz].Mdc + sol[iz].Mclvd, 
		sol[iz].Miso,
		sol[iz].Mdc,
		now->year, now->month, now->mday, now->hour, now->min, now->isec
	   );
	fprintf( fp, "\n" );

/*** full moment tensor ***/

	fprintf( fp, "insert into %s ( fpid, orid, fp_type, decomp_type,  \n", focal_plane_table_name );
        fprintf( fp, " strike1, dip1, rake1, strike2, dip2, rake2, \n" );
        fprintf( fp, " eval1, az1, pl1, eval2, az2, pl2, eval3, az3, pl3, \n" );
        fprintf( fp, " auth, devmo, isomo, dcmo, lddate ) VALUES \n" );
	fprintf( fp, " ( %ld, %ld, 'MT', 'FULLMT',   %3.0f, %2.0f, %4.0f,   %3.0f, %2.0f, %4.0f, \n",
                fpid + 1,
                orid,
                sol[iz].Maj.P1.s, sol[iz].Maj.P1.d, sol[iz].Maj.P1.r,
                sol[iz].Maj.P2.s, sol[iz].Maj.P2.d, sol[iz].Maj.P2.r );
                                                                                                                                                                        
        fprintf( fp, " %5.2f, %.0f, %.0f,  %5.2f, %.0f, %.0f,  %5.2f, %.0f, %.0f, \n",
                sol[iz].FullMT.T.ev, sol[iz].Maj.T.az, sol[iz].Maj.T.pl,
                sol[iz].FullMT.P.ev, sol[iz].Maj.P.az, sol[iz].Maj.P.pl,
                sol[iz].FullMT.B.ev, sol[iz].Maj.B.az, sol[iz].Maj.B.pl );
                                                                                                                                                                        
        fprintf( fp, " '%s', %g, %g, %g, '%4d-%02d-%02d %02d:%02d:%02d' ); \n",
                username,
                sol[iz].Mdc + sol[iz].Mclvd,
                sol[iz].Miso,
                sol[iz].Mdc,
                now->year, now->month, now->mday, now->hour, now->min, now->isec
           );
        fprintf( fp, "\n" );

	fclose(fp);

	
/***
	insert into origin ( lat, lon, depth, time, orid, evid, jdate, nass, ndef, 
		ndp, grn, srn, etype, depdp, dtype, mb, mbid, ms, msid, ml, mlid,
		algorithm, auth, lddate );
***/

        fp = fopen( "insert.sql", "a+");
	fprintf( fp, "\n" );
	fprintf( fp, "insert into %s ( lat, lon, depth, time, orid, evid, jdate, \n",origin_table_name );
	fprintf( fp, " algorithm, auth, lddate ) VALUES \n" ); 
	
	evid = 999;
	jdate = ev[0].ot.year * 1000 + ev[0].ot.jday;

		/***    lat  lon dep time orid evid jdate algor auth lddate ***/
	fprintf( fp, " ( %g, %g, %g, %lf, %ld, %ld, %d, '%s%s', '%s', '%4d-%02d-%02d %02d:%02d:%02d' ); \n",
		grn[0][iz].evla, 
		grn[0][iz].evlo, 
		grn[0][iz].evdp,
		ev[0].ot.epoch, 
		orid, 
		evid,
		jdate,
		progname,
		Version_Label,
		username,
                now->year, now->month, now->mday, now->hour, now->min, now->isec );
	fprintf( fp, "\n" );
        fclose(fp);

/***
	insert into mt_data  ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
***/

	fp = fopen( "insert.sql", "a+");

	for( ista = 0; ista < nsta; ista++ )
	{
		used = '-';
		if( ev[ista].iused == 1 ) used = 'Y';
		if( ev[ista].iused  < 1 ) used = 'N';

		fprintf( fp, "\n" );
        	fprintf( fp, "insert into %s ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) VALUES\n", mt_data_table_name );
		fprintf( fp, " ( %ld, %ld, '%s', '%s', %.1f, %.0f, '%c', '%s', '%s', '%4d-%02d-%02d %02d:%02d:%02d' ); \n",
			mtdataid + ista,
			mtid,
			grn[ista][iz].stnm,
			grn[ista][iz].net,
			grn[ista][iz].rdist, 
			grn[ista][iz].az,
			used,
			"-",
			username,
			now->year, now->month, now->mday, now->hour, now->min, now->isec );
		fprintf( fp, "\n" );
	}
	fprintf( fp, "\n" );
        fclose(fp);

/*** 
	insert into mt_waveform_segment ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, 
	end_time, mtfilterid, redvel, tshift, distnorm, r0, distdeg, distkm, az, baz, interpsamprate, used, algor, lddate );
****/
	
	fp = fopen( "insert.sql", "a+");
	for( ista = 0; ista < nsta; ista++ )
	{
	  for( ichan = 0; ichan < 3; ichan++ )
	  {
		used = '-';
		if( ev[ista].iused == 1 ) used = 'Y';
		if( ev[ista].iused == 0 ) used = 'N';

		if( ev[ista].grd_mo_type == DISPLACEMENT ) 
			grd_mo_type = 'D';
                else if( ev[ista].grd_mo_type == VELOCITY )
			grd_mo_type = 'V';
		else
			grd_mo_type = '-';

		if( ichan == 0 ) strcpy( chan, "BHZ" );
		if( ichan == 1 ) strcpy( chan, "BHR" );
		if( ichan == 2 ) strcpy( chan, "BHT" );
	
		fprintf( fp, "\n" );
		fprintf( fp, "insert into %s ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, \n", mt_waveform_segment_table_name );
		fprintf( fp, "  end_time, mtfilterid, mtvmodid, redvel, tshift, distnorm, distdeg, \n" );
		fprintf( fp, "  distkm, az, baz, interpsamprate, used, algor, lddate ) VALUES \n" );

	fprintf( fp, "( %ld, %ld, '%s-%c', '%s', '%s', %lf, %lf, %ld, %ld, %g, %g, '%c', %.2f, %.1f, %.0f, %.0f, %g, '%c', '%s', '%4d-%02d-%02d %02d:%02d:%02d' ); \n",
			mtwfsegid++,
			mtdataid + ista,
			"FULL",
			grd_mo_type,
			grn[ista][iz].stnm,
			chan,
			ev[ista].z.beg.epoch,
			ev[ista].z.end.epoch,
			mtfilterid + ista,
			mtvmodid + ista,
			ev[ista].redv,
			ev[ista].time_shift_all,
			'N',
			grn[ista][iz].rdist/111.13,
			grn[ista][iz].rdist,
			grn[ista][iz].az, 
			grn[ista][iz].baz,
			ev[ista].ew.s.delta,
			used,
			"FK",
			now->year, now->month, now->mday, now->hour, now->min, now->isec );
		fprintf( fp, "\n" );
	  }
	  fprintf( fp, "\n" );
	}
	fprintf( fp, "\n" );
        fclose(fp);

/*** 
	insert into mt_filter ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass )
****/
	fp = fopen( "insert.sql", "a+");
 	fprintf( fp, "\n" );
        for( ista = 0; ista < nsta; ista++ )
        {
		fprintf( fp, "insert into %s ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) VALUES \n", mt_filter_table_name );
		fprintf( fp, "( %ld, '%s', '%s', %d, %g, %g, %d ); \n",
			mtfilterid + ista,
			"BP", 
			"BUTTERWORTH", 
			ev[ista].npole,
			ev[ista].lf,
			ev[ista].hf,
			ev[ista].npass );
	}
	fprintf( fp, "\n" );
        fclose(fp);

/****
	insert into mt_earthmodel ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
****/
	fp = fopen( "insert.sql", "a+");
	fprintf( fp, "\n" );
	for( ista = 0; ista < nsta; ista++ )
	{
		fprintf( fp, "insert into %s ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) VALUES \n",
			mt_earthmodel_table_name );

		fprintf( fp, "( %ld, '%s', %d, %d, %d, '%s', '%s', '%4d-%02d-%02d %02d:%02d:%02d' ); \n",
			mtvmodid + ista,
			"1-D",
			1,
			1,
			grn[ista][iz].v.nlay,
			grn[ista][iz].v.modfile,
			username,
			now->year, now->month, now->mday, now->hour, now->min, now->isec );
		fprintf( fp, "\n" );
	}
	fprintf( fp, "\n" );
	fclose(fp);

/***
	insert into mt_earthmodel_node ( nodeid, mtvmodid, ztop, thickness, vp, qp, vs, qs, rho, poissonratio ) VALUES \n",
***/
	fp = fopen( "insert.sql", "a+");
	fprintf( fp, "\n" );
	for( ista = 0; ista < nsta; ista++ )
	{
		for( ilay = 0; ilay < grn[ista][iz].v.nlay; ilay++ )
		{
	  		fprintf( fp, "insert into %s ( nodeid, mtvmodid, ztop,  \n", mt_earthmodel_node_table_name );
			fprintf( fp, " thickness, vp, qp, vs, qs, rho, poissonratio ) VALUES \n" );
			fprintf( fp, "( %ld, %ld, %g, %g, %g, %g, %g, %g, %g, %g );\n",
				nodeid,
				mtvmodid + ista,
				grn[ista][iz].v.ztop[ilay],
				grn[ista][iz].v.thick[ilay],
				grn[ista][iz].v.vp[ilay],
				grn[ista][iz].v.qa[ilay],
				grn[ista][iz].v.vs[ilay],
                                grn[ista][iz].v.qb[ilay],
				grn[ista][iz].v.rho[ilay],
                                grn[ista][iz].v.sigma[ilay] );
			nodeid++;
		}
	}
	fprintf( fp, "\n" );
	fclose(fp);

/**** show rows ****/
/***
	fp = fopen( "insert.sql", "a+");
	fprintf( fp, "select * from %s;\n", moment_table_name );
	fprintf( fp, "select * from %s;\n", focal_plane_table_name );
	fprintf( fp, "select * from %s;\n", origin_table_name );
	fprintf( fp, "select * from %s;\n", mt_data_table_name );
	fprintf( fp, "select * from %s;\n", mt_waveform_segment_table_name );
	fprintf( fp, "select * from %s;\n", mt_filter_table_name );
	fprintf( fp, "select * from %s;\n", mt_earthmodel_table_name );
	fprintf( fp, "select * from %s;\n", mt_earthmodel_node_table_name );
	fprintf( fp, "\n" );
	fclose(fp);
***/

}

/******************************************************************************************************************/
/****                                                                                                          ****/
/******************************************************************************************************************/

void mysql_db_create( void )
{
	FILE *fp;
	char moment_table_name[256];
	char focal_plane_table_name[256];
	char origin_table_name[256];
	char mt_data_table_name[256];
	char mt_waveform_segment_table_name[256];
	char mt_filter_table_name[256];
	char mt_earthmodel_table_name[256];
	char mt_earthmodel_node_table_name[256];

        sprintf( moment_table_name, "MOMENT_STAGE" );
	sprintf( focal_plane_table_name, "FOCAL_PLANE_STAGE" );
	sprintf( origin_table_name, "MT_ORIGIN_STAGE" );
	sprintf( mt_data_table_name, "MT_DATA_STAGE" );
	sprintf( mt_waveform_segment_table_name, "MT_WAVEFORM_SEGMENT_STAGE" );
	sprintf( mt_filter_table_name, "MT_FILTER_STAGE" );
 	sprintf( mt_earthmodel_table_name, "MT_EARTHMODEL_STAGE" );
	sprintf( mt_earthmodel_node_table_name, "MT_EARTHMODEL_NODE_STAGE" );
	extern long evid;

/***/
	fp = fopen( "create.sql", "w");
	fprintf( fp, "\n\n" );
	
	fprintf( fp, "DROP TABLE %s;\n", moment_table_name );
        fprintf( fp, "DROP TABLE %s;\n", focal_plane_table_name );
        fprintf( fp, "DROP TABLE %s;\n", origin_table_name );

	fprintf( fp, "DROP TABLE %s;\n", mt_data_table_name );
        fprintf( fp, "DROP TABLE %s;\n", mt_waveform_segment_table_name );
        fprintf( fp, "DROP TABLE %s;\n", mt_filter_table_name );

	fprintf( fp, "DROP TABLE %s;\n", mt_earthmodel_table_name );
	fprintf( fp, "DROP TABLE %s;\n", mt_earthmodel_node_table_name );

	fprintf( fp, "\n\n" );

/*** create table for moment ***/
	fprintf( fp, "CREATE TABLE %s ( \n", moment_table_name );
	fprintf( fp, " mtid          INT(9) not null auto_increment, \n" );
        fprintf( fp, " fpid          INT(9)     default   -1, \n");
        fprintf( fp, " orid          INT(9)         not null, \n");
        fprintf( fp, " mxx           FLOAT(53)      not null, \n");
        fprintf( fp, " myy           FLOAT(53)      not null, \n");
        fprintf( fp, " mzz           FLOAT(53)      not null, \n");
        fprintf( fp, " mxy           FLOAT(53)      not null, \n");
        fprintf( fp, " mxz           FLOAT(53)      not null, \n");
        fprintf( fp, " myz           FLOAT(53)      not null, \n");
        fprintf( fp, " smxx          FLOAT(53)  default   -1, \n");
        fprintf( fp, " smyy          FLOAT(53)  default   -1, \n");
        fprintf( fp, " smzz          FLOAT(53)  default   -1, \n");
        fprintf( fp, " smxy          FLOAT(53)  default   -1, \n");
        fprintf( fp, " smxz          FLOAT(53)  default   -1, \n");
        fprintf( fp, " smyz          FLOAT(53)  default   -1, \n");
        fprintf( fp, " mt_type       VARCHAR(6) default  '-', \n");
        fprintf( fp, " mo            FLOAT(53)  default   -1, \n");
        fprintf( fp, " smo           FLOAT(53)  default   -1, \n");
        fprintf( fp, " momag         FLOAT(24)  default -999, \n");
        fprintf( fp, " piso          FLOAT(24)  default   -1, \n");
        fprintf( fp, " pclvd         FLOAT(24)  default   -1, \n");
        fprintf( fp, " pdc           FLOAT(24)  default   -1, \n");
        fprintf( fp, " kiso          FLOAT(24)  default -999, \n");
        fprintf( fp, " kclvd         FLOAT(24)  default -999, \n");
        fprintf( fp, " epsilon       FLOAT(24)  default -999, \n");
        fprintf( fp, " f_factor      FLOAT(24)  default -999, \n");
        fprintf( fp, " var_red       FLOAT(53)  default  999, \n");
        fprintf( fp, " residual      FLOAT(53)  default   -1, \n");
        fprintf( fp, " variance      FLOAT(53)  default   -1, \n");
        fprintf( fp, " nslpb         INT(5)     default   -1, \n");
        fprintf( fp, " nrlpb         INT(5)     default   -1, \n");
        fprintf( fp, " tmnlpb        FLOAT(24)  default   -1, \n");
        fprintf( fp, " nsmw          INT(5)     default   -1, \n");
        fprintf( fp, " nrmw          INT(5)     default   -1, \n");
        fprintf( fp, " tmnmw         FLOAT(24)  default   -1, \n");
        fprintf( fp, " nssurf        INT(5)      default   -1, \n");
        fprintf( fp, " nrsurf        INT(5)      default   -1, \n");
        fprintf( fp, " tmnsurf       FLOAT(24)   default   -1, \n");
        fprintf( fp, " half_duration FLOAT(24)   default -1, \n");
        fprintf( fp, " srcfunc       VARCHAR(12) default '-', \n");
        fprintf( fp, " algorithm     VARCHAR(15) default '-', \n");
        fprintf( fp, " epi_fixed     CHAR(1)     default '-', \n");
        fprintf( fp, " time_fixed    CHAR(1)     default '-', \n");
        fprintf( fp, " depth_fixed   CHAR(1)     default '-', \n");
        fprintf( fp, " auth          VARCHAR(20) default '-', \n");
        fprintf( fp, " commid        INT(9)      default -1, \n");
        fprintf( fp, " lddate        DATETIME    not null,   \n");
        fprintf( fp, " primary key ( mtid ) );\n" );
	fprintf( fp, "\n\n" );

/*** create table for focal_plane ***/

	fprintf( fp, "CREATE TABLE %s ( \n", focal_plane_table_name );
        fprintf( fp, " fpid          INT(9)      not null auto_increment, \n" );
        fprintf( fp, " orid          INT(9)      not null, \n" );
        fprintf( fp, " fp_type       VARCHAR(6)  default  '-', \n" );
        fprintf( fp, " decomp_type   VARCHAR(8)  default  '-', \n" );
        fprintf( fp, " strike1       FLOAT(24)   default -999, \n" );
        fprintf( fp, " dip1          FLOAT(24)   default -999, \n" );
        fprintf( fp, " rake1         FLOAT(24)   default -999, \n" );
        fprintf( fp, " strike2       FLOAT(24)   default -999, \n" );
        fprintf( fp, " dip2          FLOAT(24)   default -999, \n" );
        fprintf( fp, " rake2         FLOAT(24)   default -999, \n" );
        fprintf( fp, " sstrike       FLOAT(24)   default -999, \n" );
        fprintf( fp, " sdip          FLOAT(24)   default -999, \n" );
        fprintf( fp, " srake         FLOAT(24)   default -999, \n" );
        fprintf( fp, " primary_plane INTEGER(2)  default   -1, \n" );
        fprintf( fp, " eval1         FLOAT(53)   default    0, \n" );
        fprintf( fp, " az1           FLOAT(24)   default -999, \n" );
        fprintf( fp, " pl1           FLOAT(24)   default -999, \n" );
        fprintf( fp, " eval2         FLOAT(53)   default    0, \n" );
        fprintf( fp, " az2           FLOAT(24)   default -999, \n" );
        fprintf( fp, " pl2           FLOAT(24)   default -999, \n" );
        fprintf( fp, " eval3         FLOAT(53)   default    0, \n" );
        fprintf( fp, " az3           FLOAT(24)   default -999, \n" );
        fprintf( fp, " pl3           FLOAT(24)   default -999, \n" );
        fprintf( fp, " dcmo          FLOAT(53)   default   -1, \n" );
        fprintf( fp, " devmo         FLOAT(53)   default   -1, \n" );
        fprintf( fp, " isomo         FLOAT(53)   default   -1, \n" );
        fprintf( fp, " auth          VARCHAR(20) default  '-', \n" );
        fprintf( fp, " commid        INT(9)      default   -1, \n" );
        fprintf( fp, " lddate        DATETIME   not null, \n" );
        fprintf( fp, " primary key ( fpid ) ); \n" );
	fprintf( fp, "\n\n" );
                                                                                                                                                               
/*** create table for origin ***/

	fprintf( fp, "CREATE TABLE %s ( \n", origin_table_name );
        fprintf( fp, " lat       FLOAT(53)   default           -999, \n" );
        fprintf( fp, " lon       FLOAT(53)   default           -999, \n" );
        fprintf( fp, " depth     FLOAT(24)   default           -999, \n" );
        fprintf( fp, " time      FLOAT(53)   default 9999999999.999, \n" );
        fprintf( fp, " orid      INT(9)                    not null, \n" );
        fprintf( fp, " evid      INT(9)      default             -1, \n" );
        fprintf( fp, " jdate     INT(8)      default             -1, \n" );
        fprintf( fp, " nass      INT(4)      default             -1, \n" );
        fprintf( fp, " ndef      INT(4)      default             -1, \n" );
        fprintf( fp, " ndp       INT(4)      default             -1, \n" );
        fprintf( fp, " grn       INT(8)      default             -1, \n" );
        fprintf( fp, " srn       INT(8)      default             -1, \n" );
        fprintf( fp, " etype     VARCHAR(7)  default            '-', \n" );
        fprintf( fp, " depdp     FLOAT(24)   default           -999, \n" );
        fprintf( fp, " dtype     VARCHAR(1)  default            '-', \n" );
        fprintf( fp, " mb        FLOAT(24)   default           -999, \n" );
        fprintf( fp, " mbid      INT(9)      default             -1, \n" );
        fprintf( fp, " ms        FLOAT(24)   default           -999, \n" );
        fprintf( fp, " msid      INT(9)      default             -1, \n" );
        fprintf( fp, " ml        FLOAT(24)   default           -999, \n" );
        fprintf( fp, " mlid      INT(9)      default             -1, \n" );
        fprintf( fp, " algorithm VARCHAR(15) default            '-', \n" );
        fprintf( fp, " auth      VARCHAR(20) default            '-', \n" );
        fprintf( fp, " commid    INT(9)      default             -1, \n" );
        fprintf( fp, " lddate    DATETIME                  not null, \n" );
        fprintf( fp, " primary key ( orid ), \n" );
        fprintf( fp, " unique key ( lat, lon, depth, time, algorithm, auth ) ); \n" );
	fprintf( fp, "\n\n" );

/*** create table for mt_data ***/
	fprintf( fp, "CREATE TABLE %s ( \n", mt_data_table_name );
	fprintf( fp, " mtdataid INT(9)       not null auto_increment,  \n" );
	fprintf( fp, " mtid     INT(9)       default             -1, \n" );
	fprintf( fp, " sta      VARCHAR(8)   default            '-', \n" );
	fprintf( fp, " net      VARCHAR(8)   default            '-', \n" );
	fprintf( fp, " distkm   FLOAT(24)    default           -999, \n" );
	fprintf( fp, " azimuth  FLOAT(24)    default           -999, \n" );
	fprintf( fp, " used     VARCHAR(1)   default            '-', \n" );
	fprintf( fp, " quality  VARCHAR(32)  default            '-', \n" );
	fprintf( fp, " auth     VARCHAR(20)  default            '-', \n" );
	fprintf( fp, " commid   INT(9)       default             -1, \n" );
	fprintf( fp, " lddate   DATETIME     not null,\n" );
	fprintf( fp, "primary key ( mtdataid ) ); \n" );
	fprintf( fp, "\n\n" );
	
/*** create table for mt_wfseg  ***/
	fprintf( fp, "CREATE TABLE %s ( \n", mt_waveform_segment_table_name );
	fprintf( fp, " mtwfsegid      INT(9)      not null auto_increment,  \n" );
	fprintf( fp, " mtdataid	      INT(9)      default            -1, \n" );
	fprintf( fp, " wfsegtype      VARCHAR(16) default            -1, \n" );
	fprintf( fp, " sta            VARCHAR(8)  default           '-', \n" );
	fprintf( fp, " chan           VARCHAR(8)  default           '-', \n" );
	fprintf( fp, " chanid         INT(9)      default            -1, \n" );
	fprintf( fp, " start_time     FLOAT(53)   default          -999, \n" );
	fprintf( fp, " end_time       FLOAT(53)   default   -9999999999, \n" );
	fprintf( fp, " mtfilterid     INT(9)      default            -1, \n" );
	fprintf( fp, " mtvmodid       INT(9)      default            -1, \n" );
	fprintf( fp, " redvel         FLOAT(24)   default          -999, \n" );
	fprintf( fp, " tshift         FLOAT(24)   default          -999, \n" );
	fprintf( fp, " ampweight      FLOAT(24)   default            -1, \n" );
	fprintf( fp, " distnorm	      VARCHAR(1)  default           '-', \n" );
	fprintf( fp, " r0             FLOAT(24)   default            -1, \n" );
	fprintf( fp, " rayparam       FLOAT(24)   default            -1, \n" );
	fprintf( fp, " distdeg        FLOAT(24)   default          -999, \n" );
	fprintf( fp, " distkm         FLOAT(24)   default          -999, \n" );
	fprintf( fp, " az             FLOAT(24)   default          -999, \n" );
	fprintf( fp, " baz            FLOAT(24)   default          -999, \n" );
	fprintf( fp, " interpsamprate FLOAT(24)   default            -1, \n" );
	fprintf( fp, " used	      VARCHAR(1)  default           '-', \n" );
	fprintf( fp, " quality        VARCHAR(64) default           '-', \n" );
	fprintf( fp, " algor          VARCHAR(32) default           '-', \n" );
	fprintf( fp, " lddate         DATETIME    not null,\n" );
	fprintf( fp, "primary key ( mtwfsegid ) ); \n" );
        fprintf( fp, "\n\n" );

/*** create table for mt_filter ***/
	fprintf( fp, "CREATE TABLE %s ( \n", mt_filter_table_name );
	fprintf( fp, " mtfilterid   INT(9)       not null auto_increment,  \n" );
	fprintf( fp, " filtertype   VARCHAR(2)   not null, \n" );
	fprintf( fp, " filterdesign VARCHAR(16)  not null, \n" );
	fprintf( fp, " npole        INT(10)      default   -1, \n" );
	fprintf( fp, " lcorner      FLOAT(24)    default  999, \n" );
	fprintf( fp, " hcorner      FLOAT(24)    default  999, \n" );
	fprintf( fp, " npass        INT(2)       default   -1, \n" );
	fprintf( fp, " tranbw       FLOAT(24)    default   -1, \n" );
	fprintf( fp, " attenfactor  FLOAT(24)    default   -1, \n" );
	fprintf( fp, "primary key ( mtfilterid ) ); \n" );
	fprintf( fp, "\n\n" );

/*** create table for mt_earthmodel ***/
	fprintf( fp, "CREATE TABLE %s ( \n", mt_earthmodel_table_name );
	fprintf( fp, " mtvmodid  INT(9)       not null auto_increment,  \n" );
	fprintf( fp, " dimension VARCHAR(16)  not null, \n" );
	fprintf( fp, " nlon      INT(9)       not null, \n" );
	fprintf( fp, " nlat      INT(9)       not null, \n" );
	fprintf( fp, " ndep      INT(9)       not null, \n" );
	fprintf( fp, " modelname VARCHAR(64) default   '-', \n" );
	fprintf( fp, " auth      VARCHAR(32) default   '-', \n" );
	fprintf( fp, " lddate    DATETIME    not null,\n" );
	fprintf( fp, "primary key ( mtvmodid ) ); \n" );
	fprintf( fp, "\n\n" );

/*** create table for mt_earthmodel_node ****/
	fprintf( fp, "CREATE TABLE %s ( \n", mt_earthmodel_node_table_name );
	fprintf( fp, " nodeid      INT(9)      not null auto_increment,  \n" );
	fprintf( fp, " mtvmodid    INT(9)      default     -1, \n" );
	fprintf( fp, " lon         FLOAT(24)   default   -999, \n" );
	fprintf( fp, " lat         FLOAT(24)   default   -999, \n" );
	fprintf( fp, " interpstyle VARCHAR(16) default    '-', \n" );
	fprintf( fp, " z           FLOAT(24)   default   -999, \n" );
	fprintf( fp, " ztop        FLOAT(24)   default   -999, \n" );
	fprintf( fp, " thickness   FLOAT(24)   default   -999, \n" );
	fprintf( fp, " vp          FLOAT(24)   default   -999, \n" );
	fprintf( fp, " qp          FLOAT(24)   default   -999, \n" );
	fprintf( fp, " vs          FLOAT(24)   default   -999, \n" );
	fprintf( fp, " qs          FLOAT(24)   default   -999, \n" );
	fprintf( fp, " rho         FLOAT(24)   default   -999, \n" );
	fprintf( fp, " poissonratio FLOAT(24)  default   -999, \n" );
	fprintf( fp, "primary key ( nodeid ) ); \n" );
	fprintf( fp, "\n\n" );

/*** show the tables ***/
/***
	fprintf( fp, "desc %s;\n", moment_table_name );
	fprintf( fp, "desc %s;\n", focal_plane_table_name );
	fprintf( fp, "desc %s;\n", origin_table_name );
	fprintf( fp, "desc %s;\n", mt_data_table_name );
	fprintf( fp, "desc %s;\n", mt_waveform_segment_table_name );
	fprintf( fp, "desc %s;\n", mt_filter_table_name );
	fprintf( fp, "desc %s;\n", mt_earthmodel_table_name );
	fprintf( fp, "desc %s;\n", mt_earthmodel_node_table_name );
	fprintf( fp, "source insert.sql \n" );
	fprintf( fp, "\n\n" );
***/

	fclose(fp);
}

void oracle_db_create( void )
{
	FILE *fp;
        char moment_table_name[256];
        char focal_plane_table_name[256];
        char origin_table_name[256];
        char mt_data_table_name[256];
        char mt_waveform_segment_table_name[256];
        char mt_filter_table_name[256];
        char mt_earthmodel_table_name[256];
        char mt_earthmodel_node_table_name[256];
	extern long evid;

	sprintf( moment_table_name, "MOMENT_STAGE" );
        sprintf( focal_plane_table_name, "FOCAL_PLANE_STAGE" );
        sprintf( origin_table_name, "MT_ORIGIN_STAGE" );
        sprintf( mt_data_table_name, "MT_DATA_STAGE" );
        sprintf( mt_waveform_segment_table_name, "MT_WAVEFORM_SEGMENT_STAGE" );
        sprintf( mt_filter_table_name, "MT_FILTER_STAGE" );
        sprintf( mt_earthmodel_table_name, "MT_EARTHMODEL_STAGE" );
	sprintf( mt_earthmodel_node_table_name, "MT_EARTHMODEL_NODE_STAGE" );

	fp = fopen( "create.sql", "w" );

	fprintf( fp, "DROP TABLE %s;\n", moment_table_name );
        fprintf( fp, "DROP TABLE %s;\n", focal_plane_table_name );
        fprintf( fp, "DROP TABLE %s;\n", origin_table_name );
        fprintf( fp, "DROP TABLE %s;\n", mt_data_table_name );
        fprintf( fp, "DROP TABLE %s;\n", mt_waveform_segment_table_name );
        fprintf( fp, "DROP TABLE %s;\n", mt_filter_table_name );
        fprintf( fp, "DROP TABLE %s;\n", mt_earthmodel_table_name );
        fprintf( fp, "DROP TABLE %s;\n", mt_earthmodel_node_table_name );

	fprintf( fp, "\n\n" );

	fprintf( fp, "DROP SEQUENCE mtid_seq       start with 1000; \n" );
	fprintf( fp, "DROP SEQUENCE fpid_seq       start with 1000; \n" );
	fprintf( fp, "DROP SEQUENCE mtorid_seq     start with 1000; \n" );
	fprintf( fp, "DROP SEQUENCE mtdataid_seq   start with 1000; \n" );
	fprintf( fp, "DROP SEQUENCE mtwfsegid_seq  start with 1000; \n" );
	fprintf( fp, "DROP SEQUENCE mtfilterid_seq start with 1000; \n" );
	fprintf( fp, "DROP SEQUENCE mtvmodid_seq   start with 1000; \n" );
	fprintf( fp, "DROP SEQUENCE nodeid_seq     start with 1000; \n" );

	fprintf( fp, "\n\n" );

/*** create table for moment ***/

        fprintf( fp, "CREATE TABLE %s ( \n", moment_table_name );
        fprintf( fp, " mtid          NUMBER(9)    not null primary key, \n" );
        fprintf( fp, " fpid          NUMBER(9)    default   -1, \n");
        fprintf( fp, " orid          NUMBER(9)    not null, \n");
        fprintf( fp, " mxx           FLOAT(53)    not null, \n");
        fprintf( fp, " myy           FLOAT(53)    not null, \n");
        fprintf( fp, " mzz           FLOAT(53)    not null, \n");
        fprintf( fp, " mxy           FLOAT(53)    not null, \n");
        fprintf( fp, " mxz           FLOAT(53)    not null, \n");
        fprintf( fp, " myz           FLOAT(53)    not null, \n");
        fprintf( fp, " smxx          FLOAT(53)    default   -1, \n");
        fprintf( fp, " smyy          FLOAT(53)    default   -1, \n");
        fprintf( fp, " smzz          FLOAT(53)    default   -1, \n");
        fprintf( fp, " smxy          FLOAT(53)    default   -1, \n");
        fprintf( fp, " smxz          FLOAT(53)    default   -1, \n");
        fprintf( fp, " smyz          FLOAT(53)    default   -1, \n");
        fprintf( fp, " mt_type       VARCHAR2(6)  default  '-', \n");
        fprintf( fp, " mo            FLOAT(53)    default   -1, \n");
        fprintf( fp, " smo           FLOAT(53)    default   -1, \n");
        fprintf( fp, " momag         FLOAT(24)    default -999, \n");
        fprintf( fp, " piso          FLOAT(24)    default   -1, \n");
        fprintf( fp, " pclvd         FLOAT(24)    default   -1, \n");
        fprintf( fp, " pdc           FLOAT(24)    default   -1, \n");
        fprintf( fp, " kiso          FLOAT(24)    default -999, \n");
        fprintf( fp, " kclvd         FLOAT(24)    default -999, \n");
        fprintf( fp, " epsilon       FLOAT(24)    default -999, \n");
        fprintf( fp, " f_factor      FLOAT(24)    default -999, \n");
        fprintf( fp, " var_red       FLOAT(53)    default  999, \n");
        fprintf( fp, " residual      FLOAT(53)    default   -1, \n");
        fprintf( fp, " variance      FLOAT(53)    default   -1, \n");
        fprintf( fp, " nslpb         NUMBER(5)    default   -1, \n");
        fprintf( fp, " nrlpb         NUMBER(5)    default   -1, \n");
        fprintf( fp, " tmnlpb        FLOAT(24)    default   -1, \n");
        fprintf( fp, " nsmw          NUMBER(5)    default   -1, \n");
        fprintf( fp, " nrmw          NUMBER(5)    default   -1, \n");
        fprintf( fp, " tmnmw         FLOAT(24)    default   -1, \n");
        fprintf( fp, " nssurf        NUMBER(5)    default   -1, \n");
        fprintf( fp, " nrsurf        NUMBER(5)    default   -1, \n");
        fprintf( fp, " tmnsurf       FLOAT(24)    default   -1, \n");
        fprintf( fp, " half_duration FLOAT(24)    default -1, \n");
        fprintf( fp, " srcfunc       VARCHAR2(12) default '-', \n");
        fprintf( fp, " algorithm     VARCHAR2(15) default '-', \n");
        fprintf( fp, " epi_fixed     CHAR(1)      default '-', \n");
        fprintf( fp, " time_fixed    CHAR(1)      default '-', \n");
        fprintf( fp, " depth_fixed   CHAR(1)      default '-', \n");
        fprintf( fp, " auth          VARCHAR2(20) default '-', \n");
        fprintf( fp, " commid        NUMBER(9)    default -1, \n");
        fprintf( fp, " lddate        DATE         not null ); \n");
        fprintf( fp, "\n\n" );
 
/*** create table for focal_plane ***/
 
        fprintf( fp, "CREATE TABLE %s ( \n", focal_plane_table_name );
        fprintf( fp, " fpid          NUMBER(9)    not null primary key, \n" );
        fprintf( fp, " orid          NUMBER(9)    not null, \n" );
        fprintf( fp, " fp_type       VARCHAR2(6)  default  '-', \n" );
        fprintf( fp, " decomp_type   VARCHAR2(8)  default  '-', \n" );
        fprintf( fp, " strike1       FLOAT(24)    default -999, \n" );
        fprintf( fp, " dip1          FLOAT(24)    default -999, \n" );
        fprintf( fp, " rake1         FLOAT(24)    default -999, \n" );
        fprintf( fp, " strike2       FLOAT(24)    default -999, \n" );
        fprintf( fp, " dip2          FLOAT(24)    default -999, \n" );
        fprintf( fp, " rake2         FLOAT(24)    default -999, \n" );
        fprintf( fp, " sstrike       FLOAT(24)    default -999, \n" );
        fprintf( fp, " sdip          FLOAT(24)    default -999, \n" );
        fprintf( fp, " srake         FLOAT(24)    default -999, \n" );
        fprintf( fp, " primary_plane NUMBER(2)    default   -1, \n" );
        fprintf( fp, " eval1         FLOAT(53)    default    0, \n" );
        fprintf( fp, " az1           FLOAT(24)    default -999, \n" );
        fprintf( fp, " pl1           FLOAT(24)    default -999, \n" );
        fprintf( fp, " eval2         FLOAT(53)    default    0, \n" );
        fprintf( fp, " az2           FLOAT(24)    default -999, \n" );
        fprintf( fp, " pl2           FLOAT(24)    default -999, \n" );
        fprintf( fp, " eval3         FLOAT(53)    default    0, \n" );
        fprintf( fp, " az3           FLOAT(24)    default -999, \n" );
        fprintf( fp, " pl3           FLOAT(24)    default -999, \n" );
        fprintf( fp, " dcmo          FLOAT(53)    default   -1, \n" );
        fprintf( fp, " devmo         FLOAT(53)    default   -1, \n" );
        fprintf( fp, " isomo         FLOAT(53)    default   -1, \n" );
        fprintf( fp, " auth          VARCHAR2(20) default  '-', \n" );
        fprintf( fp, " commid        NUMBER(9)    default   -1, \n" );
        fprintf( fp, " lddate        DATE         not null );   \n" );
        fprintf( fp, "\n\n" );
                                                                                                                                               
/*** create table for origin ***/
 
        fprintf( fp, "CREATE TABLE %s ( \n", origin_table_name );
        fprintf( fp, " lat       FLOAT(53)    default           -999, \n" );
        fprintf( fp, " lon       FLOAT(53)    default           -999, \n" );
        fprintf( fp, " depth     FLOAT(24)    default           -999, \n" );
        fprintf( fp, " time      FLOAT(53)    default 9999999999.999, \n" );
        fprintf( fp, " orid      NUMBER(9)    not null   primary key, \n" );
        fprintf( fp, " evid      NUMBER(9)    default             -1, \n" );
        fprintf( fp, " jdate     NUMBER(8)    default             -1, \n" );
        fprintf( fp, " nass      NUMBER(4)    default             -1, \n" );
        fprintf( fp, " ndef      NUMBER(4)    default             -1, \n" );
        fprintf( fp, " ndp       NUMBER(4)    default             -1, \n" );
        fprintf( fp, " grn       NUMBER(8)    default             -1, \n" );
        fprintf( fp, " srn       NUMBER(8)    default             -1, \n" );
        fprintf( fp, " etype     VARCHAR2(7)  default            '-', \n" );
        fprintf( fp, " depdp     FLOAT(24)    default           -999, \n" );
        fprintf( fp, " dtype     VARCHAR2(1)  default            '-', \n" );
        fprintf( fp, " mb        FLOAT(24)    default           -999, \n" );
        fprintf( fp, " mbid      NUMBER(9)    default             -1, \n" );
        fprintf( fp, " ms        FLOAT(24)    default           -999, \n" );
        fprintf( fp, " msid      NUMBER(9)    default             -1, \n" );
        fprintf( fp, " ml        FLOAT(24)    default           -999, \n" );
        fprintf( fp, " mlid      NUMBER(9)    default             -1, \n" );
        fprintf( fp, " algorithm VARCHAR2(15) default            '-', \n" );
        fprintf( fp, " auth      VARCHAR2(20) default            '-', \n" );
        fprintf( fp, " commid    NUMBER(9)    default             -1, \n" );
        fprintf( fp, " lddate    DATE         not null,               \n" );
        fprintf( fp, " unique ( lat, lon, depth, time, algorithm, auth ) );\n" );
        fprintf( fp, "\n\n" );
 
/*** create table for mt_data ***/

        fprintf( fp, "CREATE TABLE %s ( \n", mt_data_table_name );
        fprintf( fp, " mtdataid NUMBER(9)    not null primary key, \n" );
        fprintf( fp, " mtid     NUMBER(9)    default           -1, \n" );
        fprintf( fp, " sta      VARCHAR2(8)  default          '-', \n" );
        fprintf( fp, " net      VARCHAR2(8)  default          '-', \n" );
        fprintf( fp, " distkm   FLOAT(24)    default         -999, \n" );
        fprintf( fp, " azimuth  FLOAT(24)    default         -999, \n" );
        fprintf( fp, " used     VARCHAR2(1)  default          '-', \n" );
        fprintf( fp, " quality  VARCHAR2(32) default          '-', \n" );
        fprintf( fp, " auth     VARCHAR2(20) default          '-', \n" );
        fprintf( fp, " commid   NUMBER(9)    default           -1, \n" );
        fprintf( fp, " lddate   DATE         not null           ); \n" );
        fprintf( fp, "\n\n" );
         
/*** create table for mt_wfseg  ***/

        fprintf( fp, "CREATE TABLE %s ( \n", mt_waveform_segment_table_name );
        fprintf( fp, " mtwfsegid      NUMBER(9)    not null  primary key, \n" );
        fprintf( fp, " mtdataid       NUMBER(9)    default            -1, \n" );
        fprintf( fp, " wfsegtype      VARCHAR2(16) default            -1, \n" );
        fprintf( fp, " sta            VARCHAR2(8)  default           '-', \n" );
        fprintf( fp, " chan           VARCHAR2(8)  default           '-', \n" );
        fprintf( fp, " chanid         NUMBER(9)    default            -1, \n" );
        fprintf( fp, " start_time     FLOAT(53)    default          -999, \n" );
        fprintf( fp, " end_time       FLOAT(53)    default   -9999999999, \n" );
        fprintf( fp, " mtfilterid     NUMBER(9)    default            -1, \n" );
        fprintf( fp, " mtvmodid       NUMBER(9)    default            -1, \n" );
        fprintf( fp, " redvel         FLOAT(24)    default          -999, \n" );
        fprintf( fp, " tshift         FLOAT(24)    default          -999, \n" );
        fprintf( fp, " ampweight      FLOAT(24)    default            -1, \n" );
        fprintf( fp, " distnorm       VARCHAR2(1)  default           '-', \n" );
        fprintf( fp, " r0             FLOAT(24)    default            -1, \n" );
        fprintf( fp, " rayparam       FLOAT(24)    default            -1, \n" );
        fprintf( fp, " distdeg        FLOAT(24)    default          -999, \n" );
        fprintf( fp, " distkm         FLOAT(24)    default          -999, \n" );
        fprintf( fp, " az             FLOAT(24)    default          -999, \n" );
        fprintf( fp, " baz            FLOAT(24)    default          -999, \n" );
        fprintf( fp, " interpsamprate FLOAT(24)    default            -1, \n" );
        fprintf( fp, " used           VARCHAR2(1)  default           '-', \n" );
        fprintf( fp, " quality        VARCHAR2(64) default           '-', \n" );
        fprintf( fp, " algor          VARCHAR2(32) default           '-', \n" );
        fprintf( fp, " lddate         DATE         not null );            \n" );
        fprintf( fp, "\n\n" );
 
/*** create table for mt_filter ***/

        fprintf( fp, "CREATE TABLE %s ( \n", mt_filter_table_name );
        fprintf( fp, " mtfilterid   NUMBER(9)    not null primary key, \n" );
        fprintf( fp, " filtertype   VARCHAR2(2)  not null, \n" );
        fprintf( fp, " filterdesign VARCHAR2(16) not null, \n" );
        fprintf( fp, " npole        NUMBER(10)   default   -1, \n" );
        fprintf( fp, " lcorner      FLOAT(24)    default  999, \n" );
        fprintf( fp, " hcorner      FLOAT(24)    default  999, \n" );
        fprintf( fp, " npass        NUMBER(2)    default   -1, \n" );
        fprintf( fp, " tranbw       FLOAT(24)    default   -1, \n" );
        fprintf( fp, " attenfactor  FLOAT(24)    default   -1 );\n" );
        fprintf( fp, "\n\n" );
 
/*** create table for mt_earthmodel ***/

        fprintf( fp, "CREATE TABLE %s ( \n", mt_earthmodel_table_name );
        fprintf( fp, " mtvmodid  NUMBER(9)    not null primary key, \n" );
        fprintf( fp, " dimension VARCHAR2(16) not null, \n" );
        fprintf( fp, " nlon      NUMBER(9)    not null, \n" );
        fprintf( fp, " nlat      NUMBER(9)    not null, \n" );
        fprintf( fp, " ndep      NUMBER(9)    not null, \n" );
        fprintf( fp, " modelname VARCHAR2(64) default   '-', \n" );
        fprintf( fp, " auth      VARCHAR2(32) default   '-', \n" );
        fprintf( fp, " lddate    DATE         not null  );   \n" );
        fprintf( fp, "\n\n" );
 
/*** create table for mt_earthmodel_node ****/

        fprintf( fp, "CREATE TABLE %s ( \n", mt_earthmodel_node_table_name );
        fprintf( fp, " nodeid      NUMBER(9)    not null primary key, \n" );
        fprintf( fp, " mtvmodid    NUMBER(9)    default     -1, \n" );
        fprintf( fp, " lon         FLOAT(24)    default   -999, \n" );
        fprintf( fp, " lat         FLOAT(24)    default   -999, \n" );
        fprintf( fp, " interpstyle VARCHAR2(16) default    '-', \n" );
        fprintf( fp, " z           FLOAT(24)    default   -999, \n" );
        fprintf( fp, " ztop        FLOAT(24)    default   -999, \n" );
        fprintf( fp, " thickness   FLOAT(24)    default   -999, \n" );
        fprintf( fp, " vp          FLOAT(24)    default   -999, \n" );
        fprintf( fp, " qp          FLOAT(24)    default   -999, \n" );
        fprintf( fp, " vs          FLOAT(24)    default   -999, \n" );
        fprintf( fp, " qs          FLOAT(24)    default   -999, \n" );
        fprintf( fp, " rho         FLOAT(24)    default   -999, \n" );
        fprintf( fp, " poissonratio FLOAT(24)   default   -999);\n" );
        fprintf( fp, "\n\n" );
 
/*** show the tables ***/
/***
        fprintf( fp, "desc %s;\n", moment_table_name );
        fprintf( fp, "desc %s;\n", focal_plane_table_name );
        fprintf( fp, "desc %s;\n", origin_table_name );
        fprintf( fp, "desc %s;\n", mt_data_table_name );
        fprintf( fp, "desc %s;\n", mt_waveform_segment_table_name );
        fprintf( fp, "desc %s;\n", mt_filter_table_name );
        fprintf( fp, "desc %s;\n", mt_earthmodel_table_name );
        fprintf( fp, "desc %s;\n", mt_earthmodel_node_table_name );
        fprintf( fp, "source insert.sql \n" );
        fprintf( fp, "\n\n" );
***/

        fclose(fp);
}
