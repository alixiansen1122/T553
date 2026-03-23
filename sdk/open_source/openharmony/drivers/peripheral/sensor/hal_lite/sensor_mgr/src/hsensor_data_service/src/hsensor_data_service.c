/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor data service impl
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#include "hsensor_data_service.h"
#include "sensor_common.h"
#include "sensor_utils.h"
#include "sensor_log.h"
#include "hsensormgr_service.h"

struct SensorEvents g_sensorEvent = {};

int32_t HsensorDataServiceInit(enum SensorTypeTag type)
{
    return SENSOR_OK;
}

/* ******************************************************************************
  函 数 名		:  HsensorDataServiceNotifyResultData
  功能描述		:  数据分发，目前尚未做降采样，直接将所有数据上报，
                  目前将所有数据的batch和采样率设置为一样，等待缓冲区和降采样完成
  输入参数		:  None
  输出参数		:  None
  返 回 值		:  None
****************************************************************************** */
static void HsensorDataServiceNotifyResultData(HsensorScb *hsensorScb, HsensorListener *listener)
{
    if (listener->resultDataCb == NULL) {
        return;
    }
    g_sensorEvent.sensorId = hsensorScb->type;
    g_sensorEvent.version = 0;
    g_sensorEvent.timestamp = hsensorScb->sysTick;
    g_sensorEvent.option = hsensorScb->decisionPara.option;
    g_sensorEvent.mode = hsensorScb->decisionPara.mode;
    g_sensorEvent.data = hsensorScb->dataBuf;
    g_sensorEvent.dataLen = hsensorScb->dataPackSize;
    HILOG_DEBUG(HILOG_MODULE_SEN, "hsensorScb->type:%d data:%x", hsensorScb->type, hsensorScb->dataBuf);

    listener->resultDataCb(&g_sensorEvent);
}

static void HsensorDataServiceNotifyData(HsensorScb *hsensorScb, HsensorListener *listener)
{
    if (listener->notifyCallback == NULL) {
        return;
    }
    SensorNotifyInfo notifyInfo = {
        .type = hsensorScb->type,
        .data = hsensorScb->dataBuf,
        .dataLen = hsensorScb->dataPackSize,
        .option = hsensorScb->decisionPara.option,
        .timestamp = hsensorScb->sysTick,
    };
    listener->notifyCallback(&notifyInfo, listener->notifyArg);
}

static void HsensorDataServiceNotifyAll(HsensorScb *hsensorScb)
{
    HsensorListener *listenerIndex = NULL;
    osal_list_for_each_entry(listenerIndex, &hsensorScb->listenerList, list)
    {
        if (listenerIndex->status != SENSOR_LISTENER_WORKING) {
            continue;
        }
        HsensorDataServiceNotifyResultData(hsensorScb, listenerIndex);
        HsensorDataServiceNotifyData(hsensorScb, listenerIndex);
    }
}

uint32_t HsensorDataServiceProcess(enum SensorTypeTag type)
{
    HsensorScb *hsensorScb = HsensorMgrHsensorScbFind(type);
    uint32_t samplingInterval = hsensorScb->decisionPara.period;
    CHECK_NULL_PTR_RETURN_VALUE(hsensorScb, samplingInterval);
    CHECK_NULL_PTR_RETURN_VALUE(hsensorScb->batchDataBuf, samplingInterval);
    HILOG_DEBUG(HILOG_MODULE_SEN,
        "hsensor type:%d period:%u batch:%u option:%u mode:%d reportPeriod:%u",
        type,
        hsensorScb->decisionPara.period,
        hsensorScb->decisionPara.batch,
        hsensorScb->decisionPara.option,
        hsensorScb->decisionPara.mode,
        hsensorScb->reportPeriod);
    int32_t dataLen = hsensorScb->hsensor->read(hsensorScb->batchDataBuf, hsensorScb->batchDataBufSize);
    if (dataLen <= 0) {
        HILOG_WARN(HILOG_MODULE_SEN, "type: %d read data fail.", type);
        return samplingInterval;
    }
    hsensorScb->batchCount++;
    hsensorScb->dataPackSize += dataLen;
    hsensorScb->batchDataBuf += dataLen;
    hsensorScb->batchDataBufSize -= dataLen;
    HILOG_DEBUG(HILOG_MODULE_SEN, "batchCount: %d dataLen: %d", hsensorScb->batchCount, dataLen);
    if (hsensorScb->batchCount < hsensorScb->decisionPara.batch) {
        return samplingInterval;
    }
    hsensorScb->sysTick = SensorUtilsGetRtcTimeMs();
    HsensorDataServiceNotifyAll(hsensorScb);
    hsensorScb->batchDataBuf = hsensorScb->dataBuf;
    hsensorScb->batchDataBufSize = hsensorScb->dataBufSize;
    hsensorScb->dataPackSize = 0;
    hsensorScb->batchCount = 0;
    return samplingInterval;
}

int32_t HsensorDataServiceDeInit(enum SensorTypeTag type)
{
    return SENSOR_OK;
}