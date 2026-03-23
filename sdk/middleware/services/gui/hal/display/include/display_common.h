/*
 * Copyright (c) CompanyNameMagicTag
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

#ifndef DISPLAY_COMMON_H
#define DISPLAY_COMMON_H
#ifdef USE_DIAG_LOG
#include "soc_diag_util.h"
#include "log_oam_logger.h"
#include "log_def.h"
#endif

#define DISP_BYTE_ALIGNMENT 16
#define DISP_STRIDE_DIVIDER 8
#ifdef USE_DIAG_LOG
#define DISP_LOGE(fmt, arg...) uapi_diag_error_log(0, fmt, ##arg)
#define DISP_LOGW(fmt, arg...) uapi_diag_warning_log(0, fmt, ##arg)
#define DISP_LOGI(fmt, arg...) uapi_diag_info_log(0, fmt, ##arg)
#define DISP_LOGD(fmt, arg...) uapi_diag_debug_log(0, fmt, ##arg)
#else
#define DISP_LOGE(fmt, arg...) printf("[DISPLAY E/] %s: " fmt, __FUNCTION__, ##arg)
#define DISP_LOGW(fmt, arg...) printf("[DISPLAY W/] %s: " fmt, __FUNCTION__, ##arg)
#define DISP_LOGI(fmt, arg...) printf("[DISPLAY I/] %s: " fmt, __FUNCTION__, ##arg)
#define DISP_LOGD(fmt, arg...) printf("[DISPLAY D/] %s: " fmt, __FUNCTION__, ##arg)
#endif
#define CHECK_NULLPOINTER_RETURN_VALUE(pointer, ret) do { \
    if ((pointer) == NULL) { \
        DISP_LOGE("pointer is null and return ret\n"); \
        return (ret); \
    } \
} while (0)

#define CHECK_NULLPOINTER_RETURN(pointer) do { \
    if ((pointer) == NULL) { \
        DISP_LOGE("pointer is null and return\n"); \
        return; \
    } \
} while (0)

#endif /* DISPLAY_COMMON_H */
