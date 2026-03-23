/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: hsensor mgr impl
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#include "hsensormgr_service.h"
#include <string.h>
#include <stdbool.h>
#include "securec.h"
#include "sensor_log.h"
#include "sensor_common.h"
#include "sensor_memory.h"
#include "sensor_tmsg.h"
#include "sensor_utils.h"
#include "sensor_para_decision.h"

#define HSENSOR_SCB_MAX_NUM 10
#define HSENSORMGR_MAX_DATA_LEN (1024 * 1024)
static HsensorMgr g_hsensormgr = {0};
static HsensorScb g_hsensorScb[HSENSOR_SCB_MAX_NUM] = {0};

static HsensorMgr *HsensorMgrGetInstance(void)
{
    return &g_hsensormgr;
}

static int32_t HsensorScbMallocDataBuffer(HsensorScb *hsensorScb)
{
    if (hsensorScb == NULL) {
        return SENSOR_ERROR;
    }
    uint32_t dataBufSize = hsensorScb->hsensor->caps.sampleElementlen * hsensorScb->decisionPara.batch;
    if (dataBufSize >= HSENSORMGR_MAX_DATA_LEN) {
        HILOG_ERROR(HILOG_MODULE_SEN, "hsensorScb dataBufSize :%u is invalid.", dataBufSize);
        return SENSOR_ERROR;
    }
    hsensorScb->dataBuf = SensorMalloc(dataBufSize);
    if (hsensorScb->dataBuf == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "hsensorScb MallocDataBuffer NULL.");
        return SENSOR_ERROR;
    }
    hsensorScb->batchDataBuf = hsensorScb->dataBuf;
    hsensorScb->batchDataBufSize = dataBufSize;
    hsensorScb->dataBufSize = dataBufSize;
    return SENSOR_OK;
}

static int32_t HsensorScbFreeDataBuffer(HsensorScb *hsensorScb)
{
    if (hsensorScb == NULL) {
        return SENSOR_ERROR;
    }
    if (hsensorScb->dataBuf == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "hsensorScb cDataBuffer NULL.");
        return SENSOR_ERROR;
    }
    SensorFree(hsensorScb->dataBuf);
    hsensorScb->dataBuf = NULL;
    hsensorScb->batchDataBuf = NULL;
    hsensorScb->batchDataBufSize = 0;
    hsensorScb->dataBufSize = 0;
    hsensorScb->dataPackSize = 0;
    return SENSOR_OK;
}

static void HsensorScbInit(HsensorScb *hsensorScb)
{
    if (hsensorScb == NULL) {
        return;
    }
    OSAL_INIT_LIST_HEAD(&(hsensorScb->list));
    OSAL_INIT_LIST_HEAD(&(hsensorScb->listenerList));
    hsensorScb->listenerNum = 0;
    hsensorScb->type = SENSOR_TYPE_NONE;
    hsensorScb->status = HSENSOR_SCB_UNINITED;
    hsensorScb->dataBuf = NULL;
    hsensorScb->dataBufSize = 0;
    hsensorScb->dataBufIndex = 0;
    hsensorScb->dataPackSize = 0;
    hsensorScb->batchCount = 0;
    SensorPara initDecisionPara = {0};
    hsensorScb->decisionPara = initDecisionPara;
    hsensorScb->hsensor = NULL;
}

static HsensorScb *HsensorScbRequest(uint16_t scbNum)
{
    return (scbNum >= HSENSOR_SCB_MAX_NUM) ? NULL : (&g_hsensorScb[scbNum]);
}

HsensorScb *HsensorMgrHsensorScbFind(const enum SensorTypeTag type)
{
    HsensorMgr *hsensorMgr = HsensorMgrGetInstance();
    HsensorScb *hsensorScbIndex = NULL;
    osal_list_for_each_entry(hsensorScbIndex, &hsensorMgr->hsensorScbHead, list)
    {
        if (type == hsensorScbIndex->type) {
            return hsensorScbIndex;
        }
    }
    return NULL;
}

int32_t HsensorMgrInit(void)
{
        HILOG_DEBUG(HILOG_MODULE_SEN, "in.");
    HsensorMgr *hsensorMgr = HsensorMgrGetInstance();
    if (hsensorMgr->inited) {
        return SENSOR_OK;
    }
    OSAL_INIT_LIST_HEAD(&(hsensorMgr->hsensorScbHead));
    hsensorMgr->hsensorScbNum = 0;
    hsensorMgr->dataBuf = NULL;
    hsensorMgr->dataDispatchBuf = NULL;
    for (uint16_t i = 0; i < HSENSOR_SCB_MAX_NUM; i++) {
        HsensorScb *hsensorScb = HsensorScbRequest(i);
        HsensorScbInit(hsensorScb);
    }
    hsensorMgr->inited = true;
        HILOG_DEBUG(HILOG_MODULE_SEN, "out.");
    return SENSOR_OK;
}

/* ******************************************************************************
  函 数 名		:  HsensormgrCheckListener
  功能描述		:  检查listener是否存在于该scb中
  输入参数		:  None
  输出参数		:  None
  返 回 值		:  None
****************************************************************************** */
static int32_t HsensormgrCheckListener(const HsensorScb *hsensorScb, const HsensorListener *listener)
{
    HsensorListener *listenerIndex = NULL;
    osal_list_for_each_entry(listenerIndex, &hsensorScb->listenerList, list)
    {
        if (memcmp(listenerIndex->name, listener->name, (uint32_t)HSENSOR_LISTENER_NAME_MAX_LEN) == 0) {
            return SENSOR_OK;
        }
    }
    return SENSOR_ERROR;
}

int32_t HsensorMgrRegisterListener(HsensorListener *listener)
{
    HsensorScb *hsensorScb = HsensorMgrHsensorScbFind(listener->type);
    if (hsensorScb == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type %d hsensorScb get fail.", listener->type);
        return SENSOR_HSENSOR_SCB_NULL;
    }
    if (HsensormgrCheckListener(hsensorScb, listener) == SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "hsensorListener name  %s already exits.", listener->name);
        return SENSOR_HSENSOR_SCB_LISTENER_EXIST;
    }

    osal_list_add(&listener->list, &hsensorScb->listenerList);
    hsensorScb->listenerNum++;
    listener->status = SENSOR_LISTENER_REGISTER;
    HILOG_DEBUG(HILOG_MODULE_SEN,
        "hsensorListener name: %s type:%d listenerNum:%u",
        listener->name,
        listener->type,
        hsensorScb->listenerNum);
    return SENSOR_OK;
}

int32_t HsensorMgrUnRegisterListener(HsensorListener *listener)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "UnRegister hsensor type %d listener name:%s.",
                listener->type, listener->name);
    /* 表示vsensor 注册的 Listener 不需要 UnRegister */
    if (listener->notifyCallback != NULL) {
        return SENSOR_OK;
    }
    HsensorScb *hsensorScb = HsensorMgrHsensorScbFind(listener->type);
    if (hsensorScb == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type %d hsensorScb get fail.", listener->type);
        return SENSOR_HSENSOR_SCB_NULL;
    }
    if (HsensormgrCheckListener(hsensorScb, listener) != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "hsensorListener name  %s not exits.", listener->name);
        return SENSOR_HSENSOR_SCB_LISTENER_NOT_EXIST;
    }

    osal_list_del(&listener->list);
    hsensorScb->listenerNum--;
    listener->status = SENSOR_LISTENER_UNREGISTER;
    SensorUtilsFreeListener(listener);
    HILOG_DEBUG(HILOG_MODULE_SEN, "UnRegister listenerNum: :%u", hsensorScb->listenerNum);
    return SENSOR_OK;
}

static void HsensormgrCheckOpenParam(SensorPara *openParam)
{
    if (openParam->period == 0) {
        openParam->period = SENSOR_PARA_REALTIME_PERIOD_MIN;
    }
    if (openParam->batch == 1 || openParam->batch == 0) {
        openParam->batch = 1;
        openParam->mode = SENSOR_MODE_REALTIME;
    }
    if (openParam->mode == SENSOR_MODE_REALTIME && (openParam->period < SENSOR_PARA_REALTIME_PERIOD_MIN)) {
        openParam->period = SENSOR_PARA_REALTIME_PERIOD_MIN;
    }
}

static void HsensormgrOpenCheckOpenParam(HsensorListener *listener, SensorPara *openParam, uint32_t *reportPeriod)
{
    HsensormgrCheckOpenParam(openParam);
    listener->notifyPeriod = openParam->period * openParam->batch;
    *reportPeriod = listener->notifyPeriod;
}

/* ******************************************************************************
  函 数 名		:  HsensormgrCalculateGCD
  功能描述		:  计算最大公约数
  输入参数		:  None
  输出参数		:  None
  返 回 值		:  None
****************************************************************************** */
static uint32_t HsensormgrCalculateGCD(uint32_t a, uint32_t b)
{
    uint32_t c;
    while (b) {
        c = b;
        b = a % b;
        a = c;
    }
    return a;
}

/* ******************************************************************************
  函 数 名		:  HsensormgrUpdateOpenParam
  功能描述		:  更新 scb的 openParam ，遍历所有enable的listener
  输入参数		:  None
  输出参数		:  None
  返 回 值		:  None
****************************************************************************** */
static bool HsensormgrUpdateOpenParam(HsensorScb *hsensorScb, SensorPara *openParam, uint32_t reportPeriod)
{
    uint8_t openMode = 0;
    uint32_t reportPeriodGcd = reportPeriod;
    uint32_t notifyPeriodTemp = 0;
    bool needOpen = false;
    uint32_t periodMin = openParam->period;
    uint32_t notifyPeriodMin = reportPeriod;
    HsensorListener *listenerIndex = NULL;
    osal_list_for_each_entry(listenerIndex, &hsensorScb->listenerList, list)
    {
        HsensorMgrPrintListener(listenerIndex->type);
        if (listenerIndex->status == SENSOR_LISTENER_WORKING || listenerIndex->status == SENSOR_LISTENER_ENABLE) {
            openMode |= (1 << listenerIndex->openParam.mode);
            periodMin = (periodMin > listenerIndex->openParam.period) ? listenerIndex->openParam.period : periodMin;
            notifyPeriodMin =
                (notifyPeriodMin > listenerIndex->notifyPeriod) ? listenerIndex->notifyPeriod : notifyPeriodMin;
            if (needOpen == false) {
                reportPeriodGcd = listenerIndex->notifyPeriod;
            }
            reportPeriodGcd = HsensormgrCalculateGCD(reportPeriodGcd, notifyPeriodTemp);
            notifyPeriodTemp = notifyPeriodMin;
            needOpen = true;
        }
    }
    if (openMode & (1 << SENSOR_MODE_REALTIME)) {
        openParam->mode = SENSOR_MODE_REALTIME;
    } else {
        openParam->mode = SENSOR_MODE_FIFO_MODE;
    }
    openParam->period = periodMin;
    if (periodMin != 0) {
        openParam->batch = reportPeriodGcd / periodMin;
    } else {
        openParam->batch = 0;
    }
    HILOG_DEBUG(HILOG_MODULE_SEN, "needOpen: %d", needOpen);
    return needOpen;
}

/* ******************************************************************************
  函 数 名		:  HsensormgrUpdateListener
  功能描述		:  更新 listener ，并决定是否重新打开 sensor
                  1. 如果不重启sensor，只需要更新该listener的相关参数即可
                  2. 如果需要重启sensor，则需要更新所有listener的相关参数
                  参考6.22未合入的提交
  输入参数		:  None
  输出参数		:  None
  返 回 值		:  None
****************************************************************************** */
int32_t HsensormgrUpdateListener(HsensorScb *hsensorScb, HsensorListener *listener, const SensorPara *openParam)
{
    if (memcmp(&(hsensorScb->decisionPara), openParam, sizeof(SensorPara)) == 0) {
        /* 说明不需要重启,更新此次listener的相关参数 */
        if (hsensorScb->reportPeriod != 0) {
            listener->sampleCnt = 0;
            listener->dispatchCnt = listener->notifyPeriod / hsensorScb->reportPeriod;
            return SENSOR_OK;
        } else {
            HILOG_ERROR(HILOG_MODULE_SEN,
                "hsensorListener name %s type %d open error, batch %d period %d.",
                listener->name,
                listener->type,
                openParam->batch,
                openParam->period);
            return SENSOR_HSENSOR_SCB_REPORT_PERIOD_ZERO;
        }
    }
    /* 需要重启hensor */
    (void)memcpy_s(&hsensorScb->decisionPara, sizeof(SensorPara), openParam, sizeof(SensorPara));
    hsensorScb->reportPeriod = openParam->batch * openParam->period;
    if (hsensorScb->reportPeriod == 0) {
        HILOG_ERROR(HILOG_MODULE_SEN,
            "hsensorListener name %s type %d open error, batch %d period %d.",
            listener->name,
            listener->type,
            openParam->batch,
            openParam->period);
        return SENSOR_HSENSOR_SCB_REPORT_PERIOD_ZERO;
    }

    int32_t ret = hsensorScb->hsensor->open(&hsensorScb->decisionPara);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN,
            "hsensorListener name %s type %d open error, batch %u period %u.",
            listener->name,
            listener->type,
            openParam->batch,
            openParam->period);
        return ret;
    }
    HILOG_INFO(HILOG_MODULE_SEN,
        "hsensor name %s type %d open batch %u period %u.",
        hsensorScb->hsensor->name,
        hsensorScb->type,
        hsensorScb->decisionPara.batch,
        hsensorScb->decisionPara.period);
    return SENSOR_OK;
}

/* ******************************************************************************
  函 数 名      : HsensorMgrOpenListener
  功能描述      :  1.检查open 参数 2.更新所有listener 3.打开对应的hsensor

  输入参数      :  None
  输出参数      :  None
  返 回 值      :  None
****************************************************************************** */
int32_t HsensorMgrOpenListener(HsensorListener *listener)
{
    if (listener == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "listener is null.", listener->type);
        return SENSOR_HSENSOR_LISTENER_NULL;
    }
    HsensorScb *hsensorScb = HsensorMgrHsensorScbFind(listener->type);
    if (hsensorScb == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type %d hsensorScb get fail.", listener->type);
        return SENSOR_HSENSOR_SCB_NULL;
    }
    // 检查该 listener 是否 存在于scb中
    if (HsensormgrCheckListener(hsensorScb, listener) != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "hsensorListener name  %s type %d not exits.", listener->name, listener->type);
        return SENSOR_HSENSOR_SCB_LISTENER_NOT_EXIST;
    }
    listener->status = SENSOR_LISTENER_ENABLE;
    SensorPara openParam = listener->openParam;
    uint32_t reportPeriod = 0;
    /* 计算此次listener的reportPeriod等 */
    HILOG_DEBUG(HILOG_MODULE_SEN,
        "open listener para period: %u batch: %u mode: %d option: %u ",
        openParam.period,
        openParam.batch,
        openParam.mode,
        openParam.option);
    HsensormgrOpenCheckOpenParam(listener, &openParam, &reportPeriod);
    /* 遍历所有使能的listener,更新全局的openParam */
    if (HsensormgrUpdateOpenParam(hsensorScb, &openParam, reportPeriod) == false) {
        HILOG_INFO(
            HILOG_MODULE_SEN, "hsensor not need open, listener open type %d name %s ", listener->type, listener->name);
        return SENSOR_OK;
    }
    if (HsensormgrUpdateListener(hsensorScb, listener, &openParam) != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "hsensorListener name  %s type %d open error.", listener->name, listener->type);
        return SENSOR_ERROR;
    }
    listener->status = SENSOR_LISTENER_WORKING;
    uint32_t taskId = SensorUtilsGetRegisteredTaskId(listener->type);
    if (taskId != SENSOR_FAILURE) {  // 已经创建线程
        return SENSOR_OK;
    }
    int32_t ret = HsensorScbMallocDataBuffer(hsensorScb);
    if (ret != SENSOR_OK) {
        return ret;
    }
    ret = SensorTaskCreateHsensorTask(hsensorScb->hsensor);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "hsensor type %d CreateHsensorTask failed.", listener->type);
        return ret;
    }
    HILOG_INFO(HILOG_MODULE_SEN, "hsensor listener open type %d name %s ", listener->type, listener->name);
    return SENSOR_OK;
}

/* ******************************************************************************
  函 数 名		:  HsensormgrCloseCheckOpenParam
  功能描述		:  遍历剩余的使能的listener，找到一个开启的openParam
  输入参数		:  None
  输出参数		:  None
  返 回 值		:  None
****************************************************************************** */
static void HsensormgrCloseCheckOpenParam(HsensorScb *hsensorScb, SensorPara *openParam, uint32_t *reportPeriod)
{
    HsensorListener *listenerIndex = NULL;
    osal_list_for_each_entry(listenerIndex, &hsensorScb->listenerList, list)
    {
        if (listenerIndex->status == SENSOR_LISTENER_WORKING || listenerIndex->status == SENSOR_LISTENER_ENABLE) {
            (void)memcpy_s(openParam, sizeof(SensorPara), &listenerIndex->openParam, sizeof(SensorPara));
            HsensormgrCheckOpenParam(openParam);
            *reportPeriod = openParam->period * openParam->batch;
            break;
        }
    }
}

int32_t HsensorMgrCloseListener(HsensorListener *listener)
{
    if (listener == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "close listener is null.");
        return SENSOR_HSENSOR_LISTENER_NULL;
    }
    HsensorScb *hsensorScb = HsensorMgrHsensorScbFind(listener->type);
    if (hsensorScb == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type %d hsensorScb get fail.", listener->type);
        return SENSOR_HSENSOR_SCB_NULL;
    }
    if (HsensormgrCheckListener(hsensorScb, listener) != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "hsensorListener name  %s not exits.", listener->name);
        return SENSOR_ERROR;
    }
    uint32_t reportPeriod = 0;
    SensorPara openParam = {.period = 0, .batch = 0, .mode = SENSOR_MODE_REALTIME, .option = 0};
    listener->status = SENSOR_LISTENER_DISABLE;

    HsensormgrCloseCheckOpenParam(hsensorScb, &openParam, &reportPeriod);
    /* 遍历所有使能的listener,更新全局的openParam */
    if (HsensormgrUpdateOpenParam(hsensorScb, &openParam, reportPeriod) == false) {
        /* 不存在打开的listener,关闭hsensor */
        int32_t ret = SensorTaskDestroyHsensorTask(hsensorScb->type);
        if (ret != SENSOR_OK) {
            HILOG_ERROR(HILOG_MODULE_SEN, "hsensor type %d Destroy failed.", hsensorScb->type);
        }
        ret = HsensorScbFreeDataBuffer(hsensorScb);
        if (ret != SENSOR_OK) {
            HILOG_ERROR(HILOG_MODULE_SEN, "hsensor type %d FreeData failed.", hsensorScb->type);
            return ret;
        }
        memset_s(&hsensorScb->decisionPara, sizeof(SensorPara), 0, sizeof(SensorPara));
        hsensorScb->hsensor->close();
        HILOG_INFO(HILOG_MODULE_SEN, "hsensor name %s type %d close.", hsensorScb->hsensor->name, hsensorScb->type);
        return SENSOR_OK;
    } else {
        /* 还存在使能的listener, */
        if (HsensormgrUpdateListener(hsensorScb, listener, &openParam) != SENSOR_OK) {
            HILOG_ERROR(
                HILOG_MODULE_SEN, "hsensorListener name  %s type %d open error.", listener->name, listener->type);
            return SENSOR_ERROR;
        }
    }
    HILOG_INFO(HILOG_MODULE_SEN, "hsensor listener close type %d name %s ", listener->type, listener->name);
    return SENSOR_OK;
}

static int32_t HsensorMgrSensorOpen(const Sensor *sensor, const SensorPara *openPara)
{
    HILOG_INFO(HILOG_MODULE_SEN,
        "SensorOpen hsensor type:%d name:%s period:%u batch:%u option:%u mode:%d",
        sensor->sensorInfo.type,
        sensor->sensorInfo.name,
        openPara->period,
        openPara->batch,
        openPara->option,
        openPara->mode);
    int32_t ret = SENSOR_OK;
    HsensorScb *hsensorScb = HsensorMgrHsensorScbFind(sensor->sensorInfo.type);
    if (hsensorScb == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type %d hsensorScb get fail.", sensor->sensorInfo.type);
        return SENSOR_HSENSOR_SCB_NULL;
    }
    HsensorMgrPrintListener(sensor->sensorInfo.type);
    HsensorListener *listenerIndex = NULL;
    osal_list_for_each_entry(listenerIndex, &hsensorScb->listenerList, list)
    {
        HILOG_DEBUG(HILOG_MODULE_SEN,
            "hsensor type: %d openParam:%d name:%s",
            sensor->sensorInfo.type,
            SensorParaIsClear(&listenerIndex->openParam),
            listenerIndex->name);
        /* 表示vsensor 注册的 Listener 不需要 APP open */
        if (listenerIndex->notifyCallback != NULL) {
            continue;
        }
        if (SensorParaIsClear(&listenerIndex->openParam)) {
            SensorParaCopy(&listenerIndex->openParam, openPara);
        }
        ret = HsensorMgrOpenListener(listenerIndex);
        if (ret != SENSOR_OK) {
            HILOG_ERROR(HILOG_MODULE_SEN, "hsensor type %d OpenListener failed.", sensor->sensorInfo.type);
        }
    }
    return SENSOR_OK;
}

static int32_t HsensorMgrSensorClose(const Sensor *sensor)
{
    int32_t ret = SENSOR_OK;
    HsensorScb *hsensorScb = HsensorMgrHsensorScbFind(sensor->sensorInfo.type);
    if (hsensorScb == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type %d hsensorScb get fail.", sensor->sensorInfo.type);
        return SENSOR_HSENSOR_SCB_NULL;
    }
    HsensorMgrPrintListener(sensor->sensorInfo.type);
    HsensorListener *listenerIndex = NULL;
    HsensorListener *listenerIndexTemp = NULL;
    osal_list_for_each_entry_safe(listenerIndex, listenerIndexTemp, &hsensorScb->listenerList, list)
    {
        ret = HsensorMgrCloseListener(listenerIndex);
        if (ret != SENSOR_OK) {
            HILOG_ERROR(HILOG_MODULE_SEN, "hsensor type %d CloseListener failed.", sensor->sensorInfo.type);
        }

        ret = HsensorMgrUnRegisterListener(listenerIndex);
        if (ret != SENSOR_OK) {
            HILOG_ERROR(HILOG_MODULE_SEN, "UnRegisterListener fail.");
        }
    }
    HILOG_DEBUG(HILOG_MODULE_SEN, "close sensorInfo type : %d", sensor->sensorInfo.type);
    return SENSOR_OK;
}

/* samplingInterval Indicates the sensor data sampling interval to set, in millisecond. 数据采样间隔 */
/* reportInterval Indicates the sensor data reporting interval, in millisecond.数据上报间隔 */
static int32_t HsensorMgrSensorSetBatch(Sensor *sensor, int64_t samplingInterval, int64_t reportInterval)
{
    HsensorScb *hsensorScb = HsensorMgrHsensorScbFind(sensor->sensorInfo.type);
    if (hsensorScb == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type %d hsensorScb get fail.", sensor->sensorInfo.type);
        return SENSOR_HSENSOR_SCB_NULL;
    }
    if (samplingInterval < SENSOR_PARA_REALTIME_PERIOD_MIN || samplingInterval > SENSOR_PARA_REALTIME_PERIOD_MAX) {
        HILOG_ERROR(HILOG_MODULE_SEN,
            "samplingInterval is max: %d to min: %d, samplingInterval: %d ",
            SENSOR_PARA_REALTIME_PERIOD_MAX,
            SENSOR_PARA_REALTIME_PERIOD_MIN,
            samplingInterval,
            reportInterval);
        return SENSOR_HSENSOR_PERIOD_BATCH_ERR;
    }
    uint32_t batch = reportInterval / samplingInterval;
    uint32_t maxBatchCount = hsensorScb->hsensor->caps.maxBatchCount;
    if (batch < SENSOR_PARA_BATCH_MIN || batch > maxBatchCount) {
        HILOG_ERROR(
            HILOG_MODULE_SEN, "batch is max: %d to min: %d, batch: %u ", maxBatchCount, SENSOR_PARA_BATCH_MIN, batch);
        return SENSOR_HSENSOR_PERIOD_BATCH_ERR;
    }
    sensor->sensorInfo.openPara.period = samplingInterval;
    sensor->sensorInfo.openPara.batch = batch;
    HILOG_INFO(HILOG_MODULE_SEN,
        "samplingInterval: %lld reportInterval: %lld batch: %u",
        samplingInterval,
        reportInterval,
        batch);
    return SENSOR_OK;
}

static int32_t HsensorMgrSensorSetMode(Sensor *sensor, int32_t mode)
{
    HsensorScb *hsensorScb = HsensorMgrHsensorScbFind(sensor->sensorInfo.type);
    if (hsensorScb == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type %d hsensorScb get fail.", sensor->sensorInfo.type);
        return SENSOR_HSENSOR_SCB_NULL;
    }
    if (mode < 0 || mode >= SENSOR_MODE_MAX) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type %d set mode error %d", hsensorScb->type, mode);
        return SENSOR_HSENSOR_MODE_ERR;
    }
    sensor->sensorInfo.openPara.mode = mode;
    HILOG_INFO(HILOG_MODULE_SEN, "mode: %d ", mode);
    return SENSOR_OK;
}

static int32_t HsensorMgrSensorSetOption(Sensor *sensor, uint32_t option)
{
    HsensorScb *hsensorScb = HsensorMgrHsensorScbFind(sensor->sensorInfo.type);
    if (hsensorScb == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type %d hsensorScb get fail.", sensor->sensorInfo.type);
        return SENSOR_HSENSOR_SCB_NULL;
    }
    hsensorScb->decisionPara.option = option;
    return SENSOR_OK;
}

static HsensorListener *HsensorMgrHlistenerInit(const Sensor *sensor, RecordDataCallback cb)
{
    HsensorListener *listener = SensorUtilsRequestListener();
    if (listener == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type %d listener get fail.", sensor->sensorInfo.type);
        return NULL;
    }
    errno_t err = memset_s(listener, sizeof(HsensorListener), 0, sizeof(HsensorListener));
    if (err != EOK) {
        SensorUtilsFreeListener(listener);
        HILOG_ERROR(HILOG_MODULE_SEN, "ResultCallback memset failed! type:%d", sensor->sensorInfo.type);
        return NULL;
    }
    err = strcat_s(listener->name, sizeof(listener->name), "APP");
    if (err != EOK) {
        SensorUtilsFreeListener(listener);
        HILOG_ERROR(HILOG_MODULE_SEN, "ResultCallback init name strcat full! type:%d", sensor->sensorInfo.type);
        return NULL;
    }
    err = strcat_s(listener->name, sizeof(listener->name), sensor->sensorInfo.name);
    if (err != EOK) {
        SensorUtilsFreeListener(listener);
        HILOG_ERROR(HILOG_MODULE_SEN, "ResultCallback init name strcat full! type:%d", sensor->sensorInfo.type);
        return NULL;
    }
    listener->type = sensor->sensorInfo.type;
    listener->resultDataCb = cb;
    HILOG_INFO(HILOG_MODULE_SEN,
        "HlistenerInit hsensor type:%d name:%s period:%u batch:%u option:%u mode:%u",
        sensor->sensorInfo.type,
        sensor->sensorInfo.name,
        sensor->sensorInfo.openPara.period,
        sensor->sensorInfo.openPara.batch,
        sensor->sensorInfo.openPara.option,
        sensor->sensorInfo.openPara.mode);
    return listener;
}

static int32_t HsensorMgrSensorRegisterResultCallback(const Sensor *sensor, RecordDataCallback cb)
{
    HsensorScb *hsensorScb = HsensorMgrHsensorScbFind(sensor->sensorInfo.type);
    if (hsensorScb == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type %d hsensorScb or simpleName get fail.", sensor->sensorInfo.type);
        return SENSOR_HSENSOR_SCB_NULL;
    }
    HsensorListener *listener = HsensorMgrHlistenerInit(sensor, cb);
    if (listener == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "RegisterListener failed! type:%d", sensor->sensorInfo.type);
        return SENSOR_HSENSOR_LISTENER_NULL;
    }
    int32_t ret = HsensorMgrRegisterListener(listener);
    if (ret != SENSOR_OK) {
        SensorUtilsFreeListener(listener);
        HILOG_ERROR(HILOG_MODULE_SEN, "RegisterListener failed! type:%d", sensor->sensorInfo.type);
        return ret;
    }
    return SENSOR_OK;
}

static int32_t HsensorMgrSensorUnregisterResultCallback(const Sensor *sensor, RecordDataCallback cb)
{
    HsensorScb *hsensorScb = HsensorMgrHsensorScbFind(sensor->sensorInfo.type);
    if (hsensorScb == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type %d hsensorScb get fail.", sensor->sensorInfo.type);
        return SENSOR_HSENSOR_SCB_NULL;
    }
    HsensorListener *listenerIndex = NULL;
    HsensorListener *listener = NULL;
    osal_list_for_each_entry(listenerIndex, &hsensorScb->listenerList, list)
    {
        if (listenerIndex->resultDataCb == cb) {
            listener = listenerIndex;
            HILOG_INFO(HILOG_MODULE_SEN, "hsensor type %d RecordDataCallback exits.", sensor->sensorInfo.type);
            break;
        }
    }
    if (listener == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "HsensorListener get fail.");
        return SENSOR_HSENSOR_LISTENER_NULL;
    }
    int32_t ret = HsensorMgrCloseListener(listener);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "hsensor type %d CloseListener failed.", sensor->sensorInfo.type);
    }
    ret = HsensorMgrUnRegisterListener(listener);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "UnRegisterListener fail.");
    }
    HILOG_INFO(HILOG_MODULE_SEN, "hsensor type: %d Unregister Success", sensor->sensorInfo.type);
    return SENSOR_OK;
}

static int32_t HsensorMgrSensorCalibrate(const Sensor *sensor, void *para, uint32_t len)
{
    HILOG_WARN(HILOG_MODULE_SEN, "unsupport");
    return SENSOR_OK;
}

static int32_t HsensorMgrSensorSelfTest(const Sensor *sensor, void *para, uint32_t len)
{
    HILOG_WARN(HILOG_MODULE_SEN, "unsupport");
    return SENSOR_OK;
}

static int32_t HsensorMgrSensorIoctl(const Sensor *sensor, SensorIoctlCmd cmd, void *data, uint32_t len)
{
    if (cmd >= SENSOR_IOCTL_MAX || cmd < SENSOR_IOCTL_SET_PARA) {
        HILOG_ERROR(HILOG_MODULE_SEN, "invalid command!");
        return SENSOR_ERROR;
    }
    HsensorScb *hsensorScb = HsensorMgrHsensorScbFind(sensor->sensorInfo.type);
    if (hsensorScb->hsensor->ioctl == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "invalid ioctl function!");
        return SENSOR_ERROR;
    }
    return hsensorScb->hsensor->ioctl(cmd, data, len);
}

static int32_t HsensorMgrSensorRegisterProc(const Hsensor *hsensor)
{
    Sensor *sensor = SensorUtilsRequestSensor(hsensor->type);
    if (sensor == NULL || sensor->sensorInfo.state != SENSOR_STATE_IDLE) {
        HILOG_ERROR(HILOG_MODULE_SEN, "sensor type %d name %s request fail.", hsensor->type, hsensor->name);
        return SENSOR_ERROR;
    }
    sensor->sensorInfo.type = hsensor->type;
    sensor->sensorInfo.name = hsensor->name;
    SensorParaDefaultInit(&sensor->sensorInfo.openPara);
    sensor->open = HsensorMgrSensorOpen;
    sensor->close = HsensorMgrSensorClose;
    sensor->setBatch = HsensorMgrSensorSetBatch;
    sensor->setMode = HsensorMgrSensorSetMode;
    sensor->setOption = HsensorMgrSensorSetOption;
    sensor->registerResultCallback = HsensorMgrSensorRegisterResultCallback;
    sensor->unregisterResultCallback = HsensorMgrSensorUnregisterResultCallback;
    sensor->calibrate = HsensorMgrSensorCalibrate;
    sensor->selfTest = HsensorMgrSensorSelfTest;
    sensor->ioctl = HsensorMgrSensorIoctl;
    HILOG_INFO(HILOG_MODULE_SEN,
        "hsensor type:%d name:%s period:%u batch:%u option:%u mode:%d",
        hsensor->type,
        hsensor->name,
        sensor->sensorInfo.openPara.period,
        sensor->sensorInfo.openPara.batch,
        sensor->sensorInfo.openPara.option,
        sensor->sensorInfo.openPara.mode);
    return SENSOR_OK;
}

int32_t HsensorMgrRegister(const Hsensor *hsensor)
{
    HILOG_INFO(HILOG_MODULE_SEN, "register hsensor type:%d name:%s ", hsensor->type, hsensor->name);
    HsensorMgr *hsensorMgr = HsensorMgrGetInstance();
    if (!hsensorMgr->inited && (HsensorMgrInit() != SENSOR_OK)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "hsensorMgr inited failed.");
        return SENSOR_ERROR;
    }
    HsensorScb *hsensorScbIndex = NULL;
    osal_list_for_each_entry(hsensorScbIndex, &hsensorMgr->hsensorScbHead, list)
    {
        if (hsensorScbIndex->type == hsensor->type) {
            HILOG_WARN(HILOG_MODULE_SEN, "hsensor type %d already exits.", hsensorScbIndex->type);
            return SENSOR_HSENSOR_REGISTER_TYPE_DUPLICATE;
        }
    }
    HsensorScb *hsensorScb = HsensorScbRequest(hsensorMgr->hsensorScbNum);
    if (hsensorScb == NULL || hsensorScb->status != HSENSOR_SCB_UNINITED) {
        HILOG_ERROR(HILOG_MODULE_SEN, "hsensorScb type %d name %s request fail.", hsensor->type, hsensor->name);
        return SENSOR_ERROR;
    }
    if (hsensor->init() != SENSOR_OK) {
        hsensor->deinit();
        HILOG_ERROR(HILOG_MODULE_SEN, "hsensor type %d name %s init fail.", hsensor->type, hsensor->name);
        return SENSOR_ERROR;
    }
    hsensorScb->hsensor = (Hsensor *)hsensor;
    hsensorScb->type = hsensor->type;
    int32_t ret = SensorUtilsAddHsensorList(hsensorScb);
    if (ret != SENSOR_OK) {
        hsensorScb->type = SENSOR_TYPE_NONE;
        hsensor->deinit();
        HILOG_ERROR(
            HILOG_MODULE_SEN, "hsensor type %d name %s SensorUtilsAddSensorList fail", hsensor->type, hsensor->name);
        return SENSOR_ERROR;
    }
    ret = HsensorMgrSensorRegisterProc(hsensor);
    if (ret != SENSOR_OK) {
        hsensorScb->type = SENSOR_TYPE_NONE;
        hsensor->deinit();
        HILOG_ERROR(HILOG_MODULE_SEN, "hsensor type %d name %s SensorRegisterProc fail", hsensor->type, hsensor->name);
        return SENSOR_ERROR;
    }
    hsensorScb->status = HSENSOR_SCB_INITED;
    osal_list_add(&hsensorScb->list, &hsensorMgr->hsensorScbHead);
    hsensorMgr->hsensorScbNum++;
    HILOG_INFO(HILOG_MODULE_SEN, "register hsensor type %d name  %s success.", hsensor->type, hsensor->name);
    return SENSOR_OK;
}

int32_t HsensorMgrIoctl(enum SensorTypeTag type, SensorIoctlCmd cmd, void *data, uint32_t len)
{
    HsensorScb *hsensorScb = HsensorMgrHsensorScbFind(type);
    if (hsensorScb == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type %d hsensorScb get fail.", type);
        return SENSOR_ERROR;
    }
    if (hsensorScb->hsensor != NULL && hsensorScb->hsensor->ioctl != NULL) {
        return hsensorScb->hsensor->ioctl(cmd, data, len);
    }
    return SENSOR_ERROR;
}

void HsensorMgrPrintListener(enum SensorTypeTag type)
{
    HsensorScb *hsensorScb = HsensorMgrHsensorScbFind(type);
    if (hsensorScb == NULL) {
        return;
    }
    HILOG_INFO(HILOG_MODULE_SEN, "type: %d listenerNum:%u", type, hsensorScb->listenerNum);
    HsensorListener *listenerIndex = NULL;
    osal_list_for_each_entry(listenerIndex, &hsensorScb->listenerList, list)
    {
        HILOG_INFO(HILOG_MODULE_SEN,
            "type %d name %s period %u batch %u mode %d option %u status %d",
            listenerIndex->type,
            listenerIndex->name,
            listenerIndex->openParam.period,
            listenerIndex->openParam.batch,
            listenerIndex->openParam.mode,
            listenerIndex->openParam.option,
            listenerIndex->status);
    }
}