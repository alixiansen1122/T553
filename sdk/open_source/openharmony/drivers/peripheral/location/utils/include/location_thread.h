/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef LOCATION_THREAD_H
#define LOCATION_THREAD_H

#include <stdio.h>
#include <stdint.h>
#include <cmsis_os.h>
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    bool isRecursive;
} MutexAttr;

typedef void *MutexId;

MutexId MutexCreate(MutexAttr *attr);

void MutexDestroy(MutexId *mutex);

void MutexLock(MutexId mutex);

void MutexUnLock(MutexId mutex);

void MutexGlobalLock(void);

void MutexGlobalUnLock(void);

void InitStaticMutexLock(MutexId *mutex);

struct AutoLock {
public:
    AutoLock(MutexId mutex) : mutex_(mutex)
    {
        MutexLock(mutex_);
    }
    ~AutoLock()
    {
        MutexUnLock(mutex_);
    }

private:
    MutexId mutex_;
    // No copying allowed
    AutoLock(const AutoLock &);
    void operator=(const AutoLock &);
};

typedef void *ThreadId;
typedef void (*ThreadFunc)(void *argv);

typedef struct {
    const char *name;    // name of the thread
    uint32_t stackSize;  // size of stack
    uint8_t priority;    // initial thread priority
    uint8_t reserved1;   // reserved1 (must be 0)
    uint16_t reserved2;  // reserved2 (must be 0)
} ThreadAttr;

ThreadId ThreadCreate(ThreadFunc func, void *argv, const ThreadAttr *attr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif  // LOCATION_THREAD_H
