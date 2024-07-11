#!/bin/csh

set mod=ak135

cat >! ${mod}.par << EOF
velmod=${mod}
zrange=2,2,22
evla=-0.1
evlo=-0.1
dt=0.15
nt=1024
fmax=0.5
t0=0.
redv=18.
damp=1.
kmax=20000
eps=0.0005
smin=0.0005
modeldb=../../data/modeldb
stadb=./rdseed.stations
verbose
nodump
EOF

rayp z=2 par=ak135.par stnm=TE08 net=XX
