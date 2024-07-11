#!/bin/csh

cat /dev/null >! taup.out
echo ">  " >! taup.out
foreach dist ( `awk '{ print $6 }' run.out ` )
set ttime = ( `taup_time -km ${dist} -h 2.0 -mod ak135 -ph P,Pn,p,Pg -time ` )
 echo $dist $ttime[1] >> taup.out
end
