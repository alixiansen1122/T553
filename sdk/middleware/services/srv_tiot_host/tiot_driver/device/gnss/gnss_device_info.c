/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: GNSS device info. \n
 *
 * History: \n
 * 2023-03-25, Create file. \n
 */

#include "gnss_device_info.h"
#include "tiot_pm_default.h"
#include "tiot_board_uart_port.h"
#include "tiot_board_log.h"
#include "tiot_controller.h"
#include "tiot_board_pin_port.h"
#include "tiot_cfg_handle_priv.h"
#include "tiot_sys_msg_handle.h"

#define GNSS_POWERON_WAIT_MS      1      /* Wait time before power on. */
#define GNSS_BOOT_TIME_MS         200    /* Power on ==> Boot time. */
#define GNSS_INIT_TIME_MS         600    /* Boot ==> Init time. */
#define GNSS_WAKEUP_PULSE_MS      1      /* GPIO wake up pulse. */
#define GNSS_WAKEUP_WAIT_MS       50     /* Wake up wait ms. */
#define GNSS_BAUD_CHANGE_WAIT_US  11000  /* Baudrate change wait μs. */

#ifdef CONFIG_GNSS_TCXO_FREQ_38P4M
#define GNSS_WAKEUP_PIN_INIT_LEVEL  TIOT_PIN_LEVEL_LOW
#else
#define GNSS_WAKEUP_PIN_INIT_LEVEL  TIOT_PIN_LEVEL_HIGH
#endif

static inline int32_t gnss_pin_set_level_if_exist(uint32_t pin, uint32_t pin_level)
{
    if (pin != TIOT_PIN_NONE) {
        return tiot_board_pin_set_level(pin, pin_level);
    }
    return ERRCODE_TIOT_SUCC;
}

static tiot_pm_retcode gnss_pm_power_on(tiot_pm *pm)
{
    uint32_t wakeout_pin = pm->pm_info[TIOT_PIN_WAKE_OUT];
    /* GNSS板级连接唤醒管脚时，上拉或下拉 */
    if (gnss_pin_set_level_if_exist(wakeout_pin, GNSS_WAKEUP_PIN_INIT_LEVEL) != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIoT][pm]set wakeout pin level %d fail.\n", GNSS_WAKEUP_PIN_INIT_LEVEL);
        return TIOT_PM_EVTRET_FAIL;
    }
    if (tiot_pm_default_power_on(pm) == TIOT_PM_EVTRET_FAIL) {
        return TIOT_PM_EVTRET_FAIL;
    }
    if (gnss_pin_set_level_if_exist(wakeout_pin, TIOT_PIN_LEVEL_LOW) != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIoT][pm]set wakeout pin level low fail.\n");
        return TIOT_PM_EVTRET_FAIL;
    }
    return TIOT_PM_EVTRET_NEXT_STATE;
}

static const tiot_pm_event_entry g_gnss_pm_event_map[TIOT_PM_EVENT_MAX] = {
    { TIOT_PM_TAG_CAN_POWERON, TIOT_PM_STATE_POWEROFF, tiot_pm_default_power_off },
    /* 上电后等待init消息或业务消息才说明进入work状态 */
    /* GNSS板级连接唤醒管脚时，上电特殊处理 */
    { TIOT_PM_TAG_CAN_POWEROFF, TIOT_PM_STATE_INIT, gnss_pm_power_on },
#ifdef CONFIG_GNSS_WAKEUP_TYPE_UART
    /* 只有sleep态才需要唤醒 */
    { TIOT_PM_TAG_CAN_WAKEUP, TIOT_PM_STATE_WAKING, tiot_pm_default_wakeup_device_by_uart },
#else
    /* 只有sleep态才需要唤醒 */
    { TIOT_PM_TAG_CAN_WAKEUP, TIOT_PM_STATE_WAKING, tiot_pm_default_wakeup_device_by_gpio },
#endif
    /* 有device wakeup host管脚时，通过此管脚说明已唤醒，且需要下发disallow消息 */
    { TIOT_PM_TAG_CAN_POWERON, TIOT_PM_STATE_WORK, tiot_pm_default_wakeup_ack_handle },
    /* 接收到request sleep消息，下发allow sleep消息 */
    { TIOT_PM_TAG_CAN_ALLOW_SLEEP, TIOT_PM_STATE_SLEEPING, tiot_pm_default_request_sleep_handle },
    /* 接收到dev wkup host gpio拉低，需要释放唤醒锁 */
    { TIOT_PM_TAG_CAN_SLEEP, TIOT_PM_STATE_SLEEP, tiot_pm_default_sleep_ack_handle },
    /* 非S3状态，下发allow sleep并释放唤醒锁 */
    { TIOT_PM_TAG_CAN_ALLOW_SLEEP, TIOT_PM_STATE_SLEEP, tiot_pm_default_request_sleep_ack_handle },
    /* 上报init消息或业务消息说明唤醒成功 */
    { TIOT_PM_TAG_CAN_WORK, TIOT_PM_STATE_WORK, tiot_pm_default_report_work },
    { TIOT_PM_TAG_CAN_POWERON, TIOT_PM_STATE_INVALID, tiot_pm_default_work_vote_up },
    { TIOT_PM_TAG_CAN_POWERON, TIOT_PM_STATE_INVALID, tiot_pm_default_work_vote_down }
};

static void gnss_firmware_before_load(tiot_fw *fw)
{
    /* 加载前先清掉rx_buff */
#ifdef CONFIG_XFER_DEFAULT_RX_BUFF
    tiot_controller *ctrl = tiot_container_of(fw, tiot_controller, firmware);
    tiot_xfer_manager *xfer = &ctrl->transfer;
    if (xfer->xmit_ops->rx_mode == TIOT_XMIT_RX_MODE_BUFFED) {
        circ_buf_flush(&xfer->rx_buff);
    }
#endif
}

static void gnss_firmware_after_load(tiot_fw *fw)
{
    /* 加载过程中可能切换波特率，加载后设置为业务默认波特率 */
    tiot_xmit_config gnss_transfer_config = def_uart_cfg(GNSS_UART_DEFAULT_BAUDRATE,
                                                         GNSS_UART_ATTR_FLOW_CTRL);
    tiot_controller *ctrl = tiot_container_of(fw, tiot_controller, firmware);
    tiot_xfer_manager *xfer = &ctrl->transfer;
    if (tiot_xfer_set_config(xfer, &gnss_transfer_config) != ERRCODE_TIOT_SUCC) {
        tiot_print_dbg("[TIoT:fw]config after load fail.\r\n");
    }
}

static int gnss_rx_wait_func(const void *param)
{
    (void)param;
    /* GNSS通过UART上报数据，不需要拉高管脚来通知读取 */
    return 0;
}

static const tiot_device_info g_gnss_device_info = {
    .pm_event_map = (tiot_pm_event_entry *)g_gnss_pm_event_map,
    .pm_event_map_size = sizeof(g_gnss_pm_event_map) / sizeof(tiot_pm_event_entry),
    .exec_cbs = { gnss_firmware_before_load, gnss_firmware_after_load, tiot_fw_ext_cmd_handle_priv, NULL },
    .xfer_info = { TIOT_XMIT_TYPE_UART, gnss_rx_wait_func },
    .pkt_sys_msg_handle = tiot_pkt_sys_msg_handle,
    .timings = {
        .power_on_wait_ms = GNSS_POWERON_WAIT_MS,
        .boot_time_ms = GNSS_BOOT_TIME_MS,
        .init_time_ms = GNSS_INIT_TIME_MS,
        .wakeup_pulse_ms = GNSS_WAKEUP_PULSE_MS,
        .wakeup_wait_ms = GNSS_WAKEUP_WAIT_MS,
        .baud_change_wait_us = GNSS_BAUD_CHANGE_WAIT_US,
    }
};

const tiot_device_info *gnss_device_get_info(void)
{
    return &g_gnss_device_info;
}
