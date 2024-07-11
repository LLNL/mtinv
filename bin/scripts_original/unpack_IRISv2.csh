#!/bin/csh

if( $#argv == 1 ) then
	set tar_file=$1
	set tar_directory=(`echo $tar_file | cut -d. -f1` )

        echo " unpacking tar file ${tar_file} into director ${tar_directory} "
else
        echo "$0 needs 1 argument"
        echo " Usage : $0 tar_file "
        exit
endif

### untar the iris download 
###
tar xvf ${tar_file}

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
cat >! setupMT.csh << EOF
#!/bin/csh
#setupMT -z 10 -ev 38.4395 44.9455 -velocity_model wus -ot "2023-02-17T00:00:00" -com "Put Comment Here" -gmt5 -realtime ../Data/*.??Z.?.SAC ../Data/*.??Z.SAC
#setupMT -z 2.0 -ev 35.876667 -84.898167 -velocity_model cus -ot "2021-08-13T11:57:35" -com "Franklin Mine, TN" -gmt5 -realtime ../Data/*Z.?.SAC
#setupMT -z 10.0 -ev -5.619 +35.221 -velocity_model safrica0 -ot "2023-02-17T09:26:09" -com "Tanzania" -gmt5 -realtime ../Data/*.??Z.?.SAC ../Data/*.??Z.SAC
#setupMT -z 16 -ev 36.907667 -90.543 -velocity_model cus -ot "2021-11-18T02:53:03" -com "New Madrid, Missouri" -gmt5 -realtime ../Data/*.??Z.?.SAC ../Data/*.??Z.SAC
#
EOF
chmod a+x setupMT.csh
cd ..
