/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor mgr
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#include "sensor_manager.h"
#include "sensor_common.h"
#include "sensor_utils.h"
#include "sensor_if.h"
#include "hsensormgr_service.h"
#include "vsensormgr_service.h"

int32_t SensorMgrInit(void)
{
    int32_t ret = HsensorMgrInit();
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "HsensorMgrInit err!", ret);
        return ret;
    }
    ret = VsensorMgrInit();
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "VsensorMgrInit err!", ret);
        return ret;
    }
    SensorUtilsInit();
    return SENSOR_OK;
}

int32_t SensorMgrDeInit(void)
{
    return SENSOR_OK;
}

int32_t SensorMgrRegistHsensor(const Hsensor *sensor)
{
    int32_t ret = SensorUtilsHsensorCheck(sensor);
    if (ret != SENSOR_OK) {
        HILOG_INFO(HILOG_MODULE_SEN, "hsensor:%d entry check err!", sensor->type);
        return ret;
    }
    return HsensorMgrRegister(sensor);
}

int32_t SensorMgrRegistVsensor(const Vsensor *sensor)
{
    int32_t ret = SensorUtilsVsensorCheck(sensor);
    if (ret != SENSOR_OK) {
        HILOG_INFO(HILOG_MODULE_SEN, "vsensor:%d entry check err!", sensor->type);
        return ret;
    }
    return VsensorMgrRegister(sensor);
}

int32_t SensorMgrGetAllSensors(struct SensorInformation **sensorInfo, int32_t *count)
{
    return SensorUtilsGetAllSensors(sensorInfo, count);
}

int32_t SensorMgrEnable(int32_t sensorTypeId)
{
    Sensor *sensor = SensorUtilsFindSensor(sensorTypeId);
    if (sensor == NULL || sensor->open == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[INF] sensor NULL!\n");
        return SENSOR_ENTRY_PARA_ERROR;
    }
    return sensor->open(sensor, &sensor->sensorInfo.openPara);
}

int32_t SensorMgrDisable(int32_t sensorTypeId)
{
    Sensor *sensor = SensorUtilsFindSensor(sensorTypeId);
    if (sensor == NULL || sensor->close == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[INF] sensor NULL!\n");
        return SENSOR_ENTRY_PARA_ERROR;
    }
    return sensor->close(sensor);
}

int32_t SensorMgrSetBatch(int32_t sensorTypeId, int64_t samplingInterval, int64_t reportInterval)
{
    Sensor *sensor = SensorUtilsFindSensor(sensorTypeId);
    if (sensor == NULL || sensor->setBatch == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[INF] sensor NULL!\n");
        return SENSOR_ENTRY_PARA_ERROR;
    }
    return sensor->setBatch(sensor, samplingInterval, reportInterval);
}

int32_t SensorMgrSetMode(int32_t sensorTypeId, int32_t mode)
{
    Sensor *sensor = SensorUtilsFindSensor(sensorTypeId);
    if (sensor == NULL || sensor->setMode == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[INF] sensor NULL!\n");
        return SENSOR_ENTRY_PARA_ERROR;
    }
    return sensor->setMode(sensor, mode);
}

int32_t SensorMgrSetOption(int32_t sensorTypeId, uint32_t option)
{
    Sensor *sensor = SensorUtilsFindSensor(sensorTypeId);
    if (sensor == NULL || sensor->setOption == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[INF] sensor NULL!\n");
        return SENSOR_ENTRY_PARA_ERROR;
    }
    return sensor->setOption(sensor, option);
}

int32_t SensorMgrRegistDataCallback(int32_t sensorTypeId, RecordDataCallback cb)
{
    Sensor *sensor = SensorUtilsFindSensor(sensorTypeId);
    CHECK_NULL_PTR_RETURN_VALUE(sensor, SENSOR_ENTRY_PARA_ERROR);
    CHECK_NULL_PTR_RETURN_VALUE(sensor->registerResultCallback, SENSOR_ENTRY_PARA_ERROR);
    return sensor->registerResultCallback(sensor, cb);
}

int32_t SensorMgrUnregistDataCallback(int32_t sensorTypeId, RecordDataCallback cb)
{
    Sensor *sensor = SensorUtilsFindSensor(sensorTypeId);
    CHECK_NULL_PTR_RETURN_VALUE(sensor, SENSOR_ENTRY_PARA_ERROR);
    CHECK_NULL_PTR_RETURN_VALUE(sensor->unregisterResultCallback, SENSOR_ENTRY_PARA_ERROR);
    return sensor->unregisterResultCallback(sensor, cb);
}

int32_t SensorMgrGetData(int32_t sensorTypeId, SensorData *sensorData)
{
    Sensor *sensor = SensorUtilsFindSensor(sensorTypeId);
    CHECK_NULL_PTR_RETURN_VALUE(sensor, SENSOR_ENTRY_PARA_ERROR);
    CHECK_NULL_PTR_RETURN_VALUE(sensor->ioctl, SENSOR_ENTRY_PARA_ERROR);
    return sensor->ioctl(sensor, SENSOR_IOCTL_GET_SENSOR_DATA, sensorData, sizeof(SensorData));
}