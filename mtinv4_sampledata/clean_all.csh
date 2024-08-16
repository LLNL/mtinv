#!/bin/csh
 
###
### clean all the test runs for each directory
###
foreach i ( 2009-12-22T055435_Kyrgyzstan 2021-08-13T115717_Tennessee 2021-11-18T025303_Missouri mtinv_sw4 test_grn2Mxy compare_gfs )
cd $i
csh ./clean.csh
cd ..
end
du -sh *

###  Also, clean the sqlite mt.db database entries, 
###  otherwise no prob just get unique key constraints errors
###  and new solutions wont load. Note, i don't know 
###  what the mtid keys are ahead of time so I clear all entries.
###
list_MTdb.csh
# remove_MTdb.csh all
# list_MTdb.csh
