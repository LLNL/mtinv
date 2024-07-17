#!/bin/csh

if( $#argv == 1 ) then
	set tar_file=$1

	## when using just filename no path
	## set tar_directory=(`echo $tar_file | cut -d. -f1` )

	## when using full or relative paths
        set tar_directory=(`echo $tar_file | awk -F/ '{print $NF}' | cut -f1 -d. `)
        echo " unpacking tar file ${tar_file} into directory ${tar_directory} "
else
        echo "$0 needs 1 argument"
        echo " Usage : $0 tar_file "
        exit
endif

### untar the iris download 
###
if( -e ${tar_file} ) then
  tar xvf ${tar_file} -C .
else
  echo "$0 ERROR! IRIS download tar file does NOT exist in current directory"
  exit(-1)
endif

### rename the SAC files and move them to the ../Data directory
###
cd ${tar_directory}
renamesac *.SAC
mkdir ../Data
mv 1*.SAC 2*.SAC ../Data
/bin/ls
/bin/rm -f *.SAC

### rename and move the responses
###
foreach i ( * )
cd $i
 rename_SACPZs.csh
cd ..
end

mkdir ../Resp
mv */SAC_PZs_* ../Resp
/bin/rm -r *

cd ..
/bin/rm -rf ${tar_directory}

### create rdseed.station file and make a GMT map from SAC files
###
cd ./Data
sac2gmtmap -gmt4 -include-epicenter -include-sta-label -rdseed.station *.??Z.?.SAC *.??Z.SAC
cd ..

mkdir dev
cd dev

cat >! setupMT.csh << EOF_SETUPMT.CSH
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

EOF_SETUPMT.CSH

chmod a+x setupMT.csh

cd ..
