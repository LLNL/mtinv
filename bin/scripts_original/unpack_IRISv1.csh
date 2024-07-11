#!/bin/csh

if( $#argv != 1 ) then
	echo "$0 needs 1 arguments"
	exit
else
  set seedfile=$argv[1]
endif

set RDSEED=/Users/ichinose1/bin/rdseed
which rdseed
echo $RDSEED

## run ${RDSEED} to unpack station file
##
${RDSEED} -p -S -f ${seedfile}

## run rdseed to unpack event file (if available, typically included in WILBURv3 downloads)
${RDSEED} -e -f ${seedfile}

## run rdseed to unpack SAC files
##
${RDSEED} -d -o 1 -f ${seedfile}

## move data files into ./Data and response files into ./Resp
##
mkdir Data
mv rdseed.stations *.SAC Data

mkdir Resp
mv SAC_PZs* Resp

mkdir dev full

cat >! dev/setup.csh << EOF
#!/bin/csh
#
# For help type: > setupMT -help 
# Rdseed find event information by typing: > cat ../rdseed.events
#
 
setupMT -depth 10 -comment "comment" -origintime "2004/12/16,18:59:12.0" -gmt5 ../Data/*BHZ*SAC
EOF

chmod a+x dev/setup.csh
