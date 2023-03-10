#include "global.h"

#define DIR_NULL (unsigned char)dir_map->null_value
#define DIR(row, col) dir_map->cells.byte[(row) * ncols + (col)]
#define ACCUM(row, col) accum_map->cells.uint32[(row) * ncols + (col)]
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

#ifdef USE_LESS_MEMORY
#define UP(row, col) FIND_UP(row, col)
#else
#define UP(row, col) up_cells[(row) * ncols + (col)]
static unsigned char *up_cells;
#endif

static int nrows, ncols;

static void trace_down(struct raster_map *, struct raster_map *, int, int,
                       int);
static int sum_up(struct raster_map *, int, int, int);

void accumulate(struct raster_map *dir_map, struct raster_map *accum_map)
{
    int row, col;

    nrows = dir_map->nrows;
    ncols = dir_map->ncols;

#ifndef USE_LESS_MEMORY
    up_cells = calloc(nrows * ncols, sizeof *up_cells);

#ifdef _MSC_VER
    int ncells = nrows * ncols;
    int rowcol;

#pragma omp parallel for schedule(guided)
    for (rowcol = 0; rowcol < ncells; rowcol++) {
        row = rowcol / ncols;
        col = rowcol % ncols;
#else
#pragma omp parallel for schedule(guided) collapse(2)
    for (row = 0; row < nrows; row++) {
        for (col = 0; col < ncols; col++)
#endif
            if (DIR(row, col) != DIR_NULL)
                UP(row, col) = FIND_UP(row, col);
    }
#endif

#ifdef _MSC_VER
#pragma omp parallel for schedule(guided)
    for (rowcol = 0; rowcol < ncells; rowcol++) {
        row = rowcol / ncols;
        col = rowcol % ncols;
#else
#pragma omp parallel for schedule(guided) collapse(2)
    for (row = 0; row < nrows; row++) {
        for (col = 0; col < ncols; col++)
#endif
            /* if the current cell is not null and has no upstream cells, start
             * tracing down */
            if (DIR(row, col) != DIR_NULL && !UP(row, col))
                trace_down(dir_map, accum_map, row, col, 1);
    }

#ifndef USE_LESS_MEMORY
    free(up_cells);
#endif
}

static void trace_down(struct raster_map *dir_map,
                       struct raster_map *accum_map, int row, int col,
                       int accum)
{
    int up, accum_up = 0;

    /* accumulate the current cell itself */
#pragma omp atomic write seq_cst
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

    /* if the downstream cell is null or any upstream cells of the downstream
     * cell have never been visited, stop tracing down */
    if (row < 0 || row >= nrows || col < 0 || col >= ncols ||
        DIR(row, col) == DIR_NULL || !(up = UP(row, col)) ||
        !(accum_up = sum_up(accum_map, row, col, up)))
        return;

    /* use gcc -O2 or -O3 flags for tail-call optimization
     * (-foptimize-sibling-calls) */
    trace_down(dir_map, accum_map, row, col, accum_up + 1);
}

/* if any upstream cells have never been visited, 0 is returned; otherwise, the
 * sum of upstream accumulation is returned */
static int sum_up(struct raster_map *accum_map, int row, int col, int up)
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
