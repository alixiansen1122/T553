/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: Description: openvg context adapter source file
 * Author: @CompanyNameTag
 * Create: 2021-07-20
 */

#include "VG/openvg.h"
#include "vg_math.h"
#include "vg_context.h"
#include "vg_common.h"

static ext_vau_matrix *get_current_matrix(vg_context *context)
{
    switch (context->matrix_mode) {
        case VG_MATRIX_PATH_USER_TO_SURFACE:
            return &context->path_user_to_surface;

        case VG_MATRIX_IMAGE_USER_TO_SURFACE:
            return &context->image_user_to_surface;

        case VG_MATRIX_FILL_PAINT_TO_USER:
            return &context->fill_paint_to_user;

        case VG_MATRIX_STROKE_PAINT_TO_USER:
            return &context->stroke_paint_to_user;

        case VG_MATRIX_GLYPH_USER_TO_SURFACE:
            return &context->glyph_user_to_surface;

        default:
            vg_err("error matrix mode:0x%x\n", context->matrix_mode);
            return &context->image_user_to_surface;
    }
}

static td_void matrix_degenerate_to_affine(VGMatrixMode matrix_mode, ext_vau_matrix *m)
{
    if (matrix_mode != VG_MATRIX_IMAGE_USER_TO_SURFACE) {
        m->matrix[2][0] = 0; /* index 2 0 */
        m->matrix[2][1] = 0; /* index 2 1 */
        m->matrix[2][2] = 1; /* index 2 2 */
    }
}

static td_void load_matrix_from_float_values(ext_vau_matrix *o, const VGfloat *m)
{
    for (int i = 0; i < 3; i++) { /* 3 * 3 matrix */
        for (int j = 0; j < 3; j++) { /* 3 * 3 matrix */
            o->matrix[i][j] = m[j * 3 + i]; /* 3 * 3 matrix */
        }
    }
}

VG_API_CALL void VG_API_ENTRY vgLoadIdentity(void) VG_API_EXIT
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    ext_vau_matrix *m = get_current_matrix(context);

    vg_matrix_identity(m);

    vg_return(VG_NO_RETVAL);
}

VG_API_CALL void VG_API_ENTRY vgLoadMatrix(const VGfloat *m) VG_API_EXIT
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    ext_vau_matrix *c = get_current_matrix(context);

    load_matrix_from_float_values(c, m);

    matrix_degenerate_to_affine(context->matrix_mode, c);

    vg_return(VG_NO_RETVAL);
}

VG_API_CALL void VG_API_ENTRY vgGetMatrix(VGfloat *m) VG_API_EXIT
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    ext_vau_matrix *c = get_current_matrix(context);

    for (int i = 0; i < 3; i++) { /* 3 * 3 matrix */
        for (int j = 0; j < 3; j++) { /* 3 * 3 matrix */
            m[j * 3 + i] = c->matrix[i][j]; /* 3 * 3 matrix */
        }
    }

    vg_return(VG_NO_RETVAL);
}

VG_API_CALL void VG_API_ENTRY vgMultMatrix(const VGfloat *m) VG_API_EXIT
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    ext_vau_matrix n;
    ext_vau_matrix *c = get_current_matrix(context);

    load_matrix_from_float_values(&n, m);

    matrix_degenerate_to_affine(context->matrix_mode, &n);

    vg_matrix_mult(c, &n);

    matrix_degenerate_to_affine(context->matrix_mode, c);

    vg_return(VG_NO_RETVAL);
}

VG_API_CALL void VG_API_ENTRY vgTranslate(VGfloat tx, VGfloat ty) VG_API_EXIT
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    ext_vau_matrix *c = get_current_matrix(context);

    vg_matrix_trans(c, tx, ty);

    matrix_degenerate_to_affine(context->matrix_mode, c);

    vg_return(VG_NO_RETVAL);
}

VG_API_CALL void VG_API_ENTRY vgScale(VGfloat sx, VGfloat sy) VG_API_EXIT
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    ext_vau_matrix *c = get_current_matrix(context);

    vg_matrix_scale(c, sx, sy);

    matrix_degenerate_to_affine(context->matrix_mode, c);

    vg_return(VG_NO_RETVAL);
}

VG_API_CALL void VG_API_ENTRY vgShear(VGfloat shx, VGfloat shy) VG_API_EXIT
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    ext_vau_matrix *c = get_current_matrix(context);

    vg_matrix_shear(c, shx, shy);

    matrix_degenerate_to_affine(context->matrix_mode, c);

    vg_return(VG_NO_RETVAL);
}

VG_API_CALL void VG_API_ENTRY vgRotate(VGfloat angle) VG_API_EXIT
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    ext_vau_matrix *c = get_current_matrix(context);

    vg_matrix_rotate(c, angle);

    matrix_degenerate_to_affine(context->matrix_mode, c);

    vg_return(VG_NO_RETVAL);
}
