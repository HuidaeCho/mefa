#define _MAIN_C_

#include <stdio.h>
#include <stdlib.h>
#include <gdal.h>
#ifdef _MSC_VER
#include <winsock2.h>
#else
#include <sys/time.h>
#endif
#include "global.h"

int main(int argc, char *argv[])
{
    char *dir_path, *accum_path;
    struct raster_map *dir_map, *accum_map;
    int nrows, ncols;
    struct timeval start_time, end_time;

    if (argc != 3) {
        printf("Usage: meffa dir.tif accum.tif\n");
        printf("\n");
        printf("  dir.tif\tInput GeoTIFF file of flow direction raster\n");
        printf
            ("  accum.tif\tOutput GeoTIFF file for flow accumulation raster\n");
        exit(EXIT_SUCCESS);
    }

    dir_path = argv[1];
    accum_path = argv[2];

    GDALAllRegister();

    printf("Reading flow direction raster <%s>...\n", dir_path);
    gettimeofday(&start_time, NULL);
    if (!(dir_map = read_raster(dir_path, RASTER_MAP_TYPE_BYTE))) {
        printf("%s: Failed to read flow direction raster\n", dir_path);
        exit(EXIT_FAILURE);
    }
    gettimeofday(&end_time, NULL);
    printf("Input time for flow direction: %lld microsec\n",
           timeval_diff(NULL, &end_time, &start_time));

    nrows = dir_map->nrows;
    ncols = dir_map->ncols;
    accum_map = init_raster(nrows, ncols, RASTER_MAP_TYPE_UINT32);
    copy_raster_metadata(accum_map, dir_map);

    printf("Accumulating flows...\n");
    gettimeofday(&start_time, NULL);
    accumulate(dir_map, accum_map);
    gettimeofday(&end_time, NULL);
    printf("Computation time for flow accumulation: %lld microsec\n",
           timeval_diff(NULL, &end_time, &start_time));
    free_raster(dir_map);

    printf("Writing flow accumulation raster <%s>...\n", accum_path);
    gettimeofday(&start_time, NULL);
    if (write_raster(accum_path, accum_map) > 0) {
        printf("%s: Failed to write flow accumulation raster\n", accum_path);
        free_raster(accum_map);
        exit(EXIT_FAILURE);
    }
    gettimeofday(&end_time, NULL);
    printf("Output time for flow accumulation: %lld microsec\n",
           timeval_diff(NULL, &end_time, &start_time));
    free_raster(accum_map);

    exit(EXIT_SUCCESS);
}
