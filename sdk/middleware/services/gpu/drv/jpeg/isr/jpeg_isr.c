/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg isr
 */

#include "jpeg_isr.h"
#include "soc_osal.h"
#include "drv_gfx_port.h"
#include "drv_jpeg_struct.h"
#include "jpeg_hal.h"
#include "jpeg_dfx.h"
#include "jpeg_context.h"

static td_void jpeg_isr_callback(td_u32 param)
{
    td_u32 inttype = 0;
    td_ulong lock_flag = 0;

    uapi_unused(param);
    
    drv_jpeg_context *jpeg_context = jpeg_context_get();

    jpeg_hal_get_inttype(&inttype);
    jpeg_hal_set_inttype(inttype);

    osal_spin_lock_irqsave(&jpeg_context->lock, &lock_flag);
    if ((inttype & 0x1) != 0) {
        jpeg_context->int_type = DRV_JPEG_INT_TYPE_FINISH;
    } else if ((inttype & 0x2) != 0) {
        jpeg_context->int_type = DRV_JPEG_INT_TYPE_ERROR;
    } else if ((inttype & 0x4) != 0) {
        jpeg_context->int_type = DRV_JPEG_INT_TYPE_CONTINUE;
    } else if ((inttype & 0x20) != 0) {
        jpeg_context->int_type = DRV_JPEG_INT_TYPE_LOWDEALY;
    } else if ((inttype & 0x40) != 0) {
        jpeg_context->int_type = DRV_JPEG_INT_TYPE_STREAM_ERROR;
    }
    osal_spin_unlock_irqrestore(&jpeg_context->lock, &lock_flag);

    osal_wait_wakeup(&jpeg_context->wait_int);

    return;
}

td_s32 jpeg_isr_init(td_void)
{
    td_s32 ret;

    ret = osal_irq_request(JPEG_IRQ_NUM, (osal_irq_handler)jpeg_isr_callback, TD_NULL, TD_NULL, TD_NULL);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call osal_irq_request failure");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 jpeg_wait_all_finish(td_void)
{
    unsigned long flag = 0;
    drv_jpeg_dfx_info *dfx_info = jpeg_dfx_get_info();
    const td_u32 wait_time = 200;
    td_u32 cnt = 0;
    drv_jpeg_context *jpeg_context = jpeg_context_get();

    while (cnt++ < wait_time) {
        osal_spin_lock_irqsave(&jpeg_context->lock, &flag);
        if (dfx_info->create_cnt == dfx_info->destroy_cnt) {
            osal_spin_unlock_irqrestore(&jpeg_context->lock, &flag);
            break;
        }
        osal_spin_unlock_irqrestore(&jpeg_context->lock, &flag);
        osal_udelay(100); // 100us
    }
    if (cnt >= wait_time) {
        jpeg_err("jpeg_wait_all_finish timeout");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}
