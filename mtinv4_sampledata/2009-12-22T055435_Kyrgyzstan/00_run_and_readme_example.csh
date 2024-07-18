#!/bin/csh
##############################################################################
# Example Dataset for testing mtinv version 4
#
# mb 4.4 - 45 km ESE of Toktogul, Kyrgyzstan
# 2009-12-22 05:54:35 (UTC) 41.678°N 73.423°E 10.0 km depth
# https://earthquake.usgs.gov/earthquakes/eventpage/usp000h58r/executive
#
# ./mtinv4_sampledata/2009-12-22T055435_Kyrgyzstan
#
## Note, this is dam construction chemical explosion so it will not fit a devatoric MT, 
##  must also do full-MT to get the best fit mechanism
##############################################################################

csh ./clean.csh

unpack.csh 2009-12-22-mb44-kyrgyzstan-4.tar

##############################################################################
# deviatoric-MT solution
##############################################################################
cd dev

### edit setupMT.csh script and uncomment approriate line or add new event information, 
###   then run the script which executes the helper application to setup GFs calc
###
# vi setupMT.csh
# setupMT.csh

setupMT -hspec -z  2 -ev 41.7392 +73.3770 -mod wus \
              -ot "2009-12-22T05:54:35.0" -com "Kambrata, Kyrgyzstan" \
              -fil 0.033 0.100 -data ../Data -resp ../Resp ../Data/*Z.?.SAC ../Data/*Z.SAC

##
## after setupMT.csh run makeglib.csh to do the GFs calculations
##
makeglib.csh

### other optional tests  - makeglib.csh
# dump grnsfuncs wus.par : nodump -> dump
# turn on verbose wus.par : noverbose -> verbose

##############################################################################
# run the deviatoric-MT solution
##############################################################################
run.csh

#### optional tests
#### glib2inv nodumpgrn  -> dumpgrn   glib2inv par=mtinv.par verbose parallel dumpgrn verbose
####          notest_special -> test_special

#### optional tests
#### grnlib2sac tests
# cp ../fkstrain.csh .
# fkstrain.csh
# grnlib2sac glib=KN.TKM2..wus.glib z=2 dumpgrn wavetype="Rotational"
#
# foreach i ( *.glib )
#  grnlib2sac glib=$i z=2 dumpgrn wavetype="Surf/Pnl" mkdir plotgrn
# end

#### things to do:
# view plots, open plot*jpg and results.5.jpg
# based on plot results, edit file run.csh and change filters, remove or add stations, change time shifts 
# hint 1: bandpass filter 0.033 - 0.100 Hz
# hint 2: use ARLS, change "n" -> "y"
# hint 3: best DevMT origin-time is 05:54:34 best FullMT origin-time is 05:54:35
# rerun run.csh

###
### when good then run2.csh
###
run2.csh

###
### deviatoric MT solution is done, view plot gmtwf.000.jpg or view index.html in a browser
### Note, this is a mine collapse so it will not fit a devatoric MT, must use full-MT
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

# full-MT solution is done, view plot gmtwf.000.jpg or view index.html in a browser

##############################################################################
# Network Sensitivity Plot (%Variance Reduction on the Lune)
##############################################################################

mteig.csh
open best.all.out.lune.jpg
