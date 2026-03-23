/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: gyroScope simu
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

static int32_t GyroScopeInit(void);
static int32_t GyroScopeDeinit(void);
static int32_t GyroScopeOpen(SensorPara *para);
static int32_t GyroScopeClose(void);
static int32_t GyroScopeRead(uint8_t *data, uint32_t len);
static int32_t GyroScopeIoctl(SensorIoctlCmd cmd, void *data, uint32_t len);

const static Hsensor g_gyroScopeHsensor = {
    .type = SENSOR_TYPE_GYROSCOPE,
    .name = "GYRO_SCOPE",
    .caps.sampleElementlen = sizeof(struct GyroData),
    .caps.maxBatchCount = ACCEL_MAX_BATCH_COUNT,
    .privateData = NULL,
    .privateDataLen = 0,
    .init = GyroScopeInit,
    .deinit = GyroScopeDeinit,
    .open = GyroScopeOpen,
    .close = GyroScopeClose,
    .read = GyroScopeRead,
    .ioctl = GyroScopeIoctl
};

static struct GyroData g_gyroScopeData = {
    .axisX = 8,
    .axisY = 9,
    .axisZ = 10
};

static int32_t GyroScopeInit(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "GyroScopeInit!");
    return SENSOR_OK;
}

static int32_t GyroScopeDeinit(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "GyroScopeDeinit!");
    return SENSOR_OK;
}

static int32_t GyroScopeOpen(SensorPara *para)
{
    HILOG_INFO(HILOG_MODULE_SEN, "GyroScopeOpen!");
    return SENSOR_OK;
}

static int32_t GyroScopeClose(void)
{
    g_gyroScopeData.axisX = 0;
    g_gyroScopeData.axisY = 0;
    g_gyroScopeData.axisZ = 0;
    HILOG_INFO(HILOG_MODULE_SEN, "GyroScopeClose!");
    return SENSOR_OK;
}

static int32_t GyroScopeRead(uint8_t *data, uint32_t len)
{
    HILOG_INFO(HILOG_MODULE_SEN, "GyroScopeRead");
    CHECK_NULL_PTR_RETURN_VALUE(data, SENSOR_ERROR);
    int32_t readLen = sizeof(struct GyroData);
    int32_t ret = memcpy_s(data, len, &g_gyroScopeData, readLen);
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "memcpy_s err");
        return -1;
    }
    g_gyroScopeData.axisX++;
    g_gyroScopeData.axisY++;
    g_gyroScopeData.axisZ++;
    return readLen;
}

static int32_t GyroScopeIoctl(SensorIoctlCmd cmd, void *data, uint32_t len)
{
    HILOG_INFO(HILOG_MODULE_SEN, "GyroScopeIoctl!");
    return SENSOR_OK;
}

static void GyroScopeHsensorRigister(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "RegistHsensor type:%d !", g_gyroScopeHsensor.type);
    int32_t ret = SensorMgrRegistHsensor(&g_gyroScopeHsensor);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "RegistHsensor type:%d ERR!", g_gyroScopeHsensor.type);
        return;
    }
    HILOG_INFO(HILOG_MODULE_SEN, "RegistHsensor type:%d SUCCESS!", g_gyroScopeHsensor.type);
}

APP_FEATURE_INIT_PRI(GyroScopeHsensorRigister, 3);
