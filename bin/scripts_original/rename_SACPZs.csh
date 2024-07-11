#!/bin/csh
foreach i ( SACPZ.* )
 set net=( ` echo $i | cut -f2 -d. ` )
 set sta=( ` echo $i | cut -f3 -d. ` )
 set loc=( ` echo $i | cut -f4 -d. ` )
 set cha=( ` echo $i | cut -f5 -d. ` )
 if( $loc == "--" ) then
  set myloc=""
 else
  set myloc=${loc}
 endif
### echo $net $sta $myloc $cha
 set newfile=SAC_PZs_${net}_${sta}_${cha}_${myloc}_1960.001.00.00.00.00000_2599.365.23.59.59.99999
### echo $i $newfile
 /bin/mv -v $i $newfile
end
