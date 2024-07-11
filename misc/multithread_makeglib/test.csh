#!/bin/csh

cat >! wus.par << EOF
velmod=wus
zrange=3,3,54
evla=39.395
evlo=72.812
dt=0.1
nt=2048
fmax=0.2
t0=-1.0
redv=14.
damp=1.
kmax=9999999
eps=0.00001
smin=0.00001
modeldb=/Users/ichinose1/Work/mtinv.v4.0.0/data/modeldb
stadb=rdseed.stations
noverbose
nodump
EOF
                                                                                                                                                  
#mkgrnlib par=wus.par stnm=AAK   net=II dt=0.1
#mkgrnlib par=wus.par stnm=KKAR  net=KZ dt=0.1
#mkgrnlib par=wus.par stnm=MKAR  net=KZ dt=0.25
#mkgrnlib par=wus.par stnm=ABKAR net=KZ dt=0.25
#mkgrnlib par=wus.par stnm=MAKZ  net=IU dt=0.25
#mkgrnlib par=wus.par stnm=EKS2  net=KN dt=0.1
#mkgrnlib par=wus.par stnm=USP   net=KN dt=0.1
#mkgrnlib par=wus.par stnm=KBK   net=KN dt=0.1
#mkgrnlib par=wus.par stnm=ULHL  net=KN dt=0.1
#mkgrnlib par=wus.par stnm=UCH   net=KN dt=0.1
#mkgrnlib par=wus.par stnm=TKM2  net=KN dt=0.1
#mkgrnlib par=wus.par stnm=CHM   net=KN dt=0.1
#mkgrnlib par=wus.par stnm=AML   net=KN dt=0.1
                                                                                                                                                  

cat >! mkgrnlib.par << EOF
AAK   II "" wus.par 0.1
AAK   II 00 wus.par 0.1
KKAR  KZ "" wus.par 0.1
KKAR  KZ 10 wus.par 0.1
MKAR  KZ "" wus.par 0.25
ABKAR KZ "" wus.par 0.25
MAKZ  IU "" wus.par 0.25
EKS2  KN "" wus.par 0.1
USP   KN "" wus.par 0.1
KBK   KN "" wus.par 0.1
ULHL  KN "" wus.par 0.1
UCH   KN "" wus.par 0.1
AML   KN "" wus.par 0.1
EOF

cat >! mkgrnlib2.par << EOF
TES01 XX "" wus.par 0.1
TES02 XX "" wus.par 0.1
TES03 XX "" wus.par 0.1
TES04 XX "" wus.par 0.1
TES05 XX "" wus.par 0.1
TES06 XX "" wus.par 0.1
TES07 XX "" wus.par 0.1
TES08 XX "" wus.par 0.1
TES09 XX "" wus.par 0.1
TES10 XX "" wus.par 0.1
TES11 XX "" wus.par 0.1
TES12 XX "" wus.par 0.1
TES13 XX "" wus.par 0.1
TES14 XX "" wus.par 0.1
TES15 XX "" wus.par 0.1
TES16 XX "" wus.par 0.1
TES17 XX "" wus.par 0.1
TES18 XX "" wus.par 0.1
TES19 XX "" wus.par 0.1
TES20 XX "" wus.par 0.1
TES21 XX "" wus.par 0.1
TES22 XX "" wus.par 0.1
TES23 XX "" wus.par 0.1
TES24 XX "" wus.par 0.1
TES25 XX "" wus.par 0.1
TES26 XX "" wus.par 0.1
TES27 XX "" wus.par 0.1
TES28 XX "" wus.par 0.1
TES29 XX "" wus.par 0.1
TES30 XX "" wus.par 0.1
EOF

multithread_mkgrnlib parfile=mkgrnlib.par \
                     executable_pathname=/Users/ichinose1/Work/mtinv.v4.0.0/bin/mkgrnlib > test.out
