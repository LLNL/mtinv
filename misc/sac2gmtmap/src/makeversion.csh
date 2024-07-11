#!/bin/csh
set VERSION=( "1.0" )
set TIMESTAMP=( `/bin/date` )
set SRCDIR=( `/bin/pwd` )
echo 'static char  Version_Label[]={"'"${VERSION}"'"};'   >! version.h
echo 'static char   Version_Date[]={"'"${TIMESTAMP}"'"};' >> version.h
echo 'static char Version_SrcDir[]={"'"${SRCDIR}"'"};'    >> version.h

cat ./version.h
