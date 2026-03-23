/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */
#ifndef LV_MUTEX_H
#define LV_MUTEX_H

#include "lv_conf.h"
#if LV_USE_MUTEX

#include <stdio.h>
#include <stdbool.h>
#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef osMutexId_t LvMutex;
typedef osEventFlagsId_t LvCondition;

bool LvInitMutex(LvMutex* mutex);

void LvDeinitMutex(LvMutex* mutex);

bool LvInitCondition(LvCondition* cond);

void LvDeinitCondition(LvCondition* cond);

bool LvLock(const LvMutex* mutex);

bool LvUnlock(const LvMutex* mutex);

bool LvWaitOnCondition(const LvMutex* mutex, const LvCondition* cond);

bool LvSignal(const LvCondition* cond);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
#endif