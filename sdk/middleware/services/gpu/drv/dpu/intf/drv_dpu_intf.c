/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dpu interface
 */

#include "drv_dpu.h"
#include "soc_osal.h"
#include "drv_dpu_ioctl.h"
#include "dpu_disp.h"
#include "dpu_layer.h"
#include "dpu_isr.h"
#include "dpu_ioctl.h"
#include "dpu_dfx.h"
#include "dpu_qspi.h"

static td_bool g_is_qspi = TD_FALSE;
static td_bool g_dpu_suspend_flag = TD_FALSE;

#define DRV_DPU_IOCTL_CMD_NUM 20

static td_s32 drv_dpu_open(td_void *private_data)
{
    uapi_unused(private_data);

    return EXT_SUCCESS;
}

static td_s32 drv_dpu_close(td_void *private_data)
{
    uapi_unused(private_data);

    return EXT_SUCCESS;
}

static td_void drv_dpu_set_suspend_flag(td_bool flag)
{
    unsigned long flags = 0;
    dpu_disp_info *disp_info = dpu_disp_get_info();

    osal_spin_lock_irqsave(&disp_info->disp_lock, &flags);
    g_dpu_suspend_flag = flag;
    osal_spin_unlock_irqrestore(&disp_info->disp_lock, &flags);
}

td_bool drv_dpu_get_suspend_flag(td_void)
{
    unsigned long flags = 0;
    dpu_disp_info *disp_info = dpu_disp_get_info();

    osal_spin_lock_irqsave(&disp_info->disp_lock, &flags);
    td_bool suspend_flag = g_dpu_suspend_flag;
    osal_spin_unlock_irqrestore(&disp_info->disp_lock, &flags);
    return suspend_flag;
}


td_s32 drv_dpu_suspend(td_void *private_data)
{
    uapi_unused(private_data);

    if (g_is_qspi == TD_TRUE) {
        return EXT_SUCCESS;
    }

    td_s32 ret = dpu_wait_all_finish();

    dpu_dfx_info *dfx_info = dpu_dfx_get_info();
    dfx_info->suspend_cnt++;

    dpu_dbg("dpu suspend start");
    dpu_hal_deinit();
    dpu_layer_suspend();
    drv_dpu_set_suspend_flag(TD_TRUE);
    dpu_dbg("dpu suspend ok");

    return ret;
}

td_s32 drv_dpu_resume(td_void *private_data)
{
    uapi_unused(private_data);

    if (g_is_qspi == TD_TRUE) {
        return EXT_SUCCESS;
    }

    dpu_dfx_info *dfx_info = dpu_dfx_get_info();
    dfx_info->resume_cnt++;

    dpu_dbg("dpu resume start");
    dpu_hal_init();
    dpu_layer_resume();
    dpu_disp_chl_cfg();
    drv_dpu_set_suspend_flag(TD_FALSE);
    dpu_dbg("dpu resume ok");

    return EXT_SUCCESS;
}

td_s32 dpu_open(const td_char *path_name, td_s32 flags, td_s32 mode)
{
    uapi_unused(path_name);
    uapi_unused(flags);
    uapi_unused(mode);

    return EXT_SUCCESS;
}

td_s32 dpu_close(td_s32 module_idx)
{
    uapi_unused(module_idx);

    return EXT_SUCCESS;
}

static osal_ioctl_cmd g_dpu_ioctl_cmd_list[DRV_DPU_IOCTL_CMD_NUM] = {
    { DRV_DPU_CMD_CREATELAYER,         dpu_ioctl_create_layer },
    { DRV_DPU_CMD_DESTROYLAYER,        dpu_ioctl_destroy_layer },
    { DRV_DPU_CMD_PUTLAYERALPHA,       dpu_ioctl_set_layer_alpha },
    { DRV_DPU_CMD_GETLAYERALPHA,       dpu_ioctl_get_layer_alpha },
    { DRV_DPU_CMD_PUTLAYERPOS,         dpu_ioctl_set_layer_pos },
    { DRV_DPU_CMD_GETLAYERPOS,         dpu_ioctl_get_layer_pos },
    { DRV_DPU_CMD_PUTLAYERZORDER,      dpu_ioctl_set_layer_zorder },
    { DRV_DPU_CMD_GETLAYERZORDER,      dpu_ioctl_get_layer_zorder },
    { DRV_DPU_CMD_SHOWLAYER,           dpu_ioctl_show_layer },
    { DRV_DPU_CMD_HIDELAYER,           dpu_ioctl_hide_layer },
    { DRV_DPU_CMD_PUTLAYERINFO,        dpu_ioctl_set_layer_info },
    { DRV_DPU_CMD_GETLAYERINFO,        dpu_ioctl_get_layer_info },
    { DRV_DPU_CMD_PUTYLAYERCOLORKYE,   dpu_ioctl_set_layer_colorkey },
    { DRV_DPU_CMD_GETYLAYERCOLORKYE,   dpu_ioctl_get_layer_colorkey },
    { DRV_DPU_CMD_REFRESHLAYERS,       dpu_ioctl_refresh_layers },
    { DRV_DPU_CMD_WAITLAYERVSYNC,      dpu_ioctl_wait_layer_vsync },
    { DRV_DPU_CMD_PUTLAYERVBLANK,      dpu_ioctl_set_layer_vblank },
    { DRV_DPU_CMD_GETLAYERVBLANK,      dpu_ioctl_get_layer_vblank },
    { DRV_DPU_CMD_QUERYLAYERSTATE,     dpu_ioctl_query_layer_state },

    { DRV_DPU_IOCTL_CMD_NUM - 1,       TD_NULL },
};

static osal_fileops g_dpu_file_fops = {
    .open     = drv_dpu_open,
    .release  = drv_dpu_close,
    .cmd_list = g_dpu_ioctl_cmd_list,
    .cmd_cnt  = DRV_DPU_IOCTL_CMD_NUM,
};

static osal_pmops g_dpu_ops = {
    .pm_suspend        = drv_dpu_suspend,
    .pm_resume         = drv_dpu_resume,
};

static osal_dev g_dpu_register_data = {
    .minor = 0,
    .fops  = &g_dpu_file_fops,
    .pmops = &g_dpu_ops,
};

td_s32 dpu_ioctl(td_s32 module_idx, td_ulong ioctl_cmd, td_void *para)
{
    uapi_unused(module_idx);

    if (ioctl_cmd >= DRV_DPU_IOCTL_CMD_NUM) {
        dpu_err("Invalid cmd, cmd = %lu!", ioctl_cmd);
        return EXT_FAILURE;
    }

    if (g_dpu_suspend_flag == TD_TRUE) {
        dpu_err("dpu is in suspend state!, suspend_flag = %u", g_dpu_suspend_flag);
        return EXT_FAILURE;
    }

    if (g_is_qspi == TD_TRUE) {
        return dpu_qspi_ioctl(ioctl_cmd, para);
    }

    return (td_s32)g_dpu_ioctl_cmd_list[ioctl_cmd].handler((td_u32)ioctl_cmd, para, TD_NULL);
}

static td_s32 dpu_register_device(td_void)
{
    td_s32 ret;

    ret = snprintf_s(g_dpu_register_data.name, OSAL_DEV_NAME_LEN - 1, strlen(DPU_DEV_NAME), "%s", DPU_DEV_NAME);
    if (ret <= 0) {
        dpu_err("snprintf_s failed! ret=%d.\n", ret);
        return EXT_FAILURE;
    }

    if (osal_dev_register(&g_dpu_register_data) < 0) {
        dpu_err("register vau failed.\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_void dpu_unregister_device(td_void)
{
    osal_dev_unregister(&g_dpu_register_data);
}

td_s32 uapi_dpu_module_init(void)
{
    td_s32 ret;

    g_is_qspi = dpu_qspi_check_is_qspi();
    if (g_is_qspi == TD_TRUE) {
        return dpu_qspi_init();
    }

    ret = dpu_hal_init();
    if (ret != EXT_SUCCESS) {
        dpu_err("dpu_hal_init fail, ret=%#x!", ret);
        return ret;
    }

    ret = dpu_isr_init();
    if (ret != EXT_SUCCESS) {
        dpu_hal_deinit();
        dpu_err("dpu_isr_init fail, ret=%#x!", ret);
        return ret;
    }

    ret = dpu_disp_init();
    if (ret != EXT_SUCCESS) {
        dpu_isr_deinit();
        dpu_hal_deinit();
        dpu_err("dpu_disp_init fail, ret=%#x!", ret);
        return ret;
    }

    ret = dpu_layer_init();
    if (ret != EXT_SUCCESS) {
        dpu_disp_deinit();
        dpu_isr_deinit();
        dpu_hal_deinit();
        dpu_err("dpu_layer_init fail, ret=%#x!", ret);
        return ret;
    }

    ret = dpu_register_device();
    if (ret != EXT_SUCCESS) {
        dpu_layer_deinit();
        dpu_disp_deinit();
        dpu_isr_deinit();
        dpu_hal_deinit();
        dpu_err("dpu_register_device failed, ret=%#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_void uapi_dpu_module_exit(void)
{
    dpu_unregister_device();
    dpu_layer_deinit();
    dpu_disp_deinit();
    dpu_isr_deinit();
    dpu_hal_deinit();
}
