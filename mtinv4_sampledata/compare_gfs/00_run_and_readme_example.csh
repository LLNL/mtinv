#!/bin/csh
##############################################################################
# Example Dataset for testing mtinv version 4
# 
# M 4.0 - 7 km S of Williamsville, Missouri
# 2021-11-18 02:53:03 (UTC) 36.908°N 90.543°W 16.5 km depth
# https://earthquake.usgs.gov/earthquakes/eventpage/nm60363582/executive
# 
# This test the hspec96 GFs calculations and compare_grn_gmt, and uses a 
#  smaller 3-sta test dataset than what was previously tested to save time
# 
##############################################################################

##############################################################################
### clean the Data, Resp, dev, and full directories, and redo the unpacking
###  of the EarthScope/IRIS data
##############################################################################
csh ./clean.csh

unpack.csh 2021-11-18-mwr40-new-madrid-missouri-region-1.tar 

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
## use Y. Zeng's Greens Function calculations
##
makeglib.csh

###
### use R.B.Herrmann's hspec96 for GFs calculations
###
cd hspec96_GFs
csh ./runall_hspec96_to_glib.csh
cp *.glib ..
cd ..

##############################################################################
# run the deviatoric-MT solution
##############################################################################
run.csh

###
### when good then run2.csh
###
run2.csh

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


cd ..
run_compare_rbh_yz.csh
