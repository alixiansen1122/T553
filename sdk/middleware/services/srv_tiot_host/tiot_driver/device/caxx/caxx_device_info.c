/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2024. All rights reserved.
 *
 * Description: CAXX device info. \n
 *
 * History: \n
 * 2024-01-02, Create file. \n
 */

#include "tiot_controller.h"
#include "tiot_device_info.h"
#include "tiot_cfg_handle_xci.h"
#include "tiot_pm_default.h"
#include "tiot_board_log.h"

#include "caxx_board_port.h"
#include "caxx_device_info.h"

#define CAXX_POWERON_WAIT_MS      20     /* Wait time before power on. */
#define CAXX_BOOT_TIME_MS         12     /* Power on ==> Boot time. */
#define CAXX_INIT_TIME_MS         30     /* Boot ==> Init time. */
#define CAXX_WAKEUP_PULSE_MS      1      /* GPIO wake up pulse. */
#define CAXX_WAKEUP_WAIT_MS       1      /* Wake up wait ms. */
#define CAXX_BAUD_CHANGE_WAIT_US  0

#define CAXX_I2C_BAUDRATE   400000
#define CAXX_I2C_DEV_ADDR   0x28

#define CAXX_PM_EVENT_MAP_SIZE (TIOT_PM_EVENT_WAKEUP + 1)

static tiot_pm_event_entry g_caxx_pm_event_map[CAXX_PM_EVENT_MAP_SIZE] = {
    { TIOT_PM_TAG_CAN_POWERON, TIOT_PM_STATE_POWEROFF, tiot_pm_default_power_off },
    /* 上电后等待init消息或业务消息才说明进入work状态 */
    { TIOT_PM_TAG_CAN_POWEROFF, TIOT_PM_STATE_POWERON, tiot_pm_default_power_on },
    /* 只有发送失败时才尝试唤醒，或者在业务接口内提前唤醒，只需处于上电态即可 */
    { TIOT_PM_TAG_CAN_POWERON, TIOT_PM_STATE_POWERON, tiot_pm_default_wakeup_device_by_gpio },
};

static void caxx_firmware_before_load(tiot_fw *fw)
{
    tiot_xmit_config caxx_fw_load_cfg = {
        .i2c_config = {
            .speed = CAXX_I2C_BAUDRATE,
            .addr  = CAXX_I2C_DEV_ADDR
        }
    };
    tiot_controller *ctrl = tiot_container_of(fw, tiot_controller, firmware);
    if (tiot_xfer_set_config(&ctrl->transfer, &caxx_fw_load_cfg) != ERRCODE_TIOT_SUCC) {
        tiot_print_dbg("[TIoT][caxx]config before load fail.\r\n");
    }
}

static int caxx_rx_wait_func(const void *param)
{
    uint8_t level;
    tiot_xfer_manager *xfer = (tiot_xfer_manager *)param;
    tiot_controller *ctrl = tiot_container_of(xfer, tiot_controller, transfer);
    /* 此处wakein_pin用作中断管脚，通知是否有数据上报 */
    uint32_t wakein_pin = ctrl->pm.pm_info[TIOT_PIN_WAKE_IN];
    (void)tiot_board_pin_get_level(wakein_pin, &level);
    return level == TIOT_PIN_LEVEL_HIGH ? 1 : 0;
}

static const tiot_device_info g_caxx_device_info = {
    .pm_event_map       = g_caxx_pm_event_map,
    .pm_event_map_size  = CAXX_PM_EVENT_MAP_SIZE,
    .exec_cbs   = { caxx_firmware_before_load, NULL, tiot_fw_ext_cmd_handle_xci, NULL },
    .xfer_info  = { TIOT_XMIT_TYPE_I2C, caxx_rx_wait_func },
    .timings    = {
        .power_on_wait_ms = CAXX_POWERON_WAIT_MS,
        .boot_time_ms     = CAXX_BOOT_TIME_MS,
        .init_time_ms     = CAXX_INIT_TIME_MS,
        .wakeup_pulse_ms  = CAXX_WAKEUP_PULSE_MS,
        .wakeup_wait_ms   = CAXX_WAKEUP_WAIT_MS,
        .baud_change_wait_us = CAXX_BAUD_CHANGE_WAIT_US,
    },
};

const tiot_device_info *caxx_device_get_info(void)
{
    return &g_caxx_device_info;
}
