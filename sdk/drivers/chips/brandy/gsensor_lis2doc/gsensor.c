/**
 * Copyright (c) @CompanyNameMagicTag 2025-2025. All rights reserved. \n
 *
 * Description: Provides gsensor driver source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-05-20, Create file. \n
 */
#include "i2c.h"
#include "pinctrl.h"
#include "pinctrl_porting.h"
#include "osal_timer.h"
#include "soc_osal.h"
#include "pm_definition.h"
#include "gsensor.h"

#define GSENSOR_CHIPID_VAL       0x44
#define GSENSOR_TIMER_INTERVAL   400
#define GSENSOR_FIFO_DEPTH       32
#define GSENSOR_XTZ_REG_NUM      6
#define GSENSOR_DATA_RESOLUTION  13
#define GSENSOR_FULL_SCALE       4
#define GSENSOR_G                9.8
#define GSENSOR_ACC_REG_LEN      6
#define GSENSOR_XL_OFS           0
#define GSENSOR_XH_OFS           1
#define GSENSOR_YL_OFS           2
#define GSENSOR_YH_OFS           3
#define GSENSOR_ZL_OFS           4
#define GSENSOR_ZH_OFS           5
#define GSENSOR_X_ACC_IDX        0
#define GSENSOR_Y_ACC_IDX        1
#define GSENSOR_Z_ACC_IDX        2
#define GSENSOR_DATA_OFS         8

static osal_timer g_gsensor_timer;
static uint8_t g_gsensor_fifo[GSENSOR_FIFO_DEPTH * GSENSOR_XTZ_REG_NUM];
static uint8_t gsensor_open_flag = 0;

typedef union ctrl1_reg {
    uint8_t d8;
    struct {
        uint8_t reserved0                    :  1;
        uint8_t reserved1                    :  1;
        uint8_t mode                         :  2;
        uint8_t odr                          :  4;
    } b;
} ctrl1_reg_t;

typedef union ctrl2_reg {
    uint8_t d8;
    struct {
        uint8_t sim                          :  1;
        uint8_t i2c_disable                  :  1;
        uint8_t if_add_inc                   :  1;
        uint8_t bdu                          :  1;
        uint8_t cs_pu_disc                   :  1;
        uint8_t reserved                     :  1;
        uint8_t soft_reset                   :  1;
        uint8_t boot                         :  1;
    } b;
} ctrl2_reg_t;

typedef union ctrl3_reg {
    uint8_t d8;
    struct {
        uint8_t slp_mode_1                   :  1;
        uint8_t slp_mode_sel                 :  1;
        uint8_t reserved                     :  1;
        uint8_t h_lactive                    :  1;
        uint8_t lir                          :  1;
        uint8_t pp_od                        :  1;
        uint8_t st                           :  2;
    } b;
} ctrl3_reg_t;

typedef union ctrl6_reg {
    uint8_t d8;
    struct {
        uint8_t reserved                     :  2;
        uint8_t low_noise                    :  1;
        uint8_t fds                          :  1;
        uint8_t fs                           :  2;
        uint8_t bw_filt                      :  2;
    } b;
} ctrl6_reg_t;

typedef union fifo_ctrl_reg {
    uint8_t d8;
    struct {
        uint8_t fth                          :  5;
        uint8_t fmode                        :  3;
    } b;
} fifo_ctrl_reg_t;

typedef union fifo_samples_reg {
    uint8_t d8;
    struct {
        uint8_t diff                         :  6;
        uint8_t fifo_ovr                     :  1;
        uint8_t fifo_fth                     :  1;
    } b;
} fifo_samples_reg_t;

typedef enum {
    GSENSOR_REG_OUT_T_L = 0xd,
    GSENSOR_REG_OUT_T_H,
    GSENSOR_REG_WHO_AM_I,
    GSENSOR_REG_CTRL1 = 0x20,
    GSENSOR_REG_CTRL2,
    GSENSOR_REG_CTRL3,
    GSENSOR_REG_CTRL4_INT1_PAD_CTRL,
    GSENSOR_REG_CTRL5_INT1_PAD_CTRL,
    GSENSOR_REG_CTRL6,
    GSENSOR_REG_OUT_T,
    GSENSOR_REG_STATUS,
    GSENSOR_REG_OUT_X_L,
    GSENSOR_REG_OUT_X_H,
    GSENSOR_REG_OUT_Y_L,
    GSENSOR_REG_OUT_Y_H,
    GSENSOR_REG_OUT_Z_L,
    GSENSOR_REG_OUT_Z_H,
    GSENSOR_REG_FIFO_CTRL,
    GSENSOR_REG_FIFO_SAMPLES,
    GSENSOR_REG_TAP_THS_X,
    GSENSOR_REG_TAP_THS_Y,
    GSENSOR_REG_TAP_THS_Z,
    GSENSOR_REG_INT_DUR,
    GSENSOR_REG_WAKE_UP_THS,
    GSENSOR_REG_WAKE_UP_DUR,
    GSENSOR_REG_FREE_FALL,
    GSENSOR_REG_STATUS_DUP,
    GSENSOR_REG_WAKE_UP_SRC,
    GSENSOR_REG_TAP_SRC,
    GSENSOR_REG_SIXD_SRC,
    GSENSOR_REG_ALL_INT_SRC,
    GSENSOR_REG_X_OFS_USR,
    GSENSOR_REG_Y_OFS_USR,
    GSENSOR_REG_Z_OFS_USR,
    GSENSOR_REG_CTRL_REG7,
} gsensor_regs_t;


static errcode_t gensor_i2c_write_regsiter(uint8_t addr, uint8_t val)
{
    errcode_t ret;
    uint8_t buf[2] = {0};
    buf[0] = addr;
    buf[1] = val;
    i2c_data_t gsensor_xfer_data = {0};
    gsensor_xfer_data.send_buf = buf;
    gsensor_xfer_data.send_len = sizeof(buf);
    return uapi_i2c_master_write(CONFIG_SENSOR_I2C_BUS_ID, CONFIG_GSENSOR_I2C_SLAVE_ADDR, &gsensor_xfer_data);
}

static errcode_t gensor_i2c_read_regsiter(uint8_t addr, uint8_t *val, uint8_t len)
{
    errcode_t ret;
    uint8_t tx_buf = addr;
    i2c_data_t gsensor_xfer_data = {0};
    gsensor_xfer_data.send_buf = &tx_buf;
    gsensor_xfer_data.send_len = sizeof(tx_buf);
    gsensor_xfer_data.receive_buf = val;
    gsensor_xfer_data.receive_len = len;
    return uapi_i2c_master_writeread(CONFIG_SENSOR_I2C_BUS_ID, CONFIG_GSENSOR_I2C_SLAVE_ADDR, &gsensor_xfer_data);
}

static errcode_t gsensor_check_chipid(void)
{
    uint8_t val = 0;
    errcode_t ret = gensor_i2c_read_regsiter(GSENSOR_REG_WHO_AM_I, &val, 1);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return (val == GSENSOR_CHIPID_VAL) ? ERRCODE_SUCC : ERRCODE_FAIL;
}

static errcode_t gsensor_device_init(gensor_lp_odr_t sample_freq, gensor_bandwidth_t bandwidth)
{
    errcode_t ret;
    uint8_t cnt = 0;
    do {
        ret = gsensor_check_chipid();
        if (ret == ERRCODE_SUCC) {
            break;
        } else {
            ctrl2_reg_t val;
            val.d8 = 0;
            val.b.boot = 1;
            ret = gensor_i2c_write_regsiter(GSENSOR_REG_CTRL2, val.d8);
            if (ret != ERRCODE_SUCC) {
                return ret;
            }
        }
        cnt++;
        if (cnt > 5) {  // 5：在连接不通的情况下，尝试5次
            break;
        }
    } while (ret != ERRCODE_SUCC);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ctrl2_reg_t ctrl2_data_tmp;
    ctrl2_data_tmp.d8 = 0;
    ctrl2_data_tmp.b.soft_reset = 1;
    ret = gensor_i2c_write_regsiter(GSENSOR_REG_CTRL2, ctrl2_data_tmp.d8);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ctrl1_reg_t ctrl1_data;
    ctrl1_data.d8 = 0;
    ctrl1_data.b.odr = sample_freq;    // 采样率及low power mode
    ctrl1_data.b.reserved0 = 0;
    ctrl1_data.b.reserved1 = 1;
    ret = gensor_i2c_write_regsiter(GSENSOR_REG_CTRL1, ctrl1_data.d8);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ctrl2_reg_t ctrl2_data;
    ctrl2_data.d8 = 0;
    ctrl2_data.b.if_add_inc = 1;      // 地址自增
    ctrl2_data.b.bdu = 1;             // block data update
    ret = gensor_i2c_write_regsiter(GSENSOR_REG_CTRL2, ctrl2_data.d8);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ctrl3_reg_t ctrl3_data;
    ctrl3_data.d8 = 0;
    ctrl3_data.b.slp_mode_sel = 1;   // 通过i2c触发数据采样
    ret = gensor_i2c_write_regsiter(GSENSOR_REG_CTRL3, ctrl3_data.d8);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ctrl6_reg_t ctrl6_data;
    ctrl6_data.d8 = 0;
    ctrl6_data.b.low_noise = 1;       // 启用低噪声
    ctrl6_data.b.fs = FULLSCALE_4G;   // 测量范围设置为4g
    ctrl6_data.b.bw_filt = bandwidth; // 设置带宽
    ret = gensor_i2c_write_regsiter(GSENSOR_REG_CTRL6, ctrl6_data.d8);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    fifo_ctrl_reg_t fifo_ctrl_data;
    fifo_ctrl_data.d8 = 0;
    fifo_ctrl_data.b.fth = 0x4;
    fifo_ctrl_data.b.fmode = 0x6;       // fifo 连续模式
    ret = gensor_i2c_write_regsiter(GSENSOR_REG_FIFO_CTRL, fifo_ctrl_data.d8);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ERRCODE_SUCC;
}

static uint8_t gsensor_fifo_len_get(void)
{
    fifo_samples_reg_t fifo_samples_data;
    fifo_samples_data.d8 = 0;
    gensor_i2c_read_regsiter(GSENSOR_REG_FIFO_SAMPLES, &fifo_samples_data, 1);
    if (fifo_samples_data.b.diff == 0) {
        return 0;
    }
    if (fifo_samples_data.b.diff > GSENSOR_FIFO_DEPTH) {
        return GSENSOR_FIFO_DEPTH;
    }
    return fifo_samples_data.b.diff;
}

static int16_t gsensor_acc_data_get(uint16_t data)
{
    uint16_t temp = 0;
    if (((data >> 15) & 0x1) == 0x1) {     // 15：看最高位为0还是1
        temp = (data >> 2) | 0xc000;       // 2:去掉最低两位无效数字
    } else {
        temp = data >> 2;                  // 2:去掉最低两位无效数字
    }
    return (int16_t)temp;
}

static void gsensor_timer_handle(unsigned long data)
{
    unused(data);
    ctrl3_reg_t ctrl3_data;
    ctrl3_data.d8 = 0;
    ctrl3_data.b.slp_mode_sel = 1;
    ctrl3_data.b.slp_mode_1 = 1;
    if (gensor_i2c_write_regsiter(GSENSOR_REG_CTRL3, ctrl3_data.d8) != ERRCODE_SUCC) {
        return;
    }
    uint8_t data_len = gsensor_fifo_len_get();
    if (data_len == 0) {
        return;
    }
    memset_s(g_gsensor_fifo, sizeof(g_gsensor_fifo), 0, sizeof(g_gsensor_fifo));
    if (gensor_i2c_read_regsiter(GSENSOR_REG_OUT_X_L, g_gsensor_fifo, data_len * GSENSOR_XTZ_REG_NUM) != ERRCODE_SUCC) {
        return;
    }
    gsensor_data_t gsensor_data;
    for (uint8_t i = 0; i < data_len; i++) {
        gsensor_data.accel_raw[GSENSOR_X_ACC_IDX] = gsensor_acc_data_get(((g_gsensor_fifo[i * GSENSOR_ACC_REG_LEN +
            GSENSOR_XH_OFS] << GSENSOR_DATA_OFS) | (g_gsensor_fifo[i * GSENSOR_ACC_REG_LEN + GSENSOR_XL_OFS] & 0xfc)));
        gsensor_data.accel_raw[GSENSOR_Y_ACC_IDX] = gsensor_acc_data_get(((g_gsensor_fifo[i * GSENSOR_ACC_REG_LEN +
            GSENSOR_YH_OFS] << GSENSOR_DATA_OFS) | (g_gsensor_fifo[i * GSENSOR_ACC_REG_LEN + GSENSOR_YL_OFS] & 0xfc)));
        gsensor_data.accel_raw[GSENSOR_Z_ACC_IDX] = gsensor_acc_data_get(((g_gsensor_fifo[i * GSENSOR_ACC_REG_LEN +
            GSENSOR_ZH_OFS] << GSENSOR_DATA_OFS) | (g_gsensor_fifo[i * GSENSOR_ACC_REG_LEN + GSENSOR_ZL_OFS] & 0xfc)));

        gsensor_data.accel_proc[GSENSOR_X_ACC_IDX] = - ((float)(gsensor_data.accel_raw[GSENSOR_X_ACC_IDX]) /
            (1 << GSENSOR_DATA_RESOLUTION)) * (GSENSOR_FULL_SCALE * GSENSOR_G);
        gsensor_data.accel_proc[GSENSOR_Y_ACC_IDX] = - ((float)(gsensor_data.accel_raw[GSENSOR_Y_ACC_IDX]) /
            (1 << GSENSOR_DATA_RESOLUTION)) * (GSENSOR_FULL_SCALE * GSENSOR_G);
        gsensor_data.accel_proc[GSENSOR_Z_ACC_IDX] = - ((float)(gsensor_data.accel_raw[GSENSOR_Z_ACC_IDX]) /
            (1 << GSENSOR_DATA_RESOLUTION)) * (GSENSOR_FULL_SCALE * GSENSOR_G);

        queue_enqueue(gsensor_data);
    }
    osal_timer_start(&g_gsensor_timer);
}

static errcode_t gsensor_timer_init(void)
{
    g_gsensor_timer.handler = gsensor_timer_handle;
    g_gsensor_timer.data = 0;
    g_gsensor_timer.interval = GSENSOR_TIMER_INTERVAL;               // 每隔400ms取一次数据放到队列中
    if (osal_timer_init(&g_gsensor_timer) != OSAL_SUCCESS) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t gsensor_driver_init(gensor_lp_odr_t sample_freq, gensor_bandwidth_t bandwidth)
{
    if (gsensor_open_flag == 1) {
        return ERRCODE_SUCC;
    }

    queue_mutex_init();
    queue_init();

    errcode_t ret;
    ret = gsensor_timer_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = gsensor_device_init(sample_freq, bandwidth);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    if (osal_timer_start(&g_gsensor_timer) != OSAL_SUCCESS) {
        return ERRCODE_FAIL;
    }

    gsensor_open_flag = 1;
    return ERRCODE_SUCC;
}

errcode_t gsensor_driver_deinit(void)
{
    gsensor_open_flag = 0;
    int ret = 0;
    ret = osal_timer_stop(&g_gsensor_timer);
    if (ret == OSAL_FAILURE) {
        return ERRCODE_FAIL;
    }
    ret = osal_timer_destroy(&g_gsensor_timer);
    if (ret == OSAL_FAILURE) {
        return ERRCODE_FAIL;
    }
    queue_mutex_deinit();
    g_gsensor_timer.timer = NULL;

    return ERRCODE_SUCC;
}

errcode_t gsensor_driver_acc_data_get(gsensor_data_t *data)
{
    return queue_dequeue(data);
}