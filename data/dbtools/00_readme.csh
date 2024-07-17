#!/bin/csh -x

### ------------------------------------------------------- ###
### WARNING, this script will drop all tables and seq_id's! ###
### ------------------------------------------------------- ###

### /Users/ichinose1/Work/mtinv/data/dbtools
### setenv MTINV_DATABASE_FILE /Users/ichinose1/Work/mtinv.v4.0.1/data/mt.db

###
### to clean out the database completely or start new
###
/bin/rm -f ../mt.db

###
### drop tables, and create new schema and sequence ids
### if new then soft errors from dropping nonexisting tables
###
sqlite3 ../mt.db < create.sql

###
### reload the georegion table
###
sqlite3 ../mt.db < create_georegion.sql

###
### now work a MT, get a solution and use the run2.csh to 
### insert a new MT solution, here is just an example
###
sqlite3 ../mt.db < sample.sql

###
### run mtinv/bin/updateMTdb to link the georegion, otherwise the next query wont work
###
updateMTdb

###
### use the provided script mtinv/bin/list_MTdb.csh to list the solution
###
list_MTdb.csh
