/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: vau adp affine file
 */

#include "vau_hal_affine.h"
#include "vau_dfx.h"

#define VAU_AFFINE_FLOAT_EXP     0.000001
#define VAU_AFFINE_SHIFT         10
#define VAU_AFFINE_SHIFT_VALUE   (1 << VAU_AFFINE_SHIFT)
#define VAU_AFFINE_RTOTATION90   90
#define VAU_AFFINE_RTOTATION180  180
#define VAU_AFFINE_RTOTATION270  270
#define VAU_AFFINE_RTOTATION360  360

const static td_float g_sin0_90_table[] = {
    0.000000,
    17.871264,   35.737085,   53.592019,    71.430629,    89.247481,   /* 01~05 */
    107.037146,  124.794208,  142.513255,   160.188892,   177.815734,  /* 06~10 */
    195.388411,  212.901571,  230.349880,   247.728021,   265.030702,  /* 11~15 */
    282.252652,  299.388626,  316.433402,   333.381790,   350.228627,  /* 16~20 */
    366.968780,  383.597152,  400.108676,   416.498323,   432.761100,  /* 21~25 */
    448.892054,  464.886272,  480.738880,   496.445051,   512.000000,  /* 26~30 */
    527.398989,  542.637327,  557.710372,   572.613533,   587.342271,  /* 31~35 */
    601.892098,  616.258584,  630.437351,   644.424080,   658.214512,  /* 36~40 */
    671.804446,  685.189741,  698.366321,   711.330171,   724.077344,  /* 41~45 */
    736.603956,  748.906190,  760.980301,   772.822610,   784.429510,  /* 46~50 */
    795.797465,  806.923012,  817.802762,   828.433402,   838.811693,  /* 51~55 */
    848.934474,  858.798662,  868.401250,   877.739316,   886.810013,  /* 56~60 */
    895.610580,  904.138335,  912.390681,   920.365103,   928.059174,  /* 61~65 */
    935.470549,  942.596970,  949.436267,   955.986357,   962.245244,  /* 66~70 */
    968.211021,  973.881873,  979.256070,   984.331977,   989.108046,  /* 71~75 */
    993.582824,  997.754946,  1001.623143,  1005.186236,  1008.443139, /* 76~80 */
    1011.392861, 1014.034502, 1016.367259,  1018.390421,  1020.103371, /* 81~85 */
    1021.505587, 1022.596644, 1023.376207,  1023.844040,  1024.000000  /* 86~90 */
};

static td_float vau_math_sin_256(td_s32 degree)
{
    td_float value;
    td_s32 tmp_degree = degree;

    tmp_degree = tmp_degree % VAU_AFFINE_RTOTATION360;
    if (tmp_degree < 0) {
        tmp_degree = tmp_degree + VAU_AFFINE_RTOTATION360;
    }

    if (tmp_degree < VAU_AFFINE_RTOTATION90) {
        value = g_sin0_90_table[tmp_degree];
    } else if (tmp_degree < VAU_AFFINE_RTOTATION180) {
        value = g_sin0_90_table[VAU_AFFINE_RTOTATION180 - tmp_degree];
    } else if (tmp_degree < VAU_AFFINE_RTOTATION270) {
        value = -g_sin0_90_table[tmp_degree - VAU_AFFINE_RTOTATION180];
    } else {
        value = -g_sin0_90_table[VAU_AFFINE_RTOTATION360 - tmp_degree];
    }

    return value;
}

static td_float vau_math_cos_256(td_s32 degree)
{
    return vau_math_sin_256(degree + VAU_AFFINE_RTOTATION90);
}

static td_void vau_affine_matrix_trans(hal_vau_matrix m, td_u32 length, td_float tx, td_float ty)
{
    hal_vau_matrix b = {
        { 1, 0, tx },
        { 0, 1, ty },
        { 0, 0, 1 }
    };
    hal_vau_matrix t = {0};

    for (td_u32 i = 0; i < length; i++) {
        for (td_u32 j = 0; j < length; j++) {
            t[i][j] = m[i][0] * b[0][j] + m[i][1] * b[1][j] +
            m[i][2] * b[2][j]; /* 2 is idx */
        }
    }

    for (td_u32 i = 0; i < length; i++) {
        for (td_u32 j = 0; j < length; j++) {
            m[i][j] = t[i][j];
        }
    }
}

static td_void vau_affine_matrix_init(hal_vau_affine_cfg *hal_cfg)
{
    /* init pos_matrix */
    hal_cfg->pos_m[0][0] = 1;
    hal_cfg->pos_m[0][1] = 0;
    hal_cfg->pos_m[0][2] = 0; /* 2:index */

    hal_cfg->pos_m[1][0] = 0;
    hal_cfg->pos_m[1][1] = 1;
    hal_cfg->pos_m[1][2] = 0; /* 2:index */

    hal_cfg->pos_m[2][0] = 0; /* 2:index */
    hal_cfg->pos_m[2][1] = 0; /* 2:index */
    hal_cfg->pos_m[2][2] = 1; /* 2:index */

    /* init inv_matrix */
    hal_cfg->inv_m[0][0] = 1;
    hal_cfg->inv_m[0][1] = 0;
    hal_cfg->inv_m[0][2] = 0; /* 2:index */

    hal_cfg->inv_m[1][0] = 0;
    hal_cfg->inv_m[1][1] = 1;
    hal_cfg->inv_m[1][2] = 0; /* 2:index */

    hal_cfg->inv_m[2][0] = 0; /* 2:index */
    hal_cfg->inv_m[2][1] = 0; /* 2:index */
    hal_cfg->inv_m[2][2] = 1; /* 2:index */

    return;
}

static td_void vau_calc_affine_dst_area(hal_vau_affine_cfg *hal_cfg)
{
    td_float xtmp, ytmp, xmax, xmin, ymax, ymin;
    const td_float p1x = 0;
    const td_float p1y = 0;
    td_float src_width  = (td_float)hal_cfg->src_width;
    td_float src_height = (td_float)hal_cfg->src_height;

    td_float p2x = (hal_cfg->pos_m[0][0] * src_width) + hal_cfg->pos_m[0][2];  /* 2:index */
    td_float p2y = (hal_cfg->pos_m[1][0] * src_width) + hal_cfg->pos_m[1][2];  /* 2:index */
    td_float p3x = (hal_cfg->pos_m[0][1] * src_height) + hal_cfg->pos_m[0][2]; /* 2:index */
    td_float p3y = (hal_cfg->pos_m[1][1] * src_height) + hal_cfg->pos_m[1][2]; /* 2:index */
    /* 2:index */
    td_float p4x = (hal_cfg->pos_m[0][0] * src_width + hal_cfg->pos_m[0][1] * src_height) + hal_cfg->pos_m[0][2];
    /* 2:index */
    td_float p4y = (hal_cfg->pos_m[1][0] * src_width + hal_cfg->pos_m[1][1] * src_height) + hal_cfg->pos_m[1][2];

    xtmp = (p2x  < p1x) ? p1x : p2x;
    xtmp = (xtmp > p3x) ? xtmp : p3x;
    xmax = (xtmp > p4x) ? xtmp : p4x;
    xtmp = (p2x  > p1x) ? p1x : p2x;
    xtmp = (xtmp < p3x) ? xtmp : p3x;
    xmin = (xtmp < p4x) ? xtmp : p4x;
    hal_cfg->dst_width = (td_u32)(xmax - xmin);

    ytmp = (p2y  < p1y) ? p1y : p2y;
    ytmp = (ytmp > p3y) ? ytmp : p3y;
    ymax = (ytmp > p4y) ? ytmp : p4y;
    ytmp = (p2y  > p1y) ? p1y : p2y;
    ytmp = (ytmp < p3y) ? ytmp : p3y;
    ymin = (ytmp < p4y) ? ytmp : p4y;
    hal_cfg->dst_height = (td_u32)(ymax - ymin);

    hal_cfg->pos_m[0][2] -= xmin; /* 2:index */
    hal_cfg->pos_m[1][2] -= ymin; /* 2:index */

    return;
}

static td_void vau_generate_inverse_affine(const drv_vau_matrix pos_m, drv_vau_matrix inv_m, td_u32 len)
{
    if (len != HAL_VAU_MATRIX_ARRAY_LENGTH) {
        return;
    }

    td_float det, delta;
    td_float det00, det01, det02;
    td_float det10, det11, det12;
    td_float det20, det21, det22;

    det00 = (pos_m[1][1] * pos_m[2][2] - pos_m[2][1] * pos_m[1][2]); /* 2:index */
    det01 = (pos_m[1][0] * pos_m[2][2] - pos_m[2][0] * pos_m[1][2]); /* 2:index */
    det02 = (pos_m[1][0] * pos_m[2][1] - pos_m[2][0] * pos_m[1][1]); /* 2:index */

    det10 = (pos_m[0][1] * pos_m[2][2] - pos_m[2][1] * pos_m[0][2]); /* 2:index */
    det11 = (pos_m[0][0] * pos_m[2][2] - pos_m[2][0] * pos_m[0][2]); /* 2:index */
    det12 = (pos_m[0][0] * pos_m[2][1] - pos_m[2][0] * pos_m[0][1]); /* 2:index */

    det20 = (pos_m[0][1] * pos_m[1][2] - pos_m[1][1] * pos_m[0][2]); /* 2:index */
    det21 = (pos_m[0][0] * pos_m[1][2] - pos_m[1][0] * pos_m[0][2]); /* 2:index */
    det22 = (pos_m[0][0] * pos_m[1][1] - pos_m[1][0] * pos_m[0][1]); /* 2:index */

    det = pos_m[2][0] * det20 - pos_m[2][1] * det21 + pos_m[2][2] * det22; /* 2:index */
    if ((det > -VAU_AFFINE_FLOAT_EXP) && (det < VAU_AFFINE_FLOAT_EXP)) {
        vau_dbg("det is zero, rot matrix is invalid!");
        det = VAU_AFFINE_FLOAT_EXP;
    }

    inv_m[0][0] =  (det00 * VAU_AFFINE_SHIFT_VALUE) / det;
    inv_m[0][1] = -(det10 * VAU_AFFINE_SHIFT_VALUE) / det;
    inv_m[0][2] =  (det20 * VAU_AFFINE_SHIFT_VALUE) / det; /* 2:index */

    inv_m[1][0] = -(det01 * VAU_AFFINE_SHIFT_VALUE) / det;
    inv_m[1][1] =  (det11 * VAU_AFFINE_SHIFT_VALUE) / det;
    inv_m[1][2] = -(det21 * VAU_AFFINE_SHIFT_VALUE) / det; /* 2:index */

    inv_m[2][0] =  (det02 * VAU_AFFINE_SHIFT_VALUE) / det; /* 2:index */
    inv_m[2][1] = -(det12 * VAU_AFFINE_SHIFT_VALUE) / det; /* 2:index */
    inv_m[2][2] =  (det22 * VAU_AFFINE_SHIFT_VALUE) / det; /* 2:index */

    if ((fabs(pos_m[0][0] * pos_m[0][0] + pos_m[0][1] * pos_m[0][1] - 1.0) <= (1e-6)) &&
        (fabs(pos_m[1][0] * pos_m[1][0] + pos_m[1][1] * pos_m[1][1] - 1.0) <= (1e-6))) {
        /* rotate center position compensation */
        delta = (td_float)VAU_AFFINE_SHIFT_VALUE;
        inv_m[0][2] += ((inv_m[0][0] + inv_m[0][1]) - delta) / 2.0f; /* 2:index */
        inv_m[1][2] += ((inv_m[1][0] + inv_m[1][1]) - delta) / 2.0f; /* 2:index */
        inv_m[2][2] += (inv_m[2][0] + inv_m[2][1]) / 2.0f;           /* 2:index */
    }

    return;
}

static td_void vau_generate_affine_coef(const drv_vau_affine_attr *affine_attr, drv_vau_matrix pos_m, td_u32 len)
{
    if (len != HAL_VAU_MATRIX_ARRAY_LENGTH) {
        return;
    }

    switch (affine_attr->mode) {
        case DRV_VAU_AFFINE_MODE_TRANSLATE:
            pos_m[0][2] = affine_attr->translate.x; /* 2:index */
            pos_m[1][2] = affine_attr->translate.y; /* 2:index */
            break;
        case DRV_VAU_AFFINE_MODE_ZOOM:
            pos_m[0][0] = affine_attr->zoom.x_multiplier;
            pos_m[1][1] = affine_attr->zoom.y_multiplier;
            break;
        case DRV_VAU_AFFINE_MODE_ROTATE:
            pos_m[0][0] =  vau_math_cos_256(affine_attr->rotate.degree);
            pos_m[0][1] = -vau_math_sin_256(affine_attr->rotate.degree);
            pos_m[1][0] =  vau_math_sin_256(affine_attr->rotate.degree);
            pos_m[1][1] =  vau_math_cos_256(affine_attr->rotate.degree);
            break;
        case DRV_VAU_AFFINE_MODE_REFLECT:
            if (affine_attr->reflect.mode == DRV_VAU_REFLECT_MODE_X) {
                pos_m[1][1] = -1;
            } else if (affine_attr->reflect.mode == DRV_VAU_REFLECT_MODE_Y) {
                pos_m[0][0] = -1;
            } else if (affine_attr->reflect.mode == DRV_VAU_REFLECT_MODE_XY) {
                pos_m[0][0] = -1;
                pos_m[1][1] = -1;
            }
            break;
        case DRV_VAU_AFFINE_MODE_SHEARING:
            pos_m[0][1] = affine_attr->shearing.x;
            pos_m[1][0] = affine_attr->shearing.y;
            break;
        default:
            pos_m[0][0] = affine_attr->matrix[0][0];
            pos_m[0][1] = affine_attr->matrix[0][1];
            pos_m[0][2] = affine_attr->matrix[0][2]; /* 2:index */

            pos_m[1][0] = affine_attr->matrix[1][0];
            pos_m[1][1] = affine_attr->matrix[1][1];
            pos_m[1][2] = affine_attr->matrix[1][2]; /* 2:index */

            pos_m[2][0] = affine_attr->matrix[2][0]; /* 2:index */
            pos_m[2][1] = affine_attr->matrix[2][1]; /* 2:index */
            pos_m[2][2] = affine_attr->matrix[2][2]; /* 2:index */
    }

    return;
}

td_void vau_hal_affine_config(vau_hardware_node *node, const drv_vau_surface *src_surface,
                              const drv_vau_surface *dst_surface)
{
    hal_vau_affine_cfg hal_cfg = {0};

    vau_affine_matrix_init(&hal_cfg);

    vau_generate_affine_coef(&src_surface->attr.affine, hal_cfg.pos_m, HAL_VAU_MATRIX_ARRAY_LENGTH);

    hal_cfg.src_width  = src_surface->rect.width;
    hal_cfg.src_height = src_surface->rect.height;

    if (src_surface->attr.affine.mode == DRV_VAU_AFFINE_MODE_REFLECT) {
        vau_calc_affine_dst_area(&hal_cfg);
    }

    if (!vau_dfx_check_mask(VAU_DBG_MASK_AFFINE_TRANS_DISABLE)) {
        vau_affine_matrix_trans(hal_cfg.pos_m, HAL_VAU_MATRIX_ARRAY_LENGTH,
            -1 * src_surface->rect.x, -1 * src_surface->rect.y);
    } else {
        vau_dbg("disable matrix_trans\n");
    }
    vau_generate_inverse_affine(hal_cfg.pos_m, hal_cfg.inv_m, HAL_VAU_MATRIX_ARRAY_LENGTH);

    hal_cfg.dst_width  = dst_surface->rect.width;
    hal_cfg.dst_height = dst_surface->rect.height;

    /* affine only support ARGB8888 */
    hal_cfg.src_fmt = HAL_VAU_FMT_ARGB8888;
    hal_cfg.dst_fmt = HAL_VAU_FMT_ARGB8888;
    hal_cfg.transform_mode = HAL_VAU_TRANSFORM_MODE_BILINEAR;
    hal_cfg.pattern_mode = (hal_vau_pattern_mode)src_surface->attr.pattern.mode;
    hal_cfg.pattern_argb = src_surface->attr.pattern.argb;

    vau_hal_node_set_affine_info(node, &hal_cfg);
}
