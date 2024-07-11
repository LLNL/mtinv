#
# Top Level Makefile for mtinv package
#

all :
	for dir in misc/Cgraph misc/getpar; do cd $$dir ; make all ; cd ../../ ; done
	for dir in src ; do cd $$dir ; make all ; cd .. ; done
	for dir in misc/multithread_makeglib ; do cd $$dir ; make all ; cd ../../ ; done
	for dir in misc/renamesac misc/sac2xy misc/sacmerge ; do cd $$dir ; make all ; cd ../../ ; done
	for dir in misc/sac2gmtmap ; do cd $$dir ; make all ; cd ../../ ; done
	for dir in misc/Flinn_Engdahl ; do cd $$dir ; make all ; cd ../../ ; done

clean :
	/bin/rm -f lib/*.a
	for dir in misc/Cgraph misc/getpar; do cd $$dir ; make clean ; cd ../.. ; done
	for dir in src ; do cd $$dir ; make clean ; cd .. ; done
	for dir in misc/multithread_makeglib ; do cd $$dir ; make clean ; cd ../../ ; done
	for dir in misc/renamesac misc/sac2xy misc/sacmerge ; do cd $$dir ; make clean ; cd ../../ ; done
	for dir in misc/sac2gmtmap ; do cd $$dir ; make clean ; cd ../../ ; done
	for dir in misc/Flinn_Engdahl ; do cd $$dir ; make clean ; cd ../../ ; done
