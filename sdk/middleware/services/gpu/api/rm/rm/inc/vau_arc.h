/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: vau_arc
 */

#ifndef RM_BASIC_ARC_H
#define RM_BASIC_ARC_H

#include "soc_vau_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef CIRCLE_TO_BEZIER_H_90_DEG
#define CIRCLE_TO_BEZIER_H_90_DEG 0.551915024494f
#endif

typedef struct {
    float x;
    float y;
} float_point;

typedef struct {
    float rx;
    float ry;
    td_float cos_phi;
    td_float sin_phi;
    td_float xhalf;
    td_float yhalf;
    td_float x1p;
    td_float y1p;
    float_point r2;
    td_float x12p;
    td_float y12p;
    td_float cr;
    td_float dq;
    td_float pq;
    td_float sc;
    td_float cxp;
    td_float cyp;
    td_float cx;
    td_float cy;
    td_float theta;
    td_float delta;
} local_arc_params;

typedef struct {
    float_point en1;
    float_point en2;
    float_point edn1;
    float_point edn2;
    td_u8 split_len;
    td_u8 t_len;
    td_float alpha;
    td_float step;
    td_float alpha_t;
    td_float theta;
    td_float delta;
    float_point r;
    float_point c;
    float_point angles;
    td_float cos_phi;
    td_float sin_phi;
    td_float interval;
} local_bezier_points;

float_point *create_arc_utility(const float_point *center, float r, float start_angle, float angle_extend, int *num);
float_point *create_arc_cmd(const float_point *start, const float_point *end, unsigned char cmd, float r, int *num);
void destroy_arc_path(float_point *cubic);
td_s32 vau_draw_create_arc_fit_path(const ext_vau_path *org, ext_vau_path *fit);
td_void vau_draw_destroy_arc_fit_path(ext_vau_path *path);
float_point *create_elliptical_arc_utility(float_point p[2], unsigned char cmd, float_point r, float phi, int *num);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* RM_BASIC_ARC_H */
