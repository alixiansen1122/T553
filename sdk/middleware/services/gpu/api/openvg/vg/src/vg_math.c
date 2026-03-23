/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Description: openvg context adapter source file
 * Author: @CompanyNameTag
 * Create: 2021-07-20
 */

#include "vg_math.h"

typedef union {
    float f;
    unsigned int i;
} float_int;

void vg_matrix_mult(ext_vau_matrix *m, const ext_vau_matrix *m2)
{
    ext_vau_matrix t;
    for (int i = 0; i < 3; i++) { /* 3 * 3 matrix */
        for (int j = 0; j < 3; j++) { /* 3 * 3 matrix */
            t.matrix[i][j] = m->matrix[i][0] * m2->matrix[0][j] + m->matrix[i][1] * m2->matrix[1][j] +
                m->matrix[i][2] * m2->matrix[2][j]; /* 2 is index */
        }
    }
    for (int i = 0; i < 3; i++) { /* 3 * 3 matrix */
        for (int j = 0; j < 3; j++) { /* 3 * 3 matrix */
            m->matrix[i][j] = t.matrix[i][j];
        }
    }
}

void vg_matrix_identity(ext_vau_matrix *m)
{
    for (int i = 0; i < 3; i++) { /* 3 * 3 matrix */
        for (int j = 0; j < 3; j++) { /* 3 * 3 matrix */
            m->matrix[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

void vg_matrix_trans(ext_vau_matrix *m, float tx, float ty)
{
    ext_vau_matrix m2 = { .matrix = {
        { 1, 0, tx },
        { 0, 1, ty },
        { 0, 0, 1 } }
    };
    vg_matrix_mult(m, &m2);
}

void vg_matrix_scale(ext_vau_matrix *m, float sx, float sy)
{
    ext_vau_matrix m2 = { .matrix = {
        { sx, 0, 0 },
        { 0, sy, 0 },
        { 0, 0, 1 } }
    };
    vg_matrix_mult(m, &m2);
}

void vg_matrix_shear(ext_vau_matrix *m, float shx, float shy)
{
    ext_vau_matrix m2 = { .matrix = {
        { 1, shx, 0 },
        { shy, 1, 0 },
        { 0, 0, 1 } }
    };
    vg_matrix_mult(m, &m2);
}

void vg_matrix_rotate(ext_vau_matrix *m, float angle)
{
    float a = angle * M_PI / 180.0f;
    ext_vau_matrix m2 = { .matrix = {
        { cos(a), -sin(a), 0 },
        { sin(a), cos(a), 0 },
        { 0, 0, 1 } }
    };
    vg_matrix_mult(m, &m2);
}

bool vg_float_is_equal(float a, float b)
{
    return fabs(a - b) <= 1e-6;
}

bool vg_float_is_nan(float a)
{
    float_int p;
    p.f = a;
    unsigned int exponent = (p.i >> 23) & 0xff; /* 23: int count */
    unsigned int mantissa = p.i & 0x7fffff;
    if ((exponent == 0xff) && (mantissa != 0)) {
        return VG_TRUE;
    }
    return VG_FALSE;
}

float vg_float_clamp(float a, float l, float h)
{
    if (vg_float_is_nan(a)) {
        return l;
    }
    return (a < l) ? l : (a > h) ? h : a;
}

float circle_to_bezier_h(float rad)
{
    /* 4.0f 3.0f and 2 is alg num */
    return 4.0f / 3.0f * (1 - cos(rad / 2)) / sin(rad / 2);
}
