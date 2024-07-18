#!/bin/csh


###
### do the hspec96 fk computations here
###
foreach nsl ( KR.ARLS. KN.EKS2. KN.UCH. KN.KBK. KN.CHM. KN.USP. KN.TKM2. KN.ULHL. KZ.KKAR. )
echo ${nsl}
cd ${nsl}
pwd
csh ${nsl}.wus.hspec.csh
cd ..
end


###
### create glib files from hspec96 output
###
cat >! hspec96_to_grnlib.par << EOF
velmod=wus
modeldb=/Users/ichinose1/Work/mtinv.v4.0.1/data/modeldb
stadb=../../Data/rdseed.stations
zrange=1,1,25
evla=41.7392
evlo=73.377
EOF

hspec96_to_grnlib par=hspec96_to_grnlib.par net=KR sta=ARLS loc="" ./KR.ARLS./*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=KN sta=EKS2 loc="" ./KN.EKS2./*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=KN sta=UCH loc="" ./KN.UCH./*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=KN sta=KBK loc="" ./KN.KBK./*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=KN sta=CHM loc="" ./KN.CHM./*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=KN sta=USP loc="" ./KN.USP./*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=KN sta=TKM2 loc="" ./KN.TKM2./*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=KN sta=ULHL loc="" ./KN.ULHL./*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=KZ sta=KKAR loc="" ./KZ.KKAR./*.SAC
