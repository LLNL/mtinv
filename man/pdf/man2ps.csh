#!/bin/csh
foreach i ( *.1 )
echo $i
groff -man $i > $i.ps
ps2pdf $i.ps
end
