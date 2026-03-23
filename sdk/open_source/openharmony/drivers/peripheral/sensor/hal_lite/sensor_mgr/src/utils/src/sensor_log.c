/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor log
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#include "sensor_log.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "sensor_type.h"
#ifdef SUPPORT_AT_UART_PRINT
#include "app_at_uart.h"
#endif

#ifdef ENABLE_INFO_LEVEL_LOG
static SENSOR_LOG_LEVEL g_enabledLevel = SENSOR_LOG_DEBUG;
#else
static SENSOR_LOG_LEVEL g_enabledLevel = SENSOR_LOG_ERR;
#endif

#define MAX_BUFFER_SZ 256

static void LogTime(void)
{
    struct timeval tv = {0};
    gettimeofday(&tv, NULL);
    struct tm tm = {};
    localtime_r(&tv.tv_sec, &tm);
    const int64_t usecToMsec = 1000;
    printf("[%02d:%02d:%02d:%03lld]", tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec / usecToMsec);
}

int32_t SensorLogPrintf(SENSOR_LOG_LEVEL level, const char *fmt, ...)
{
    if (level < g_enabledLevel) {
        return SENSOR_SUCCESS;
    }
    if (fmt == NULL) {
        return SENSOR_INVALID_PARAM;
    }
    const char *levelStr = NULL;
    char out[MAX_BUFFER_SZ] = {0};
    switch (level) {
        case SENSOR_LOG_DEBUG:
            levelStr = "D";
            break;
        case SENSOR_LOG_INFO:
            levelStr = "I";
            break;
        case SENSOR_LOG_WARN:
            levelStr = "W";
            break;
        case SENSOR_LOG_ERR:
            levelStr = "E";
            break;
        default:
            levelStr = "E";
            break;
    }
    LogTime();
    va_list args;
#ifdef SUPPORT_AT_UART_PRINT
    at_uart_printf("[%s]", levelStr);
#else
    printf("[%s]", levelStr);
#endif
    va_start(args, fmt);
    int result = vsnprintf_s(out, MAX_BUFFER_SZ, MAX_BUFFER_SZ - 1, fmt, args);
    if (result == -1) {
        printf("vsnprintf_s failed!\n");
        va_end(args);
        return SENSOR_INVALID_PARAM;
    }
    va_end(args);
#ifdef SUPPORT_AT_UART_PRINT
    at_uart_printf("%s \n", out);
#else
    printf("%s \n", out);
#endif
    return SENSOR_SUCCESS;
}

int32_t SetSensorLogEnabledLevel(SENSOR_LOG_LEVEL level)
{
    if (level < SENSOR_LOG_DEBUG ||
        level > SENSOR_LOG_ERR) {
        return SENSOR_INVALID_PARAM;
    }
    g_enabledLevel = level;
    return SENSOR_SUCCESS;
}