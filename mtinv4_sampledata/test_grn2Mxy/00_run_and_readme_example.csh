#!/bin/csh
##############################################################################
# Example Dataset for testing mtinv version 4
#
# M 4.0 - 7 km S of Williamsville, Missouri
# 2021-11-18 02:53:03 (UTC) 36.908°N 90.543°W 16.5 km depth
# https://earthquake.usgs.gov/earthquakes/eventpage/nm60363582/executive
#
# ./mtinv4_sampledata/2021-11-18T025303_Missouri
#
# This test the grn2Mxy and special load features of mtinv using the earthquake data
#
##############################################################################

##############################################################################
### clean the Data, Resp, dev, and full directories, and redo the unpacking
###  of the EarthScope/IRIS data
##############################################################################
csh ./clean.csh

unpack.csh ../2021-11-18T025303_Missouri/2021-11-18-mwr40-new-madrid-missouri-region.tar 

##############################################################################
# deviatoric-MT solution
##############################################################################
cd dev

### edit setupMT.csh script and uncomment approriate line or add new event information,
###   then run the script which executes the helper application to setup GFs calc
###
# vi setupMT.csh
# setupMT.csh

setupMT -hspec -z 16 -ev 36.9077 -90.543 -mod cus \
         -ot "2021-11-18T02:53:04" -com "New Madrid, MO" \
         -fil 0.075 0.150 -data ../Data -resp ../Resp  ../Data/*Z.?.SAC ../Data/*Z.SAC

##
## after setupMT.csh run makeglib.csh to do the GFs calculations
##
makeglib.csh

##############################################################################
###   write the greens function out into different format 
### Create test Greens functions using glib files for depth of 14 km
###  this depth is the best fit given an origin time of 2021-11-18T02:53:04
##############################################################################

foreach i ( *.glib )
 grn2Mxy mtdegfree=5 z=14 verbose glib=$i
end

### add these two line in file run.csh replacing old ones
###
#### macos/Darwin or Linux ####
####
if( `uname` == "Darwin" ) then
  sed -i.bak 's/glib2inv par=mtinv.par noverbose parallel/glib2inv par=mtinv.par noverbose parallel test_special/' run.csh
  sed -i.bak 's/mtinv AutoAuth/mtinv special AutoAuth/' run.csh
else if(  `uname` == "Linux" ) then
  sed -i 's/glib2inv par=mtinv.par noverbose parallel/glib2inv par=mtinv.par noverbose parallel test_special/' run.csh
  sed -i 's/mtinv AutoAuth/mtinv special AutoAuth/' run.csh
else
  echo "unknown"
endif

run.csh

#### macos/Darwin or Linux ####
####
if( `uname` == "Darwin" ) then
  sed -i.bak 's/par=mtinv.par/par=mtinv.par special/' run2.csh
else if(  `uname` == "Linux" ) then
  sed -i 's/par=mtinv.par/par=mtinv.par special/' run2.csh
else
  echo "unknown"
endif

run2.csh

#
# full-MT solution is done, view plot gmtwf.000.jpg and index.html in a browser
# compare
#       ../../test_grn2Mxy/dev/plot_T004.0sec_Z014.0km_.p01.jpg
# with
#       ../../2021-11-18T025303_Missouri/orig/dev/plot_T004.0sec_Z014.0km_.p01.jpg
#
# open ../../test_grn2Mxy/dev/plot_T004.0sec_Z014.0km_.p01.jpg ../../2021-11-18T025303_Missouri/orig/dev/plot_T004.0sec_Z014.0km_.p01.jpg

##############################################################################
# full-MT solution
##############################################################################

### next do the full-MT, first clone the dev directory to full, use rsync instead of copy
###
cd ..
#cp -Rp dev full
rsync -av --progress ./dev/ ./full --exclude hspec96_GFs
cd full

##############################################################################
###   write the greens function out into different format
### Create test Greens functions using glib files for depth of 14 km
###  this depth is the best fit given an origin time of 2021-11-18T02:53:04
##############################################################################
rm -rf CCM CGM3 GNAR HENM PENM SIUC SLM WVT
foreach i ( *.glib )
 grn2Mxy mtdegfree=6 z=14 verbose glib=$i
end

###
### edit file run.csh: set DEGFREE=6  # 1-isotropic_mt 5-deviatoric_mt 6-full_mt
###  or use sed to find and replace, instead of manually editing with vi
###
#vi run.csh

#### macos/Darwin or Linux ####
####
if( `uname` == "Darwin" ) then
  sed -i.bak 's/DEGFREE=5/DEGFREE=6/' run.csh
else if(  `uname` == "Linux" ) then
  sed -i 's/DEGFREE=5/DEGFREE=6/' run.csh
else
  echo "unknown"
 sed 's/DEGFREE=5/DEGFREE=6/' run.csh >! tmp
 mv tmp run.csh
 chmod a+x run.csh
endif

run.csh

#### macos/Darwin or Linux ####
####
if( `uname` == "Darwin" ) then
  sed -i.bak 's/par=mtinv.par/par=mtinv.par special/' run2.csh
else if(  `uname` == "Linux" ) then
  sed -i 's/par=mtinv.par/par=mtinv.par special/' run2.csh
else    
  echo "unknown"
endif

run2.csh

#
# full-MT solution is done, view plot gmtwf.000.jpg and index.html in a browser
# compare
#	../../test_grn2Mxy/full/plot_T004.0sec_Z014.0km_.p01.jpg 
# with
#	../../2021-11-18T025303_Missouri/orig/full/plot_T004.0sec_Z014.0km_.p01.jpg
#
# open ../../test_grn2Mxy/full/plot_T004.0sec_Z014.0km_.p01.jpg ../../2021-11-18T025303_Missouri/orig/full/plot_T004.0sec_Z014.0km_.p01.jpg

### mteig does not support special load feature for Mxy gfs ####

##############################################################################
# Network Sensitivity Plot (%Variance Reduction on the Lune)
##############################################################################
# mteig.csh
# open best.all.out.lune.jpg

