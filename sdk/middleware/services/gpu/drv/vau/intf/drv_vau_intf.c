/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: vau interface
 */

#include "drv_vau.h"
#include "soc_osal.h"
#include "drv_vau_ioctl.h"
#include "vau_ioctl.h"
#include "vau_isr.h"
#include "vau_mem.h"
#include "vau_job.h"
#include "vau_list.h"
#include "vau_hal.h"
#include "vau_dfx.h"
#include "vau_pre_process.h"
#include "vau_handle.h"
#include "drv_vau_struct.h"
#include "drv_gmmu_intf.h"
#include "pmp_config.h"

#define DRV_VAU_IOCTL_CMD_NUM 20

static td_bool g_vau_suspend_flag = TD_FALSE;

static td_s32 drv_vau_open(td_void *private_data)
{
    uapi_unused(private_data);

    return EXT_SUCCESS;
}

static td_s32 drv_vau_close(td_void *private_data)
{
    uapi_unused(private_data);

    return EXT_SUCCESS;
}

static td_void drv_vau_set_suspend_flag(td_bool flag)
{
    unsigned long flags = 0;
    vau_handle_mgr *vau_handle_list = vau_handle_list_get();

    osal_spin_lock_irqsave(&vau_handle_list->lock, &flags);
    g_vau_suspend_flag = flag;
    osal_spin_unlock_irqrestore(&vau_handle_list->lock, &flags);
}

td_bool drv_vau_get_suspend_flag(td_void)
{
    unsigned long flags = 0;
    vau_handle_mgr *vau_handle_list = vau_handle_list_get();

    osal_spin_lock_irqsave(&vau_handle_list->lock, &flags);
    td_bool suspend_flag = g_vau_suspend_flag;
    osal_spin_unlock_irqrestore(&vau_handle_list->lock, &flags);
    return suspend_flag;
}

td_s32 drv_vau_suspend(td_void *private_data)
{
    uapi_unused(private_data);

    td_s32 ret = drv_vau_wait_all_done();
    vau_dfx_info *dfx_info = vau_dfx_get_info();
    dfx_info->suspend_cnt++;

    vau_dbg("vau suspend start");
    vau_hal_suspend();
    drv_vau_set_suspend_flag(TD_TRUE);
    vau_dbg("vau suspend ok");

#ifdef SUPPORT_GPU_GMMU
    pmp_configure_gmmu_region(0);
    (td_void)drv_gmmu_suspend(NULL);
#endif

    return ret;
}

td_s32 drv_vau_resume(td_void *private_data)
{
    uapi_unused(private_data);

    vau_dfx_info *dfx_info = vau_dfx_get_info();
    dfx_info->resume_cnt++;

    vau_ip_dcmp_tmp();

    vau_dbg("vau resume start");

    vau_hal_resume();
#ifdef CONFIG_CLDO1PD_SUPPORT
    vau_mem_draw_pool_init();
#endif
    drv_vau_set_suspend_flag(TD_FALSE);
    vau_dbg("vau resume ok");

#ifdef SUPPORT_GPU_GMMU
    (void)drv_gmmu_resume(NULL);
    pmp_configure_gmmu_region(1);
#endif

    return EXT_SUCCESS;
}

td_s32 vau_open(const td_char *path_name, td_s32 flags, td_s32 mode)
{
    uapi_unused(path_name);
    uapi_unused(flags);
    uapi_unused(mode);

    return EXT_SUCCESS;
}

td_s32 vau_close(td_s32 module_idx)
{
    uapi_unused(module_idx);

    return EXT_SUCCESS;
}

static osal_ioctl_cmd g_vau_ioctl_cmd_list[DRV_VAU_IOCTL_CMD_NUM] = {
    { DRV_VAU_CMD_CREATE,              vau_ioctl_create        },
    { DRV_VAU_CMD_DESTROY,             vau_ioctl_destroy       },
    { DRV_VAU_CMD_FILL,                vau_ioctl_fill          },
    { DRV_VAU_CMD_BLIT,                vau_ioctl_blit          },
    { DRV_VAU_CMD_COMPOSE,             vau_ioctl_compose       },
    { DRV_VAU_CMD_SUBMIT,              vau_ioctl_submit        },
    { DRV_VAU_CMD_CANCEL,              vau_ioctl_cancel        },
    { DRV_VAU_CMD_WAIT_FOR_DONE,       vau_ioctl_wait_for_done },
    { DRV_VAU_CMD_SET_CUBIC_THRESHOLD, vau_ioctl_set_cubic_threshold },
    { DRV_VAU_IOCTL_CMD_NUM - 1, TD_NULL }
};

static osal_fileops g_vau_file_fops = {
    .open     = drv_vau_open,
    .release  = drv_vau_close,
    .cmd_list = g_vau_ioctl_cmd_list,
    .cmd_cnt  = DRV_VAU_IOCTL_CMD_NUM,
};

static osal_pmops g_vau_ops = {
    .pm_suspend        = drv_vau_suspend,
    .pm_resume         = drv_vau_resume,
};

static osal_dev g_vau_register_data = {
    .minor = 0,
    .fops  = &g_vau_file_fops,
    .pmops = &g_vau_ops,
};

static td_void vau_print_api(td_u32 ioctl_cmd, td_void *para)
{
    td_handle handle = *(td_handle *)para;

    if (ioctl_cmd == DRV_VAU_CMD_CREATE) {
        vau_err("uapi_vau_create:handle = %u", handle);
    } else if (ioctl_cmd == DRV_VAU_CMD_DESTROY) {
        vau_err("uapi_vau_destroy:handle = %u", handle);
    } else if (ioctl_cmd == DRV_VAU_CMD_CANCEL) {
        vau_err("uapi_vau_cancel:handle = %u", handle);
    } else if (ioctl_cmd == DRV_VAU_CMD_WAIT_FOR_DONE) {
        vau_err("uapi_vau_wait_for_done:handle = %u", handle);
    } else if (ioctl_cmd == DRV_VAU_CMD_FILL) {
        drv_vau_blit_cmd *blit_cmd = (drv_vau_blit_cmd *)para;
        handle = blit_cmd->hanlde;
        vau_err("uapi_vau_fill:handle = %u", handle);
    } else if (ioctl_cmd == DRV_VAU_CMD_BLIT) {
        drv_vau_blit_cmd *blit_cmd = (drv_vau_blit_cmd *)para;
        handle = blit_cmd->hanlde;
        vau_err("uapi_vau_render:handle = %u", handle);
    } else if (ioctl_cmd == DRV_VAU_CMD_COMPOSE) {
        drv_vau_blit_cmd *blit_cmd = (drv_vau_blit_cmd *)para;
        handle = blit_cmd->hanlde;
        vau_err("uapi_vau_compose:handle = %u", handle);
    } else if (ioctl_cmd == DRV_VAU_CMD_SUBMIT) {
        drv_vau_submit_cmd *submit_cmd = (drv_vau_submit_cmd *)para;
        handle = submit_cmd->handle;
        vau_err("uapi_vau_submit:handle = %u", handle);
    } else {
    }
}
td_s32 vau_ioctl(td_s32 module_idx, td_ulong ioctl_cmd, td_void *para)
{
    uapi_unused(module_idx);

    if (ioctl_cmd >= DRV_VAU_IOCTL_CMD_NUM) {
        vau_err("Invalid cmd, cmd = %lu!", ioctl_cmd);
        return EXT_FAILURE;
    }

    if (g_vau_suspend_flag == TD_TRUE) {
        vau_err("vau is in suspend state!, suspend_flag = %u", g_vau_suspend_flag);
        return EXT_FAILURE;
    }

    td_s32 ret =  (td_s32)g_vau_ioctl_cmd_list[ioctl_cmd].handler((td_u32)ioctl_cmd, para, TD_NULL);
    
    vau_dfx_info *vau_info = vau_dfx_get_info();
    if (vau_info->is_print_vau_api) {
        vau_print_api((td_u32)ioctl_cmd, para);
    }
    return ret;
}

static td_s32 vau_register_device(td_void)
{
    td_s32 ret;

    ret = snprintf_s(g_vau_register_data.name, OSAL_DEV_NAME_LEN - 1, strlen(VAU_DEV_NAME), "%s", VAU_DEV_NAME);
    if (ret <= 0) {
        vau_err("snprintf_s failed! ret=%d.\n", ret);
        return EXT_FAILURE;
    }

    if (osal_dev_register(&g_vau_register_data) < 0) {
        vau_err("register vau failed.\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_void vau_unregister_device(td_void)
{
    osal_dev_unregister(&g_vau_register_data);
}

td_s32 uapi_vau_module_init(void)
{
    td_s32 ret;
#ifdef SUPPORT_GPU_GMMU
    ret = drv_gmmu_module_init();
    if (ret != EXT_SUCCESS) {
        vau_err("drv_gmmu_module_init failed, ret=%#x!", ret);
        return ret;
    }
    pmp_configure_gmmu_region(1);
#endif
    vau_ip_dcmp_tmp();

    ret = vau_mem_init();
    if (ret != EXT_SUCCESS) {
        vau_err("vau_mem_init failed, ret=%#x!", ret);
        return ret;
    }

    ret = vau_list_init();
    if (ret != EXT_SUCCESS) {
        vau_mem_deinit();
        vau_err("vau_list_init failed, ret=%#x!", ret);
        return ret;
    }

    ret = vau_register_device();
    if (ret != EXT_SUCCESS) {
        vau_mem_deinit();
        vau_list_deinit();
        vau_err("vau_register_device failed, ret=%#x!", ret);
        return ret;
    }

    vau_hal_init();

    ret = vau_isr_init();
    if (ret != EXT_SUCCESS) {
        vau_mem_deinit();
        vau_list_deinit();
        vau_hal_deinit();
        vau_unregister_device();
        vau_err("vau_isr_init fail, ret=%#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_void uapi_vau_module_exit(void)
{
    vau_isr_deinit();
    vau_hal_deinit();
    vau_unregister_device();
    vau_list_deinit();
    vau_mem_deinit();
#ifdef SUPPORT_GPU_GMMU
    pmp_configure_gmmu_region(0);
#endif
}
