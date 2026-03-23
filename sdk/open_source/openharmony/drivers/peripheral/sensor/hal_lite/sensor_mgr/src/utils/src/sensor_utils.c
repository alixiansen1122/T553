/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor utils impl
 * Author: Media Software Group
 * Create: 2025-06-06
 */

#include "sensor_utils.h"
#include <sys/time.h>
#include "sensor_log.h"
#include "sensor_task.h"
#include "sensor_memory.h"
#include "sensor_common.h"

#define MS_TO_US 1000

typedef struct {
    enum SensorTypeTag type;
    const char* simpleName;
} SensorSimpleNameInfo;

typedef struct {
    enum SensorTypeTag type;
    uint32_t taskId;
} SensorTaskTable;

static SensorTaskTable g_registeredSensorTaskTable[SENSOR_MAX_NUM] = {0};

static struct SensorInformation g_sensorList[SENSOR_MAX_NUM] = {0};
static Sensor g_sensorPool[SENSOR_MAX_NUM] = {0};
static enum SensorTypeTag g_registeredVSensorTable[VSENSOR_MAX_NUM] = {0};

const SensorSimpleNameInfo g_sensorSimpleNameInfoDefines[] = {
    {SENSOR_TYPE_NONE, "none"},
    {SENSOR_TYPE_ACCELEROMETER, "acc"},
    {SENSOR_TYPE_GYROSCOPE, "gyro"},
    {SENSOR_TYPE_PHOTOPLETHYSMOGRAPH, "ppg"},
    {SENSOR_TYPE_ELECTROCARDIOGRAPH, "ecg"},
    {SENSOR_TYPE_AMBIENT_LIGHT, "amblitght"},
    {SENSOR_TYPE_MAGNETIC_FIELD, "magn"},
    {SENSOR_TYPE_CAPACITIVE, "cap"},
    {SENSOR_TYPE_BAROMETER, "bar"},
    {SENSOR_TYPE_TEMPERATURE, "temp"},
    {SENSOR_TYPE_HALL, "hall"},
    {SENSOR_TYPE_GESTURE, "gest"},
    {SENSOR_TYPE_PROXIMITY, "prox"},
    {SENSOR_TYPE_HUMIDITY, "humi"},
    {SENSOR_TYPE_PHYSICAL_MAX, "phymax"},
    {SENSOR_TYPE_ORIENTATION, "ori"},
    {SENSOR_TYPE_GRAVITY, "grav"},
    {SENSOR_TYPE_LINEAR_ACCELERATION, "lacc"},
    {SENSOR_TYPE_ROTATION_VECTOR, "rvec"},
    {SENSOR_TYPE_AMBIENT_TEMPERATURE, "atemp"},
    {SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED, "magnu"},
    {SENSOR_TYPE_GAME_ROTATION_VECTOR, "grvec"},
    {SENSOR_TYPE_GYROSCOPE_UNCALIBRATED, "gyrou"},
    {SENSOR_TYPE_SIGNIFICANT_MOTION, "smo"},
    {SENSOR_TYPE_PEDOMETER_DETECTION, "pedde"},
    {SENSOR_TYPE_PEDOMETER, "ped"},
    {SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR, "grvec"},
    {SENSOR_TYPE_HEART_RATE, "hr"},
    {SENSOR_TYPE_DEVICE_ORIENTATION, "devor"},
    {SENSOR_TYPE_WEAR_DETECTION, "wd"},
    {SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED, "accu"},
    {SENSOR_TYPE_VSENSOR_ACCELEROMETER_ALG_SIMU, "accalgsimu"},
 
    {SENSOR_TYPE_MAX, "max"}
};

void SensorUtilsInit(void)
{
}

char *SensorUtilsGetSimpleName(enum SensorTypeTag type)
{
    char* ret = NULL;
    for (uint32_t i = 0; i < (sizeof(g_sensorSimpleNameInfoDefines) / sizeof(SensorSimpleNameInfo)); i++) {
        if (g_sensorSimpleNameInfoDefines[i].type == type) {
            ret = (char*)(g_sensorSimpleNameInfoDefines[i].simpleName);
            break;
        }
    }
    return ret;
}

Sensor *SensorUtilsFindSensor(enum SensorTypeTag type)
{
    CHECK_SENSOR_TYPE_ILLEGAL_RETURN_RET(type, NULL);

    for (uint32_t i = 0; i < SENSOR_MAX_NUM; i++) {
        Sensor *sensor = &(g_sensorPool[i]);
        if (type == sensor->sensorInfo.type) {
            return sensor;
        }
    }

    return NULL;
}

Sensor *SensorUtilsRequestSensor(enum SensorTypeTag type)
{
    Sensor *sensor = SensorUtilsFindSensor(type);
    if (sensor != NULL) {
        /* sensor已存在，不允许重复注册 */
        HILOG_ERROR(HILOG_MODULE_SEN, "[util] find sensor:%d exit!", sensor->sensorInfo.type);
        return NULL;
    }

    for (uint32_t i = 0; i < SENSOR_MAX_NUM; i++) {
        sensor = &(g_sensorPool[i]);
        if (sensor->sensorInfo.type == SENSOR_TYPE_NONE) {
            return sensor;
        }
    }
    HILOG_WARN(HILOG_MODULE_SEN, "[util] find sensor:%d Exceed max!", sensor->sensorInfo.type);
    return NULL;
}

/* 获取全部sensor */
int32_t SensorUtilsGetAllSensors(struct SensorInformation **sensorInfo, int32_t *count)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "in!");
    if (sensorInfo == NULL || count == NULL) {
        return SENSOR_ENTRY_PARA_ERROR;
    }
    *count = 0;
    for (uint32_t i = 0; i < SENSOR_MAX_NUM; i++) {
        HILOG_DEBUG(HILOG_MODULE_SEN, "index:%d sensorID:%d sensorTypeId:%d", i, g_sensorList[i].sensorId, g_sensorList[i].sensorTypeId);
        if (g_sensorList[i].sensorTypeId == (int32_t)SENSOR_TYPE_NONE) {
            break;
        }
        (*count)++;
    }
    *sensorInfo = g_sensorList;
    HILOG_DEBUG(HILOG_MODULE_SEN, "find sensorlist:%d! list addr:%x", *count, g_sensorList);
    return SENSOR_OK;
}

static struct SensorInformation *SensorUtilsFindSensorsList(enum SensorTypeTag type)
{
    CHECK_SENSOR_TYPE_ILLEGAL_RETURN_RET(type, NULL);

    for (uint32_t i = 0; i < SENSOR_MAX_NUM; i++) {
        struct SensorInformation *sensorList = &(g_sensorList[i]);
        if (type == (enum SensorTypeTag)sensorList->sensorTypeId) {
            return sensorList;
        }
    }

    return NULL;
}

static struct SensorInformation *SensorUtilsRequestSensorList(enum SensorTypeTag type)
{
    struct SensorInformation *sensorList = SensorUtilsFindSensorsList(type);
    if (sensorList != NULL) {
        /* sensor已存在，不允许重复注册 */
        HILOG_ERROR(HILOG_MODULE_SEN, "[util] find sensorlist:%d exit!", sensorList->sensorTypeId);
        return NULL;
    }

    for (uint32_t i = 0; i < SENSOR_MAX_NUM; i++) {
        sensorList = &(g_sensorList[i]);
        if (sensorList->sensorTypeId == (int32_t)SENSOR_TYPE_NONE) {
            return sensorList;
        }
    }
    return NULL;
}

int32_t SensorUtilsAddHsensorList(HsensorScb *hsensorScb)
{
    struct SensorInformation *sensorList = SensorUtilsRequestSensorList(hsensorScb->type);
    if (sensorList == NULL) {
        return SENSOR_ERROR;
    }

    sensorList->sensorTypeId = hsensorScb->type;
    // sensorList->sensorName = hsensorScb->hsensor->name;
    return SENSOR_OK;
}

int32_t SensorUtilsAddVsensorList(enum SensorTypeTag type)
{
    struct SensorInformation *sensorList = SensorUtilsRequestSensorList(type);
    if (sensorList == NULL) {
        return SENSOR_ERROR;
    }

    sensorList->sensorTypeId = type;
    return SENSOR_OK;
}

HsensorListener *SensorUtilsRequestListener(void)
{
    return (HsensorListener *)SensorMalloc(sizeof(HsensorListener));
}

void SensorUtilsFreeListener(HsensorListener *listener)
{
    if (listener) {
        SensorFree(listener);
    }
}

/* 用户信息有效性检查 */
int32_t SensorUtilsUserInfoCheck(SensorPersonInfo *info)
{
    /* 参数检查 */
    if ((info->weight > MAX_USER_WEIGHT) || (info->weight < MIN_USER_WEIGHT)) {
        HILOG_INFO(HILOG_MODULE_SEN, "user weight out range!");
        info->weight = DEFAULT_USER_WEIGHT;
    }

    if ((info->height > MAX_USER_HEIGHT) || (info->height < MIN_USER_HEIGHT)) {
        HILOG_INFO(HILOG_MODULE_SEN, "user height out range!");
        info->height = DEFAULT_USER_HEIGHT;
    }

    if ((info->age > MAX_USER_AGE) || (info->age == 0)) {
        HILOG_INFO(HILOG_MODULE_SEN, "user age out range!");
        info->age = DEFAULT_USER_AGE;
    }

    if (info->gender > ALG_USER_SEX_UNKNOWN) {
        HILOG_INFO(HILOG_MODULE_SEN, "user gender out range!");
        info->gender = ALG_USER_SEX_MAN;
    }

    return SENSOR_OK;
}

static int32_t SensorUtilsVsensorRelyCheck(const RelySensorsInfo *relySensors)
{
    if (relySensors->relySensorCnt == 0) {
        return SENSOR_ENTRY_VSENSOR_RELY_NULL;
    }
    return SENSOR_OK;
}

bool SensorUtilsCheckHSensorType(enum SensorTypeTag type)
{
    if ((type == SENSOR_TYPE_NONE) || (type >= SENSOR_TYPE_VSENSOR_EXPAND_START)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type :%d illegal", type);
        return false;
    }
    return true;
}

bool SensorUtilsCheckVSensorType(enum SensorTypeTag type)
{
    if ((type <= SENSOR_TYPE_VSENSOR_EXPAND_START) || (type >= SENSOR_TYPE_MAX)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "type :%d illegal", type);
        return false;
    }
    return true;
}

void SensorUtilsRegisterVsensorType(enum SensorTypeTag type)
{
    CHECK_SENSOR_TYPE_ILLEGAL_RETURN(type);

    for (uint8_t i = 0; i < VSENSOR_MAX_NUM; i++) {
        if (type == g_registeredVSensorTable[i]) {
            return;
        }
        if (g_registeredVSensorTable[i] == SENSOR_TYPE_NONE) {
            g_registeredVSensorTable[i] = type;
            return;
        }
    }
}

bool SensorUtilsIsVSensor(enum SensorTypeTag type)
{
    for (uint8_t i = 0; i < VSENSOR_MAX_NUM; i++) {
        if (type == g_registeredVSensorTable[i]) {
            return true;
        }
    }
    return false;
}

int32_t SensorUtilsHsensorCheck(const Hsensor *hsensor)
{
    CHECK_NULL_PTR_RETURN_VALUE(hsensor, SENSOR_ENTRY_HSENSOR_NULL);
    CHECK_NULL_PTR_RETURN_VALUE(hsensor->name, SENSOR_ENTRY_HSENSOR_NAME_NULL);
    CHECK_NULL_PTR_RETURN_VALUE(hsensor->init, SENSOR_ENTRY_HSENSOR_FUNC_NULL);
    CHECK_NULL_PTR_RETURN_VALUE(hsensor->deinit, SENSOR_ENTRY_HSENSOR_FUNC_NULL);
    CHECK_NULL_PTR_RETURN_VALUE(hsensor->close, SENSOR_ENTRY_HSENSOR_FUNC_NULL);
    CHECK_NULL_PTR_RETURN_VALUE(hsensor->read, SENSOR_ENTRY_HSENSOR_FUNC_NULL);
    CHECK_NULL_PTR_RETURN_VALUE(hsensor->ioctl, SENSOR_ENTRY_HSENSOR_FUNC_NULL);
    if (hsensor->caps.maxBatchCount == 0 || hsensor->caps.sampleElementlen == 0) {
        HILOG_ERROR(HILOG_MODULE_SEN, "maxBatchCount :%u sampleElementlen :%u illegal",
            hsensor->caps.maxBatchCount, hsensor->caps.sampleElementlen);
        return SENSOR_HSENSOR_PERIOD_BATCH_ERR;
    }

    if (!SensorUtilsCheckHSensorType(hsensor->type)) {
        return SENSOR_ENTRY_SENSOR_TYPE_ILLEGAL;
    }

    return SENSOR_OK;
}

int32_t SensorUtilsVsensorCheck(const Vsensor *vsensor)
{
    CHECK_NULL_PTR_RETURN_VALUE(vsensor, SENSOR_ENTRY_VSENSOR_NULL);
    CHECK_NULL_PTR_RETURN_VALUE(vsensor->name, SENSOR_ENTRY_VSENSOR_NAME_NULL);
    CHECK_NULL_PTR_RETURN_VALUE(vsensor->relySensors, SENSOR_ENTRY_VSENSOR_RELY_NULL);

    CHECK_NULL_PTR_RETURN_VALUE(vsensor->registDataCb, SENSOR_ENTRY_VSENSOR_FUNC_NULL);
    CHECK_NULL_PTR_RETURN_VALUE(vsensor->unRegistDataCb, SENSOR_ENTRY_VSENSOR_FUNC_NULL);
    CHECK_NULL_PTR_RETURN_VALUE(vsensor->open, SENSOR_ENTRY_VSENSOR_FUNC_NULL);
    CHECK_NULL_PTR_RETURN_VALUE(vsensor->close, SENSOR_ENTRY_VSENSOR_FUNC_NULL);
    CHECK_NULL_PTR_RETURN_VALUE(vsensor->process, SENSOR_ENTRY_VSENSOR_FUNC_NULL);

    if (!SensorUtilsCheckVSensorType(vsensor->type)) {
        return SENSOR_ENTRY_SENSOR_TYPE_ILLEGAL;
    }

    int32_t ret = SensorUtilsVsensorRelyCheck(vsensor->relySensors);
    if (ret != SENSOR_OK) {
        return ret;
    }

    return SENSOR_OK;
}

void SensorUtilsRegisterTaskTable(enum SensorTypeTag type, uint32_t taskId)
{
    CHECK_SENSOR_TYPE_ILLEGAL_RETURN(type);
    for (uint32_t i = 0; i < SENSOR_MAX_NUM; i++) {
        if (type == g_registeredSensorTaskTable[i].type) {
            g_registeredSensorTaskTable[i].taskId = taskId;
            return;
        }
    }

    for (uint32_t i = 0; i < SENSOR_MAX_NUM; i++) {
        if (g_registeredSensorTaskTable[i].type == SENSOR_TYPE_NONE) {
            g_registeredSensorTaskTable[i].type = type;
            g_registeredSensorTaskTable[i].taskId = taskId;
            return;
        }
    }
}

void SensorUtilsUnRegisterTaskTable(enum SensorTypeTag type, uint32_t taskId)
{
    CHECK_SENSOR_TYPE_ILLEGAL_RETURN(type);
    for (uint32_t i = 0; i < SENSOR_MAX_NUM; i++) {
        if (type == g_registeredSensorTaskTable[i].type && taskId == g_registeredSensorTaskTable[i].taskId) {
            g_registeredSensorTaskTable[i].taskId = 0;
            g_registeredSensorTaskTable[i].type = SENSOR_TYPE_NONE;
            return;
        }
    }
}

uint32_t SensorUtilsGetRegisteredTaskId(enum SensorTypeTag type)
{
    for (uint32_t i = 0; i < SENSOR_MAX_NUM; i++) {
        if (type == g_registeredSensorTaskTable[i].type) {
            return g_registeredSensorTaskTable[i].taskId;
        }
    }
    return SENSOR_FAILURE;
}

enum SensorTypeTag SensorUtilsGetRegisteredType(uint32_t taskId)
{
    for (uint32_t i = 0; i < SENSOR_MAX_NUM; i++) {
        if (taskId == g_registeredSensorTaskTable[i].taskId) {
            return g_registeredSensorTaskTable[i].type;
        }
    }
    return SENSOR_TYPE_MAX;
}

bool SensorUtilsSensorIsRegistered(enum SensorTypeTag type)
{
    for (uint32_t i = 0; i < SENSOR_MAX_NUM; i++) {
        Sensor *sensor = &(g_sensorPool[i]);
        if (sensor->sensorInfo.type == type) {
            return true;
        }
    }
    return false;
}

uint64_t SensorUtilsGetRtcTimeMs(void)
{
    struct timeval tv;
    (void)gettimeofday(&tv, NULL);
    return (tv.tv_sec * MS_TO_US + tv.tv_usec / MS_TO_US);
}
