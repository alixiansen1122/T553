/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor data service
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#ifndef HSENSOR_DATA_SERVICE_H
#define HSENSOR_DATA_SERVICE_H

#include "sensor_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int32_t HsensorDataServiceInit(enum SensorTypeTag type);
uint32_t HsensorDataServiceProcess(enum SensorTypeTag type);
int32_t HsensorDataServiceDeInit(enum SensorTypeTag type);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HSENSOR_DATA_SERVICE_H */