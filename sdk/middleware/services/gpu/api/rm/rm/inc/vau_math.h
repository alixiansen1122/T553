/*
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description : vau math file
 */

#ifndef API_RM_VAU_MATH_H
#define API_RM_VAU_MATH_H

#include <math.h>
#include "td_base.h"
#include "soc_vau_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define VAU_TRIGO_SHIFT 15
#ifndef CIRCLE_TO_BEZIER_H_90_DEG
#define CIRCLE_TO_BEZIER_H_90_DEG 0.551915024494f
#endif

#define HALF_M_PI (M_PI / 2)

#define deg_to_rad(a) ((a) * M_PI / 180.0f)
#define rad_to_deg(a) ((a) * 180.0f / M_PI)

#define vau_matrix_x(m, x, y) ((x) * (m)[0][0] + (y) * (m)[0][1] + (m)[0][2])
#define vau_matrix_y(m, x, y) ((x) * (m)[1][0] + (y) * (m)[1][1] + (m)[1][2])

static inline td_bool double_is_equal(double a, double b)
{
    return fabs(a - b) <= 1e-6;
}

typedef struct {
    float x;
    float y;
} vau_point_float, vau_vector;

td_float vau_math_fabs(td_float x);
td_float vau_math_fsqrt(td_float x);
td_float vau_math_inv_fsqrt(td_float x);
td_s32 vau_math_min(td_s32 a, td_s32 b);
td_float vau_math_min_f(td_float a, td_float b);
td_s32 vau_math_max(td_s32 a, td_s32 b);
td_float vau_math_max_f(td_float a, td_float b);
td_float vau_math_clamp(td_float value, td_float min_value, td_float max_value);
td_bool vau_math_float_is_equal(td_float a, td_float b);
td_bool vau_math_float_is_zero(td_float x);
td_void vau_matrix_identity(ext_vau_matrix *m);
td_void vau_matrix_mult(ext_vau_matrix *m, const ext_vau_matrix *m2);
td_void vau_matrix_trans(ext_vau_matrix *m, float tx, float ty);
td_void vau_matrix_scale(ext_vau_matrix *m, float sx, float sy);
td_void vau_matrix_shear(ext_vau_matrix *m, float shx, float shy);
td_void vau_matrix_rotate(ext_vau_matrix *m, float angle);
td_void vau_matrix_reflect_x(ext_vau_matrix *m);
td_void vau_matrix_reflect_y(ext_vau_matrix *m);
td_void vau_matrix_reflect_o(ext_vau_matrix *m);
td_s32 vector_dot_product_sign(const vau_vector *v1, const vau_vector *v2);
td_float fast_inv_sqrt(td_float x);
bool float_is_nan(float a);
float vau_circle_to_bezier_h(float rad);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_RM_VAU_MATH_H */
