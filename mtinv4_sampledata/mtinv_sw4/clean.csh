#!/bin/csh

### fk directory
###
/bin/rm -f fk/IM.TKL..cus.glib
/bin/rm -f fk/N4.T50A.00.cus.glib
/bin/rm -f fk/N4.U49A.00.cus.glib
/bin/rm -f fk/N4.V48A.00.cus.glib
/bin/rm -f fk/N4.V53A.00.cus.glib
/bin/rm -f fk/N4.W50A.00.cus.glib
/bin/rm -f fk/N4.W52A.00.cus.glib
/bin/rm -f fk/N4.X51A.00.cus.glib
/bin/rm -f fk/US.TZTN.00.cus.glib
/bin/rm -f fk/cus.mod
/bin/rm -f fk/cus.par
/bin/rm -f fk/mkgrnlib.par
/bin/rm -f fk/multithread_mkgrnlib.out
/bin/rm -f fk/rdseed.stations

### sw4 directory
###
/bin/rm -rf sw4/cus.sw4output.???
/bin/rm -f  sw4/*.glib
/bin/rm -f  sw4/*.grns
/bin/rm -f  sw4/cus.mod
/bin/rm -f  sw4/rdseed.stations
/bin/rm -f  sw4/sw4_to_grnlib.par

### curent directory
###
/bin/rm -f *.jpg 
/bin/rm -f gmt.* 
/bin/rm -f plotgrn.csh
/bin/rm -f *.grn
/bin/rm -f compare_grn_gmt.par
