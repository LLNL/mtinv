#!/bin/csh

psbasemap -R0/3100/0/400 -JX6i/6i \
  -Bf20a500:"Epicentral Distance (km)":/f10a50:"P-wave Travel Time (sec)":nSeW -P -K >! ttplot.ps

psxy -R -JX -M taup.out -W1.5p/0 -O -K >> ttplot.ps

awk '{ print $6, $12 }' run.out | psxy -R -JX -M -W1.5p/255/0/0 -O -K >> ttplot.ps

pstext -R -JX -N -D0.1i/-0.1i -O -K >> ttplot.ps << EOF
500 350 14 0 1 0 TAUP_TIME AK135 (SRC Depth 2 km)
500 300 14 0 1 0 MKGRNLIB/RAYP 
EOF

psxy -R -JX -M -N -O -K >> ttplot.ps << EOF
> -W5p/0
100 350
500 350
> -W5p/255/0/0
100 300
500 300
EOF

awk '{ if( $1 != ">") print $1, $2 }' taup.out > taup.out.x
awk '{ print $12 }' run.out > run.out.x

pr -m -t taup.out.x run.out.x | awk '{ print $1, $3-$2 }' | \
  psxy -R0/3100/-2/2 -JX6i/2i -Y7i -Bf20a500:"Epicentral Distance (km)":/f0.2a1:"D_tt (sec)":SW -W1p/0 -O -K >> ttplot.ps

awk '{ print $6, $10 }' run.out | \
  psxy -R0/3100/0.05/0.2 -JX6i/2i -Bf20a500:"Epicentral Distance (km)":/f0.02a0.04:"Ray P(s/km)":nE -W1.5p/0/0/255 -O >> ttplot.ps

gs ttplot.ps
