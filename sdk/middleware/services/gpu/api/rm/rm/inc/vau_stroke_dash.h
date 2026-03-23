/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: render path basic
 */

#ifndef VAU_STROKE_DASH_H
#define VAU_STROKE_DASH_H

#include "soc_vau_type.h"
#include "vau_math.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VAU_CUBIC_BEZIER_POINT_NUM  4

typedef struct {
    float x;
    float y;
} vau_path_line_to;

typedef struct {
    float x0; /* ctrl point */
    float y0; /* ctrl point */
    float x1; /* end point */
    float y1; /* end point */
} vau_path_quad_to;

typedef struct {
    float x0; /* ctrl point */
    float y0; /* ctrl point */
    float x1; /* ctrl point */
    float y1; /* ctrl point */
    float x2; /* end point */
    float y2; /* end point */
} vau_path_cubic_to;

typedef struct {
    vau_point_float s;
    vau_point_float p;
    vau_point_float o;
    td_uchar cmd;
    td_u32 offset;
    td_u32 data_num;
    vau_path_line_to line_to;
    vau_path_quad_to quad_to;
    vau_path_cubic_to cubic_to;
    vau_point_float cubic[VAU_CUBIC_BEZIER_POINT_NUM];
    td_bool rel;
} local_stroke_dash_path;

td_s32 vau_create_stroke_dash_path(const ext_vau_path *path, const ext_vau_stroke_attr *stroke,
    ext_vau_path *dash_path);
td_void vau_destroy_stroke_dash_path(ext_vau_path *dash_path);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* VAU_STROKE_DASH_H */
