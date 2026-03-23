/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: source file for aw88262 driver
 */

#include <stdio.h>
#include "audio_debug.h"
#if (SAP_OS_TYPE != os_type_xtensa)
#include "gpio.h"
#include "hal_gpio.h"
#include "i2c.h"
#include "pinctrl.h"
#else
#include "i2c_test.h"
#endif
#include "vendor_common.h"
#include "aw88262.h"

#define I2C_WRITE_READ_CNT 5
static td_bool g_i2c_inited = TD_FALSE;

typedef struct {
    td_u8 offset;
    td_u16 value;
} reg_default;

static reg_default g_aw88262_reg_defaults[] = {
    {AW88262_SYSCTRL_REG, 0x4441},
    {AW88262_SYSCTRL2_REG, 0x0020},
    {AW88262_I2SCTRL_REG, 0x18e8},
    {AW88262_I2SCFG1_REG, 0x0372},
    {AW88262_HAGCCFG4_REG, 0x0864},
    {AW88262_HAGCCFG1_REG, 0x3A41},
    {AW88262_I2SCFG2_REG, 0x210C},
    {AW88262_HAGCCFG5_REG, 0xa0db},
    {AW88262_HAGCCFG6_REG, 0x2716},
    {AW88262_HAGCCFG7_REG, 0x5F00},
    {AW88262_DBGCTRL_REG, 0x0001},
    {AW88262_CRCIN_REG, 0x0060},
    {AW88262_VSNCTRL1_REG, 0x0282},
    {AW88262_ISNCTRL1_REG, 0x20c6},
    {AW88262_ISNCTRL2_REG, 0xa040},
    {AW88262_VTMCTRL1_REG, 0xff19},
    {AW88262_VTMCTRL2_REG, 0x00d8},
    {AW88262_VTMCTRL3_REG, 0x212A},
    {AW88262_PWMCTRL_REG, 0x000c},
    {AW88262_PWMCTRL2_REG, 0x2100},
    {AW88262_BSTCTRL1_REG, 0x0c0b},
    {AW88262_BSTCTRL2_REG, 0x6367},
    {AW88262_BSTCTRL3_REG, 0x04F1},
    {AW88262_BSTDBG1_REG, 0x260F},
    {AW88262_BSTDBG2_REG, 0x15C3},
    {AW88262_BSTDBG3_REG, 0x3d82},
    {AW88262_PLLCTRL1_REG, 0x7f07},
    {AW88262_PLLCTRL2_REG, 0x0002},
    {AW88262_PLLCTRL3_REG, 0x3002},
    {AW88262_CDACTRL1_REG, 0x04a0},
    {AW88262_CDACTRL2_REG, 0xa078},
    {AW88262_SADCCTRL_REG, 0x8884},
    {AW88262_TESTCTRL1_REG, 0xFF00},
    {AW88262_TESTCTRL2_REG, 0x00c0},
    {AW88262_I2SCFG1_REG, 0x0361},
    {AW88262_SYSCTRL_REG, 0x4440},
};

static td_s32 aw88262_write_reg(td_u8 reg, td_u16 val)
{
    td_s32 ret;
    td_u8 send_data[] = {reg, (val >> I2C_SEND_DATA_SHIFT_COUNT),  val & 0xff}; // size 3
    td_u8 send_len = (td_u8)(sizeof(send_data) / sizeof(send_data[0]));

#if (SAP_OS_TYPE != os_type_xtensa)
    i2c_data_t data;
    td_u8 tmp;

    data.send_buf = send_data;
    data.send_len = send_len;
    data.receive_buf = NULL;
    data.receive_len = 0;
    for (tmp = 0; tmp < I2C_WRITE_READ_CNT; tmp++) {
        ret = (td_s32)uapi_i2c_master_write(SAP_AW88262_I2C_BUS_ID, SAP_AW88262_I2C_ADDR, &data);
        if (ret != ERRCODE_SUCC) {
            vendor_printf("uapi_i2c_master_write failed");
            vendor_err_log_h32(ret);
        } else {
            break;
        }
    }
#else
    ret = i2c_write(SAP_AW88262_I2C_ADDR, send_data, (td_u32)send_len);
    if (ret != send_len) {
        vendor_log_fun_err(i2c_write, ret);
        return AUDIO_FAILURE;
    }
#endif

    return ret;
}

static td_s32 aw88262_read_reg(td_u8 reg, td_u8 *val)
{
    td_s32 ret;

#if (SAP_OS_TYPE != os_type_xtensa)
    i2c_data_t data;
    td_u8 tmp;

    data.send_buf = &reg;
    data.send_len = 1; /* 1：reg data len */
    data.receive_buf = val;
    data.receive_len = REC_I2C_DATA_LEN;
    for (tmp = 0; tmp < I2C_WRITE_READ_CNT; tmp++) {
        ret = (td_s32)uapi_i2c_master_writeread(SAP_AW88262_I2C_BUS_ID, SAP_AW88262_I2C_ADDR, &data);
        if (ret != ERRCODE_SUCC) {
            vendor_printf("uapi_i2c_master_writeread failed");
            vendor_err_log_h32(ret);
        } else {
            break;
        }
    }
#else
    ret = i2c_read(SAP_AW88262_I2C_ADDR, reg, val, REC_I2C_DATA_LEN);
    if (ret != REC_I2C_DATA_LEN) {
        vendor_log_fun_err(i2c_read, ret);
        return AUDIO_FAILURE;
    }
#endif

    return ret;
}

static td_s32 aw88262_set_gain(td_void)
{
    td_s32 ret;

    /*
     * The high eight bits of this register control the volume
     * ranging from 0dB to -96dB
     * bit[15:12]: in unit of -6dB
     * bit[11:8]: in unit of -0.5dB
     */
    ret = aw88262_write_reg(AW88262_HAGCCFG4_REG, 0x1664);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_write_reg, ret);
    }

    return ret;
}

static td_s32 aw88262_verify_i2c(td_void)
{
    td_s32 ret;
    td_u8 val_tab[READ_DATA_SIZE];
    td_u8 *val = val_tab;
    td_u32 chip_id;
    /* verify chip id */
    ret = aw88262_read_reg(AW88262_ID_REG, val);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_read_reg, ret);
        return ret;
    }

    chip_id = ((td_u32)val_tab[0] << REG_UNITE_SHIFT_COUNT) | val_tab[1];
    if (chip_id != AW88262_CHIP_ID) {
        vendor_printf("aw88262 verify failed\n");
        vendor_err_log_u32(chip_id);
        return AUDIO_FAILURE;
    }
    return AUDIO_SUCCESS;
}

#if (SAP_OS_TYPE != os_type_xtensa)

static td_void aw88262_pwr_on(td_void)
{
    pin_t pwr_on_pin;

	/* 1p8 delay 10ms */
    vendor_msleep(10);

    if (SAP_AW88262_I2C_BUS_ID == 0) { /* 0:I2C0; Init 88262 Using I2C0 on E0B2 */
        pwr_on_pin = S_MGPIO4;
    } else if (SAP_AW88262_I2C_BUS_ID == 3) { /* 3:I2C3; Init 88262 Using I2C3 on E0B4 */
        pwr_on_pin = S_AGPIO_L25;
    }

    uapi_gpio_set_dir(pwr_on_pin, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(pwr_on_pin, GPIO_LEVEL_LOW);
    uapi_gpio_set_val(pwr_on_pin, GPIO_LEVEL_HIGH);
    /* reset high delay 10ms */
    vendor_msleep(10);
    return;
}
#endif

static td_s32 aw88262_i2c_init(td_void)
{
    td_s32 ret;
    vendor_func_enter();

#if (SAP_OS_TYPE != os_type_xtensa)
    /* init i2c */
    vendor_trace_log_u32(SAP_AW88262_I2C_BUS_ID);
    ret = (td_s32)uapi_i2c_master_init(SAP_AW88262_I2C_BUS_ID, I2C_BAUDRATE, I2C_HIGH_SPEED_CODE);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(uapi_i2c_master_init, ret);
        return ret;
    }
#endif

    ret = aw88262_verify_i2c();
    if (ret != AUDIO_SUCCESS) {
        (td_void)uapi_i2c_deinit(SAP_AW88262_I2C_BUS_ID);
        vendor_log_fun_err(aw88262_verify_i2c, ret);
        return ret;
    }

    vendor_printf("aw88262 i2c init success\n");
    vendor_func_exit();
    return AUDIO_SUCCESS;
}

static td_s32 aw88262_i2c_deinit(td_void)
{
    td_s32 ret;
    vendor_func_enter();

#if (SAP_OS_TYPE != os_type_xtensa)
    /* deinit i2c */
    vendor_trace_log_u32(SAP_AW88262_I2C_BUS_ID);
    ret = (td_s32)uapi_i2c_deinit(SAP_AW88262_I2C_BUS_ID);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(uapi_i2c_deinit, ret);
        return ret;
    }
#endif

    vendor_printf("aw88262 i2c deinit success\n");
    vendor_func_exit();
    return AUDIO_SUCCESS;
}

static td_s32 aw88262_set_sample_rate(td_u32 sample_rate)
{
    td_s32 ret;
    td_u8 reg_val_tab[READ_DATA_SIZE];
    td_u8 *reg_val = reg_val_tab;

    ret = aw88262_read_reg(AW88262_I2SCTRL_REG, reg_val);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_read_reg, ret);
    }
    reg_val_tab[1] &= ~(0X0F << 0);

    if (sample_rate == UAPI_AUDIO_SAMPLE_RATE_8K) {
        reg_val_tab[1] |= 0X00;
    } else if (sample_rate == UAPI_AUDIO_SAMPLE_RATE_16K) {
        reg_val_tab[1] |= 0X03;
    } else if (sample_rate == UAPI_AUDIO_SAMPLE_RATE_32K) {
        reg_val_tab[1] |= 0X06;
    } else if (sample_rate == UAPI_AUDIO_SAMPLE_RATE_48K) {
        reg_val_tab[1] |= 0X08;
    } else if (sample_rate == UAPI_AUDIO_SAMPLE_RATE_96K) {
        reg_val_tab[1] |= 0X09;
    } else if (sample_rate == UAPI_AUDIO_SAMPLE_RATE_192K) {
        reg_val_tab[1] |= 0X0A;
    } else {
        ret = AUDIO_FAILURE;
        return ret;
    }
    ret = aw88262_write_reg(AW88262_I2SCTRL_REG, ((td_u16)reg_val_tab[0] << REG_UNITE_SHIFT_COUNT) | reg_val_tab[1]);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_write_reg, ret);
    }

    return ret;
}

static td_s32 aw88262_set_bit_depth(td_u32 bit_depth)
{
    td_s32 ret;
    td_u8 reg_val_tab[READ_DATA_SIZE];
    td_u8 *reg_val = reg_val_tab;

    ret = aw88262_read_reg(AW88262_I2SCTRL_REG, reg_val);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_read_reg, ret);
    }

    reg_val_tab[1] &= ~(0x0F << REG_BIT_SHIFT_COUNT);
    if (bit_depth == UAPI_AUDIO_BIT_DEPTH_16) {
        reg_val_tab[1] |= (0x00 << REG_BIT_SHIFT_COUNT);
    } else if (bit_depth == UAPI_AUDIO_BIT_DEPTH_24) {
        reg_val_tab[1] |= (0x09 << REG_BIT_SHIFT_COUNT);
    } else if (bit_depth == UAPI_AUDIO_BIT_DEPTH_32) {
        reg_val_tab[1] |= (0x0E << REG_BIT_SHIFT_COUNT);
    }
    ret = aw88262_write_reg(AW88262_I2SCTRL_REG, ((td_u16)reg_val_tab[0] << REG_UNITE_SHIFT_COUNT) | reg_val_tab[1]);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_write_reg, ret);
    }

    return ret;
}

static td_s32 aw88262_sys_init(td_void)
{
    td_s32 ret;
    td_u32 i;

    ret = aw88262_write_reg(AW88262_ID_REG, 0);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_write_reg, ret);
    }

    vendor_msleep(DELAY_MTIME);
    for (i = 0; i < (sizeof(g_aw88262_reg_defaults) / sizeof(g_aw88262_reg_defaults[0])); i++) {
        ret = aw88262_write_reg(g_aw88262_reg_defaults[i].offset, g_aw88262_reg_defaults[i].value);
        if (ret != AUDIO_SUCCESS) {
            audio_log_err("aw88262_write_reg 0x%x", g_aw88262_reg_defaults[i].offset);
            return ret;
        }
    }

    return ret;
}

static td_s32 aw88262_get_icalk(td_s16 *icalk)
{
    td_s32 ret;
    td_u16 reg_icalk;
    td_u8 reg_val_tab[READ_DATA_SIZE];
    td_u8 *reg_val = reg_val_tab;

    ret = aw88262_read_reg(AW88262_EFRM1_REG, reg_val);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_read_reg, ret);
    }

    reg_icalk = (((td_u16)reg_val_tab[0] << REG_UNITE_SHIFT_COUNT) | reg_val_tab[1]) & 0x03ff;
    if ((reg_icalk & ICALK_SIGN_MASK) != TD_FALSE) {
        reg_icalk = reg_icalk | ICALK_NEG_MASK;
    }

    *icalk = (td_s16)reg_icalk;

    return ret;
}

static td_s32 aw88262_get_vcalk(td_s16 *vcalk)
{
    td_s32 ret;
    td_u16 reg_vcalk;
    td_u8 reg_val_tab[READ_DATA_SIZE];
    td_u8 *reg_val = reg_val_tab;

    ret = aw88262_read_reg(AW88262_EFRH_REG, reg_val);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_read_reg, ret);
    }

    reg_vcalk = (((td_u16)reg_val_tab[0] << REG_UNITE_SHIFT_COUNT) | reg_val_tab[1]) & 0x03ff;
    if ((reg_vcalk & VCALK_SIGN_MASK) != TD_FALSE) {
        reg_vcalk = reg_vcalk | VCALK_NEG_MASK;
    }
    *vcalk = (td_s16)reg_vcalk;

    return ret;
}

static td_s32 aw88262_set_vcalb(td_void)
{
    td_s32 ret;
    td_u16 reg_val;
    td_s32 vcalb;
    td_s32 icalk;
    td_s32 vcalk;
    td_s16 icalk_val = 0;
    td_s16 vcalk_val = 0;

    ret = aw88262_get_icalk(&icalk_val);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_get_icalk, ret);
        return ret;
    }

    ret = aw88262_get_vcalk(&vcalk_val);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_get_vcalk, ret);
        return ret;
    }

    icalk = CABL_BASE_VALUE + icalk_val;
    vcalk = CABL_BASE_VALUE + vcalk_val;
    if (vcalk == 0) {
        vendor_err_log_h32(ret);
        return AUDIO_FAILURE;
    }

    vcalb = (1 << VCALB_OFFSET) * icalk / vcalk;
    reg_val = (td_u16)vcalb;
    ret = aw88262_write_reg(AW88262_VTMCTRL3_REG, reg_val);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_write_reg, ret);
    }

    return ret;
}

static td_s32 aw88262_mute(td_u8 mute_state)
{
    td_s32 ret;
    td_u16 reg_value;
    vendor_func_enter();

    if (mute_state != TD_FALSE) {
        reg_value = 0x0035; /* mute DAC state machine  DSM */
    } else {
        reg_value = 0x0025; /* unmute DAC state machine  DSM */
    }
    ret = aw88262_write_reg(AW88262_SYSCTRL2_REG, reg_value);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_write_reg, ret);
    }

    vendor_func_exit();
    return ret;
}

static td_s32 audio_aw88262_configuration(const uapi_vendor_codec_attr *attr)
{
    td_s32 ret;

    ret = aw88262_mute(AW88262_MUTE);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_mute, ret);
    }

    ret = aw88262_sys_init();
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_sys_init, ret);
    }

    ret = aw88262_set_vcalb();
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_set_vcalb, ret);
    }

    ret = aw88262_set_sample_rate(attr->sample_rate);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_set_sample_rate, ret);
    }

    ret = aw88262_set_bit_depth(attr->bit_depth);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_set_bit_depth, ret);
    }

    ret = aw88262_set_gain();
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_set_gain, ret);
    }

    ret = aw88262_mute(AW88262_UNMUTE);
    if (ret != AUDIO_SUCCESS) {
        vendor_log_fun_err(aw88262_mute, ret);
    }

    return ret;
}

td_s32 audio_aw88262_init(const uapi_vendor_codec_attr *attr)
{
    td_s32 ret;

    vendor_func_enter();

#if (SAP_OS_TYPE != os_type_xtensa)
    aw88262_pwr_on();
#endif

#ifndef CONFIG_AUDIO_NOT_CTRL_I2C_INIT
    if (g_i2c_inited != TD_TRUE) {
        ret = aw88262_i2c_init();
        if (ret != AUDIO_SUCCESS) {
            vendor_log_fun_err(aw88262_i2c_init, ret);
            return ret;
        }
        g_i2c_inited = TD_TRUE;
    } else {
        ret = 0;
    }
#endif

    ret = audio_aw88262_configuration(attr);
    if (ret != AUDIO_SUCCESS) {
        (td_void)audio_aw88262_deinit();
        vendor_log_fun_err(audio_aw88262_configuration, ret);
    }

    return ret;
}

td_s32 audio_aw88262_deinit(td_void)
{
    td_s32 ret = 0;
    vendor_func_enter();

#ifndef CONFIG_AUDIO_NOT_CTRL_I2C_INIT
    if (g_i2c_inited != TD_FALSE) {
        ret = aw88262_i2c_deinit();
        if (ret != AUDIO_SUCCESS) {
            vendor_log_fun_err(aw88262_i2c_deinit, ret);
            return ret;
        }
        g_i2c_inited = TD_FALSE;
    } else {
        ret = 0;
    }
#endif

    vendor_func_exit();
    return ret;
}

