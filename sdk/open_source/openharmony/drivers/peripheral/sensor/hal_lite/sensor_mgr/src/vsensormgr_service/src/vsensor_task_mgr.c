/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: vsensor task mgr impl
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#include "vsensor_task_mgr.h"
#include "vsensor_task_class.h"
#include "sensor_common.h"
#include "sensor_log.h"
#include "sensor_tmsg.h"

/* 任务初始化 */
int32_t VsensorTaskMgrInit(uint32_t taskId)
{
    VsensorTaskClass* aclassMotion = VsensorTaskClassInit(taskId, "VSENSOR_", NULL, NULL, 0);
    if (aclassMotion == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "extern alg task create ERR!\n");
        return SENSOR_ERROR;
    }

    return SENSOR_OK;
}

/* 任务销毁 */
void VsensorTaskMgrDeInit(uint32_t taskId)
{
    VsensorTaskClass* vSensorClass = VsensorTaskClassGetByTaskid(taskId);
    VsensorTaskClassDestruct(vSensorClass);
}

/* 任务消息处理函数入口 */
int32_t VsensorTaskMgrMsgProcess(uint32_t taskId, const SensorTmsg *tmsg)
{
    VsensorTaskClass* vSensorClass = VsensorTaskClassGetByTaskid(taskId);
    if (vSensorClass == NULL) {
        return SENSOR_VSENSOR_TASK_CLASS_NULL;
    }

    return VsensorTaskClassMsgProcess(vSensorClass, tmsg);
}
