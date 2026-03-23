/*
* Copyright (c) @CompanyNameMagicTag. 2022-2022. All rights reserved.
* Description: convert raw stream from Camera Device to format required for application
* Author: Media Software Group
* Create: 2022-12-20
*/

#ifndef H_HW_COMMON_H
#define H_HW_COMMON_H

#include <cstdint>
#include <cstdio>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SUCCESS 0
#define FAILURE (-1)
#define VERBOSE_LOG_SWITCH 1
#define DEBUG_LOG_SWITCH 0
#define INFO_LOG_SWITCH 1
#define WARNING_LOG_SWITCH 1
#define ERROR_LOG_SWITCH 1
#define HW_HAL_UNUSED(x) ((void)(x))

#define HAL_LOGE(fmt, args...) do {                                                      \
        if (ERROR_LOG_SWITCH) {                               \
            printf("%s:%d: " fmt, __func__, __LINE__, ##args); \
            printf("\n"); \
        }                                                     \
    } while (0)

#define HAL_LOGW(fmt, args...) do {                                                      \
        if (WARNING_LOG_SWITCH) {                             \
            printf("%s:%d: " fmt, __func__, __LINE__, ##args); \
            printf("\n"); \
        }                                                     \
    } while (0)

#define HAL_LOGI(fmt, args...) do {                                                      \
        if (INFO_LOG_SWITCH) {                                \
            printf("%s:%d: " fmt, __func__, __LINE__, ##args); \
            printf("\n"); \
        }                                                     \
    } while (0)

#define HAL_LOGD(fmt, args...) do {                                                      \
        if (DEBUG_LOG_SWITCH) {                               \
            printf("%s:%d: " fmt, __func__, __LINE__, ##args); \
            printf("\n"); \
        }                                                     \
    } while (0)

#define HAL_LOGV(fmt, args...) do {                                                      \
        if (VERBOSE_LOG_SWITCH) {                             \
            printf("%s:%d: " fmt, __func__, __LINE__, ##args); \
            printf("\n"); \
        }                                                     \
    } while (0)

#define LOG_CHK_RETURN(val) \
    do {                       \
        if ((val)) {           \
            HAL_LOGE("");      \
            return;            \
        };                     \
    } while (0)

#define LOG_CHK_RETURN_ERR(val, ret) \
    do {                                \
        if ((val)) {                    \
            HAL_LOGE("");               \
            return ret;                 \
        }                               \
    } while (0)

#define LOG_CHK_RETURN_FAIL(val, ret) \
    do {                                \
        if ((val)) {                    \
            HAL_LOGV("");               \
            return ret;                 \
        }                               \
    } while (0)

#define LOG_DOFUNC_RETURN(func)                                                 \
    do {                                                                        \
        int32_t logRet = func;                                                   \
        if (logRet != SUCCESS) {                                             \
            HAL_LOGE("Call %{public}s return %{public}d[0x%08{public}X]", #func, logRet, logRet);       \
            return logRet;                                                      \
        };                                                                      \
    } while (0)

#define LOG_DOFUNC(func)                                                 \
    do {                                                                        \
        int32_t logRet = func;                                                   \
        if (logRet != SUCCESS) {                                             \
            HAL_LOGE("Call %{public}s return %{public}d[0x%08{public}X]", #func, logRet, logRet);       \
        };                                                                      \
    } while (0)

#define HAL_ENTER() HAL_LOGI("enter")
#define HAL_EXIT() HAL_LOGI("exit")


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif

