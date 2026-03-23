/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SENSOR_LOG_H
#define SENSOR_LOG_H

#include <stdio.h>
#include <stdint.h>

#ifdef ONLY_SUPPORT_HSO
#undef SW_UART_DEBUG
#undef SW_RTT_DEBUG
#endif

#ifndef SENSOR_LOG_DOMAIN
#define SENSOR_LOG_DOMAIN 0xD002400
#endif

typedef enum {
    /** DFX */
    HILOG_MODULE_HIVIEW = 0,
    /** Third-party applications */
    HILOG_MODULE_APP,
    /** Sensors */
    HILOG_MODULE_SEN,
    HILOG_MODULE_MAX
} HiLogModuleType;

/*
 * @brief  log level type.
 */
typedef enum {
    SENSOR_LOG_DEBUG = 0,
    SENSOR_LOG_INFO,
    SENSOR_LOG_WARN,
    SENSOR_LOG_ERR,
    SENSOR_LOG_FATAL,
} SENSOR_LOG_LEVEL;

#ifndef SENSOR_FUNC_FMT
#define SENSOR_FUNC_FMT "[%s:%d]"
#endif

#ifndef SENSOR_FILE_NAME
#define SENSOR_FILE_NAME (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#ifndef SENSOR_FUNC_INFO
#define SENSOR_FUNC_INFO __FUNCTION__, __LINE__
#endif

int32_t SensorLogPrintf(SENSOR_LOG_LEVEL level, const char *fmt, ...);

int32_t SetSensorLogEnabledLevel(SENSOR_LOG_LEVEL level);

#define DECORATOR_LOG(op, level, fmt, args...)                         \
    do {                                                               \
        op(level, SENSOR_FUNC_FMT fmt, SENSOR_FUNC_INFO, ##args);   \
    } while (0)

#define HILOG_DEBUG(mod, format, ...) DECORATOR_LOG(SensorLogPrintf, SENSOR_LOG_DEBUG, format, ##__VA_ARGS__)
#define HILOG_WARN(mod, format, ...) DECORATOR_LOG(SensorLogPrintf, SENSOR_LOG_WARN, format, ##__VA_ARGS__)
#define HILOG_INFO(mod, format, ...) DECORATOR_LOG(SensorLogPrintf, SENSOR_LOG_INFO, format, ##__VA_ARGS__)
#define HILOG_ERROR(mod, format, ...) DECORATOR_LOG(SensorLogPrintf, SENSOR_LOG_ERR, format, ##__VA_ARGS__)
#define HILOG_FATAL(mod, format, ...) DECORATOR_LOG(SensorLogPrintf, SENSOR_LOG_FATAL, format, ##__VA_ARGS__)

#endif // LOCATION_LOG_H
