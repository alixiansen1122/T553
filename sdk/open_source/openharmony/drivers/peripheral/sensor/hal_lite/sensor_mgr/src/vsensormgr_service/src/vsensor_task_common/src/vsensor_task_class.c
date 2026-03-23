/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: vsensor task class impl
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#include "vsensor_task_class.h"
#include "securec.h"
#include "sensor_log.h"
#include "sensor_common.h"
#include "sensor_utils.h"
#include "sensor_memory.h"
#include "sensor_tmsg.h"

/* 存储 taskid 和 aclass 对象的对应表 */
static VsensorTaskClass *g_vsensorTaskObjArray[SENSOR_TASK_MAX_CNT] = {NULL};

/* aclass对象创建 */
VsensorTaskClass *VsensorTaskClassInit(uint32_t taskId, const char *taskPrefix, NotifyCallback hdataCb,
    const SensorTmsgProcMap *tmsgProcMap, uint16_t tmsgProcMapLen)
{
    if (taskPrefix == NULL) {
        return NULL;
    }

    VsensorTaskClass *vsensorTaskObj = (VsensorTaskClass *)SensorMalloc(sizeof(VsensorTaskClass));
    if (vsensorTaskObj == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "aclass:%d malloc fail!\n", taskId);
        return NULL;
    }

    memset_s(vsensorTaskObj, sizeof(VsensorTaskClass), 0, sizeof(VsensorTaskClass));

    vsensorTaskObj->component.taskId = taskId;
    vsensorTaskObj->component.taskPrefix = (char *)taskPrefix;
    vsensorTaskObj->component.hdataCb = hdataCb;
    vsensorTaskObj->tmsgProcMap = (SensorTmsgProcMap *)tmsgProcMap;
    vsensorTaskObj->tmsgProcMapLen = tmsgProcMapLen;
    VsensorTaskCommInit(&(vsensorTaskObj->component));
    g_vsensorTaskObjArray[taskId] = vsensorTaskObj;

    return vsensorTaskObj;
}

/* aclass对象销毁 */
void VsensorTaskClassDestruct(VsensorTaskClass *vSensorClass)
{
    if (vSensorClass == NULL) {
        return;
    }
    g_vsensorTaskObjArray[vSensorClass->component.taskId] = NULL;
    SensorFree(vSensorClass);
}

/* 通过 taskid 查找 aclass 对象 */
VsensorTaskClass *VsensorTaskClassGetByTaskid(uint32_t taskId)
{
    if (taskId < SENSOR_TASK_MAX_CNT) {
        return g_vsensorTaskObjArray[taskId];
    } else {
        return NULL;
    }
}

/* aclass类公用的消息处理函数，当aclass对象没有指定消息处理函数时，会使用此函数的消息处理流程 */
static int32_t VsensorTaskGenericMsgProcess(VsensorTaskComm *component, const SensorTmsg *tmsg)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "msg:%d task:%d proc", tmsg->id, component->taskId);
    /* 无需多重校验入参指针，在函数上下文中，已经判断了，提升代码运行效率 */
    int32_t ret = SENSOR_OK;
    switch (tmsg->id) {
        case SENSOR_TMSG_VSENSOR_REG_EVENT: {
            Vsensor *vsensor = (Vsensor *)(tmsg->value);
            CHECK_NULL_PTR_RETURN_VALUE(vsensor, SENSOR_VSENSOR_TASK_REG_VENSOR_NULL);
            ret = VsensorTaskVsensorRegEvent(component, (Vsensor *)(tmsg->value));
            break;
        }
        case SENSOR_TMSG_HSENSOR_DATA_UPDATE:
        case SENSOR_TMSG_VSENSOR_DATA_UPDATE: {
            ret = VsensorTaskSensorDataUpdateEvent(component, (VsensorProcessData *)(tmsg->info));
            break;
        }
        case SENSOR_TMSG_VSENSOR_OPEN_EVENT: {
            ret = VsensorTaskVsensorOpenEvent(
                component, (enum SensorTypeTag)(tmsg->value), (SensorPara *)(tmsg->info), APP_CTRL);
            break;
        }
        case SENSOR_TMSG_VSENSOR_CLOSE_EVENT: {
            ret = VsensorTaskVsensorCloseEvent(component, (enum SensorTypeTag)(tmsg->value), APP_CTRL);
            break;
        }
        case SENSOR_TMSG_VSENSOR_RELY_OPEN_EVENT: {
            ret = VsensorTaskVsensorOpenEvent(
                component, (enum SensorTypeTag)(tmsg->value), (SensorPara *)(tmsg->info), RELY_CTRL);
            break;
        }
        case SENSOR_TMSG_VSENSOR_RELY_CLOSE_EVENT: {
            ret = VsensorTaskVsensorCloseEvent(component, (enum SensorTypeTag)(tmsg->value), RELY_CTRL);
            break;
        }
        default:
            HILOG_INFO(HILOG_MODULE_SEN, "cannot get msg:%d task:%d proc.\n", tmsg->id, component->taskId);
    }
    HILOG_DEBUG(HILOG_MODULE_SEN, "msg:%d task:%d proc ret:%d", tmsg->id, component->taskId, ret);
    return ret;
}

/* aclass类公用的消息处理函数 */
int32_t VsensorTaskClassMsgProcess(VsensorTaskClass *vSensorClass, const SensorTmsg *tmsg)
{
    VsensorTaskComm *component = &(vSensorClass->component);

    CHECK_NULL_PTR_RETURN_VALUE(vSensorClass, SENSOR_VSENSOR_TASK_CLASS_NULL);
    int32_t ret = VsensorTaskCheckCommComponents(component);
    SENSOR_CHECK_OK(ret);

    if (tmsg == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "VsensorTaskClassMsgProcess tmsg NULL\r\n");
        return SENSOR_RECV_TMSG_NULL;
    }

    if (vSensorClass->tmsgProcMap == NULL || vSensorClass->tmsgProcMapLen == 0) {
        return VsensorTaskGenericMsgProcess(component, tmsg);
    }

    for (uint32_t i = 0; i < vSensorClass->tmsgProcMapLen; i++) {
        if (vSensorClass->tmsgProcMap[i].tmsgId == tmsg->id && vSensorClass->tmsgProcMap[i].tmsgProc != NULL) {
            return vSensorClass->tmsgProcMap[i].tmsgProc(tmsg);
        }
    }
    return VsensorTaskGenericMsgProcess(component, tmsg);
}