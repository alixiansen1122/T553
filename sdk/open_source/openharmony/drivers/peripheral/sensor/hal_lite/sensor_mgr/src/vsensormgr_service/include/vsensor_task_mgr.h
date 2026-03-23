/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: vsensor task mgr
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#ifndef VENSOR_TASK_MGR_H
#define VENSOR_TASK_MGR_H

#include <stdbool.h>
#include "sensor_typedef.h"
#include "sensor_tmsg.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int32_t VsensorTaskMgrInit(uint32_t taskId);
void VsensorTaskMgrDeInit(uint32_t taskId);
int32_t VsensorTaskMgrMsgProcess(uint32_t taskId, const SensorTmsg *tmsg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
