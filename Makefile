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
	accumulate_lessmem.o \
	accumulate_moremem.o
	$(CC) $(LDFLAGS) -o $@ $^ $(GDAL_LIBS)

*.o: global.h raster.h
accumulate_*.o: accumulate_funcs.h
