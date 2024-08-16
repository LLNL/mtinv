#!/bin/csh

### clean everything out
###
csh ./clean.csh

### convert sw4 GFs output to grnlib format
###
cd sw4
csh run_sw4_to_grnlib.csh
cd ..

### generate GFs fk-synthetics 
###
cd fk
csh makeglib.csh
cd ..

###
### compare fk-reflect Gfs with sw4 GFs
###
cat >! compare_grn_gmt.par << EOF
z=1.0
verbose
bpfilter lf=0.01 hf=0.25 npole=3 npass=2
clean
EOF

compare_grn_gmt par=compare_grn_gmt.par glib1=sw4/IM.TKL..cus.glib    glib2=fk/IM.TKL..cus.glib    
compare_grn_gmt par=compare_grn_gmt.par glib1=sw4/N4.T50A.00.cus.glib glib2=fk/N4.T50A.00.cus.glib 
compare_grn_gmt par=compare_grn_gmt.par glib1=sw4/N4.U49A.00.cus.glib glib2=fk/N4.U49A.00.cus.glib 
compare_grn_gmt par=compare_grn_gmt.par glib1=sw4/N4.V48A.00.cus.glib glib2=fk/N4.V48A.00.cus.glib 
compare_grn_gmt par=compare_grn_gmt.par glib1=sw4/N4.V53A.00.cus.glib glib2=fk/N4.V53A.00.cus.glib 
compare_grn_gmt par=compare_grn_gmt.par glib1=sw4/N4.W50A.00.cus.glib glib2=fk/N4.W50A.00.cus.glib 
compare_grn_gmt par=compare_grn_gmt.par glib1=sw4/N4.W52A.00.cus.glib glib2=fk/N4.W52A.00.cus.glib 
compare_grn_gmt par=compare_grn_gmt.par glib1=sw4/N4.X51A.00.cus.glib glib2=fk/N4.X51A.00.cus.glib 
compare_grn_gmt par=compare_grn_gmt.par glib1=sw4/US.TZTN.00.cus.glib glib2=fk/US.TZTN.00.cus.glib 
