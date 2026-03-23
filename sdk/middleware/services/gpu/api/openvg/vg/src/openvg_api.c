/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: openvg api file
 * Author: @CompanyNameTag
 * Create: 2021-07-20
 */

#include <stdlib.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "vg_common.h"
#include "vg_paint.h"
#include "vg_path.h"
#include "vg_context.h"
#include "hdi_hardware_capability.h"

#define RI_MAX_DASH_COUNT 12
#define RI_MAX_SCISSOR_RECTANGLES 1

static VGErrorCode image_check_is_invalid_fmt(VGImageFormat fmt)
{
    uapi_unused(fmt);
    return VG_NO_ERROR;
}

VG_API_CALL VGErrorCode VG_API_ENTRY vgGetError(void) VG_API_EXIT
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_CONTEXT_ERROR);

    VGErrorCode error = context->error;
    context->error = VG_NO_ERROR;

    vg_return(error);
}

void vgSetf(VGParamType type, VGfloat value)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    switch (type) {
        case VG_STROKE_LINE_WIDTH:
            context->stroke_line_width = value;
            break;
        case VG_STROKE_MITER_LIMIT:
            context->stroke_miter_limit = value;
            break;
        case VG_STROKE_DASH_PHASE:
            context->stroke_dash_phase = value;
            break;
        default:
            context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
            vg_err("invalid type:0x%x", type);
            break;
    }

    vg_return(VG_NO_RETVAL);
}

void vgSeti(VGParamType type, VGint value)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    switch (type) {
        case VG_MATRIX_MODE:
            if (value < VG_MATRIX_PATH_USER_TO_SURFACE || value > VG_MATRIX_GLYPH_USER_TO_SURFACE) {
                goto illigal_para;
            }
            context->matrix_mode = value;
            break;
        case VG_FILL_RULE:
            if (value < VG_EVEN_ODD || value > VG_NON_ZERO) {
                goto illigal_para;
            }
            context->fill_rule = value;
            break;
        case VG_IMAGE_QUALITY:
            if (value < VG_IMAGE_QUALITY_NONANTIALIASED || value > VG_IMAGE_QUALITY_BETTER) {
                goto illigal_para;
            }
            context->image_quality = value;
            break;
        case VG_RENDERING_QUALITY:
            if (value < VG_RENDERING_QUALITY_NONANTIALIASED || value > VG_RENDERING_QUALITY_BETTER) {
                goto illigal_para;
            }
            context->render_quality = value;
            break;
        case VG_BLEND_MODE:
            if (value < VG_BLEND_SRC || value > VG_BLEND_ADDITIVE) {
                goto illigal_para;
            }
            context->blend_mode = value;
            break;
        case VG_IMAGE_MODE:
            if (value < VG_DRAW_IMAGE_NORMAL || value > VG_DRAW_IMAGE_STENCIL) {
                goto illigal_para;
            }
            context->image_mode = value;
            break;
        case VG_COLOR_TRANSFORM:
            context->color_transform = value;
            break;
        case VG_STROKE_CAP_STYLE:
            if (value < VG_CAP_BUTT || value > VG_CAP_SQUARE) {
                goto illigal_para;
            }
            context->stroke_cap_style = value;
            break;
        case VG_STROKE_JOIN_STYLE:
            if (value < VG_JOIN_MITER || value > VG_JOIN_BEVEL) {
                goto illigal_para;
            }
            context->stroke_join_style = value;
            break;
        case VG_STROKE_DASH_PHASE_RESET:
            context->stroke_dash_phase_reset = value;
            break;
        case VG_MASKING:
            context->masking = value;
            break;
        case VG_SCISSORING:
            context->scissoring = value;
            break;
        default:
            vg_err("invalid type:0x%x", type);
            context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
            break;
    }

    vg_return(VG_NO_RETVAL);

illigal_para:
    context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
    vg_return(VG_NO_RETVAL);
}

static VGErrorCode vg_setfv_color_transfrom_value(vg_context *context, VGint count, const VGfloat *values)
{
    if (count != 8 || values == NULL) { /* VG_COLOR_TRANSFORM_VALUES has 8 float values */
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    /* VG_COLOR_TRANSFORM_VALUES has 8 float values */
    for (int i = 0; i < 8; i++) {
        context->color_transform_values[i] = values[i];
    }
    return VG_NO_ERROR;
}

static VGErrorCode vg_setfv_dash_pattern(vg_context *context, VGint count, const VGfloat *values)
{
    if (context->stroke_dash_pattern != NULL) {
        free(context->stroke_dash_pattern);
        context->stroke_dash_pattern = NULL;
    }
    context->stroke_dash_pattern_num = (td_u8)(count < RI_MAX_DASH_COUNT ? count : RI_MAX_DASH_COUNT);
    if (context->stroke_dash_pattern_num == 0 || values == NULL) {
        return VG_NO_ERROR;
    }
    context->stroke_dash_pattern = (VGfloat*)malloc(sizeof(VGfloat) * context->stroke_dash_pattern_num);
    if (context->stroke_dash_pattern == NULL) {
        context->stroke_dash_pattern_num = 0;
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    for (int i = 0; i < context->stroke_dash_pattern_num; i++) {
        context->stroke_dash_pattern[i] = values[i];
    }
    return VG_NO_ERROR;
}

static VGErrorCode vg_setfv_fill_color(vg_context *context, VGint count, const VGfloat *values)
{
    if (count != 4 || values == NULL) { /* VG_TILE_FILL_COLOR has 4 float values */
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    /* VG_TILE_FILL_COLOR has 4 float values */
    for (int i = 0; i < 4; i++) {
        context->tile_fill_color[i] = values[i];
    }
    return VG_NO_ERROR;
}

static VGErrorCode vg_setfv_clear_color(vg_context *context, VGint count, const VGfloat *values)
{
    if (count != 4 || values == NULL) { /* VG_CLEAR_COLOR has 4 float values */
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    /* VG_CLEAR_COLOR has 4 float values */
    for (int i = 0; i < 4; i++) {
        context->clear_color[i] = values[i];
    }
    return VG_NO_ERROR;
}

void vgSetfv(VGParamType type, VGint count, const VGfloat *values)
{
    VGErrorCode ret;
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    switch (type) {
        case VG_COLOR_TRANSFORM_VALUES:
            ret = vg_setfv_color_transfrom_value(context, count, values);
            break;
        case VG_STROKE_DASH_PATTERN:
            ret = vg_setfv_dash_pattern(context, count, values);
            break;
        case VG_TILE_FILL_COLOR:
            ret = vg_setfv_fill_color(context, count, values);
            break;
        case VG_CLEAR_COLOR:
            ret = vg_setfv_clear_color(context, count, values);
            break;
        default:
            ret = VG_ILLEGAL_ARGUMENT_ERROR;
            break;
    }

    if (ret != VG_NO_ERROR) {
        vg_err("set:0x%x, count:%d failed", type, count);
        context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
    }

    vg_return(VG_NO_RETVAL);
}

void vgSetiv(VGParamType type, VGint count, const VGint *values)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    if (count <= 0 || values == NULL) {
        goto illigal_para;
    }

    switch (type) {
        case VG_SCISSOR_RECTS:
            if ((count % 4) != 0) { /* VG_SCISSOR_RECTS is mult of 4 */
                goto illigal_para;
            }
            if (context->scissor != NULL) {
                free(context->scissor);
            }
            /* VG_SCISSOR_RECTS is mult of 4 */
            context->scissor_num = (td_u8)(count / 4 < RI_MAX_SCISSOR_RECTANGLES ?
                                   (count / 4) : RI_MAX_SCISSOR_RECTANGLES); /* VG_SCISSOR_RECTS is mult of 4 */
            context->scissor = (ext_rect*)malloc(sizeof(ext_rect) * context->scissor_num);
            if (context->scissor == NULL) {
                context->scissor_num = 0;
                goto illigal_para;
            }
            /* VG_SCISSOR_RECTS is mult of 4 */
            for (int i = 0; i < context->scissor_num; i += 4) {
                context->scissor[i].x = values[i]; /* index 0 */
                context->scissor[i].y = values[i + 1]; /* index 1 */
                context->scissor[i].width = (td_u32)values[i + 2]; /* index 2 */
                context->scissor[i].height = (td_u32)values[i + 3]; /* index 3 */
            }
            break;
        default:
            vg_err("invalid type:0x%x", type);
            context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
            break;
    }

    vg_return(VG_NO_RETVAL);

illigal_para:
    context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
    vg_return(VG_NO_RETVAL);
}

VGfloat vgGetf(VGParamType type)
{
    egl *e = NULL;
    VGfloat value = 0.0f;
    vg_context *context = NULL;
    vg_get_context(value);

    switch (type) {
        case VG_STROKE_LINE_WIDTH:
            vg_return(context->stroke_line_width);
        case VG_STROKE_MITER_LIMIT:
            vg_return(context->stroke_miter_limit);
        case VG_STROKE_DASH_PHASE:
            vg_return(context->stroke_dash_phase);
        case VG_MAX_GAUSSIAN_STD_DEVIATION:
            vg_return(HARDWARE_MAX_GAUSSIAN_STD_DEVIATION);
        case VG_MAX_FLOAT:
            vg_return(HARDWARE_MAX_FLOAT);
        default:
            vg_err("invalid type:0x%x", type);
            context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
            break;
    }

    vg_return(value);
}

VGint vgGeti(VGParamType type)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(0);

    switch (type) {
        case VG_MATRIX_MODE:
            vg_return(context->matrix_mode);
        case VG_FILL_RULE:
            vg_return(context->fill_rule);
        case VG_IMAGE_QUALITY:
            vg_return(context->image_quality);
        case VG_RENDERING_QUALITY:
            vg_return(context->render_quality);
        case VG_BLEND_MODE:
            vg_return(context->blend_mode);
        case VG_IMAGE_MODE:
            vg_return(context->image_mode);
        case VG_COLOR_TRANSFORM:
            vg_return(context->color_transform);
        case VG_STROKE_CAP_STYLE:
            vg_return(context->stroke_cap_style);
        case VG_STROKE_JOIN_STYLE:
            vg_return(context->stroke_join_style);
        case VG_STROKE_DASH_PHASE_RESET:
            vg_return(context->stroke_dash_phase_reset);
        case VG_SCISSORING:
            vg_return(context->scissoring);
        case VG_MAX_SCISSOR_RECTS:
            vg_return(HARDWARE_MAX_SCISSOR_RECTS);
        case VG_MAX_DASH_COUNT:
            vg_return(HARDWARE_MAX_DASH_COUNT);
        case VG_MAX_COLOR_RAMP_STOPS:
            vg_return(HARDWARE_MAX_COLOR_RAMP_STOPS);
        case VG_MAX_IMAGE_WIDTH:
            vg_return(HARDWARE_MAX_IMAGE_WIDTH);
        case VG_MAX_IMAGE_HEIGHT:
            vg_return(HARDWARE_MAX_IMAGE_HEIGHT);
        case VG_MAX_IMAGE_PIXELS:
            vg_return(HARDWARE_MAX_IMAGE_PIXELS);
        case VG_MAX_IMAGE_BYTES:
            vg_return(HARDWARE_MAX_IMAGE_BYTES);
        default:
            vg_err("invalid type:0x%x", type);
            context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
            break;
    }

    vg_return(0);
}

VGint vgGetVectorSize(VGParamType type)
{
    egl *e = NULL;
    VGint value = 0;
    vg_context *context = NULL;
    vg_get_context(value);

    switch (type) {
        case VG_SCISSOR_RECTS:
            value = context->scissor_num * 4; /* VG_SCISSOR_RECTS is mult of 4 */
            break;
        case VG_COLOR_TRANSFORM_VALUES:
            value = 8; /* VG_COLOR_TRANSFORM_VALUES has 8 float values */
            break;
        case VG_STROKE_DASH_PATTERN:
            value = context->stroke_dash_pattern_num;
            break;
        case VG_TILE_FILL_COLOR:
            value = 4; /* VG_TILE_FILL_COLOR has 4 float values */
            break;
        case VG_CLEAR_COLOR:
            value = 4; /* VG_CLEAR_COLOR has 4 float values */
            break;
        case VG_GLYPH_ORIGIN:
            value = 2; /* VG_GLYPH_ORIGIN has 2 float values */
            break;
        default:
            vg_err("invalid type:0x%x", type);
            context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
            break;
    }

    vg_return(value);
}

void vgGetfv(VGParamType type, VGint count, VGfloat *values)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    if (count <= 0 || values == NULL) {
        vg_err("invalid parm count = %d", count);
        goto illigal_para;
    }

    switch (type) {
        case VG_COLOR_TRANSFORM_VALUES:
            if (count != 8) { /* VG_COLOR_TRANSFORM_VALUES has 8 float values */
                goto illigal_para;
            }
            for (int i = 0; i < 8; i++) { /* VG_COLOR_TRANSFORM_VALUES has 8 float values */
                values[i] = context->color_transform_values[i];
            }
            break;
        case VG_STROKE_DASH_PATTERN:
            for (int i = 0; i < context->stroke_dash_pattern_num; i++) {
                values[i] = context->stroke_dash_pattern[i];
            }
            break;
        case VG_TILE_FILL_COLOR:
            if (count != 4) { /* VG_TILE_FILL_COLOR has 4 float values */
                goto illigal_para;
            }
            for (int i = 0; i < 4; i++) { /* VG_TILE_FILL_COLOR has 4 float values */
                values[i] = context->tile_fill_color[i];
            }
            break;
        case VG_CLEAR_COLOR:
            if (count != 4) { /* VG_CLEAR_COLOR has 4 float values */
                goto illigal_para;
            }
            for (int i = 0; i < 4; i++) { /* VG_CLEAR_COLOR has 4 float values */
                values[i] = context->clear_color[i];
            }
            break;
        case VG_GLYPH_ORIGIN:
            if (count != 2) { /* VG_GLYPH_ORIGIN has 2 float values */
                goto illigal_para;
            }
            values[0] = context->glyph_origin[0];
            values[1] = context->glyph_origin[1];
            break;
        default:
            vg_err("invalid type:0x%x", type);
            context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
            break;
    }

    vg_return(VG_NO_RETVAL);

illigal_para:
    context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
    vg_return(VG_NO_RETVAL);
}

void vgGetiv(VGParamType type, VGint count, VGint *values)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    if (count <= 0 || values == NULL) {
        vg_err("invalid parm count = %d", count);
        goto illigal_para;
    }

    switch (type) {
        case VG_SCISSOR_RECTS:
            if ((count % 4) != 0) { /* VG_SCISSOR_RECTS is mult of 4 */
                goto illigal_para;
            }
            for (int i = 0; i < context->scissor_num; i += 4) { /* VG_SCISSOR_RECTS is mult of 4 */
                values[i] = context->scissor[i].x; /* index 0 */
                values[i + 1] = context->scissor[i].y; /* index 1 */
                values[i + 2] = (VGint)context->scissor[i].width; /* index 2 */
                values[i + 3] = (VGint)context->scissor[i].height; /* index 3 */
            }
            break;
        default:
            vg_err("invalid type:0x%x", type);
            context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
            break;
    }

    vg_return(VG_NO_RETVAL);

illigal_para:
    context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
    vg_return(VG_NO_RETVAL);
}

void vgSetParameterf(VGHandle object, VGint paramType, VGfloat value)
{
    egl *e = NULL;
    VGErrorCode ret = VG_NO_ERROR;
    vg_context *context = NULL;

    uapi_unused(paramType);
    uapi_unused(value);

    vg_get_context(VG_NO_RETVAL);

    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)object);

    switch (resource_type) {
        case VG_TYPE_RESOURCE_IMAGE:
        case VG_TYPE_RESOURCE_FONT:
            // no param need to set
            ret = VG_ILLEGAL_ARGUMENT_ERROR;
            break;
        case VG_TYPE_RESOURCE_PAINT:
            break;
        default:
            vg_err("invalid object");
            ret = VG_BAD_HANDLE_ERROR;
            break;
    }
    if (ret != VG_NO_ERROR) {
        context->set_error(context, ret);
    }
    vg_return(VG_NO_RETVAL);
}

void vgSetParameteri(VGHandle object, VGint paramType, VGint value)
{
    egl *e = NULL;
    VGErrorCode ret = VG_NO_ERROR;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)object);

    switch (resource_type) {
        case VG_TYPE_RESOURCE_IMAGE: {
            vg_image *image = (vg_image *)(uintptr_t)object;
            ret = image->seti(image, paramType, value);
            break;
        }
        case VG_TYPE_RESOURCE_FONT:
            // no param need to set
            ret = VG_ILLEGAL_ARGUMENT_ERROR;
            break;
        case VG_TYPE_RESOURCE_PAINT: {
            vg_paint *paint = (vg_paint *)(uintptr_t)object;
            ret = paint->seti(paint, paramType, value);
            break;
        }
        default:
            vg_err("invalid object");
            ret = VG_BAD_HANDLE_ERROR;
            break;
    }
    if (ret != VG_NO_ERROR) {
        context->set_error(context, ret);
    }
    vg_return(VG_NO_RETVAL);
}

void vgSetParameterfv(VGHandle object, VGint paramType, VGint count, const VGfloat *values)
{
    egl *e = NULL;
    VGErrorCode ret = VG_NO_ERROR;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)object);

    switch (resource_type) {
        case VG_TYPE_RESOURCE_IMAGE: {
            vg_image *image = (vg_image *)(uintptr_t)object;
            ret = image->setfv(image, paramType, count, values);
            break;
        }
        case VG_TYPE_RESOURCE_FONT:
            // no param need to set
            ret = VG_ILLEGAL_ARGUMENT_ERROR;
            break;
        case VG_TYPE_RESOURCE_PAINT: {
            vg_paint *paint = (vg_paint *)(uintptr_t)object;
            ret = paint->setfv(paint, paramType, count, values);
            break;
        }
        default:
            vg_err("invalid object");
            ret = VG_BAD_HANDLE_ERROR;
            break;
    }
    if (ret != VG_NO_ERROR) {
        context->set_error(context, ret);
    }
    vg_return(VG_NO_RETVAL);
}

void vgSetParameteriv(VGHandle object, VGint paramType, VGint count, const VGint *values)
{
    egl *e = NULL;
    VGErrorCode ret = VG_NO_ERROR;
    vg_context *context = NULL;

    uapi_unused(paramType);
    uapi_unused(count);
    uapi_unused(values);

    vg_get_context(VG_NO_RETVAL);

    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)object);

    switch (resource_type) {
        case VG_TYPE_RESOURCE_IMAGE:
        case VG_TYPE_RESOURCE_PATH:
        case VG_TYPE_RESOURCE_FONT:
            // no param need to set
            ret = VG_ILLEGAL_ARGUMENT_ERROR;
            break;
        case VG_TYPE_RESOURCE_PAINT:
            // need do something
            break;
        default:
            vg_err("invalid object");
            ret = VG_BAD_HANDLE_ERROR;
            break;
    }
    if (ret != VG_NO_ERROR) {
        context->set_error(context, ret);
    }
    vg_return(VG_NO_RETVAL);
}

VGfloat vgGetParameterf(VGHandle object, VGint paramType)
{
    egl *e = NULL;
    VGErrorCode ret = VG_NO_ERROR;
    vg_context *context = NULL;

    uapi_unused(paramType);

    vg_get_context(0);
    VGfloat value = 0.0f;

    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)object);

    switch (resource_type) {
        case VG_TYPE_RESOURCE_IMAGE:
            ret = VG_ILLEGAL_ARGUMENT_ERROR;
            break;
        case VG_TYPE_RESOURCE_PATH:
        case VG_TYPE_RESOURCE_FONT:
        case VG_TYPE_RESOURCE_PAINT:
            break;
        default:
            vg_err("invalid object");
            ret = VG_BAD_HANDLE_ERROR;
            break;
    }
    if (ret != VG_NO_ERROR) {
        context->set_error(context, ret);
    }
    vg_return(value);
}

VGint vgGetParameteri(VGHandle object, VGint paramType)
{
    egl *e = NULL;
    VGErrorCode ret = VG_NO_ERROR;
    vg_context *context = NULL;
    VGint value = 0;
    vg_get_context(0);

    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)object);

    switch (resource_type) {
        case VG_TYPE_RESOURCE_IMAGE: {
            vg_image *image = (vg_image *)(uintptr_t)object;
            ret = image->geti(image, paramType, &value);
            break;
        }
        case VG_TYPE_RESOURCE_PATH: {
            vg_path *path = (vg_path *)(uintptr_t)object;
            ret = path->geti(path, paramType, &value);
            break;
        }
        case VG_TYPE_RESOURCE_FONT:
            break;
        case VG_TYPE_RESOURCE_PAINT: {
            vg_paint *paint = (vg_paint *)(uintptr_t)object;
            ret = paint->geti(paint, paramType, &value);
            break;
        }
        default:
            vg_err("invalid object");
            ret = VG_BAD_HANDLE_ERROR;
            break;
    }
    if (ret != VG_NO_ERROR) {
        context->set_error(context, ret);
    }
    vg_return(value);
}

VGint vgGetParameterVectorSize(VGHandle object, VGint paramType)
{
    egl *e = NULL;
    VGErrorCode ret = VG_NO_ERROR;
    vg_context *context = NULL;
    vg_get_context(0);

    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)object);
    VGint count = 0;

    switch (resource_type) {
        case VG_TYPE_RESOURCE_IMAGE: {
            vg_image *image = (vg_image *)(uintptr_t)object;
            ret = image->get_size(image, paramType, &count);
            break;
        }
        case VG_TYPE_RESOURCE_FONT:
            break;
        case VG_TYPE_RESOURCE_PAINT: {
            vg_paint *paint = (vg_paint *)(uintptr_t)object;
            ret = paint->get_size(paint, paramType, &count);
            break;
        }
        default:
            vg_err("invalid object");
            ret = VG_BAD_HANDLE_ERROR;
            break;
    }
    if (ret != VG_NO_ERROR) {
        context->set_error(context, ret);
    }
    vg_return(count);
}

void vgGetParameterfv(VGHandle object, VGint paramType, VGint count, VGfloat *values)
{
    egl *e = NULL;
    VGErrorCode ret = VG_NO_ERROR;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);
    vg_if_error_return(object == VG_INVALID_HANDLE, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);
    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)object);

    switch (resource_type) {
        case VG_TYPE_RESOURCE_IMAGE: {
            vg_image *image = (vg_image *)(uintptr_t)object;
            ret = image->getfv(image, paramType, count, values);
            break;
        }
        case VG_TYPE_RESOURCE_FONT:
            break;
        case VG_TYPE_RESOURCE_PAINT: {
            vg_paint *paint = (vg_paint *)(uintptr_t)object;
            ret = paint->getfv(paint, paramType, count, values);
            break;
        }
        default:
            vg_err("invalid object");
            ret = VG_BAD_HANDLE_ERROR;
            break;
    }
    if (ret != VG_NO_ERROR) {
        context->set_error(context, ret);
    }
    vg_return(VG_NO_RETVAL);
}

void vgGetParameteriv(VGHandle object, VGint paramType, VGint count, VGint *values)
{
    egl *e = NULL;
    VGErrorCode ret = VG_NO_ERROR;
    vg_context *context = NULL;

    uapi_unused(paramType);
    uapi_unused(values);
    uapi_unused(count);

    vg_get_context(VG_NO_RETVAL);
    vg_if_error_return(object == VG_INVALID_HANDLE, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)object);

    switch (resource_type) {
        case VG_TYPE_RESOURCE_IMAGE:
        case VG_TYPE_RESOURCE_PATH:
        case VG_TYPE_RESOURCE_FONT:
        case VG_TYPE_RESOURCE_PAINT:
            ret = VG_ILLEGAL_ARGUMENT_ERROR;
            break;
        default:
            vg_err("invalid object");
            ret = VG_BAD_HANDLE_ERROR;
            break;
    }
    if (ret != VG_NO_ERROR) {
        context->set_error(context, ret);
    }
    vg_return(VG_NO_RETVAL);
}

/* Images */
VGImage vgCreateImage(VGImageFormat format, VGint width, VGint height,
    VGbitfield allowedQuality)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_INVALID_HANDLE);

    if (image_check_is_invalid_fmt(format) != VG_NO_ERROR) {
        vg_err("invalid_fmt: %u", format);
        goto illigal_para;
    }
    if (width < 0 || height < 0) {
        vg_err("width:%d, height:%d illigal", width, height);
        goto illigal_para;
    }

    vg_image *image = new_object(vg_image, format, width, height, allowedQuality);
    if (image == NULL) {
        context->set_error(context, VG_OUT_OF_MEMORY_ERROR);
        vg_err("new object failed");
        vg_return(VG_INVALID_HANDLE);
    }
    context->image_manager->add_resource(context->image_manager, (vg_resource *)(uintptr_t)image);

    vg_return((VGImage)(uintptr_t)image);

illigal_para:
    context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
    vg_return(VG_INVALID_HANDLE);
}

void vgDestroyImage(VGImage image)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);
    vg_if_error_return(image == VG_INVALID_HANDLE, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

    VGboolean ret = context->image_manager->is_valid(context->image_manager, (vg_resource *)(uintptr_t)image);
    if (ret != VG_TRUE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("image is not valid");
        vg_return(VG_NO_RETVAL);
    }
    vg_image *m = (vg_image *)(uintptr_t)image;
    if (m->parent != NULL) {
        if (remove_ref(m->parent) == 0) {
            delete_object(m->parent);
        }
    }
    context->image_manager->remove_resource(context->image_manager, (vg_resource *)(uintptr_t)image);
    vg_return(VG_NO_RETVAL);
}

void vgClearImage(VGImage image,
    VGint x, VGint y, VGint width, VGint height)
{
    egl *e = NULL;
    vg_context *context = NULL;

    uapi_unused(x);
    uapi_unused(y);
    uapi_unused(width);
    uapi_unused(height);

    vg_get_context(VG_NO_RETVAL);
    vg_if_error_return(image == VG_INVALID_HANDLE, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)image);
    if (resource_type != VG_TYPE_RESOURCE_IMAGE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not image object");
        vg_return(VG_NO_RETVAL);
    }
    // need to do something
    vg_return(VG_NO_RETVAL);
}

void vgImageSubData(VGImage image, const void *data, VGint dataStride,
    VGImageFormat dataFormat, VGint x, VGint y, VGint width, VGint height)
{
    egl *e = NULL;
    vg_context *context = NULL;

    uapi_unused(data);
    uapi_unused(dataStride);
    uapi_unused(dataFormat);
    uapi_unused(x);
    uapi_unused(y);
    uapi_unused(width);
    uapi_unused(height);

    vg_get_context(VG_NO_RETVAL);
    vg_if_error_return(image == VG_INVALID_HANDLE, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)image);
    if (resource_type != VG_TYPE_RESOURCE_IMAGE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not image object");
        vg_return(VG_NO_RETVAL);
    }
    // need to do something
    vg_return(VG_NO_RETVAL);
}

void vgGetImageSubData(VGImage image, void *data, VGint dataStride,
    VGImageFormat dataFormat, VGint x, VGint y, VGint width, VGint height)
{
    egl *e = NULL;
    vg_context *context = NULL;

    uapi_unused(data);
    uapi_unused(dataStride);
    uapi_unused(dataFormat);
    uapi_unused(x);
    uapi_unused(y);
    uapi_unused(width);
    uapi_unused(height);

    vg_get_context(VG_NO_RETVAL);
    vg_if_error_return(image == VG_INVALID_HANDLE, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)image);
    if (resource_type != VG_TYPE_RESOURCE_IMAGE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not image object");
        vg_return(VG_NO_RETVAL);
    }
    // need to do something
    vg_return(VG_NO_RETVAL);
}

VGImage vgChildImage(VGImage parent, VGint x, VGint y, VGint width, VGint height)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_INVALID_HANDLE);

    if (width < 0 || height < 0) {
        goto illigal_para;
    }
    vg_image *p = (vg_image *)(uintptr_t)parent;
    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)parent);
    if (resource_type != VG_TYPE_RESOURCE_IMAGE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not image object");
        vg_return(VG_BAD_HANDLE_ERROR);
    }

    vg_image *image = new_object(vg_image, p->fmt, width, height, p->allowed_quality);
    if (image == NULL) {
        context->set_error(context, VG_OUT_OF_MEMORY_ERROR);
        vg_err("new image object failed");
        vg_return(VG_INVALID_HANDLE);
    }
    image->offset_x = (td_u32)x;
    image->offset_y = (td_u32)y;
    image->parent = p;

    add_ref(p);
    context->image_manager->add_resource(context->image_manager, (vg_resource *)(uintptr_t)image);

    vg_return((VGImage)(uintptr_t)image);

illigal_para:
    context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
    vg_return(VG_INVALID_HANDLE);
}

VGImage vgGetParent(VGImage image)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_INVALID_HANDLE);

    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)image);
    if (resource_type != VG_TYPE_RESOURCE_IMAGE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not image object");
        vg_return(VG_INVALID_HANDLE);
    }

    vg_return((VGImage)(uintptr_t)(((vg_image *)(uintptr_t)image)->parent));
}

void vgCopyImage(VGImage dst, VGint dx, VGint dy, VGImage src, VGint sx, VGint sy,
    VGint width, VGint height, VGboolean dither)
{
    egl *e = NULL;
    vg_context *context = NULL;

    uapi_unused(dx);
    uapi_unused(dy);
    uapi_unused(sx);
    uapi_unused(sy);

    uapi_unused(width);
    uapi_unused(height);
    uapi_unused(dither);

    vg_get_context(VG_NO_RETVAL);
    vg_if_error_return(dst == VG_INVALID_HANDLE, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);
    vg_if_error_return(src == VG_INVALID_HANDLE, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)dst);
    if (resource_type != VG_TYPE_RESOURCE_IMAGE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not image object");
        vg_return(VG_NO_RETVAL);
    }
    // need to do something
    vg_return(VG_NO_RETVAL);
}

void vgDrawImage(VGImage image)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);
    vg_if_error_return(image == VG_INVALID_HANDLE, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)image);
    if (resource_type != VG_TYPE_RESOURCE_IMAGE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not image object");
        vg_return(VG_NO_RETVAL);
    }

    egl_surface *surface = e->get_current_surface(e);
    vg_if_error_return(surface == NULL, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);
    vg_image *dst_image = surface->image;
    vg_if_error_return(dst_image == NULL, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

    vg_image *src_image = (vg_image*)(uintptr_t)image;
    VGErrorCode ret = dst_image->draw_image(dst_image, src_image, context);
    vg_if_error_return(ret != VG_NO_ERROR, ret, VG_NO_RETVAL);

    vg_return(VG_NO_RETVAL);
}

void vgSetPixels(VGint dx, VGint dy, VGImage src, VGint sx, VGint sy,
    VGint width, VGint height)
{
    egl *e = NULL;
    vg_context *context = NULL;

    uapi_unused(dx);
    uapi_unused(dy);
    uapi_unused(sx);
    uapi_unused(sy);
    uapi_unused(width);
    uapi_unused(height);

    vg_get_context(VG_NO_RETVAL);
    vg_if_error_return(src == VG_INVALID_HANDLE, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)src);
    if (resource_type != VG_TYPE_RESOURCE_IMAGE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not image object");
        vg_return(VG_NO_RETVAL);
    }
    // need to do something
    vg_return(VG_NO_RETVAL);
}


VG_API_CALL void VG_API_ENTRY vgWritePixels(const void *data, VGint dataStride, VGImageFormat dataFormat,
    VGint dx, VGint dy, VGint width, VGint height) VG_API_EXIT;
VG_API_CALL void VG_API_ENTRY vgGetPixels(VGImage dst, VGint dx, VGint dy, VGint sx, VGint sy,
    VGint width, VGint height) VG_API_EXIT;
VG_API_CALL void VG_API_ENTRY vgReadPixels(void *data, VGint dataStride, VGImageFormat dataFormat,
    VGint sx, VGint sy, VGint width, VGint height) VG_API_EXIT;
VG_API_CALL void VG_API_ENTRY vgCopyPixels(VGint dx, VGint dy, VGint sx, VGint sy,
    VGint width, VGint height) VG_API_EXIT;


void vgClear(VGint x, VGint y, VGint width, VGint height)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    egl_surface *surface = e->get_current_surface(e);
    vg_if_error_return(surface == NULL, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);
    vg_image *dst_image = surface->image;
    vg_if_error_return(dst_image == NULL, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

    ext_rect rect = { x, y, width, height };
    VGErrorCode ret = dst_image->clear_image(dst_image, &rect, context);
    vg_if_error_return(ret != VG_NO_ERROR, ret, VG_NO_RETVAL);

    vg_return(VG_NO_RETVAL);
}

void vgFlush(void)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);
    context->flush(context);
    vg_return(VG_NO_RETVAL);
}

VG_API_CALL void VG_API_ENTRY vgFinish(void) VG_API_EXIT
{
}

VGPaint vgCreatePaint(void)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_INVALID_HANDLE);

    vg_paint *paint = new_object(vg_paint);
    if (paint == NULL) {
        context->set_error(context, VG_OUT_OF_MEMORY_ERROR);
        vg_err("new paint object failed");
        vg_return(VG_INVALID_HANDLE);
    }
    context->paint_manager->add_resource(context->paint_manager, (vg_resource *)(uintptr_t)paint);

    vg_return((VGPaint)(uintptr_t)paint);
}

void vgDestroyPaint(VGPaint paint)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);
    vg_if_error_return(paint == VG_INVALID_HANDLE, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

    VGboolean ret = context->paint_manager->is_valid(context->paint_manager, (vg_resource *)(uintptr_t)paint);
    if (ret != VG_TRUE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not paint object");
        vg_return(VG_NO_RETVAL);
    }
    context->paint_manager->remove_resource(context->paint_manager, (vg_resource *)(uintptr_t)paint);
    vg_return(VG_NO_RETVAL);
}

void vgSetPaint(VGPaint paint, VGbitfield paintModes)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    context->release_paint(context, paintModes);

    if (paint == VG_INVALID_HANDLE) {
        vg_return(VG_NO_RETVAL);
    }
    VGboolean ret = context->paint_manager->is_valid(context->paint_manager, (vg_resource *)(uintptr_t)paint);
    if (ret != VG_TRUE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not paint object");
        vg_return(VG_NO_RETVAL);
    }

    if ((paintModes & VG_FILL_PATH) != 0) {
        context->fill_paint = paint;
        vg_paint *p = (vg_paint *)(uintptr_t)paint;
        add_ref(p);
    }

    if ((paintModes & VG_STROKE_PATH) != 0) {
        context->stroke_paint = paint;
        vg_paint *p = (vg_paint *)(uintptr_t)paint;
        add_ref(p);
    }
    vg_return(VG_NO_RETVAL);
}

VGPaint vgGetPaint(VGPaintMode paintMode)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_INVALID_HANDLE);

    vg_paint *p = NULL;
    if (((unsigned int)paintMode & VG_FILL_PATH) != 0) {
        p = (vg_paint *)(uintptr_t)context->fill_paint;
    } else if (((unsigned int)paintMode & VG_STROKE_PATH) != 0) {
        p = (vg_paint *)(uintptr_t)context->stroke_paint;
    }
    vg_return((VGPaint)(uintptr_t)p);
}

void vgSetColor(VGPaint paint, VGuint rgba)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    VGboolean ret = context->paint_manager->is_valid(context->paint_manager, (vg_resource *)(uintptr_t)paint);
    if (ret != VG_TRUE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not paint object");
        vg_return(VG_NO_RETVAL);
    }

    vg_paint *p = (vg_paint *)(uintptr_t)paint;
    p->set_color(p, rgba);

    vg_return(VG_NO_RETVAL);
}

VGuint vgGetColor(VGPaint paint)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_INVALID_HANDLE);

    VGboolean ret = context->paint_manager->is_valid(context->paint_manager, (vg_resource *)(uintptr_t)paint);
    if (ret != VG_TRUE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not paint object");
        vg_return(VG_INVALID_HANDLE);
    }

    vg_paint *p = (vg_paint *)(uintptr_t)paint;
    VGuint color = p->get_color(p);

    vg_return(color);  // ·µ»ØÖµrgba ? argb
}

// path
VGPath vgCreatePath(VGint pathFormat, VGPathDatatype datatype, VGfloat scale, VGfloat bias,
                    VGint segmentCapacityHint, VGint coordCapacityHint, VGbitfield capabilities)
{
    egl *e = NULL;
    vg_context *context = NULL;

    uapi_unused(pathFormat);
    uapi_unused(segmentCapacityHint);
    uapi_unused(coordCapacityHint);

    vg_get_context(VG_INVALID_HANDLE);

    if (!is_data_type_supported(datatype)) {
        context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
        vg_err("data type not support");
        vg_return(VG_INVALID_HANDLE);
    }

    vg_path *path = new_object(vg_path, datatype, scale, bias, capabilities);
    if (path == NULL) {
        context->set_error(context, VG_OUT_OF_MEMORY_ERROR);
        vg_err("new path object failed");
        vg_return(VG_INVALID_HANDLE);
    }
    context->path_manager->add_resource(context->path_manager, (vg_resource *)(uintptr_t)path);

    vg_return((VGPaint)(uintptr_t)path);
}

void vgDestroyPath(VGPath path)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);
    vg_if_error_return(path == VG_INVALID_HANDLE, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);
    VGboolean ret = context->path_manager->is_valid(context->path_manager, (vg_resource *)(uintptr_t)path);
    if (ret != VG_TRUE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not path object");
        vg_return(VG_NO_RETVAL);
    }
    context->path_manager->remove_resource(context->path_manager, (vg_resource *)(uintptr_t)path);
    vg_return(VG_NO_RETVAL);
}

void vgAppendPathData(VGPath dstPath, VGint numSegments, const VGubyte *pathSegments, const void *pathData)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);
    VGboolean is_valid = context->path_manager->is_valid(context->path_manager, (vg_resource *)(uintptr_t)dstPath);
    if (is_valid != VG_TRUE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not path object");
        vg_return(VG_NO_RETVAL);
    }
    vg_path *p = (vg_path *)(uintptr_t)dstPath;
    if (p == NULL) {
        vg_return(VG_NO_RETVAL);
    }
    VGErrorCode ret = p->append_path_data(p, (VGuint)numSegments, pathSegments, pathData);
    if (ret != VG_NO_ERROR) {
        context->set_error(context, ret);
        vg_err("call append path data failed, ret:0x%x", ret);
        vg_return(VG_NO_RETVAL);
    }
    vg_return(VG_NO_RETVAL);
}

void vgAppendPath(VGPath dstPath, VGPath srcPath)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);

    VGboolean dst_ret = context->path_manager->is_valid(context->path_manager, (vg_resource *)(uintptr_t)dstPath);
    if (dst_ret != VG_TRUE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not path object");
        vg_return(VG_NO_RETVAL);
    }

    VGboolean src_ret = context->path_manager->is_valid(context->path_manager, (vg_resource *)(uintptr_t)srcPath);
    if (src_ret != VG_TRUE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not path object");
        vg_return(VG_NO_RETVAL);
    }

    vg_path *dst = (vg_path *)(uintptr_t)dstPath;
    vg_path *src = (vg_path *)(uintptr_t)srcPath;
    if (dst == NULL || src == NULL) {
        context->set_error(context, VG_ILLEGAL_ARGUMENT_ERROR);
        vg_return(VG_NO_RETVAL);
    }
    VGErrorCode ret = dst->append_path(dst, src);
    if (ret != VG_NO_ERROR) {
        context->set_error(context, ret);
        vg_err("call append path failed, ret:0x%x", ret);
        vg_return(VG_NO_RETVAL);
    }
    vg_return(VG_NO_RETVAL);
}

void vgDrawPath(VGPath path, VGbitfield paintModes)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);
    vg_if_error_return(path == VG_INVALID_HANDLE, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

    VGboolean is_valid = context->path_manager->is_valid(context->path_manager, (vg_resource *)(uintptr_t)path);
    if (is_valid != VG_TRUE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not path object");
        vg_return(VG_NO_RETVAL);
    }

    egl_surface *surface = e->get_current_surface(e);
    vg_if_error_return(surface == NULL, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);
    VGboolean image_ret = context->image_manager->is_valid(context->image_manager,
        (vg_resource *)(uintptr_t)surface->image);
    if (image_ret != VG_TRUE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not path object");
        vg_return(VG_NO_RETVAL);
    }

    vg_image *dst_image = surface->image;
    vg_if_error_return(dst_image == NULL, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

    vg_path *p = (vg_path *)(uintptr_t)path;
    VGErrorCode ret = p->draw_path(p, dst_image, paintModes, context);
    vg_if_error_return(ret != VG_NO_ERROR, ret, VG_NO_RETVAL);
    vg_return(VG_NO_RETVAL);
}

void vgClearPath(VGPath path, VGbitfield capabilities)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);
    vg_if_error_return(path == VG_INVALID_HANDLE, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

    VGboolean is_valid = context->path_manager->is_valid(context->path_manager, (vg_resource *)(uintptr_t)path);
    if (is_valid != VG_TRUE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not path object");
        vg_return(VG_NO_RETVAL);
    }
    vg_path *p = (vg_path *)(uintptr_t)path;
    VGErrorCode ret = p->clear_path(p, capabilities);
    vg_if_error_return(ret != VG_NO_ERROR, ret, VG_NO_RETVAL);
    vg_return(VG_NO_RETVAL);
}

void vgRemovePathCapabilities(VGPath path, VGbitfield capabilities)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(VG_NO_RETVAL);
    vg_if_error_return(path == VG_INVALID_HANDLE, VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

    VGboolean is_valid = context->path_manager->is_valid(context->path_manager, (vg_resource *)(uintptr_t)path);
    if (is_valid != VG_TRUE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not path object");
        vg_return(VG_NO_RETVAL);
    }
    vg_path *p = (vg_path *)(uintptr_t)path;
    VGErrorCode ret = p->remove_path_capabilities(p, capabilities);
    vg_if_error_return(ret != VG_NO_ERROR, ret, VG_NO_RETVAL);
    vg_return(VG_NO_RETVAL);
}

VGbitfield vgGetPathCapabilities(VGPath path)
{
    egl *e = NULL;
    vg_context *context = NULL;
    vg_get_context(0);
    vg_if_error_return(path == VG_INVALID_HANDLE, VG_BAD_HANDLE_ERROR, 0);

    VGboolean is_valid = context->path_manager->is_valid(context->path_manager, (vg_resource *)(uintptr_t)path);
    if (is_valid != VG_TRUE) {
        context->set_error(context, VG_BAD_HANDLE_ERROR);
        vg_err("not path object");
        vg_return(0);
    }
    vg_path *p = (vg_path *)(uintptr_t)path;
    VGbitfield ret = p->get_path_capabilities(p);
    vg_return(ret);
}
