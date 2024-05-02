ifeq ($(OS),Windows_NT)
	CFLAGS=-Wall -Werror -O3 -fopenmp -I/c/OSGeo4W/include
	GDALLIBS=/c/OSGeo4W/lib/gdal_i.lib
	EXT=.exe
else
	CFLAGS=-Wall -Werror -O3 -fopenmp
	GDALLIBS=`gdal-config --libs`
	EXT=
endif
LDFLAGS=-O3 -fopenmp -lm

all: mefa$(EXT)

clean:
	$(RM) *.o

mefa$(EXT): \
	main.o \
	timeval_diff.o \
	raster.o \
	accumulate.o \
	accumulate_lessmem.o \
	accumulate_moremem.o
	$(CC) $(LDFLAGS) -o $@ $^ $(GDALLIBS)

*.o: global.h raster.h
accumulate_*.o: accumulate_funcs.h
