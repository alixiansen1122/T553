/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: Audio utils interface
* Author: Media Software Group
* Create: 2021-02-28
*/

#ifndef AUDIO_UTILS_H
#define AUDIO_UTILS_H

#include <stdint.h>
#include "media_thread_adapt.h"

#ifdef ONLY_SUPPORT_HSO
#undef SW_UART_DEBUG
#undef SW_RTT_DEBUG
#endif

#if !defined(SW_UART_DEBUG) && !defined(SW_RTT_DEBUG)
#include "soc_diag_util.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef void *Handle;
typedef void *VoidPtr;

#define AUDIO_LOG_ENABLE  1

#define AUDIO_UNUSED(x) ((void)(x))

/*
 * @brief  log level type.
 */
typedef enum {
    AUDIO_LOG_LEVEL_VERBOSE = 0,
    AUDIO_LOG_LEVEL_DEBUG,
    AUDIO_LOG_LEVEL_TRACE,
    AUDIO_LOG_LEVEL_INFO,
    AUDIO_LOG_LEVEL_WARN,
    AUDIO_LOG_LEVEL_ERR,
    AUDIO_LOG_LEVEL_FATAL,
    AUDIO_LOG_LEVEL_BUTT
} AUDIO_LOG_LEVEL;

int32_t AudioLogPrintf(const char *modName, AUDIO_LOG_LEVEL level, const char *fmt, ...);

int32_t SetAudioLogEnabledLevel(AUDIO_LOG_LEVEL level);

#define DECORATOR_LOG(op, modName, level, fmt, args...)                         \
    do {                                                                        \
        op(modName, level, "{%s():%d} " fmt, __FUNCTION__, __LINE__, ##args);   \
    } while (0)

#undef ALOGV
#undef ALOGD
#undef ALOGI
#undef ALOGW
#undef ALOGE

#ifdef AUDIO_LOG_ENABLE
#if defined(SW_UART_DEBUG) || defined(SW_RTT_DEBUG)
#define ALOGV(fmt, ...) DECORATOR_LOG(AudioLogPrintf, LOG_MODULE_NAME, AUDIO_LOG_LEVEL_VERBOSE, \
                                                        fmt, ##__VA_ARGS__)

#define ALOGD(fmt, ...) DECORATOR_LOG(AudioLogPrintf, LOG_MODULE_NAME, AUDIO_LOG_LEVEL_DEBUG,  \
                                                        fmt, ##__VA_ARGS__)

#define ALOGI(fmt, ...) DECORATOR_LOG(AudioLogPrintf, LOG_MODULE_NAME, AUDIO_LOG_LEVEL_INFO,   \
                                                        fmt, ##__VA_ARGS__)

#define ALOGW(fmt, ...) DECORATOR_LOG(AudioLogPrintf, LOG_MODULE_NAME, AUDIO_LOG_LEVEL_WARN,   \
                                                        fmt, ##__VA_ARGS__)

#define ALOGE(fmt, ...) DECORATOR_LOG(AudioLogPrintf, LOG_MODULE_NAME, AUDIO_LOG_LEVEL_ERR, \
                                                        fmt, ##__VA_ARGS__)
#else
#define ALOGV(fmt, ...) uapi_diag_debug_log(0, fmt, ##__VA_ARGS__)
#define ALOGD(fmt, ...) uapi_diag_debug_log(0, fmt, ##__VA_ARGS__)
#define ALOGI(fmt, ...) uapi_diag_info_log(0, fmt, ##__VA_ARGS__)
#define ALOGW(fmt, ...) uapi_diag_warning_log(0, fmt, ##__VA_ARGS__)
#define ALOGE(fmt, ...) uapi_diag_error_log(0, fmt, ##__VA_ARGS__)
#endif
#else
#define ALOGV(...)

#define ALOGD(...)

#define ALOGI(...)

#define ALOGW(...)

#define ALOGE(...)
#endif // AUDIO_LOG_LEVEL

#define CHK_NULL_RETURN_NO_LOG(ptr, ret) \
do { \
    if ((ptr) == NULL) { \
        return (ret); \
    } \
} while (0)

#define CHK_FAILED_RETURN_NO_LOG(value, target, ret) \
do { \
    if ((value) != (target)) { \
        return (ret); \
    } \
} while (0)

#if defined(SW_UART_DEBUG) || defined(SW_RTT_DEBUG)
#define CHK_NULL_RETURN(ptr, ret, printfString) \
do { \
    if ((ptr) == NULL) { \
        ALOGE("%s :ret:%d", ((printfString) == NULL) ? "" : (printfString), ret); \
        return (ret); \
    } \
} while (0)

#define CHK_NULL_RETURN_NONE(ptr, printfString) \
do { \
    if ((ptr) == NULL) { \
        ALOGE("%s ", ((printfString) == NULL) ? "" : (printfString)); \
        return; \
    } \
} while (0)

#define CHK_FAILED_RETURN(value, target, ret, print) \
do { \
    if ((value) != (target)) { \
        ALOGE("%s, %#x != %d ", ((print) == NULL) ? "" : (print), (value), (target)); \
        return (ret); \
    } \
} while (0)

#define CHK_COND_RETURN(condition, ret, print) \
do { \
    if (condition) { \
        ALOGE("%s ", ((print) == NULL) ? "" : (print)); \
        return ret; \
    } \
} while (0)

#define CHK_FALSE_RETURN(condition, ret, print) \
do { \
    if (!(condition)) { \
        ALOGE("%s ", ((print) == NULL) ? "" : (print)); \
        return ret; \
    } \
} while (0)
#else
#define CHK_NULL_RETURN(ptr, ret, printfString) \
do { \
    if ((ptr) == NULL) { \
        uapi_diag_error_log(0, printfString); \
        return (ret); \
    } \
} while (0)

#define CHK_NULL_RETURN_NONE(ptr, printfString) \
do { \
    if ((ptr) == NULL) { \
        uapi_diag_error_log(0, printfString); \
        return; \
    } \
} while (0)

#define CHK_FAILED_RETURN(value, target, ret, print) \
do { \
    if ((value) != (target)) { \
        uapi_diag_error_log(0, print); \
        return (ret); \
    } \
} while (0)

#define CHK_COND_RETURN(condition, ret, print) \
do { \
    if (condition) { \
        uapi_diag_error_log(0, print); \
        return ret; \
    } \
} while (0)

#define CHK_FALSE_RETURN(condition, ret, print) \
do { \
    if (!(condition)) { \
        uapi_diag_error_log(0, print); \
        return ret; \
    } \
} while (0)
#endif

#define AUDIO_LOCK(mutex)               \
    do {                                    \
        (void)MediaMutexLock((mutex)); \
    } while (0)

#define AUDIO_UNLOCK(mutex)               \
    do {                                      \
        (void)MediaMutexUnLock((mutex)); \
    } while (0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // AUDIO_UTILS_H
