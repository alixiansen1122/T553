/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: baroMeter simu
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

static int32_t BaroMeterInit(void);
static int32_t BaroMeterDeInit(void);
static int32_t BaroMeterOpen(SensorPara *para);
static int32_t BaroMeterClose(void);
static int32_t BaroMeterRead(uint8_t *data, uint32_t len);
static int32_t BaroMeterIoctl(SensorIoctlCmd cmd, void *data, uint32_t len);

const static Hsensor g_baroMeterHsensor = {
    .type = SENSOR_TYPE_BAROMETER,
    .name = "BARO_METER",
    .caps.sampleElementlen = sizeof(struct PressureData),
    .caps.maxBatchCount = MAX_BATCH_COUNT,
    .privateData = NULL,
    .privateDataLen = 0,
    .init = BaroMeterInit,
    .deinit = BaroMeterDeInit,
    .open = BaroMeterOpen,
    .close = BaroMeterClose,
    .read = BaroMeterRead,
    .ioctl = BaroMeterIoctl
};

static struct PressureData g_baroMeterData = {
    .pressure = 8,
    .temperature = 9,
    .dataValid = 0
};

static int32_t BaroMeterInit(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "BaroMeterInit!");
    g_baroMeterData.dataValid = 1;
    return SENSOR_OK;
}

static int32_t BaroMeterDeInit(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "BaroMeterDeInit!");
    return SENSOR_OK;
}

static int32_t BaroMeterOpen(SensorPara *para)
{
    HILOG_INFO(HILOG_MODULE_SEN, "BaroMeterOpen!");
    return SENSOR_OK;
}

static int32_t BaroMeterClose(void)
{
    g_baroMeterData.pressure = 0;
    g_baroMeterData.temperature = 0;
    g_baroMeterData.dataValid = 0;
    HILOG_INFO(HILOG_MODULE_SEN, "BaroMeterClose!");
    return SENSOR_OK;
}

static int32_t BaroMeterRead(uint8_t *data, uint32_t len)
{
    HILOG_INFO(HILOG_MODULE_SEN, "BaroMeterRead");
    CHECK_NULL_PTR_RETURN_VALUE(data, SENSOR_ERROR);
    int32_t readLen = sizeof(struct PressureData);
    int32_t ret = memcpy_s(data, len, &g_baroMeterData, readLen);
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "memcpy_s err");
        return -1;
    }
    g_baroMeterData.pressure++;
    g_baroMeterData.temperature++;
    return readLen;
}

static int32_t BaroMeterIoctl(SensorIoctlCmd cmd, void *data, uint32_t len)
{
    HILOG_INFO(HILOG_MODULE_SEN, "BaroMeterIoctl!");
    return SENSOR_OK;
}

static void BaroMeterHsensorRigister(void)
{
    HILOG_INFO(HILOG_MODULE_SEN, "RegistHsensor type:%d !", g_baroMeterHsensor.type);
    int32_t ret = SensorMgrRegistHsensor(&g_baroMeterHsensor);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_SEN, "RegistHsensor type:%d ERR!", g_baroMeterHsensor.type);
        return;
    }
    HILOG_INFO(HILOG_MODULE_SEN, "RegistHsensor type:%d SUCCESS!", g_baroMeterHsensor.type);
}

APP_FEATURE_INIT_PRI(BaroMeterHsensorRigister, 3);
