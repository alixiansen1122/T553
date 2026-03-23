/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_MEDIA_LOG_H
#define OHOS_MEDIA_LOG_H

#include <stdio.h>
#include <stdint.h>

#ifdef ONLY_SUPPORT_HSO
#undef SW_UART_DEBUG
#undef SW_RTT_DEBUG
#endif

#if !defined(SW_UART_DEBUG) && !defined(SW_RTT_DEBUG)
#include "soc_diag_util.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#undef LOG_DOMAIN
#undef LOG_TAG
#define LOG_DOMAIN 0xD002B00
#define LOG_TAG "MultiMedia"

#define MEDIA_HAL_LOG_ENABLE  1
#define MEDIA_UNUSED(x) ((void)(x))
/*
 * @brief  log level type.
 */
typedef enum {
    MEDIA_LOG_DEBUG = 0,
    MEDIA_LOG_INFO,
    MEDIA_LOG_WARN,
    MEDIA_LOG_ERR,
    MEDIA_LOG_FATAL,
} MEDIA_LOG_LEVEL;

int32_t MediaLogPrintf(MEDIA_LOG_LEVEL level, const char *fmt, ...);

int32_t SetMediaLogEnabledLevel(MEDIA_LOG_LEVEL level);

#define DECORATOR_LOG(op, level, fmt, args...)                         \
    do {                                                               \
        op(level, "{%s():%d} " fmt, __FUNCTION__, __LINE__, ##args);   \
    } while (0)

#if defined(SW_UART_DEBUG) || defined(SW_RTT_DEBUG)
#define MEDIA_DEBUG_LOG(fmt, ...) DECORATOR_LOG(MediaLogPrintf, MEDIA_LOG_DEBUG, fmt, ##__VA_ARGS__)
#define MEDIA_ERR_LOG(fmt, ...) DECORATOR_LOG(MediaLogPrintf, MEDIA_LOG_ERR, fmt, ##__VA_ARGS__)
#define MEDIA_WARNING_LOG(fmt, ...) DECORATOR_LOG(MediaLogPrintf, MEDIA_LOG_WARN, fmt, ##__VA_ARGS__)
#define MEDIA_INFO_LOG(fmt, ...) DECORATOR_LOG(MediaLogPrintf, MEDIA_LOG_INFO, fmt, ##__VA_ARGS__)
#define MEDIA_FATAL_LOG(fmt, ...) DECORATOR_LOG(MediaLogPrintf, MEDIA_LOG_FATAL, fmt, ##__VA_ARGS__)
#else
#define MEDIA_DEBUG_LOG(fmt, ...) uapi_diag_debug_log(0, fmt, ##__VA_ARGS__)
#define MEDIA_ERR_LOG(fmt, ...) uapi_diag_error_log(0, fmt, ##__VA_ARGS__)
#define MEDIA_WARNING_LOG(fmt, ...) uapi_diag_warning_log(0, fmt, ##__VA_ARGS__)
#define MEDIA_INFO_LOG(fmt, ...) uapi_diag_info_log(0, fmt, ##__VA_ARGS__)
#define MEDIA_FATAL_LOG(fmt, ...) uapi_diag_error_log(0, fmt, ##__VA_ARGS__)
#endif

#define MEDIA_OK 0
#define MEDIA_INVALID_PARAM (-1)
#define MEDIA_INIT_FAIL (-2)
#define MEDIA_ERR (-3)
#define MEDIA_PERMISSION_DENIED (-4)
#define MEDIA_READ_EOS (-5)

#ifdef __cplusplus
}
#endif

#endif // OHOS_MEDIA_LOG_H
