/*
 * Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: media hal common
 * Author: Media Software Group
 * Create: 2021-04-02
 */

#ifndef MEDIA_HAL_COMMON_H
#define MEDIA_HAL_COMMON_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include "media_hal_thread_adapt.h"

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

typedef void* HalHandle;

#define MEDIA_HAL_OK 0
#define MEDIA_HAL_ERR (-1)
#define MEDIA_HAL_NO_MEM (-2)
#define MEDIA_HAL_INIT_FAIL (-3)
#define MEDIA_HAL_INVALID_PARAM (-4)
#define MEDIA_HAL_PERMISSION_DENIED (-5)
#define MEDIA_HAL_TIMEOUT (-6)
#define MEDIA_HAL_ERR_AGAIN (-7)
#define MEDIA_HAL_NOT_SUPPORT_GET_MULT_FRAME (-8)

#define MEDIA_HAL_LOG_ENABLE  1
#define MEDIA_HAL_UNUSED(x) ((void)(x))

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#define NANOS_PER_MICROSECOND 1000
#define MICROS_PER_SECOND 1000000

#define MEDIA_HAL_MIN_VOLUME 0.0
#define MEDIA_HAL_MAX_VOLUME 1.0

/*
 * @brief  log level type.
 */
typedef enum {
    MEDIA_HAL_LOG_LEVEL_VERBOSE = 0,
    MEDIA_HAL_LOG_LEVEL_DEBUG,
    MEDIA_HAL_LOG_LEVEL_INFO,
    MEDIA_HAL_LOG_LEVEL_WARN,
    MEDIA_HAL_LOG_LEVEL_ERR
} MEDIA_HAL_LOG_LEVEL;

int64_t CalDiffTimeBetween(struct timeval timeStart, struct timeval timeEnd,
    const char *tag, const char *name, bool showLog);
int32_t MediaHalLogPrintf(const char *modName, MEDIA_HAL_LOG_LEVEL level, const char *fmt, ...);
int32_t SetMediaHalLogEnabledLevel(MEDIA_HAL_LOG_LEVEL level);

#define MEDIA_DECORATOR_LOG(op, modName, level, fmt, args...)                         \
    do {                                                                        \
        op(modName, level, "{%s():%d} " fmt, __FUNCTION__, __LINE__, ##args);   \
    } while (0)

#undef MEDIA_HAL_LOGV
#undef MEDIA_HAL_LOGD
#undef MEDIA_HAL_LOGI
#undef MEDIA_HAL_LOGW
#undef MEDIA_HAL_LOGE

#ifdef MEDIA_HAL_LOG_ENABLE
#if defined(SW_UART_DEBUG) || defined(SW_RTT_DEBUG)
#define MEDIA_HAL_LOGV(modName, fmt, ...) MEDIA_DECORATOR_LOG(MediaHalLogPrintf, modName, MEDIA_HAL_LOG_LEVEL_VERBOSE, \
                                                        fmt, ##__VA_ARGS__)

#define MEDIA_HAL_LOGD(modName, fmt, ...) MEDIA_DECORATOR_LOG(MediaHalLogPrintf, modName, MEDIA_HAL_LOG_LEVEL_DEBUG,  \
                                                        fmt, ##__VA_ARGS__)

#define MEDIA_HAL_LOGI(modName, fmt, ...) MEDIA_DECORATOR_LOG(MediaHalLogPrintf, modName, MEDIA_HAL_LOG_LEVEL_INFO,   \
                                                        fmt, ##__VA_ARGS__)

#define MEDIA_HAL_LOGW(modName, fmt, ...) MEDIA_DECORATOR_LOG(MediaHalLogPrintf, modName, MEDIA_HAL_LOG_LEVEL_WARN,   \
                                                        fmt, ##__VA_ARGS__)

#define MEDIA_HAL_LOGE(modName, fmt, ...) MEDIA_DECORATOR_LOG(MediaHalLogPrintf, modName, MEDIA_HAL_LOG_LEVEL_ERR, \
                                                        fmt, ##__VA_ARGS__)
#else
#define MEDIA_HAL_LOGV(modName, fmt, ...) uapi_diag_debug_log(0, fmt, ##__VA_ARGS__)
#define MEDIA_HAL_LOGD(modName, fmt, ...) uapi_diag_debug_log(0, fmt, ##__VA_ARGS__)
#define MEDIA_HAL_LOGI(modName, fmt, ...) uapi_diag_info_log(0, fmt, ##__VA_ARGS__)
#define MEDIA_HAL_LOGW(modName, fmt, ...) uapi_diag_warning_log(0, fmt, ##__VA_ARGS__)
#define MEDIA_HAL_LOGE(modName, fmt, ...) uapi_diag_error_log(0, fmt, ##__VA_ARGS__)
#endif
#else
#define MEDIA_HAL_LOGV(...)

#define MEDIA_HAL_LOGD(...)

#define MEDIA_HAL_LOGI(...)

#define MEDIA_HAL_LOGW(...)

#define MEDIA_HAL_LOGE(...)
#endif // MEDIA_HAL_LOG_ENABLE

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

#define MEDIA_HAL_LOCK(mutex) \
do { \
    (void)pthread_mutex_lock(&(mutex)); \
} while (0)

#define MEDIA_HAL_UNLOCK(mutex) \
do { \
    (void)pthread_mutex_unlock(&(mutex)); \
} while (0)

#if defined(SW_UART_DEBUG) || defined(SW_RTT_DEBUG)
#define CHK_NULL_RETURN(ptr, ret, printfString) \
do { \
    if ((ptr) == NULL) { \
        MEDIA_HAL_LOGE(MODULE_NAME, "%s :ret:%d", ((printfString) == NULL) ? "" : (printfString), ret); \
        return (ret); \
    } \
} while (0)

#define CHK_NULL_RETURN_NONE(ptr, printfString) \
do { \
    if ((ptr) == NULL) { \
        MEDIA_HAL_LOGE(MODULE_NAME, "%s ", ((printfString) == NULL) ? "" : (printfString)); \
        return; \
    } \
} while (0)

#define CHK_FAILED_RETURN(value, target, ret, print) \
do { \
    if ((value) != (target)) { \
        MEDIA_HAL_LOGE(MODULE_NAME, "%s, %#x != %d ", ((print) == NULL) ? "" : (print), (value), (target)); \
        return (ret); \
    } \
} while (0)

#define CHK_FAILED_NO_RETURN(value, target, print) \
do { \
    if ((value) != (target)) { \
        MEDIA_HAL_LOGE(MODULE_NAME, "%s, %#x != %d ", ((print) == NULL) ? "" : (print), (value), (target)); \
    } \
} while (0)

#define CHK_COND_RETURN(condition, ret, print) \
do { \
    if (condition) { \
        MEDIA_HAL_LOGE(MODULE_NAME, "%s ", ((print) == NULL) ? "" : (print)); \
        return ret; \
    } \
} while (0)

#define MEDIA_HAL_DOFUNC_TIME(func) \
    do { \
        MEDIA_HAL_LOGD(MODULE_NAME, "[%s] in", #func); \
        func; \
        MEDIA_HAL_LOGD(MODULE_NAME, "[%s] out", #func); \
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

#define CHK_FAILED_NO_RETURN(value, target, print) \
do { \
    if ((value) != (target)) { \
        uapi_diag_error_log(0, print); \
    } \
} while (0)

#define CHK_COND_RETURN(condition, ret, print) \
do { \
    if (condition) { \
        uapi_diag_error_log(0, print); \
        return ret; \
    } \
} while (0)

#define MEDIA_HAL_DOFUNC_TIME(func) \
do { \
    func; \
} while (0)
#endif

#ifdef SUPPORT_DL
void *MediaHalDLOpen(const char *filePath);

void *MediaHalDLSym(const VoidPtr module, const char *symbol);

void MediaHalDLClose(VoidPtr dllHdl);
#endif
int64_t CalcDiffTimeMs(struct timeval begin, struct timeval end);

int64_t CalcDiffTimeUs(struct timeval begin, struct timeval end);

bool MediaHalIntMulIsOverflow64(const int64_t a, const int64_t b, int64_t *value);

void *MediaMallocz(uint32_t len);
void *MediaHalMalloc(const size_t bytesNum);
void *MediaHalCalloc(const size_t bytesNum);
void MediaHalFree(const void *src);

int32_t MediaHalConvertUSecToSecAndNSec(int64_t usec, int64_t *sec, int64_t *nsec);

uint64_t GetTimeTicks64(void);

#ifdef HMF_DECRYPT_DATA_ENABLE
typedef struct {
    bool (*isDecrypt)(void *context);
    int32_t (*initDecrypt)(void *context, const char *url);
    int32_t (*decryptReadData)(int32_t fd, void *context, uint8_t *buf, int32_t size);
    int64_t (*decryptSeekData)(int32_t fd, void *context, uint64_t seekPos, const int32_t whence);
    void (*deInitDecrypt)(void *context);
    void *bufContext;
} DecryptDataCb;
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // MEDIA_HAL_COMMON_H
