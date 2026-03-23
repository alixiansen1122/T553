/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: wear detection simu
 * Author: Media Software Group
 * Create: 2025-09-25
 */

#include "sensor_typedef.h"
#include "sensor_log.h"
#include "sensor_common.h"
#include "securec.h"
#include "sensor_manager.h"
#include "ohos_init.h"

#define ACCEL_MAX_BATCH_COUNT 10

static int32_t WearDetectionInit(void);
static int32_t WearDetectionDeinit(void);
static int32_t WearDetectionOpen(SensorPara *para);
static int32_t WearDetectionClose(void);
static int32_t WearDetectionRead(uint8_t *data, uint32_t len);
static int32_t WearDetectionIoctl(SensorIoctlCmd cmd, void *data, uint32_t len);

const static Hsensor g_wearDetectionHsensor = {
    .type = SENSOR_TYPE_WEAR_DETECTION,
    .name = "WEAR_DETECTION",
    .caps.sampleElementlen = sizeof(bool),
    .caps.maxBatchCount = ACCEL_MAX_BATCH_COUNT,
    .privateData = NULL,
    .privateDataLen = 0,
    .init = WearDetectionInit,
    .deinit = WearDetectionDeinit,
    .open = WearDetectionOpen,
    .close = WearDetectionClose,
    .read = WearDetectionRead,
    .ioctl = WearDetectionIoctl
};

static bool g_wearStatus = false;

static int32_t WearDetectionInit(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "WearDetectionInit!");
    return SENSOR_OK;
}

static int32_t WearDetectionDeinit(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "WearDetectionDeinit!");
    return SENSOR_OK;
}

static int32_t WearDetectionOpen(SensorPara *para)
{
    HILOG_INFO(HILOG_MODULE_SEN, "WearDetectionOpen!");
    g_wearStatus = true;
    return SENSOR_OK;
}

static int32_t WearDetectionClose(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "WearDetectionClose!");
    g_wearStatus = false;
    return SENSOR_OK;
}

static int32_t WearDetectionRead(uint8_t *data, uint32_t len)
{
    HILOG_INFO(HILOG_MODULE_SEN, "WearDetectionRead");
    CHECK_NULL_PTR_RETURN_VALUE(data, SENSOR_ERROR);
    int32_t readLen = sizeof(bool);
    int32_t ret = memcpy_s(data, len, &g_wearStatus, readLen);
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "memcpy_s err");
        return -1;
    }
    return readLen;
}

static int32_t WearDetectionIoctl(SensorIoctlCmd cmd, void *data, uint32_t len)
{
    HILOG_INFO(HILOG_MODULE_SEN, "WearDetectionIoctl!");
    return SENSOR_OK;
}

static void WearDetectionHsensorRigister(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "RegistHsensor type:%d !", g_wearDetectionHsensor.type);
    int32_t ret = SensorMgrRegistHsensor(&g_wearDetectionHsensor);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "RegistHsensor type:%d ERR!", g_wearDetectionHsensor.type);
        return;
    }
    HILOG_INFO(HILOG_MODULE_SEN, "RegistHsensor type:%d SUCCESS!", g_wearDetectionHsensor.type);
}

APP_FEATURE_INIT_PRI(WearDetectionHsensorRigister, 3);
