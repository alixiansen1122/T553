/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dpu api source file
 */

#include "soc_dpu_api.h"
#include <pthread.h>
#include "soc_osal.h"
#include "dpu_debug.h"
#include "drv_dpu_ioctl.h"

static td_s32 g_dpu_dev_fd = -1;
static td_u32 g_dpu_dev_init_cnt = 0;
static pthread_mutex_t g_dpu_dev_mutex = PTHREAD_MUTEX_INITIALIZER;

#define DPU_MAX_LAYER_ID     DRV_DPU_LAYER_ID2
#define DPU_MAX_LAYER_WIDTH  640
#define DPU_MAX_LAYER_HEIGHT 640

static td_void dpu_dev_lock(td_void)
{
    (td_void)pthread_mutex_lock(&g_dpu_dev_mutex);
}

static td_void dpu_dev_unlock(td_void)
{
    (td_void)pthread_mutex_unlock(&g_dpu_dev_mutex);
}

#define check_dpu_dev_fd_return()              \
    do {                                       \
        dpu_dev_lock();                        \
        if (g_dpu_dev_fd < 0) {                \
            dpu_err("DPU is not inited!");     \
            dpu_dev_unlock();                  \
            return EXT_FAILURE;                \
        }                                      \
        dpu_dev_unlock();                      \
    } while (0)

#define get_dpu_layer_id_return(layer, layer_id)     \
    do {                                             \
        layer_id = (drv_dpu_layer_id)(layer);        \
        if ((layer_id) < DRV_DPU_LAYER_ID1 ||        \
            (layer_id) > DRV_DPU_MAX_LAYER_ID) {    \
            dpu_err("invalid handle:%u", layer);     \
            return EXT_ERR_GFX_INVALID_PARAM;        \
        }                                            \
    } while (0)

#define check_dpu_null_pointer_return(in_ptr)        \
    do {                                             \
        if ((in_ptr) == TD_NULL) {                   \
            dpu_err("null pointer\n");               \
            return EXT_FAILURE;                      \
        }                                            \
    } while (0)


static drv_dpu_layer_id dpu_trans_layer_id_api2drv(ext_dpu_layer_id api_type)
{
    drv_dpu_layer_id drv_type;

    switch (api_type) {
        case EXT_DPU_LAYER_ID1:
            drv_type = DRV_DPU_LAYER_ID1;
            break;
        case EXT_DPU_LAYER_ID2:
            drv_type = DRV_DPU_LAYER_ID2;
            break;
        case EXT_DPU_LAYER_ID3:
            drv_type = DRV_DPU_LAYER_ID3;
            break;
        case EXT_DPU_LAYER_ID4:
            drv_type = DRV_DPU_LAYER_ID4;
            break;
        default:
            drv_type = DRV_DPU_LAYER_ID_MAX;
    }

    return drv_type;
}

static drv_dpu_buf_mode dpu_trans_buf_mode_api2drv(ext_dpu_buf_mode api_type)
{
    drv_dpu_buf_mode drv_type;

    switch (api_type) {
        case EXT_DPU_BUFFER_MODE_NORMAL:
            drv_type = DRV_DPU_BUF_MODE_NORMAL;
            break;
        case EXT_DPU_BUFFER_MODE_SINGLE:
            drv_type = DRV_DPU_BUF_MODE_SINGLE;
            break;
        default:
            drv_type = DRV_DPU_BUF_MODE_MAX;
    }

    return drv_type;
}

static drv_gfx_fmt dpu_trans_layer_fmt_api2drv(ext_gfx_fmt api_type)
{
    drv_gfx_fmt drv_type;

    switch (api_type) {
        case EXT_GFX_FMT_ARGB8888:
            drv_type = DRV_GFX_FMT_ARGB8888;
            break;
        case EXT_GFX_FMT_ARGB1555:
            drv_type = DRV_GFX_FMT_ARGB1555;
            break;
        case EXT_GFX_FMT_ARGB4444:
            drv_type = DRV_GFX_FMT_ARGB4444;
            break;
        case EXT_GFX_FMT_RGB888:
            drv_type = DRV_GFX_FMT_RGB888;
            break;
        case EXT_GFX_FMT_RGB565:
            drv_type = DRV_GFX_FMT_RGB565;
            break;
        case EXT_GFX_FMT_YUV420SP:
            drv_type = DRV_GFX_FMT_YUV420SP;
            break;
        default:
            drv_type = DRV_GFX_FMT_MAX;
    }

    return drv_type;
}

static ext_gfx_fmt dpu_trans_layer_fmt_drv2api(drv_gfx_fmt drv_type)
{
    ext_gfx_fmt api_type;

    switch (drv_type) {
        case DRV_GFX_FMT_ARGB8888:
            api_type = EXT_GFX_FMT_ARGB8888;
            break;
        case DRV_GFX_FMT_ARGB1555:
            api_type = EXT_GFX_FMT_ARGB1555;
            break;
        case DRV_GFX_FMT_ARGB4444:
            api_type = EXT_GFX_FMT_ARGB4444;
            break;
        case DRV_GFX_FMT_RGB888:
            api_type = EXT_GFX_FMT_RGB888;
            break;
        case DRV_GFX_FMT_RGB565:
            api_type = EXT_GFX_FMT_RGB565;
            break;
        default:
            api_type = EXT_GFX_FMT_MAX;
    }

    return api_type;
}

static drv_gfx_compress_mode dpu_trans_compress_mode_api2drv(ext_gfx_compress_mode api_type)
{
    drv_gfx_compress_mode drv_type;

    switch (api_type) {
        case EXT_GFX_COMPRESS_NONE:
            drv_type = DRV_GFX_COMPRESS_NONE;
            break;
        case EXT_GFX_COMPRESS_HFBC:
            drv_type = DRV_GFX_COMPRESS_HFBC;
            break;
        case EXT_GFX_COMPRESS_HFBC_ABYPASS:
            drv_type = DRV_GFX_COMPRESS_HFBC_ABYPASS;
            break;
        default:
            drv_type = DRV_GFX_COMPRESS_MAX;
    }

    return drv_type;
}

static ext_gfx_compress_mode dpu_trans_compress_mode_drv2api(drv_gfx_compress_mode drv_type)
{
    ext_gfx_compress_mode api_type;

    switch (drv_type) {
        case DRV_GFX_COMPRESS_NONE:
            api_type = EXT_GFX_COMPRESS_NONE;
            break;
        case DRV_GFX_COMPRESS_HFBC:
            api_type = EXT_GFX_COMPRESS_HFBC;
            break;
        case DRV_GFX_COMPRESS_HFBC_ABYPASS:
            api_type = EXT_GFX_COMPRESS_HFBC_ABYPASS;
            break;
        default:
            api_type = EXT_GFX_COMPRESS_MAX;
    }

    return api_type;
}

static td_void dpu_trans_layer_info_api2drv(const ext_dpu_layer_info *layer_info,
                                            drv_dpu_layer_info *drv_layer_info)
{
    drv_layer_info->width         = layer_info->width;
    drv_layer_info->height        = layer_info->height;
    drv_layer_info->layer_fmt     = dpu_trans_layer_fmt_api2drv(layer_info->layer_fmt);
    drv_layer_info->layer_id      = dpu_trans_layer_id_api2drv(layer_info->layer_id);
    drv_layer_info->buf_mode      = dpu_trans_buf_mode_api2drv(layer_info->buf_mode);
    drv_layer_info->compress_mode = dpu_trans_compress_mode_api2drv(layer_info->compress_mode);
}

static td_void dpu_trans_surface_api2drv(const ext_dpu_surface *surface, drv_dpu_surface *drv_surface)
{
    drv_surface->width         = surface->width;
    drv_surface->height        = surface->height;
    drv_surface->stride        = surface->stride;
    drv_surface->uv_stride     = surface->uv_stride;
    drv_surface->uv_offset     = surface->uv_offset;
    drv_surface->size          = surface->size;
    drv_surface->phy_addr      = surface->phy_addr;
    drv_surface->vir_addr      = surface->vir_addr;
    drv_surface->update_rect   = surface->update_rect;
    drv_surface->premulti_en   = surface->premulti_en;
    drv_surface->color_fmt     = dpu_trans_layer_fmt_api2drv(surface->color_fmt);
    drv_surface->compress_mode = dpu_trans_compress_mode_api2drv(surface->compress_mode);
}

static td_void dpu_trans_surface_drv2api(ext_dpu_surface *surface, const drv_dpu_surface *drv_surface)
{
    surface->width         = drv_surface->width;
    surface->height        = drv_surface->height;
    surface->stride        = drv_surface->stride;
    surface->uv_stride     = drv_surface->uv_stride;
    surface->uv_offset     = drv_surface->uv_offset;
    surface->size          = drv_surface->size;
    surface->phy_addr      = drv_surface->phy_addr;
    surface->vir_addr      = drv_surface->vir_addr;
    surface->update_rect   = drv_surface->update_rect;
    surface->premulti_en   = drv_surface->premulti_en;
    surface->color_fmt     = dpu_trans_layer_fmt_drv2api(drv_surface->color_fmt);
    surface->compress_mode = dpu_trans_compress_mode_drv2api(drv_surface->compress_mode);
}

td_s32 uapi_dpu_create_layer(const ext_dpu_layer_info *layer_info, td_handle *layer)
{
    td_s32 ret;
    drv_dpu_layer_info drv_layer_info = {0};

    if ((layer_info == TD_NULL) || (layer == TD_NULL)) {
        dpu_err("null pointer");
        return EXT_ERR_GFX_NULL_POINTER;
    }

    if ((layer_info->width > DPU_MAX_LAYER_WIDTH) || (layer_info->height > DPU_MAX_LAYER_HEIGHT)) {
        dpu_err("Max layer is 640x640, cur:wh[%ux%u]", layer_info->width, layer_info->height);
        return EXT_ERR_GFX_INVALID_PARAM;
    }

    if (layer_info->layer_fmt >= EXT_GFX_FMT_MAX) {
        dpu_err("invalid layer fmt:%d", layer_info->layer_fmt);
        return EXT_ERR_GFX_INVALID_PARAM;
    }

    if (layer_info->layer_id > DPU_MAX_LAYER_ID) {
        dpu_err("invalid layer id:%d", layer_info->layer_id);
        return EXT_ERR_GFX_INVALID_PARAM;
    }

    if (layer_info->compress_mode >= EXT_GFX_COMPRESS_MAX) {
        dpu_err("invalid compress mode:%d", layer_info->compress_mode);
        return EXT_ERR_GFX_INVALID_PARAM;
    }

    dpu_trans_layer_info_api2drv(layer_info, &drv_layer_info);

    dpu_dev_lock();

    if (g_dpu_dev_init_cnt == 0) {
        g_dpu_dev_fd = dpu_open(DPU_DEV_NAME, OSAL_O_RDWR, 0);
    }

    if (g_dpu_dev_fd < 0) {
        g_dpu_dev_fd = -1;
        dpu_dev_unlock();
        dpu_err("open dpu device failed!");
        return EXT_FAILURE;
    }

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_CREATELAYER, &drv_layer_info);
    if (ret != EXT_SUCCESS) {
        if (g_dpu_dev_init_cnt == 0) {
            dpu_close(g_dpu_dev_fd);
        }
        *layer = EXT_DPU_INVALID_HANDLE;
        dpu_dev_unlock();
        dpu_err("uapi_dpu_create_layer failed, ret = %#x!", ret);
        return EXT_FAILURE;
    }

    *layer = (td_handle)(drv_layer_info.layer_id);

    g_dpu_dev_init_cnt++;

    dpu_dev_unlock();

    return ret;
}

td_s32 uapi_dpu_destroy_layer(td_handle layer)
{
    td_s32 ret;
    drv_dpu_layer_id layer_id;

    get_dpu_layer_id_return(layer, layer_id);

    dpu_dev_lock();

    if (g_dpu_dev_init_cnt == 0) {
        dpu_dev_unlock();
        return EXT_SUCCESS;
    }

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_DESTROYLAYER, &layer_id);
    if (ret != EXT_SUCCESS) {
        dpu_dev_unlock();
        dpu_err("uapi_dpu_destroy_layer failed, ret = %#x!", ret);
        return ret;
    }

    g_dpu_dev_init_cnt--;

    if (g_dpu_dev_init_cnt == 0) {
        dpu_close(g_dpu_dev_fd);
        g_dpu_dev_fd = -1;
    }

    dpu_dev_unlock();

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_set_layer_alpha(td_handle layer, td_u8 alpha)
{
    td_s32 ret;
    drv_dpu_layer_id layer_id;
    dpu_layer_alpha_cmd alpha_cmd = {0};

    get_dpu_layer_id_return(layer, layer_id);
    check_dpu_dev_fd_return();

    alpha_cmd.layer_id = layer_id;
    alpha_cmd.alpha    = alpha;

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_PUTLAYERALPHA, &alpha_cmd);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_set_layer_alpha failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_get_layer_alpha(td_handle layer, td_u8 *alpha)
{
    td_s32 ret;
    drv_dpu_layer_id layer_id;
    dpu_layer_alpha_cmd alpha_cmd = {0};

    check_dpu_null_pointer_return(alpha);
    get_dpu_layer_id_return(layer, layer_id);
    check_dpu_dev_fd_return();

    alpha_cmd.layer_id = layer_id;

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_GETLAYERALPHA, &alpha_cmd);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_get_layer_alpha failed, ret = %#x!", ret);
        return ret;
    }

    *alpha = alpha_cmd.alpha;

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_set_layer_pos(td_handle layer, td_s32 pos_x, td_s32 pos_y)
{
    td_s32 ret;
    drv_dpu_layer_id layer_id;
    dpu_layer_pos_cmd pos_cmd = {0};

    get_dpu_layer_id_return(layer, layer_id);
    check_dpu_dev_fd_return();

    pos_cmd.layer_id = layer_id;
    pos_cmd.pos_x    = pos_x;
    pos_cmd.pos_y    = pos_y;

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_PUTLAYERPOS, &pos_cmd);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_set_layer_pos failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_get_layer_pos(td_handle layer, td_s32 *pos_x, td_s32 *pos_y)
{
    td_s32 ret;
    drv_dpu_layer_id layer_id;
    dpu_layer_pos_cmd pos_cmd = {0};

    check_dpu_null_pointer_return(pos_x);
    check_dpu_null_pointer_return(pos_y);
    get_dpu_layer_id_return(layer, layer_id);
    check_dpu_dev_fd_return();

    pos_cmd.layer_id = layer_id;

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_GETLAYERPOS, &pos_cmd);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_get_layer_pos failed, ret = %#x!", ret);
        return ret;
    }

    *pos_x = pos_cmd.pos_x;
    *pos_y = pos_cmd.pos_y;

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_set_layer_zorder(td_handle layer, ext_dpu_zorder_mode zorder)
{
    td_s32 ret;
    drv_dpu_layer_id layer_id;
    dpu_layer_zorder_cmd zorder_cmd = {0};

    get_dpu_layer_id_return(layer, layer_id);
    check_dpu_dev_fd_return();

    zorder_cmd.layer_id = layer_id;
    zorder_cmd.zorder   = (td_u32)zorder;

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_PUTLAYERZORDER, &zorder_cmd);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_set_layer_zorder failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_get_layer_zorder(td_handle layer, td_u32 *zorder)
{
    td_s32 ret;
    drv_dpu_layer_id layer_id;
    dpu_layer_zorder_cmd zorder_cmd = {0};

    check_dpu_null_pointer_return(zorder);
    get_dpu_layer_id_return(layer, layer_id);
    check_dpu_dev_fd_return();

    zorder_cmd.layer_id = layer_id;

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_GETLAYERZORDER, &zorder_cmd);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_get_layer_zorder failed, ret = %#x!", ret);
        return ret;
    }

    *zorder = zorder_cmd.zorder;

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_show_layer(td_handle layer)
{
    td_s32 ret;
    drv_dpu_layer_id layer_id;

    get_dpu_layer_id_return(layer, layer_id);
    check_dpu_dev_fd_return();

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_SHOWLAYER, &layer_id);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_show_layer failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_hide_layer(td_handle layer)
{
    td_s32 ret;
    drv_dpu_layer_id layer_id;

    get_dpu_layer_id_return(layer, layer_id);
    check_dpu_dev_fd_return();

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_HIDELAYER, &layer_id);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_hide_layer failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_set_layer_info(td_handle layer, const ext_dpu_surface *surface)
{
    td_s32 ret;
    drv_dpu_layer_id layer_id;
    dpu_layer_info_cmd layer_info_cmd = {0};

    check_dpu_null_pointer_return(surface);
    get_dpu_layer_id_return(layer, layer_id);
    check_dpu_dev_fd_return();

    layer_info_cmd.layer_id = layer_id;
    dpu_trans_surface_api2drv(surface, &layer_info_cmd.surface);

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_PUTLAYERINFO, &layer_info_cmd);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_set_layer_info failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_get_layer_info(td_handle layer, ext_dpu_surface *surface)
{
    td_s32 ret;
    drv_dpu_layer_id layer_id;
    dpu_layer_info_cmd layer_info_cmd = {0};

    check_dpu_null_pointer_return(surface);
    get_dpu_layer_id_return(layer, layer_id);
    check_dpu_dev_fd_return();

    layer_info_cmd.layer_id = layer_id;

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_GETLAYERINFO, &layer_info_cmd);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_set_layer_info failed, ret = %#x!", ret);
        return ret;
    }

    dpu_trans_surface_drv2api(surface, &layer_info_cmd.surface);

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_set_layer_colorkey(td_handle layer, const ext_dpu_colorkey *colorkey)
{
    td_s32 ret;
    drv_dpu_layer_id layer_id;
    dpu_layer_colokey_cmd colorkey_cmd = {0};

    check_dpu_null_pointer_return(colorkey);
    get_dpu_layer_id_return(layer, layer_id);
    check_dpu_dev_fd_return();

    colorkey_cmd.layer_id = layer_id;
    colorkey_cmd.colorkey.key_en = colorkey->key_en;
    colorkey_cmd.colorkey.key_value = colorkey->key_value;

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_PUTYLAYERCOLORKYE, &colorkey_cmd);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_set_layer_info failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_get_layer_colorkey(td_handle layer, ext_dpu_colorkey *colorkey)
{
    td_s32 ret;
    drv_dpu_layer_id layer_id;
    dpu_layer_colokey_cmd colorkey_cmd = {0};

    check_dpu_null_pointer_return(colorkey);
    get_dpu_layer_id_return(layer, layer_id);
    check_dpu_dev_fd_return();

    colorkey_cmd.layer_id = layer_id;

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_GETYLAYERCOLORKYE, &colorkey_cmd);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_set_layer_info failed, ret = %#x!", ret);
        return ret;
    }

    colorkey->key_en = colorkey_cmd.colorkey.key_en;
    colorkey->key_value = colorkey_cmd.colorkey.key_value;

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_refresh_layers(td_void)
{
    td_s32 ret;

    check_dpu_dev_fd_return();

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_REFRESHLAYERS, TD_NULL);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_refresh_layers failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_wait_layer_vsync(td_void)
{
    td_s32 ret;

    check_dpu_dev_fd_return();

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_WAITLAYERVSYNC, TD_NULL);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_wait_layer_vsync failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_set_layer_vblank(td_handle layer, td_bool vblank)
{
    td_s32 ret;
    drv_dpu_layer_id layer_id;
    dpu_layer_vlank_cmd vblank_cmd = {0};

    get_dpu_layer_id_return(layer, layer_id);
    check_dpu_dev_fd_return();

    vblank_cmd.layer_id = layer_id;
    vblank_cmd.vblank   = vblank;

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_PUTLAYERVBLANK, &vblank_cmd);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_set_layer_vblank failed, ret = %#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_get_layer_vblank(td_handle layer, td_bool *vblank)
{
    td_s32 ret;
    drv_dpu_layer_id layer_id;
    dpu_layer_vlank_cmd vblank_cmd = {0};

    check_dpu_null_pointer_return(vblank);
    get_dpu_layer_id_return(layer, layer_id);
    check_dpu_dev_fd_return();

    vblank_cmd.layer_id = layer_id;

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_GETLAYERVBLANK, &vblank_cmd);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_set_layer_vblank failed, ret = %#x!", ret);
        return ret;
    }

    *vblank = vblank_cmd.vblank;

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_query_layer_state(td_handle layer)
{
    td_s32 ret;
    drv_dpu_layer_id layer_id;
    dpu_layer_state_cmd state_cmd = {0};

    get_dpu_layer_id_return(layer, layer_id);
    check_dpu_dev_fd_return();

    state_cmd.layer_id = layer_id;

    ret = dpu_ioctl(g_dpu_dev_fd, DRV_DPU_CMD_QUERYLAYERSTATE, &state_cmd);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_dpu_query_layer_state failed, ret = %#x!", ret);
        return ret;
    }

    return state_cmd.state;
}
