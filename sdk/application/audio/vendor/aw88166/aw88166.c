/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: source file for aw88166 driver
 */

#include <stdio.h>
#include "soc_osal.h"
#include "audio_debug.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "i2c.h"
#include "pinctrl.h"
#include "vendor_common.h"
#include "soc_uapi_vendor.h"

#include "aw883xx_init.h"
#include "aw883xx.h"
#include "aw_params.h"

static td_bool g_aw88166_i2c_inited = TD_FALSE;

#define AW88166_PWR_ON_PIN   S_AGPIO_R3
#define AW88166_1_8V_ON_PIN     S_AGPIO_L12
#define AW88166_I2C_BUS_ID   I2C_BUS_1
#define AW88166_I2C_DEV_ADDR 0x34

static td_s32 aw_dev0_i2c_write_func(td_u16 dev_addr, td_u8 reg_addr, td_u8 *pdata, td_u16 len)
{
    td_s32 ret;
    i2c_data_t data;
    const td_u8 reg_addr_len = sizeof(reg_addr); /* data len of reg_addr */
    const td_u8 send_len = len + reg_addr_len; /* pdata bytes && reg_addr len */
    td_u8 *send_data = osal_kmalloc_align(send_len, 0, 8); /* 8 : Align up to 8 bytes */

    if (send_data == TD_NULL) {
        ret = -1; /* -1:FAILED ERR CODE */
        vendor_log_fun_err(malloc, ret);
        return ret;
    }

    /* pack reg addr & reg data */
    *send_data = reg_addr; /* reg_addr */
    ret = memcpy_s(send_data + reg_addr_len, (send_len - reg_addr_len), pdata, len); /* pdata */
    if (ret != EOK) {
        vendor_log_fun_err(memcpy_s, ret);
        goto out;
    }

    data.send_buf = send_data;
    data.send_len = send_len;
    data.receive_buf = NULL;
    data.receive_len = 0;
    ret = (td_s32)uapi_i2c_master_write(AW88166_I2C_BUS_ID, dev_addr, &data);
    if (ret != ERRCODE_SUCC) {
        vendor_log_fun_err(uapi_i2c_master_write, ret);
        goto out;
    }

out:
    if (send_data != TD_NULL) {
        osal_kfree(send_data);
    }
    return ret;
}

static td_s32 aw_dev0_i2c_read_func(td_u16 dev_addr, td_u8 reg_addr, td_u8 *pdata, td_u16 len)
{
    td_s32 ret;

    i2c_data_t data;

    data.send_buf = &reg_addr;
    data.send_len = sizeof(reg_addr);
    data.receive_buf = pdata;
    data.receive_len = len;
    ret = (td_s32)uapi_i2c_master_writeread(AW88166_I2C_BUS_ID, dev_addr, &data);
    if (ret != ERRCODE_SUCC) {
        vendor_log_fun_err(uapi_i2c_master_writeread, ret);
        return ret;
    }

    return AUDIO_SUCCESS;
}

static td_void aw_dev0_reset_gpio_ctl(bool state)
{
    uapi_gpio_set_val(AW88166_PWR_ON_PIN, state);
}
static td_void aw_1_8V_ctl(td_bool state)
{
    uapi_gpio_set_val(AW88166_1_8V_ON_PIN, state);
}
static td_void aw88166_pwr_on_pin_init(td_void)
{
    uapi_gpio_set_dir(AW88166_PWR_ON_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_dir(AW88166_1_8V_ON_PIN, GPIO_DIRECTION_OUTPUT);
}

static td_s32 aw88166_i2c_init(td_void)
{
    td_s32 ret;
    audio_func_enter();

    /* init i2c */
    vendor_trace_log_u32(AW88166_I2C_BUS_ID);
    ret = (td_s32)uapi_i2c_master_init(AW88166_I2C_BUS_ID, I2C_BAUDRATE, I2C_HIGH_SPEED_CODE);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(uapi_i2c_master_init, ret);
        // return ret;
    }

    vendor_printf("aw88166 i2c init success\n");
    audio_func_exit();
    return AUDIO_SUCCESS;
}

static td_s32 aw88166_i2c_deinit(td_void)
{
    td_s32 ret;
    audio_func_enter();

    /* deinit i2c */
    vendor_trace_log_u32(AW88166_I2C_BUS_ID);
    ret = (td_s32)uapi_i2c_deinit(AW88166_I2C_BUS_ID);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(uapi_i2c_deinit, ret);
        // return ret;
    }

    vendor_printf("aw88166 i2c deinit success\n");
    audio_func_exit();
    return AUDIO_SUCCESS;
}

td_s32 aw883xx_pid_2066_dev_init(td_void *aw883xx);

struct aw_init_info init_info[] = {
    {
        .dev = AW_DEV_0,
        .i2c_addr = AW88166_I2C_DEV_ADDR,
        .re_min = 2000, /* awinic:If the re_min(ohms) and re_max(ohms) are not set, the default value is used */
        .re_max = 39000,
        .mix_chip_count = AW_DEV0_MIX_CHIP_NUM,
        .prof_info = g_dev0_prof_info,
        .i2c_read_func = aw_dev0_i2c_read_func,
        .i2c_write_func = aw_dev0_i2c_write_func,
        .reset_gpio_ctl = aw_dev0_reset_gpio_ctl,
        .dev_init_ops = {aw883xx_pid_2066_dev_init},
    }
};

static td_s32 audio_aw88166_configuration(const uapi_vendor_codec_attr *attr)
{
    td_s32 ret;
    vendor_printf("audio_aw88166_configuration\n");
    /* awinic:Control interface of dev0 */
    /* awinic:initialization */
    ret = aw883xx_smartpa_init((td_void *)&init_info[AW_DEV_0]);
    if (ret != AUDIO_SUCCESS) {
        vendor_printf("aw883xx_smartpa_init already init\n");
    }
    /* awinic:set mode "Music", "aw_params.h" contains id information */
    ret = aw883xx_set_profile_byname(AW_DEV_0, "Music");
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw883xx_set_profile_byname, ret);
    }

    vendor_unused(attr);
    return ret;
}

static td_s32 audio_aw88166_deconfiguration(td_void)
{
    /* awinic:Free the requested memory when PA is not used */
    aw883xx_smartpa_deinit(AW_DEV_0);

    return AUDIO_SUCCESS;
}

td_s32 audio_aw88166_init(const uapi_vendor_codec_attr *attr)
{
    td_s32 ret;

    audio_func_enter();

    aw88166_pwr_on_pin_init();
    aw_1_8V_ctl(1);
    AW_MS_DELAY(10);
    aw_dev0_reset_gpio_ctl(0);
    AW_MS_DELAY(2);
    aw_dev0_reset_gpio_ctl(1);
    AW_MS_DELAY(2);
#ifndef CONFIG_AUDIO_NOT_CTRL_I2C_INIT
    if (g_aw88166_i2c_inited != TD_TRUE) {
        ret = aw88166_i2c_init();
        if (ret != AUDIO_SUCCESS) {
            vendor_log_fun_err(aw88166_i2c_init, ret);
            return ret;
        }
        g_aw88166_i2c_inited = TD_TRUE;
    } else {
        ret = 0;
    }
#endif

    ret = audio_aw88166_configuration(attr);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(audio_aw88166_configuration, ret);
    }

    return ret;
}

td_s32 audio_aw88166_deinit(td_void)
{
    td_s32 ret;
    audio_func_enter();

    ret = audio_aw88166_deconfiguration();
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(audio_aw88166_deconfiguration, ret);
    }

#ifndef CONFIG_AUDIO_NOT_CTRL_I2C_INIT
    if (g_aw88166_i2c_inited != TD_FALSE) {
        ret = aw88166_i2c_deinit();
        if (ret != AUDIO_SUCCESS) {
            vendor_log_fun_err(aw88166_i2c_deinit, ret);
            return ret;
        }
        g_aw88166_i2c_inited = TD_FALSE;
    } else {
        ret = AUDIO_SUCCESS;
    }
#endif

    audio_func_exit();
    return ret;
}

td_s32 audio_aw88166_start(const uapi_vendor_codec_attr *attr)
{
    td_s32 ret;

    audio_func_enter();

    /* awinic:The platform needs to output I2s first, and then ctl start */
    ret = aw883xx_ctrl_state(AW_DEV_0, AW_START);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw883xx_ctrl_state, ret);
    }

    vendor_unused(attr);
    return ret;
}

td_s32 audio_aw88166_stop(td_void)
{
    td_s32 ret;
    audio_func_enter();

    /* awinic:The platform needs to control the stop first and then stop I2S */
    ret = aw883xx_ctrl_state(AW_DEV_0, AW_STOP);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw883xx_ctrl_state, ret);
    }
    audio_func_exit();
    return ret;
}

td_s32 audio_aw88166_set_aef_profile(uapi_snd_aef_profile aef_profile)
{
    td_s32 ret;

    audio_func_enter();

    /* awinic:set mode "Receiver"/"Music", "aw_params.h" contains id information */
    if (aef_profile == UAPI_SND_AEF_PROFILE_VOIP) {
        /* awinic:set mode "Receiver" */
        ret = aw883xx_set_profile_byname(AW_DEV_0, "Voice");
    } else {
        /* awinic:set mode "Music" */
        ret = aw883xx_set_profile_byname(AW_DEV_0, "Music");
    }

    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw883xx_set_profile_byname, ret);
    }

    audio_func_exit();
    return ret;
}
