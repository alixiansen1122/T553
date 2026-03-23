/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef SECURITY_APP_COMMON_H
#define SECURITY_APP_COMMON_H
//#ifdef LITE_WEARABLE
//#include "verify_util_log.h"
//#include "ohos_hal_log.h"
//#endif
#include "verify_util_config.h"
#include "app_verify_pub.h"
#include "mem_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_PRINT_LEN 1024
#define DOUBLE_SIZE  2
#define MAX_CHARACTER  9
#define BYTE_BITS  8
#define MAX_HASH_SIZE 64

#define FILE_CERCHAIN_VERIFY  10
#define FILE_INTEGRITY_VERIFY  48
#define PROFILE_CERCHAIN_VERIFY  50
#define SECTION_LEN (2 * 1024 * 1024)

#define MAX_MALLOC_SIZE 0x100000  /* 1M */

typedef struct TagVMemStats {
    void *buf;
    int len;
    int state;
    const char *fileName;
    int line;
    struct TagVMemStats *next;
} AppVMemStats;

typedef struct {
    AppVMemStats *head;
    int totalMem;
    int totalMallocCnt;
    int totalFree;
    int totalFreeCnt;
    int maxMemUsed;
} MemStastics;

#define LOG_BUF_SIZE 4096

#ifdef LITE_WEARABLE
#define LOG_DEBUG_V(fmt, argNum, args...) LogPrintf(LOG_MODULE_AAFWK, LOG_LV_INFO, fmt"\r\n", argNum, ##args)
#define LOG_PRINT_STR(fmt, args...)
#else
#ifdef __Z_LITE__
#define LOG_DEBUG_V(fmt, argNum, args...) printf("["__FILE__":%d] :"fmt"\r\n", __LINE__, ##args)
#define LOG_PRINT_STR(fmt, args...)     printf("["__FILE__":%d] :"fmt"\r\n", __LINE__, ##args)
#else
void PrintLog(const char *format, ...);
#define LOG_DEBUG_V(fmt, argNum, args...)  PrintLog("["__FILE__":%d] :"fmt"\r\n", __LINE__, ##args)
#define LOG_PRINT_STR(fmt, args...) PrintLog("["__FILE__":%d] :"fmt"\r\n", __LINE__, ##args)
#endif
#endif
#define P_NULL_RETURN_WTTH_LOG(v) \
do { \
    if ((v) == NULL) { \
        return V_ERR; \
    } \
} \
while (0)

#define P_NULL_RETURN_RET_WTTH_LOG(v, ret) \
do { \
    if ((v) == NULL) { \
        return ret; \
    } \
} \
while (0)

#define P_NULL_RETURN_NULL_WTTH_LOG(v) \
do { \
    if ((v) == NULL) { \
        return NULL; \
    } \
} \
while (0)

#define P_ERR_RETURN_WTTH_LOG(v) \
do { \
    if ((v) != V_OK) { \
        return v; \
    } \
} \
while (0)

#define P_NULL_GOTO_WTTH_LOG(v) \
do { \
    if ((v) == NULL) { \
        goto EXIT; \
    } \
} \
while (0)

#define P_ERR_GOTO_WTTH_LOG(v) \
do { \
    if ((v) != V_OK) { \
        goto EXIT; \
    } \
} \
while (0)

#define FREE_IF_NOT_NULL(p) \
do { \
    if ((p) != NULL) { \
        APPV_FREE(p); \
        p = NULL; \
    } \
} \
while (0)

#if defined(LITE_WEARABLE)
void *AppVPsmemMalloc(uint32_t size);
void AppVPsmemFree(const void *buffer);
#define APPV_MALLOC(size) PSRAM_Malloc(size)
#define APPV_FREE(buf) \
do { \
    if (buf != NULL) { \
        PSRAM_Free(buf); \
        buf = NULL; \
    } \
} while (0)
#define APPV_PSMEM_MALLOC(size) AppVPsmemMalloc(size)
#define APPV_PSMEM_FREE(buf) \
do { \
    if (buf != NULL) { \
        AppVPsmemFree(buf); \
        buf = NULL; \
    } \
} while (0)
#else
#if defined(__Z_LITE__)
#define APPV_MALLOC(size) malloc(size)
#define APPV_FREE(buf) \
do { \
    if ((buf) != NULL) { \
        free(buf); \
        (buf) = NULL; \
    } \
} while (0)
#define APPV_PSMEM_MALLOC(size) malloc(size)
#define APPV_PSMEM_FREE(buf) \
do { \
    if (buf != NULL) { \
        free(buf); \
        buf = NULL; \
    } \
} while (0)
#else
#define APPV_MALLOC(size) AppVMalloc(size, __FILE__, __LINE__);
#define APPV_CALLOC(size, num) AppVCalloc(size, num, __FILE__, __LINE__);
#define APPV_FREE(buf) \
do { \
    if ((buf) != NULL) { \
        AppVFree(buf); \
        (buf) = NULL; \
    } \
} while (0)
#define APPV_PSMEM_MALLOC(size) AppVMalloc(size, __FILE__, __LINE__);
#define APPV_PSMEM_FREE(buf) \
do { \
    if ((buf) != NULL) { \
        AppVFree(buf); \
        (buf) = NULL; \
    } \
} while (0)

#endif
#endif

#if !defined(LITE_WEARABLE) && !defined(__Z_LITE__)
void AppVFree(void *buf);
void *AppVCalloc(int size, int num, const char *filename, const int line);
void *AppVMalloc(int size, const char *filename, const int line);
void AppVCheckMemStatics();
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
