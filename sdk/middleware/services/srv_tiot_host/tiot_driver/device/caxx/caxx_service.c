/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2024. All rights reserved.
 *
 * Description: CAXX service interface. \n
 *
 * History: \n
 * 2024-01-02, Create file. \n
 */
#include "tiot_board_log.h"
#include "tiot_controller.h"
#include "tiot_controller_helper.h"
#include "tiot_nci_packet.h"
#include "tiot_board_pin_port.h"
#include "tiot_board.h"
#include "tiot_service_interface.h"
#include "tiot_pm_wakelock.h"
#include "caxx_board_port.h"
#include "caxx_device_info.h"
#include "caxx_service.h"

#ifdef CONFIG_FILE_BY_ARRAY
#include "caxx_fw_file_array.h"
#endif
#define CAXX_MAX_NUM_OF_RE_WRITE 10
static tiot_controller g_caxx_controllers[CONFIG_CAXX_DEV_NUM] = { 0 };

static int32_t caxx_service_install_packet(tiot_controller *ctrl, uint8_t dev_id)
{
    tiot_unused(dev_id);
    int32_t ret;
    tiot_xfer_packet_ops caxx_packet_ops = {
        .read_out = tiot_nci_rx_data_out,
        .rx_data_store = NULL, /* use default */
        .tx_push = NULL, /* use default */
    };
    const tiot_device_timings *timings = &ctrl->dev_info->timings;

    /* 收发模块注册包格式 */
    ret = tiot_xfer_install_packet(&ctrl->transfer, &caxx_packet_ops, (uintptr_t)NULL);
    if (ret != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIoT]packet install fail.\n");
        return ret;
    }
    /* 必须等待device I2C初始化后才能接收消息, 待确认时间. */
    osal_msleep(timings->init_time_ms);
    tiot_pm_set_event(&ctrl->pm, TIOT_PM_EVENT_REPORT_WORK);
    return ERRCODE_TIOT_SUCC;
}

static inline int32_t caxx_get_dev_id(tiot_controller *ctrl, uint8_t *dev_id)
{
    uint8_t id;
    for (id = 0; id < CONFIG_CAXX_DEV_NUM; id++) {
        if (ctrl == &g_caxx_controllers[id]) {
            *dev_id = id;
            return ERRCODE_TIOT_SUCC;
        }
    }
    tiot_print_err("[TIoT]invalid device\n");
    return ERRCODE_TIOT_DEV_INVALID;
}

static void caxx_wakein_cb(uint32_t pin)
{
    (void)tiot_board_pin_enable_int(pin, TIOT_PIN_INT_DISABLE);
    tiot_controller *ctrl = tiot_find_controller_by_pin(g_caxx_controllers, CONFIG_CAXX_DEV_NUM,
                                                        pin, TIOT_PIN_WAKE_IN);
    /* 有数据上报 */
    osal_wait_wakeup_interruptible(&ctrl->transfer.rx_wait);
    (void)tiot_board_pin_enable_int(pin, TIOT_PIN_INT_ENABLE);
}

static inline int32_t caxx_firmware_load(tiot_fw *firmware)
{
    int32_t ret;
#ifdef CONFIG_FILE_BY_ARRAY
    ret = tiot_firmware_load(firmware, caxx_fw_file_path_get(), TIOT_FW_FILE_ARRAY_CFG_NAME);
#else
    ret = tiot_firmware_load(firmware, NULL, caxx_board_get_info()->cfg_path);
#endif
    if (ret != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIoT] Caxx firmware load fail.\n");
    }
    return ret;
}

static int32_t caxx_service_open(tiot_controller *ctrl, void *param)
{
    int32_t ret;
    uint8_t dev_id;
    uint32_t wakein_pin;
    tiot_xfer_open_param xfer_open_param = { 0 };

    ret = caxx_get_dev_id(ctrl, &dev_id);
    if (ret != ERRCODE_TIOT_SUCC) {
        return ret;
    }
    if (param != NULL) {
        tiot_xfer_cbk_param *cbk_param = param;
        xfer_open_param.rx_cbk_param.buff          = cbk_param->buff;
        xfer_open_param.rx_cbk_param.buff_len      = cbk_param->buff_len;
        xfer_open_param.rx_cbk_param.rx_callback   = cbk_param->rx_callback;
    }
    ret = tiot_xfer_open(&ctrl->transfer, &xfer_open_param);
    if (ret != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIoT]xfer open fail.\n");
        goto xfer_open_fail;
    }
    /* 使能中断 */
    wakein_pin = ctrl->pm.pm_info[TIOT_PIN_WAKE_IN];
    tiot_pin_int_request(wakein_pin, caxx_wakein_cb);
    /* 上电 */
    ret = tiot_pm_set_event(&ctrl->pm, TIOT_PM_EVENT_POWERON);
    if (ret != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIoT]power on fail.\n");
        goto power_on_fail;
    }
    ret = caxx_firmware_load(&ctrl->firmware);
    if (ret != ERRCODE_TIOT_SUCC) {
        goto fw_load_fail;
    }
    ret = caxx_service_install_packet(ctrl, dev_id);
    if (ret != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIoT]install packet fail.\n");
        goto fw_load_fail;
    }
    tiot_print_suc("[TIoT]controller open success.\n");
    tiot_pm_wake_unlock(); //  释放上电时的唤醒锁
    return ERRCODE_TIOT_SUCC;

fw_load_fail:
    (void)tiot_pm_set_event(&ctrl->pm, TIOT_PM_EVENT_POWEROFF);
power_on_fail:
    tiot_pin_int_release(wakein_pin);
    tiot_xfer_close(&ctrl->transfer);
xfer_open_fail:
    return ret;
}

static void caxx_service_close(tiot_controller *ctrl)
{
    uint8_t dev_id;
    uint32_t wakein_pin;

    if (caxx_get_dev_id(ctrl, &dev_id) != ERRCODE_TIOT_SUCC) {
        return;
    }

    (void)tiot_pm_set_event(&ctrl->pm, TIOT_PM_EVENT_POWEROFF);
    wakein_pin = ctrl->pm.pm_info[TIOT_PIN_WAKE_IN];
    tiot_pin_int_release(wakein_pin);
    tiot_xfer_uninstall_packet(&ctrl->transfer);
    tiot_xfer_close(&ctrl->transfer);
}

static int32_t caxx_service_write(tiot_controller *ctrl, const uint8_t *data,
                                  uint32_t len, const tiot_xfer_tx_param *param)
{
    int32_t ret;
    uint8_t dev_id;
    uint8_t loop = 0;
    tiot_unused(param);
    const tiot_device_timings *timings = &ctrl->dev_info->timings;

    ret = caxx_get_dev_id(ctrl, &dev_id);
    if (ret != ERRCODE_TIOT_SUCC) {
        return ret;
    }

    while (loop < CAXX_MAX_NUM_OF_RE_WRITE) {
        ret = tiot_xfer_send(&ctrl->transfer, data, len, NULL);
        if (ret != (int32_t)len) {
            ret = tiot_pm_set_event(&ctrl->pm, TIOT_PM_EVENT_WAKEUP);
            if (ret != ERRCODE_TIOT_SUCC) {
                tiot_print_err("[TIoT]wake up fail.\n");
                return ret;
            }
            osal_msleep(timings->wakeup_wait_ms);
            loop++;
        } else {
            break;
        }
    }

    if (loop == CAXX_MAX_NUM_OF_RE_WRITE) {
        tiot_print_err("[TIoT]caxx write fail.\n");
        return ERRCODE_TIOT_XFER_WRITE_FAIL;
    }
    return ret;
}

static int32_t caxx_service_read(tiot_controller *ctrl, uint8_t *buff, uint32_t buff_len,
                                 const tiot_xfer_rx_param *param)
{
    int32_t ret;
    uint8_t dev_id;

    ret = caxx_get_dev_id(ctrl, &dev_id);
    if (ret != ERRCODE_TIOT_SUCC) {
        return ret;
    }
    return tiot_xfer_recv(&ctrl->transfer, buff, buff_len, param);
}

static void service_deinit(uint8_t ctrl_num, uint8_t board_num)
{
    uint8_t i;
    for (i = 0; i < ctrl_num; i++) {
        tiot_controller_deinit(&g_caxx_controllers[i]);
    }
    for (i = 0; i < board_num; i++) {
        caxx_board_deinit(&i);
    }
}

int32_t caxx_service_init(void)
{
    int32_t ret;
    uint8_t dev_id;
    tiot_controller_info ctrl_info;
    tiot_board_info per_board_info;
    const caxx_board_info *board_info;

    for (dev_id = 0; dev_id < CONFIG_CAXX_DEV_NUM; dev_id++) {
        if (caxx_board_init(&dev_id) != ERRCODE_TIOT_SUCC) {
            tiot_print_err("[TIoT]board init fail.\n");
            service_deinit(0, dev_id);
            return ERRCODE_TIOT_DEV_BOARD_INIT_FAIL;
        }
    }

    board_info = caxx_board_get_info();
    if (!board_info) {
        tiot_print_err("[TIoT]no board info.\n");
        service_deinit(0, CONFIG_CAXX_DEV_NUM);
        return ERRCODE_TIOT_DEV_NO_BOARD_INFO;
    }
    for (dev_id = 0; dev_id < CONFIG_CAXX_DEV_NUM; dev_id++) {
        per_board_info.hw_info.xmit_id = board_info->hw_infos[dev_id].xmit_id;
        per_board_info.hw_info.pm_info = board_info->hw_infos[dev_id].pm_info;
        ctrl_info.board_info = &per_board_info;
        ctrl_info.dev_info = caxx_device_get_info();
        ctrl_info.dev_cus = 0;
        ctrl_info.is_host = true;
        ret = tiot_controller_init(&g_caxx_controllers[dev_id], &ctrl_info);
        if (ret != ERRCODE_TIOT_SUCC) {
            tiot_print_err("[TIoT]controller init fail.\n");
            service_deinit(dev_id, CONFIG_CAXX_DEV_NUM);
            return ret;
        }
    }

    for (dev_id = 0; dev_id < CONFIG_CAXX_DEV_NUM; dev_id++) {
        g_caxx_controllers[dev_id].ops.open  = caxx_service_open;
        g_caxx_controllers[dev_id].ops.close = caxx_service_close;
        g_caxx_controllers[dev_id].ops.write = caxx_service_write;
        g_caxx_controllers[dev_id].ops.read  = caxx_service_read;
        g_caxx_controllers[dev_id].ops.pm_ctrl = NULL;
    }
    tiot_print_info("[TIoT]service init success.\n");
    return ERRCODE_TIOT_SUCC;
}

void caxx_service_deinit(void)
{
    service_deinit(CONFIG_CAXX_DEV_NUM, CONFIG_CAXX_DEV_NUM);
}

uintptr_t caxx_service_get_ctrl(uint8_t dev_id)
{
    return (uintptr_t)&g_caxx_controllers[dev_id];
}
