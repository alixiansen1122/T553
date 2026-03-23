/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: wearable_onlinelog src fie.
 * Author: CompanyName
 * Create:
 */
#include "wearable_log.h"
#include "securec.h"
#include "diag_ind_src.h"
#include "diag_service.h"
#include "diag_common.h"
#include "msg_center_protocol.h"
#include "msg_center_device.h"
#if defined(SW_UART_DEBUG)
#include "uart.h"
#endif

#if (CONFIG_ONLINE_LOG == 1) && !defined(_WIN32)
#define TL_BYTE1 1
#define TL_BYTE2 2
#define SHIFT_8_BIT 8

#define SET_UART_CHANNEL_CMD 1
#define SET_BR_CHANNEL_CMD 2
#define SET_BLE_CHANNEL_CMD 3

static uint8_t g_log_diag_channel = DIAG_FRAME_FID_PC;

// 在线日志级别对应的字符串
static const char *gLevelStrings[] = {"[WEARABLE D/] ", "[WEARABLE I/] ", "[WEARABLE W/] ", "[WEARABLE E/] "};
// 在线日志过滤器;
static uint8_t gOnlineLevelFilter = 0xf;

errcode_t uapi_diag_service_send_log(uint8_t channel, uint8_t type, uint8_t *payload, uint32_t pay_len);

static uint8_t calculate_tl_byte(uint32_t payloadLen)
{
    uint8_t tl_byte = 0;
    if (payloadLen <= 0x7f) {
        tl_byte = 0;
    } else if (payloadLen > 0xff) {
        tl_byte = TL_BYTE2;
    } else {
        tl_byte = TL_BYTE1;
    }
    return tl_byte;
}

void set_online_log_level(uint8_t filter)
{
    gOnlineLevelFilter = filter;
}

static void send_uart_str(const char *s, uint32_t str_len)
{
#if defined(SW_UART_DEBUG)
    if (hal_uart_get_funcs(SW_DEBUG_UART_BUS) == NULL) {
        return;
    }
    uapi_uart_write(SW_DEBUG_UART_BUS, (const void *)s, str_len, 0);
#endif
}

void change_log_diag_channel(uint8_t channel)
{
    if (channel == SET_UART_CHANNEL_CMD) {
        g_log_diag_channel = DIAG_FRAME_FID_PC;
    } else if (channel == SET_BR_CHANNEL_CMD) {
        g_log_diag_channel = DIAG_FRAME_FID_PHONE;
    } else if (channel == SET_BLE_CHANNEL_CMD) {
        g_log_diag_channel = DIAG_FRAME_FID_BT;
    } else {
        g_log_diag_channel = DIAG_FRAME_FID_PC;
    }
}

uint8_t get_log_diag_channel()
{
    return g_log_diag_channel;
}

void online_print_str(uint16_t level, const char *str, ...)
{
    static uint8_t buffer[1024];
    uint8_t filter = (1 << level) & gOnlineLevelFilter;
    if (!filter) {
        return;
    }
    int offset = sprintf_s((char *)buffer, sizeof(buffer), "%s", gLevelStrings[level]);
    if (offset < 0 || (size_t)offset >= sizeof(buffer)) {
        return;
    }
    va_list args;
    va_start(args, str);
    offset += vsprintf_s((char *)(buffer + offset), sizeof(buffer) - offset, str, args);
    if (offset < 0 || (size_t)offset >= sizeof(buffer)) {
        va_end(args);
        return;
    }
    va_end(args);
    uint8_t channel = get_log_diag_channel();
    if (channel == DIAG_FRAME_FID_PC) {
        send_uart_str((const char *)buffer, offset);
    } else {
        uapi_diag_service_send_log(channel, MSGCENTER_TYPE_ID_ONLINELOG_REPORT, buffer, offset);
    }
}

errcode_t uapi_diag_service_send_log(uint8_t channel, uint8_t type, uint8_t *payload, uint32_t pay_len)
{
    uint8_t size;
    diag_ser_data_t *diag_data;
    diag_ser_frame_t *frame;
    uint32_t result = ERRCODE_FAIL;
    // 计算tl的长度及填充tl数据;
    uint8_t tl_byte = calculate_tl_byte(pay_len);
    uint32_t tl_len = sizeof(msg_center_pkt_tlv_t) + tl_byte;
    uint8_t tl_data[4];
    msg_center_pkt_tlv_t *tl = (msg_center_pkt_tlv_t *)tl_data;
    tl->type = 0x80 | type;
    if (tl_byte == TL_BYTE1) {
        tl->len = tl_byte;
        tl->data[0] = (uint8_t)pay_len;
    } else if (tl_byte == TL_BYTE2) {
        tl->len = tl_byte;
        tl->data[0] = (uint16_t)pay_len & 0xff;
        tl->data[1] = (uint16_t)pay_len >> SHIFT_8_BIT;
    } else {
        tl->len = 0x80 | (uint8_t)pay_len;
    }

    size = sizeof(diag_ser_data_t) + sizeof(diag_ser_frame_t) + pay_len + tl_len;
    diag_data = (diag_ser_data_t *)malloc(size);
    if (diag_data == NULL) {
        return ERRCODE_FAIL;
    }

    diag_data->header.ser_id = DIAG_SER_MSG_CENTER;
    diag_data->header.cmd_id = MSGCENTER_CMD_DEVICE;
    diag_data->header.ack_en = false;
    diag_data->header.src = DIAG_FRAME_FID_MCU;
    diag_data->header.dst = channel;
    diag_data->header.crc_en = 1;
    diag_data->header.length = size - sizeof(diag_ser_data_t);  // size = modeleId + cmdId + tlv

    frame = (diag_ser_frame_t *)&(diag_data->payload);
    frame->module_id = DIAG_SER_MSG_CENTER;
    frame->cmd_id = MSGCENTER_CMD_DEVICE;
    (void)memcpy_s(&(frame->tlv), tl_len, tl, tl_len);
    (void)memcpy_s(&(frame->tlv[tl_len]), pay_len, payload, pay_len);
    result = uapi_diag_service_send_data(diag_data);
    free(diag_data);
    return result;
}
#endif
