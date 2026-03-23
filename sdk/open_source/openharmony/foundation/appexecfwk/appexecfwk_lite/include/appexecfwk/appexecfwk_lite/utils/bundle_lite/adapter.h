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

#ifndef FRAMEWORKS_APP_ADAPTER_H
#define FRAMEWORKS_APP_ADAPTER_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#define NULLPTR nullptr
#else
#define EXTERNC
#define NULLPTR NULL
#endif

#include "mem_util.h"
#ifdef _WIN32
#define AdapterMalloc(a) malloc(a)
#define AdapterFree(a) \
    do { \
        if (a != NULLPTR) { \
            (void) free((void *)a); \
            a = NULLPTR; \
        } \
    } while (0)
#else
#ifndef APP_PLATFORM_WATCHGT
// memory operator define
#include <stdlib.h>

#include "ohos_errno.h"
#include "ohos_types.h"

#define AdapterMalloc(a) PSRAM_Malloc(a)
#define AdapterFree(a) \
    do { \
        if (a != NULLPTR) { \
            (void) PSRAM_Free((void *)a); \
            a = NULLPTR; \
        } \
    } while (0)

// mutex operation define
#define MutexDelete(a) pthread_mutex_destroy(a)
#define MutexAcquire(a, b) pthread_mutex_lock(a)
#define MutexRelease(a) pthread_mutex_unlock(a)

#else // APP_PLATFORM_WATCHGT
#include "stdint.h"
#include "mem_util.h"

#define ERROR_SLEEP_TIMES       300
#define RETRY_TIMES             10

#if 0
#define AdapterMalloc(a) OhosMalloc(MEM_TYPE_APPFMK_LSRAM, a)
#define AdapterFree(a) \
    do { \
        if (a != nullptr) { \
            (void) OhosFree((void *)a); \
            a = nullptr; \
        } \
    } while (0)
#endif

#define AdapterMalloc(a) PSRAM_Malloc(a)
#define AdapterFree(a) \
    do { \
        if (a != NULLPTR) { \
            (void) PSRAM_Free((void *)a); \
            a = NULLPTR; \
        } \
    } while (0)

#if 0
#define UI_Malloc(a) OhosMalloc(MEM_TYPE_APPFMK, a)
#define UI_Free(a) \
    do { \
        if (a != nullptr) { \
            (void) OhosFree((void *)a); \
            a = nullptr; \
        } \
    } while (0)
#endif

#define UI_Malloc(a) PSRAM_Malloc(a)
#define UI_Free(a) \
    do { \
        if (a != NULLPTR) { \
            (void) PSRAM_Free((void *)a); \
            a = NULLPTR; \
        } \
    } while (0)

#define APP_ERRCODE_EXTRA(code1, code2)
#define APP_EVENT(code1)
#define RecordAbiityInfoEvt(code1)
#define MutexDelete(a) osMutexDelete(a)
#define MutexAcquire(a, b) osMutexAcquire(a, b)
#define MutexRelease(a) osMutexRelease(a)
#define SvrFree AdapterFree
#define SvrMalloc AdapterMalloc

#endif // APP_PLATFORM_WATCHGT
#endif // _WIN32
#endif // FRAMEWORKS_APP_ADAPTER_H
