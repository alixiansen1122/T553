/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : openvg math file
 */

#ifndef API_OPENVG_MATH_H
#define API_OPENVG_MATH_H

#include <math.h>
#include "VG/openvg.h"
#include "soc_vau_type.h"

#define HALF_M_PI (M_PI / 2)

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void vg_matrix_mult(ext_vau_matrix *m, const ext_vau_matrix *m2);
void vg_matrix_identity(ext_vau_matrix *m);
void vg_matrix_trans(ext_vau_matrix *m, float tx, float ty);
void vg_matrix_scale(ext_vau_matrix *m, float sx, float sy);
void vg_matrix_shear(ext_vau_matrix *m, float shx, float shy);
void vg_matrix_rotate(ext_vau_matrix *m, float angle);

bool vg_float_is_equal(float a, float b);
bool vg_float_is_nan(float a);
float vg_float_clamp(float a, float l, float h);

float circle_to_bezier_h(float rad);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_OPENVG_MATH_H */
