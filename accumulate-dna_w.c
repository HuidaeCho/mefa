#include "global.h"

#define DIR_NULL (unsigned char)dir_buf->null_value
#define DIR(row, col) dir_buf->cells.byte[(row) * ncols + (col)]
#define ACCUM(row, col) accum_buf->cells.uint32[(row) * ncols + (col)]
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
         (col < ncols-1 && DIR(row + 1, col + 1) == NW ? SE : 0) : 0))

#ifdef USE_LESS_MEMORY
#define UP(row, col) FIND_UP(row, col)
#else
#define UP(row, col) up_buf->cells.byte[(row) * ncols + (col)]
static struct raster_map *up_buf;
#endif

static int nrows, ncols;

static void trace_down(struct raster_map *, struct raster_map *, int, int);
static int sum_up(struct raster_map *, int, int, int);

void accumulate(struct raster_map *dir_buf, struct raster_map *accum_buf)
{
    int row, col;

    nrows = dir_buf->nrows;
    ncols = dir_buf->ncols;

#ifndef USE_LESS_MEMORY
    up_buf = init_raster(nrows, ncols, dir_buf->type);

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
            if (DIR(row, col) != DIR_NULL && !UP(row, col))
                trace_down(dir_buf, accum_buf, row, col);
    }

#ifndef USE_LESS_MEMORY
    free_raster(up_buf);
#endif
}

static void trace_down(struct raster_map *dir_buf,
                       struct raster_map *accum_buf, int row, int col)
{
    int up, accum_up = 0;

    do {
        /* accumulate the current cell itself */
#pragma omp atomic write seq_cst
        ACCUM(row, col) = accum_up + 1;

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
        /* if the downstream cell is null or any upstream cells of the downstream
         * cell have never been visited, stop tracing down */
    } while (row >= 0 && row < nrows && col >= 0 && col < ncols &&
             DIR(row, col) != DIR_NULL && (up = UP(row, col)) &&
             (accum_up = sum_up(accum_buf, row, col, up)));
}

/* if any upstream cells have never been visited, 0 is returned; otherwise, the
 * sum of upstream accumulation is returned */
static int sum_up(struct raster_map *accum_buf, int row, int col, int up)
{
    int sum = 0, accum;

    if (up & NW) {
#pragma omp atomic read seq_cst
        accum = ACCUM(row - 1, col - 1);
        if (!accum)
            return 0;
        sum += accum;
    }
    if (up & N) {
#pragma omp atomic read seq_cst
        accum = ACCUM(row - 1, col);
        if (!accum)
            return 0;
        sum += accum;
    }
    if (up & NE) {
#pragma omp atomic read seq_cst
        accum = ACCUM(row - 1, col + 1);
        if (!accum)
            return 0;
        sum += accum;
    }
    if (up & W) {
#pragma omp atomic read seq_cst
        accum = ACCUM(row, col - 1);
        if (!accum)
            return 0;
        sum += accum;
    }
    if (up & E) {
#pragma omp atomic read seq_cst
        accum = ACCUM(row, col + 1);
        if (!accum)
            return 0;
        sum += accum;
    }
    if (up & SW) {
#pragma omp atomic read seq_cst
        accum = ACCUM(row + 1, col - 1);
        if (!accum)
            return 0;
        sum += accum;
    }
    if (up & S) {
#pragma omp atomic read seq_cst
        accum = ACCUM(row + 1, col);
        if (!accum)
            return 0;
        sum += accum;
    }
    if (up & SE) {
#pragma omp atomic read seq_cst
        accum = ACCUM(row + 1, col + 1);
        if (!accum)
            return 0;
        sum += accum;
    }

    return sum;
}
