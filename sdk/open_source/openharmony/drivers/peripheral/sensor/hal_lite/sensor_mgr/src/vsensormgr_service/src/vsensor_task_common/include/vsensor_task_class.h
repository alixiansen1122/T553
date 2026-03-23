/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: vsensor task class
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#ifndef VSENSOR_TASK_CLASS_H
#define VSENSOR_TASK_CLASS_H

#include "vsensor_task_comm.h"

typedef struct {
    VsensorTaskComm component;
    SensorTmsgProcMap *tmsgProcMap;
    uint16_t tmsgProcMapLen;
} VsensorTaskClass;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* aclass对象创建 */
VsensorTaskClass *VsensorTaskClassInit(uint32_t taskId, const char *taskPrefix, NotifyCallback hdataCb,
    const SensorTmsgProcMap *tmsgProcMap, uint16_t tmsgProcMapLen);

/* aclass对象销毁 */
void VsensorTaskClassDestruct(VsensorTaskClass *vSensorClass);

/* 通过 taskid 查找 aclass 对象 */
VsensorTaskClass *VsensorTaskClassGetByTaskid(uint32_t taskId);

/* aclass类公用的消息处理函数 */
int32_t VsensorTaskClassMsgProcess(VsensorTaskClass *vSensorClass, const SensorTmsg *tmsg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif