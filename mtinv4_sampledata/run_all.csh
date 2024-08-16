#!/bin/csh

###
### run all the examples, compare_gfs - requires hspec96, hpulse96, f96tosac, RBH-CPS3.30
###
### read the scripts 00_run_and_readme_example.csh for details
###

foreach i ( 2009-12-22T055435_Kyrgyzstan 2021-08-13T115717_Tennessee 2021-11-18T025303_Missouri mtinv_sw4 test_grn2Mxy compare_gfs )
cd $i
csh ./00_run_and_readme_example.csh
cd ..
end
