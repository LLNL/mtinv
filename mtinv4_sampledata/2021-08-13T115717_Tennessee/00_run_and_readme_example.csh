#!/bin/csh
##############################################################################
# Example Dataset for testing mtinv version 4
# 
# M 4.3 Mine Collapse - 2 km ENE of Crab Orchard, Tennessee
# 2021-08-13 11:57:34 (UTC) 35.916°N 84.853°W 0.2 km depth
# https://earthquake.usgs.gov/earthquakes/eventpage/se60133543/executive
#
# /Users/ichinose1/Work/mtinv4_sampledata/2021-08-13T115717_Tennessee
#
## Note, this is a mine collapse so it will not fit a devatoric MT, 
##  must also do full-MT to get the best fit
#
##############################################################################

### clean the Data, Resp, dev, and full directories, and redo the unpacking 
###  of the EarthScope/IRIS data 
###
csh ./clean.csh

unpack.csh 2021-08-13-mw43-tennessee-4.tar

##############################################################################
# deviatoric-MT solution
##############################################################################
cd dev

### edit setupMT.csh script and uncomment approriate line or add new event information,
###   then run the script which executes the helper application to setup GFs calc
###
# vi setupMT.csh
# setupMT.csh

setupMT -hspec -z  2 -ev 35.8767 -84.898 -mod cus \
              -ot "2021-08-13T11:57:35" -com "Franklin Mine, TN" \
              -fil 0.075 0.120 -data ../Data -resp ../Resp ../Data/*Z.?.SAC ../Data/*Z.SAC

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
# hint: best bandpass filter 0.075 - 0.120 Hz
# hint: best origin time is 35 sec
# rerun run.csh

###
### when good then run2.csh
###
run2.csh

###
### deviatoric MT solution is done, view plot gmtwf.000.jpg or view index.html in a browser
### Note, this is a mine collapse so it will not fit a devatoric MT, must use full-MT
### for the NSS source-type ID/discrim
###

##############################################################################
# full-MT solution
##############################################################################

### next do the full-MT, first clone the dev directory to full, use rsync instead of copy
###
cd ..
# cp -Rp dev full
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
