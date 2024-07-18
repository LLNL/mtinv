#!/bin/csh


###
### do the hspec96 fk computations here
###
foreach nsl ( N4.W50A.00 IM.TKL. N4.U49A.00 N4.W52A.00 N4.T50A.00 US.TZTN.00 N4.X51A.00 N4.V48A.00 N4.V53A.00 )
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
evla=35.8767
evlo=-84.898
EOF

hspec96_to_grnlib par=hspec96_to_grnlib.par net=N4 sta=W50A loc="00" ./N4.W50A.00/*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=IM sta=TKL loc="" ./IM.TKL./*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=N4 sta=U49A loc="00" ./N4.U49A.00/*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=N4 sta=W52A loc="00" ./N4.W52A.00/*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=N4 sta=T50A loc="00" ./N4.T50A.00/*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=US sta=TZTN loc="00" ./US.TZTN.00/*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=N4 sta=X51A loc="00" ./N4.X51A.00/*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=N4 sta=V48A loc="00" ./N4.V48A.00/*.SAC
hspec96_to_grnlib par=hspec96_to_grnlib.par net=N4 sta=V53A loc="00" ./N4.V53A.00/*.SAC
