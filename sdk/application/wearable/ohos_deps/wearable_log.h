/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: Wearable log header file.
 * Author:
 * Create:
 */

#ifndef WEARABLE_LOG_H
#define WEARABLE_LOG_H

#if !defined(SW_UART_DEBUG) && !defined(SW_RTT_DEBUG)
#include "soc_diag_util.h"
#include "log_oam_logger.h"
#include "log_def.h"
#else
#include "debug_print.h"
#endif
#include "wearable_onlinelog.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief Enumerates logging module types.
 *
 * The module type must be globally unique. A maximum of 64 module types can be defined.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    /** applications */
    WEARABLE_LOG_MODULE_APP = 0,
    /** Launcher Framework */
    WEARABLE_LOG_MODULE_LAUNCHER_FWK,
    /** Main Service */
    WEARABLE_LOG_MODULE_MAIN_SERVICE,
    /** Broadcast Service */
    WEARABLE_LOG_MODULE_BROADCASE_SERVICE,
    /** Atomic Ability Framework */
    WEARABLE_LOG_MODULE_AAFWK,
    /** smart voice */
    WEARABLE_LOG_MODULE_SMART_VOICE,
    /* UI SERVICE */
    WEARABLE_LOG_MODULE_UI_SERVICE,
    /* msg_center */
    WEARABLE_LOG_MODULE_MSG_CENTER,

    WEARABLE_LOG_MODULE_MAX
} WearableLogModuleType;

#if !defined(SW_UART_DEBUG) && !defined(SW_RTT_DEBUG) && !defined(CONFIG_ONLINE_LOG)
#define WEARABLE_LOGE(mod, fmt, arg...) uapi_diag_error_log(0, fmt, ##arg)
#define WEARABLE_LOGW(mod, fmt, arg...) uapi_diag_warning_log(0, fmt, ##arg)
#define WEARABLE_LOGI(mod, fmt, arg...) uapi_diag_info_log(0, fmt, ##arg)
#ifdef WEARABLE_LOG_DEBUG
#define WEARABLE_LOGD(mod, fmt, arg...) uapi_diag_debug_log(0, fmt, ##arg)
#else
#define WEARABLE_LOGD(mod, fmt, arg...)
#endif
#elif (CONFIG_ONLINE_LOG == 1) && !defined(_WIN32)
#define WEARABLE_LOGE(mod, fmt, arg...) ONLINE_PRINT(ONLINE_ERROR, fmt "\n", ##arg)
#define WEARABLE_LOGW(mod, fmt, arg...) ONLINE_PRINT(ONLINE_WARN, fmt "\n", ##arg)
#define WEARABLE_LOGI(mod, fmt, arg...) ONLINE_PRINT(ONLINE_INFO, fmt "\n", ##arg)
#ifdef WEARABLE_LOG_DEBUG
#define WEARABLE_LOGD(mod, fmt, arg...) ONLINE_PRINT(ONLINE_DEBUG, fmt "\n", ##arg)
#else
#define WEARABLE_LOGD(mod, fmt, arg...)
#endif
#else
#define WEARABLE_LOGE(mod, fmt, arg...) PRINT("[WEARABLE E/] " fmt "\n", ##arg)
#define WEARABLE_LOGW(mod, fmt, arg...) PRINT("[WEARABLE W/] " fmt "\n", ##arg)
#define WEARABLE_LOGI(mod, fmt, arg...) PRINT("[WEARABLE I/] " fmt "\n", ##arg)
#ifdef WEARABLE_LOG_DEBUG
#define WEARABLE_LOGD(mod, fmt, arg...) PRINT("[WEARABLE D/] " fmt "\n", ##arg)
#else
#define WEARABLE_LOGD(mod, fmt, arg...)
#endif
#endif

#define CHECK_NULL_AND_RETURN(pointer, mod, fmt) \
    do {                                         \
        if ((pointer) == nullptr) {              \
            WEARABLE_LOGE((mod), (fmt));         \
            return;                              \
        }                                        \
    } while (0)

#define CHECK_NULL_AND_RETURN_NULL(pointer, mod, fmt) \
    do {                                              \
        if ((pointer) == nullptr) {                   \
            WEARABLE_LOGE((mod), (fmt));              \
            return nullptr;                           \
        }                                             \
    } while (0)

#define CHECK_NULL_AND_RETURN_CODE(pointer, mod, fmt, code) \
    do {                                                    \
        if ((pointer) == nullptr) {                         \
            WEARABLE_LOGE((mod), (fmt));                    \
            return (code);                                  \
        }                                                   \
    } while (0)

#define DELETE_IF_NOT_NULL(pointer) \
    do {                            \
        if ((pointer) != nullptr) { \
            delete (pointer);       \
            (pointer) = nullptr;    \
        }                           \
    } while (0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
