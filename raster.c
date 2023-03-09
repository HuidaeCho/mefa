#include <gdal.h>
#include <cpl_string.h>
#include "global.h"

struct raster_map *init_raster(int nrows, int ncols, int type)
{
    struct raster_map *raster_buf;
    size_t row_size;
    int i;

    raster_buf = malloc(sizeof *raster_buf);
    raster_buf->nrows = nrows;
    raster_buf->ncols = row_size = ncols;
    raster_buf->type = type;

    switch (type) {
    case RASTER_MAP_TYPE_UINT32:
        row_size *= sizeof(unsigned int);
        break;
    case RASTER_MAP_TYPE_INT32:
        row_size *= sizeof(int);
        break;
    }

    raster_buf->cells.v = calloc(nrows, row_size);

    raster_buf->null_value = 0;
    raster_buf->projection = NULL;
    for (i = 0; i < 6; i++)
	raster_buf->geotransform[i] = 0;
    raster_buf->compress = 0;

    return raster_buf;
}

void free_raster(struct raster_map *raster_buf)
{
    free(raster_buf->cells.v);
    free(raster_buf->projection);
}

void copy_raster_metadata(struct raster_map *dest_buf,
                          const struct raster_map *src_buf)
{
    int i;

    dest_buf->projection = strdup(src_buf->projection);
    for (i = 0; i < 6; i++)
        dest_buf->geotransform[i] = src_buf->geotransform[i];
}

struct raster_map *read_raster(const char *path, int type)
{
    struct raster_map *raster_buf;
    GDALDatasetH dataset;
    GDALRasterBandH band;
    GDALDataType gdt_type;
    size_t row_size;
    int row;

    if (!(dataset = GDALOpen(path, GA_ReadOnly)))
        return NULL;

    raster_buf = malloc(sizeof *raster_buf);
    raster_buf->type = type;
    raster_buf->nrows = GDALGetRasterYSize(dataset);
    raster_buf->ncols = row_size = GDALGetRasterXSize(dataset);
    raster_buf->projection = strdup(GDALGetProjectionRef(dataset));
    GDALGetGeoTransform(dataset, raster_buf->geotransform);

    band = GDALGetRasterBand(dataset, 1);
    raster_buf->null_value = GDALGetRasterNoDataValue(band, NULL);
    raster_buf->compress = 0;

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

    raster_buf->cells.v = malloc(raster_buf->nrows * row_size);
    for (row = 0; row < raster_buf->nrows; row++) {
        if (GDALRasterIO
            (band, GF_Read, 0, row, raster_buf->ncols, 1,
             (char *)raster_buf->cells.v + row * row_size, raster_buf->ncols,
             1, gdt_type, 0, 0) != CE_None)
            return NULL;
    }

    GDALClose(dataset);

    return raster_buf;
}

int write_raster(const char *path, struct raster_map *raster_buf)
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

    if (raster_buf->compress)
        options = CSLSetNameValue(options, "COMPRESS", "LZW");

    row_size = raster_buf->ncols;

    switch (raster_buf->type) {
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
         GDALCreate(driver, path, raster_buf->ncols, raster_buf->nrows, 1,
                    gdt_type, options)))
        return 3;

    GDALSetProjection(dataset, raster_buf->projection);
    GDALSetGeoTransform(dataset, raster_buf->geotransform);

    band = GDALGetRasterBand(dataset, 1);
    GDALSetRasterNoDataValue(band, raster_buf->null_value);

    for (row = 0; row < raster_buf->nrows; row++) {
        if (GDALRasterIO
            (band, GF_Write, 0, row, raster_buf->ncols, 1,
             (char *)raster_buf->cells.v + row * row_size, raster_buf->ncols,
             1, gdt_type, 0, 0) != CE_None)
            return 4;
    }

    GDALClose(dataset);

    return 0;
}
