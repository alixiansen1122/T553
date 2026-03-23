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

#ifndef GRAPHIC_LITE_GRAPHIC_MUTEX_H
#define GRAPHIC_LITE_GRAPHIC_MUTEX_H
#include "stdbool.h"
#include "stdint.h"
#ifdef _WIN32
#include <windows.h>
#elif defined(__FREERTOS__)
#include "cmsis_os2.h"
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
#include <pthread.h>
#else
#include "los_mux.h"
#endif // WIN32
#include "gfx_utils/heap_base.h"

namespace OHOS {
/** @brief graphic mutex adapter for different platform. */
class GraphicMutex : public HeapBase {
public:
    /** Default constructor */
    GraphicMutex()
    {
#ifdef _WIN32
        mutex_ = CreateMutex(NULL, FALSE, NULL);
        initFlag_ = (mutex_ != NULL);
#elif defined(__FREERTOS__)
        mutex_ = osMutexNew(NULL);
        initFlag_ = (mutex_ != NULL);
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
        initFlag_ = (pthread_mutex_init(&mutex_, NULL) == 0);
#else
        initFlag_ = (LOS_MuxCreate(&mutex_) == LOS_OK);
#endif // WIN32
    }

    /** Default destructor */
    ~GraphicMutex()
    {
        if (initFlag_ == false) {
            return;
        }
#ifdef _WIN32
        CloseHandle(mutex_);
#elif defined(__FREERTOS__)
        osMutexDelete(mutex_);
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
        pthread_mutex_destroy(&mutex_);
#else
        LOS_MuxDelete(mutex_);
#endif // WIN32
    }

    inline bool Lock()
    {
        if (initFlag_ == false) {
            return false;
        }
#ifdef _WIN32
        return (WaitForSingleObject(mutex_, INFINITE) == WAIT_OBJECT_0);
#elif defined(__FREERTOS__)
        return (osMutexAcquire(mutex_, osWaitForever) == osOK);
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
        return (pthread_mutex_lock(&mutex_) == 0);
#else
        return (LOS_MuxPend(mutex_, LOS_WAIT_FOREVER) == LOS_OK);
#endif // WIN32
    }

    inline bool Unlock()
    {
        if (initFlag_ == false) {
            return false;
        }
#ifdef _WIN32
        return ReleaseMutex(mutex_);
#elif defined(__FREERTOS__)
        return (osMutexRelease(mutex_) == osOK);
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
        return (pthread_mutex_unlock(&mutex_) == 0);
#else
        return (LOS_MuxPost(mutex_) == LOS_OK);
#endif // WIN32
    }

#if !defined(__FREERTOS__) && (defined __linux__ || defined __LITEOS__)
    pthread_mutex_t& GetMutex()
    {
        return mutex_;
    }
#endif

private:
    bool initFlag_;
#ifdef _WIN32
    HANDLE mutex_;
#elif defined(__FREERTOS__)
    osMutexId_t mutex_ = NULL;
#elif defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_t mutex_;
#else
    uint32_t mutex_;
#endif // WIN32
};

class GraphicCond : public HeapBase {
public:
    GraphicCond()
    {
#ifdef _WIN32
        cond_ = CreateEvent(NULL, TRUE, FALSE, NULL);
#elif defined(__FREERTOS__)
        cond_ = osEventFlagsNew(NULL);
#elif defined __linux__ || defined __LITEOS__
        pthread_cond_init(&cond_, NULL);
#endif
    }

    virtual ~GraphicCond()
    {
#ifdef _WIN32
        CloseHandle(cond_);
#elif defined(__FREERTOS__)
        osEventFlagsDelete(cond_);
#elif defined __linux__ || defined __LITEOS__
        pthread_cond_destroy(&cond_);
#endif
    }

    inline bool Wait(GraphicMutex& mutex)
    {
#ifdef _WIN32
        mutex.Unlock();
        WaitForSingleObject(cond_, INFINITE);
        ResetEvent(cond_);
        mutex.Lock();
#elif defined(__FREERTOS__)
        mutex.Unlock();
        osEventFlagsClear(cond_, 0x0001U);
        uint32_t ret = osEventFlagsWait(cond_, 0x0001U, osFlagsWaitAny, osWaitForever);
        mutex.Lock();
        if ((ret == osFlagsErrorParameter) || (ret == osFlagsErrorResource) ||
            (ret == osFlagsErrorTimeout) || (ret == osFlagsErrorUnknown)) {
            return false;
        }
        return true;
#elif defined __linux__ || defined __LITEOS__
        pthread_mutex_t& lock = mutex.GetMutex();
        return (pthread_cond_wait(&cond_, &lock) == 0);
#endif
    }

    inline bool Signal()
    {
#if defined(_WIN32)
        SetEvent(cond_);
#elif defined(__FREERTOS__)
        return (osEventFlagsSet(cond_, 0x0001U) == osOK);
#elif defined __linux__ || defined __LITEOS__
        return (pthread_cond_signal(&cond_) == 0);
#endif
    }

protected:
#if defined(_WIN32)
    HANDLE cond_;
#elif defined(__FREERTOS__)
    osEventFlagsId_t cond_;
#elif defined __linux__ || defined __LITEOS__
    pthread_cond_t cond_;
#endif
};

// do not support multi-thread
class GraphicLockGuard {
public:
    explicit GraphicLockGuard(GraphicMutex& mutex) : mutex_(mutex)
    {
        Lock();
    }
    ~GraphicLockGuard()
    {
        Unlock();
    }
    void Lock()
    {
        mutex_.Lock();
        lockCnt_++;
    }
    void Unlock()
    {
        if (lockCnt_ > 0) {
            mutex_.Unlock();
            lockCnt_--;
        }
    }

    GraphicLockGuard() = delete;
    GraphicLockGuard(const GraphicLockGuard&) = delete;
    GraphicLockGuard(const GraphicLockGuard&&) = delete;
    GraphicLockGuard& operator=(const GraphicLockGuard&) = delete;
    GraphicLockGuard& operator=(const GraphicLockGuard&&) = delete;

private:
    GraphicMutex& mutex_;
    int8_t lockCnt_ = 0;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_GRAPHIC_MUTEX_H