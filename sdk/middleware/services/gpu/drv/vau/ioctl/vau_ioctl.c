/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: vau ioctl file
 */

#include "vau_ioctl.h"
#include "drv_vau_struct.h"
#include "vau_job.h"
#include "vau_dfx.h"
#include "vau_hal_apb.h"

td_s32 vau_ioctl_create(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_u32 time_start = 0;
    td_u32 time_end = 0;
    td_handle *handle = (td_handle *)arg;

    uapi_unused(cmd);
    uapi_unused(file);

    vau_dfx_info *vau_info = vau_dfx_get_info();
    vau_dfx_get_time_us(&time_start);
    vau_info->func_info.func_cnt.create_cnt++;

    if (handle == TD_NULL) {
        vau_err("arg is null!");
        return EXT_FAILURE;
    }

    ret = drv_vau_begin_job(handle, TD_NULL);

    vau_dfx_get_time_us(&time_end);
    vau_info->func_info.time_info.create_cost = (td_float)(time_end - time_start);
    return ret;
}

td_s32 vau_ioctl_destroy(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_u32 time_start = 0;
    td_u32 time_end = 0;
    td_handle handle;

    uapi_unused(cmd);
    uapi_unused(file);

    vau_dfx_info *vau_info = vau_dfx_get_info();
    vau_dfx_get_time_us(&time_start);
    vau_info->func_info.func_cnt.destroy_cnt++;

    if (arg == TD_NULL) {
        vau_err("arg is null!");
        return EXT_FAILURE;
    }

    handle = *(td_handle *)arg;
    ret = drv_vau_force_cancel_job(handle); /* same as vau_ioctl_cancel */

    vau_dfx_get_time_us(&time_end);
    vau_info->func_info.time_info.destroy_cost = (td_float)(time_end - time_start);

    return ret;
}

td_s32 vau_ioctl_fill(td_u32 cmd, td_void *arg, td_void *file)
{
    drv_vau_blit_cmd *blit_cmd = (drv_vau_blit_cmd *)arg;

    uapi_unused(cmd);
    uapi_unused(file);

    return drv_vau_fill(blit_cmd->hanlde, &blit_cmd->surface_list);
}

td_s32 vau_ioctl_blit(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_u32 time_start = 0;
    td_u32 time_end = 0;
    drv_vau_blit_cmd *blit_cmd = (drv_vau_blit_cmd *)arg;

    uapi_unused(cmd);
    uapi_unused(file);

    vau_dfx_info *vau_info = vau_dfx_get_info();
    vau_dfx_get_time_us(&time_start);
    vau_info->func_info.func_cnt.blit_cnt++;

    if (blit_cmd == TD_NULL) {
        vau_err("arg is null!");
        return EXT_FAILURE;
    }

    ret = drv_vau_blit(blit_cmd->hanlde, &blit_cmd->surface_list);

    vau_dfx_get_time_us(&time_end);
    vau_info->func_info.time_info.blit_cost = (td_float)(time_end - time_start);

    return ret;
}

td_s32 vau_ioctl_compose(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_u32 time_start = 0;
    td_u32 time_end = 0;
    drv_vau_blit_cmd *blit_cmd = (drv_vau_blit_cmd *)arg;

    uapi_unused(cmd);
    uapi_unused(file);

    vau_dfx_info *vau_info = vau_dfx_get_info();
    vau_dfx_get_time_us(&time_start);
    vau_info->func_info.func_cnt.compose_cnt++;

    if (blit_cmd == TD_NULL) {
        vau_err("arg is null!");
        return EXT_FAILURE;
    }

    ret = drv_vau_compose(blit_cmd->hanlde, &blit_cmd->surface_list);

    vau_dfx_get_time_us(&time_end);
    vau_info->func_info.time_info.compose_cost = (td_float)(time_end - time_start);

    return ret;
}

td_s32 vau_ioctl_submit(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_u32 time_start = 0;
    td_u32 time_end = 0;
    drv_vau_callback_func callback_func = {0};
    drv_vau_submit_cmd *submit_cmd = (drv_vau_submit_cmd *)arg;

    uapi_unused(cmd);
    uapi_unused(file);

    vau_dfx_info *vau_info = vau_dfx_get_info();
    vau_dfx_get_time_us(&time_start);
    vau_info->func_info.func_cnt.submit_usr_cnt++;

    if (submit_cmd == TD_NULL) {
        vau_err("arg is null!");
        return EXT_FAILURE;
    }

    ret = drv_vau_end_job(submit_cmd->handle, submit_cmd->is_sync, submit_cmd->time_out, callback_func);

    vau_dfx_get_time_us(&time_end);
    vau_info->func_info.time_info.submit_cost = (td_float)(time_end - time_start);

    return ret;
}

td_s32 vau_ioctl_cancel(td_u32 cmd, td_void *arg, td_void *file)
{
    vau_dfx_info *vau_info = vau_dfx_get_info();
    vau_info->func_info.func_cnt.cancel_cnt++;
    return vau_ioctl_destroy(cmd, arg, file);
}

td_s32 vau_ioctl_wait_for_done(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_u32 time_start = 0;
    td_u32 time_end = 0;
    td_handle *handle = (td_handle *)arg;

    uapi_unused(cmd);
    uapi_unused(file);

    vau_dfx_info *vau_info = vau_dfx_get_info();
    vau_dfx_get_time_us(&time_start);
    vau_info->func_info.func_cnt.wait_for_done_cnt++;

    if (handle == TD_NULL) {
        vau_err("arg is null!");
        return EXT_FAILURE;
    }

    ret = drv_vau_wait_for_done(*handle, 0);

    vau_dfx_get_time_us(&time_end);
    vau_info->func_info.time_info.wait_for_done_cost = (td_float)(time_end - time_start);

    return ret;
}

td_s32 vau_ioctl_set_cubic_threshold(td_u32 cmd, td_void *arg, td_void *file)
{
    uapi_unused(cmd);
    uapi_unused(file);

    td_float cubic_threshold = *(td_float *)arg;

    // cubic_threshold Range: 1.0/0.5/0.25/0.125
    if ((cubic_threshold != 0.125) && (cubic_threshold != 0.25) && (cubic_threshold != 0.5) &&
        (cubic_threshold != 1.0)) {
        vau_err("input val [%f] is wrong, need equal 0.125 or 0.25 or 0.5 or 1.0", cubic_threshold);
        return EXT_FAILURE;
    }

    vau_hal_apb_set_cubic_threshold(cubic_threshold);
    return EXT_SUCCESS;
}
