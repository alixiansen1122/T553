/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg ioctl
 */

#include "jpeg_ioctl.h"
#include "drv_gfx_port.h"
#include "soc_osal.h"
#include "drv_jpeg_struct.h"
#include "jpeg_isr.h"
#include "jpeg_hal.h"
#include "jpeg_clock.h"
#include "jpeg_context.h"
#include "jpeg_dfx.h"
#include "pm.h"

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

static td_void jpeg_ioctl_decompress_init(const drv_jpeg_mgr *reg_mgr);
static td_void jpeg_ioctl_decompress_deinit(jpeg_int_type int_type);
static td_void jpeg_ioctl_wait_soft_timeout(jpeg_int_type *int_type);

/*-------------------------------- par define ---------------------------------------------*/

/*-------------------------------- func release -------------------------------------------*/
static td_void jpeg_drv_mgr_dfx(drv_jpeg_mgr *reg_mgr, drv_jpeg_dfx_info *jpeg_dfx_info)
{
    jpeg_dfx_info->data_buf_size      = reg_mgr->data_buf_size;
    jpeg_dfx_info->data_phy_buf       = reg_mgr->data_phy_buf;
    jpeg_dfx_info->jpeg_color_space   = reg_mgr->jpeg_color_space;
    jpeg_dfx_info->output_color_space = reg_mgr->output_color_space;
    jpeg_dfx_info->scale              = reg_mgr->scale;
    jpeg_dfx_info->stream_buf_size    = reg_mgr->stream_buf_size;
    jpeg_dfx_info->stream_phy_buf     = reg_mgr->stream_phy_buf;
    jpeg_dfx_info->uv_phy_buf         = reg_mgr->uv_phy_buf;
    jpeg_dfx_info->uv_buf_stride      = reg_mgr->uv_buf_stride;
    jpeg_dfx_info->uv_mcu_height      = reg_mgr->uv_mcu_height;
    jpeg_dfx_info->y_buf_stride       = reg_mgr->y_buf_stride;
    jpeg_dfx_info->y_mcu_height       = reg_mgr->y_mcu_height;
    jpeg_dfx_info->y_phy_buf          = reg_mgr->y_phy_buf;
}

td_s32 jpeg_ioctl_start_decompress(td_u32 cmd, td_void *arg, td_void *file)
{
    td_u32 time_start = 0;
    td_u32 time_end = 0;
    drv_jpeg_mgr *reg_mgr = (drv_jpeg_mgr *)arg;

    uapi_unused(cmd);
    uapi_unused(file);

    drv_jpeg_dfx_info *jpeg_dfx_info = jpeg_dfx_get_info();
    jpeg_dfx_get_time_us(&time_start);
    if (reg_mgr == TD_NULL) {
        jpeg_err("input reg_mgr is null");
        return EXT_FAILURE;
    }

    if (reg_mgr->mask != DRV_JPEG_MASK) {
        return EXT_FAILURE;
    }

    if ((reg_mgr->stream_phy_buf == 0) || (reg_mgr->stream_buf_size == 0)) {
        jpeg_err("stream buf or stream size is not support");
        return EXT_FAILURE;
    }

    if ((reg_mgr->data_phy_buf == 0) || (reg_mgr->data_buf_size == 0)) {
        jpeg_err("data buf or data size is not support");
        return EXT_FAILURE;
    }

    if ((reg_mgr->y_phy_buf == 0) || (reg_mgr->uv_phy_buf == 0)) {
        jpeg_err("y buf or uv buf is not support");
        return EXT_FAILURE;
    }
    if (reg_mgr->is_start != TD_TRUE) {
        jpeg_ioctl_decompress_init(reg_mgr);
    }

    jpeg_hal_set_data_start_addr((td_u32)reg_mgr->data_phy_buf);
    jpeg_hal_set_data_end_addr((td_u32)reg_mgr->data_phy_buf + reg_mgr->data_buf_size);

    jpeg_hal_set_start(reg_mgr->start_value);
    jpeg_hal_set_resume(reg_mgr->resume_value);
    jpeg_hal_set_eoi(reg_mgr->is_eoi);
    jpeg_drv_mgr_dfx(reg_mgr, jpeg_dfx_info);
    jpeg_dfx_get_time_us(&time_end);

    jpeg_dfx_info->start_cost = time_end - time_start;
    return EXT_SUCCESS;
}

static td_s32 wait_intstatus_condition(const td_void *data)
{
    if ((data != TD_NULL) && (*(jpeg_int_type *)data != DRV_JPEG_INT_TYPE_NONE)) {
        return 1;
    }

    return 0;
}

td_s32 jpeg_ioctl_get_decompress_status(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_u32 time_start = 0;
    td_u32 time_end = 0;

    uapi_unused(cmd);
    uapi_unused(file);

    drv_jpeg_dfx_info *jpeg_dfx_info = jpeg_dfx_get_info();
    drv_jpeg_intstatus *int_status = (drv_jpeg_intstatus *)arg;
    drv_jpeg_context *jpeg_context = jpeg_context_get();

    jpeg_dfx_get_time_us(&time_start);
    if (int_status == TD_NULL) {
        jpeg_err("input int_status is null");
        return EXT_FAILURE;
    }

    if (int_status->mask != DRV_JPEG_MASK) {
        return EXT_FAILURE;
    }

    int_status->int_type = DRV_JPEG_INT_TYPE_NONE;

    osal_irq_disable(JPEG_IRQ_NUM);
    if (jpeg_context->int_type != DRV_JPEG_INT_TYPE_NONE) {
        int_status->int_type = jpeg_context->int_type;
        jpeg_context->int_type = DRV_JPEG_INT_TYPE_NONE;
        osal_irq_enable(JPEG_IRQ_NUM);
        jpeg_ioctl_decompress_deinit(int_status->int_type);
        return EXT_SUCCESS;
    }
    osal_irq_enable(JPEG_IRQ_NUM);

    ret = osal_wait_timeout_interruptible(&jpeg_context->wait_int, wait_intstatus_condition,
                                          &jpeg_context->int_type, 100); /* 100 ms */
    if (ret > 0 || (jpeg_context->int_type != DRV_JPEG_INT_TYPE_NONE)) {
        osal_irq_disable(JPEG_IRQ_NUM);
        int_status->int_type = jpeg_context->int_type;
        jpeg_context->int_type = DRV_JPEG_INT_TYPE_NONE;
        osal_irq_enable(JPEG_IRQ_NUM);
    } else if (ret == -OSAL_ERESTARTSYS) {
        jpeg_ioctl_wait_soft_timeout(&(int_status->int_type));
    } else {
        jpeg_ioctl_decompress_deinit(int_status->int_type);
        jpeg_err("wait interrupt timeout");
        return EXT_FAILURE;
    }

    jpeg_ioctl_decompress_deinit(int_status->int_type);
    jpeg_dfx_get_time_us(&time_end);
    jpeg_dfx_info->get_status_cost = time_end - time_start;
    return EXT_SUCCESS;
}

static td_void jpeg_ioctl_decompress_init(const drv_jpeg_mgr *reg_mgr)
{
    td_u32 stream_start;
    td_u32 stream_end;
    td_u32 time_start = 0;
    td_u32 time_end = 0;
    drv_jpeg_dfx_info *jpeg_dfx_info = jpeg_dfx_get_info();
    jpeg_dfx_get_time_us(&time_start);

    jpeg_context_lock();

    jpeg_clock_open();
    jpeg_clock_reset();
    jpeg_clock_dereset();

    jpeg_hal_set_ck_enable();
    jpeg_hal_set_outstanding();
    jpeg_hal_set_intmask();

    jpeg_hal_set_picture_size(reg_mgr->image_mcu_width, reg_mgr->image_mcu_height);
    jpeg_hal_set_picture_fmt(reg_mgr->jpeg_color_space);

    jpeg_hal_set_stride(reg_mgr->y_buf_stride, reg_mgr->uv_buf_stride);
    jpeg_hal_set_scale(reg_mgr->scale);
    jpeg_hal_set_output_size(reg_mgr->y_mcu_height);
    jpeg_hal_set_output_fmt(reg_mgr->output_color_space);

    jpeg_hal_set_factor(reg_mgr->y_fac, reg_mgr->u_fac, reg_mgr->v_fac);
    jpeg_hal_set_dri(reg_mgr->restart_interval);

    jpeg_hal_set_dqt_y(reg_mgr->quant_ytable,   DRV_JPEG_MAX_QTB_SIZE);
    jpeg_hal_set_dqt_cb(reg_mgr->quant_cbtable, DRV_JPEG_MAX_QTB_SIZE);
    jpeg_hal_set_dqt_cr(reg_mgr->quant_crtable, DRV_JPEG_MAX_QTB_SIZE);

    jpeg_hal_set_dht_dc_lu_table(reg_mgr->dht_dc_lu_table, DRV_JPEG_MAX_HTB_DC_SIZE);
    jpeg_hal_set_dht_dc_ch_table(reg_mgr->dht_dc_ch_table, DRV_JPEG_MAX_HTB_DC_SIZE);
    jpeg_hal_set_dht_ac_lu_mincode_even(reg_mgr->dht_ac_lu_mincode_even, DRV_JPEG_MAX_HTB_AC_MIN_SIZE);
    jpeg_hal_set_dht_ac_lu_mincode_odd(reg_mgr->dht_ac_lu_mincode_odd,   DRV_JPEG_MAX_HTB_AC_MIN_SIZE);
    jpeg_hal_set_dht_ac_ch_mincode_even(reg_mgr->dht_ac_ch_mincode_even, DRV_JPEG_MAX_HTB_AC_MIN_SIZE);
    jpeg_hal_set_dht_ac_ch_mincode_odd(reg_mgr->dht_ac_ch_mincode_odd,   DRV_JPEG_MAX_HTB_AC_MIN_SIZE);
    jpeg_hal_set_dht_ac_lu_base_even(reg_mgr->dht_ac_lu_base_even, DRV_JPEG_MAX_HTB_AC_BASE_SIZE);
    jpeg_hal_set_dht_ac_lu_base_odd(reg_mgr->dht_ac_lu_base_odd,   DRV_JPEG_MAX_HTB_AC_BASE_SIZE);
    jpeg_hal_set_dht_ac_ch_base_even(reg_mgr->dht_ac_ch_base_even, DRV_JPEG_MAX_HTB_AC_BASE_SIZE);
    jpeg_hal_set_dht_ac_ch_base_odd(reg_mgr->dht_ac_ch_base_odd,   DRV_JPEG_MAX_HTB_AC_BASE_SIZE);
    jpeg_hal_set_dht_ac_lu_symblo(reg_mgr->dht_ac_lu_symblo,   DRV_JPEG_MAX_HTB_AC_SYMBOL_SIZE);
    jpeg_hal_set_dht_ac_ch_symblo(reg_mgr->dht_ac_ch_symblo,   DRV_JPEG_MAX_HTB_AC_SYMBOL_SIZE);

    stream_start = (td_u32)(((reg_mgr->stream_phy_buf + 64 - 1) & (~(64 - 1))) - 64);  /* 64:align val */
    /* 64:align val */
    stream_end = (td_u32)(((reg_mgr->stream_phy_buf + reg_mgr->stream_buf_size + 64 - 1) & (~(64 - 1))) + 64);
    jpeg_hal_set_stream_start_addr(stream_start);
    jpeg_hal_set_stream_end_addr(stream_end);

    jpeg_hal_set_y_addr((td_u32)(reg_mgr->y_phy_buf));
    jpeg_hal_set_uv_addr((td_u32)(reg_mgr->uv_phy_buf));

    jpeg_dfx_get_time_us(&time_end);
    jpeg_dfx_info->init_cost = time_end - time_start;
    return;
}

static td_void jpeg_ioctl_decompress_deinit(jpeg_int_type int_type)
{
    if (int_type != DRV_JPEG_INT_TYPE_CONTINUE) {
        jpeg_clock_reset();
        jpeg_hal_set_ck_disable();
        jpeg_clock_close();
        jpeg_context_unlock();
    }
    return;
}

static td_void jpeg_ioctl_wait_soft_timeout(jpeg_int_type *int_type)
{
    td_s32 loop;
    td_u32 first_count = 1;
    td_slong timeout = 900; /* 900 for calc ms time */
    drv_jpeg_context *jpeg_context = jpeg_context_get();

    osal_timeval time_start = {0};
    osal_timeval time_end = {0};

    do {
        if (first_count != 0) {
            osal_gettimeofday(&time_start);
            first_count = 0;
            loop = 1;
        } else {
            osal_gettimeofday(&time_end);
            loop = (((time_end.tv_usec - time_start.tv_usec) / 1000) < timeout) ? (1) : (0); /* 1000:us */
        }

        if (jpeg_context->int_type != DRV_JPEG_INT_TYPE_NONE) {
            break;
        }

        if (loop == 0) {
            break;
        }
    } while (loop != 0);

    osal_irq_disable(JPEG_IRQ_NUM);
    *int_type = jpeg_context->int_type;
    jpeg_context->int_type = DRV_JPEG_INT_TYPE_NONE;
    osal_irq_enable(JPEG_IRQ_NUM);

    return;
}

td_s32 jpeg_ioctl_decomress_create(td_u32 cmd, td_void *arg, td_void *file)
{
    uapi_unused(cmd);
    uapi_unused(file);
    uapi_unused(arg);

    drv_jpeg_dfx_info *jpeg_dfx_info = jpeg_dfx_get_info();
    jpeg_dfx_info->create_cnt++;
    uapi_pm_add_sleep_veto(PM_ID_JPEG);
    return EXT_SUCCESS;
}

td_s32 jpeg_ioctl_decompress_destroy(td_u32 cmd, td_void *arg, td_void *file)
{
    uapi_unused(cmd);
    uapi_unused(file);
    uapi_unused(arg);

    drv_jpeg_dfx_info *jpeg_dfx_info = jpeg_dfx_get_info();
    jpeg_dfx_info->destroy_cnt++;
    uapi_pm_remove_sleep_veto(PM_ID_JPEG);
    return EXT_SUCCESS;
}
