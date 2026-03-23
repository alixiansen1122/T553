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

#ifndef LOCATION_LOG_H
#define LOCATION_LOG_H

#include <stdio.h>
#include <stdint.h>

#ifdef ONLY_SUPPORT_HSO
#undef SW_UART_DEBUG
#undef SW_RTT_DEBUG
#endif

#ifndef LOCATION_LOG_DOMAIN
#define LOCATION_LOG_DOMAIN 0xD002300
#endif

/*
 * @brief  log level type.
 */
typedef enum {
    LOCATION_LOG_DEBUG = 0,
    LOCATION_LOG_INFO,
    LOCATION_LOG_WARN,
    LOCATION_LOG_ERR,
    LOCATION_LOG_FATAL,
} LOCATION_LOG_LEVEL;

#define LOCATOR "Locator"
#define LOCATOR_STANDARD "Locator_standard"
#define LOCATOR_GNSS "GnssAbility"
#define REQUEST_MANAGER "RequestManager"
#define REPORT_MANAGER "ReportManager"

#define HDI_GNSS "hdi_gnss"
#define HDI_AGNSS "hdi_agnss"

#ifndef LOCATION_FUNC_FMT
#define LOCATION_FUNC_FMT "[%s:%d]"
#endif

#ifndef LOCATION_FILE_NAME
#define LOCATION_FILE_NAME (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#ifndef LOCATION_FUNC_INFO
#define LOCATION_FUNC_INFO __FUNCTION__, __LINE__
#endif

int32_t LocationLogPrintf(LOCATION_LOG_LEVEL level, const char *fmt, ...);

int32_t SetLocationLogEnabledLevel(LOCATION_LOG_LEVEL level);

#define DECORATOR_LOG(op, level, fmt, args...)                         \
    do {                                                               \
        op(level, LOCATION_FUNC_FMT fmt, LOCATION_FUNC_INFO, ##args);   \
    } while (0)

#define LBSLOGD(label, fmt, ...) DECORATOR_LOG(LocationLogPrintf, LOCATION_LOG_DEBUG, fmt, ##__VA_ARGS__)
#define LBSLOGE(label, fmt, ...) DECORATOR_LOG(LocationLogPrintf, LOCATION_LOG_ERR, fmt, ##__VA_ARGS__)
#define LBSLOGW(label, fmt, ...) DECORATOR_LOG(LocationLogPrintf, LOCATION_LOG_WARN, fmt, ##__VA_ARGS__)
#define LBSLOGI(label, fmt, ...) DECORATOR_LOG(LocationLogPrintf, LOCATION_LOG_INFO, fmt, ##__VA_ARGS__)
#define LBSLOGF(label, fmt, ...) DECORATOR_LOG(LocationLogPrintf, LOCATION_LOG_FATAL, fmt, ##__VA_ARGS__)

#endif // LOCATION_LOG_H
