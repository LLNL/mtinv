#!/bin/csh

tar zxf cus-sw4-GF-NoFilter-H200.tar.gz

cat >! rdseed.stations << EOF
T50A N4 37.0204 -84.8384 302 "HHZ00 " 
U49A N4 36.5129 -85.7796 234 "HHZ00 " 
V48A N4 35.74 -86.8219 278 "HHZ00 " 
V53A N4 35.6694 -82.8124 681 "HHZ00 " 
W50A N4 35.2002 -85.3119 587 "HHZ00 " 
W52A N4 35.0935 -83.9277 519 "HHZ00 " 
X51A N4 34.5658 -84.8574 214 "HHZ00 " 
TZTN US 36.5439 -83.549 394 "BHZ00 " 
TKL IM 35.658 -83.774 351 "BHZ " 
EOF

cat >! cus.mod << EOF
### Central US model 
### R. B. Herrmann, H. Benz, C. J. Ammon; 
### Monitoring the Earthquake Source Process in North America. 
### Bulletin of the Seismological Society of America ; 101 (6): 2609–2625. 
### doi: https://doi.org/10.1785/0120110095
###
# thick  vp     qp     vs       qb   rho
   1.0  5.00   581.0  2.89   258.0   2.50
   9.0  6.10   625.0  3.52   275.5   2.73 
  10.0  6.40   671.1  3.70   275.5   2.82 
  20.0  6.70   671.1  3.87   297.6   2.90 
 700.0  8.15   515.4  4.70   232.0   3.36
EOF

cat >! sw4_to_grnlib.par << EOF
noverbose
stadb=./rdseed.stations
#
evla=35.8767
evlo=-84.898
evdp=1.0
#
# ot=2021-08-13T11:57:35
#
velmod=cus
modeldb=./
#
## cmp info, and amplitude scaling
#
ncmp=10
######code,directory,ew,ns,z,amp scaling
cmp01=zss,cus.sw4output.ZSS,y,x,z,1,1,1
cmp02=zds,cus.sw4output.ZDS,y,x,z,1,1,1
cmp03=zdd,cus.sw4output.ZDD,y,x,z,1,1,1
cmp04=zex,cus.sw4output.ZEX,y,x,z,1,1,1
cmp05=rss,cus.sw4output.ZSS,y,x,z,1,1,1
cmp06=rds,cus.sw4output.ZDS,y,x,z,1,1,1
cmp07=rdd,cus.sw4output.ZDD,y,x,z,1,1,1
cmp08=rep,cus.sw4output.ZEX,y,x,z,1,1,1
cmp09=tss,cus.sw4output.TSS,y,x,z,1,1,1
cmp10=tds,cus.sw4output.TDS,y,x,z,1,1,1
##
## processing, rotate to great-circle-path
##             integrate from velocity to displacement
##             scale amplitudes using reference or base moment 
##
rotate gcp # or az=XXX
integrate
##
## no scaling, comparing SW4 output processes with SAC with itself processed using this code
##
base_moment_scaling rbh_to_yz
#nobase_moment_scaling 
EOF

sw4_to_grnlib par=sw4_to_grnlib.par net=N4 sta=T50A loc=00
grnlib2sac z=1 glib=N4.T50A.00.cus.glib dumpgrn

sw4_to_grnlib par=sw4_to_grnlib.par net=N4 sta=U49A loc=00
grnlib2sac z=1 glib=N4.U49A.00.cus.glib dumpgrn

sw4_to_grnlib par=sw4_to_grnlib.par net=N4 sta=V48A loc=00
grnlib2sac z=1 glib=N4.V48A.00.cus.glib dumpgrn

sw4_to_grnlib par=sw4_to_grnlib.par net=N4 sta=V53A loc=00
grnlib2sac z=1 glib=N4.V53A.00.cus.glib dumpgrn

sw4_to_grnlib par=sw4_to_grnlib.par net=N4 sta=W50A loc=00
grnlib2sac z=1 glib=N4.W50A.00.cus.glib dumpgrn

sw4_to_grnlib par=sw4_to_grnlib.par net=N4 sta=W52A loc=00
grnlib2sac z=1 glib=N4.W52A.00.cus.glib dumpgrn

sw4_to_grnlib par=sw4_to_grnlib.par net=N4 sta=X51A loc=00
grnlib2sac z=1 glib=N4.X51A.00.cus.glib dumpgrn

sw4_to_grnlib par=sw4_to_grnlib.par net=US sta=TZTN loc=00
grnlib2sac z=1 glib=US.TZTN.00.cus.glib dumpgrn

sw4_to_grnlib par=sw4_to_grnlib.par net=IM sta=TKL loc=""
grnlib2sac z=1 glib=IM.TKL..cus.glib dumpgrn

