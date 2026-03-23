/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: pedometer simu
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

static int32_t PedoMeterInit(void);
static int32_t PedoMeterDeinit(void);
static int32_t PedoMeterOpen(SensorPara *para);
static int32_t PedoMeterClose(void);
static int32_t PedoMeterRead(uint8_t *data, uint32_t len);
static int32_t PedoMeterIoctl(SensorIoctlCmd cmd, void *data, uint32_t len);

const static Hsensor g_pedoMeterHsensor = {
    .type = SENSOR_TYPE_PEDOMETER,
    .name = "PEDO_METER",
    .caps.sampleElementlen = sizeof(int32_t),
    .caps.maxBatchCount = ACCEL_MAX_BATCH_COUNT,
    .privateData = NULL,
    .privateDataLen = 0,
    .init = PedoMeterInit,
    .deinit = PedoMeterDeinit,
    .open = PedoMeterOpen,
    .close = PedoMeterClose,
    .read = PedoMeterRead,
    .ioctl = PedoMeterIoctl
};

static int32_t g_pedoMeterData = 0;

static int32_t PedoMeterInit(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "PedoMeterInit!");
    return SENSOR_OK;
}

static int32_t PedoMeterDeinit(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "PedoMeterDeinit!");
    return SENSOR_OK;
}

static int32_t PedoMeterOpen(SensorPara *para)
{
    HILOG_INFO(HILOG_MODULE_SEN, "PedoMeterOpen!");
    return SENSOR_OK;
}

static int32_t PedoMeterClose(void)
{
    g_pedoMeterData = 0;
    HILOG_INFO(HILOG_MODULE_SEN, "PedoMeterClose!");
    return SENSOR_OK;
}

static int32_t PedoMeterRead(uint8_t *data, uint32_t len)
{
    HILOG_INFO(HILOG_MODULE_SEN, "PedoMeterRead");
    CHECK_NULL_PTR_RETURN_VALUE(data, SENSOR_ERROR);
    int32_t readLen = sizeof(int32_t);
    int32_t ret = memcpy_s(data, len, &g_pedoMeterData, readLen);
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "memcpy_s err");
        return -1;
    }
    g_pedoMeterData++;
    return readLen;
}

static int32_t PedoMeterIoctl(SensorIoctlCmd cmd, void *data, uint32_t len)
{
    HILOG_INFO(HILOG_MODULE_SEN, "PedoMeterIoctl!");
    return SENSOR_OK;
}

static void PedoMeterHsensorRigister(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "RegistHsensor type:%d !", g_pedoMeterHsensor.type);
    int32_t ret = SensorMgrRegistHsensor(&g_pedoMeterHsensor);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "RegistHsensor type:%d ERR!", g_pedoMeterHsensor.type);
        return;
    }
    HILOG_INFO(HILOG_MODULE_SEN, "RegistHsensor type:%d SUCCESS!", g_pedoMeterHsensor.type);
}

APP_FEATURE_INIT_PRI(PedoMeterHsensorRigister, 3);
