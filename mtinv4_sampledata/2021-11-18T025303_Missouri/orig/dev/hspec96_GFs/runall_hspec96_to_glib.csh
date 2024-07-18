#!/bin/csh


###
### do the hspec96 fk computations here
###
foreach nsl ( NM.CGM3.00 NM.PENM.00 NM.HENM.00 NM.GNAR.00 IU.CCM.00 IU.CCM.10 NM.SIUC.00 NM.SLM.00 IU.WVT.10 IU.WVT.00 )
echo ${nsl}
cd ${nsl}
pwd
csh ${nsl}.cus.hspec.csh
cd ..
end


###
### create glib files from hspec96 output
###
cat >! hspec96_to_grnlib.par << EOF
velmod=cus
modeldb=/Users/ichinose1/Work/mtinv.v4.0.1/data/modeldb
stadb=../../Data/rdseed.stations
zrange=1,1,25
evla=36.9077
evlo=-90.543
EOF

hspec96_to_grnlib par=hspec96_to_grnlib.par net=NM sta=CGM3 loc="00" ./NM.CGM3.00/*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=NM sta=PENM loc="00" ./NM.PENM.00/*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=NM sta=HENM loc="00" ./NM.HENM.00/*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=NM sta=GNAR loc="00" ./NM.GNAR.00/*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=IU sta=CCM loc="00" ./IU.CCM.00/*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=IU sta=CCM loc="10" ./IU.CCM.10/*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=NM sta=SIUC loc="00" ./NM.SIUC.00/*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=NM sta=SLM loc="00" ./NM.SLM.00/*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=IU sta=WVT loc="10" ./IU.WVT.10/*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=IU sta=WVT loc="00" ./IU.WVT.00/*.SAC
