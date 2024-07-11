#!/bin/csh

if( $#argv == 1 ) then
	echo "User Environment: MT_DATABASE_FILE = ${MT_DATABASE_FILE}"

        set DATABASE=${MT_DATABASE_FILE}
	if( $argv[1] == "all" || $argv[1] == "ALL" ) then
          set MODE=0
          set MTID=-1
	else
          set MODE=1
          set MTID=$argv[1]
	endif
	echo "$0 mode=$MODE Removing MTID = ${MTID} from DATABASE=${DATABASE} "

else if ( $#argv == 2 ) then

	if( $argv[1] == "all" || $argv[1] == "ALL" ) then
          set MODE=0
          set MTID=-1
          set DATABASE=$argv[2]
	else
	  set MODE=1
          set MTID=$argv[1]
 	  set DATABASE=$argv[2]
	endif
	echo "$0 mode=$MODE Removing MTID = ${MTID} from DATABASE=${DATABASE} "

else
        echo "$0 requires 1 or 2 argument"
        echo " Usage : $0 MTID "
	echo " Usage : $0 MTID DATABASE "

# echo " (default database = ${MT_DATABASE_FILE})"

	echo " MTID=all truncates all tables! "
        exit
endif

if( $MODE == 0 ) then
echo " ================= TRUNCATING ALL TABLES =========================== "
echo " ================= FROM ${DATABASE} "
echo " ================= TRUNCATING ALL TABLES =========================== "
read -p "Are you sure? Hit any key to continue. "

sqlite3 ${DATABASE} << EOF
delete from MT_EARTHMODEL_STAGE;
delete from MT_FILTER_STAGE;
delete from MT_WAVEFORM_SEGMENT_STAGE;
delete from MT_DATA_STAGE;
delete from FOCAL_PLANE_STAGE;
delete from MT_ORIGIN_STAGE;
delete from MOMENT_STAGE;
VACUUM;
.quit
EOF

else

sqlite3 ${DATABASE} << EOF
delete from MT_EARTHMODEL_STAGE where mtvmodid in 
  ( select mtvmodid from MT_WAVEFORM_SEGMENT_STAGE where mtdataid in ( select mtdataid from MT_DATA_STAGE where mtid = ${MTID} ) );
delete from MT_FILTER_STAGE where mtfilterid in 
  ( select mtfilterid from MT_WAVEFORM_SEGMENT_STAGE where mtdataid in ( select mtdataid from MT_DATA_STAGE where mtid = ${MTID} ) );
delete from MT_WAVEFORM_SEGMENT_STAGE where mtdataid in ( select mtdataid from MT_DATA_STAGE where mtid = ${MTID} );
delete from MT_DATA_STAGE where mtid = ${MTID};
delete from FOCAL_PLANE_STAGE where orid = (select orid from MOMENT_STAGE where mtid = ${MTID} );
delete from MT_ORIGIN_STAGE where orid = ( select orid from MOMENT_STAGE where mtid = ${MTID} );
delete from MOMENT_STAGE where mtid = ${MTID};
.quit
EOF

endif

##
## G. Ichinose 2019March11
## How do we orphan MOMENT_STAGE rows with no corresponding MT_ORIGIN_STAGE rows by orid link?
##
