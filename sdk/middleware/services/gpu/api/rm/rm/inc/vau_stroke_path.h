/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: stroke path
 */

#ifndef STROKE_INC_VAU_STROKE_PATH_H
#define STROKE_INC_VAU_STROKE_PATH_H

#include "soc_vau_type.h"
#include "vau_stroke_dash.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    EXT_BEZIER_MODE_LINE_AVG,
    EXT_BEZIER_MODE_BEZIER_4,
    EXT_BEZIER_MODE_BEZIER_CLOSE,
    EXT_BEZIER_MODE_BEZIER_MONO,
    EXT_BEZIER_MODE_BEZIER_ELIA,
    EXT_BEZIER_MODE_BEZIER_ELIA_4,
    EXT_BEZIER_MODE_BEZIER_ELIA_CLOSE,
    EXT_BEZIER_MODE_LINE_FREETYPE,
    EXT_BEZIER_MODE_LINE_AGG,
    EXT_BEZIER_MODE_MAX
} ext_vau_bezier_stroke_split_mode;

typedef struct {
    vau_vector v;
    vau_point_float c1;
    vau_point_float c2;
    vau_point_float p4;
    float h;
    float p1p3;
    float p1p4;
    float p3p4;
    float sin;
    float cos;
} local_stroke_add_round;

typedef struct {
    td_s32 x1;
    td_s32 y1;
    td_s32 x2;
    td_s32 y2;
} ext_vau_line_fixed;

typedef struct {
    td_u32 i;
    float sx;
    float sy;
    float px;
    float py;
    float ox;
    float oy;
    td_uchar cmd;
    td_u32 offset;
    td_u32 data_num;
    vau_path_line_to line_to;
    vau_path_quad_to quad_to;
    vau_path_cubic_to cubic_to;
    ext_vau_line_fixed line_inner;
    ext_vau_line_fixed line_outer;
    td_u32 cubic_split_count;
    vau_point_float p0;
    vau_point_float p1;
    vau_point_float left;
    vau_point_float right;
    td_bool rel;
    float width;
    float half_width;
    td_bool has_close;
    td_u32 sub_index;
    int l_count;
    int c_count;
    td_bool add_join;
    td_bool add_cap;
    td_s32 ret;
    float rx;
    float ry;
    td_u32 point_num;
    td_u32 cubic_split_count_out;
} local_parse_stroke_path;

typedef struct {
    float miter_limit;
    float half_width;
} vau_line_cap_param;

typedef struct {
    td_bool add_join;
    td_bool add_cap;
} vau_join_cap_flag;

td_s32 vau_create_stroke_to_fill_path(const ext_vau_path *path, const ext_vau_stroke_attr *stroke_attr,
    ext_vau_path *fill_path, ext_vau_fill_attr *fill_attr, ext_vau_bezier_stroke_split_mode mode);
td_void vau_destroy_stroke_to_fill_path(ext_vau_path *fill_path);
td_u32 cubic_bezier_subdivision(const vau_point_float *points, vau_point_float *points_subdivision);
td_void split_cubic_to_line_average(const vau_point_float *cubic, vau_point_float *line_points, td_u32 point_num);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* STROKE_INC_VAU_STROKE_PATH_H */
