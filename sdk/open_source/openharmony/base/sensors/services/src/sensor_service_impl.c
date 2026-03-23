/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "sensor_service_impl.h"

#include <securec.h>
#include <stdlib.h>
#include <string.h>
#include "sensor_service.h"
#include "sensor_type.h"

static struct SensorInformation *g_sensorLists;
static int32_t g_sensorListsLength;
const struct SensorInterface *g_sensorDevice;
static Identity g_svcIdentity;

static int32_t InitSensorList(void)
{
#ifdef HAS_HDI_SENSOR_LITE_PRAT
    if (g_sensorDevice == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: %s g_sensorDevice is NULL", SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    int32_t ret = g_sensorDevice->GetAllSensors(&g_sensorLists, &g_sensorListsLength);
    if ((ret != 0) || (g_sensorLists == NULL)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: %s getAllSensors failed, ret: %d", SENSOR_SERVICE, __func__, ret);
        return SENSOR_ERROR_INVALID_PARAM;
    }
#endif  // HAS_HDI_SENSOR_LITE_PRAT
    HILOG_DEBUG(HILOG_MODULE_SEN, "[SERVICE:%s]: sensorListsLength:%d", SENSOR_SERVICE, g_sensorListsLength);
    for (int32_t i = 0; i < g_sensorListsLength; i++) {
        HILOG_DEBUG(HILOG_MODULE_SEN,
            "[SERVICE:%s]: index:%d  sensorName:%s, sensorTypeId:%d",
            SENSOR_SERVICE,
            i,
            g_sensorLists[i].sensorName,
            g_sensorLists[i].sensorTypeId);
    }
    return SENSOR_OK;
}

const char *SENSOR_GetName(Service *service)
{
    (void)service;
    return SENSOR_SERVICE;
}

BOOL Initialize(Service *service, Identity identity)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
    g_svcIdentity = identity;
#ifdef HAS_HDI_SENSOR_LITE_PRAT
    g_sensorDevice = NewSensorInterfaceInstance();
    if (g_sensorDevice == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: %s g_sensorDevice is NULL", SENSOR_SERVICE, __func__);
    }
#endif  // HAS_HDI_SENSOR_LITE_PRAT
    return TRUE;
}

BOOL MessageHandle(Service *service, Request *msg)
{
    return TRUE;
}

TaskConfig GetTaskConfig(Service *service)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
    TaskConfig config = {LEVEL_HIGH, PRI_BELOW_NORMAL, TASK_CONFIG_STACK_SIZE, TASK_CONFIG_QUEUE_SIZE, SHARED_TASK};
    return config;
}

int32_t GetAllSensorsImpl(SensorInfo **sensorInfo, int32_t *count)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
    if ((sensorInfo == NULL) || (count == NULL)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: %s sensorInfo or count is null", SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    if ((g_sensorLists == NULL) || (g_sensorListsLength <= 0)) {
        if (InitSensorList() != 0) {
            HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: %s initSensorList failed!", SENSOR_SERVICE, __func__);
            return SENSOR_ERROR_INVALID_PARAM;
        }
    }
    *sensorInfo = (SensorInfo *)g_sensorLists;
    *count = g_sensorListsLength;
    return SENSOR_OK;
}

bool CheckSensorTypeId(int32_t sensorTypeId)
{
    if ((sensorTypeId >= SENSOR_TYPE_ID_MAX) || (sensorTypeId < SENSOR_TYPE_ID_NONE)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: sensorId: %d is invalid", SENSOR_SERVICE, sensorTypeId);
        return false;
    }
    if ((g_sensorLists == NULL) || (g_sensorListsLength <= 0)) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: SensorList invalid!", SENSOR_SERVICE);
        return false;
    }
    for (int32_t i = 0; i < g_sensorListsLength; i++) {
        if (sensorTypeId == g_sensorLists[i].sensorTypeId) {
            return true;
        }
    }
    return false;
}

int32_t ActivateSensorImpl(int32_t sensorTypeId, const SensorUser *user)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
    if (!CheckSensorTypeId(sensorTypeId)) {
        return SENSOR_ERROR_INVALID_ID;
    }
    if (user == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: %s user is NULL", SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
#ifdef HAS_HDI_SENSOR_LITE_PRAT
    if (g_sensorDevice == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: %s g_sensorDevice is NULL", SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    int32_t ret = g_sensorDevice->Enable(sensorTypeId);
    if (ret != 0) {
        HILOG_ERROR(
            HILOG_MODULE_SEN, "[SERVICE:%s]: %s ActivateSensor sensor failed, ret: %d", SENSOR_SERVICE, __func__, ret);
        return ret;
    }
#endif  // HAS_HDI_SENSOR_LITE_PRAT
    return SENSOR_OK;
}

int32_t DeactivateSensorImpl(int32_t sensorTypeId, const SensorUser *user)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
    if (!CheckSensorTypeId(sensorTypeId)) {
        return SENSOR_ERROR_INVALID_ID;
    }
    if (user == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: %s user is NULL", SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
#ifdef HAS_HDI_SENSOR_LITE_PRAT
    if (g_sensorDevice == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: %s g_sensorDevice is NULL", SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    int32_t ret = g_sensorDevice->Disable(sensorTypeId);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_SEN,
            "[SERVICE:%s]: %s DeactivateSensor sensor failed, ret: %d",
            SENSOR_SERVICE,
            __func__,
            ret);
        return ret;
    }
#endif  // HAS_HDI_SENSOR_LITE_PRAT
    return SENSOR_OK;
}

int32_t SetBatchImpl(int32_t sensorTypeId, const SensorUser *user, int64_t samplingInterval, int64_t reportInterval)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
    if (!CheckSensorTypeId(sensorTypeId)) {
        return SENSOR_ERROR_INVALID_ID;
    }
    if ((samplingInterval < 0) || (reportInterval < 0)) {
        HILOG_ERROR(HILOG_MODULE_SEN,
            "[SERVICE:%s]: samplingInterval: %lld or reportInterval: %lld is invalid",
            SENSOR_SERVICE,
            samplingInterval,
            reportInterval);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    return SENSOR_OK;
}

int32_t SubscribeSensorImpl(int32_t sensorTypeId, const SensorUser *user)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
    if (!CheckSensorTypeId(sensorTypeId)) {
        return SENSOR_ERROR_INVALID_ID;
    }
    if (user == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: %s user is NULL", SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
#ifdef HAS_HDI_SENSOR_LITE_PRAT
    if (g_sensorDevice == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: %s g_sensorDevice is NULL", SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    int32_t ret = g_sensorDevice->Register(sensorTypeId, (RecordDataCallback)user->callback);
    if (ret != 0) {
        HILOG_ERROR(
            HILOG_MODULE_SEN, "[SERVICE:%s]: %s register sensor user failed, ret: %d", SENSOR_SERVICE, __func__, ret);
        return SENSOR_ERROR_INVALID_PARAM;
    }
#endif  // HAS_HDI_SENSOR_LITE_PRAT
    return SENSOR_OK;
}

int32_t UnsubscribeSensorImpl(int32_t sensorTypeId, const SensorUser *user)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
    if (!CheckSensorTypeId(sensorTypeId)) {
        return SENSOR_ERROR_INVALID_ID;
    }
    if (user == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: %s user is NULL", SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
#ifdef HAS_HDI_SENSOR_LITE_PRAT
    if (g_sensorDevice == NULL) {
        HILOG_ERROR(HILOG_MODULE_SEN, "[SERVICE:%s]: %s g_sensorDevice is NULL", SENSOR_SERVICE, __func__);
        return SENSOR_ERROR_INVALID_PARAM;
    }
    int32_t ret = g_sensorDevice->Unregister(sensorTypeId, (RecordDataCallback)user->callback);
    if (ret != 0) {
        HILOG_ERROR(
            HILOG_MODULE_SEN, "[SERVICE:%s]: %s unregister sensor failed, ret: %d", SENSOR_SERVICE, __func__, ret);
        return SENSOR_ERROR_INVALID_PARAM;
    }
#endif  // HAS_HDI_SENSOR_LITE_PRAT
    return SENSOR_OK;
}

int32_t SetModeImpl(int32_t sensorTypeId, const SensorUser *user, int32_t mode)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
    if (!CheckSensorTypeId(sensorTypeId)) {
        return SENSOR_ERROR_INVALID_ID;
    }
    return SENSOR_OK;
}

int32_t SetOptionImpl(int32_t sensorTypeId, const SensorUser *user, int32_t option)
{
    HILOG_DEBUG(HILOG_MODULE_SEN, "[SERVICE:%s]: %s begin", SENSOR_SERVICE, __func__);
   if (!CheckSensorTypeId(sensorTypeId)) {
        return SENSOR_ERROR_INVALID_ID;
    }
    return SENSOR_OK;
}

// ohos-hisi begin
int32_t GetSensorDataImpl(int32_t sensorId, const SensorUser *user, void *sensorData)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "[SERVICE:%s]: %s begin",
        SENSOR_SERVICE, __func__);
    if (!CheckSensorTypeId(sensorId)) {
        return SENSOR_ERROR_INVALID_ID;
    }
    return g_sensorDevice->GetSensorData(sensorId, (SensorData *)sensorData);
}
// ohos-hisi end