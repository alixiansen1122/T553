/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor mgr
 * Author: Media Software Group
 * Create: 2025-06-06
 */


#ifndef HDI_SENSOR_MANAGER_H
#define HDI_SENSOR_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "sensor_typedef.h"

int32_t SensorMgrInit(void);
int32_t SensorMgrDeInit(void);

int32_t SensorMgrRegistHsensor(const Hsensor *sensor);
int32_t SensorMgrRegistVsensor(const Vsensor *sensor);

int32_t SensorMgrGetAllSensors(struct SensorInformation **sensorInfo, int32_t *count);

int32_t SensorMgrEnable(int32_t sensorTypeId);

int32_t SensorMgrDisable(int32_t sensorTypeId);

int32_t SensorMgrSetBatch(int32_t sensorTypeId, int64_t samplingInterval, int64_t reportInterval);

int32_t SensorMgrSetMode(int32_t sensorTypeId, int32_t mode);

int32_t SensorMgrSetOption(int32_t sensorTypeId, uint32_t option);

int32_t SensorMgrRegistDataCallback(int32_t sensorTypeId, RecordDataCallback cb);

int32_t SensorMgrUnregistDataCallback(int32_t sensorTypeId, RecordDataCallback cb);

#endif /* HDI_SENSOR_MANAGER_H */
