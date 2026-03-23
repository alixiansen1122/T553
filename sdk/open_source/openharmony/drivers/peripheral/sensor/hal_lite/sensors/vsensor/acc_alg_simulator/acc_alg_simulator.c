/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: accel vsensor simu
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#include "sensor_common.h"
#include "vsensor_common.h"
#include "securec.h"
#include "sensor_manager.h"
#include "sensor_log.h"
#include "ohos_init.h"

#define ACC_ALG_PERIOD_MS 100 // ms
#define ACC_ALG_BATCH_COUNT 5

#define ACC_ALG_OFFSET 10

static int32_t AccAlgSimuDataCbRegister(RecordDataCallback cb);
static int32_t AccAlgSimuDataCbUnRegister(RecordDataCallback cb);
static int32_t AccAlgSimuOpen(SensorPara *para);
static int32_t AccAlgSimuClose(void);
static int32_t AccAlgSimuProcess(enum SensorTypeTag type, uint8_t *data, uint64_t ts, uint32_t len);
static int32_t AccAlgSimuIoctl(SensorIoctlCmd cmd, void *data, uint32_t len);
static int32_t AccAlgSimuGetData(SensorData *sensorData);

typedef struct {
    struct osal_list_head dataCbListHead; /* DataCallbackInfo head */
    uint32_t dataCbNum;
    uint32_t batchCount;
    uint32_t timeStamp;
    struct AccelData accAlgSimuData[ACC_ALG_BATCH_COUNT];
} AccAlgSimuContext;

const RelySensorInfo g_accAlgSimuRelySensor[] = {
    {
        .type = SENSOR_TYPE_ACCELEROMETER,
        .param = {
            .period = ACC_ALG_PERIOD_MS,
            .batch = ACC_ALG_BATCH_COUNT,
            .mode = SENSOR_MODE_REALTIME,
            .option = 0
        }
    }
};

const RelySensorsInfo g_accAlgSimuRelySensors = {
    .relySensorArray = (RelySensorInfo*)g_accAlgSimuRelySensor,
    .relySensorCnt = sizeof(g_accAlgSimuRelySensor) / sizeof(RelySensorInfo)
};

const static Vsensor g_accAlgSimuVsensor = {
    .type = SENSOR_TYPE_VSENSOR_ACCELEROMETER_ALG_SIMU,
    .name = "ACCEL_ALG_SIMU",
    .privateData = NULL,
    .privateDataLen = 0,
    .relySensors = (RelySensorsInfo*)&g_accAlgSimuRelySensors,
    .registDataCb = AccAlgSimuDataCbRegister,
    .unRegistDataCb = AccAlgSimuDataCbUnRegister,
    .open = AccAlgSimuOpen,
    .close = AccAlgSimuClose,
    .process = AccAlgSimuProcess,
    .ioctl = AccAlgSimuIoctl,
};

AccAlgSimuContext g_accAlgSimuCtx = {
    .dataCbNum = 0,
    .batchCount = 0,
    .timeStamp = 0,
    .accAlgSimuData = {0},
};

struct AccelData g_storeAccData[ACC_ALG_BATCH_COUNT] = {0};
static uint32_t g_storeBatchCnt = 1;
static uint32_t g_storeTimeStamp = 0;
static int32_t AccAlgSimuDataCbRegister(RecordDataCallback cb)
{
    HILOG_INFO(HILOG_MODULE_SEN, "IN");
    if (g_accAlgSimuCtx.dataCbNum == 0) {
        OSAL_INIT_LIST_HEAD(&(g_accAlgSimuCtx.dataCbListHead));
    }
    DataCallbackInfo *dataCallbackInfoIndex = NULL;
    osal_list_for_each_entry(dataCallbackInfoIndex, &g_accAlgSimuCtx.dataCbListHead, list)
    {
        if (cb == dataCallbackInfoIndex->dataCb) {
            HILOG_INFO(HILOG_MODULE_SEN, "cb :%p is Registed");
            return SENSOR_OK;
        }
    }
    DataCallbackInfo *dataCallbackInfo = (DataCallbackInfo *)malloc(sizeof(DataCallbackInfo));
    if (dataCallbackInfo == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "malloc DataCallbackInfo failed!");
        return SENSOR_ERROR;
    }
    OSAL_INIT_LIST_HEAD(&(dataCallbackInfo->list));
    dataCallbackInfo->dataCb = cb;
    osal_list_add(&dataCallbackInfo->list, &g_accAlgSimuCtx.dataCbListHead);
    g_accAlgSimuCtx.dataCbNum++;
    return SENSOR_OK;
}

static int32_t AccAlgSimuDataCbUnRegister(RecordDataCallback cb)
{
    HILOG_INFO(HILOG_MODULE_SEN, "IN");
    DataCallbackInfo *dataCallbackInfoIndex = NULL;
    DataCallbackInfo *dataCallbackInfo = NULL;
    osal_list_for_each_entry(dataCallbackInfoIndex, &g_accAlgSimuCtx.dataCbListHead, list)
    {
        if (cb == dataCallbackInfoIndex->dataCb) {
            dataCallbackInfo = dataCallbackInfoIndex;
            HILOG_INFO(HILOG_MODULE_SEN, "cb is Registed");
            break;
        }
    }
    if (dataCallbackInfo == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "DataCallback not Registed!");
        return SENSOR_ERROR;
    }
    osal_list_del(&dataCallbackInfo->list);
    free(dataCallbackInfo);
    g_accAlgSimuCtx.dataCbNum--;
    return SENSOR_OK;
}


static int32_t AccAlgSimuOpen(SensorPara *para)
{
    HILOG_INFO(HILOG_MODULE_SEN, "IN");
    return SENSOR_OK;
}

static int32_t AccAlgSimuClose(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "IN");
    for (uint32_t i = 0; i < ACC_ALG_BATCH_COUNT; i++) {
        g_accAlgSimuCtx.accAlgSimuData[i].axisX = 0;
        g_accAlgSimuCtx.accAlgSimuData[i].axisY = 0;
        g_accAlgSimuCtx.accAlgSimuData[i].axisZ = 0;
    }
    return SENSOR_OK;
}

static int32_t AccAlgSimuProcess(enum SensorTypeTag type, uint8_t *data, uint64_t ts, uint32_t len)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "Acc ALG Process ts :%llu len:%u!", ts, len);
    uint32_t accelDataLen = sizeof(struct AccelData);
    struct AccelData *orgAccelData = (struct AccelData*)data;
    if (orgAccelData == NULL || (len % sizeof(struct AccelData)) != 0) {
        HILOG_INFO(HILOG_MODULE_SEN, "Acc ALG Process :%llu invalid data!", ts);
        return -1;
    }
    uint32_t batchCount = len / accelDataLen;
    if (batchCount > ACC_ALG_BATCH_COUNT) {
        HILOG_INFO(HILOG_MODULE_SEN, "Acc ALG Process :%llu invalid batchCount:%u!", ts, batchCount);
        return -1;
    }
    for (uint32_t i = 0; i < batchCount; i++) {
        g_accAlgSimuCtx.accAlgSimuData[i].axisX = orgAccelData[i].axisX + ACC_ALG_OFFSET;
        g_accAlgSimuCtx.accAlgSimuData[i].axisY = orgAccelData[i].axisY + ACC_ALG_OFFSET;
        g_accAlgSimuCtx.accAlgSimuData[i].axisZ = orgAccelData[i].axisZ + ACC_ALG_OFFSET;
        g_storeAccData[i].axisX = orgAccelData[i].axisX + ACC_ALG_OFFSET;
        g_storeAccData[i].axisY = orgAccelData[i].axisY + ACC_ALG_OFFSET;
        g_storeAccData[i].axisZ = orgAccelData[i].axisZ + ACC_ALG_OFFSET;
    }
    g_accAlgSimuCtx.batchCount = batchCount;
    g_accAlgSimuCtx.timeStamp = ts;
    g_storeTimeStamp = g_accAlgSimuCtx.timeStamp;
    g_storeBatchCnt = g_accAlgSimuCtx.batchCount;
    DataCallbackInfo *dataCallbackInfoIndex = NULL;
    osal_list_for_each_entry(dataCallbackInfoIndex, &g_accAlgSimuCtx.dataCbListHead, list)
    {
        struct SensorEvents sensorEvent = {
            .sensorId = type,
            .version = 0,
            .timestamp = ts,
            .option = 0,
            .mode = 0,
            .data = (uint8_t *)g_accAlgSimuCtx.accAlgSimuData,
            .dataLen = len,
        };
        dataCallbackInfoIndex->dataCb(&sensorEvent);
    }
    return SENSOR_OK;
}

static int32_t AccAlgSimuIoctl(SensorIoctlCmd cmd, void *data, uint32_t len)
{
    HILOG_INFO(HILOG_MODULE_SEN, "AccAlgSimuIoctl!");
    switch (cmd) {
        case SENSOR_IOCTL_GET_SENSOR_DATA:
            AccAlgSimuGetData((SensorData *)data);
            break;
        default:
            return SENSOR_ERROR;
    }
    return SENSOR_OK;
}

static int32_t AccAlgSimuGetData(SensorData *sensorData)
{
    HILOG_INFO(HILOG_MODULE_SEN, "AccAlgSimuGetData!");
    sensorData->data = (uint8_t *)(g_storeAccData);
    size_t len = sizeof(struct AccelData);
    sensorData->dataLen = (uint32_t)len;
    sensorData->batchCnt = g_storeBatchCnt;
    sensorData->timestamp = g_storeTimeStamp;
    HILOG_INFO(HILOG_MODULE_SEN,
        "SensorData datalen:%u batchcnt:%u timestamp:%lld",
        sensorData->dataLen,
        sensorData->batchCnt,
        sensorData->timestamp);
    return SENSOR_OK;
}

static void AccAlgSimuVsensorRegister(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "RegistVsensor type:%d in !", g_accAlgSimuVsensor.type);
    int32_t ret = SensorMgrRegistVsensor(&g_accAlgSimuVsensor);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "RegistHsensor type:%d ERR!", g_accAlgSimuVsensor.type);
        return;
    }
    HILOG_INFO(HILOG_MODULE_SEN, "RegistVsensor type:%d SUCCESS!", g_accAlgSimuVsensor.type);
}

APP_FEATURE_INIT_PRI(AccAlgSimuVsensorRegister, 4);
