/*
 * Copyright (c) CompanyNameMagicTag. 2022-2022. All rights reserved.
 * Description: video play sample wrapper
 * Author: Media Software Group
 * Create: 2022-10-17
 */

#include "sensor_sample.h"

#include <climits>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include "securec.h"
#include "common_def.h"
#include "wearable_log.h"
#include "sensor_agent.h"
#include "sensor_agent_type.h"
#include "sensor_agent_type_ext.h"
#include <cmsis_os.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SUPPORT_SENSOR_CNT 6
#define SENSOR_CMD_LEN_MAX 64
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
typedef void *ThreadId;

static int32_t SensorGetAllSensors(SensorTypeId sensorTypeId);
static int32_t SensorSubscribe(SensorTypeId sensorTypeId);
static int32_t SensorActivate(SensorTypeId sensorTypeId);
static int32_t SensorUnsubscribe(SensorTypeId sensorTypeId);
static int32_t SensorDeactivate(SensorTypeId sensorTypeId);
static int32_t SensorGetData(SensorTypeId sensorTypeId);

typedef struct {
    char cmd[SENSOR_CMD_LEN_MAX];
    int32_t (*func)(SensorTypeId sensorTypeId);
} FuncsMap;

static FuncsMap g_sensorFuncs[] = {
    {"getall", SensorGetAllSensors},
    {"subscribe", SensorSubscribe},
    {"activate", SensorActivate},
    {"unsubscribe", SensorUnsubscribe},
    {"deactivate", SensorDeactivate},
    {"getdata", SensorGetData},
};

typedef struct {
    char cmd[SENSOR_CMD_LEN_MAX];
    SensorTypeId sensorTypeId;
} SensorIdMap;

static SensorIdMap g_sensorIdMap[] = {
    {"accelerometer", SENSOR_TYPE_ID_ACCELEROMETER},
    {"barometer", SENSOR_TYPE_ID_BAROMETER},
    {"gyroscope", SENSOR_TYPE_ID_GYROSCOPE},
    {"heartRate", SENSOR_TYPE_ID_HEART_RATE},
    {"onBodyState", SENSOR_TYPE_ID_WEAR_DETECTION},
    {"stepCounter", SENSOR_TYPE_ID_PEDOMETER},
};

void RecordAccelSensorCallback(SensorEvent *event)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
        "sensorTypeId:%d version:%d timestamp:%lld option:%u  mode:%d \n",
        event->sensorTypeId,
        event->version,
        event->timestamp,
        event->option,
        event->mode);
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(AccelData)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SensorEvent data invalid\n");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_ACCELEROMETER) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sensorTypeId %d is not ACCELEROMETER\n", event->sensorTypeId);
        return;
    }
    AccelData *accelData = (AccelData *)event->data;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
        "accelData axisX:%d axisY:%d axisZ:%d \n",
        accelData->axisX,
        accelData->axisY,
        accelData->axisZ);
}

void RecordBarSensorCallback(SensorEvent *event)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
        "sensorTypeId:%d version:%d timestamp:%lld option:%u  mode:%d \n",
        event->sensorTypeId,
        event->version,
        event->timestamp,
        event->option,
        event->mode);
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(PressureData)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SensorEvent data invalid\n");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_BAROMETER) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sensorTypeId %d is not BAROMETER\n", event->sensorTypeId);
        return;
    }
    PressureData *pressureData = (PressureData *)event->data;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
        "pressureData pressure:%u temperature:%u dataValid:%u \n",
        pressureData->pressure,
        pressureData->temperature,
        pressureData->dataValid);
}

void RecordCyroSensorCallback(SensorEvent *event)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
        "sensorTypeId:%d version:%d timestamp:%lld option:%u  mode:%d \n",
        event->sensorTypeId,
        event->version,
        event->timestamp,
        event->option,
        event->mode);
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(GyroData)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SensorEvent data invalid\n");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_GYROSCOPE) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sensorTypeId %d is not GYROSCOPE\n", event->sensorTypeId);
        return;
    }
    GyroData *gyroData = (GyroData *)event->data;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
        "gyroData axisX:%d axisY:%d axisZ:%d \n",
        gyroData->axisX,
        gyroData->axisY,
        gyroData->axisZ);
}

void RecordHrSensorCallback(SensorEvent *event)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
        "sensorTypeId:%d version:%d timestamp:%lld option:%u  mode:%d \n",
        event->sensorTypeId,
        event->version,
        event->timestamp,
        event->option,
        event->mode);
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(uint32_t)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SensorEvent data invalid\n");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_HEART_RATE) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sensorTypeId %d is not HEART_RATE\n", event->sensorTypeId);
        return;
    }
    uint32_t heartRate = *(uint32_t *)event->data;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "heartRate:%u", heartRate);
}

void RecordObsSensorCallback(SensorEvent *event)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
        "sensorTypeId:%d version:%d timestamp:%lld option:%u  mode:%d \n",
        event->sensorTypeId,
        event->version,
        event->timestamp,
        event->option,
        event->mode);
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(uint8_t)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SensorEvent data invalid\n");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_WEAR_DETECTION) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sensorTypeId %d is not WEAR_DETECTION\n", event->sensorTypeId);
        return;
    }
    uint8_t isonbody = *event->data;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "isonbody:%u", isonbody);
}

void RecordScSensorCallback(SensorEvent *event)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
        "sensorTypeId:%d version:%d timestamp:%lld option:%u  mode:%d \n",
        event->sensorTypeId,
        event->version,
        event->timestamp,
        event->option,
        event->mode);
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(uint32_t)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SensorEvent data invalid\n");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_PEDOMETER) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sensorTypeId %d is not PEDOMETER\n", event->sensorTypeId);
        return;
    }
    uint32_t steps = *(uint32_t *)event->data;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "steps:%u", steps);
}

typedef struct {
    SensorTypeId sensorTypeId;
    SensorUser user;
    bool activated;
    bool subscribed;
} SensorDev;

typedef struct {
    bool isStarted;
    SensorInfo *sensorInfo;
    int32_t snsCnt;
    SensorDev sensorDev[SUPPORT_SENSOR_CNT];
} SensorSampleContext;

static SensorSampleContext g_sensorContext = {
    .isStarted = false,
    .sensorInfo = NULL,
    .snsCnt = 0,
    .sensorDev =
        {
            {SENSOR_TYPE_ID_ACCELEROMETER, {"AT_Test", RecordAccelSensorCallback, NULL}, false, false},
            {SENSOR_TYPE_ID_BAROMETER, {"AT_Test", RecordBarSensorCallback, NULL}, false, false},
            {SENSOR_TYPE_ID_GYROSCOPE, {"AT_Test", RecordCyroSensorCallback, NULL}, false, false},
            {SENSOR_TYPE_ID_HEART_RATE, {"AT_Test", RecordHrSensorCallback, NULL}, false, false},
            {SENSOR_TYPE_ID_WEAR_DETECTION, {"AT_Test", RecordObsSensorCallback, NULL}, false, false},
            {SENSOR_TYPE_ID_PEDOMETER, {"AT_Test", RecordScSensorCallback, NULL}, false, false},
        },
};

static SensorDev *GetSensorIdBySensorDevIndex(SensorTypeId sensorTypeId)
{
    if (g_sensorContext.snsCnt <= 0 || g_sensorContext.sensorInfo == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "must GetAllSensors first\n");
        return NULL;
    }
    bool found = false;
    for (int32_t index = 0; index < g_sensorContext.snsCnt; index++) {
        if (g_sensorContext.sensorInfo[index].sensorTypeId == sensorTypeId) {
            found = true;
            break;
        }
    }
    if (!found) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "not found sensorTypeId:%d\n", sensorTypeId);
        return NULL;
    }
    for (uint32_t index = 0; index < ARRAY_SIZE(g_sensorContext.sensorDev); index++) {
        if (g_sensorContext.sensorDev[index].sensorTypeId == sensorTypeId) {
            return &g_sensorContext.sensorDev[index];
        }
    }
    return NULL;
}

static int32_t SensorGetAllSensors(SensorTypeId sensorTypeId)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SensorGetAllSensors\n");
    (void)sensorTypeId;
    if (g_sensorContext.snsCnt != 0 || g_sensorContext.sensorInfo != NULL) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "sensorInfo is Got\n");
        return SENSOR_OK;
    }
    int32_t errCode = GetAllSensors(&g_sensorContext.sensorInfo, &g_sensorContext.snsCnt);
    if (errCode != SENSOR_OK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sensorInfo failed:%d\n", errCode);
        return errCode;
    }
    return SENSOR_OK;
}

static int32_t SensorSubscribe(SensorTypeId sensorTypeId)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SensorSubscribe :%d\n", sensorTypeId);
    SensorDev *sensorDev = GetSensorIdBySensorDevIndex(sensorTypeId);
    if (sensorDev == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetSensorIdBySensorDevIndex failed\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    if (sensorDev->subscribed) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, " ACCELEROMETER subscribed\n");
        return SENSOR_OK;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SubscribeSensor\n");
    int32_t errCode = SubscribeSensor(sensorTypeId, &sensorDev->user);
    if (errCode != SENSOR_OK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SubscribeSensor failed:%d\n", errCode);
        return errCode;
    }
    sensorDev->subscribed = true;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SensorSubscribe end\n");
    return SENSOR_OK;
}

static int32_t SensorActivate(SensorTypeId sensorTypeId)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SensorActivate :%d\n", sensorTypeId);
    SensorDev *sensorDev = GetSensorIdBySensorDevIndex(sensorTypeId);
    if (sensorDev == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetSensorIdBySensorDevIndex failed\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    if (sensorDev->activated) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Activated\n");
        return SENSOR_OK;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ActivateSensor \n");
    int32_t errCode = ActivateSensor(sensorTypeId, &sensorDev->user);
    if (errCode != SENSOR_OK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ActivateSensor failed:%d\n", errCode);
        return errCode;
    }
    sensorDev->activated = true;
    return SENSOR_OK;
}

static int32_t SensorUnsubscribe(SensorTypeId sensorTypeId)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SensorUnsubscribe :%d\n", sensorTypeId);
    SensorDev *sensorDev = GetSensorIdBySensorDevIndex(sensorTypeId);
    if (sensorDev == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetSensorIdBySensorDevIndex failed\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    if (!sensorDev->subscribed) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "not subscribed\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    int32_t errCode = UnsubscribeSensor(sensorTypeId, &sensorDev->user);
    if (errCode != SENSOR_OK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "UnsubscribeSensor failed:%d\n", errCode);
        return errCode;
    }
    sensorDev->subscribed = false;
    return SENSOR_OK;
}

static int32_t SensorDeactivate(SensorTypeId sensorTypeId)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SensorDeactivate :%d\n", sensorTypeId);
    SensorDev *sensorDev = GetSensorIdBySensorDevIndex(sensorTypeId);
    if (sensorDev == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetSensorIdBySensorDevIndex failed\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    if (!sensorDev->activated) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "not activated\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    int32_t errCode = DeactivateSensor(sensorTypeId, &sensorDev->user);
    if (errCode != SENSOR_OK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "DeactivateSensor failed:%d\n", errCode);
        return errCode;
    }
    sensorDev->activated = false;
    return SENSOR_OK;
}

static int32_t SensorGetData(SensorTypeId sensorTypeId)
{
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "getData :%d\n", sensorTypeId);
    SensorDev *sensorDev = GetSensorIdBySensorDevIndex(sensorTypeId);
    if (sensorDev == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetSensorIdBySensorDevIndex failed\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    SensorData *sensorData = (SensorData *)malloc(sizeof(SensorData));
    if (sensorData == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc failed\n");
        return SENSOR_ERROR_UNKNOWN;
    }
    int32_t errCode = GetSensorData(sensorTypeId, &sensorDev->user, sensorData);
    if (errCode != SENSOR_OK) {
        free(sensorData);
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetSensorData failed:%d\n", errCode);
        return errCode;
    }
    AccelData *accData = (AccelData *)sensorData->data;
    for (uint32_t i = 0; i < sensorData->batchCnt; i++) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
            "SensorData dataX:%x, datY:%x, dataZ:%x\n",
            accData[i].axisX,
            accData[i].axisY,
            accData[i].axisZ);
    }
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SensorData batchCnt:%u timestamp:%lld\n",
        sensorData->batchCnt, sensorData->timestamp);
    free(sensorData);
    return SENSOR_OK;
}

static SensorTypeId GetSensorIdByParam(const char *argv)
{
    if (argv == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "argv invalid\n");
        return SENSOR_TYPE_ID_MAX;
    }
    for (uint32_t index = 0; index < ARRAY_SIZE(g_sensorIdMap); index++) {
        if (strcmp(argv, g_sensorIdMap[index].cmd) == 0) {
            return g_sensorIdMap[index].sensorTypeId;
        }
    }
    return SENSOR_TYPE_ID_MAX;
}

typedef struct {
    const char **argv;
    int32_t argc;
} Params;

static void *SensorTaskEntry(void *argv)
{
    Params parms = *(Params *)argv;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "enter SensorTaskEntry :argc:%d argv:%s\n", parms.argc, parms.argv[0]);
    for (uint32_t i = 0; i < ARRAY_SIZE(g_sensorFuncs); i++) {
        if (strcmp(parms.argv[0], g_sensorFuncs[i].cmd) == 0) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "execute argv :%s\n", parms.argv[1]);
            SensorTypeId sensorTypeId = GetSensorIdByParam(parms.argv[1]);
            int32_t ret = g_sensorFuncs[i].func(sensorTypeId);
            WEARABLE_LOGI(
                WEARABLE_LOG_MODULE_APP, "%s execute %s\n", g_sensorFuncs[i].cmd, (ret != 0) ? "failed" : "success");
            break;
        }
    }
    osDelay(300);  // sleep 300ms
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SensorSample end\n");
    return nullptr;
}

int32_t SensorSample(const char *argv[], int32_t argc)
{
    if (argv == nullptr || argc < 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "argv is invalid\n");
        return -1;
    }
    Params parms = {argv, argc};
    osThreadAttr_t taskAttr = {"SensorTaskEntry", 0, NULL, 0, NULL, 0x2000, (osPriority_t)(osPriorityHigh), 0, 0};
    // taskAttr.stack_mem = memalign(16, 0x2000);
    osThreadNew((osThreadFunc_t)SensorTaskEntry, &parms, &taskAttr);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SensorSample exit\n");
    return 0;
}

#ifdef __cplusplus
};
#endif
