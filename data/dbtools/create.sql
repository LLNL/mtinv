DROP TABLE MOMENT_STAGE;
DROP TABLE FOCAL_PLANE_STAGE;
DROP TABLE MT_ORIGIN_STAGE;
DROP TABLE MT_DATA_STAGE;
DROP TABLE MT_WAVEFORM_SEGMENT_STAGE;
DROP TABLE MT_PHASE_DATA_STAGE;
DROP TABLE MT_FILTER_STAGE;
DROP TABLE MT_EARTHMODEL_STAGE;
DROP TABLE MT_EARTHMODEL_NODE_STAGE;


DROP TABLE mtid_seq;
DROP TABLE fpid_seq;
DROP TABLE mtorid_seq;
DROP TABLE mtdataid_seq;
DROP TABLE mtwfsegid_seq;
DROP TABLE phaid_seq;
DROP TABLE mtfilterid_seq;
DROP TABLE mtvmodid_seq;
DROP TABLE nodeid_seq;

CREATE TABLE       mtid_seq (      mtid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);
CREATE TABLE       fpid_seq (      fpid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);
CREATE TABLE     mtorid_seq (    mtorid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);
CREATE TABLE   mtdataid_seq (  mtdataid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);
CREATE TABLE  mtwfsegid_seq ( mtwfsegid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);
CREATE TABLE      phaid_seq (     phaid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);
CREATE TABLE mtfilterid_seq (mtfilterid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);
CREATE TABLE   mtvmodid_seq (  mtvmodid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);
CREATE TABLE     nodeid_seq (    nodeid INTEGER PRIMARY KEY AUTOINCREMENT, lddate DATETIME default CURRENT_TIMESTAMP);


INSERT into       mtid_seq (       mtid, lddate) VALUES (  1000, CURRENT_TIMESTAMP );
INSERT into       fpid_seq (       fpid, lddate) VALUES (   100, CURRENT_TIMESTAMP );
INSERT into     mtorid_seq (     mtorid, lddate) VALUES ( 10000, CURRENT_TIMESTAMP );
INSERT into   mtdataid_seq (   mtdataid, lddate) VALUES (   100, CURRENT_TIMESTAMP );
INSERT into  mtwfsegid_seq (  mtwfsegid, lddate) VALUES (    10, CURRENT_TIMESTAMP );
INSERT into      phaid_seq (      phaid, lddate) VALUES (     1, CURRENT_TIMESTAMP );
INSERT into mtfilterid_seq ( mtfilterid, lddate) VALUES (     1, CURRENT_TIMESTAMP );
INSERT into   mtvmodid_seq (   mtvmodid, lddate) VALUES (     1, CURRENT_TIMESTAMP );
INSERT into     nodeid_seq (     nodeid, lddate) VALUES (     1, CURRENT_TIMESTAMP );

CREATE TABLE MOMENT_STAGE ( 
 mtid          NUMBER(9)    not null,
 fpid          NUMBER(9)    default  -1,
 orid          NUMBER(9)    not null, 
 mxx           FLOAT(53)    not null, 
 myy           FLOAT(53)    not null, 
 mzz           FLOAT(53)    not null, 
 mxy           FLOAT(53)    not null, 
 mxz           FLOAT(53)    not null, 
 myz           FLOAT(53)    not null, 
 smxx          FLOAT(53)    default   -1, 
 smyy          FLOAT(53)    default   -1, 
 smzz          FLOAT(53)    default   -1, 
 smxy          FLOAT(53)    default   -1, 
 smxz          FLOAT(53)    default   -1, 
 smyz          FLOAT(53)    default   -1, 
 mt_type       VARCHAR2(6)  default  '-', 
 mo            FLOAT(53)    default   -1, 
 smo           FLOAT(53)    default   -1, 
 momag         FLOAT(24)    default -999, 
 piso          FLOAT(24)    default   -1, 
 pclvd         FLOAT(24)    default   -1, 
 pdc           FLOAT(24)    default   -1, 
 kiso          FLOAT(24)    default -999, 
 kclvd         FLOAT(24)    default -999, 
 epsilon       FLOAT(24)    default -999, 
 f_factor      FLOAT(24)    default -999, 
 var_red       FLOAT(53)    default  999, 
 residual      FLOAT(53)    default   -1, 
 variance      FLOAT(53)    default   -1, 
 nslpb         NUMBER(5)    default   -1, 
 nrlpb         NUMBER(5)    default   -1, 
 tmnlpb        FLOAT(24)    default   -1, 
 nsmw          NUMBER(5)    default   -1, 
 nrmw          NUMBER(5)    default   -1, 
 tmnmw         FLOAT(24)    default   -1, 
 nssurf        NUMBER(5)    default   -1, 
 nrsurf        NUMBER(5)    default   -1, 
 tmnsurf       FLOAT(24)    default   -1, 
 half_duration FLOAT(24)    default -1, 
 srcfunc       VARCHAR2(12) default '-', 
 algorithm     VARCHAR2(15) default '-', 
 epi_fixed     CHAR(1)      default '-', 
 time_fixed    CHAR(1)      default '-', 
 depth_fixed   CHAR(1)      default '-', 
 auth          VARCHAR2(20) default '-', 
 commid        NUMBER(9)    default -1, 
 lddate        DATETIME     default CURRENT_TIMESTAMP ); 


CREATE TABLE FOCAL_PLANE_STAGE ( 
 fpid          NUMBER(9)    not null, 
 orid          NUMBER(9)    not null, 
 fp_type       VARCHAR2(6)  default  '-', 
 decomp_type   VARCHAR2(8)  default  '-', 
 strike1       FLOAT(24)    default -999, 
 dip1          FLOAT(24)    default -999, 
 rake1         FLOAT(24)    default -999, 
 strike2       FLOAT(24)    default -999, 
 dip2          FLOAT(24)    default -999, 
 rake2         FLOAT(24)    default -999, 
 sstrike       FLOAT(24)    default -999, 
 sdip          FLOAT(24)    default -999, 
 srake         FLOAT(24)    default -999, 
 primary_plane NUMBER(2)    default   -1, 
 eval1         FLOAT(53)    default    0, 
 az1           FLOAT(24)    default -999, 
 pl1           FLOAT(24)    default -999, 
 eval2         FLOAT(53)    default    0, 
 az2           FLOAT(24)    default -999, 
 pl2           FLOAT(24)    default -999, 
 eval3         FLOAT(53)    default    0, 
 az3           FLOAT(24)    default -999, 
 pl3           FLOAT(24)    default -999, 
 dcmo          FLOAT(53)    default   -1, 
 devmo         FLOAT(53)    default   -1, 
 isomo         FLOAT(53)    default   -1, 
 auth          VARCHAR2(20) default  '-', 
 commid        NUMBER(9)    default   -1, 
 lddate        DATETIME     default CURRENT_TIMESTAMP );   


CREATE TABLE MT_ORIGIN_STAGE ( 
 lat       FLOAT(53)    default           -999, 
 lon       FLOAT(53)    default           -999, 
 depth     FLOAT(24)    default           -999, 
 time      FLOAT(53)    default 9999999999.999, 
 orid      NUMBER(9)    not null              , 
 evid      NUMBER(9)    default             -1, 
 jdate     NUMBER(8)    default             -1, 
 nass      NUMBER(4)    default             -1, 
 ndef      NUMBER(4)    default             -1, 
 ndp       NUMBER(4)    default             -1, 
 grn       NUMBER(8)    default             -1, 
 srn       NUMBER(8)    default             -1, 
 etype     VARCHAR2(7)  default            '-', 
 depdp     FLOAT(24)    default           -999, 
 dtype     VARCHAR2(1)  default            '-', 
 mb        FLOAT(24)    default           -999, 
 mbid      NUMBER(9)    default             -1, 
 ms        FLOAT(24)    default           -999, 
 msid      NUMBER(9)    default             -1, 
 ml        FLOAT(24)    default           -999, 
 mlid      NUMBER(9)    default             -1, 
 algorithm VARCHAR2(15) default            '-', 
 auth      VARCHAR2(20) default            '-', 
 commid    NUMBER(9)    default             -1, 
 lddate    DATETIME     default CURRENT_TIMESTAMP, 
 unique ( lat, lon, depth, time, algorithm, auth ) ); 


CREATE TABLE MT_DATA_STAGE ( 
 mtdataid NUMBER(9)    not null,
 mtid     NUMBER(9)    not null,
 sta      VARCHAR2(8)  default          '-', 
 net      VARCHAR2(8)  default          '-', 
 distkm   FLOAT(24)    default         -999, 
 azimuth  FLOAT(24)    default         -999, 
 used     VARCHAR2(1)  default          '-', 
 quality  VARCHAR2(32) default          '-', 
 auth     VARCHAR2(20) default          '-', 
 commid   NUMBER(9)    default           -1, 
 lddate   DATETIME     default CURRENT_TIMESTAMP ); 


CREATE TABLE MT_WAVEFORM_SEGMENT_STAGE ( 
 mtwfsegid      NUMBER(9)    not null             , 
 mtdataid       NUMBER(9)    not null             , 
 wfsegtype      VARCHAR2(16) default            -1, 
 sta            VARCHAR2(8)  default           '-', 
 chan           VARCHAR2(8)  default           '-', 
 chanid         NUMBER(9)    default            -1, 
 start_time     FLOAT(53)    default          -999, 
 end_time       FLOAT(53)    default   -9999999999, 
 mtfilterid     NUMBER(9)    default            -1, 
 mtvmodid       NUMBER(9)    default            -1, 
 redvel         FLOAT(24)    default          -999, 
 tshift         FLOAT(24)    default          -999, 
 ampweight      FLOAT(24)    default            -1, 
 distnorm       VARCHAR2(1)  default           '-', 
 r0             FLOAT(24)    default            -1, 
 rayparam       FLOAT(24)    default            -1, 
 distdeg        FLOAT(24)    default          -999, 
 distkm         FLOAT(24)    default          -999, 
 az             FLOAT(24)    default          -999, 
 baz            FLOAT(24)    default          -999, 
 interpsamprate FLOAT(24)    default            -1, 
 used           VARCHAR2(1)  default           '-', 
 quality        VARCHAR2(64) default           '-', 
 snr            FLOAT(24)    default            -1, 
 xcor           FLOAT(24)    default          -999, 
 lagtime        FLOAT(24)    default          -999, 
 algor          VARCHAR2(32) default           '-', 
 lddate         DATETIME     default CURRENT_TIMESTAMP); 


CREATE TABLE MT_PHASE_DATA_STAGE ( 
 phaid          NUMBER(9)    not null,              
 mtwfsegid      NUMBER(9)    not null,              
 phasetype      VARCHAR2(8)  default           '-', 
 sta            VARCHAR2(8)  default           '-', 
 chan           VARCHAR2(8)  default           '-', 
 gvlo           FLOAT(24)    default            -1, 
 gvhi           FLOAT(24)    default            -1, 
 amp            FLOAT(24)    default          -999, 
 time           FLOAT(53)    default   -9999999999, 
 dur            FLOAT(24)    default            -1, 
 freq           FLOAT(24)    default            -1, 
 per            FLOAT(24)    default            -1, 
 lddate         DATETIME     default CURRENT_TIMESTAMP); 


CREATE TABLE MT_FILTER_STAGE ( 
 mtfilterid   NUMBER(9)    not null, 
 filtertype   VARCHAR2(2)  not null, 
 filterdesign VARCHAR2(16) not null, 
 npole        NUMBER(10)   default   -1, 
 lcorner      FLOAT(24)    default  999, 
 hcorner      FLOAT(24)    default  999, 
 npass        NUMBER(2)    default   -1, 
 tranbw       FLOAT(24)    default   -1, 
 attenfactor  FLOAT(24)    default   -1 );


CREATE TABLE MT_EARTHMODEL_STAGE ( 
 mtvmodid  NUMBER(9)    not null, 
 dimension VARCHAR2(16) not null, 
 nlon      NUMBER(9)    not null, 
 nlat      NUMBER(9)    not null, 
 ndep      NUMBER(9)    not null, 
 modelname VARCHAR2(64) default   '-', 
 auth      VARCHAR2(32) default   '-', 
 lddate    DATETIME     default CURRENT_TIMESTAMP);


CREATE TABLE MT_EARTHMODEL_NODE_STAGE ( 
 nodeid      NUMBER(9)    not null primary key, 
 mtvmodid    NUMBER(9)    default     -1, 
 lon         FLOAT(24)    default   -999, 
 lat         FLOAT(24)    default   -999, 
 interpstyle VARCHAR2(16) default    '-', 
 z           FLOAT(24)    default   -999, 
 ztop        FLOAT(24)    default   -999, 
 thickness   FLOAT(24)    default   -999, 
 vp          FLOAT(24)    default   -999, 
 qp          FLOAT(24)    default   -999, 
 vs          FLOAT(24)    default   -999, 
 qs          FLOAT(24)    default   -999, 
 rho         FLOAT(24)    default   -999, 
 poissonratio FLOAT(24)   default   -999);


