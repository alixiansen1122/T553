/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: nativeapu_timer_task header file.
 * Author: CompanyName
 * Create:
 */

#ifndef JS_NATIVEAPI_TIMER_TASK_H
#define JS_NATIVEAPI_TIMER_TASK_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef void *timerHandle_t;

int InitTimerTask(void);
int StartTimerTask(bool isPeriodic, const unsigned int delay, void* userCallback,
                   void* userContext, timerHandle_t* timerHandle);
int StopTimerTask(const timerHandle_t timerHandle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif  /* __cplusplus */

#endif /* JS_NATIVEAPI_TIMER_TASK_H */
