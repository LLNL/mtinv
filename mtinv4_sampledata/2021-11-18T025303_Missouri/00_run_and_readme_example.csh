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
# Note, this is an earthquake so it will fit a devatoric MT,
#   also do full-MT to get the NSS source-type id/discrim
##############################################################################

### clean the Data, Resp, dev, and full directories, and redo the unpacking 
###  of the EarthScope/IRIS data 
###
csh ./clean.csh

unpack.csh 2021-11-18-mwr40-new-madrid-missouri-region.tar 

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
# run the deviatoric-MT solution
##############################################################################
run.csh

### optional grnlib2sac tests
# foreach i ( *.glib )
#  grnlib2sac glib=$i z=2 dumpgrn wavetype="Surf/Pnl" mkdir plotgrn
# end

#### things to do:
# view plots, open plot*jpg and results.5.jpg
# based on plot results, edit file run.csh and change filters, remove or add stations, change time shifts 
# hint: bandpass filter 0.075 - 0.150 Hz
# rerun run.csh
# when good then run2.csh

###
### when good then run2.csh
###
run2.csh

###
### deviatoric MT solution is done, view plot gmtwf.000.jpg or view index.html in a browser
### Note, this is an earthquake so it will fit a devatoric MT, next do the full-MT
### for the NSS source-type ID/discrim
###

##############################################################################
# full-MT solution
##############################################################################

### next do the full-MT, first clone the dev directory to full, use rsync instead of copy
###
cd ..
#cp -Rp dev full
rsync -av --progress ./dev/ ./full --exclude hspec96_GFs
cd full

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
run2.csh

# full-MT solution is done, view plot gmtwf.000.jpg and index.html in a browser

##############################################################################
# Network Sensitivity Plot (%Variance Reduction on the Lune)
##############################################################################

mteig.csh
open best.all.out.lune.jpg
