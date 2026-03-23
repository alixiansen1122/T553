/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor task impl
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#include "sensor_task.h"
#include "sensor_tmsg.h"
#include "vsensor_task_mgr.h"
#include "securec.h"
#include "sensor_log.h"
#include "sensor_utils.h"
#include "sensor_common.h"
#include "sensor_memory.h"
#include "hsensor_data_service.h"

static void VsensorTaskFunc(void* param);
static void HsensorTaskFunc(void* param);

typedef struct {
    struct osal_list_head sensorTaskHead; // SensorTaskCfg
    uint32_t sensorTaskNum;
    bool sensorTaskId[SENSOR_TASK_MAX_CNT];
} SensorTaskContext;

static SensorTaskContext g_sensorTaskCxt = {
    .sensorTaskNum = 0,
    .sensorTaskId = {0}
};

static SensorTaskContext *SensorTaskGetInstance(void)
{
    return &g_sensorTaskCxt;
}

int32_t SensorTaskInit(void)
{
    SensorTaskContext *ctx = SensorTaskGetInstance();
    OSAL_INIT_LIST_HEAD(&(ctx->sensorTaskHead));
    ctx->sensorTaskNum = 0;
    return SENSOR_OK;
}

SensorMsgQueueCfg *SensorTaskGetTaskMsgqueueHandle(uint32_t taskId)
{
    SensorTaskContext *ctx = SensorTaskGetInstance();
    SensorTaskCfg *sensorTaskIndex = NULL;
    osal_list_for_each_entry(sensorTaskIndex, &ctx->sensorTaskHead, list)
    {
        if (taskId == sensorTaskIndex->taskId) {
            return &(sensorTaskIndex->msgQueueCfg);
        }
    }
    return NULL;
}

static void SensorTaskAddTaskCfg(SensorTaskCfg *sensorTask)
{
    SensorTaskContext *ctx = SensorTaskGetInstance();
    osal_list_add(&sensorTask->list, &ctx->sensorTaskHead);
    ctx->sensorTaskNum++;
    HILOG_INFO(HILOG_MODULE_SEN, "[task] add task:%u!", sensorTask->taskId);
}

static SensorTaskCfg *SensorTaskFindTaskCfg(enum SensorTypeTag type)
{
    SensorTaskContext *ctx = SensorTaskGetInstance();
    uint32_t taskId = SensorUtilsGetRegisteredTaskId(type);
    if (taskId == SENSOR_FAILURE) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SYS] TaskSync not found taskId type:%d", type);
        return NULL;
    }
    SensorTaskCfg *sensorTaskIndex = NULL;
    SensorTaskCfg *sensorTask = NULL;
    osal_list_for_each_entry(sensorTaskIndex, &ctx->sensorTaskHead, list)
    {
        if (sensorTaskIndex->taskId != taskId) {
            continue;
        }
        sensorTask = sensorTaskIndex;
        break;
    }
    return sensorTask;
}

static void SensorTaskRemoveTaskCfg(SensorTaskCfg *sensorTask)
{
    CHECK_NULL_PTR_RETURN(sensorTask);
    SensorTaskContext *ctx = SensorTaskGetInstance();
    SensorTaskCfg *sensorTaskIndex = NULL;
    bool found = false;
    osal_list_for_each_entry(sensorTaskIndex, &ctx->sensorTaskHead, list)
    {
        if (sensorTask->taskId == sensorTaskIndex->taskId) {
            found = true;
        }
    }
    if (found) {
        g_sensorTaskCxt.sensorTaskId[sensorTask->taskId] = false;
        osal_list_del(&sensorTask->list);
        ctx->sensorTaskNum--;
    }
    SensorFree(sensorTask);
}

bool SensorTaskGetRunningFlag(uint32_t taskId)
{
    SensorTaskContext *ctx = SensorTaskGetInstance();
    SensorTaskCfg *sensorTaskIndex = NULL;
    osal_list_for_each_entry(sensorTaskIndex, &ctx->sensorTaskHead, list)
    {
        if (taskId == sensorTaskIndex->taskId) {
            return sensorTaskIndex->runningFlag;
        }
    }
    HILOG_INFO(HILOG_MODULE_SEN, "[task] run_flag cannot find task:%d!", taskId);
    return false;
}

static void VsensorTaskFunc(void* param)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "[task] in");
    SensorTmsg tmsg = {0};
    SensorTaskCfg *taskCfg = (SensorTaskCfg *)param;
    CHECK_NULL_PTR_RETURN(taskCfg);
    int32_t ret = VsensorTaskMgrInit(taskCfg->taskId);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[task] VsensorTaskMgrInit FAIL! ret:0x%X, taskId:%d", ret, taskCfg->taskId);
        return;
    }
    taskCfg->runningFlag = true;
    while (taskCfg->runningFlag) {
        int32_t ret = SensorTmsgRecv(taskCfg->taskId, &tmsg);
        if ((ret != SENSOR_OK) || (tmsg.id == SENSOR_TMSG_INVALID)) {
            SensorTmsgFree(&tmsg);
            continue;
        }
        (void)VsensorTaskMgrMsgProcess(taskCfg->taskId, &tmsg);

        SensorTmsgFree(&tmsg);
    }
    (void)VsensorTaskMgrDeInit(taskCfg->taskId);
    taskCfg->taskExited = true;
    HILOG_DEBUG(HILOG_MODULE_SEN, "[task] out");
}

static void HsensorTaskFunc(void* param)
{
    SensorTaskCfg *taskCfg = (SensorTaskCfg *)param;
    CHECK_NULL_PTR_RETURN(taskCfg);
    enum SensorTypeTag type = SensorUtilsGetRegisteredType(taskCfg->taskId);
    CHECK_SENSOR_TYPE_ILLEGAL_RETURN(type);
    int32_t ret = HsensorDataServiceInit(type);
    if (ret != SENSOR_OK) {
        return;
    }
    taskCfg->runningFlag = true;

    while (taskCfg->runningFlag) {
        uint32_t delayTime = HsensorDataServiceProcess(type);
        osDelay(delayTime);
    }
    (void)HsensorDataServiceDeInit(type);
    taskCfg->taskExited = true;
}

/* 创建任务接口（异步接口） */
static int32_t SensorCreateTaskAsync(SensorTaskCfg *taskCfg, enum SensorTypeTag type)
{
    /* 创建消息队列 */
    int32_t ret = SensorMsgQueueCreate(&(taskCfg->msgQueueCfg), (uint32_t)(taskCfg->taskId));
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SYS] msg queue create FAIL! ret:0x%X, taskId:%d", ret, taskCfg->taskId);
        return SENSOR_MSG_QUEUE_CREATE_FAIL;
    }
    SensorUtilsRegisterTaskTable(type, taskCfg->taskId);
    /* 创建工作线程 */
    ThreadId *task = SensorThreadCreate(&(taskCfg->threadCfg));
    if (task == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SYS] task create FAIL! task:%d", taskCfg->taskId);
        SensorUtilsUnRegisterTaskTable(type, taskCfg->taskId);
        return SENSOR_TASK_CREATE_FAIL;
    }
    taskCfg->task = task;
    taskCfg->taskExited = false;

    HILOG_INFO(HILOG_MODULE_SEN, "[SYS] task:%d prio:%d.", taskCfg->taskId, taskCfg->threadCfg.priority);
    return SENSOR_OK;
}

/* 销毁任务接口（同步接口，阻塞等待任务销毁完成） */
static void SensorTaskDestoryTaskSync(SensorTaskCfg *taskCfg)
{
    if (taskCfg->taskExited) {
        return;
    }

    taskCfg->runningFlag = false;

    uint16_t taskReadyWait = 0;
    while ((taskReadyWait < SENSOR_TASK_CREATE_OK_WAIT_TIMEOUT) && (!taskCfg->taskExited)) {
        osal_msleep(SENSOR_TASK_CREATE_TICK_TIME);
        taskReadyWait += SENSOR_TASK_CREATE_TICK_TIME;
    }
    SensorThreadDestroy(&taskCfg->threadCfg);
    SensorMsgQueueDelete((uint32_t)(taskCfg->msgQueueCfg.queueId));
}

static int32_t GetSensorTaskId(SensorTaskCfg *taskCfg)
{
    SensorTaskContext *ctx = SensorTaskGetInstance();
    for (int32_t index = 0; index < SENSOR_TASK_MAX_CNT; ++index) {
        if (!ctx->sensorTaskId[index]) {
            taskCfg->taskId = index;
            ctx->sensorTaskId[index] = true;
            return index;
        }
    }
    return SENSOR_FAILURE;
}

static SensorTaskCfg *SensorTaskCreateVsensorTaskCfg(const Vsensor *vsensor)
{
    SensorTaskCfg *taskCfg = (SensorTaskCfg *)SensorMalloc(sizeof(SensorTaskCfg));
    CHECK_NULL_PTR_RETURN_VALUE(taskCfg, NULL);
    SensorTaskContext *ctx = SensorTaskGetInstance();
    OSAL_INIT_LIST_HEAD(&(taskCfg->list));
    SensorThreadCfg threadCfg = {
        vsensor->name,
        VSENSOR_TASK_STACK_SIZE,
        SENSOR_TASK_PRIORITY_HIGH,
        VsensorTaskFunc,
        taskCfg
    };
    taskCfg->threadCfg = threadCfg;
    SensorMsgQueueCfg msgQueueCfg = {
        vsensor->name,
        SENSORMGR_TASK_MSG_QUEUE_LENGTH,
        sizeof(SensorTmsg),
        0,
        0
    };
    taskCfg->msgQueueCfg = msgQueueCfg;
    taskCfg->runningFlag = false;
    taskCfg->task = NULL;
    taskCfg->taskId = GetSensorTaskId(taskCfg);
    if (taskCfg->taskId < 0) {
        HILOG_ERROR(HILOG_MODULE_SEN, "no free taskID!");
        return NULL;
    }
    SensorTaskAddTaskCfg(taskCfg);
    HILOG_INFO(HILOG_MODULE_SEN, "TaskSync create type:%d name:%s", vsensor->type, threadCfg.name);
    return taskCfg;
}

static SensorTaskCfg *SensorTaskCreateHsensorTaskCfg(const Hsensor *hsensor)
{
    SensorTaskCfg *taskCfg = (SensorTaskCfg *)SensorMalloc(sizeof(SensorTaskCfg));
    CHECK_NULL_PTR_RETURN_VALUE(taskCfg, NULL);
    SensorTaskContext *ctx = SensorTaskGetInstance();
    OSAL_INIT_LIST_HEAD(&(taskCfg->list));
    SensorThreadCfg threadCfg = {
        hsensor->name,
        HSENSOR_TASK_STACK_SIZE,
        SENSOR_TASK_PRIORITY_HIGH,
        HsensorTaskFunc,
        taskCfg
    };
    taskCfg->threadCfg = threadCfg;
    SensorMsgQueueCfg msgQueueCfg = {
        hsensor->name,
        SENSORMGR_TASK_MSG_QUEUE_LENGTH,
        sizeof(SensorTmsg),
        0,
        0
    };
    taskCfg->msgQueueCfg = msgQueueCfg;
    taskCfg->runningFlag = false;
    taskCfg->task = NULL;
    taskCfg->taskId = GetSensorTaskId(taskCfg);
    if (taskCfg->taskId < 0) {
        HILOG_ERROR(HILOG_MODULE_SEN, "no free taskID!");
        return NULL;
    }
    SensorTaskAddTaskCfg(taskCfg);
    return taskCfg;
}

/* 创建Vsensor算法任务（同步接口，阻塞等待任务创建完成） */
int32_t SensorTaskCreateVsensorTask(const Vsensor *vsensor)
{
    SensorTaskContext *ctx = SensorTaskGetInstance();
    SensorTaskCfg *taskCfg = SensorTaskCreateVsensorTaskCfg(vsensor);
    CHECK_NULL_PTR_RETURN_VALUE(taskCfg, SENSOR_ERROR);
    int32_t ret = SensorCreateTaskAsync(taskCfg, vsensor->type);
    if (ret != SENSOR_OK) {
        SensorTaskRemoveTaskCfg(taskCfg);
        HILOG_ERROR(HILOG_MODULE_SEN, "[SYS] TaskSync create FAIL! ret:0x%X, type:%d", ret, vsensor->type);
        return SENSOR_TASK_CREATE_FAIL;
    }
    SensorUtilsRegisterTaskTable(vsensor->type, taskCfg->taskId);
    return SENSOR_OK;
}

/* 销毁vsensor任务 */
int32_t SensorTaskDestroyVsensorTask(enum SensorTypeTag type)
{
    SensorTaskCfg *sensorTask = SensorTaskFindTaskCfg(type);
    if (sensorTask == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SYS] vsensorTask not found type:%d", type);
        return SENSOR_ERROR;
    }
    SensorTaskDestoryTaskSync(sensorTask);
    SensorUtilsUnRegisterTaskTable(type, sensorTask->taskId);
    SensorTaskRemoveTaskCfg(sensorTask);
    return SENSOR_OK;
}

int32_t SensorTaskCreateHsensorTask(const Hsensor *hsensor)
{
    SensorTaskContext *ctx = SensorTaskGetInstance();
    SensorTaskCfg *taskCfg = SensorTaskCreateHsensorTaskCfg(hsensor);
    CHECK_NULL_PTR_RETURN_VALUE(taskCfg, SENSOR_ERROR);
    int32_t ret = SensorCreateTaskAsync(taskCfg, hsensor->type);
    if (ret != SENSOR_OK) {
        SensorTaskRemoveTaskCfg(taskCfg);
        HILOG_ERROR(HILOG_MODULE_SEN, "[SYS] TaskSync create FAIL! ret:0x%X, type:%d", ret, hsensor->type);
        return SENSOR_TASK_CREATE_FAIL;
    }
    HILOG_INFO(HILOG_MODULE_SEN, "[SYS] TaskSync create sensorTaskNum:%u, type:%d", ctx->sensorTaskNum, hsensor->type);
    return SENSOR_OK;
}

/* 销毁hsensor任务 */
int32_t SensorTaskDestroyHsensorTask(enum SensorTypeTag type)
{
    SensorTaskCfg *sensorTask = SensorTaskFindTaskCfg(type);
    if (sensorTask == NULL) {
        HILOG_DEBUG(HILOG_MODULE_SEN, "[SYS] hsensorTask not found type:%d", type);
        return SENSOR_ERROR;
    }
    SensorTaskDestoryTaskSync(sensorTask);
    SensorUtilsUnRegisterTaskTable(type, sensorTask->taskId);
    SensorTaskRemoveTaskCfg(sensorTask);
    return SENSOR_OK;
}
