#include <gdal.h>
#include <cpl_string.h>
#include <omp.h>
#include "global.h"

struct raster_map *init_raster(int nrows, int ncols, int type)
{
    struct raster_map *rast_map;
    size_t row_size;
    int i;

    rast_map = malloc(sizeof *rast_map);
    rast_map->nrows = nrows;
    rast_map->ncols = row_size = ncols;
    rast_map->type = type;

    switch (type) {
    case RASTER_MAP_TYPE_UINT32:
        row_size *= sizeof(unsigned int);
        break;
    case RASTER_MAP_TYPE_INT32:
        row_size *= sizeof(int);
        break;
    }

    rast_map->cells.v = calloc(nrows, row_size);

    rast_map->null_value = 0;
    rast_map->projection = NULL;
    for (i = 0; i < 6; i++)
        rast_map->geotransform[i] = 0;
    rast_map->compress = 0;

    return rast_map;
}

void free_raster(struct raster_map *rast_map)
{
    free(rast_map->cells.v);
    free(rast_map->projection);
}

void copy_raster_metadata(struct raster_map *dest_map,
                          const struct raster_map *src_map)
{
    int i;

    dest_map->projection = strdup(src_map->projection);
    for (i = 0; i < 6; i++)
        dest_map->geotransform[i] = src_map->geotransform[i];
}

struct raster_map *read_raster(const char *path, int type)
{
    struct raster_map *rast_map;
    GDALDatasetH *datasets, dataset;
    GDALRasterBandH *bands, band;
    GDALDataType gdt_type;
    size_t row_size;
    int row, error = 0;

#pragma omp parallel
    {
#pragma omp single
        datasets = malloc(sizeof *datasets * omp_get_num_threads());
        datasets[omp_get_thread_num()] = GDALOpen(path, GA_ReadOnly);
    }

    if (!(dataset = datasets[0]))
        return NULL;

    rast_map = malloc(sizeof *rast_map);
    rast_map->type = type;
    rast_map->nrows = GDALGetRasterYSize(dataset);
    rast_map->ncols = row_size = GDALGetRasterXSize(dataset);
    rast_map->projection = strdup(GDALGetProjectionRef(dataset));
    GDALGetGeoTransform(dataset, rast_map->geotransform);

#pragma omp parallel
    {
#pragma omp single
        bands = malloc(sizeof *bands * omp_get_num_threads());
        bands[omp_get_thread_num()] =
            GDALGetRasterBand(datasets[omp_get_thread_num()], 1);
    }

    band = bands[0];
    rast_map->null_value = GDALGetRasterNoDataValue(band, NULL);
    rast_map->compress = 0;

    switch (type) {
    case RASTER_MAP_TYPE_UINT32:
        gdt_type = GDT_UInt32;
        row_size *= sizeof(unsigned int);
        break;
    case RASTER_MAP_TYPE_INT32:
        gdt_type = GDT_Int32;
        row_size *= sizeof(int);
        break;
    default:
        gdt_type = GDT_Byte;
        break;
    }

    rast_map->cells.v = malloc(rast_map->nrows * row_size);

#pragma omp parallel for schedule(dynamic)
    for (row = 0; row < rast_map->nrows; row++) {
        if (GDALRasterIO
            (bands[omp_get_thread_num()], GF_Read, 0, row, rast_map->ncols, 1,
             (char *)rast_map->cells.v + row * row_size, rast_map->ncols,
             1, gdt_type, 0, 0) != CE_None)
            error = 1;
    }

#pragma omp parallel
    GDALClose(datasets[omp_get_thread_num()]);

    if (error)
        return NULL;

    return rast_map;
}

int write_raster(const char *path, struct raster_map *rast_map)
{
    GDALDriverH driver = GDALGetDriverByName("GTiff");
    char **metadata, **options = NULL;
    GDALDatasetH dataset;
    GDALRasterBandH band;
    GDALDataType gdt_type;
    size_t row_size;
    int row;

    if (!driver)
        return 1;

    metadata = GDALGetMetadata(driver, NULL);
    if (!CSLFetchBoolean(metadata, GDAL_DCAP_CREATE, FALSE))
        return 2;

    if (rast_map->compress)
        options = CSLSetNameValue(options, "COMPRESS", "LZW");

    row_size = rast_map->ncols;

    switch (rast_map->type) {
    case RASTER_MAP_TYPE_UINT32:
        gdt_type = GDT_UInt32;
        row_size *= sizeof(unsigned int);
        break;
    case RASTER_MAP_TYPE_INT32:
        gdt_type = GDT_Int32;
        row_size *= sizeof(int);
        break;
    default:
        gdt_type = GDT_Byte;
        break;
    }

    if (!
        (dataset =
         GDALCreate(driver, path, rast_map->ncols, rast_map->nrows, 1,
                    gdt_type, options)))
        return 3;

    GDALSetProjection(dataset, rast_map->projection);
    GDALSetGeoTransform(dataset, rast_map->geotransform);

    band = GDALGetRasterBand(dataset, 1);
    GDALSetRasterNoDataValue(band, rast_map->null_value);

    for (row = 0; row < rast_map->nrows; row++) {
        if (GDALRasterIO
            (band, GF_Write, 0, row, rast_map->ncols, 1,
             (char *)rast_map->cells.v + row * row_size, rast_map->ncols,
             1, gdt_type, 0, 0) != CE_None)
            return 4;
    }

    GDALClose(dataset);

    return 0;
}
