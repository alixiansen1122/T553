/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor para
 * Author: Media Software Group
 * Create: 2025-06-06
 */
#ifndef SENSOR_PARA_DECISION_H
#define SENSOR_PARA_DECISION_H

#include "sensor_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SENSOR_PARA_REALTIME_PERIOD_MIN 1
#define SENSOR_PARA_REALTIME_PERIOD_MAX 1000000
#define SENSOR_PARA_BATCH_MIN 1
#define SENSOR_PARA_BATCH_MAX 100

void SensorParaDefaultInit(SensorPara *sensorPara);
void SensorParaClear(SensorPara *para);
void SensorParaCopy(SensorPara *dest, const SensorPara *src);
int32_t SensorOpenParaDecision(
    enum SensorTypeTag type, const SensorPara *cur, const SensorPara *require, SensorPara *result);
int32_t SensorOptionDecision(enum SensorTypeTag type, int32_t cur, int32_t require);
bool SensorParaIsClear(const SensorPara *para);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SENSOR_PARA_DECISION_H */