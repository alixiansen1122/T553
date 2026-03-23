/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: gnss log
 * Author:
 * Create:
 */
#ifndef GNSS_LOG_H
#define GNSS_LOG_H

#ifdef BRANDY_PRODUCT_EVB4 // 3321diting
#include "app_at_process.h"

#define GNSS_LOG_DEBUG              printf
#define GNSS_LOG_INFO(s, ...)       factory_test_print(FT_RETURN_SUCC, s, ##__VA_ARGS__)
#define GNSS_LOG_WARNING            printf
#ifdef  GNSS_DEBUG_ENABLE
#define GNSS_LOG_ERROR(s, ...)      factory_test_print(FT_RETURN_SUCC, s, ##__VA_ARGS__)
#else
#define GNSS_LOG_ERROR              printf
#endif
#else  // 3322 diting
#include "debug_print.h"

#define GNSS_LOG_DEBUG              printf
#define GNSS_LOG_INFO               printf
#define GNSS_LOG_WARNING            printf
#ifdef  GNSS_DEBUG_ENABLE
#define GNSS_LOG_ERROR              printf
#else
#define GNSS_LOG_ERROR              printf
#endif
#endif

#endif
