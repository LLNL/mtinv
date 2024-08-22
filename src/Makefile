include ../Makefile.inc

### set these in include file ../Makefile.inc
# CC = gcc
# CC = clang
# FC = gfortran
####

INCLUDE = -I../include

GRN_LIBS = -L../lib -lglib -lget -lm

#PTHREAD_LIBS = -lpthread
PTHREAD_LIBS = -pthread

PLOT_LIBS = -L../lib -lcgraph
INSTALL = ../bin
CFLAGS = $(INCLUDE)
FFLAGS = 

.c.o :
	$(CC) $(CFLAGS) -c $< -o $@
.f.o : 
	$(FC) $(FFLAGS) -c $< -o $@

EXEC = libglib mkgrnlib grnlib2sac glib2inv sacdata2inv mtinv makepar \
       setupMT pltmod mteig whatshere mtbestfit grn2Mxy \
         hspec96_to_grnlib compare_grn_gmt sw4_to_grnlib

# removed for distribution: mt2lune lune2mt, go back to older versions to find these

all : $(EXEC)

### now in sacio.o

OBJS0 = rtrend/rtrend.o \
	rmean.o \
	transfer/transfersubs.o transfer/fftsub.o \
	interpolate/interpolate_subs.o \
	filter/filtersubs.o filter/complex.o \
	saccut/saccut_subs.o \
	source/source_subs.o \
	timesubs.o \
	modsubs.o \
	misc_tools/distaz.o \
	misc_tools/ampshift.o \
	misc_tools/scale_data.o \
	taper/taper_subs.o \
	rotate/rotate.o \
	math/math.o \
	crosscorrelation/cross_correlation.o \
	tdif/Differentiates.o \
	shift/shift.o \
	random/ransubs.o \
	sacio/sacio.o \
	writesacfile.o \
        load_special_grns.o

libglib : $(OBJS0)
	ar -rv libglib.a $(OBJS0)
	mv libglib.a ../lib
	ranlib ../lib/libglib.a

OBJS1 = mkgrnlib.o Greens_subs.o stasubs.o greensf.o besasy.o bessel.o cinv2.o fork.o \
	genrefl.o timesubs.o modsubs.o misc_tools/distaz.o rayp/rayp_subs.o shorten_path.o \
        misc_tools/ampshift.o sacio/sacio.o mkdirp2.o

mkgrnlib : $(OBJS1)
	$(FC) $(FFLAGS) -o $@ $(OBJS1) -L../lib -lget -lm
	mv $@ $(INSTALL)

OBJS2 = grnlib2sac.o Greens_subs.o radiation_pattern_subs.o mkdirp2.o
grnlib2sac : $(OBJS2)
	$(CC) $(CFLAGS) -o $@ $(OBJS2) $(GRN_LIBS)
	mv $@ $(INSTALL)

grn2Mxy : grn2Mxy.o timesubs.o mkdirp2.o sacio/sacio.o
	$(CC) $(CFLAGS) -o $@ grn2Mxy.o timesubs.o mkdirp2.o sacio/sacio.o $(GRN_LIBS)
	mv $@ $(INSTALL)

OBJS3 = sacdata2inv.o sacdata2inv_subs.o glib2inv_subs.o getrespfile_sub.o compute_Peak_to_Peak.o \
         shorten_path.o sacdata2inv_serial.o sacdata2inv_parallel.o sacdata2inv_loaddata.o

sacdata2inv : $(OBJS3) libglib
	$(CC) $(CFLAGS) -pthread -o $@ $(OBJS3) $(GRN_LIBS) $(PTHREAD_LIBS)
	mv $@ $(INSTALL)

OBJS4 = glib2inv.o glib2inv_subs.o glib2inv_serial.o glib2inv_parallel.o Greens_subs.o shorten_path.o mkdirp2.o
glib2inv : $(OBJS4)
	$(CC) $(CFLAGS) -pthread -o $@ $(OBJS4) $(GRN_LIBS) $(PTHREAD_LIBS)
	mv $@ $(INSTALL)

MTINV_SUBS= mtinv.o glib2inv_subs.o svbksb.o svdcmp.o tred2.o tqli.o trans1.o tpdss.o Greens_subs.o mkdirp2.o \
	mtinv_subs.o mtinv_gmtsubs.o ascii_mech.o psplot.o svdvar.o psmech.o make_amatrix.o indexx.o \
        check_depths.o dbsubs.o eig2lune.o shorten_path.o find_best_shift.o realtime_subs.o dumpSAC.o

mtinv : $(MTINV_SUBS) libglib
	$(CC) $(CFLAGS)  -o $@ $(MTINV_SUBS) $(GRN_LIBS) $(PLOT_LIBS)
	mv $@ $(INSTALL)

MTEIG_SUBS = mteig_subs.o simulate_MTeig2evec_serial.o simulate_MT_parallel.o simulate_MT_lite.o \
        forward_calc_from_MT.o glib2inv_subs.o tred2.o tqli.o trans1.o tpdss.o mteig_gmt.o Greens_subs.o mkdirp2.o \
        mtinv_subs.o mtinv_gmtsubs.o ascii_mech.o psplot.o psmech.o make_amatrix.o indexx.o bessel/bessel_subs.o \
        check_depths.o dbsubs.o eig2lune.o shorten_path.o find_best_shift.o realtime_subs.o dumpSAC.o misc_tools/distaz.o

mteig : mteig.o $(MTEIG_SUBS) libglib
	$(CC) $(CFLAGS) -pthread -o $@ mteig.o $(MTEIG_SUBS) $(GRN_LIBS) $(PLOT_LIBS) $(PTHREAD_LIBS)
	mv $@ $(INSTALL)
	
SETUPMT_OBJS = setupMT.o indexx.o shorten_path.o sacio/sacio.o timesubs.o misc_tools/distaz.o mkdirp2.o modsubs.o
setupMT : $(SETUPMT_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SETUPMT_OBJS) $(GRN_LIBS)
	mv $@ $(INSTALL)

makepar : makepar.o indexx.o shorten_path.o
	$(CC) $(CFLAGS) -o $@ makepar.o indexx.o $(GRN_LIBS)
	mv $@ $(INSTALL)

pltmod : pltmod.o modsubs.o
	$(CC) $(CFLAGS) -o $@ pltmod.o modsubs.o $(GRN_LIBS)
	mv $@ $(INSTALL)

whatshere : whatshere.o timesubs.o
	$(CC) $(CFLAGS) -o $@ whatshere.o timesubs.o $(GRN_LIBS)
	mv $@ $(INSTALL)

mtbestfit : mtbestfit.o timesubs.o 
	$(CC) $(CFLAGS) -o $@ mtbestfit.o timesubs.o $(GRN_LIBS)
	mv $@ $(INSTALL)

OBJS_HSPEC96 = timesubs.o modsubs.o shorten_path.o sacio/sacio.o mkdirp2.o misc_tools/distaz.o stasubs.o

hspec96_to_grnlib : hspec96_to_grnlib.o $(OBJS_HSPEC96)
	$(CC) $(CFLAGS) -o $@ hspec96_to_grnlib.o $(OBJS_HSPEC96) $(GRN_LIBS)
	mv $@ $(INSTALL)

OBJS_SW4_TO_GRNLIB = timesubs.o modsubs.o shorten_path.o sacio/sacio.o mkdirp2.o misc_tools/distaz.o stasubs.o Integrate.o rotate/rotate.o

sw4_to_grnlib : sw4_to_grnlib.o $(OBJS_SW4_TO_GRNLIB)
	$(CC) $(CFLAGS) -o $@ sw4_to_grnlib.o $(OBJS_SW4_TO_GRNLIB) $(GRN_LIBS)
	mv $@ $(INSTALL)

compare_grn_gmt : compare_grn_gmt.o Greens_subs.o mkdirp2.o check_gmt_version.o
	$(CC) $(CFLAGS) -o $@ compare_grn_gmt.o Greens_subs.o mkdirp2.o check_gmt_version.o $(GRN_LIBS)
	mv $@ $(INSTALL)

clean :
	rm -f *.o */*.o
