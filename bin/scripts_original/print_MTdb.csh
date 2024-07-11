#!/bin/csh

if( $#argv == 1 ) then
	set MTID=$argv[1]
	echo "User Environment:"
	echo "MTINV_PATH          = ${MTINV_PATH}"
	echo "MT_DATABASE_FILE    = ${MT_DATABASE_FILE}"

	set DATABASE=${MT_DATABASE_FILE}
	echo " Default database = ${DATABASE} "
	set opt=1

else if ( $#argv == 2 ) then
	set MTID=$argv[1]
	set DATABASE=$argv[2]
	echo " Default database = ${DATABASE} "
	set opt=1

else if( $#argv == 0 ) then
	echo "User Environment:"
	echo "MTINV_PATH          = ${MTINV_PATH}"
	echo "MT_DATABASE_FILE    = ${MT_DATABASE_FILE}"

	set DATABASE=${MT_DATABASE_FILE} 
	echo " Default Database = ${DATABASE} "
	echo " Return most recent DB uploaded "
        set opt=2
	set MTID=-1
else
	echo "$0 : needs 0, 1, or 2 arguments, got $#argv "
	echo " Usage:  $0 - queries the most recent mtid, orid from default db "
	echo " Usage:  $0 MTID - queries for MTID from default db "
	echo " Usage:  $0 MTID DATABASE - return query for MTID from DATABASE "
	echo " Default Database = ${DATABASE} "
	exit(-1)
endif

if( $opt == 1 ) then
set QUERY1=" ( select orid from MOMENT_STAGE where mtid = ${MTID} ) "
set QUERY2=" ( select mtid from MOMENT_STAGE where mtid = ${MTID} ) "
endif

if( $opt == 2 ) then
set QUERY1=" ( select max(orid) from MOMENT_STAGE ) "
set QUERY2=" ( select max(mtid) from MOMENT_STAGE ) "
endif

sqlite3 ${DATABASE} << EOF
.headers on
.mode column
.stats off
.timer off

.print "\nMT_ORIGIN_STAGE\n"

select evid, grname from MT_ORIGIN_STAGE o, GEOREGION g where o.grn = g.grn and orid = ${QUERY1} ;

.print "\nMT_ORIGIN_STAGE\n"

select 
	orid,
	datetime( time, 'unixepoch' ) as OriginTime,
	lat,
	lon,
	depth,
	evid,
	jdate,
	algorithm,
	auth,
	lddate
from MT_ORIGIN_STAGE where orid = ${QUERY1} ;

/* from MT_ORIGIN_STAGE where orid = ( select orid from MOMENT_STAGE where mtid = ${MTID} ); */

.print "\nMOMENT_STAGE\n"

select 
	mtid, 
	fpid,
	orid,
	mt_type,
	momag,
	piso,
	pclvd,
	pdc,
	var_red,
	residual,
	algorithm,
	auth
/* from MOMENT_STAGE where mtid = ${MTID}; */
from MOMENT_STAGE where mtid = ${QUERY2} ;

.print "\nMOMENT_STAGE\n"

select 
	mxx,
	myy,
	mzz,
	mxy,
	mxz,
	myz,
	mo,
	kiso,
	kclvd,
	epsilon,
	f_factor
/* from MOMENT_STAGE where mtid = ${MTID}; */
from MOMENT_STAGE where mtid = ${QUERY2} ;


.print "\nFOCAL_PLANE_STAGE\n" 

select 
	fpid, 
	orid,
	fp_type,
	decomp_type,
	strike1, dip1, rake1,
	strike2, dip2, rake2,
	auth
/* from FOCAL_PLANE_STAGE where orid = (select orid from MOMENT_STAGE where mtid = ${MTID} ); */
from FOCAL_PLANE_STAGE where orid = ${QUERY1} ;

.print "\nFOCAL_PLANE_STAGE\n"

select 
	eval1, az1, pl1, eval2, az2, pl2, eval3, az3, pl3
/* from FOCAL_PLANE_STAGE where orid = (select orid from MOMENT_STAGE where mtid = ${MTID} ); */
from FOCAL_PLANE_STAGE where orid = ${QUERY1} ;

.print "\nMT_DATA_STAGE\n"

select 
	/* mtdataid, */
	net, 
	sta, 
	distkm, 
	azimuth, 
	used, 
	auth, 
	lddate
from 
	/* MT_DATA_STAGE where mtid = ${MTID}; */
	MT_DATA_STAGE where mtid = ${QUERY2} ;



.print "\n MT_WAVEFORM_SEGMENT_STAGE, MT_FILTER_STAGE, MT_EARTHMODEL_STAGE\n"

select sta,
        chan,
	lcorner,
        hcorner,
        modelname,
	used,
        round(xcor,2) as xcor
from
        MT_WAVEFORM_SEGMENT_STAGE,
        MT_FILTER_STAGE,
        MT_EARTHMODEL_STAGE
where
        mtdataid in ( select mtdataid from MT_DATA_STAGE where mtid = ${QUERY2} ) and
        MT_FILTER_STAGE.mtfilterid = MT_WAVEFORM_SEGMENT_STAGE.mtfilterid and
        MT_EARTHMODEL_STAGE.mtvmodid = MT_WAVEFORM_SEGMENT_STAGE.mtvmodid;


.width 6 4 20 8 7 7 9 6 6 14 4 -5 -7 -10
select
        /* mtwfsegid, */
        sta,
        chan,
	datetime( start_time, 'unixepoch' ) as ST,
	round( (end_time - start_time), 2 ) as LENGTH,
        lcorner,
	hcorner,
	modelname,
        redvel,
        tshift,
        interpsamprate,
        used,
	round(xcor,2) as xcor,
	round(lagtime,2) as lagtime,
	round(snr,2) as SNR
from
	MT_WAVEFORM_SEGMENT_STAGE, 
	MT_FILTER_STAGE,
	MT_EARTHMODEL_STAGE
where 
	mtdataid in ( select mtdataid from MT_DATA_STAGE where mtid = ${QUERY2} ) and
	MT_FILTER_STAGE.mtfilterid = MT_WAVEFORM_SEGMENT_STAGE.mtfilterid and
	MT_EARTHMODEL_STAGE.mtvmodid = MT_WAVEFORM_SEGMENT_STAGE.mtvmodid;

.print "\n MT_PHASE_DATA_STAGE\n"

.width 6 6 4 -6 -5 -13 -8 -8 6 4
select 
	/* phaid, */
	phasetype,
	sta,
	chan,
	gvhi,
	gvlo,
	amp,
	round(time) as time, 
	round(dur) as dur,
	freq,
	per
from 
	MT_PHASE_DATA_STAGE
where
	mtwfsegid in ( 
		select mtwfsegid from MT_WAVEFORM_SEGMENT_STAGE where mtdataid in ( 
		select mtdataid from MT_DATA_STAGE where mtid = ${QUERY2} )
	);

.print "\n"

.quit
EOF
