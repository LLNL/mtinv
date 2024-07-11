#!/bin/csh
sqlite3 /Users/ichinose1/Work/mtinv.v3.0.5/data/mt.db << EOF
.read update.sql
.quit
EOF
