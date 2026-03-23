/*
 * Copyright (c) CompanyNameMagicTag 2020-2021. All rights reserved.
 * Description: source file for module operation
 */

#ifndef _VENDOR_COMMON_H_
#define _VENDOR_COMMON_H_

#include <unistd.h>
#include <stdio.h>
#include "tcxo.h"
#include "td_type.h"

#define VENDOR_LOG_TRACE 0 /* 0: trace log off; 1: trace log on */

#define I2C_BAUDRATE 400000 /* 标准模式上限：100KHz，快速模式上限：400KHz，高速模式上限：3.4MKHz */
#define I2C_HIGH_SPEED_CODE 1 /* 高速模式主机码，每个主机唯一，范围0~7，仅高速模式生效 */

#define SAP_CORE "A"

#define vendor_printf(fmt...)                                    \
    do {                                                         \
        printf("[%s][%s:%d]", SAP_CORE, __FUNCTION__, __LINE__); \
        printf(fmt);                                             \
    } while (0)

#if (VENDOR_LOG_TRACE == 1)
#define vendor_printf_trace vendor_printf
#else
#define vendor_printf_trace
#endif

#define vendor_func_enter() vendor_printf_trace("enter...\n")
#define vendor_func_exit()  vendor_printf_trace("exit...\n")

/* function trace log, strictly prohibited to expand */
#define vendor_log_err_code(err_code) vendor_printf("Error Code: [0x%08X]\n", err_code)
/* function trace log, print the called function name when function is error */
#define vendor_log_fun_err(func, err_code) vendor_printf("Call %s Failed, Error Code: [0x%08X]\n", #func, err_code)
/* Function trace log, print the pointer name when pointer is null */
#define vendor_log_null_pointer(val) vendor_printf("%s = %p,  Null Pointer!\n", #val, val)

#define vendor_err_log_u32(val) vendor_printf("%s = %u\n", #val, val)
#define vendor_err_log_h32(val) vendor_printf("%s = 0x%08X\n", #val, val)

#define vendor_trace_log_u32(val) vendor_printf_trace("%s = %u\n", #val, val)
#define vendor_trace_log_h32(val) vendor_printf_trace("%s = 0x%08X\n", #val, val)

#define EXT_SUCCESS 0
#define EXT_FAILURE (-1)

#define vendor_unused(var) (td_void)(var)

#define vendor_msleep(ms) usleep((ms) * 1000) /* 1000 : 1ms=1000us */
#define vendor_udelay uapi_tcxo_delay_us
#define vendor_mdelay uapi_tcxo_delay_ms

#endif
