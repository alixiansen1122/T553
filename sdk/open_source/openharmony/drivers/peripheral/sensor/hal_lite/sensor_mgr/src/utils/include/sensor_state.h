/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor state
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#ifndef SENSOR_STATE_H
#define SENSOR_STATE_H

#include "sensor_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void SensorSetState(enum SensorTypeTag type, SensorState state);
void SensorSetPara(enum SensorTypeTag type, const SensorPara *para);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SENSOR_STATE_H */
