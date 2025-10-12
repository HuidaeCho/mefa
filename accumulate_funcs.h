#include <stdlib.h>
#include "global.h"

#define INDEX(row, col) ((size_t)(row) * ncols + (col))
#define DIR_NULL dir_map->null_value
#define DIR(row, col) dir_map->cells.byte[INDEX(row, col)]

#if ACCUM_RAST_TYPE == RASTER_MAP_TYPE_BYTE
#define ACCUMULATE(m, w) accumulate_b##m##w
#define ACCUM_MAP_CELLS  accum_map->cells.byte
#define ACCUM_TYPE       unsigned char
#elif ACCUM_RAST_TYPE == RASTER_MAP_TYPE_INT16
#define ACCUMULATE(m, w) accumulate_i##m##w
#define ACCUM_MAP_CELLS  accum_map->cells.int16
#define ACCUM_TYPE       short
#elif ACCUM_RAST_TYPE == RASTER_MAP_TYPE_UINT16
#define ACCUMULATE(m, w) accumulate_u##m##w
#define ACCUM_MAP_CELLS  accum_map->cells.uint16
#define ACCUM_TYPE       unsigned short
#elif ACCUM_RAST_TYPE == RASTER_MAP_TYPE_INT32
#define ACCUMULATE(m, w) accumulate_j##m##w
#define ACCUM_MAP_CELLS  accum_map->cells.int32
#define ACCUM_TYPE       int
#elif ACCUM_RAST_TYPE == RASTER_MAP_TYPE_UINT32
#define ACCUMULATE(m, w) accumulate_v##m##w
#define ACCUM_MAP_CELLS  accum_map->cells.uint32
#define ACCUM_TYPE       unsigned int
#elif ACCUM_RAST_TYPE == RASTER_MAP_TYPE_FLOAT32
#define ACCUMULATE(m, w) accumulate_f##m##w
#define ACCUM_MAP_CELLS  accum_map->cells.float32
#define ACCUM_TYPE       float
#else
#define ACCUMULATE(m, w) accumulate_d##m##w
#define ACCUM_MAP_CELLS  accum_map->cells.float64
#define ACCUM_TYPE       double
#endif

#define ACCUM(row, col) ACCUM_MAP_CELLS[INDEX(row, col)]

#ifdef USE_WEIGHT
#define WEIGHT(row, col) \
        ((ACCUM_TYPE)(weight_map->type == RASTER_MAP_TYPE_BYTE \
         ? weight_map->cells.byte[INDEX(row, col)] \
         : (weight_map->type == RASTER_MAP_TYPE_INT16 \
            ? weight_map->cells.int16[INDEX(row, col)] \
            : (weight_map->type == RASTER_MAP_TYPE_UINT16 \
               ? weight_map->cells.uint16[INDEX(row, col)] \
               : (weight_map->type == RASTER_MAP_TYPE_INT32 \
                  ? weight_map->cells.int32[INDEX(row, col)] \
                  : (weight_map->type == RASTER_MAP_TYPE_UINT32 \
                     ? weight_map->cells.uint32[INDEX(row, col)] \
                     : (weight_map->type == RASTER_MAP_TYPE_FLOAT32 \
                        ? weight_map->cells.float32[INDEX(row, col)] \
                        : weight_map->cells.float64[INDEX(row, col)])))))))
#define IS_WEIGHT_NULL(row, col) is_null(weight_map, row, col)
#define FIND_UP(row, col) ( \
	(row > 0 ? \
	 (col > 0 && DIR(row - 1, col - 1) == SE && \
	  !IS_WEIGHT_NULL(row - 1, col - 1) ? NW : 0) | \
	 (DIR(row - 1, col) == S && !IS_WEIGHT_NULL(row - 1, col) ? N : 0) | \
	 (col < ncols - 1 && DIR(row - 1, col + 1) == SW && \
	  !IS_WEIGHT_NULL(row - 1, col + 1) ? NE : 0) : 0) | \
	(col > 0 && DIR(row, col - 1) == E && \
	 !IS_WEIGHT_NULL(row, col - 1) ? W : 0) | \
	(col < ncols - 1 && DIR(row, col + 1) == W && \
	 !IS_WEIGHT_NULL(row, col + 1) ? E : 0) | \
	(row < nrows - 1 ? \
	 (col > 0 && DIR(row + 1, col - 1) == NE && \
	  !IS_WEIGHT_NULL(row + 1, col - 1) ? SW : 0) | \
	 (DIR(row + 1, col) == N && !IS_WEIGHT_NULL(row + 1, col) ? S : 0) | \
	 (col < ncols - 1 && DIR(row + 1, col + 1) == NW && \
	  !IS_WEIGHT_NULL(row + 1, col + 1) ? SE : 0) : 0))
#else
#define WEIGHT(row, col) 1
#define FIND_UP(row, col) ( \
        (row > 0 ? \
         (col > 0 && DIR(row - 1, col - 1) == SE ? NW : 0) | \
         (DIR(row - 1, col) == S ? N : 0) | \
         (col < ncols - 1 && DIR(row - 1, col + 1) == SW ? NE : 0) : 0) | \
        (col > 0 && DIR(row, col - 1) == E ? W : 0) | \
        (col < ncols - 1 && DIR(row, col + 1) == W ? E : 0) | \
        (row < nrows - 1 ? \
         (col > 0 && DIR(row + 1, col - 1) == NE ? SW : 0) | \
         (DIR(row + 1, col) == N ? S : 0) | \
         (col < ncols - 1 && DIR(row + 1, col + 1) == NW ? SE : 0) : 0))
#endif

#ifdef USE_LESS_MEMORY
#define UP(row, col) FIND_UP(row, col)
#else
#define UP(row, col) up_cells[INDEX(row, col)]
static unsigned char *up_cells;
#endif

static int nrows, ncols;

static void trace_down(struct raster_map *,
#ifdef USE_WEIGHT
                       struct raster_map *,
#endif
                       struct raster_map *, int, int, ACCUM_TYPE);
static ACCUM_TYPE sum_up(struct raster_map *, int, int, int);

void ACCUMULATE(
#ifdef USE_LESS_MEMORY
                   m
#endif
                   ,
#ifdef USE_WEIGHT
                   w
#endif
) (struct raster_map * dir_map,
#ifdef USE_WEIGHT
   struct raster_map * weight_map,
#endif
   struct raster_map * accum_map)
{
    int row, col;

    nrows = dir_map->nrows;
    ncols = dir_map->ncols;

#ifndef USE_LESS_MEMORY
    up_cells = calloc((size_t)nrows * ncols, sizeof *up_cells);

#pragma omp parallel for schedule(dynamic) private(col)
    for (row = 0; row < nrows; row++) {
        for (col = 0; col < ncols; col++)
            if (DIR(row, col) != DIR_NULL)
                UP(row, col) = FIND_UP(row, col);
    }
#endif

#pragma omp parallel for schedule(dynamic) private(col)
    for (row = 0; row < nrows; row++) {
        for (col = 0; col < ncols; col++)
            /* if the current cell is not null and has no upstream cells, start
             * tracing down */
            if (DIR(row, col) != DIR_NULL && !UP(row, col)
#ifdef USE_WEIGHT
                && !IS_WEIGHT_NULL(row, col)
#endif
                )
                trace_down(dir_map,
#ifdef USE_WEIGHT
                           weight_map,
#endif
                           accum_map, row, col, WEIGHT(row, col));
    }

#ifndef USE_LESS_MEMORY
    free(up_cells);
#endif
}

static void trace_down(struct raster_map *dir_map,
#ifdef USE_WEIGHT
                       struct raster_map *weight_map,
#endif
                       struct raster_map *accum_map, int row, int col,
                       ACCUM_TYPE accum)
{
#ifdef DONT_USE_TCO
    do {
#endif
        int up;
        ACCUM_TYPE accum_up = 0;

        /* accumulate the current cell itself */
        ACCUM(row, col) = accum;

        /* find the downstream cell */
        switch (DIR(row, col)) {
        case NW:
            row--;
            col--;
            break;
        case N:
            row--;
            break;
        case NE:
            row--;
            col++;
            break;
        case W:
            col--;
            break;
        case E:
            col++;
            break;
        case SW:
            row++;
            col--;
            break;
        case S:
            row++;
            break;
        case SE:
            row++;
            col++;
            break;
        }

        /* if the downstream cell is null or any upstream cells of the
         * downstream cell have never been visited, stop tracing down */
        if (row < 0 || row >= nrows || col < 0 || col >= ncols ||
            DIR(row, col) == DIR_NULL || !(up = UP(row, col)) ||
            !(accum_up = sum_up(accum_map, row, col, up)))
            return;

#ifdef USE_WEIGHT
        if (IS_WEIGHT_NULL(row, col))
            return;
#endif

#ifdef DONT_USE_TCO
        accum = accum_up + WEIGHT(row, col);
    } while (1);
    /* XXX: work around an indent bug
     * #else
     * doesn't work */
#endif
#ifndef DONT_USE_TCO
    /* use gcc -O2 or -O3 flags for tail-call optimization
     * (-foptimize-sibling-calls) */
    trace_down(dir_map,
#ifdef USE_WEIGHT
               weight_map,
#endif
               accum_map, row, col, accum_up + WEIGHT(row, col));
#endif
}

/* if any upstream cells have never been visited, 0 is returned; otherwise, the
 * sum of upstream accumulation is returned */
static ACCUM_TYPE sum_up(struct raster_map *accum_map, int row, int col,
                         int up)
{
    ACCUM_TYPE sum = 0, accum;

#pragma omp flush(accum_map)
    if (up & NW) {
        if (!(accum = ACCUM(row - 1, col - 1)))
            return 0;
        sum += accum;
    }
    if (up & N) {
        if (!(accum = ACCUM(row - 1, col)))
            return 0;
        sum += accum;
    }
    if (up & NE) {
        if (!(accum = ACCUM(row - 1, col + 1)))
            return 0;
        sum += accum;
    }
    if (up & W) {
        if (!(accum = ACCUM(row, col - 1)))
            return 0;
        sum += accum;
    }
    if (up & E) {
        if (!(accum = ACCUM(row, col + 1)))
            return 0;
        sum += accum;
    }
    if (up & SW) {
        if (!(accum = ACCUM(row + 1, col - 1)))
            return 0;
        sum += accum;
    }
    if (up & S) {
        if (!(accum = ACCUM(row + 1, col)))
            return 0;
        sum += accum;
    }
    if (up & SE) {
        if (!(accum = ACCUM(row + 1, col + 1)))
            return 0;
        sum += accum;
    }

    return sum;
}
