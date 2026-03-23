/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor task
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include <stdbool.h>
#include "sensor_typedef.h"
#include "sensor_thread.h"
#include "sensor_msgqueue.h"
#include "soc_osal.h"

typedef struct {
    struct osal_list_head list;          /* SensorTaskCfg link point */
    uint32_t taskId;
    SensorThreadCfg threadCfg;
    SensorMsgQueueCfg msgQueueCfg;
    bool runningFlag;
    ThreadId *task;
    bool taskExited;
} SensorTaskCfg;

typedef enum {
    SENSOR_TASK_PRIORITY_HIGH = osPriorityHigh,
    SENSOR_TASK_PRIORITY_MIDDLE = osPriorityNormal,
    SENSOR_TASK_PRIORITY_LOW = osPriorityLow6
} SensorTaskPriority;

#define SENSORMGR_TASK_MSG_QUEUE_LENGTH         32
#define TASK_MSG_QUEUE_LENGTH_DEFAULT           32
#ifdef JS_ENABLE
#define HSENSOR_TASK_STACK_SIZE               8192
#define VSENSOR_TASK_STACK_SIZE               8192
#else
#define HSENSOR_TASK_STACK_SIZE               2048
#define VSENSOR_TASK_STACK_SIZE               4096
#endif

#define SENSOR_TASK_CREATE_OK_WAIT_TIMEOUT          1000 // ms
#define SENSOR_TASK_CREATE_TICK_TIME                50   // ms

#define SENSOR_TASK_MAX_CNT                10

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int32_t SensorTaskInit(void);

/* 创建vsensor任务 */
int32_t SensorTaskCreateVsensorTask(const Vsensor *vsensor);

/* 创建hsensor任务 */
int32_t SensorTaskCreateHsensorTask(const Hsensor *hsensor);

/* 销毁vsensor任务 */
int32_t SensorTaskDestroyVsensorTask(enum SensorTypeTag type);

/* 销毁hsensor任务 */
int32_t SensorTaskDestroyHsensorTask(enum SensorTypeTag type);

/* 获取任务消息队列句柄 */
SensorMsgQueueCfg *SensorTaskGetTaskMsgqueueHandle(uint32_t taskId);

/* 判断任务运行状态（true和false） */
bool SensorTaskGetRunningFlag(uint32_t taskId);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif