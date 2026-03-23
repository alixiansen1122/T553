/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description:  \n
 *
 * History: \n
 * 2023-03-31, Create file. \n
 */
#include "gnss_service.h"
#include "gnss_device_info.h"
#include "gnss_board_port.h"
#include "tiot_board_log.h"
#include "tiot_controller.h"
#include "tiot_controller_helper.h"
#include "tiot_packet.h"
#include "tiot_sys_msg_types.h"
#include "tiot_board_pin_port.h"
#include "tiot_board_uart_port.h"
#include "tiot_board_misc_port.h"
#include "tiot_board.h"
#include "tiot_service_interface.h"
#ifdef CONFIG_FILE_BY_ARRAY
#include "gnss_fw_file_array.h"
#endif

#ifndef CONFIG_BOARD_DYNAMIC_ALLOC
/* 加载 & 数据收发复用buffer，不同时使用 */
/* 数据收发需求 ~10K用于缓存数据. */
/* 固件加载时 ~3K用于保存解析后命令, ~128字节用于加载回复接收. */
#define GNSS_FW_LOAD_REUSE_BUFF_START 0U
#define GNSS_FW_LOAD_RX_BUFF_START    GNSS_FW_LOAD_REUSE_BUFF_START
#define GNSS_FW_LOAD_RX_BUFF_SIZE     128U

#define GNSS_FW_LOAD_REUSE_BUFF_END   (GNSS_FW_LOAD_RX_BUFF_START + GNSS_FW_LOAD_RX_BUFF_SIZE)

#define GNSS_COMMON_BUFF_MAX    ((CONFIG_GNSS_PACKET_BUFF_MAX > GNSS_FW_LOAD_REUSE_BUFF_END) ? \
                                 CONFIG_GNSS_PACKET_BUFF_MAX : GNSS_FW_LOAD_REUSE_BUFF_END)
#endif

#define GNSS_PACKET_QUEUE_NODES    8

typedef enum {
    GNSS_RX_SUBSYS_0 = 0,
    GNSS_RX_SUBSYS_MAX
} gnss_rx_subsys_id;

typedef enum {
    GNSS_PLT_SWITCH_GUART_BPS = 0,
    GNSS_PLT_SWITCH_DBGUART_BPS,
    GNSS_PLT_SWITCH_UART_SELECT,
    GNSS_PLT_SWITCH_HSO_OR_SDT,
    GNSS_PLT_SWITCH_DBGUART_SKIP,
    GNSS_PLT_CMD_MAXSIZE
} gnss_plt_cmd;

typedef struct {
    uint32_t bps;
    uint32_t cr_crg;
} tiot_plt_guart_para;

typedef struct {
    uint8_t frame_start;
    uint8_t func_type;
    uint8_t prime_id;
    uint8_t reserved;
    uint16_t frame_len;
    uint16_t sn;
} tiot_plt_om_header;

typedef struct {
    uint16_t cmd;
    uint16_t para_len;
} tiot_plt_cmd_header;

#pragma pack(push, 1)
typedef struct {
    tiot_plt_om_header om_header;
    tiot_plt_cmd_header cmd_header;
    tiot_plt_guart_para para;
    uint8_t tail;
} tiot_plt_guart_cmd;
#pragma pack(pop)

#define tiot_plat_guart_baud_cmd(bps, flow_ctrl)                                \
{                                                                               \
    .om_header = { 0x7e, 0x0c, 0x00, 0x00, 0x15, 0x00 },                        \
    .cmd_header = { GNSS_PLT_SWITCH_GUART_BPS, sizeof(tiot_plt_guart_para) },   \
    .para = { (bps),                                                            \
              ((flow_ctrl) == TIOT_UART_ATTR_FLOW_CTRL_ENABLE) ? 0xC301 : 0x301 \
    },                                                                          \
    .tail = 0x7e                                                                \
}

static tiot_controller g_gnss_controllers[CONFIG_GNSS_DEV_NUM] = { 0 };
static tiot_packet_context g_gnss_pkt_ctx[CONFIG_GNSS_DEV_NUM];
static tiot_packet g_gnss_pkt_queue_nodes[CONFIG_GNSS_DEV_NUM][GNSS_PACKET_QUEUE_NODES];
static tiot_packet_queue g_gnss_pkt_queues[CONFIG_GNSS_DEV_NUM][GNSS_RX_SUBSYS_MAX];

#ifndef CONFIG_BOARD_DYNAMIC_ALLOC
static uint8_t g_gnss_common_buff_mem[CONFIG_GNSS_DEV_NUM][GNSS_COMMON_BUFF_MAX];
#endif

static uint16_t gnss_packet_manager_match_func(uint32_t subsys_code)
{
    if ((subsys_code == GNSS_FIRST_MSG) || (subsys_code == GNSS_COMMON_MSG) ||
        (subsys_code == GNSS_LAST_MSG) || (subsys_code == ASCII_MSG)) {
        return GNSS_RX_SUBSYS_0;
    }
    return GNSS_RX_SUBSYS_MAX;
}

static void gnss_wakein_cb(uint32_t pin)
{
    (void)tiot_board_pin_enable_int(pin, TIOT_PIN_INT_DISABLE);
    tiot_controller *cur_ctrl = tiot_find_controller_by_pin(g_gnss_controllers, CONFIG_GNSS_DEV_NUM,
                                                            pin, TIOT_PIN_WAKE_IN);
    if (cur_ctrl != NULL) {
        tiot_pm_set_event(&cur_ctrl->pm, TIOT_PM_EVENT_WAKEUP_ACK);
    }
    (void)tiot_board_pin_enable_int(pin, TIOT_PIN_INT_ENABLE);
}

static int32_t gnss_change_uart_config(tiot_xfer_manager *xfer, tiot_xmit_config *uart_cfg)
{
    int32_t ret;
    tiot_plt_guart_cmd uart_cmd = tiot_plat_guart_baud_cmd(uart_cfg->uart_config.baudrate,
                                                           uart_cfg->uart_config.attr.flow_ctrl);
    tiot_xfer_tx_param param = { SUBSYS_OM };

    ret = tiot_xfer_send(xfer, (const uint8_t *)&uart_cmd, sizeof(tiot_plt_guart_cmd), &param);
    if (ret != sizeof(tiot_plt_guart_cmd)) {
        tiot_print_err("[TIoT]set user baudrate cmd fail.\r\n");
        return ret;
    }
    ret = tiot_xfer_set_config(xfer, uart_cfg);
    if (ret != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIoT]set user baudrate fail.\r\n");
    }
    return ret;
}

static inline int32_t gnss_send_host_need_wakeup(tiot_controller *ctrl)
{
    int32_t ret;
    const uint8_t host_need_wakeup_cmd = SYS_PL_PM_SET_WAKEUP_HOST_VALID;
    const tiot_xfer_tx_param param = { SUBSYS_SYS };

    ret = tiot_xfer_send(&ctrl->transfer, &host_need_wakeup_cmd, sizeof(uint8_t), &param);
    if (ret != sizeof(uint8_t)) {
        tiot_print_err("[TIoT]set host need wakeup cmd fail.\r\n");
        return ret;
    }
    return ERRCODE_TIOT_SUCC;
}

static int32_t gnss_send_change_buadrate(tiot_controller *ctrl, uint32_t new_buadrate)
{
    int32_t ret;
    const tiot_device_timings *timings = &ctrl->dev_info->timings;
    tiot_xmit_config uart_config = { 0 };

    (void)tiot_xfer_get_config(&ctrl->transfer, &uart_config);
    /* 配置用户波特率 */
    if (uart_config.uart_config.baudrate == new_buadrate) {
        return ERRCODE_TIOT_SUCC;
    }
    uart_config.uart_config.baudrate = new_buadrate;
    ret = gnss_change_uart_config(&ctrl->transfer, &uart_config);
    if (ret != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIoT]send change buadrate fail\n");
        return ret;
    }
    /* 切换波特率后等待波特率稳定，device或host? */
    tiot_board_udelay(timings->baud_change_wait_us);
    return ERRCODE_TIOT_SUCC;
}

static int32_t gnss_service_packet_init(tiot_controller *ctrl, uint8_t dev_id)
{
    int32_t ret;
#ifndef CONFIG_BOARD_DYNAMIC_ALLOC
    tiot_packet_buffer pkt_buff = { g_gnss_common_buff_mem[dev_id], CONFIG_GNSS_PACKET_BUFF_MAX, 0 };
#endif
    tiot_packet_context_param param = {
        .flags = TIOT_PKT_FLAGS_SUPPORT_ASCII,
        .rx_manager_info = { g_gnss_pkt_queues[dev_id], GNSS_RX_SUBSYS_MAX, gnss_packet_manager_match_func },
        .buff = NULL
    };
#ifndef CONFIG_BOARD_DYNAMIC_ALLOC
    param.buff = &pkt_buff;
#endif
    g_gnss_pkt_queues[dev_id][GNSS_RX_SUBSYS_0].queue_nodes = g_gnss_pkt_queue_nodes[dev_id];
    g_gnss_pkt_queues[dev_id][GNSS_RX_SUBSYS_0].queue_size = GNSS_PACKET_QUEUE_NODES;
    tiot_packet_context *pkt_ctx = &g_gnss_pkt_ctx[dev_id];
    ret = tiot_packet_init(pkt_ctx, &ctrl->transfer, &param);
    if (ret != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIOT]packet init fail.\r\n");
    }
    return ret;
}

static int32_t gnss_service_install_packet(tiot_controller *ctrl, uint8_t dev_id)
{
    int32_t ret;
    tiot_xfer_packet_ops gnss_packet_ops = {
        .read_out = tiot_packet_read_out,
        .rx_data_store = tiot_packet_rx_data_store,
        .tx_push = tiot_packet_tx_push,
    };
    const tiot_device_timings *timings = &ctrl->dev_info->timings;

    /* 收发模块注册包格式 */
    ret = tiot_xfer_install_packet(&ctrl->transfer, (tiot_xfer_packet_ops *)&gnss_packet_ops,
                                   (uintptr_t)&g_gnss_pkt_ctx[dev_id]);
    if (ret != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIOT]packet install fail.\r\n");
        return ret;
    }
    /* 必须等待device UART初始化后才能接收消息, 待确认时间. */
    osal_msleep(timings->init_time_ms);
    tiot_pm_set_event(&ctrl->pm, TIOT_PM_EVENT_REPORT_WORK);
    return ERRCODE_TIOT_SUCC;
}

static inline int32_t gnss_service_custom_msg_send(tiot_controller *ctrl, uint32_t wakein_pin,
                                                   uint32_t custom_buadrate)
{
    int32_t ret;
    /* 通过管脚有无判断host是否需要唤醒 */
    if (wakein_pin != TIOT_PIN_NONE) {
        ret = gnss_send_host_need_wakeup(ctrl);
        if (ret != ERRCODE_TIOT_SUCC) {
            return ret;
        }
    }
    /* 切换波特率 */
    return gnss_send_change_buadrate(ctrl, custom_buadrate);
}

static inline int32_t gnss_get_dev_id(tiot_controller *ctrl, uint8_t *dev_id)
{
    uint8_t i;
    for (i = 0; i < CONFIG_GNSS_DEV_NUM; i++) {
        if (ctrl == &g_gnss_controllers[i]) {
            *dev_id = i;
            return ERRCODE_TIOT_SUCC;
        }
    }
    tiot_print_err("[TIoT]invalid device\n");
    return ERRCODE_TIOT_DEV_INVALID;
}

static inline void gnss_set_open_param(tiot_xfer_open_param *xfer_open_param, tiot_service_open_param *param)
{
    if (param == NULL) {
        return;
    }
    xfer_open_param->rx_cbk_param.buff = param->buff;
    xfer_open_param->rx_cbk_param.buff_len = param->buff_len;
    xfer_open_param->rx_cbk_param.rx_callback = param->rx_callback;
}

static int32_t gnss_xfer_open(tiot_xfer_manager *xfer, tiot_xfer_open_param *param)
{
    int32_t ret;
    tiot_xmit_config initial_config = def_uart_cfg(GNSS_UART_DEFAULT_BAUDRATE,
                                                   GNSS_UART_ATTR_FLOW_CTRL);

    ret = tiot_xfer_open(xfer, param);
    if (ret != ERRCODE_TIOT_SUCC) {
        /* 通信接口打开失败 */
        tiot_print_err("[TIoT]Transfer port open fail\n");
        return ret;
    }
    /*
     * 解决不同host平台uart在未打开流控功能时rts管脚输出状态不确定(上电时需要保持低电平)，
     * 导致gnss芯片上电字错误(gnss的cts管脚被复用作上电配置字)。
     */
    ret = tiot_xfer_set_config(xfer, &initial_config);
    if (ret != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIoT]gnss set initial uart config fail.\n");
        tiot_xfer_close(xfer);
    }
    return ret;
}

static inline int32_t gnss_firmware_load(tiot_fw *firmware)
{
    int32_t ret;
#ifdef CONFIG_FILE_BY_ARRAY
    ret = tiot_firmware_load(firmware, gnss_fw_file_path_get(), TIOT_FW_FILE_ARRAY_CFG_NAME);
#else
    ret = tiot_firmware_load(firmware, NULL, gnss_board_get_info()->cfg_path);
#endif
    if (ret != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIoT]Gnss firmware load fail.\n");
    }
    return ret;
}

static int32_t gnss_service_open(tiot_controller *ctrl, void *param)
{
    int32_t ret;
    uint8_t dev_id;
    uint32_t wakein_pin;
    tiot_xfer_open_param xfer_open_param = { 0 };

    ret = gnss_get_dev_id(ctrl, &dev_id);
    if (ret != ERRCODE_TIOT_SUCC) {
        return ret;
    }
    gnss_set_open_param(&xfer_open_param, param);
    /* 传输启动 */
    ret = gnss_xfer_open(&ctrl->transfer, &xfer_open_param);
    if (ret != ERRCODE_TIOT_SUCC) {
        goto xfer_open_fail;
    }
    /* 上电 */
    ret = tiot_pm_set_event(&ctrl->pm, TIOT_PM_EVENT_POWERON);
    if (ret != ERRCODE_TIOT_SUCC) {
        /* 上电失败 */
        tiot_print_err("[TIoT]Power on fail\n");
        goto power_on_fail;
    }
    /* 加载固件 */
    ret = gnss_firmware_load(&ctrl->firmware);
    if (ret != ERRCODE_TIOT_SUCC) {
        goto fw_load_fail;
    }
    /* 使能中断 */
    wakein_pin = ctrl->pm.pm_info[TIOT_PIN_WAKE_IN];
    tiot_pin_int_request(wakein_pin, gnss_wakein_cb);

    ret = gnss_service_packet_init(ctrl, dev_id);
    if (ret != ERRCODE_TIOT_SUCC) {
        goto pkt_init_fail;
    }
    ret = gnss_service_install_packet(ctrl, dev_id);
    if (ret != ERRCODE_TIOT_SUCC) {
        goto pkt_install_fail;
    }
    ret = gnss_service_custom_msg_send(ctrl, wakein_pin, CONFIG_GNSS_UART_BAUDRATE);
    if (ret != ERRCODE_TIOT_SUCC) {
        goto custom_msg_send_fail;
    }
    /* 打开OK */
    tiot_print_suc("[TIoT]Controller open succ.\n");
    return ERRCODE_TIOT_SUCC;

custom_msg_send_fail:
    tiot_xfer_uninstall_packet(&ctrl->transfer);
pkt_install_fail:
    tiot_packet_deinit(&g_gnss_pkt_ctx[dev_id]);
pkt_init_fail:
    tiot_pin_int_release(wakein_pin);
fw_load_fail:
    (void)tiot_pm_set_event(&ctrl->pm, TIOT_PM_EVENT_POWEROFF);
power_on_fail:
    tiot_xfer_close(&ctrl->transfer);
xfer_open_fail:
    return ret;
}

static void gnss_service_close(tiot_controller *ctrl)
{
    uint8_t dev_id;
    uint32_t wakein_pin;

    if (gnss_get_dev_id(ctrl, &dev_id) != ERRCODE_TIOT_SUCC) {
        return;
    }
    wakein_pin = ctrl->pm.pm_info[TIOT_PIN_WAKE_IN];
    (void)tiot_pm_set_event(&ctrl->pm, TIOT_PM_EVENT_POWEROFF);
    tiot_pin_int_release(wakein_pin);
    tiot_xfer_close(&ctrl->transfer);
    tiot_xfer_uninstall_packet(&ctrl->transfer);
    tiot_packet_deinit(&g_gnss_pkt_ctx[dev_id]);
}

static int32_t gnss_service_write(tiot_controller *ctrl, const uint8_t *data,
                                  uint32_t len, const tiot_xfer_tx_param *param)
{
    int32_t ret;
    uint8_t dev_id;
    const tiot_xfer_tx_param tx_param = { SUBSYS_GNSS };
    tiot_pm *pm;
    tiot_unused(param);

    ret = gnss_get_dev_id(ctrl, &dev_id);
    if (ret != ERRCODE_TIOT_SUCC) {
        return ret;
    }
    pm = &ctrl->pm;
    (void)tiot_pm_set_event(pm, TIOT_PM_EVENT_WORK_VOTE_UP);
    /* 尝试唤醒device */
    ret = tiot_pm_set_event(pm, TIOT_PM_EVENT_WAKEUP);
    if (ret != ERRCODE_TIOT_SUCC) {
        tiot_print_err("[TIoT]wake up error\n");
        goto vote_down;
    }
    /* 等待成功唤醒device */
    ret = tiot_pm_wait_state(pm, TIOT_PM_STATE_WORK, ctrl->dev_info->timings.wakeup_wait_ms);
    if (ret <= 0) {
        tiot_print_warning("[TIoT]wakeup device fail, pm state is 0x%x.\n", pm->state);
    }
    ret = tiot_xfer_send(&ctrl->transfer, data, len, &tx_param);
vote_down:
    (void)tiot_pm_set_event(pm, TIOT_PM_EVENT_WORK_VOTE_DOWN);
    return ret;
}

static int32_t gnss_service_read(tiot_controller *ctrl, uint8_t *buff, uint32_t buff_len,
                                 const tiot_xfer_rx_param *param)
{
    int32_t ret;
    uint8_t dev_id;

    ret = gnss_get_dev_id(ctrl, &dev_id);
    if (ret != ERRCODE_TIOT_SUCC) {
        return ret;
    }
    return tiot_xfer_recv(&ctrl->transfer, buff, buff_len, param);
}

static void service_deinit(uint8_t ctrl_num, uint8_t board_num)
{
    uint8_t i;
    for (i = 0; i < ctrl_num; i++) {
        tiot_controller_deinit(&g_gnss_controllers[i]);
    }
    for (i = 0; i < board_num; i++) {
        gnss_board_deinit(&i);
    }
}

int32_t gnss_service_init(void)
{
    uint8_t i;
    int32_t ret;
    tiot_board_info per_board_info;
    tiot_controller_info ctrl_info;
    const gnss_board_info *board_info;

    for (i = 0; i < CONFIG_GNSS_DEV_NUM; i++) {
        if (gnss_board_init(&i) != ERRCODE_TIOT_SUCC) {
            tiot_print_err("[TIoT]board init fail\n");
            service_deinit(0, i);
            return ERRCODE_TIOT_DEV_BOARD_INIT_FAIL;
        }
    }
    board_info = gnss_board_get_info();
    if (board_info == NULL) {
        tiot_print_err("[TIoT]no board info.\n");
        service_deinit(0, CONFIG_GNSS_DEV_NUM);
        return ERRCODE_TIOT_DEV_NO_BOARD_INFO;
    }
    for (i = 0; i < CONFIG_GNSS_DEV_NUM; i++) {
        per_board_info.hw_info.xmit_id = board_info->hw_infos[i].xmit_id;
        per_board_info.hw_info.pm_info = board_info->hw_infos[i].pm_info;
        ctrl_info.board_info = &per_board_info;
        ctrl_info.dev_info = gnss_device_get_info();
        ctrl_info.dev_cus = 0;
        ctrl_info.is_host = true;
        ret = tiot_controller_init(&g_gnss_controllers[i], &ctrl_info);
        if (ret != ERRCODE_TIOT_SUCC) {
            tiot_print_err("[TIoT]controller init fail\n");
            service_deinit(i, CONFIG_GNSS_DEV_NUM);
            return ret;
        }
    }
    for (i = 0; i < CONFIG_GNSS_DEV_NUM; i++) {
        g_gnss_controllers[i].ops.open = gnss_service_open;
        g_gnss_controllers[i].ops.close = gnss_service_close;
        g_gnss_controllers[i].ops.write = gnss_service_write;
        g_gnss_controllers[i].ops.read = gnss_service_read;
        g_gnss_controllers[i].ops.pm_ctrl = NULL;
    }
    return ERRCODE_TIOT_SUCC;
}

void gnss_service_deinit(void)
{
    service_deinit(CONFIG_GNSS_DEV_NUM, CONFIG_GNSS_DEV_NUM);
}

uintptr_t gnss_service_get_ctrl(uint8_t dev_id)
{
    return (uintptr_t)&g_gnss_controllers[dev_id];
}
