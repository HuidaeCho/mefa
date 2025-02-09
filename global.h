#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#ifdef _MSC_VER
#include <winsock2.h>
/* gettimeofday.c */
int gettimeofday(struct timeval *, struct timezone *);
#else
#include <sys/time.h>
#endif
#include "raster.h"

#define NE 128
#define N 64
#define NW 32
#define W 16
#define SW 8
#define S 4
#define SE 2
#define E 1

/* timeval_diff.c */
long long timeval_diff(struct timeval *, struct timeval *, struct timeval *);

/* recode.c */
double recode_encoding(double, void *);
double recode_degree(double, void *);

/* accumulate.c */
void accumulate(struct raster_map *, struct raster_map *, int);

/* accumulate_lessmem.c */
void accumulate_lessmem(struct raster_map *, struct raster_map *);

/* accumulate_moremem.c */
void accumulate_moremem(struct raster_map *, struct raster_map *);

#endif
