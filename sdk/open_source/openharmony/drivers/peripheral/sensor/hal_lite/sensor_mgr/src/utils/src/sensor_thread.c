/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor thread impl
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#include "sensor_thread.h"
#include <unistd.h>
#include <sys/time.h>
#include "securec.h"

#ifndef CMSIS_SUPPORT
pthread_mutex_t g_globalMutexLock = PTHREAD_MUTEX_INITIALIZER;
static void *ThreadMallocZ(uint32_t len)
{
    if (len == 0) {
        return NULL;
    }

    void *value = malloc(len);
    if (value ==  NULL) {
        return  NULL;
    }

    if (memset_s(value, len, 0, len) != EOK) {
        free(value);
        value =  NULL;
    }

    return value;
}
#endif

MutexId SensorMutexCreate(MutexAttr *attr)
{
#ifdef CMSIS_SUPPORT
    osMutexAttr_t mutexAttr = {  NULL, 0,  NULL, 0 };
    osMutexAttr_t *pAttr =  NULL;
    if (attr !=  NULL && attr->isRecursive) {
        mutexAttr.attr_bits |= osMutexRecursive;
        pAttr = &mutexAttr;
    }
    osMutexId_t mutex = osMutexNew(pAttr);
    return mutex;
#else
    pthread_mutex_t *mutexId = (pthread_mutex_t *)ThreadMallocZ(sizeof(pthread_mutex_t));
    if (mutexId ==  NULL) {
        return  NULL;
    }
    pthread_mutexattr_t mutexAttr = {0};
    pthread_mutexattr_t *pAttr =  NULL;
    bool needSetAttr = (attr !=  NULL && attr->isRecursive && pthread_mutexattr_init(&mutexAttr) == 0);
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

void SensorMutexDestroy(MutexId *mutex)
{
    if (mutex ==  NULL || *mutex ==  NULL) {
        return;
    }
#ifdef CMSIS_SUPPORT
    (void)osMutexDelete(*mutex);
#else
    if (*mutex ==  NULL) {
        return;
    }

    (void)pthread_mutex_destroy((pthread_mutex_t *)*mutex);
    free(*mutex);
#endif
    *mutex =  NULL;
}

void SensorMutexLock(MutexId mutex)
{
    if (mutex ==  NULL) {
        return;
    }

#ifdef CMSIS_SUPPORT
    (void)osMutexAcquire(mutex, osWaitForever);
#else
    (void)pthread_mutex_lock((pthread_mutex_t *)mutex);
#endif
}

void SensorMutexUnLock(MutexId mutex)
{
    if (mutex ==  NULL) {
        return;
    }

#ifdef CMSIS_SUPPORT
    (void)osMutexRelease(mutex);
#else
    (void)pthread_mutex_unlock((pthread_mutex_t *)mutex);
#endif
}

void SensorMutexGlobalLock(void)
{
#ifdef CMSIS_SUPPORT
    osKernelLock();
#else
    pthread_mutex_lock(&g_globalMutexLock);
#endif
}

void SensorMutexGlobalUnLock(void)
{
#ifdef CMSIS_SUPPORT
    osKernelUnlock();
#else
    pthread_mutex_unlock(&g_globalMutexLock);
#endif
}

void SensorInitStaticMutexLock(MutexId *mutex)
{
    if (mutex ==  NULL || *mutex !=  NULL) {
        return;
    }

    SensorMutexGlobalLock();
    if (*mutex == NULL) {
        MutexAttr attr = { false };
        *mutex = SensorMutexCreate(&attr);
    }
    SensorMutexGlobalUnLock();
}

ThreadId SensorThreadCreate(SensorThreadCfg *cfg)
{
    osThreadAttr_t taskAttr = {cfg->name, 0, NULL, 0, NULL, cfg->stackSize, (osPriority_t)(cfg->priority), 0, 0};
    taskAttr.stack_mem = memalign(16, taskAttr.stack_size); ;
    cfg->stackMem = taskAttr.stack_mem;
    return (ThreadId)osThreadNew((osThreadFunc_t)cfg->func, cfg->argv, &taskAttr);
}

void SensorThreadDestroy(SensorThreadCfg *cfg)
{
    SensorFree(cfg->stackMem);
    cfg->stackMem = NULL;
}