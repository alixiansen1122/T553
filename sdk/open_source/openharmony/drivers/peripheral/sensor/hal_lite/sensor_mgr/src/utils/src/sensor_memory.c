/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Sensor Malloc impl
 * Create: 2025-6-6
 */

#include "sensor_memory.h"
#include "sensor_log.h"

void *SensorMalloc(unsigned long size)
{
    return malloc(size);
}

void SensorFree(void *addr)
{
    if (addr == NULL) {
        return;
    }
    free(addr);
}
