#!/bin/csh

mkdir ../Data ../Resp
/bin/rm -f ../Data/rdseed.stations

foreach net ( `/bin/ls *.mseed | cut -f2 -d. | sort -u ` )
#echo $net

 foreach sta ( `/bin/ls *.${net}.mseed | cut -f1 -d. | sort -u ` )
##echo $sta $net

  setenv ALT_RESPONSE_FILE IRISDMC-${sta}.${net}.dataless

  rdseed -f ${sta}.${net}.mseed -S
  rdseed -f ${sta}.${net}.mseed -d
  rdseed -f ${sta}.${net}.mseed -p

  cat rdseed.stations >> ../Data/rdseed.stations

  mv *.${net}.${sta}.*.SAC ../Data

  mv SAC_PZs_${net}_${sta}_* ../Resp

end  # sta
end  # net
