/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor msgqueue impl
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#include "sensor_msgqueue.h"
#include "sensor_log.h"

int32_t SensorMsgQueueCreate(SensorMsgQueueCfg* cfg, uint32_t taskId)
{
    if (cfg == NULL) {
        return 0;
    }
    HILOG_INFO(HILOG_MODULE_SEN, "task:%u create msgqueue.\n", taskId);
    return osal_msg_queue_create(cfg->name, cfg->queueLen, &(cfg->queueId), cfg->flags, cfg->maxMsgSize);
}

int32_t SensorMsgQueueWriteCopy(unsigned int queueId, void *bufferAddr, unsigned int bufferSize, unsigned int timeout)
{
    return osal_msg_queue_write_copy(queueId, bufferAddr, bufferSize, timeout);
}

int32_t SensorMsgQueueReadCopy(unsigned int queueId, void *bufferAddr, unsigned int *bufferSize, unsigned int timeout)
{
    return osal_msg_queue_read_copy(queueId, bufferAddr, bufferSize, timeout);
}

void SensorMsgQueueDelete(unsigned int queueId)
{
    osal_msg_queue_delete(queueId);
}