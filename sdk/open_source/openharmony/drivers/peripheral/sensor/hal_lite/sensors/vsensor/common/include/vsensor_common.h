/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: vsensor common
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#ifndef HDI_V_SENSOR_COMMON_H
#define HDI_V_SENSOR_COMMON_H

#include <stdio.h>
#include "sensor_typedef.h"
#include "sensor_log.h"
#include "soc_osal.h"

typedef struct {
    struct osal_list_head list;          /* link point */
    RecordDataCallback dataCb;
} DataCallbackInfo;

#endif /* HDI_V_SENSOR_COMMON_H */
