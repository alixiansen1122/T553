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

#include "location_utils.h"
#include "location_thread.h"
#include <cstdio>
#include <cstring>
#include <climits>
#include <cstdarg>
#include <ctime>
#include <unistd.h>
#include <sys/time.h>

#ifndef CMSIS_SUPPORT
pthread_mutex_t g_globalMutexLock = PTHREAD_MUTEX_INITIALIZER;
static void *ThreadMallocZ(uint32_t len)
{
    if (len == 0) {
        return nullptr;
    }

    void *value = malloc(len);
    if (value == nullptr) {
        return nullptr;
    }

    if (memset_s(value, len, 0, len) != EOK) {
        free(value);
        value = nullptr;
    }

    return value;
}
#endif

MutexId MutexCreate(MutexAttr *attr)
{
#ifdef CMSIS_SUPPORT
    osMutexAttr_t mutexAttr = { nullptr, 0, nullptr, 0 };
    osMutexAttr_t *pAttr = nullptr;
    if (attr != nullptr && attr->isRecursive) {
        mutexAttr.attr_bits |= osMutexRecursive;
        pAttr = &mutexAttr;
    }
    osMutexId_t mutex = osMutexNew(pAttr);
    return mutex;
#else
    pthread_mutex_t *mutexId = (pthread_mutex_t *)ThreadMallocZ(sizeof(pthread_mutex_t));
    if (mutexId == nullptr) {
        return nullptr;
    }
    pthread_mutexattr_t mutexAttr = {0};
    pthread_mutexattr_t *pAttr = nullptr;
    bool needSetAttr = (attr != nullptr && attr->isRecursive && pthread_mutexattr_init(&mutexAttr) == 0);
    if (needSetAttr) {
        if (pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE) == 0) {
            pAttr = &mutexAttr;
        }
    }
    (void)pthread_mutex_init(mutexId, pAttr);
    if (needSetAttr) {
        pthread_mutexattr_destroy(&mutexAttr);
    }
    return mutexId;
#endif
}

void MutexDestroy(MutexId *mutex)
{
    if (mutex == nullptr || *mutex == nullptr) {
        return;
    }
#ifdef CMSIS_SUPPORT
    (void)osMutexDelete(*mutex);
#else
    if (*mutex == nullptr) {
        return;
    }

    (void)pthread_mutex_destroy((pthread_mutex_t *)*mutex);
    free(*mutex);
#endif
    *mutex = nullptr;
}

void MutexLock(MutexId mutex)
{
    if (mutex == nullptr) {
        return;
    }

#ifdef CMSIS_SUPPORT
    (void)osMutexAcquire(mutex, osWaitForever);
#else
    (void)pthread_mutex_lock((pthread_mutex_t *)mutex);
#endif
}

void MutexUnLock(MutexId mutex)
{
    if (mutex == nullptr) {
        return;
    }

#ifdef CMSIS_SUPPORT
    (void)osMutexRelease(mutex);
#else
    (void)pthread_mutex_unlock((pthread_mutex_t *)mutex);
#endif
}

void MutexGlobalLock(void)
{
#ifdef CMSIS_SUPPORT
    osKernelLock();
#else
    pthread_mutex_lock(&g_globalMutexLock);
#endif
}

void MutexGlobalUnLock(void)
{
#ifdef CMSIS_SUPPORT
    osKernelUnlock();
#else
    pthread_mutex_unlock(&g_globalMutexLock);
#endif
}

void InitStaticMutexLock(MutexId *mutex)
{
    if (mutex == nullptr || *mutex != nullptr) {
        return;
    }

    MutexGlobalLock();
    if (*mutex == nullptr) {
        MutexAttr attr = { false };
        *mutex = MutexCreate(&attr);
    }
    MutexGlobalUnLock();
}

ThreadId ThreadCreate(ThreadFunc func, void *argv, const ThreadAttr *attr)
{
    osThreadAttr_t taskAttr = {attr->name, 0, NULL, 0, NULL, attr->stackSize, (osPriority_t)(attr->priority), 0, 0};
    // const int MEM_ALIGN = 16;
    // taskAttr.stack_mem = memalign(MEM_ALIGN, attr->stackSize);
    return (ThreadId)osThreadNew((osThreadFunc_t)func, argv, &taskAttr);
}