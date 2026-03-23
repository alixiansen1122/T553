/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor msgqueue
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#ifndef SENSOR_MSGQUEUE_H
#define SENSOR_MSGQUEUE_H

#include <stdint.h>
#include "soc_osal.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    const char *name;
    uint16_t queueLen;
    uint16_t maxMsgSize;
    unsigned long queueId;
    uint32_t flags;
} SensorMsgQueueCfg;

int32_t SensorMsgQueueCreate(SensorMsgQueueCfg *cfg, uint32_t taskId);
int32_t SensorMsgQueueWriteCopy(unsigned int queueId, void *bufferAddr, unsigned int bufferSize, unsigned int timeout);
int32_t SensorMsgQueueReadCopy(unsigned int queueId, void *bufferAddr, unsigned int *bufferSize, unsigned int timeout);
void SensorMsgQueueDelete(unsigned int queueId);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SENSOR_MSGQUEUE_H */
