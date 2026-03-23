/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor memory
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#ifndef SENSOR_MEMORY_H
#define SENSOR_MEMORY_H

#include <stdlib.h>

#define MEM_ALLOC_MIN_SIZE     1
#define MEM_ALLOC_MAX_SIZE     10000  // ????10k

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void *SensorMalloc(unsigned long size);
void SensorFree(void *addr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

 
#endif /* SENSOR_MEMORY_H */
