/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor thread
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#ifndef SENSOR_THREAD_H
#define SENSOR_THREAD_H

#include <stdio.h>
#include <stdbool.h>
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

MutexId SensorMutexCreate(MutexAttr *attr);

void SensorMutexDestroy(MutexId *mutex);

void SensorMutexLock(MutexId mutex);

void SensorMutexUnLock(MutexId mutex);

void SensorMutexGlobalLock(void);

void SensorMutexGlobalUnLock(void);

void SensorInitStaticMutexLock(MutexId *mutex);

typedef void *ThreadId;
typedef void (*ThreadFunc)(void *argv);

typedef struct {
    const char *name;    // name of the thread
    uint32_t stackSize;  // size of stack
    uint8_t priority;    // initial thread priority
    ThreadFunc func;
    void *argv;
    void *stackMem;
} SensorThreadCfg;

ThreadId SensorThreadCreate(SensorThreadCfg *cfg);

void SensorThreadDestroy(SensorThreadCfg *cfg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif  // SENSOR_THREAD_H
