/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: Wearable onlinelog header file.
 * Author:
 * Create:
 */

#ifndef WEARABLE_ONLINELOG_H
#define WEARABLE_ONLINELOG_H

#include "stdint.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 特性： 在线日志 */
#ifndef CONFIG_ONLINE_LOG
#define CONFIG_ONLINE_LOG 1
#endif

enum OnLineLogLevel {
    ONLINE_DEBUG = 0,
    ONLINE_INFO,
    ONLINE_WARN,
    ONLINE_ERROR
};

void online_print_str(uint16_t level, const char *str, ...);
void change_log_diag_channel(uint8_t channel);
uint8_t get_log_diag_channel(void);
void set_online_log_level(uint8_t filter);
errcode_t uapi_diag_service_send_log(uint8_t channel, uint8_t type, uint8_t *payload, uint32_t pay_len);

#ifndef ONLINE_PRINT
#define ONLINE_PRINT(level, fmt, arg...) online_print_str(level, fmt, ##arg)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // WEARABLE_ONLINELOG_H