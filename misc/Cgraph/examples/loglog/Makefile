ROOT=TFopt-lrLog

# Define USE_STDOUT to direct output to stdout for this sample
USE_STDOUT=
#USE_STDOUT=-DUSE_STDOUT

CC=cc
#CC=gcc
CFLAGS=-Wall

INCLUDE_DIR=/usr/local/include
LIB_DIR=/usr/local/lib

#LIB_FILE=libcgraph.sl
LIB_FILE=libcgraph.a

#LIBS=-lcgraph -lm
LIBS=-lcgraph

#LD_FLAGS=-s
LD_FLAGS=-s -object

# NEXTSTEP/OPENSTEP multiple architecture compile flags
#ARCHS=
#ARCHS=-arch i386 -arch m68k -arch sparc -arch hppa
ARCHS=-arch i386 -arch m68k



$(ROOT): $(ROOT).o $(LIB_DIR)/$(LIB_FILE)
	$(CC) $(LD_FLAGS) $(ARCHS) -o $(ROOT) $(ROOT).o -L$(LIB_DIR) $(LIBS)

$(ROOT).o:
	$(CC) $(CFLAGS) $(ARCHS) -c -I $(INCLUDE_DIR) $(USE_STDOUT) $(ROOT).c

clean:
	rm -f $(ROOT).o $(ROOT)
