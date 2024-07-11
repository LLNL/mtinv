#!/bin/csh

cat >! FlinnEngdahl.in << EOF
  100.5 80    1
 -120.1 40.1  2
   53.1 34.3  3
  -80.9 28.5  4
EOF
FlinnEngdahl < FlinnEngdahl.in

java FlinnEngdahlTest 100.5 80
java FlinnEngdahlTest -120.1 40.1
java FlinnEngdahlTest 53.1 34.3
java FlinnEngdahlTest -80.9 28.5

perl feregion.pl 100.5 80
perl feregion.pl -120.1 40.1
perl feregion.pl 53.1 34.3
perl feregion.pl -80.9 28.5
