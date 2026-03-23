/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: drv jpeg intf
 */

#include "drv_jpeg_intf.h"

#include <securec.h>
#include "soc_osal.h"
#include "drv_jpeg_ioctl.h"
#include "jpeg_ioctl.h"
#include "jpeg_dev.h"
#include "jpeg_isr.h"
#include "jpeg_context.h"
#ifdef GPU_DFX_VIDEO_SUB_RESET
#include "lcd_bus.h"
#include "drv_dpu.h"
#include "drv_vau.h"
#endif

/*-------------------------------- macro define -------------------------------------------*/

#define DRV_JPEG_IOCTL_CMD_NUM  5

static td_bool g_jpeg_suspend_flag = TD_FALSE;
/*-------------------------------- func declares ------------------------------------------*/

static td_s32 drv_jpeg_open(td_void *private_data);
static td_s32 drv_jpeg_close(td_void *private_data);
td_s32 drv_jpeg_suspend(td_void *private_data);
td_s32 drv_jpeg_resume(td_void *private_data);

/*-------------------------------- par define ---------------------------------------------*/

static osal_ioctl_cmd g_jpeg_ioctl_cmd_list[DRV_JPEG_IOCTL_CMD_NUM] = {
    { DRV_JPEG_CMD_DECODE,        jpeg_ioctl_start_decompress },
    { DRV_JPEG_CMD_GETINTSTATUS,  jpeg_ioctl_get_decompress_status },
    { DRV_JPEG_CMD_CREATE,        jpeg_ioctl_decomress_create },
    { DRV_JPEG_CMD_DESTROY,       jpeg_ioctl_decompress_destroy },
    { DRV_JPEG_IOCTL_CMD_NUM - 1, TD_NULL },
};

static osal_fileops g_jpeg_file_fops = {
    .open     = drv_jpeg_open,
    .release  = drv_jpeg_close,
    .cmd_list = g_jpeg_ioctl_cmd_list,
    .cmd_cnt  = DRV_JPEG_IOCTL_CMD_NUM,
};

static osal_pmops g_jpeg_ops = {
    .pm_suspend = drv_jpeg_suspend,
    .pm_resume  = drv_jpeg_resume,
};

static osal_dev g_jpeg_register_data = {
    .minor = 0,
    .fops  = &g_jpeg_file_fops,
    .pmops = &g_jpeg_ops,
};

/*-------------------------------- func release -------------------------------------------*/

static td_s32 drv_jpeg_open(td_void *private_data)
{
    uapi_unused(private_data);

    return EXT_SUCCESS;
}

static td_s32 drv_jpeg_close(td_void *private_data)
{
    uapi_unused(private_data);

    return EXT_SUCCESS;
}

static td_void drv_jpeg_set_suspend_flag(td_bool flag)
{
    unsigned long flags = 0;
    drv_jpeg_context *jpeg_context = jpeg_context_get();

    osal_spin_lock_irqsave(&jpeg_context->lock, &flags);
    g_jpeg_suspend_flag = flag;
    osal_spin_unlock_irqrestore(&jpeg_context->lock, &flags);
}

td_s32 drv_jpeg_suspend(td_void *private_data)
{
    uapi_unused(private_data);

    td_s32 ret = jpeg_wait_all_finish();
    jpeg_suspend();
    drv_jpeg_set_suspend_flag(TD_TRUE);
    return ret;
}

td_s32 drv_jpeg_resume(td_void *private_data)
{
    uapi_unused(private_data);

    jpeg_resume();
    drv_jpeg_set_suspend_flag(TD_FALSE);
    return EXT_SUCCESS;
}

td_s32 jpeg_open(const td_char *path_name, td_s32 flags, td_s32 mode)
{
    uapi_unused(path_name);
    uapi_unused(flags);
    uapi_unused(mode);

    return jpeg_dev_open();
}

td_s32 jpeg_close(td_s32 module_idx)
{
    uapi_unused(module_idx);

    return jpeg_dev_close();
}

td_s32 jpeg_ioctl(td_s32 module_idx, td_ulong ioctl_cmd, td_void *para)
{
    uapi_unused(module_idx);

    if (ioctl_cmd >= DRV_JPEG_IOCTL_CMD_NUM) {
        jpeg_err("Invalid cmd, cmd = %lu!", ioctl_cmd);
        return EXT_FAILURE;
    }

    if (g_jpeg_suspend_flag == TD_TRUE) {
        jpeg_err("jpeg is in suspend state!, suspend_flag = %u", g_jpeg_suspend_flag);
        return EXT_FAILURE;
    }

    return (td_s32)g_jpeg_ioctl_cmd_list[ioctl_cmd].handler((td_u32)ioctl_cmd, para, TD_NULL);
}

static td_s32 jpeg_register_device(td_void)
{
    td_s32 ret;

    ret = snprintf_s(g_jpeg_register_data.name, OSAL_DEV_NAME_LEN - 1, strlen(JPEG_DEV_NAME), "%s", JPEG_DEV_NAME);
    if (ret <= 0) {
        jpeg_err("snprintf_s failed! ret=%d.\n", ret);
        return EXT_FAILURE;
    }

    if (osal_dev_register(&g_jpeg_register_data) < 0) {
        jpeg_err("register vau failed.\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_void jpeg_unregister_device(td_void)
{
    osal_dev_unregister(&g_jpeg_register_data);
}

td_s32 uapi_jpeg_module_init(void)
{
    td_s32 ret;
    drv_jpeg_context *jpeg_context = jpeg_context_get();
    if ((jpeg_context != TD_NULL) && (jpeg_context->init_count != 0)) {
        return EXT_SUCCESS;
    }

    jpeg_context_init();
    jpeg_context = jpeg_context_get();
    if (jpeg_context == TD_NULL) {
        jpeg_err("call jpeg_context_get failure");
        return EXT_FAILURE;
    }

    ret = jpeg_register_device();
    if (ret != EXT_SUCCESS) {
        uapi_jpeg_module_exit();
        jpeg_err("jpeg_register_device failed, ret:0x%x", ret);
        return ret;
    }

    ret = jpeg_isr_init();
    if (ret != EXT_SUCCESS) {
        uapi_jpeg_module_exit();
        jpeg_err("call jpeg_isr_init failure");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_void uapi_jpeg_module_exit(void)
{
    jpeg_unregister_device();
    jpeg_context_deinit();
}

td_void uapi_jpeg_video_sub_reset(td_void)
{
#ifdef GPU_DFX_VIDEO_SUB_RESET
    osal_msleep(50); // sleep 50ms
    drv_vau_suspend(TD_NULL);
    drv_dpu_suspend(TD_NULL);

    lcd_get_bus_api()->bus_suspend();

    // video sub reset enable
    uapi_reg_clrbit(0x570000c0, 0);
    uapi_reg_clrbit(0x570000c0, 1);

    osal_msleep(1);

    // video sub reset disable
    uapi_reg_setbit(0x570000c0, 0);
    uapi_reg_setbit(0x570000c0, 1);

    lcd_get_bus_api()->bus_resume();

    drv_dpu_resume(TD_NULL);
    drv_vau_resume(TD_NULL);
#endif
}
