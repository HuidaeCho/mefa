CFLAGS=-Wall -Werror -O3 -fopenmp
LDFLAGS=-O3 -fopenmp

all: meffa

clean:
	$(RM) *.o

meffa: main.o raster.o accumulate.o timeval_diff.o
	$(CC) $(LDFLAGS) -o $@ $^ `gdal-config --libs`

%.o: %.c
	$(CC) $(CFLAGS) -c $<
