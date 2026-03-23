/*
* Copyright (c) @CompanyNameMagicTag. 2022-2022. All rights reserved.
* Description: thread adapt header
* Author: Media Software Group
* Create: 2022-02-14
*/

#ifndef MEDIA_HAL_THREAD_ADAPT_H
#define MEDIA_HAL_THREAD_ADAPT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool isRecursive;
} MediaHalMutexAttr;
typedef void *MediaHalMutexHandle;
MediaHalMutexHandle MediaHalMutexCreate(const MediaHalMutexAttr *attr);
void MediaHalMutexDestroy(MediaHalMutexHandle *mutex);
void MediaHalMutexLock(MediaHalMutexHandle mutex);
void MediaHalMutexUnLock(MediaHalMutexHandle mutex);
void MediaHalMutexGlobalLock(void);
void MediaHalMutexGlobalUnLock(void);
void MediaHalInitStaticMutexLock(MediaHalMutexHandle *mutex);

typedef void *MediaHalThreadIdHandle;
typedef void* (*MediaHalThreadFunType)(void *argument);
typedef enum {
    THREAD_SCHED_INVALID = 0,
    THREAD_SCHED_OTHER,
    THREAD_SCHED_FIFO,
    THREAD_SCHED_RR,
} MediaHalThreadSchedPolicy;
typedef struct {
    const char          *name; /* only for cmsis */
    uint32_t            stackSize;
    MediaHalThreadSchedPolicy   sched; /* only for posix */
    uint32_t            prioty;
    bool                detached; /* only for posix */
} MediaHalThreadattr;
MediaHalThreadIdHandle MediaHalThreadCreate(MediaHalThreadFunType func, void *argument, const MediaHalThreadattr *attr);
MediaHalThreadIdHandle MediaHalThreadGetId(void);
void MediaHalThreadJoin(MediaHalThreadIdHandle *thdId);

typedef void *MediaHalThreadCondHandle;

/*
 * create thread cond handle
 * retrun the MediaHalThreadCondHandle
 */
MediaHalThreadCondHandle MediaHalThreadCondCreate(void);

/*
 * send a signal to sub running thread
 * param[in] condHandle   threadCondHandle by MediaHalThreadCondCreate
 */
void MediaHalThreadCondSignal(MediaHalThreadCondHandle condHandle);
/*
 * wait a signal in sub running thread; it must be called in running thread use cmsis interface
 * param[in] condHandle   threadCondHandle by MediaHalThreadCondCreate
 * param[in] mutexHandle  threadMutexHandle by MediaHalMutexCreate; use only for posix
 */
void MediaHalThreadCondWait(MediaHalThreadCondHandle condHandle, MediaHalMutexHandle mutexHandle);
/*
 * wait a signal in sub running thread; it must be called in running thread use cmsis interface
 * param[in] condHandle   threadCondHandle by MediaHalThreadCondCreate
 * param[in] mutexHandle  threadMutexHandle by MediaHalMutexCreate; use only for posix
 * param[in] delayUs      will wait time
 */
void MediaHalThreadCondTimeWait(MediaHalThreadCondHandle condHandle, MediaHalMutexHandle mutexHandle, uint32_t delayUs);
/*
 * destroy thread flag handle
 * param[in] condHandle   threadCondHandle by MediaHalThreadCondCreate
 */
void MediaHalThreadCondDestroy(MediaHalThreadCondHandle *condHandle);

#ifdef __cplusplus
};
#endif
#endif