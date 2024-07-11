# Makefile for multi-architecture Cgraph library

include ../../../Makefile.inc

INCLUDE_DIR=../../../include
LIB_DIR=../../../lib
#
#INCLUDE_DIR=/usr/local/include
#LIB_DIR=/usr/local/lib

#LIB_FILE=libcgraph.sl
LIB_FILE=libcgraph.a


# NEXTSTEP/OPENSTEP multiple architecture compile flags
ARCHS=
#ARCHS=-arch i386 -arch m68k -arch sparc -arch hppa
#ARCHS=-arch i386 -arch m68k
#
#CCFLAGS =  -c -O -fpic -I/usr/include/X11R5
#CCFLAGS =  -Wall -O -c -g
#CCFLAGS = -O -c -m32
CCFLAGS = -O -c 

OBJS = bgdash.o bginit.o bgshow.o bgstart.o\
	glcmp.o gxaxs.o gxlog.o gyaxs.o gylog.o\
	gfont.o gcord.o gmlin.o gorig.o gprim.o\
	grmle.o grset.o gmark.o ggraycolor.o gstrk.o\
	gaxlabel.o gbott.o cgsave.o cgprint.o\
	gtext.o gtrnc.o gmesh.o gstyl.o gwdth.o\
	cg_newfuncs.o

all: $(LIB_FILE) install
	@echo All done, now do a make install

install:
#	mkdir -p $(LIB_DIR)
#	mkdir -p $(INCLUDE_DIR)
	cp $(LIB_FILE) $(LIB_DIR)
	ranlib $(LIB_DIR)/$(LIB_FILE)
	cp cgraph.h $(INCLUDE_DIR)

# For NeXT
$(LIB_FILE):  $(OBJS) Makefile
	rm -f $(LIB_FILE)
	ar r $(LIB_FILE) $(OBJS)
	ranlib $(LIB_FILE)

# For Unix
#$(LIB_FILE):  $(OBJS) Makefile
#	ld -b -o $(LIB_FILE) $(OBJS)


cg_newfuncs.o:  cglobals.h cgaxes.h cgfont.h
	$(CC) $(CCFLAGS) $(ARCHS) cg_newfuncs.c

cgprint.o:  cgprint.c cglobals.h
	$(CC) $(CCFLAGS) $(ARCHS) cgprint.c
	
cgsave.o:  cgsave.c cglobals.h
	$(CC) $(CCFLAGS) $(ARCHS) cgsave.c

gaxlabel.o:  gaxlabel.c cglobals.h cgaxes.h  
	$(CC) $(CCFLAGS) $(ARCHS) gaxlabel.c
	
gbott.o:  gbott.c    cglobals.h
	$(CC) $(CCFLAGS) $(ARCHS) gbott.c
	
bginit.o:  bginit.c cglobals.h cgaxes.h cgfont.h version.h cgraph.h
	$(CC) $(CCFLAGS) $(ARCHS) bginit.c
	
bgstart.o:  bgstart.c cglobals.h cgaxes.h cgfont.h version.h
	$(CC) $(CCFLAGS) $(ARCHS) bgstart.c	
	
gmark.o:   gmark.c cglobals.h  
	$(CC) $(CCFLAGS) $(ARCHS) gmark.c
	
glcmp.o:   glcmp.c cglobals.h 
	$(CC) $(CCFLAGS) $(ARCHS) glcmp.c
	
gxaxs.o:   gxaxs.c cglobals.h cgaxes.h cgfont.h  
	$(CC) $(CCFLAGS) $(ARCHS) gxaxs.c
	
gxlog.o:   gxlog.c cglobals.h cgaxes.h cgfont.h  
	$(CC) $(CCFLAGS) $(ARCHS) gxlog.c
	
gyaxs.o:   gyaxs.c cgaxes.h cgfont.h   cglobals.h
	$(CC) $(CCFLAGS) $(ARCHS) gyaxs.c
	
gylog.o:   gylog.c cglobals.h cgaxes.h  cgfont.h  
	$(CC) $(CCFLAGS) $(ARCHS) gylog.c
	
gtext.o:  gtext.c cglobals.h  
	$(CC) $(CCFLAGS) $(ARCHS) gtext.c
	
gtrnc.o:  gtrnc.c  cglobals.h
	$(CC) $(CCFLAGS) $(ARCHS) gtrnc.c
	
gmesh.o:  gmesh.c cglobals.h cgaxes.h  
	$(CC) $(CCFLAGS) $(ARCHS) gmesh.c
	
gstyl.o:  gstyl.c cgaxes.h cglobals.h
	$(CC) $(CCFLAGS) $(ARCHS) gstyl.c
	
gwdth.o:  gwdth.c  cglobals.h
	$(CC) $(CCFLAGS) $(ARCHS) gwdth.c
	
gfont.o:  gfont.c cglobals.h cgfont.h PS_font_names.h
	$(CC) $(CCFLAGS) $(ARCHS) gfont.c
	
grmle.o:  grmle.c cglobals.h  
	$(CC) $(CCFLAGS) $(ARCHS) grmle.c
	
gmlin.o:  gmlin.c cglobals.h  
	$(CC) $(CCFLAGS) $(ARCHS) gmlin.c
	
gcord.o:  gcord.c cglobals.h
	$(CC) $(CCFLAGS) $(ARCHS) gcord.c
	
gorig.o:  gorig.c cglobals.h
	$(CC) $(CCFLAGS) $(ARCHS) gorig.c 
	
	
bgdash.o:  bgdash.c cglobals.h
	$(CC) $(CCFLAGS) $(ARCHS) bgdash.c
	
gprim.o:  gprim.c cglobals.h  
	$(CC) $(CCFLAGS) $(ARCHS) gprim.c
	
grset.o:  grset.c cglobals.h
	$(CC) $(CCFLAGS) $(ARCHS) grset.c
	
ggraycolor.o:  ggraycolor.c cglobals.h
	$(CC) $(CCFLAGS) $(ARCHS) ggraycolor.c
	
gstrk.o:  gstrk.c cglobals.h
	$(CC) $(CCFLAGS) $(ARCHS) gstrk.c
	
bgshow.o:  bgshow.c cglobals.h
	$(CC) $(CCFLAGS) $(ARCHS) bgshow.c

clean:
	rm -f *.o *.a *.sl

cleannfs:
	rm -f .nfs*
