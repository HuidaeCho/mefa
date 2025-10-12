#include "global.h"

void accumulate(struct raster_map *dir_map, struct raster_map *weight_map,
                struct raster_map *accum_map, int use_lessmem)
{
    switch (accum_map->type) {
    case RASTER_MAP_TYPE_BYTE:
        if (use_lessmem) {
            if (weight_map)
                accumulate_bmw(dir_map, weight_map, accum_map);
            else
                accumulate_bm(dir_map, accum_map);
        }
        else {
            if (weight_map)
                accumulate_bw(dir_map, weight_map, accum_map);
            else
                accumulate_b(dir_map, accum_map);
        }
        break;
    case RASTER_MAP_TYPE_INT16:
        if (use_lessmem) {
            if (weight_map)
                accumulate_imw(dir_map, weight_map, accum_map);
            else
                accumulate_im(dir_map, accum_map);
        }
        else {
            if (weight_map)
                accumulate_iw(dir_map, weight_map, accum_map);
            else
                accumulate_i(dir_map, accum_map);
        }
        break;
    case RASTER_MAP_TYPE_UINT16:
        if (use_lessmem) {
            if (weight_map)
                accumulate_umw(dir_map, weight_map, accum_map);
            else
                accumulate_um(dir_map, accum_map);
        }
        else {
            if (weight_map)
                accumulate_uw(dir_map, weight_map, accum_map);
            else
                accumulate_u(dir_map, accum_map);
        }
        break;
    case RASTER_MAP_TYPE_INT32:
        if (use_lessmem) {
            if (weight_map)
                accumulate_jmw(dir_map, weight_map, accum_map);
            else
                accumulate_jm(dir_map, accum_map);
        }
        else {
            if (weight_map)
                accumulate_jw(dir_map, weight_map, accum_map);
            else
                accumulate_j(dir_map, accum_map);
        }
        break;
    case RASTER_MAP_TYPE_UINT32:
        if (use_lessmem) {
            if (weight_map)
                accumulate_vmw(dir_map, weight_map, accum_map);
            else
                accumulate_vm(dir_map, accum_map);
        }
        else {
            if (weight_map)
                accumulate_vw(dir_map, weight_map, accum_map);
            else
                accumulate_v(dir_map, accum_map);
        }
        break;
    case RASTER_MAP_TYPE_FLOAT32:
        if (use_lessmem) {
            if (weight_map)
                accumulate_fmw(dir_map, weight_map, accum_map);
            else
                accumulate_fm(dir_map, accum_map);
        }
        else {
            if (weight_map)
                accumulate_fw(dir_map, weight_map, accum_map);
            else
                accumulate_f(dir_map, accum_map);
        }
        break;
    default:
        if (use_lessmem) {
            if (weight_map)
                accumulate_dmw(dir_map, weight_map, accum_map);
            else
                accumulate_dm(dir_map, accum_map);
        }
        else {
            if (weight_map)
                accumulate_dw(dir_map, weight_map, accum_map);
            else
                accumulate_d(dir_map, accum_map);
        }
        break;
    }
}
