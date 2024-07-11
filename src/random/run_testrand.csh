#!/bin/csh

foreach seed ( -2 -1 0 1 2 )
set  PS=plot${seed}.ps
set JPG=plot${seed}.jpg

/bin/rm -f testrand.${seed}.out $PS $JPG

./testrand n=4000 seed=${seed} >! testrand.${seed}.out

awk '{print $1 }' testrand.${seed}.out | gmt pshistogram -R+0/1/1/500 -JX4i/1.5i -Bxf0.1a0.1 -Byf20a100 -BnSeW -W0.1 -L1p,black     -P -K >! ${PS}
awk '{print $2 }' testrand.${seed}.out | gmt pshistogram -R+0/1/1/500 -JX4i/1.5i                               -W0.1 -L1p,red       -O -K >> ${PS}

awk '{print $3 }' testrand.${seed}.out | gmt pshistogram -R-1/1/1/500 -JX4i/1.5i -Bxf0.1a0.2 -Byf20a100 -BnSeW -W0.2 -L1p,black -Y2i -O -K >> ${PS}
awk '{print $4 }' testrand.${seed}.out | gmt pshistogram -R-1/1/1/500 -JX4i/1.5i                               -W0.2 -L1p,red        -O -K >> ${PS}

awk '{print $5 }' testrand.${seed}.out | gmt pshistogram -R-1/1/1/500 -JX4i/1.5i -Bxf0.1a0.2 -Byf20a100 -BnSeW -W0.2 -L1p,black -Y2i -O -K >> ${PS}
awk '{print $6 }' testrand.${seed}.out | gmt pshistogram -R-1/1/1/500 -JX4i/1.5i                               -W0.2 -L1p,red        -O -K >> ${PS}

awk '{print $7 }' testrand.${seed}.out | gmt pshistogram -R-1/1/1/500 -JX4i/1.5i -Bxf0.1a0.2 -Byf20a100 -BnSeW -W0.2 -L1p,black -Y2i -O -K >> ${PS}
awk '{print $8 }' testrand.${seed}.out | gmt pshistogram -R-1/1/1/500 -JX4i/1.5i                               -W0.2 -L1p,red        -O    >> ${PS}

gmt psconvert -Tj -A -V -E300 ${PS}
/bin/rm -f ${PS}
open $JPG
end
