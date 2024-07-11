#!/bin/csh

set  PS=plot_and_compare_models.ps
set JPG=plot_and_compare_models.jpg

gmt psbasemap -R0/10/-100/0 -JX5i/5i \
   -Bxf0.2a1+l"P and S-Velocities (km/sec)" \
   -Byf2a10+l"Depth (km)" -BNSeW+t"1D Earth Models" -P -K >! ${PS}

pltmod modeldb=. velmod=iasp gmt pline="-W2p,black"        sline="-W2p,black"        | gmt psxy -R -JX -K -O >> ${PS}
pltmod modeldb=. velmod=wus  gmt pline="-W1.2p,red,5_2:0p" sline="-W1.2p,red,5_2:0p" | gmt psxy -R -JX -K -O >> ${PS}
pltmod modeldb=. velmod=cus  gmt pline="-W1.0p,blue"       sline="-W1.0p,blue"       | gmt psxy -R -JX -K -O >> ${PS}

gmt pslegend -R -JX -F+p1p+gtan -D1/-90/3i/BL -O >> ${PS} << EOF
G -0.15
H 18 Times-Roman 1-D Earth Models
G 0.05i
D 0.0i 1p,black
N 3
V 0 1p
S 0.2i r 0.8c,0.15c red   0.5p 0.45i wus
S 0.2i r 0.8c,0.15c blue  0.5p 0.45i cus
S 0.2i r 0.8c,0.15c black 0.5p 0.45i iasp
EOF

gmt psconvert -Tj -E300 -V ${PS}
open $JPG
/bin/rm -f ${PS}
