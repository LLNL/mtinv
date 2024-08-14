#!/bin/csh

###
### this script is cloned from  
### mtinv.v4.0.2/mtinv4_sampledata/2021-08-13T115717_Tennessee/dev/makeglib.csh
### but is a stand alone script for purposes of computing GFs in comparisons 
###

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
###
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

cat >! cus.par << EOF
velmod=cus
zrange=1,2,1  ### just compute 1 and 2 km depths for this test
evla=35.8767
evlo=-84.898
dt=0.15
nt=2048
fmax=2.0  ### since we are comparing sw4 synths at hi-freqs, go up to max 2Hz, although 0.05 sps is 10Hz Nyquist
t0=0.
redv=18.
damp=1.
kmax=20000000
eps=0.000001
smin=0.000001
modeldb=./
stadb=./rdseed.stations
noverbose
nodump
EOF

cat >! mkgrnlib.par << EOF
### station-code network-code location-code mkgrnlib.parfile dt(sec/sample) ### comments
W50A     N4     "00" cus.par     0.050 ### R=    84 Az=207 N4.W50A.00   nchan=1 nseg=1 (HHZ)
TKL      IM       "" cus.par     0.050 ### R=   104 Az=103 IM.TKL.      nchan=1 nseg=1 (BHZ)
U49A     N4     "00" cus.par     0.050 ### R=   106 Az=312 N4.U49A.00   nchan=1 nseg=1 (HHZ)
W52A     N4     "00" cus.par     0.060 ### R=   124 Az=134 N4.W52A.00   nchan=1 nseg=1 (HHZ)
T50A     N4     "00" cus.par     0.060 ### R=   127 Az=002 N4.T50A.00   nchan=1 nseg=1 (HHZ)
TZTN     US     "00" cus.par     0.070 ### R=   142 Az=058 US.TZTN.00   nchan=1 nseg=1 (BHZ)
X51A     N4     "00" cus.par     0.070 ### R=   146 Az=179 N4.X51A.00   nchan=1 nseg=1 (HHZ)
V48A     N4     "00" cus.par     0.070 ### R=   174 Az=266 N4.V48A.00   nchan=1 nseg=1 (HHZ)
V53A     N4     "00" cus.par     0.080 ### R=   189 Az=096 N4.V53A.00   nchan=1 nseg=1 (HHZ)
EOF

multithread_mkgrnlib \
     parfile=mkgrnlib.par \
     executable_pathname=${MTINV_PATH}/bin/mkgrnlib > multithread_mkgrnlib.out
