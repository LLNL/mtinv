#!/bin/csh 
#### Realtime version - set in makepar #### 
set DEGFREE=5  # 1-isotropic_mt 5-deviatoric_mt 6-full_mt

cat >! mtinv.par << EOF
#### REGION COMMENT ############################
CM Franklin Mine, TN
#### Date and Origin Time ######################
OT 2021-08-13T11:57:35.00
#### Forward Calculations ######################
##    stk    dip    rak   Mw  evlo  evla   Z ##########
EV -999.0 -999.0 -999.0  0.0    -84.898    35.8767  15.0
#####################################################################################################
# sta net loc model  np pas lf  hf  nt  dt   tr  tt v/d  mulfac used(Y/N)  ts0  weight ###              #
W50A	N4	00	cus 3 2 0.075 0.120  1024  0.05 0.0 0.0 d  1.0 y  +0.00 +1.0 Surf/Pnl ### R=84.0 Az=207
TKL	IM	""	cus 3 2 0.075 0.120  1024  0.05 0.0 0.0 d  1.0 y  +0.00 +1.0 Surf/Pnl ### R=104.2 Az=103
U49A	N4	00	cus 3 2 0.075 0.120  1024  0.05 0.0 0.0 d  1.0 y  +0.00 +1.0 Surf/Pnl ### R=106.0 Az=312
W52A	N4	00	cus 3 2 0.075 0.120  1024  0.06 0.0 0.0 d  1.0 y  +0.00 +1.0 Surf/Pnl ### R=123.6 Az=134
T50A	N4	00	cus 3 2 0.075 0.120  1024  0.06 0.0 0.0 d  1.0 y  +0.00 +1.0 Surf/Pnl ### R=127.2 Az=2
TZTN	US	00	cus 3 2 0.075 0.120  1024  0.07 0.0 0.0 d  1.0 y  +0.00 +1.0 Surf/Pnl ### R=141.8 Az=58
X51A	N4	00	cus 3 2 0.075 0.120  1024  0.07 0.0 0.0 d  1.0 y  +0.00 +1.0 Surf/Pnl ### R=145.7 Az=179
V48A	N4	00	cus 3 2 0.075 0.120  1024  0.07 0.0 0.0 d  1.0 y  +0.00 +1.0 Surf/Pnl ### R=174.0 Az=266
#########################################################################################################
#V53A	N4	00	cus 3 2 0.075 0.120  1024  0.08 0.0 0.0 d  1.0 n  +0.00 +1.0 Surf/Pnl ### R=189.4 Az=96
EOF

### CLEAN UP ###
/bin/rm -f *.ginv *.data plot_T???.?sec_Z???.?km_.p??.ps email_T???.?sec_Z???.?km_.txt *.?.dat.xy *.?.syn.xy
/bin/rm -f plot_T???.?sec_Z???.?km_.p??.jpg plot_T???.?sec_Z???.?km_.p??.pdf *.sql
/bin/rm -f results.?.??? plotmech.??? plotz.??? gmtmap.??? mtinv.out multithread_mkgrnlib.out snr.out var_red.out
/bin/rm -f automt.txt

### PROCESS GREENS FUNCTIONS ###
glib2inv par=mtinv.par noverbose parallel

### PROCESS DATA ### 
sacdata2inv par=mtinv.par path=../Data respdir=../Resp noverbose nodumpsac parallel

# foreach ts0 ( -8 -7 -6 -5 -4 -3 -2 -1 -0.5 0 0.5 1 2 3 4 5 6 7 8 )
foreach ts0 ( -8 -7 -6 -5 -4 -3 -2 -1 0 1 2 3 4 5 6 7 8 )
 mtinv AutoAuth ts0=${ts0} par=mtinv.par gmt5 mtdegfree=${DEGFREE} use_snr minsnr=3 shift ctol=0.85 maxshift=10 >> mtinv.out
end ### loop over ts0

### CHECK ORIGIN TIME SHIFT ###
csh results.${DEGFREE}.csh
### uncomment for review
# open results.?.jpg

### convert each postscript file to jpg 
foreach ps ( plot_T???.?sec_Z???.?km_.p??.ps ) 
 gmt psconvert -Z -Tj -E120 ${ps}
end
### uncomment 
#open plot_T???.?sec_Z???.?km_.p??.jpg

### MAKE DEPTH SENSITIVITY PLOT ###
# csh plotz.csh
# open plotz.jpg

### MAKE DEPTH / OT-SHIFT SENSITIVITY PLOT ###
# csh plotmech.csh
# open plotmech.jpg

#csh gmtmap.csh
#open gmtmap.jpg

mtbestfit gmt5 evid=-1 db pretty_plot noforce_best_vred mteig decimate_factor=2 DataDir=../Data RespDir=../Resp