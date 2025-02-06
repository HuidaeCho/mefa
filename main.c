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
    int i;
    int print_usage = 1, use_lessmem = 0, compress_output =
        0, custom_encoding = 0;
    int encoding[8];
    char *dir_path = NULL, *accum_path = NULL;
    struct raster_map *dir_map, *accum_map;
    struct timeval start_time, end_time;

    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            int j, n = strlen(argv[i]);
            int unknown = 0;

            for (j = 1; j < n && !unknown; j++) {
                switch (argv[i][j]) {
                case 'l':
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
                        custom_encoding = 0;
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
                    else if (sscanf
                             (argv[i], "%d,%d,%d,%d,%d,%d,%d,%d",
                              &encoding[0], &encoding[1], &encoding[2],
                              &encoding[3], &encoding[4], &encoding[5],
                              &encoding[6], &encoding[7]) != 8) {
                        fprintf(stderr, "%s: Invalid encoding\n", argv[i]);
                        print_usage = 2;
                        break;
                    }
                    custom_encoding = 1;
                    break;
                default:
                    unknown = 1;
                    break;
                }
            }
            if (unknown) {
                fprintf(stderr, "%c: Unknown flag\n", argv[i][j]);
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
        printf("Usage: mefa [-lc] [-e encoding] dir.tif accum.tif\n");
        printf("\n");
        printf("  -l\t\tUse less memory\n");
        printf("  -c\t\tCompress output GeoTIFF file\n");
        printf("  -e encoding\tDirection encoding\n");
        printf
            ("\t\tpower2 (default): 2^0-7 CW from E (e.g., r.terraflow, ArcGIS)\n");
        printf("\t\ttaudem: 1-8 (E-SE CCW) (e.g., d8flowdir)\n");
        printf("\t\t45degree: 1-8 (NE-E CCW) (e.g., r.watershed)\n");
        printf
            ("\t\tE,SE,S,SW,W,NW,N,NE: custom (e.g., 1,8,7,6,5,4,3,2 for taudem)\n");
        printf("  dir.tif\tInput GeoTIFF file of flow direction raster\n");
        printf
            ("  accum.tif\tOutput GeoTIFF file for flow accumulation raster\n");
        exit(EXIT_SUCCESS);
    }

    GDALAllRegister();

    printf("Reading flow direction raster <%s>...\n", dir_path);
    gettimeofday(&start_time, NULL);
    if (!(dir_map = read_raster(dir_path, RASTER_MAP_TYPE_BYTE, 0))) {
        fprintf(stderr, "%s: Failed to read flow direction raster\n",
                dir_path);
        exit(EXIT_FAILURE);
    }
    if (custom_encoding) {
        printf("Converting flow direction encoding...\n");
        if (convert_encoding(dir_map, encoding))
            exit(EXIT_FAILURE);
    }
    gettimeofday(&end_time, NULL);
    printf("Input time for flow direction: %lld microsec\n",
           timeval_diff(NULL, &end_time, &start_time));

    accum_map =
        init_raster(dir_map->nrows, dir_map->ncols, RASTER_MAP_TYPE_UINT32);
    copy_raster_metadata(accum_map, dir_map);

    printf("Accumulating flows...\n");
    gettimeofday(&start_time, NULL);
    accumulate(dir_map, accum_map, use_lessmem);
    gettimeofday(&end_time, NULL);
    printf("Computation time for flow accumulation: %lld microsec\n",
           timeval_diff(NULL, &end_time, &start_time));
    free_raster(dir_map);

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

    exit(EXIT_SUCCESS);
}
