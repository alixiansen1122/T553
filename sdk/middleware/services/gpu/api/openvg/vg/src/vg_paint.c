/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: openvg paint source file
 * Author: @CompanyNameTag
 * Create: 2021-08-16
 */

#include "vg_paint.h"
#include <securec.h>
#include "vg_debug.h"
#include "vg_math.h"
#include "hdi_hardware_capability.h"

#define VG_PAINT_COLOR_RAMP_STOPS_COUNT  5
#define VG_PAINT_COLOR_PARA_COUNT        4
#define VG_PAINT_LINEAR_PARA_COUNT       4
#define VG_PAINT_RADIAL_PARA_COUNT       5
#define VG_PAINT_SWEEP_PARA_COUNT        3
#define VG_PAINT_COLOR_STOP_OFFSET_RADIO 128

static void vg_paint_add_reference(vg_paint *thiz)
{
    thiz->ref_count++;
}

static int vg_paint_remove_reference(vg_paint *thiz)
{
    thiz->ref_count--;
    if (thiz->ref_count < 0) {
        thiz->ref_count = 0;
    }
    return thiz->ref_count;
}

static VGErrorCode vg_paint_seti(vg_paint *thiz, VGint type, VGint value)
{
    switch (type) {
        case VG_PAINT_TYPE:
            if ((value < VG_PAINT_TYPE_COLOR) || (value > VG_PAINT_TYPE_EXT_SWEEP_GRADIENT)) {
                vg_err("invalid value:0x%x", value);
                return VG_ILLEGAL_ARGUMENT_ERROR;
            }
            thiz->paint_type = value;
            break;
        case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
            if ((value < VG_COLOR_RAMP_SPREAD_PAD) || (value > VG_COLOR_RAMP_SPREAD_REFLECT)) {
                vg_err("invalid value:0x%x", value);
                return VG_ILLEGAL_ARGUMENT_ERROR;
            }
            thiz->colro_ramp_spread_mode = value;
            break;
        case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
            thiz->color_ramp_premultiplied = value;
            break;
        default:
            vg_err("invalid type:0x%x", type);
            return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    return VG_NO_ERROR;
}

static VGErrorCode vg_paint_geti(const vg_paint *thiz, VGint type, VGint *value)
{
    switch (type) {
        case VG_PAINT_TYPE:
            *value = thiz->paint_type;
            break;
        case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
            *value = thiz->colro_ramp_spread_mode;
            break;
        case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
            *value = thiz->color_ramp_premultiplied;
            break;
        default:
            vg_err("invalid type:0x%x", type);
            return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    return VG_NO_ERROR;
}

static VGErrorCode vg_paint_set_stop(vg_paint *thiz, VGint count, const VGfloat *values)
{
    VGint num;
    /* count: 1 offset , 2 r, 3: g, 4:b 5:a, count  is a multiple of 5 */
    if (count % VG_PAINT_COLOR_RAMP_STOPS_COUNT != 0) {
        vg_err("invalid count:%d", count);
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    num = count / VG_PAINT_COLOR_RAMP_STOPS_COUNT;
    // only support 7 stops
    if (num < 2 || num > HARDWARE_MAX_COLOR_RAMP_STOPS) { /* at least 2 points */
        vg_err("at least 2 stops and at last %d stops, input is %d", HARDWARE_MAX_COLOR_RAMP_STOPS, num);
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    thiz->stop.num = (VGubyte)num;

    for (int i = 0; i < count; i += VG_PAINT_COLOR_RAMP_STOPS_COUNT) {
        /* 100:eliminate normalization  */
        thiz->stop.offsets[i / VG_PAINT_COLOR_RAMP_STOPS_COUNT] = values[i];
        VGuint r = (VGuint)(values[i + 1] * 0xFF);
        VGuint g = (VGuint)(values[i + 2] * 0xFF);
        VGuint b = (VGuint)(values[i + 3] * 0xFF);
        VGuint a = (VGuint)(values[i + 4] * 0xFF);
        /* 0, 8, 16, 24 bit width */
        thiz->stop.colors[i / VG_PAINT_COLOR_RAMP_STOPS_COUNT] = (a << 24) | (r << 16) | (g << 8) | (b << 0);
    }
    return VG_NO_ERROR;
}

static VGErrorCode vg_paint_set_solid_color(vg_paint *thiz, VGint count, const VGfloat *values)
{
    if (count != VG_PAINT_COLOR_PARA_COUNT) {
        vg_err("invalid count:%d", count);
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    for (int i = 0; i < VG_PAINT_COLOR_PARA_COUNT; i++) {
        thiz->paint_color[i] = values[i];
    }
    return VG_NO_ERROR;
}

static VGErrorCode vg_paint_set_linear_gradient(vg_paint *thiz, VGint count, const VGfloat *values)
{
    if (count != VG_PAINT_LINEAR_PARA_COUNT) {
        vg_err("invalid count:%d", count);
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    thiz->linear_gradient_point0[0] = values[0]; /* start.x at index 0 */
    thiz->linear_gradient_point0[1] = values[1]; /* start.y at index 1 */
    thiz->linear_gradient_point1[0] = values[2]; /* end.x at index 2 */
    thiz->linear_gradient_point1[1] = values[3]; /* end.y at index 3 */
    return VG_NO_ERROR;
}

static VGErrorCode vg_paint_set_radial_gradient(vg_paint *thiz, VGint count, const VGfloat *values)
{
    if (count != VG_PAINT_RADIAL_PARA_COUNT) {
        vg_err("invalid count:%d", count);
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    // only support center and focus same: 0,1: center; 2,3: focus
    if ((!vg_float_is_equal(values[0], values[2])) || (!vg_float_is_equal(values[1], values[3]))) {
        vg_err("only support center and focus same");
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    thiz->radial_gradient_center[0] = values[0]; /* center.x at index 0 */
    thiz->radial_gradient_center[1] = values[1]; /* center.y at index 1 */
    thiz->radial_gradient_focus[0] = values[2];  /* focus.x at index 2 */
    thiz->radial_gradient_focus[1] = values[3];  /* focus.y at index 3 */
    thiz->radial_gradient_radius = values[4];    /* radius at index 4 */
    return VG_NO_ERROR;
}

static VGErrorCode vg_paint_set_sweep_gradient(vg_paint *thiz, VGint count, const VGfloat *values)
{
    if (count != VG_PAINT_SWEEP_PARA_COUNT) {
        vg_err("invalid count:%d", count);
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    thiz->sweep_gradient_center[0] = values[0];
    thiz->sweep_gradient_center[1] = values[1];
    thiz->sweep_gradient_angle = values[2]; /* angle at index 2 */
    return VG_NO_ERROR;
}

static VGErrorCode vg_paint_setfv(vg_paint *thiz, VGint type, VGint count, const VGfloat *values)
{
    switch (type) {
        case VG_PAINT_COLOR_RAMP_STOPS:
            return vg_paint_set_stop(thiz, count, values);
        case VG_PAINT_COLOR:
            return vg_paint_set_solid_color(thiz, count, values);
        case VG_PAINT_LINEAR_GRADIENT:
            return vg_paint_set_linear_gradient(thiz, count, values);
        case VG_PAINT_RADIAL_GRADIENT:
            return vg_paint_set_radial_gradient(thiz, count, values);
        case VG_PAINT_EXT_SWEEP_GRADIENT:
            return vg_paint_set_sweep_gradient(thiz, count, values);
        default:
            vg_err("invalid type:0x%x", type);
            return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    return VG_NO_ERROR;
}

static VGErrorCode paint_getfv_color_ramp_stops(const vg_paint *thiz, VGint type, VGint count, VGfloat *values)
{
    uapi_unused(type);

    if (count % VG_PAINT_COLOR_RAMP_STOPS_COUNT != 0) {
        vg_err("invalid count:%d", count);
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    for (int i = 0; i < count; i += VG_PAINT_COLOR_RAMP_STOPS_COUNT) {
        values[i] = thiz->stop.offsets[i / VG_PAINT_COLOR_RAMP_STOPS_COUNT];
        /* index 1 */
        values[i + 1] =
            (VGfloat)((thiz->stop.colors[i / VG_PAINT_COLOR_RAMP_STOPS_COUNT] >> (TD_U16_BITS)) & 0xFF) / 0xFF;
        /* index 2 */
        values[i + 2] =
            (VGfloat)((thiz->stop.colors[i / VG_PAINT_COLOR_RAMP_STOPS_COUNT] >> (TD_U8_BITS)) & 0xFF) / 0xFF;
        /* index 3 */
        values[i + 3] =
            (VGfloat)(thiz->stop.colors[i / VG_PAINT_COLOR_RAMP_STOPS_COUNT] & 0xFF) / 0xFF;
        /* index 4 */
        values[i + 4] =
            (VGfloat)((thiz->stop.colors[i / VG_PAINT_COLOR_RAMP_STOPS_COUNT] >> (TD_U24_BITS)) & 0xFF) / 0xFF;
    }
    return VG_NO_ERROR;
}

static VGErrorCode vg_paint_getfv(const vg_paint *thiz, VGint type, VGint count, VGfloat *values)
{
    switch (type) {
        case VG_PAINT_COLOR_RAMP_STOPS:
            return paint_getfv_color_ramp_stops(thiz, type, count, values);
        case VG_PAINT_COLOR:
            if (count != VG_PAINT_COLOR_PARA_COUNT) {
                vg_err("invalid count:%d", count);
                return VG_ILLEGAL_ARGUMENT_ERROR;
            }
            for (int i = 0; i < VG_PAINT_COLOR_PARA_COUNT; i++) {
                values[i] = thiz->paint_color[i];
            }
            break;
        case VG_PAINT_LINEAR_GRADIENT:
            if (count != VG_PAINT_LINEAR_PARA_COUNT) {
                vg_err("invalid count:%d", count);
                return VG_ILLEGAL_ARGUMENT_ERROR;
            }
            values[0] = thiz->linear_gradient_point0[0]; /* 0: index */
            values[1] = thiz->linear_gradient_point0[1]; /* 1: index */
            values[2] = thiz->linear_gradient_point1[0]; /* 2: index */
            values[3] = thiz->linear_gradient_point1[1]; /* 3: index */
            break;
        case VG_PAINT_RADIAL_GRADIENT:
            if (count != VG_PAINT_RADIAL_PARA_COUNT) {
                vg_err("invalid count:%d", count);
                return VG_ILLEGAL_ARGUMENT_ERROR;
            }
            values[0] = thiz->radial_gradient_center[0]; /* 0: index */
            values[1] = thiz->radial_gradient_center[1]; /* 1: index */
            values[2] = thiz->radial_gradient_focus[0]; /* 2: index */
            values[3] = thiz->radial_gradient_focus[1]; /* 3: index */
            values[4] = thiz->radial_gradient_radius; /* 4: index */
            break;
        case VG_PAINT_EXT_SWEEP_GRADIENT:
            if (count != VG_PAINT_SWEEP_PARA_COUNT) {
                vg_err("invalid count:%d", count);
                return VG_ILLEGAL_ARGUMENT_ERROR;
            }
            values[0] = thiz->sweep_gradient_center[0]; /* 0: index */
            values[1] = thiz->sweep_gradient_center[1]; /* 1: index */
            values[2] = thiz->sweep_gradient_angle; /* 2: index */
            break;
        default:
            vg_err("invalid type:0x%x", type);
            return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    return VG_NO_ERROR;
}

static VGErrorCode vg_paint_get_size(const vg_paint *thiz, VGint type, VGint *count)
{
    switch (type) {
        case VG_PAINT_COLOR_RAMP_STOPS:
            *count = thiz->stop.num * VG_PAINT_COLOR_RAMP_STOPS_COUNT;
            break;
        case VG_PAINT_COLOR:
            *count = VG_PAINT_COLOR_PARA_COUNT;
            break;
        case VG_PAINT_LINEAR_GRADIENT:
            *count = VG_PAINT_LINEAR_PARA_COUNT;
            break;
        case VG_PAINT_RADIAL_GRADIENT:
            *count = VG_PAINT_RADIAL_PARA_COUNT;
            break;
        default:
            vg_err("invalid type:0x%x", type);
            return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    return VG_NO_ERROR;
}

static VGErrorCode vg_paint_set_color(vg_paint *thiz, VGuint rgba)
{
    /* 0, 3: array subscript and multiple. R */
    thiz->paint_color[0] = (VGfloat)((rgba >> (TD_U8_BITS * 3)) & 0xFF) / 0xFF;
    /* 1, 2: array subscript and multiple. G */
    thiz->paint_color[1] = (VGfloat)((rgba >> (TD_U8_BITS * 2)) & 0xFF) / 0xFF;
    /* 2, 1: array subscript and multiple. B */
    thiz->paint_color[2] = (VGfloat)((rgba >> (TD_U8_BITS * 1)) & 0xFF) / 0xFF;
    /* 3, 0: array subscript and multiple. A */
    thiz->paint_color[3] = (VGfloat)((rgba >> (TD_U8_BITS * 0)) & 0xFF) / 0xFF;
    return VG_NO_ERROR;
}

static VGuint vg_paint_get_color(const vg_paint *thiz)
{
    td_u32 r = (td_u32)(thiz->paint_color[0] * 0xFF); /* 0: array subscript */
    td_u32 g = (td_u32)(thiz->paint_color[1] * 0xFF); /* 1: array subscript */
    td_u32 b = (td_u32)(thiz->paint_color[2] * 0xFF); /* 2: array subscript */
    td_u32 a = (td_u32)(thiz->paint_color[3] * 0xFF); /* 3: array subscript */
    return (r << 24) | (g << 16) | (b << 8) | (a << 0); /* rgba,24,16,8,0 is Bit width */
}

static VGErrorCode vg_paint_paint_pattern(const vg_paint *thiz, const vg_image *pattern)
{
    uapi_unused(thiz);
    uapi_unused(pattern);

    return VG_NO_ERROR;
}

static int vg_paint_init(vg_paint *p)
{
    if (p == NULL) {
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }

    init_object_common_private_member(vg_paint, p, VG_MAGIC_RESOURCE_PAINT);

    p->seti = vg_paint_seti;
    p->geti = vg_paint_geti;
    p->setfv = vg_paint_setfv;
    p->get_size = vg_paint_get_size;
    p->getfv = vg_paint_getfv;
    p->set_color = vg_paint_set_color;
    p->get_color = vg_paint_get_color;
    p->paint_pattern = vg_paint_paint_pattern;
    return VG_NO_ERROR;
}

vg_paint *vg_paint_new(void)
{
    vg_paint *p = (vg_paint *)malloc(sizeof(vg_paint));
    if (p == NULL) {
        vg_err("malloc %d failed", sizeof(vg_paint));
        return NULL;
    }
    (void)memset_s(p, sizeof(*p), 0, sizeof(vg_paint));
    vg_paint_init(p);
    return p;
}

void vg_paint_delete(vg_paint *p)
{
    if (p == NULL) {
        return;
    }

    if (p->ref_count != 0) {
        vg_err("paint ref count not zero:%d\n", p->ref_count);
    }
    free(p);
}
