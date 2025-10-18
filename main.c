#define _MAIN_C_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <gdal.h>
#include "global.h"

int main(int argc, char *argv[])
{
    int i;
    int print_usage = 1, use_lessmem = 0, compress_output = 0;
    int accum_type = RASTER_MAP_TYPE_UINT32;
    double (*recode)(double, void *) = NULL;
    int *recode_data = NULL, encoding[8];
    char *dir_path = NULL, *dir_opts = NULL,
        *weight_path = NULL, *weight_opts = NULL, *accum_path = NULL;
    int num_threads = 0;
    struct raster_map *dir_map, *weight_map, *accum_map;
    struct timeval first_time, start_time, end_time;

    gettimeofday(&first_time, NULL);

    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            int j, n = strlen(argv[i]);
            int unknown = 0;

            for (j = 1; j < n && !unknown; j++) {
                switch (argv[i][j]) {
                case 'a':
                    if (i == argc - 1) {
                        fprintf(stderr, "-%c: Missing accumulation type\n",
                                argv[i][j]);
                        print_usage = 2;
                        break;
                    }
                    if (strcmp(argv[++i], "byte") == 0)
                        accum_type = RASTER_MAP_TYPE_BYTE;
                    else if (strcmp(argv[i], "int16") == 0)
                        accum_type = RASTER_MAP_TYPE_INT16;
                    else if (strcmp(argv[i], "uint16") == 0)
                        accum_type = RASTER_MAP_TYPE_UINT16;
                    else if (strcmp(argv[i], "int32") == 0)
                        accum_type = RASTER_MAP_TYPE_INT32;
                    else if (strcmp(argv[i], "uint32") == 0)
                        accum_type = RASTER_MAP_TYPE_UINT32;
                    else if (strcmp(argv[i], "float32") == 0)
                        accum_type = RASTER_MAP_TYPE_FLOAT32;
                    else if (strcmp(argv[i], "float64") == 0)
                        accum_type = RASTER_MAP_TYPE_FLOAT64;
                    else {
                        fprintf(stderr, "%s: Invalid accumulation type\n",
                                argv[i]);
                        print_usage = 2;
                        break;
                    }
                    break;
                case 'm':
                    use_lessmem = 1;
                    break;
                case 'c':
                    compress_output = 1;
                    break;
                case 'e':
                    if (i == argc - 1) {
                        fprintf(stderr, "-%c: Missing encoding\n",
                                argv[i][j]);
                        print_usage = 2;
                        break;
                    }
                    if (strcmp(argv[++i], "power2") == 0) {
                        recode = NULL;
                        recode_data = NULL;
                        break;
                    }
                    else if (strcmp(argv[i], "taudem") == 0) {
                        int k;

                        for (k = 1; k < 9; k++)
                            encoding[k % 8] = 9 - k;
                    }
                    else if (strcmp(argv[i], "45degree") == 0) {
                        int k;

                        for (k = 0; k < 8; k++)
                            encoding[k] = 8 - k;
                    }
                    else if (strcmp(argv[i], "degree") == 0) {
                        recode = recode_degree;
                        recode_data = NULL;
                        break;
                    }
                    else if (sscanf
                             (argv[i], "%d,%d,%d,%d,%d,%d,%d,%d",
                              &encoding[0], &encoding[1], &encoding[2],
                              &encoding[3], &encoding[4], &encoding[5],
                              &encoding[6], &encoding[7]) != 8) {
                        fprintf(stderr, "%s: Invalid encoding\n", argv[i]);
                        print_usage = 2;
                        break;
                    }
                    recode = recode_encoding;
                    recode_data = encoding;
                    break;
                case 'w':
                    if (i == argc - 1) {
                        fprintf(stderr,
                                "-%c: Missing input weight\n", argv[i][j]);
                        print_usage = 2;
                        break;
                    }
                    weight_path = argv[++i];
                    break;
                case 'D':
                    if (i == argc - 1) {
                        fprintf(stderr,
                                "-%c: Missing GDAL options for input direction\n",
                                argv[i][j]);
                        print_usage = 2;
                        break;
                    }
                    dir_opts = argv[++i];
                    break;
                case 'W':
                    if (i == argc - 1) {
                        fprintf(stderr,
                                "-%c: Missing GDAL options for input weight\n",
                                argv[i][j]);
                        print_usage = 2;
                        break;
                    }
                    weight_opts = argv[++i];
                    break;
                case 't':
                    if (i == argc - 1) {
                        fprintf(stderr, "-%c: Missing number of threads\n",
                                argv[i][j]);
                        print_usage = 2;
                        break;
                    }
                    num_threads = atoi(argv[++i]);
                    break;
                default:
                    unknown = 1;
                    break;
                }
            }
            if (unknown) {
                fprintf(stderr, "%c: Unknown flag\n", argv[i][--j]);
                print_usage = 2;
                break;
            }
        }
        else if (!dir_path)
            dir_path = argv[i];
        else if (!accum_path) {
            accum_path = argv[i];
            print_usage = 0;
        }
        else {
            fprintf(stderr, "%s: Unable to process extra arguments\n",
                    argv[i]);
            print_usage = 2;
            break;
        }
    }

    if (print_usage) {
        if (print_usage == 2)
            printf("\n");
        printf("Usage: mefa OPTIONS dir accum\n\n"
               "  dir\t\tInput flow direction raster (e.g., gpkg:file.gpkg:layer)\n"
               "  accum\t\tOutput flow accumulation GeoTIFF\n"
               "  -a type\tFlow accumulation data type (default: uint32 or weight)\n"
               "\t\tbyte, int16, uint16, int32, uint32, float32, float64\n"
               "  -m\t\tUse less memory\n"
               "  -c\t\tCompress output GeoTIFF\n"
               "  -e encoding\tInput flow direction encoding\n"
               "\t\tpower2 (default): 2^0-7 CW from E (e.g., r.terraflow, ArcGIS)\n"
               "\t\ttaudem: 1-8 (E-SE CCW) (e.g., d8flowdir)\n"
               "\t\t45degree: 1-8 (NE-E CCW) (e.g., r.watershed)\n"
               "\t\tdegree: (0,360] (E-E CCW)\n"
               "\t\tE,SE,S,SW,W,NW,N,NE: custom (e.g., 1,8,7,6,5,4,3,2 for taudem)\n"
               "  -w weight\tInput weight raster\n"
               "  -D opts\tComma-separated list of GDAL options for dir\n"
               "  -W opts\tComma-separated list of GDAL options for weight\n"
               "  -t threads\tNumber of threads (default OMP_NUM_THREADS)\n");
        exit(EXIT_SUCCESS);
    }

    if (num_threads == 0)
        num_threads = omp_get_max_threads();
    else {
        if (num_threads < 0) {
            num_threads += omp_get_num_procs();
            if (num_threads < 1)
                num_threads = 1;
        }
        omp_set_num_threads(num_threads);
    }

    printf("Using %d threads...\n", num_threads);

    GDALAllRegister();

    printf("Reading flow direction raster <%s>...\n", dir_path);
    gettimeofday(&start_time, NULL);
    if (recode) {
        printf("Converting flow direction encoding...\n");
        if (!(dir_map =
              read_raster(dir_path, dir_opts, RASTER_MAP_TYPE_BYTE, 0, recode,
                          recode_data))) {
            fprintf(stderr, "%s: Failed to read flow direction raster\n",
                    dir_path);
            exit(EXIT_FAILURE);
        }
    }
    else if (!(dir_map =
               read_raster(dir_path, dir_opts, RASTER_MAP_TYPE_BYTE, 0, NULL,
                           NULL))) {
        fprintf(stderr, "%s: Failed to read flow direction raster\n",
                dir_path);
        exit(EXIT_FAILURE);
    }
    gettimeofday(&end_time, NULL);
    printf("Input time for flow direction: %lld microsec\n",
           timeval_diff(NULL, &end_time, &start_time));

    if (weight_path) {
        printf("Reading weight raster <%s>...\n", weight_path);
        gettimeofday(&start_time, NULL);
        if (!(weight_map =
              read_raster(weight_path, weight_opts, RASTER_MAP_TYPE_AUTO, 0,
                          NULL, NULL))) {
            fprintf(stderr, "%s: Failed to read weight raster\n",
                    weight_path);
            exit(EXIT_FAILURE);
        }
        gettimeofday(&end_time, NULL);
        printf("Input time for weight: %lld microsec\n",
               timeval_diff(NULL, &end_time, &start_time));
    }
    else
        weight_map = NULL;

    if (weight_map && accum_type != RASTER_MAP_TYPE_FLOAT64 &&
        accum_type < weight_map->type) {
        accum_type = weight_map->type;
        printf("Flow accumulation data type promoted to the weight type\n");
    }

    accum_map = init_raster(dir_map->nrows, dir_map->ncols, accum_type);
    copy_raster_metadata(accum_map, dir_map);

    printf("Accumulating flows...\n");
    gettimeofday(&start_time, NULL);
    accumulate(dir_map, weight_map, accum_map, use_lessmem);
    gettimeofday(&end_time, NULL);
    printf("Computation time for flow accumulation: %lld microsec\n",
           timeval_diff(NULL, &end_time, &start_time));
    free_raster(dir_map);
    if (weight_map)
        free_raster(weight_map);

    accum_map->compress = compress_output;
    printf("Writing flow accumulation raster <%s>...\n", accum_path);
    gettimeofday(&start_time, NULL);
    if (write_raster(accum_path, accum_map, RASTER_MAP_TYPE_AUTO) > 0) {
        fprintf(stderr, "%s: Failed to write flow accumulation raster\n",
                accum_path);
        free_raster(accum_map);
        exit(EXIT_FAILURE);
    }
    gettimeofday(&end_time, NULL);
    printf("Output time for flow accumulation: %lld microsec\n",
           timeval_diff(NULL, &end_time, &start_time));

    free_raster(accum_map);

    gettimeofday(&end_time, NULL);
    printf("Total elapsed time: %lld microsec\n",
           timeval_diff(NULL, &end_time, &first_time));

    exit(EXIT_SUCCESS);
}
