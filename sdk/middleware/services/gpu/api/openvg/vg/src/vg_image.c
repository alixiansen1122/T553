/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: openvg image source file
 * Author: @CompanyNameTag
 * Create: 2021-07-20
 */

#include "vg_image.h"
#include <securec.h>
#include "vg_debug.h"
#include "vg_draw.h"
#include "soc_vau_api.h"
#include "hdi_gfx.h"

static void vg_image_add_reference(vg_image *thiz)
{
    thiz->ref_count++;
}

static int vg_image_remove_reference(vg_image *thiz)
{
    thiz->ref_count--;
    if (thiz->ref_count < 0) {
        thiz->ref_count = 0;
    }
    return thiz->ref_count;
}

static VGErrorCode update_vg_image_compress_info(vg_image *m, ext_vau_cmp_header *info)
{
    m->width = info->width;
    m->height = info->height;
    m->stride = info->stride;
    m->compress_mode = (info->alpha_bypass != 0) ? EXT_GFX_COMPRESS_HFBC_ABYPASS : EXT_GFX_COMPRESS_HFBC;
    if (info->pixel_format == EXT_GFX_FMT_ARGB8888) {
        if ((m->fmt != VG_sARGB_8888_PRE) && (m->fmt != VG_sARGB_8888)) {
            vg_err("invalid fmt:0x%x", m->fmt);
            return VG_ILLEGAL_ARGUMENT_ERROR;
        }
    }
    if (info->pixel_format == EXT_GFX_FMT_RGB888) {
        if (m->fmt != VG_EXT_sRGB_888) {
            vg_err("invalid fmt:0x%x", m->fmt);
            return VG_ILLEGAL_ARGUMENT_ERROR;
        }
    }
    if (info->pixel_format == EXT_GFX_FMT_RGB565) {
        if (m->fmt != VG_sRGB_565) {
            vg_err("invalid fmt:0x%x", m->fmt);
            return VG_ILLEGAL_ARGUMENT_ERROR;
        }
    }
    return VG_NO_ERROR;
}
static VGErrorCode vg_image_seti_value(vg_image *thiz, VGint type, VGint value)
{
    switch (type) {
        case VG_IMAGE_EXT_BUFFER_TYPE: {
            ext_vau_cmp_header info = {0};
            if ((value != VG_IMAGE_EXT_TYPE_NORMAL) &&
                (value != VG_IMAGE_EXT_TYPE_COMPRESS) &&
                (value != VG_IMAGE_EXT_TYPE_COLOR)) {
                vg_err("invalid value:0x%x", value);
                return VG_ILLEGAL_ARGUMENT_ERROR;
            }
            thiz->buffer_type = value;
            if ((thiz->buffer_type == VG_IMAGE_EXT_TYPE_COMPRESS) && thiz->vir_addr != NULL) {
                uapi_vau_parse_cmp_header(thiz->vir_addr, VAU_COMPRESS_HEADER, &info);
                VGErrorCode ret = update_vg_image_compress_info(thiz, &info);
                if (ret != VG_NO_ERROR) {
                    vg_err("call update_vg_image_compress_info:0x%x", ret);
                    return ret;
                }
            }
            break;
        }
        case VG_IMAGE_EXT_STRIDE:
            thiz->stride = (VGuint)value;
            break;
        case VG_IMAGE_EXT_PHY_ADDR:
            thiz->phy_addr = (VGuint)value;
            break;
        case VG_IMAGE_EXT_VIR_ADD: {
            ext_vau_cmp_header info = {0};
            thiz->vir_addr = (td_uchar *)(uintptr_t)value;
            if (thiz->buffer_type == VG_IMAGE_EXT_TYPE_COMPRESS) {
                uapi_vau_parse_cmp_header(thiz->vir_addr, VAU_COMPRESS_HEADER, &info);
                VGErrorCode ret = update_vg_image_compress_info(thiz, &info);
                if (ret != VG_NO_ERROR) {
                    vg_err("call update_vg_image_compress_info:0x%x", ret);
                    return ret;
                }
            }
            break;
        }
        default:
            vg_err("invalid type:0x%x", type);
            return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    return VG_NO_ERROR;
}
static VGErrorCode vg_image_seti(vg_image *thiz, VGint type, VGint value)
{
    switch (type) {
        case VG_IMAGE_FORMAT:
        case VG_IMAGE_WIDTH:
        case VG_IMAGE_HEIGHT:
            // read only param
            return VG_NO_ERROR;
        default:
            break;
    }
    VGErrorCode ret = vg_image_seti_value(thiz, type, value);
    if (ret != VG_NO_ERROR) {
        vg_err("call vg_image_seti_value failed");
        return ret;
    }
    return VG_NO_ERROR;
}

static VGErrorCode vg_image_geti(const vg_image *thiz, VGint type, VGint *value)
{
    switch (type) {
        case VG_IMAGE_FORMAT:
            *value = thiz->fmt;
            break;
        case VG_IMAGE_WIDTH:
            *value = (VGint)thiz->width;
            break;
        case VG_IMAGE_HEIGHT:
            *value = (VGint)thiz->height;
            break;
        default:
            break;
    }
    switch (type) {
        case VG_IMAGE_EXT_BUFFER_TYPE:
            *value = thiz->buffer_type;
            break;
        case VG_IMAGE_EXT_PHY_ADDR:
            *value = (VGint)thiz->phy_addr;
            break;
        case VG_IMAGE_EXT_VIR_ADD:
            *value = (VGint)(uintptr_t)thiz->vir_addr;
            break;
        default:
            vg_err("invalid type:0x%x", type);
            return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    return VG_NO_ERROR;
}

static VGErrorCode vg_image_setfv(vg_image *thiz, VGint type, VGint count, const VGfloat *values)
{
    switch (type) {
        case VG_IMAGE_EXT_COLOR:
            if (count != 4) { /* 4: color array size */
                return VG_ILLEGAL_ARGUMENT_ERROR;
            }
            for (int i = 0; i < 4; i++) {  /* 4: color array size */
                thiz->color[i] = values[i];
            }
            break;
        default:
            vg_err("invalid type:0x%x", type);
            return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    return VG_NO_ERROR;
}

static VGErrorCode vg_image_get_size(const vg_image *thiz, VGint type, VGint *count)
{
    uapi_unused(thiz);

    switch (type) {
        case VG_IMAGE_EXT_COLOR:
            *count = 4; /* 4: index */
            break;
        default:
            vg_err("invalid type:0x%x", type);
            return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    return VG_NO_ERROR;
}

static VGErrorCode vg_image_getfv(const vg_image *thiz, VGint type, VGint count, VGfloat *values)
{
    switch (type) {
        case VG_IMAGE_EXT_COLOR:
            if (count != 4) { /* 4: color array size */
                return VG_ILLEGAL_ARGUMENT_ERROR;
            }
            for (int i = 0; i < 4; i++) { /* 4: color array size */
                values[i] = thiz->color[i];
            }
            break;
        default:
            vg_err("invalid type:0x%x", type);
            return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    return VG_NO_ERROR;
}

static VGErrorCode vg_image_draw_image(const vg_image *thiz, const vg_image *src, vg_context *context)
{
    // trans to vau(hdf gfx) format
    return vg_draw_image(src, thiz, context);
}

static VGErrorCode vg_image_clear_image(const vg_image *thiz, const ext_rect *rect, vg_context *context)
{
    return vg_draw_clear_image(thiz, rect, context);
}

int vg_image_init(vg_image *m, VGImageFormat fmt, VGint width, VGint height, VGbitfield allowed_quality)
{
    if (m == NULL) {
        vg_err("null ptr");
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    init_object_common_private_member(vg_image, m, VG_MAGIC_RESOURCE_IMAGE);
    m->fmt = fmt;
    m->width = (VGuint)width;
    m->height = (VGuint)height;
    m->allowed_quality = allowed_quality;

    m->parent = NULL;

    m->seti = vg_image_seti;
    m->geti = vg_image_geti;
    m->setfv = vg_image_setfv;
    m->get_size = vg_image_get_size;
    m->getfv = vg_image_getfv;
    m->draw_image = vg_image_draw_image;
    m->clear_image = vg_image_clear_image;
    return VG_NO_ERROR;
}

vg_image *vg_image_new(VGImageFormat fmt, VGint width, VGint height, VGbitfield allowed_quality)
{
    vg_image *m = (vg_image *)malloc(sizeof(vg_image));
    if (m == NULL) {
        vg_err("malloc %d failed", sizeof(vg_image));
        return NULL;
    }
    (void)memset_s(m, sizeof(*m), 0, sizeof(vg_image));
    vg_image_init(m, fmt, width, height, allowed_quality);
    return m;
}

void vg_image_delete(vg_image *m)
{
    if (m == NULL) {
        return;
    }

    if (m->ref_count != 0) {
        vg_err("image ref count not zero:%d\n", m->ref_count);
    }
    free(m);
}
