/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: accel simu
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#include "sensor_typedef.h"
#include "sensor_log.h"
#include "sensor_common.h"
#include "securec.h"
#include "sensor_manager.h"
#include "ohos_init.h"

#define ACCEL_MAX_BATCH_COUNT 10

static int32_t AccSimuInit(void);
static int32_t AccSimuDeinit(void);
static int32_t AccSimuOpen(SensorPara *para);
static int32_t AccSimuClose(void);
static int32_t AccSimuRead(uint8_t *data, uint32_t len);
static int32_t AccSimuIoctl(SensorIoctlCmd cmd, void *data, uint32_t len);

const static Hsensor g_accSimuHsensor = {
    .type = SENSOR_TYPE_ACCELEROMETER,
    .name = "ACCEL_SIMU",
    .caps.sampleElementlen = sizeof(struct AccelData),
    .caps.maxBatchCount = ACCEL_MAX_BATCH_COUNT,
    .privateData = NULL,
    .privateDataLen = 0,
    .init = AccSimuInit,
    .deinit = AccSimuDeinit,
    .open = AccSimuOpen,
    .close = AccSimuClose,
    .read = AccSimuRead,
    .ioctl = AccSimuIoctl
};

static struct AccelData g_accSimuData = {
    .axisX = 8,
    .axisY = 9,
    .axisZ = 10
};

static struct AccelData g_accSimuStoreData[ACCEL_MAX_BATCH_COUNT] = {0};
static uint32_t g_accSimuBacthCnt = 1;
static uint32_t g_accSimuTimeStamp = 0;

static void AccSimuAddStoreData()
{
    uint32_t top = g_accSimuBacthCnt == ACCEL_MAX_BATCH_COUNT ? ACCEL_MAX_BATCH_COUNT - 1 : g_accSimuBacthCnt;
    if (g_accSimuBacthCnt != ACCEL_MAX_BATCH_COUNT) {
        g_accSimuBacthCnt++;
    }
    for (uint32_t i = top; i > 0; i--) {
        g_accSimuStoreData[i].axisX = g_accSimuStoreData[i - 1].axisX;
        g_accSimuStoreData[i].axisY = g_accSimuStoreData[i - 1].axisY;
        g_accSimuStoreData[i].axisZ = g_accSimuStoreData[i - 1].axisZ;
    }
    g_accSimuStoreData[0].axisX = g_accSimuData.axisX++;
    g_accSimuStoreData[0].axisY = g_accSimuData.axisY++;
    g_accSimuStoreData[0].axisZ = g_accSimuData.axisZ++;
    HILOG_INFO(HILOG_MODULE_SEN,
        "AccSimuRead dataX:%d, dataY:%d, dataZ:%d",
        g_accSimuData.axisX,
        g_accSimuData.axisY,
        g_accSimuData.axisZ);
}

static int32_t AccSimuInit(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "AccSimuInit!");
    return SENSOR_OK;
}

static int32_t AccSimuDeinit(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "AccSimuDeinit!");
    return SENSOR_OK;
}

static int32_t AccSimuOpen(SensorPara *para)
{
    HILOG_INFO(HILOG_MODULE_SEN, "AccSimuOpen!");
    return SENSOR_OK;
}

static int32_t AccSimuClose(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "AccSimuClose!");
    g_accSimuData.axisX = 0;
    g_accSimuData.axisY = 0;
    g_accSimuData.axisZ = 0;
    return SENSOR_OK;
}

static int32_t AccSimuRead(uint8_t *data, uint32_t len)
{
    AccSimuAddStoreData();
    g_accSimuTimeStamp += 1;
    CHECK_NULL_PTR_RETURN_VALUE(data, SENSOR_ERROR);
    int32_t readLen = sizeof(struct AccelData);
    int32_t ret = memcpy_s(data, len, &g_accSimuData, readLen);
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "memcpy_s err");
        return -1;
    }
    g_accSimuData.axisX++;
    g_accSimuData.axisY++;
    g_accSimuData.axisZ++;
    return readLen;
}

static int32_t AccSimuGetData(SensorData *sensorData)
{
    sensorData->data = g_accSimuStoreData;
    size_t len = sizeof(struct AccelData);
    sensorData->dataLen = (uint32_t)len;
    sensorData->batchCnt = g_accSimuBacthCnt;
    sensorData->timestamp = g_accSimuTimeStamp;
    return SENSOR_OK;
}

static int32_t AccSimuIoctl(SensorIoctlCmd cmd, void *data, uint32_t len)
{
    HILOG_INFO(HILOG_MODULE_SEN, "AccSimuIoctl!");
    switch (cmd) {
        case SENSOR_IOCTL_GET_SENSOR_DATA:
            AccSimuGetData((SensorData *)data);
            break;
        default:
            return SENSOR_ERROR;
    }
    return SENSOR_OK;
}

static void AccSimuHsensorRigister(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "RegistHsensor type:%d !", g_accSimuHsensor.type);
    int32_t ret = SensorMgrRegistHsensor(&g_accSimuHsensor);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "RegistHsensor type:%d ERR!", g_accSimuHsensor.type);
        return;
    }
    HILOG_INFO(HILOG_MODULE_SEN, "RegistHsensor type:%d SUCCESS!", g_accSimuHsensor.type);
}

APP_FEATURE_INIT_PRI(AccSimuHsensorRigister, 3);
