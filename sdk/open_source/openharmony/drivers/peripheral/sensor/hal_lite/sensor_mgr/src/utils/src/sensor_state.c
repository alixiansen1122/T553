/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor state impl
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#include "sensor_state.h"
#include "sensor_log.h"
#include "sensor_utils.h"
#include "sensor_common.h"

void SensorSetState(enum SensorTypeTag type, SensorState state)
{
    Sensor *sensor = NULL;

    sensor = SensorUtilsFindSensor(type);
    if (sensor == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[STATE] cannot get sensor:%d, sensor may be not registered\n", type);
        return;
    }

    sensor->sensorInfo.state = state;
}

void SensorSetPara(enum SensorTypeTag type, const SensorPara *para)
{
    Sensor *sensor = NULL;
    sensor = SensorUtilsFindSensor(type);
    if (sensor == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[STATE] cannot get sensor:%d, sensor may be not registered\n", type);
        return;
    }

    if (para == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[ATASK] sensor open para NULL!");
        return;
    }
    sensor->sensorInfo.openPara.period = para->period;
    sensor->sensorInfo.openPara.batch = para->batch;
    sensor->sensorInfo.openPara.mode = para->mode;
    sensor->sensorInfo.openPara.option = para->option;

    HILOG_INFO(HILOG_MODULE_SEN,
        "[UTIL][storge msg] period:%d batch:%d mode:%d option:%d\n",
        sensor->sensorInfo.openPara.period,
        sensor->sensorInfo.openPara.batch,
        sensor->sensorInfo.openPara.mode,
        sensor->sensorInfo.openPara.option);
}