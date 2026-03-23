/*
 * Copyright (c) @CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: matrix operation
 */

#include "vau_matrix.h"
#include <math.h>
#include "soc_vau_api.h"
#include "vau_debug.h"
#include "vau_math.h"

td_void uapi_vau_matrix_identity(ext_vau_matrix *m)
{
    for (int i = 0; i < 3; i++) { /* 3 * 3 matrix */
        for (int j = 0; j < 3; j++) { /* 3 * 3 matrix */
            m->matrix[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

td_void uapi_vau_matrix_mult(ext_vau_matrix *m, const ext_vau_matrix *n)
{
    ext_vau_matrix t;
    for (int i = 0; i < 3; i++) { /* 3 * 3 matrix */
        for (int j = 0; j < 3; j++) { /* 3 * 3 matrix */
            t.matrix[i][j] = m->matrix[i][0] * n->matrix[0][j] + m->matrix[i][1] * n->matrix[1][j] +
                m->matrix[i][2] * n->matrix[2][j]; /* 2 is index */
        }
    }
    for (int i = 0; i < 3; i++) { /* 3 * 3 matrix */
        for (int j = 0; j < 3; j++) { /* 3 * 3 matrix */
            m->matrix[i][j] = t.matrix[i][j];
        }
    }
}

td_void uapi_vau_matrix_trans(ext_vau_matrix *m, td_float tx, td_float ty)
{
    ext_vau_matrix n = { .matrix = {
        { 1, 0, tx },
        { 0, 1, ty },
        { 0, 0, 1 } }
    };
    uapi_vau_matrix_mult(m, &n);
}

td_void uapi_vau_matrix_scale(ext_vau_matrix *m, td_float sx, td_float sy)
{
    ext_vau_matrix n = { .matrix = {
        { sx, 0, 0 },
        { 0, sy, 0 },
        { 0, 0, 1 } }
    };
    uapi_vau_matrix_mult(m, &n);
}

td_void uapi_vau_matrix_shear(ext_vau_matrix *m, td_float shx, td_float shy)
{
    ext_vau_matrix n = { .matrix = {
        { 1, shx, 0 },
        { shy, 1, 0 },
        { 0, 0, 1 } }
    };
    uapi_vau_matrix_mult(m, &n);
}

td_void uapi_vau_matrix_rotate(ext_vau_matrix *m, td_float angle)
{
    float a = angle * M_PI / 180.0f; /* * M_PI / 180: deg to rad */
    ext_vau_matrix n = { .matrix = {
        { cos(a), -sin(a), 0 },
        { sin(a), cos(a), 0 },
        { 0, 0, 1 } }
    };
    uapi_vau_matrix_mult(m, &n);
}

td_void uapi_vau_matrix_reflect_x(ext_vau_matrix *m)
{
    ext_vau_matrix n = { .matrix = {
        { 1, 0, 0 },
        { 0, -1, 0 },
        { 0, 0, 1 } }
    };
    uapi_vau_matrix_mult(m, &n);
}

td_void uapi_vau_matrix_reflect_y(ext_vau_matrix *m)
{
    ext_vau_matrix n = { .matrix = {
        { -1, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 } }
    };
    uapi_vau_matrix_mult(m, &n);
}

static td_bool vau_matrix_is_affine(const ext_vau_matrix *matrix)
{
    // 2: row 2
    if (vau_math_float_is_equal(matrix->matrix[2][0], 0.0) &&
        vau_math_float_is_equal(matrix->matrix[2][1], 0.0) && /* 2: index */
        vau_math_float_is_equal(matrix->matrix[2][2], 1.0)) { /* 2: row 2 */
        return TD_TRUE;
    }
    return TD_FALSE;
}

static td_bool vau_matrix_invert(ext_vau_matrix *matrix)
{
    td_bool affine = vau_matrix_is_affine(matrix);
    float det00 = matrix->matrix[1][1] * matrix->matrix[2][2] - matrix->matrix[2][1] * matrix->matrix[1][2];
    float det01 = matrix->matrix[2][0] * matrix->matrix[1][2] - matrix->matrix[1][0] * matrix->matrix[2][2];
    float det02 = matrix->matrix[1][0] * matrix->matrix[2][1] - matrix->matrix[2][0] * matrix->matrix[1][1];
    float d = matrix->matrix[0][0] * det00 + matrix->matrix[0][1] * det01 + matrix->matrix[0][2] * det02;
    if (vau_math_float_is_equal(d, 0) == TD_TRUE) {
        vau_err("matrix can not invert");
        return TD_FALSE; // singular, leave the matrix unmodified and return false
    }
    d = 1.0f / d;
    ext_vau_matrix t;
    t.matrix[0][0] = d * det00;
    t.matrix[1][0] = d * det01;
    // [2][]: row 2; [][2]: col 2
    t.matrix[2][0] = d * det02;
    // [2][]: row 2; [][2]: col 2
    t.matrix[0][1] = d * (matrix->matrix[2][1] * matrix->matrix[0][2] - matrix->matrix[0][1] * matrix->matrix[2][2]);
    // [2][]: row 2; [][2]: col 2
    t.matrix[1][1] = d * (matrix->matrix[0][0] * matrix->matrix[2][2] - matrix->matrix[2][0] * matrix->matrix[0][2]);
    // [2][]: row 2; [][2]: col 2
    t.matrix[2][1] = d * (matrix->matrix[2][0] * matrix->matrix[0][1] - matrix->matrix[0][0] * matrix->matrix[2][1]);
    // [2][]: row 2; [][2]: col 2
    t.matrix[0][2] = d * (matrix->matrix[0][1] * matrix->matrix[1][2] - matrix->matrix[1][1] * matrix->matrix[0][2]);
    // [2][]: row 2; [][2]: col 2
    t.matrix[1][2] = d * (matrix->matrix[1][0] * matrix->matrix[0][2] - matrix->matrix[0][0] * matrix->matrix[1][2]);
    // [2][]: row 2; [][2]: col 2
    t.matrix[2][2] = d * (matrix->matrix[0][0] * matrix->matrix[1][1] - matrix->matrix[1][0] * matrix->matrix[0][1]);
    if (affine) {
        // [2][]: row 2; [][2]: col 2
        t.matrix[2][0] = 0;
        // [2][]: row 2; [][2]: col 2
        t.matrix[2][1] = 0;
        // [2][]: row 2; [][2]: col 2
        t.matrix[2][2] = 1;
    }
    *matrix = t;

    return TD_TRUE;
}

td_s32 uapi_vau_compute_warp_square_to_quad(ext_vau_matrix *m, const ext_vau_point points[4])
{
    if (m == TD_NULL) {
        vau_err("invalid input");
        return EXT_ERR_GFX_INVALID_PARAM;
    }
    float diffx1 = points[1].x - points[3].x;
    float diffy1 = points[1].y - points[3].y;
    float diffx2 = points[2].x - points[3].x;
    float diffy2 = points[2].y - points[3].y;
    float det = diffx1 * diffy2 - diffx2 * diffy1;
    if (vau_math_float_is_equal(det, 0) == TD_TRUE) {
        vau_err("det is 0");
        return EXT_ERR_GFX_INVALID_PARAM;
    }
    float sumx = points[0].x - points[1].x + points[3].x - points[2].x;
    float sumy = points[0].y - points[1].y + points[3].y - points[2].y;
    if ((vau_math_float_is_equal(sumx, 0.0f) == TD_TRUE) &&
        (vau_math_float_is_equal(sumy, 0.0f) == TD_TRUE)) { // affine mapping
        m->matrix[0][0] = points[1].x - points[0].x;
        m->matrix[1][0] = points[1].y - points[0].y;
        m->matrix[2][0] = 0.0f; // [2][]: row 2; [][2]: col 2
        m->matrix[0][1] = points[3].x - points[1].x; // index 3
        m->matrix[1][1] = points[3].y - points[1].y; // index 3
        // [2][]: row 2; [][2]: col 2
        m->matrix[2][1] = 0.0f;
        // [2][]: row 2; [][2]: col 2
        m->matrix[0][2] = points[0].x;
        // [2][]: row 2; [][2]: col 2
        m->matrix[1][2] = points[0].y;
        // [2][]: row 2; [][2]: col 2
        m->matrix[2][2] = 1.0f;
        return EXT_SUCCESS;
    }
    float oodet = 1.0f / det;
    float g = (sumx * diffy2 - diffx2 * sumy) * oodet;
    float h = (diffx1 * sumy - sumx * diffy1) * oodet;
    m->matrix[0][0] = points[1].x - points[0].x + g * points[1].x;
    m->matrix[1][0] = points[1].y - points[0].y + g * points[1].y;
    // [2][]: row 2; [][2]: col 2
    m->matrix[2][0] = g;
    m->matrix[0][1] = points[2].x - points[0].x + h * points[2].x; // index 2
    m->matrix[1][1] = points[2].y - points[0].y + h * points[2].y; // index 2
    // [2][]: row 2; [][2]: col 2
    m->matrix[2][1] = h;
    // [2][]: row 2; [][2]: col 2
    m->matrix[0][2] = points[0].x;
    // [2][]: row 2; [][2]: col 2
    m->matrix[1][2] = points[0].y;
    // [2][]: row 2; [][2]: col 2
    m->matrix[2][2] = 1.0f;
    return EXT_SUCCESS;
}

td_s32 uapi_vau_compute_warp_quad_to_square(ext_vau_matrix *m, const ext_vau_point points[4])
{
    if (m == TD_NULL) {
        vau_err("invalid input");
        return EXT_ERR_GFX_INVALID_PARAM;
    }
    td_s32 ret = uapi_vau_compute_warp_square_to_quad(m, points);
    if (ret != EXT_SUCCESS) {
        return ret;
    }
    td_bool nonsingular = vau_matrix_invert(m);
    if (!nonsingular) {
        vau_err("matrix can not invert");
        return EXT_ERR_GFX_INVALID_PARAM;
    }
    return EXT_SUCCESS;
}

td_s32 uapi_vau_compute_warp_quad_to_quad(ext_vau_matrix *m, const ext_vau_point src_points[4],
                                          const ext_vau_point dst_points[4])
{
    if (m == TD_NULL) {
        vau_err("invalid input");
        return EXT_ERR_GFX_INVALID_PARAM;
    }
    ext_vau_matrix m1, m2;

    td_s32 ret = uapi_vau_compute_warp_quad_to_square(&m1, src_points);
    if (ret != EXT_SUCCESS) {
        return ret;
    }
    ret = uapi_vau_compute_warp_square_to_quad(&m2, dst_points);
    if (ret != EXT_SUCCESS) {
        return ret;
    }
    uapi_vau_matrix_mult(&m2, &m1);
    *m = m2;
    return EXT_SUCCESS;
}
