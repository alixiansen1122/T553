/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: vsensor mgr service
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#include "vsensormgr_service.h"
#include "securec.h"
#include "sensor_log.h"
#include "sensor_utils.h"
#include "sensor_memory.h"
#include "sensor_common.h"
#include "sensor_task.h"
#include "sensor_tmsg.h"
#include "sensor_para_decision.h"

typedef struct {
    struct osal_list_head list;
    const Vsensor *vsensor;
} VsensorInfo;

typedef struct {
    struct osal_list_head vsensorHead;
    uint32_t vsensorNum;
    bool inited;
} VsensorMgr;

static VsensorMgr g_vsensormgr = {0};

VsensorMgr *VsensorMgrGetInstance(void)
{
    return &g_vsensormgr;
}

int32_t VsensorMgrInit(void)
{
    VsensorMgr *vsensorMgr = VsensorMgrGetInstance();
    if (vsensorMgr->inited) {
        return SENSOR_OK;
    }
    OSAL_INIT_LIST_HEAD(&(vsensorMgr->vsensorHead));
    vsensorMgr->vsensorNum = 0;

    int32_t ret = SensorTaskInit();
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "SensorTask fail.\n");
        return SENSOR_ERROR;
    }
    vsensorMgr->inited = true;
    HILOG_INFO(HILOG_MODULE_SEN, "VsensorMgrInit OK.");
    return SENSOR_OK;
}

static int32_t NotifyVsensorTaskRegVsensor(const Vsensor *vsensor, int32_t taskId)
{
    SensorTmsg tmsgSend = {
        .id = SENSOR_TMSG_VSENSOR_REG_EVENT, .value = (uintptr_t)vsensor, .ts = 0, .info = NULL, .infoSize = 0};
    return SensorTmsgSend(taskId, &tmsgSend);
}

static const Vsensor *VsensorMgrFindVsensor(enum SensorTypeTag type)
{
    VsensorMgr *vsensorMgr = VsensorMgrGetInstance();

    HILOG_DEBUG(HILOG_MODULE_SEN, "vsensorNum:%u", vsensorMgr->vsensorNum);
    VsensorInfo *vsensorIndex = NULL;
    osal_list_for_each_entry(vsensorIndex, &vsensorMgr->vsensorHead, list)
    {
        HILOG_DEBUG(HILOG_MODULE_SEN, "vsensor type:%d", vsensorIndex->vsensor->type);
        if (vsensorIndex->vsensor->type == type) {
            return vsensorIndex->vsensor;
        }
    }
    return NULL;
}

static int32_t VsensorMgrSensorOpen(const Sensor *sensor, const SensorPara *openPara)
{
    const Vsensor *vsensor = VsensorMgrFindVsensor(sensor->sensorInfo.type);
    CHECK_NULL_PTR_RETURN_VALUE(vsensor, SENSOR_ERROR);
    uint32_t taskId = SensorUtilsGetRegisteredTaskId(sensor->sensorInfo.type);
    if (taskId == SENSOR_TASK_MAX_CNT) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SYS] not found taskId type:%d", vsensor->type);
        return SENSOR_ERROR;
    }
    /* 传递para 计算实际需要的存储空间 */
    uint32_t dataSize = (uint32_t)(sizeof(SensorPara));
    SensorPara *paraTemp = (SensorPara *)SensorMalloc(dataSize);
    if (paraTemp == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "cannot malloc for sensor:%d data!, taskId:%d", sensor->sensorInfo.type, taskId);
        return SENSOR_ERROR;
    }

    SensorParaCopy(paraTemp, openPara);

    SensorTmsg tmsgSend = {.id = SENSOR_TMSG_VSENSOR_OPEN_EVENT,
        .value = (uintptr_t)sensor->sensorInfo.type,
        .ts = 0,
        .info = paraTemp,
        .infoSize = dataSize};
    HILOG_INFO(HILOG_MODULE_SEN, "Open vsensor type: %d name: %s taskId:%u ", vsensor->type, vsensor->name, taskId);
    return SensorTmsgSend(taskId, &tmsgSend);
}

static int32_t VsensorMgrSensorClose(const Sensor *sensor)
{
    const Vsensor *vsensor = VsensorMgrFindVsensor(sensor->sensorInfo.type);
    CHECK_NULL_PTR_RETURN_VALUE(vsensor, SENSOR_ERROR);
    uint32_t taskId = SensorUtilsGetRegisteredTaskId(sensor->sensorInfo.type);
    if (taskId == SENSOR_TASK_MAX_CNT) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SYS] not found taskId type:%d", vsensor->type);
        return SENSOR_ERROR;
    }
    SensorTmsg tmsgSend = {
        .id = SENSOR_TMSG_VSENSOR_CLOSE_EVENT, .value = (uintptr_t)vsensor->type, .ts = 0, .info = NULL, .infoSize = 0};
    HILOG_INFO(HILOG_MODULE_SEN, "close vsensor type: %d name: %s taskId:%u ", vsensor->type, vsensor->name, taskId);
    return SensorTmsgSend(taskId, &tmsgSend);
}

static int32_t VsensorMgrSensorSetBatch(Sensor *sensor, int64_t samplingInterval, int64_t reportInterval)
{
    HILOG_WARN(HILOG_MODULE_SEN, "unsupport");
    return SENSOR_VSENSOR_NOT_SUPPORT;
}

static int32_t VsensorMgrSensorSetMode(Sensor *sensor, int32_t mode)
{
    HILOG_WARN(HILOG_MODULE_SEN, "unsupport");
    return SENSOR_VSENSOR_NOT_SUPPORT;
}

static int32_t VsensorMgrSensorSetOption(Sensor *sensor, uint32_t option)
{
    HILOG_WARN(HILOG_MODULE_SEN, "unsupport");
    return SENSOR_VSENSOR_NOT_SUPPORT;
}

static int32_t VsensorMgrSensorRegisterResultCallback(const Sensor *sensor, RecordDataCallback cb)
{
    const Vsensor *vsensor = VsensorMgrFindVsensor(sensor->sensorInfo.type);
    CHECK_NULL_PTR_RETURN_VALUE(vsensor, SENSOR_ERROR);
    return vsensor->registDataCb(cb);
}

static int32_t VsensorMgrSensorUnregisterResultCallback(const Sensor *sensor, RecordDataCallback cb)
{
    const Vsensor *vsensor = VsensorMgrFindVsensor(sensor->sensorInfo.type);
    CHECK_NULL_PTR_RETURN_VALUE(vsensor, SENSOR_ERROR);
    return vsensor->unRegistDataCb(cb);
}

static int32_t VsensorMgrSensorCalibrate(const Sensor *sensor, void *para, uint32_t len)
{
    HILOG_WARN(HILOG_MODULE_SEN, "unsupport");
    return SENSOR_VSENSOR_NOT_SUPPORT;
}

static int32_t VsensorMgrSensorSelfTest(const Sensor *sensor, void *para, uint32_t len)
{
    HILOG_WARN(HILOG_MODULE_SEN, "unsupport");
    return SENSOR_VSENSOR_NOT_SUPPORT;
}

static int32_t VsensorMgrSensorIoctl(const Sensor *sensor, SensorIoctlCmd cmd, void *data, uint32_t len)
{
    if (cmd >= SENSOR_IOCTL_MAX || cmd < SENSOR_IOCTL_SET_PARA) {
        HILOG_ERROR(HILOG_MODULE_SEN, "invalid command!");
        return SENSOR_ERROR;
    }
    const Vsensor *vsensor = VsensorMgrFindVsensor(sensor->sensorInfo.type);
    if (vsensor->ioctl == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "invalid ioctl function!");
        return SENSOR_ERROR;
    }
    return vsensor->ioctl(cmd, data, len);
}

static int32_t VsensorTaskRegister(const Vsensor *vsensor)
{
    int32_t ret = SensorTaskCreateVsensorTask(vsensor);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "vsensor type %d name %s CreateVsensorTask fail", vsensor->type, vsensor->name);
        return SENSOR_ERROR;
    }
    uint32_t taskId = SensorUtilsGetRegisteredTaskId(vsensor->type);
    if (taskId == SENSOR_TASK_MAX_CNT) {
        SensorTaskDestroyVsensorTask(vsensor->type);
        HILOG_ERROR(HILOG_MODULE_SEN, "[SYS] not found taskId type:%d", vsensor->type);
        return SENSOR_ERROR;
    }
    SensorUtilsRegisterVsensorType(vsensor->type);

    HILOG_DEBUG(HILOG_MODULE_SEN, "vsensor type:%d  taskId:%u", vsensor->type, taskId);
    ret = NotifyVsensorTaskRegVsensor(vsensor, taskId);
    if (ret != SENSOR_OK) {
        SensorTaskDestroyVsensorTask(vsensor->type);
        HILOG_ERROR(HILOG_MODULE_SEN, "[SYS] NotifyVsensorTaskRegVsensor failed type:%d ret:%d", vsensor->type, ret);
        return SENSOR_ERROR;
    }
    return SENSOR_OK;
}

static int32_t VsensorMgrSensorRegisterProc(const Vsensor *vsensor)
{
    Sensor *sensor = SensorUtilsRequestSensor(vsensor->type);
    if (sensor == NULL || sensor->sensorInfo.state != SENSOR_STATE_IDLE) {
        HILOG_ERROR(HILOG_MODULE_SEN, "sensor type %d name %s request fail.", vsensor->type, vsensor->name);
        return SENSOR_ERROR;
    }
    sensor->sensorInfo.type = vsensor->type;
    sensor->sensorInfo.name = vsensor->name;
    SensorParaDefaultInit(&sensor->sensorInfo.openPara);
    sensor->open = VsensorMgrSensorOpen;
    sensor->close = VsensorMgrSensorClose;
    sensor->setBatch = VsensorMgrSensorSetBatch;
    sensor->setMode = VsensorMgrSensorSetMode;
    sensor->setOption = VsensorMgrSensorSetOption;
    sensor->registerResultCallback = VsensorMgrSensorRegisterResultCallback;
    sensor->unregisterResultCallback = VsensorMgrSensorUnregisterResultCallback;
    sensor->calibrate = VsensorMgrSensorCalibrate;
    sensor->selfTest = VsensorMgrSensorSelfTest;
    sensor->ioctl = VsensorMgrSensorIoctl;
    HILOG_INFO(HILOG_MODULE_SEN, "register vsensor type:%d name:%s ", sensor->sensorInfo.type, sensor->sensorInfo.name);
    return SENSOR_OK;
}

int32_t VsensorMgrRegister(const Vsensor *vsensor)
{
    VsensorMgr *vsensorMgr = VsensorMgrGetInstance();
    if (!vsensorMgr->inited && (VsensorMgrInit() != SENSOR_OK)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "vsensorMgr inited failed.");
        return SENSOR_ERROR;
    }
    int32_t ret = VsensorTaskRegister(vsensor);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SYS] VsensorPostRegist failed type:%d", vsensor->type);
        return SENSOR_ERROR;
    }
    ret = SensorUtilsAddVsensorList(vsensor->type);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(
            HILOG_MODULE_SEN, "vsensor type %d name %s SensorUtilsAddSensorList fail", vsensor->type, vsensor->name);
        return SENSOR_ERROR;
    }
    ret = VsensorMgrSensorRegisterProc(vsensor);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN,
            "vsensor type %d name %s VsensorMgrSensorRegisterProc fail",
            vsensor->type,
            vsensor->name);
        return SENSOR_ERROR;
    }
    VsensorInfo *vsensorInfo = SensorMalloc(sizeof(VsensorInfo));
    CHECK_NULL_PTR_RETURN_VALUE(vsensorInfo, SENSOR_ERROR);
    OSAL_INIT_LIST_HEAD(&(vsensorInfo->list));
    vsensorInfo->vsensor = vsensor;
    osal_list_add(&vsensorInfo->list, &vsensorMgr->vsensorHead);
    vsensorMgr->vsensorNum++;
    HILOG_INFO(HILOG_MODULE_SEN, "register vsensor type %d name  %s success.", vsensor->type, vsensor->name);
    return SENSOR_OK;
}