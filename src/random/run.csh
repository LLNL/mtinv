#!/bin/csh

foreach seed ( 1 2 3 4 )

set PS=plot.${seed}.ps
set JPG=plot.${seed}.jpg

testgasdev seed=-${seed} n=20000 mean=1 stdev=0.5 > testgasdev.out

cat >! gaussian.par << EOF
x0=-10
x1=+10
dx=0.1
yoffset=0
noxlog
EOF

gaussian par=gaussian.par a=811.0 mu=0 sig=1  > gaussian2.out
gaussian par=gaussian.par a=1569.0 mu=1 sig=0.5 > gaussian1.out

# gmtinfo testgasdev.out

### plot istogram gaussian distribution  column 2
###
set minmax=( ` awk '{ print $2 }' testgasdev.out | gmt pshistogram -I -W0.1 ` )
set R=" -R${minmax[1]}/${minmax[2]}/${minmax[3]}/${minmax[4]} "
echo $R
set J=" -JX6i/2i "
awk '{ print $2 }' testgasdev.out | gmt pshistogram $R $J -W0.1 -L1p,black -Ggray -Bxf1a1 -Byf100a200 -BnSeW -P -K >! ${PS}
gmt psxy $R $J gaussian1.out -W1p,red -O -K >> ${PS}

### plot histogram gaussian distribution column 3
###
set minmax=( ` awk '{ print $3 }' testgasdev.out | gmt pshistogram -I -W0.1 ` )
set R=" -R${minmax[1]}/${minmax[2]}/${minmax[3]}/${minmax[4]} "
echo $R
set J=" -JX6i/2i "
awk '{ print $3 }' testgasdev.out | gmt pshistogram $R $J -W0.1 -L1p,black -Ggray -Bxf1a2 -Byf100a200 -BnSeW -O -K -Y3i >> ${PS}
gmt psxy $R $J gaussian2.out -W1p,red -O -K >> ${PS}

### plot time history
###
set minmax=( ` awk '{ print $1, $2 }' testgasdev.out | gmt gmtinfo -C ` )
set R=" -R${minmax[1]}/${minmax[2]}/${minmax[3]}/${minmax[4]} "
set J=" -JX6i/4i "
awk '{ print $1, $2 }' testgasdev.out | gmt psxy $R $J -Sc0.02i -W0.25p,black -Gwhite -O -Vn -Y3i -K >> ${PS}
gmt psbasemap $R $J -Bxf1000a5000 -Byf1a1 -BnSeW -O >> ${PS}

gmt psconvert -Tj -E300 ${PS}
open ${JPG}

/bin/rm -f ${PS}

end
