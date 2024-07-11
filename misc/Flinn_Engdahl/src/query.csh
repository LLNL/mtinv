#!/bin/csh
sqlite3 /Users/ichinose1/Work/mtinv.v3.0.5/data/mt.db << EOF
.output dump.out
.mode column
.header off
select lon, lat, orid from MT_ORIGIN_STAGE;
.quit
EOF
