#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#ifdef _MSC_VER
#include <winsock2.h>
/* gettimeofday.c */
int gettimeofday(struct timeval *, struct timezone *);
#else
#include <sys/time.h>
#endif

#define NE 128
#define N 64
#define NW 32
#define W 16
#define SW 8
#define S 4
#define SE 2
#define E 1

#define RASTER_MAP_TYPE_BYTE 0
#define RASTER_MAP_TYPE_UINT32 1
#define RASTER_MAP_TYPE_INT32 2

struct raster_map
{
    int type;
    int nrows, ncols;
    union
    {
        void *v;
        unsigned char *byte;
        unsigned int *uint32;
        int *int32;
    } cells;
    double null_value;
    char *projection;
    double geotransform[6];
    int compress;
};

/* timeval_diff.c */
long long timeval_diff(struct timeval *, struct timeval *, struct timeval *);

/* raster.c */
struct raster_map *init_raster(int, int, int);
void free_raster(struct raster_map *);
void copy_raster_metadata(struct raster_map *, const struct raster_map *);
struct raster_map *read_raster(const char *, int);
int write_raster(const char *, struct raster_map *);

/* accumulate.c */
void accumulate(struct raster_map *, struct raster_map *);

#endif
