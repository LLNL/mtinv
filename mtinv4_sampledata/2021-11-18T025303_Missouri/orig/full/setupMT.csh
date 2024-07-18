#!/bin/csh

### exclude stations file
cat >! exclude.sta << EOF
# sta code(s), one per line
EOF

###
### Usage: setupMT -z [depth(km)] -ev [lat lon] -mod [model_basename] -ot ["YYYY-MM-DDTHH:mm:SS.SS"] 
###                -com ["Comment"] -fil [lo_f(Hz) hi_f(Hz)] -evid [eventID] -exclude_sta [file name] 
###                -gmt4 -interactive -help -verbose [sac files]
###

#setupMT --hspec96 -z  2 -ev 41.7392 +73.377 -mod wus -ot "2009-12-22T05:54:35" -com "Kambrata, Kyrgyzs" -fil 0.033 0.10 -DataDir ../Data -RespDir ../Resp ../Data/*Z.?.SAC ../Data/*Z.SAC
#setupMT --hspec96 -z  2 -ev 35.8767 -84.898 -mod cus -ot "2021-08-13T11:57:35" -com "Franklin Mine, TN" -fil 0.075 0.12 -DataDir ../Data -RespDir ../Resp ../Data/*Z.?.SAC ../Data/*Z.SAC
#setupMT --hspec96 -z 16 -ev 36.9077 -90.543 -mod cus -ot "2021-11-18T02:53:04" -com "New Madrid, MO"    -fil 0.075 0.15 -DataDir ../Data -RespDir ../Resp ../Data/*Z.?.SAC ../Data/*Z.SAC
#setupMT --hspec96 -z 10 -ev 34.5379 122.567 -mod mdj2 -ot 2023-04-25T15:34:29  -com "Yellow Sea"        -fil 0.02  0.05 -DataDir ../Data -RespDir ../Resp ../Data/*Z.?.SAC ../Data/*Z.SAC

