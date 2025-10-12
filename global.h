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
void accumulate(struct raster_map *, struct raster_map *, struct raster_map *,
                int);

/* accumulate_b.c */
void accumulate_b(struct raster_map *, struct raster_map *);

/* accumulate_bm.c */
void accumulate_bm(struct raster_map *, struct raster_map *);

/* accumulate_bw.c */
void accumulate_bw(struct raster_map *, struct raster_map *,
                   struct raster_map *);

/* accumulate_bmw.c */
void accumulate_bmw(struct raster_map *, struct raster_map *,
                    struct raster_map *);

/* accumulate_i.c */
void accumulate_i(struct raster_map *, struct raster_map *);

/* accumulate_im.c */
void accumulate_im(struct raster_map *, struct raster_map *);

/* accumulate_iw.c */
void accumulate_iw(struct raster_map *, struct raster_map *,
                   struct raster_map *);

/* accumulate_imw.c */
void accumulate_imw(struct raster_map *, struct raster_map *,
                    struct raster_map *);

/* accumulate_u.c */
void accumulate_u(struct raster_map *, struct raster_map *);

/* accumulate_um.c */
void accumulate_um(struct raster_map *, struct raster_map *);

/* accumulate_uw.c */
void accumulate_uw(struct raster_map *, struct raster_map *,
                   struct raster_map *);

/* accumulate_umw.c */
void accumulate_umw(struct raster_map *, struct raster_map *,
                    struct raster_map *);

/* accumulate_j.c */
void accumulate_j(struct raster_map *, struct raster_map *);

/* accumulate_jm.c */
void accumulate_jm(struct raster_map *, struct raster_map *);

/* accumulate_jw.c */
void accumulate_jw(struct raster_map *, struct raster_map *,
                   struct raster_map *);

/* accumulate_jmw.c */
void accumulate_jmw(struct raster_map *, struct raster_map *,
                    struct raster_map *);

/* accumulate_v.c */
void accumulate_v(struct raster_map *, struct raster_map *);

/* accumulate_vm.c */
void accumulate_vm(struct raster_map *, struct raster_map *);

/* accumulate_vw.c */
void accumulate_vw(struct raster_map *, struct raster_map *,
                   struct raster_map *);

/* accumulate_vmw.c */
void accumulate_vmw(struct raster_map *, struct raster_map *,
                    struct raster_map *);

/* accumulate_f.c */
void accumulate_f(struct raster_map *, struct raster_map *);

/* accumulate_fm.c */
void accumulate_fm(struct raster_map *, struct raster_map *);

/* accumulate_fw.c */
void accumulate_fw(struct raster_map *, struct raster_map *,
                   struct raster_map *);

/* accumulate_fmw.c */
void accumulate_fmw(struct raster_map *, struct raster_map *,
                    struct raster_map *);

/* accumulate_d.c */
void accumulate_d(struct raster_map *, struct raster_map *);

/* accumulate_dm.c */
void accumulate_dm(struct raster_map *, struct raster_map *);

/* accumulate_dw.c */
void accumulate_dw(struct raster_map *, struct raster_map *,
                   struct raster_map *);

/* accumulate_dmw.c */
void accumulate_dmw(struct raster_map *, struct raster_map *,
                    struct raster_map *);

#endif
