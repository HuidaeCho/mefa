ifeq ($(OS),Windows_NT)
	GDAL_CFLAGS=-I/c/OSGeo4W/include
	GDAL_LIBS=/c/OSGeo4W/lib/gdal_i.lib
	EXT=.exe
else
	GDAL_LIBS=`gdal-config --libs`
endif
CFLAGS=-Wall -Werror -O3 -fopenmp $(GDAL_CFLAGS)
LDFLAGS=-O3 -fopenmp -lm

all: mefa$(EXT)

clean:
	$(RM) *.o

mefa$(EXT): \
	main.o \
	timeval_diff.o \
	raster.o \
	recode.o \
	accumulate.o \
	accumulate_b.o \
	accumulate_bm.o \
	accumulate_bw.o \
	accumulate_bmw.o \
	accumulate_i.o \
	accumulate_im.o \
	accumulate_iw.o \
	accumulate_imw.o \
	accumulate_u.o \
	accumulate_um.o \
	accumulate_uw.o \
	accumulate_umw.o \
	accumulate_j.o \
	accumulate_jm.o \
	accumulate_jw.o \
	accumulate_jmw.o \
	accumulate_v.o \
	accumulate_vm.o \
	accumulate_vw.o \
	accumulate_vmw.o \
	accumulate_f.o \
	accumulate_fm.o \
	accumulate_fw.o \
	accumulate_fmw.o \
	accumulate_d.o \
	accumulate_dm.o \
	accumulate_dw.o \
	accumulate_dmw.o
	$(CC) $(LDFLAGS) -o $@ $^ $(GDAL_LIBS)

*.o: global.h raster.h
accumulate*.o: accumulate_funcs.h
