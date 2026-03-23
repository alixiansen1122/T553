/*
 * Copyright (c) 2022 CompanyNameMagicTag.
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

#ifndef LITE_SAMGR_DEBUG_H
#define LITE_SAMGR_DEBUG_H

#include "ohos_types.h"
#include "stdio.h"
#if defined(CONFIG_DSP_CORE)
#include "audio_debug.h"
#endif
#ifdef HSO_SUPPORT
#include "soc_diag_util.h"
#include "log_module_id.h"
#include "log_def_ohos.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if defined(CONFIG_DSP_CORE)
#define SAMGR_LOG_INFO audio_log_info
#define SAMGR_LOG_ERROR audio_log_err
#define SAMGR_LOG_WARN audio_log_warn
#define SAMGR_LOG_FATAL audio_log_fatal

#else

#if defined(CONFIG_CGRA_CORE)
#include "cgra_platform.h"
#define printf cgra_printf
#endif

#ifndef HSO_SUPPORT
#define SAMGR_LOG_FATAL(fmt, args...)                  \
    do                                             \
    {                                              \
        (printf("[SAMGR][FATAL] "), printf(fmt, ##args)); \
    } while (0)

#define SAMGR_LOG_ERROR(fmt, args...)                  \
    do                                             \
    {                                              \
        (printf("[SAMGR][ERROR] "), printf(fmt, ##args)); \
    } while (0)

#define SAMGR_LOG_WARN(fmt, args...)                  \
    do                                             \
    {                                              \
        (printf("[SAMGR][WARN] "), printf(fmt, ##args)); \
    } while (0)

#define SAMGR_LOG_INFO(fmt, args...)                    \
    do                                             \
    {                                              \
        (printf("[SAMGR][INFO] "), printf(fmt, ##args)); \
    } while (0)
#else
#define SAMGR_LOG_ERROR(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#define SAMGR_LOG_FATAL(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#define SAMGR_LOG_INFO(fmt, args...) uapi_diag_info_log(0, fmt, ##args)
#define SAMGR_LOG_WARN(fmt, args...) uapi_diag_warning_log(0, fmt, ##args)
#endif
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // LITE_COMMON_H
/** @} */
