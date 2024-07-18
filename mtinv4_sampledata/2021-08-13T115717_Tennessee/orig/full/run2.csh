#!/bin/csh 
###
### Created by mtbestfit fitType=BEST VARIANCE-REDUCTION
### ot=-0 fsec=35 z=2 vred=90.4835 fit=90.4835 pdc=24 piso=49 pclvd=27
### fit_max=90.4835 fit_max_threshold=20 vred_diff=0 vred_diff_threshold=30
###
setenv MTINV_GMT_GRID_FILE /Users/ichinose1/Work/topogmt/etopo5.grd
setenv MTINV_GMT_INT_FILE /Users/ichinose1/Work/topogmt/etopo5.int
setenv MTINV_GMT_CPT_FILE /Users/ichinose1/Work/topogmt/etopo5.cpt
setenv MTINV_DATABASE_FILE /Users/ichinose1/Work/mtinv.v4.0.1/data/mt.db

set DEGFREE=6  # 1-isotropic_mt 5-deviatoric_mt 6-full_mt

###
### Clean Up
###
/bin/rm -f email_T???.?sec_Z???.?km_.txt plot_T???.?sec_Z???.?km_.p??.jpg *.ps

### MAKE GMT PLOT WITH LOCATION/STATION/SOLUTION ###
###
mtinv \
	 evid=-1       \
	 ts0=0         \
	 par=mtinv.par  \
	 gmt5             \
	 mtdegfree=${DEGFREE} \
	 gmtmap           \
	 write_emails     \
	 dumpxy nodumpsac print_gmtwf_mt_info \
	 sqlite             \
	 PltXcorLabel   \
	 use_snr        \
	 minsnr=3      \
	 ctol=0.85 shift  \
	 maxshift=10 >> mtinv.out

gmt psconvert -Z -Tj -E300 plot_T???.?sec_Z???.?km_.p??.ps
csh gmtmap.csh
#open gmtmap.jpg

### dumpxy option for publication quality GMT plots
gmtwf.csh


sqlite3 ${MT_DATABASE_FILE} << EOF
.read insert.sql
.quit
EOF

# updatedb conflicts in unix/Linux, changed name to updateMTdb 
updateMTdb

### mtbestfit : current MT_DATABASE_FILE=/Users/ichinose1/Work/mtinv.v4.0.1/data/mt.db
# list_MTdb.csh
print_MTdb.csh > db.txt
# remove_MTdb.csh
###
### run mtscreen.py ###
###
mtscreen.py -mxx -0.293 -mxy 0.013 -mxz 0.329 -myy -0.356 -myz 0.104 -mzz -0.832
