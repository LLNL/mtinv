#!/bin/csh

# psbasemap -R-180/+180/0/1 -JX4i/4i -Bxf10a90 -Byf0.05a0.1 -BnSeW -P -K >! plot.ps
# driver dangle=0.1 kappa=0   | psxy -R -J -W1p,black   -O -K >> plot.ps
# driver dangle=0.1 kappa=0.5 | psxy -R -J -W1p,blue    -O -K >> plot.ps
# driver dangle=0.1 kappa=1   | psxy -R -J -W1p,cyan    -O -K >> plot.ps
# driver dangle=0.1 kappa=2   | psxy -R -J -W1p,green   -O -K >> plot.ps
# driver dangle=0.1 kappa=3   | psxy -R -J -W2p,brown   -O -K >> plot.ps
# driver dangle=0.1 kappa=4   | psxy -R -J -W1p,red     -O -K >> plot.ps
# driver dangle=0.1 kappa=8   | psxy -R -J -W1p,magenta -O    >> plot.ps
# psconvert -Tj -E300 -A -V -P plot.ps
# open plot.jpg

driver3d nfac=17000 dxy=0.25 vMF3d   kappa=1.3 | gmt xyz2grd -R-30/30/-90/90 -I0.25/0.25 -Glune1.grd -Vn
driver3d nfac=10000 dxy=0.25 novMF3d kappa=2.0 | gmt xyz2grd -R-30/30/-90/90 -I0.25/0.25 -Glune2.grd -Vn

set max1=( ` gmt grdinfo -C lune1.grd | awk '{print $7}' ` )
set max2=( ` gmt grdinfo -C lune2.grd | awk '{print $7}' ` )

set dz=1000
#gmt makecpt -Crainbow -T0/${max1}/$dz -D -Z  >! vred1.cpt
#gmt makecpt -Crainbow -T0/${max2}/$dz -D -Z  >! vred2.cpt
gmt makecpt -Ccategorical -T0/${max1}/$dz -D -Z  >! vred1.cpt
gmt makecpt -Ccategorical -T0/${max2}/$dz -D -Z  >! vred2.cpt

set PS=lune.ps
set JPG=lune.jpg

gmt grdimage -R-30/30/-90/90 -JH0/1.8i -Cvred1.cpt lune1.grd -P -V -K >! $PS
gmt psbasemap -R -JH -Bxf10g10a30 -Byf10g10a30 -Bnsew -O -V -K >> ${PS}
gmt psscale -Dx2.0i/0.1i+w2i/0.18i+e0.1i -Bf${dz}a${dz}+l"vMF3d" -Cvred1.cpt -K -V -O >> ${PS}

gmt grdimage -R-30/30/-90/90 -JH0/1.8i -Cvred2.cpt lune2.grd -O -V -K -X3i >> $PS
gmt psbasemap -R -JH -Bxf10g10a30 -Byf10g10a30 -Bnsew -O -V -K >> ${PS}

gmt psscale -Dx2.0i/0.1i+w2i/0.18i+e0.1i -Bf${dz}a${dz}+l"vM1d" -Cvred2.cpt -V -O >> ${PS}

gmt psconvert -Tj -E300 -V -P ${PS}
open $JPG
