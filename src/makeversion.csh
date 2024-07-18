#!/bin/csh

### Version 4.0   - new major release will include support for 3C rotational Green's functions and data
### Version 4.0.1 - includes NSS mteig and lune plotting features, also major cleaning for release 
### Version 4.0.2 - tracking bugfixes in memory segfaults and new sql db features

set ts=(`date`)

#echo 'static char Version_Label[]={"3.0.6"}; ' >! ../include/mt_version.h
#echo 'static char Version_Label[]={"4.0.0"}; ' >! ../include/mt_version.h
#echo 'static char Version_Label[]={"4.0.1"}; ' >! ../include/mt_version.h
echo 'static char Version_Label[]={"4.0.2"}; ' >! ../include/mt_version.h

echo 'static char Version_Date[]={"\' >> ../include/mt_version.h
echo "${ts}\" >> ../include/mt_version.h
echo '" };' >> ../include/mt_version.h
