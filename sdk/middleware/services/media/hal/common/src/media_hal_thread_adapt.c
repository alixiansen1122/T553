/*
* Copyright (c) @CompanyNameMagicTag. 2022-2022. All rights reserved.
* Description: thread adapt impl
* Author: Media Software Group
* Create: 2022-02-14
*/

#include "media_hal_thread_adapt.h"
#include <stdlib.h>
#include "securec.h"
#include "media_hal_common.h"

#ifdef CMSIS_SUPPORT
#include <cmsis_os2.h>
#else
#include <pthread.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MODULE_NAME "MediaHalThreadAdapt"
#define THREAD_PRI_MAX 99
#define TIME_SCALE_S_2_US 1000000
#define TIME_SCALE 1000
#ifdef CMSIS_SUPPORT
static void MediaHalSetCmsisThreadPriorityAttr(osThreadAttr_t *attr, uint32_t priority)
{
    if (priority == 0) {
        return;
    }

    if (priority > THREAD_PRI_MAX) {
        priority = THREAD_PRI_MAX;
    }

    uint32_t tmp = priority * (osPriorityHigh - osPriorityLow3) / THREAD_PRI_MAX + osPriorityLow3;
    attr->priority = (osPriority_t)tmp;
}
#else
pthread_mutex_t g_globalMediaHalMutexLock = PTHREAD_MUTEX_INITIALIZER;
static bool MediaHalSetPosixThreadAttr(pthread_attr_t *attr, const MediaHalThreadattr *halAttr)
{
    bool set = false;
    if (halAttr->detached) {
        pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
        set = true;
    }

    if (halAttr->stackSize > 0) {
        pthread_attr_setstacksize(attr, halAttr->stackSize);
        set = true;
    }

    if (halAttr->sched == THREAD_SCHED_INVALID || halAttr->sched > THREAD_SCHED_RR || halAttr->prioty == 0) {
        return set;
    }

    int32_t threadSched;
    int32_t pri = ((halAttr->prioty > 0) && (halAttr->prioty < THREAD_PRI_MAX)) ? halAttr->prioty : THREAD_PRI_MAX;
    if (halAttr->sched == THREAD_SCHED_FIFO) {
        threadSched = SCHED_FIFO;
    } else if (halAttr->sched == THREAD_SCHED_RR) {
        threadSched = SCHED_RR;
    } else {
        threadSched = SCHED_OTHER;
        pri = 0;
    }
    pthread_attr_setschedpolicy(attr, threadSched);

    struct sched_param schedParam;
    if (memset_s(&schedParam, sizeof(schedParam), 0, sizeof(schedParam)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
    }
    schedParam.sched_priority = pri;
    pthread_attr_setschedparam(attr, (const struct sched_param *)&schedParam);

    return true;
}
#endif

static void *MediaHalThreadMallocZ(uint32_t len)
{
    if (len == 0) {
        return NULL;
    }

    void *value = malloc(len);
    if (value == NULL) {
        return NULL;
    }

    if (memset_s(value, len, 0, len) != EOK) {
        free(value);
        value = NULL;
    }

    return value;
}

MediaHalMutexHandle MediaHalMutexCreate(const MediaHalMutexAttr *attr)
{
#ifdef CMSIS_SUPPORT
    osMutexAttr_t mutexAttr = { NULL, 0, NULL, 0 };
    osMutexAttr_t *pAttr = NULL;
    if (attr != NULL && attr->isRecursive) {
        mutexAttr.attr_bits |= osMutexRecursive;
        pAttr = &mutexAttr;
    }
    osMutexId_t mutex = osMutexNew(pAttr);
    return mutex;
#else
    pthread_mutex_t *mutexId = (pthread_mutex_t *)MediaHalThreadMallocZ(sizeof(pthread_mutex_t));
    if (mutexId == NULL) {
        return NULL;
    }
    pthread_mutexattr_t mutexAttr = {0};
    pthread_mutexattr_t *pAttr = NULL;
    bool needSetAttr = (attr != NULL && attr->isRecursive && pthread_mutexattr_init(&mutexAttr) == 0);
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

void MediaHalMutexDestroy(MediaHalMutexHandle *mutex)
{
    if (mutex == NULL || *mutex == NULL) {
        return;
    }
#ifdef CMSIS_SUPPORT
    (void)osMutexDelete(*mutex);
#else
    if (*mutex == NULL) {
        return;
    }

    (void)pthread_mutex_destroy((pthread_mutex_t *)*mutex);
    free(*mutex);
#endif
    *mutex = NULL;
}

void MediaHalMutexLock(MediaHalMutexHandle mutex)
{
    if (mutex == NULL) {
        return;
    }

#ifdef CMSIS_SUPPORT
    (void)osMutexAcquire(mutex, osWaitForever);
#else
    (void)pthread_mutex_lock((pthread_mutex_t *)mutex);
#endif
}

void MediaHalMutexUnLock(MediaHalMutexHandle mutex)
{
    if (mutex == NULL) {
        return;
    }

#ifdef CMSIS_SUPPORT
    (void)osMutexRelease(mutex);
#else
    (void)pthread_mutex_unlock((pthread_mutex_t *)mutex);
#endif
}

void MediaHalMutexGlobalLock(void)
{
#ifdef CMSIS_SUPPORT
    osKernelLock();
#else
    pthread_mutex_lock(&g_globalMediaHalMutexLock);
#endif
}

void MediaHalMutexGlobalUnLock(void)
{
#ifdef CMSIS_SUPPORT
    osKernelUnlock();
#else
    pthread_mutex_unlock(&g_globalMediaHalMutexLock);
#endif
}

void MediaHalInitStaticMutexLock(MediaHalMutexHandle *mutex)
{
    if (mutex == NULL || *mutex != NULL) {
        return;
    }

    MediaHalMutexGlobalLock();
    if (*mutex == NULL) {
        MediaHalMutexAttr attr = { false };
        *mutex = MediaHalMutexCreate(&attr);
    }
    MediaHalMutexGlobalUnLock();
}

typedef struct {
    MediaHalThreadFunType process;
    void *argument;
    MediaHalThreadIdHandle mainId;
    MediaHalThreadIdHandle subId;
    bool detached;
    MediaHalThreadCondHandle cond;
    void *stackMem;
} MediaHalThreadMember;

#define THREAD_MANAGER_MAX_CNT 10
static MediaHalMutexHandle g_mediaHalThreadManagerLock;
static MediaHalThreadMember *g_mediaHalThreadManager[THREAD_MANAGER_MAX_CNT] = { 0 };

static uint32_t MediaHalGetValidThreadManagerIdx(void)
{
    for (uint32_t i = 0; i < THREAD_MANAGER_MAX_CNT; i++) {
        if (g_mediaHalThreadManager[i] == NULL) {
            return i;
        }
    }

    return UINT32_MAX;
}

static uint32_t MediaHalGetThreadManagerIdxFromIdHandle(const MediaHalThreadIdHandle id)
{
    if (id == NULL) {
        return UINT32_MAX;
    }

    for (uint32_t i = 0; i < THREAD_MANAGER_MAX_CNT; i++) {
        if (g_mediaHalThreadManager[i] != NULL &&
            g_mediaHalThreadManager[i]->subId != NULL && g_mediaHalThreadManager[i]->subId == id) {
            return i;
        }
    }

    return UINT32_MAX;
}

static void HalFreeStackMemory(MediaHalThreadMember *member)
{
    if (member != NULL && member->stackMem != NULL) {
        free(member->stackMem);
        member->stackMem = NULL;
    }
}

#define THREAD_EVENT_EXIT_FLAG 0x45584954U /* EXIT ascii */
#ifdef CMSIS_SUPPORT
static void MediaHalThreadFunc(void *arg)
#else
static void *MediaHalThreadFunc(void *arg)
#endif
{
    if (arg == NULL) {
#ifdef CMSIS_SUPPORT
        return;
#else
        return NULL;
#endif
    }
    MediaHalThreadMember *member = (MediaHalThreadMember *)arg;
    member->process(member->argument);
    MediaHalMutexLock(g_mediaHalThreadManagerLock);
    if (member->detached) {
        uint32_t idx = MediaHalGetThreadManagerIdxFromIdHandle(member->subId);
        if (idx < THREAD_MANAGER_MAX_CNT) {
            g_mediaHalThreadManager[idx] = NULL;
        }
        MediaHalThreadCondDestroy(&member->cond);
        HalFreeStackMemory(member);
        free(arg);
    } else {
#ifdef CMSIS_SUPPORT
#ifndef CMSIS_SUPPORT_JOIN
        MediaHalThreadCondSignal(member->cond);
#endif
#endif
    }
    MediaHalMutexUnLock(g_mediaHalThreadManagerLock);
#ifndef CMSIS_SUPPORT
    return NULL;
#endif
}

static int32_t MediaHalDoCreateThread(MediaHalThreadMember *member, const MediaHalThreadattr *attr)
{
#ifdef CMSIS_SUPPORT
    MediaHalThreadIdHandle subId = NULL;
    if (attr == NULL) {
        subId = osThreadNew(MediaHalThreadFunc, member, NULL);
    } else {
#ifdef CMSIS_SUPPORT_JOIN
        uint32_t attrBits = attr->detached ? osThreadDetached : osThreadJoinable;
#else
        if (!attr->detached) {
            member->cond = MediaHalThreadCondCreate();
            if (member->cond == NULL) {
                return -1;
            }
        }
        uint32_t attrBits = osThreadDetached;
#endif
        osThreadAttr_t taskAttr = { attr->name, attrBits, NULL, 0, NULL, attr->stackSize, osPriorityNormal, 0, 0 };
        if (strcmp(attr->name, "BufferThread") == 0) {
            taskAttr.stack_mem = memalign(16, attr->stackSize); // 16 meanse mem align to 16
            member->stackMem = taskAttr.stack_mem;
        }
        MediaHalSetCmsisThreadPriorityAttr(&taskAttr, attr->prioty);
        subId = osThreadNew(MediaHalThreadFunc, member, &taskAttr);
    }
    member->subId = subId;
#else
    pthread_attr_t taskAttr;
    pthread_attr_t *pTaskAttr = NULL;
    bool needSetAttr = (attr != NULL && pthread_attr_init(&taskAttr) == 0);
    if (needSetAttr && MediaHalSetPosixThreadAttr(&taskAttr, attr)) {
        pTaskAttr = &taskAttr;
    }

    pthread_t thdId = 0;
    if (pthread_create(&thdId, pTaskAttr, MediaHalThreadFunc, member) != 0) {
        if (needSetAttr) {
            (void)pthread_attr_destroy(&taskAttr);
        }
        return -1;
    }
    if (needSetAttr) {
        (void)pthread_attr_destroy(&taskAttr);
    }
    member->subId = (MediaHalThreadIdHandle)(uintptr_t)thdId;
#endif

    return  member->subId != NULL ? 0 : -1;
}

MediaHalThreadIdHandle MediaHalThreadCreate(MediaHalThreadFunType func, void *argument, const MediaHalThreadattr *attr)
{
    if (func == NULL) {
        return NULL;
    }

    MediaHalThreadMember *member = (MediaHalThreadMember *)MediaHalThreadMallocZ(sizeof(MediaHalThreadMember));
    if (member == NULL) {
        return NULL;
    }

    MediaHalThreadIdHandle id = NULL;
    MediaHalInitStaticMutexLock(&g_mediaHalThreadManagerLock);
    MediaHalMutexLock(g_mediaHalThreadManagerLock);
    uint32_t idx = MediaHalGetValidThreadManagerIdx();
    if (idx >= THREAD_MANAGER_MAX_CNT) {
        free(member);
        goto EXIT;
    }

    member->process = func;
    member->argument = argument;
    member->mainId = MediaHalThreadGetId();
    member->detached = attr != NULL ? attr->detached : false;
    member->stackMem = NULL;
    if (MediaHalDoCreateThread(member, attr) != 0) {
        HalFreeStackMemory(member);
        free(member);
        goto EXIT;
    }
    g_mediaHalThreadManager[idx] = member;
    id = member->subId;

EXIT:
    MediaHalMutexUnLock(g_mediaHalThreadManagerLock);
    return id;
}

MediaHalThreadIdHandle MediaHalThreadGetId(void)
{
#ifdef CMSIS_SUPPORT
    return osThreadGetId();
#else
    return (MediaHalThreadIdHandle)(uintptr_t)pthread_self();
#endif
}

void MediaHalThreadJoin(MediaHalThreadIdHandle *thdId)
{
    if (thdId == NULL || *thdId == NULL) {
        return;
    }

#ifdef CMSIS_SUPPORT
#ifdef CMSIS_SUPPORT_JOIN
    osThreadJoin((osThreadId_t)*thdId);
#endif
#else
    pthread_join((pthread_t)(uintptr_t)*thdId, NULL);
#endif
    MediaHalMutexLock(g_mediaHalThreadManagerLock);
    uint32_t idx = MediaHalGetThreadManagerIdxFromIdHandle(*thdId);
    if (idx < THREAD_MANAGER_MAX_CNT) {
        MediaHalThreadMember *member = g_mediaHalThreadManager[idx];
        if (member != NULL && member->detached) { /* detach sub thread, will free handle after sub thread exit */
            *thdId = NULL;
            HalFreeStackMemory(member);
            MediaHalMutexUnLock(g_mediaHalThreadManagerLock);
            return;
        }
#ifdef CMSIS_SUPPORT
#ifndef CMSIS_SUPPORT_JOIN
        MediaHalThreadCondWait(member->cond, g_mediaHalThreadManagerLock);
        MediaHalThreadCondDestroy(&member->cond);
#endif
#endif
        g_mediaHalThreadManager[idx] = NULL;
        HalFreeStackMemory(member);
        free(member);
    }
    MediaHalMutexUnLock(g_mediaHalThreadManagerLock);
    *thdId = NULL;
}

#define THREAD_EVENT_FLAG 0x00000001U
MediaHalThreadCondHandle MediaHalThreadCondCreate(void)
{
#ifdef CMSIS_SUPPORT
    return osEventFlagsNew(NULL);
#else
    pthread_cond_t *condHandle = (pthread_cond_t *)MediaHalThreadMallocZ(sizeof(pthread_cond_t));
    if (condHandle == NULL) {
        return NULL;
    }

    pthread_cond_init(condHandle, NULL);
    return condHandle;
#endif
}

void MediaHalThreadCondSignal(MediaHalThreadCondHandle condHandle)
{
    if (condHandle == NULL) {
        return;
    }

#ifdef CMSIS_SUPPORT
    osEventFlagsSet(condHandle, THREAD_EVENT_FLAG);
#else
    pthread_cond_signal((pthread_cond_t *)condHandle);
#endif
}

void MediaHalThreadCondWait(MediaHalThreadCondHandle condHandle, MediaHalMutexHandle mutexHandle)
{
    if (mutexHandle == NULL || condHandle == NULL) {
        return;
    }

#ifdef CMSIS_SUPPORT
    MediaHalMutexUnLock(mutexHandle);
    osEventFlagsWait(condHandle, THREAD_EVENT_FLAG, osFlagsWaitAll, osWaitForever);
    MediaHalMutexLock(mutexHandle);
#else
    (void)pthread_cond_wait((pthread_cond_t *)condHandle, (pthread_mutex_t *)mutexHandle);
#endif
}

void MediaHalThreadCondTimeWait(MediaHalThreadCondHandle condHandle, MediaHalMutexHandle mutexHandle, uint32_t delayUs)
{
    if (mutexHandle == NULL || condHandle == NULL) {
        return;
    }

#ifdef CMSIS_SUPPORT
    MediaHalMutexUnLock(mutexHandle);
    osEventFlagsWait(condHandle, THREAD_EVENT_FLAG, osFlagsWaitAll,
        (uint64_t)delayUs * osKernelGetTickFreq() / TIME_SCALE_S_2_US);
    MediaHalMutexLock(mutexHandle);
#else
    struct timespec outtime;
    if (delayUs >= TIME_SCALE_S_2_US) {
        outtime.tv_sec = delayUs / TIME_SCALE_S_2_US;
        uint32_t outtimeNsec = (delayUs % TIME_SCALE_S_2_US) * TIME_SCALE;
        outtime.tv_nsec = outtimeNsec;
    } else {
        outtime.tv_sec = 0;
        outtime.tv_nsec = delayUs * TIME_SCALE;
    }

    pthread_cond_timedwait((pthread_cond_t *)condHandle, (pthread_mutex_t *)mutexHandle, &outtime);
#endif
}

void MediaHalThreadCondDestroy(MediaHalThreadCondHandle *condHandle)
{
    if (condHandle == NULL || *condHandle == NULL) {
        return;
    }

#ifdef CMSIS_SUPPORT
    osEventFlagsDelete(*condHandle);
    *condHandle = NULL;
#else
    pthread_cond_destroy((pthread_cond_t *)*condHandle);
    free(*condHandle);
    *condHandle = NULL;
#endif
}

#ifdef __cplusplus
};
#endif
