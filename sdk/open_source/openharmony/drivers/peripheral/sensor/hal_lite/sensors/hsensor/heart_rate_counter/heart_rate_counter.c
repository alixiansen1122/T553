/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: heart rate simu
 * Author: Media Software Group
 * Create: 2025-09-25
 */

#include "sensor_typedef.h"
#include "sensor_log.h"
#include "sensor_common.h"
#include "securec.h"
#include "sensor_manager.h"
#include "ohos_init.h"

#define MAX_BATCH_COUNT 10
#define DEFAULT_HEART_RATE 80

static int32_t HeartRateInit(void);
static int32_t HeartRateDeinit(void);
static int32_t HeartRateOpen(SensorPara *para);
static int32_t HeartRateClose(void);
static int32_t HeartRateRead(uint8_t *data, uint32_t len);
static int32_t HeartRateIoctl(SensorIoctlCmd cmd, void *data, uint32_t len);

const static Hsensor g_heartRateHsensor = {
    .type = SENSOR_TYPE_HEART_RATE,
    .name = "HEART_RATE",
    .caps.sampleElementlen = sizeof(int32_t),
    .caps.maxBatchCount = MAX_BATCH_COUNT,
    .privateData = NULL,
    .privateDataLen = 0,
    .init = HeartRateInit,
    .deinit = HeartRateDeinit,
    .open = HeartRateOpen,
    .close = HeartRateClose,
    .read = HeartRateRead,
    .ioctl = HeartRateIoctl
};

static int32_t g_heartRate = 0;

static int32_t HeartRateInit(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "HeartRateInit!");
    return SENSOR_OK;
}

static int32_t HeartRateDeinit(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "HeartRateDeinit!");
    return SENSOR_OK;
}

static int32_t HeartRateOpen(SensorPara *para)
{
    HILOG_INFO(HILOG_MODULE_SEN, "HeartRateOpen!");
    g_heartRate = DEFAULT_HEART_RATE;
    return SENSOR_OK;
}

static int32_t HeartRateClose(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "HeartRateClose!");
    g_heartRate = 0;
    return SENSOR_OK;
}

static int32_t HeartRateRead(uint8_t *data, uint32_t len)
{
    HILOG_INFO(HILOG_MODULE_SEN, "HeartRateRead");
    CHECK_NULL_PTR_RETURN_VALUE(data, SENSOR_ERROR);
    int32_t readLen = sizeof(int32_t);
    int32_t ret = memcpy_s(data, len, &g_heartRate, readLen);
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "memcpy_s err");
        return -1;
    }
    g_heartRate++;
    return readLen;
}

static int32_t HeartRateIoctl(SensorIoctlCmd cmd, void *data, uint32_t len)
{
    HILOG_INFO(HILOG_MODULE_SEN, "HeartRateIoctl!");
    return SENSOR_OK;
}

static void HeartRateHsensorRigister(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "RegistHsensor type:%d !", g_heartRateHsensor.type);
    int32_t ret = SensorMgrRegistHsensor(&g_heartRateHsensor);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "RegistHsensor type:%d ERR!", g_heartRateHsensor.type);
        return;
    }
    HILOG_INFO(HILOG_MODULE_SEN, "RegistHsensor type:%d SUCCESS!", g_heartRateHsensor.type);
}

APP_FEATURE_INIT_PRI(HeartRateHsensorRigister, 3);
