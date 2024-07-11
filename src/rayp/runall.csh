#!/bin/csh

set mod=ak135a

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
modeldb=../../data/modeldb/
stadb=./rdseed.stations
noverbose
nodump
EOF

cat >! ./rdseed.stations << EOF
TE01 XX  0.0  0.0 0.0
TE02 XX  0.2  0.2 0.0
TE03 XX  0.4  0.4 0.0
TE04 XX  0.6  0.6 0.0
TE05 XX  0.8  0.8 0.0
TE06 XX  1.0  1.0 0.0
TE07 XX  1.2  1.2 0.0
TE08 XX  1.4  1.4 0.0
TE09 XX  1.6  1.6 0.0
TE10 XX  1.8  1.8 0.0
TE11 XX  2.0  2.0 0.0
TE12 XX  2.2  2.2 0.0
TE13 XX  2.4  2.4 0.0
TE14 XX  2.6  2.6 0.0
TE15 XX  2.8  2.8 0.0
TE16 XX  3.0  3.0 0.0
TE17 XX  3.5  3.5 0.0
TE18 XX  4.0  4.0 0.0
TE19 XX  5.0  5.0 0.0
TE20 XX  6.0  6.0 0.0
TE21 XX  7.0  7.0 0.0
TE22 XX  8.0  8.0 0.0
TE23 XX  9.0  9.0 0.0
TE24 XX 10.0 10.0 0.0
TE25 XX 11.0 11.0 0.0
TE26 XX 12.0 12.0 0.0
TE27 XX 13.0 13.0 0.0
TE28 XX 14.0 14.0 0.0
TE29 XX 15.0 15.0 0.0
TE30 XX 16.0 16.0 0.0
TE31 XX 17.0 17.0 0.0
TE32 XX 18.0 18.0 0.0
TE33 XX 19.0 19.0 0.0
TE34 XX 20.0 20.0 0.0
EOF

set z = 2

cat /dev/null >! run.out

foreach i ( 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 )
  rayp z=${z} par=${mod}.par stnm=TE${i} net=XX >> run.out
end

gmt psmeca -R0/1/0/1 -JM5i -Sa2i -Gred -W1p,black -L1p,black -K -P >! test.ps << EOF
0.5 0.5 0 45 60 -80 5.0 test
EOF

gmt pspolar run.out -R0/1/0/1 -JM5i -N -D0.5/0.5 -Sc0.2i -Egreen -e1p,black -Gblue -T0.0/0/5/12  -W1p,black -M2i -O >> test.ps

gs test.ps
