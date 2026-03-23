/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: timer src fie.
 * Author: CompanyName
 * Create:
 */

#include "nativeapi_timer_task.h"
#include "los_task_pri.h"
#include "los_hwi.h"
#include "cmsis_os2.h"

int InitTimerTask()
{
    return 0;
}

int StartTimerTask(bool isPeriodic, const unsigned int delay, void* userCallback, void* userContext, timerHandle_t* timerHandle)
{
    if (userCallback == NULL || timerHandle == NULL) {
        return -1;
    }

    uint8_t mode = isPeriodic ? osTimerPeriodic : osTimerOnce;
    *timerHandle = osTimerNew(userCallback, mode, userContext, NULL);
    if (*timerHandle == NULL) {
        return -1;
    }

    if (osTimerStart(*timerHandle, delay) != osOK) {
        return -1;
    }

    return 0;
}

int StopTimerTask(const timerHandle_t timerHandle)
{
    if (timerHandle == NULL) {
        return -1;
    }

    osTimerStop(timerHandle);
    return osTimerDelete(timerHandle);
}

void *osThreadGetArgument(void)
{
    if (OS_INT_ACTIVE) {
        return NULL;
    }
    LosTaskCB *taskCb = (LosTaskCB *)osThreadGetId();
    if (taskCb == NULL) {
        return NULL;
    }
#ifdef LOSCFG_OBSOLETE_API
    return (void *)(taskCb->args[0]);
#else
    return (void *)(taskCb->args);
#endif
}

