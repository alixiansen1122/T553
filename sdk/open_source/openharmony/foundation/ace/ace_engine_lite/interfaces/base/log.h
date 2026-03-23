/*
 * Copyright (c) 2022 CompanyNameMagicTag Technologies Co., Ltd.
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

#ifndef OHOS_AAFWK_BASE_LOG_H
#define OHOS_AAFWK_BASE_LOG_H

#include <stdio.h>
#ifdef HSO_SUPPORT
#include "soc_diag_util.h"
#include "log_module_id.h"
#include "log_def_ohos.h"
#endif
// turn off debug level logging by default
// #define HILOG_DEBUG_ENABLE 1

/**
 * @brief Enumerates logging module types.
 *
 * The module type must be globally unique. A maximum of 64 module types can be defined.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    /** DFX */
    HILOG_MODULE_HIVIEW = 0,
    /** System Ability Manager */
    HILOG_MODULE_SAMGR,
    /** Update */
    HILOG_MODULE_UPDATE,
    /** Ability Cross-platform Environment */
    HILOG_MODULE_ACE,
    /** Third-party applications */
    HILOG_MODULE_APP,
    /** Launcher Framework */
    HILOG_MODULE_LAUNCHER_FWK,
    /** Main Service */
    HILOG_MODULE_MAIN_SERVICE,
    /** Atomic Ability Framework */
    HILOG_MODULE_AAFWK,
    /** Graphic */
    HILOG_MODULE_GRAPHIC,
    /** Multimedia */
    HILOG_MODULE_MEDIA,
    /** Distributed Schedule Service */
    HILOG_MODULE_DMS,
    /** Sensors */
    HILOG_MODULE_SEN,
    /** Security */
    HILOG_MODULE_SCY,
    /** XTS */
    HILOG_MODULE_XTS,
    /** SoftBus */
    HILOG_MODULE_SOFTBUS,
    /** Maximum number of modules */
    HILOG_MODULE_MAX
} HiLogModuleType;

// #ifndef HSO_SUPPORT
// #if HILOG_DEBUG_ENABLE
#define HILOG_DEBUG(mod, format, ...)                                \
    do                                                               \
    {                                                                \
        printf("D " format "\n", ##__VA_ARGS__);                     \
    } while (0)
#else
#define HILOG_DEBUG(mod, format, ...)
#endif

#define HILOG_ERROR(mod, format, ...)                                \
    do                                                               \
    {                                                                \
        printf("E " format "\n", ##__VA_ARGS__);                     \
    } while (0)

#define HILOG_FATAL(mod, format, ...)                                \
    do                                                               \
    {                                                                \
        printf("F " format "\n", ##__VA_ARGS__);                     \
    } while (0)

#define HILOG_INFO(mod, format, ...)                                  \
    do                                                                \
    {                                                                 \
        printf("I " format "\n", ##__VA_ARGS__);                      \
    } while (0)

#define HILOG_WARN(mod, format, ...)                                  \
    do                                                                \
    {                                                                 \
        printf("W " format "\n", ##__VA_ARGS__);                      \
    } while (0)

#define HILOG_RACE(mod, format, ...)                                  \
    do                                                                \
    {                                                                 \
        printf("R " format "\n", ##__VA_ARGS__);                      \
    } while (0)

// #else
// #define HILOG_DEBUG(mod, format, ...) uapi_diag_debug_log(0, format, ##__VA_ARGS__)
// #define HILOG_ERROR(mod, format, ...) uapi_diag_error_log(0, format, ##__VA_ARGS__)
// #define HILOG_FATAL(mod, format, ...) uapi_diag_error_log(0, format, ##__VA_ARGS__)
// #define HILOG_INFO(mod, format, ...) uapi_diag_info_log(0, format, ##__VA_ARGS__)
// #define HILOG_WARN(mod, format, ...) uapi_diag_warning_log(0, format, ##__VA_ARGS__)
// #define HILOG_RACE(mod, format, ...)
// #endif
// #endif // OHOS_AAFWK_BASE_LOG_H
