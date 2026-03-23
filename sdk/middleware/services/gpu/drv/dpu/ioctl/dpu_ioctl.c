/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dpu ioctl funcions
 */
#include "dpu_ioctl.h"
#include "drv_dpu_struct.h"
#include "dpu_layer.h"
#include "dpu_dfx.h"

td_s32 dpu_ioctl_create_layer(td_u32 cmd, td_void *arg, td_void *file)
{
    drv_dpu_layer_info *layer_info;

    uapi_unused(cmd);
    uapi_unused(file);

    layer_info = (drv_dpu_layer_info *)arg;
    if (layer_info == TD_NULL) {
        dpu_err("layer_info is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    return dpu_layer_create(layer_info);
}

td_s32 dpu_ioctl_destroy_layer(td_u32 cmd, td_void *arg, td_void *file)
{
    drv_dpu_layer_id layer_id;

    uapi_unused(cmd);
    uapi_unused(file);

    if (arg == TD_NULL) {
        dpu_err("arg is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    layer_id = *(drv_dpu_layer_id *)arg;
    if (layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("layer_id is invalid, id = %d!", layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    return dpu_layer_destroy(layer_id);
}

td_s32 dpu_ioctl_set_layer_alpha(td_u32 cmd, td_void *arg, td_void *file)
{
    dpu_layer_alpha_cmd *alpha_cmd = TD_NULL;

    uapi_unused(cmd);
    uapi_unused(file);

    alpha_cmd = (dpu_layer_alpha_cmd *)arg;
    if (alpha_cmd == TD_NULL) {
        dpu_err("arg is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    if (alpha_cmd->layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("layer_id is invalid, id = %d!", alpha_cmd->layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    return dpu_layer_set_alpha(alpha_cmd->layer_id, alpha_cmd->alpha);
}

td_s32 dpu_ioctl_get_layer_alpha(td_u32 cmd, td_void *arg, td_void *file)
{
    dpu_layer_alpha_cmd *alpha_cmd = TD_NULL;

    uapi_unused(cmd);
    uapi_unused(file);

    alpha_cmd = (dpu_layer_alpha_cmd *)arg;
    if (alpha_cmd == TD_NULL) {
        dpu_err("arg is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    if (alpha_cmd->layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("layer_id is invalid, id = %d!", alpha_cmd->layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    return dpu_layer_get_alpha(alpha_cmd->layer_id, &alpha_cmd->alpha);
}

td_s32 dpu_ioctl_set_layer_pos(td_u32 cmd, td_void *arg, td_void *file)
{
    dpu_layer_pos_cmd *pos_cmd = TD_NULL;

    uapi_unused(cmd);
    uapi_unused(file);

    pos_cmd = (dpu_layer_pos_cmd *)arg;
    if (pos_cmd == TD_NULL) {
        dpu_err("arg is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    if (pos_cmd->layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("layer_id is invalid, id = %d!", pos_cmd->layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    return dpu_layer_set_pos(pos_cmd->layer_id, pos_cmd->pos_x, pos_cmd->pos_y);
}

td_s32 dpu_ioctl_get_layer_pos(td_u32 cmd, td_void *arg, td_void *file)
{
    dpu_layer_pos_cmd *pos_cmd = TD_NULL;

    uapi_unused(cmd);
    uapi_unused(file);

    pos_cmd = (dpu_layer_pos_cmd *)arg;
    if (pos_cmd == TD_NULL) {
        dpu_err("arg is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    if (pos_cmd->layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("layer_id is invalid, id = %d!", pos_cmd->layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    return dpu_layer_get_pos(pos_cmd->layer_id, &pos_cmd->pos_x, &pos_cmd->pos_y);
}

td_s32 dpu_ioctl_set_layer_zorder(td_u32 cmd, td_void *arg, td_void *file)
{
    dpu_layer_zorder_cmd *zorder_cmd = TD_NULL;

    uapi_unused(cmd);
    uapi_unused(file);

    zorder_cmd = (dpu_layer_zorder_cmd *)arg;
    if (zorder_cmd == TD_NULL) {
        dpu_err("arg is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    if (zorder_cmd->layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("layer_id is invalid, id = %u!", (td_u32)zorder_cmd->layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    if (zorder_cmd->zorder >= DRV_DPU_ZORDER_MODE_MAX) {
        dpu_err("zorder is invalid, zorder = %u!", (td_u32)zorder_cmd->zorder);
        return EXT_FAILURE;
    }

    return dpu_layer_set_zorder(zorder_cmd->layer_id, zorder_cmd->zorder);
}

td_s32 dpu_ioctl_get_layer_zorder(td_u32 cmd, td_void *arg, td_void *file)
{
    dpu_layer_zorder_cmd *zorder_cmd = TD_NULL;

    uapi_unused(cmd);
    uapi_unused(file);

    zorder_cmd = (dpu_layer_zorder_cmd *)arg;
    if (zorder_cmd == TD_NULL) {
        dpu_err("arg is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    if (zorder_cmd->layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("layer_id is invalid, id = %d!", zorder_cmd->layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    return dpu_layer_get_zorder(zorder_cmd->layer_id, &zorder_cmd->zorder);
}

td_s32 dpu_ioctl_show_layer(td_u32 cmd, td_void *arg, td_void *file)
{
    drv_dpu_layer_id layer_id;

    uapi_unused(cmd);
    uapi_unused(file);

    if (arg == TD_NULL) {
        dpu_err("arg is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    layer_id = *(drv_dpu_layer_id *)arg;
    if (layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("layer_id is invalid, id = %d!", layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    return dpu_layer_show(layer_id);
}

td_s32 dpu_ioctl_hide_layer(td_u32 cmd, td_void *arg, td_void *file)
{
    drv_dpu_layer_id layer_id;

    uapi_unused(cmd);
    uapi_unused(file);

    if (arg == TD_NULL) {
        dpu_err("arg is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    layer_id = *(drv_dpu_layer_id *)arg;
    if (layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("layer_id is invalid, id = %d!", layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    return dpu_layer_hide(layer_id);
}


td_s32 dpu_ioctl_set_layer_info(td_u32 cmd, td_void *arg, td_void *file)
{
    dpu_layer_info_cmd *layer_info_cmd = TD_NULL;

    uapi_unused(cmd);
    uapi_unused(file);

    layer_info_cmd = (dpu_layer_info_cmd *)arg;
    if (layer_info_cmd == TD_NULL) {
        dpu_err("arg is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    if (layer_info_cmd->layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("layer_id is invalid, id = %d!", layer_info_cmd->layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }
    dfx_dpu_save_fb_to_file(layer_info_cmd->layer_id, &layer_info_cmd->surface);

    return dpu_layer_set_info(layer_info_cmd->layer_id, &layer_info_cmd->surface);
}

td_s32 dpu_ioctl_get_layer_info(td_u32 cmd, td_void *arg, td_void *file)
{
    dpu_layer_info_cmd *layer_info_cmd = TD_NULL;

    uapi_unused(cmd);
    uapi_unused(file);

    layer_info_cmd = (dpu_layer_info_cmd *)arg;
    if (layer_info_cmd == TD_NULL) {
        dpu_err("arg is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    if (layer_info_cmd->layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("layer_id is invalid, id = %d!", layer_info_cmd->layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    return dpu_layer_get_info(layer_info_cmd->layer_id, &layer_info_cmd->surface);
}

td_s32 dpu_ioctl_set_layer_colorkey(td_u32 cmd, td_void *arg, td_void *file)
{
    dpu_layer_colokey_cmd *colokey_cmd = TD_NULL;

    uapi_unused(cmd);
    uapi_unused(file);

    colokey_cmd = (dpu_layer_colokey_cmd *)arg;
    if (colokey_cmd == TD_NULL) {
        dpu_err("arg is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    if (colokey_cmd->layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("layer_id is invalid, id = %d!", colokey_cmd->layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    return dpu_layer_set_colorkey(colokey_cmd->layer_id, &colokey_cmd->colorkey);
}

td_s32 dpu_ioctl_get_layer_colorkey(td_u32 cmd, td_void *arg, td_void *file)
{
    dpu_layer_colokey_cmd *colokey_cmd = TD_NULL;

    uapi_unused(cmd);
    uapi_unused(file);

    colokey_cmd = (dpu_layer_colokey_cmd *)arg;
    if (colokey_cmd == TD_NULL) {
        dpu_err("arg is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    if (colokey_cmd->layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("layer_id is invalid, id = %d!", colokey_cmd->layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    return dpu_layer_get_colorkey(colokey_cmd->layer_id, &colokey_cmd->colorkey);
}

td_s32 dpu_ioctl_refresh_layers(td_u32 cmd, td_void *arg, td_void *file)
{
    uapi_unused(cmd);
    uapi_unused(arg);
    uapi_unused(file);

    return dpu_layer_refresh();
}

td_s32 dpu_ioctl_wait_layer_vsync(td_u32 cmd, td_void *arg, td_void *file)
{
    uapi_unused(cmd);
    uapi_unused(arg);
    uapi_unused(file);

    return dpu_layer_wait_vsync();
}

td_s32 dpu_ioctl_set_layer_vblank(td_u32 cmd, td_void *arg, td_void *file)
{
    dpu_layer_vlank_cmd *vlank_cmd = TD_NULL;

    uapi_unused(cmd);
    uapi_unused(file);

    vlank_cmd = (dpu_layer_vlank_cmd *)arg;
    if (vlank_cmd == TD_NULL) {
        dpu_err("arg is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    if (vlank_cmd->layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("layer_id is invalid, id = %d!", vlank_cmd->layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    return dpu_layer_set_vblank(vlank_cmd->layer_id, vlank_cmd->vblank);
}

td_s32 dpu_ioctl_get_layer_vblank(td_u32 cmd, td_void *arg, td_void *file)
{
    dpu_layer_vlank_cmd *vlank_cmd = TD_NULL;

    uapi_unused(cmd);
    uapi_unused(file);

    vlank_cmd = (dpu_layer_vlank_cmd *)arg;
    if (vlank_cmd == TD_NULL) {
        dpu_err("arg is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    if (vlank_cmd->layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("layer_id is invalid, id = %d!", vlank_cmd->layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    return dpu_layer_get_vblank(vlank_cmd->layer_id, &vlank_cmd->vblank);
}

td_s32 dpu_ioctl_query_layer_state(td_u32 cmd, td_void *arg, td_void *file)
{
    drv_dpu_layer_id layer_id;

    uapi_unused(cmd);
    uapi_unused(file);

    if (arg == TD_NULL) {
        dpu_err("arg is null!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    layer_id = *(drv_dpu_layer_id *)arg;
    if (layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("layer_id is invalid, id = %d!", layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    return dpu_layer_query_state(layer_id);
}
