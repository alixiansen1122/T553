/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include <new>
#include "ace_log.h"
#include "js_app_context.h"
#include "js_async_work.h"
#include "product_adapter.h"
#include <securec.h>
#include "sensor_module.h"
#include "global.h"
#include "screen.h"

namespace OHOS {
namespace ACELite {

const char *const SensorModule::STEPS = "steps";
const uint32_t SensorModule::DEFAULT_STEPS = 2000;  // 2000 steps
const char *const SensorModule::PRESSURE = "pressure";
const uint32_t SensorModule::DEFAULT_PRESSURE = 101300;  // 1.013×10^5 Pa
const char *const SensorModule::HEARTRATE = "heartRate";
const uint32_t SensorModule::DEFAULT_HEARTRATE = 80;  // 80/min
const char *const SensorModule::ISONBODY = "value";
const bool SensorModule::DEFAULT_ISONBODY = true;
const uint32_t SensorModule::X = 80;
const uint32_t SensorModule::Y = 80;
const uint32_t SensorModule::Z = 80;

SensorInfo *SensorModule::sensorInfo_ = nullptr;
int32_t SensorModule::sensorCnt_ = 0;

static void RecordAccelSensorCallback(SensorEvent *event);
static void RecordBarSensorCallback(SensorEvent *event);
static void RecordGyroSensorCallback(SensorEvent *event);
static void RecordHrSensorCallback(SensorEvent *event);
static void RecordObsSensorCallback(SensorEvent *event);
static void RecordScSensorCallback(SensorEvent *event);

struct SensorDev {
    SensorTypeId sensorTypeId;
    SensorUser user;
    bool subscribed;
    SensorParams sensorParams;
};

static struct SensorDev g_sensorDev[] = {
    {SENSOR_TYPE_ID_ACCELEROMETER, {"SensorModule", RecordAccelSensorCallback, nullptr}, false},
    {SENSOR_TYPE_ID_BAROMETER, {"SensorModule", RecordBarSensorCallback, nullptr}, false},
    {SENSOR_TYPE_ID_GYROSCOPE, {"SensorModule", RecordGyroSensorCallback, nullptr}, false},
    {SENSOR_TYPE_ID_HEART_RATE, {"SensorModule", RecordHrSensorCallback, nullptr}, false},
    {SENSOR_TYPE_ID_WEAR_DETECTION, {"SensorModule", RecordObsSensorCallback, nullptr}, false},
    {SENSOR_TYPE_ID_PEDOMETER, {"SensorModule", RecordScSensorCallback, nullptr}, false},
};

static struct SensorDev *GetSensorIdBySensorDevIndex(SensorTypeId sensorTypeId)
{
    if (SensorModule::sensorCnt_ <= 0 || SensorModule::sensorInfo_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "must GetAllSensors first");
        return nullptr;
    }
    bool found = false;
    for (int32_t index = 0; index < SensorModule::sensorCnt_; index++) {
        if (SensorModule::sensorInfo_[index].sensorTypeId == sensorTypeId) {
            found = true;
            break;
        }
    }
    if (!found) {
        HILOG_ERROR(HILOG_MODULE_ACE, "not found sensorTypeId:%d", sensorTypeId);
        return nullptr;
    }
    for (uint32_t index = 0; index < ARRAY_SIZE(g_sensorDev); index++) {
        if (g_sensorDev[index].sensorTypeId == sensorTypeId) {
            return &g_sensorDev[index];
        }
    }
    return nullptr;
}

void SensorModule::SuccessCallBack(const JSIValue thisVal, const JSIValue args, JSIValue jsiValue, bool flag)
{
    if (JSI::ValueIsUndefined(args)) {
        return;
    }
    JSIValue success = JSI::GetNamedProperty(args, CB_SUCCESS);
    JSIValue complete = JSI::GetNamedProperty(args, CB_COMPLETE);
    if (!JSI::ValueIsUndefined(success)) {
        if (JSI::ValueIsUndefined(jsiValue)) {
            JSI::CallFunction(success, thisVal, nullptr, 0);
        } else {
            JSI::CallFunction(success, thisVal, &jsiValue, ARGC_ONE);
        }
    }
    if (!JSI::ValueIsUndefined(complete) && flag) {
        JSI::CallFunction(complete, thisVal, nullptr, 0);
    }
    JSI::ReleaseValueList(success, complete, ARGS_END);
}

void SensorModule::FailCallBack(const JSIValue thisVal, const JSIValue args, int ret, bool flag)
{
    if (JSI::ValueIsUndefined(args)) {
        return;
    }
    JSIValue errInfo;
    errInfo = JSI::CreateString("Sensor faild");
    JSIValue errCode = JSI::CreateNumber(ret);
    JSIValue fail = JSI::GetNamedProperty(args, CB_FAIL);
    JSIValue complete = JSI::GetNamedProperty(args, CB_COMPLETE);
    JSIValue argv[ARGC_TWO] = {errInfo, errCode};
    if (!JSI::ValueIsUndefined(fail)) {
        JSI::CallFunction(fail, thisVal, argv, ARGC_TWO);
    }
    if (!JSI::ValueIsUndefined(complete) && flag) {
        JSI::CallFunction(complete, thisVal, nullptr, 0);
    }
    JSI::ReleaseValueList(fail, complete, errInfo, errCode, ARGS_END);
}

void RecordAccelSensorCallbackDispatch(void *arg)
{
    SensorEvent *event = reinterpret_cast<SensorEvent *>(arg);
    if (event == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Sensor event is NULL!");
        return;
    }
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(AccelData)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SensorEvent data invalid");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_ACCELEROMETER) {
        HILOG_ERROR(HILOG_MODULE_ACE, "sensorTypeId %d is not ACCELEROMETER", event->sensorTypeId);
        return;
    }
    struct SensorDev *sensorDev = GetSensorIdBySensorDevIndex((SensorTypeId)event->sensorTypeId);
    if (sensorDev == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetSensorIdBySensorDevIndex failed");
        return;
    }
    AccelData *accelData = (AccelData *)event->data;
    char *interval = JSI::GetStringProperty(sensorDev->sensorParams.args, "interval");
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "x", static_cast<double>(accelData->axisX));
    JSI::SetNumberProperty(result, "y", static_cast<double>(accelData->axisY));
    JSI::SetNumberProperty(result, "z", static_cast<double>(accelData->axisZ));
    SensorModule::SuccessCallBack(sensorDev->sensorParams.thisVal, sensorDev->sensorParams.args, result, false);
    JSI::ReleaseString(interval);
    JSI::ReleaseValueList(result, ARGS_END);
}

void RecordAccelSensorCallback(SensorEvent *event)
{
    JsAsyncWork::DispatchAsyncWork(RecordAccelSensorCallbackDispatch, event);
}

void RecordBarSensorCallbackDispatch(void *arg)
{
    SensorEvent *event = reinterpret_cast<SensorEvent *>(arg);
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(PressureData)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SensorEvent data invalid");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_BAROMETER) {
        HILOG_ERROR(HILOG_MODULE_ACE, "sensorTypeId %d is not BAROMETER", event->sensorTypeId);
        return;
    }
    struct SensorDev *sensorDev = GetSensorIdBySensorDevIndex((SensorTypeId)event->sensorTypeId);
    if (sensorDev == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetSensorIdBySensorDevIndex failed");
        return;
    }
    PressureData *pressureData = (PressureData *)event->data;
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, SensorModule::PRESSURE, static_cast<double>(pressureData->pressure));
    SensorModule::SuccessCallBack(sensorDev->sensorParams.thisVal, sensorDev->sensorParams.args, result, false);
    JSI::ReleaseValueList(result, ARGS_END);
}

void RecordBarSensorCallback(SensorEvent *event)
{
    JsAsyncWork::DispatchAsyncWork(RecordBarSensorCallbackDispatch, event);
}

void RecordGyroSensorCallbackDispatch(void *arg)
{
    SensorEvent *event = reinterpret_cast<SensorEvent *>(arg);
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(GyroData)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SensorEvent data invalid");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_GYROSCOPE) {
        HILOG_ERROR(HILOG_MODULE_ACE, "sensorTypeId %d is not GYROSCOPE", event->sensorTypeId);
        return;
    }
    struct SensorDev *sensorDev = GetSensorIdBySensorDevIndex((SensorTypeId)event->sensorTypeId);
    if (sensorDev == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetSensorIdBySensorDevIndex failed");
        return;
    }
    GyroData *gyroData = (GyroData *)event->data;
    char *interval = JSI::GetStringProperty(sensorDev->sensorParams.args, "interval");
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "x", static_cast<double>(gyroData->axisX));
    JSI::SetNumberProperty(result, "y", static_cast<double>(gyroData->axisY));
    JSI::SetNumberProperty(result, "z", static_cast<double>(gyroData->axisZ));
    SensorModule::SuccessCallBack(sensorDev->sensorParams.thisVal, sensorDev->sensorParams.args, result, false);
    JSI::ReleaseString(interval);
    JSI::ReleaseValueList(result, ARGS_END);
}

void RecordGyroSensorCallback(SensorEvent *event)
{
    JsAsyncWork::DispatchAsyncWork(RecordGyroSensorCallbackDispatch, event);
}

void RecordHrSensorCallbackDispatch(void *arg)
{
    SensorEvent *event = reinterpret_cast<SensorEvent *>(arg);
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(uint32_t)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SensorEvent data invalid");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_HEART_RATE) {
        HILOG_ERROR(HILOG_MODULE_ACE, "sensorTypeId %d is not HEART_RATE", event->sensorTypeId);
        return;
    }
    struct SensorDev *sensorDev = GetSensorIdBySensorDevIndex((SensorTypeId)event->sensorTypeId);
    if (sensorDev == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetSensorIdBySensorDevIndex failed");
        return;
    }
    uint32_t heartRate = *(uint32_t *)event->data;
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, SensorModule::HEARTRATE, static_cast<double>(heartRate));
    SensorModule::SuccessCallBack(sensorDev->sensorParams.thisVal, sensorDev->sensorParams.args, result, false);
    JSI::ReleaseValueList(result, ARGS_END);
}

void RecordHrSensorCallback(SensorEvent *event)
{
    JsAsyncWork::DispatchAsyncWork(RecordHrSensorCallbackDispatch, event);
}

void RecordObsSensorCallbackDispatch(void *arg)
{
    SensorEvent *event = reinterpret_cast<SensorEvent *>(arg);
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(uint8_t)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SensorEvent data invalid");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_WEAR_DETECTION) {
        HILOG_ERROR(HILOG_MODULE_ACE, "sensorTypeId %d is not WEAR_DETECTION", event->sensorTypeId);
        return;
    }
    struct SensorDev *sensorDev = GetSensorIdBySensorDevIndex((SensorTypeId)event->sensorTypeId);
    if (sensorDev == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetSensorIdBySensorDevIndex failed");
        return;
    }
    uint8_t isonbody = *event->data;
    bool value = isonbody == 0 ? false : true;
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, SensorModule::ISONBODY, static_cast<double>(value));
    SensorModule::SuccessCallBack(sensorDev->sensorParams.thisVal, sensorDev->sensorParams.args, result, false);
    JSI::ReleaseValueList(result, ARGS_END);
}

void RecordObsSensorCallback(SensorEvent *event)
{
    JsAsyncWork::DispatchAsyncWork(RecordObsSensorCallbackDispatch, event);
}

void RecordScSensorCallbackDispatch(void *arg)
{
    SensorEvent *event = reinterpret_cast<SensorEvent *>(arg);
    if (event == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetSensorIdBySensorDevIndex failed");
        return;
    }
    if (event->data == NULL || event->dataLen == 0 || event->dataLen != sizeof(uint32_t)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SensorEvent data invalid");
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_PEDOMETER) {
        HILOG_ERROR(HILOG_MODULE_ACE, "sensorTypeId %d is not PEDOMETER", event->sensorTypeId);
        return;
    }
    struct SensorDev *sensorDev = GetSensorIdBySensorDevIndex((SensorTypeId)event->sensorTypeId);
    if (sensorDev == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetSensorIdBySensorDevIndex failed");
        return;
    }
    uint32_t steps = *(uint32_t *)event->data;
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, SensorModule::STEPS, static_cast<double>(steps));
    SensorModule::SuccessCallBack(sensorDev->sensorParams.thisVal, sensorDev->sensorParams.args, result, false);
    JSI::ReleaseValueList(result, ARGS_END);
}

void RecordScSensorCallback(SensorEvent *event)
{
    JsAsyncWork::DispatchAsyncWork(RecordScSensorCallbackDispatch, event);
}

void SensorModule::ExecuteGetAllSensors()
{
    if (sensorCnt_ != 0 || sensorInfo_ != nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "sensorInfo is Got");
        return;
    }
    int32_t errCode = GetAllSensors(&sensorInfo_, &sensorCnt_);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "sensorInfo failed:%d", errCode);
        return;
    }
}

void SensorModule::ExecuteUnsubscribeAllSensors()
{
    HILOG_INFO(HILOG_MODULE_ACE, "ExecuteUnsubscribeAllSensors");
    for (uint32_t index = 0; index < ARRAY_SIZE(g_sensorDev); index++) {
        if (!g_sensorDev[index].subscribed) {
            continue;
        }
        UnsubscribeBySensorTypeId(g_sensorDev[index].sensorTypeId);
    }
}

int32_t SensorModule::SubscribeBySensorTypeId(void *data, SensorTypeId sensorTypeId)
{
    SensorParams *params = reinterpret_cast<SensorParams *>(data);
    struct SensorDev *sensorDev = GetSensorIdBySensorDevIndex(sensorTypeId);
    if (sensorDev == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetSensorIdBySensorDevIndex failed");
        return SENSOR_ERROR_UNKNOWN;
    }
    int32_t errCode = SubscribeSensor(sensorTypeId, &sensorDev->user);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SubscribeSensor failed:%d", errCode);
        return SENSOR_ERROR_UNKNOWN;
    }
    errCode = ActivateSensor(sensorTypeId, &sensorDev->user);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ActivateSensor failed:%d", errCode);
        return SENSOR_ERROR_UNKNOWN;
    }
    sensorDev->sensorParams.args = params->args;
    sensorDev->sensorParams.thisVal = params->thisVal;
    sensorDev->subscribed = true;
    return SENSOR_OK;
}

int32_t SensorModule::UnsubscribeBySensorTypeId(SensorTypeId sensorTypeId)
{
    struct SensorDev *sensorDev = GetSensorIdBySensorDevIndex(sensorTypeId);
    if (sensorDev == nullptr || sensorDev->subscribed == false) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetSensorIdBySensorDevIndex failed");
        return SENSOR_ERROR_UNKNOWN;
    }
    int32_t errCode = UnsubscribeSensor(sensorTypeId, &sensorDev->user);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SubscribeSensor failed:%d", errCode);
    }
    errCode = DeactivateSensor(sensorTypeId, &sensorDev->user);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "DeactivateSensor failed:%d", errCode);
    }
    sensorDev->subscribed = false;
    JSI::ReleaseValueList(sensorDev->sensorParams.args, sensorDev->sensorParams.thisVal, ARGS_END);
    return SENSOR_OK;
}

void SensorModule::ExecuteSubscribeStepCounter(void *data)
{
    SensorParams *params = reinterpret_cast<SensorParams *>(data);
    if (params == nullptr) {
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    int32_t errCode = SubscribeBySensorTypeId(data, SENSOR_TYPE_ID_PEDOMETER);
    if (errCode != SENSOR_OK) {
        FailCallBack(thisVal, args, -1, false);
        HILOG_ERROR(HILOG_MODULE_ACE, "SubscribeSensor failed:%d", errCode);
        delete params;
        return;
    }
    delete params;
}

void SensorModule::ExecuteSubscribeBarometer(void *data)
{
    SensorParams *params = reinterpret_cast<SensorParams *>(data);
    if (params == nullptr) {
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    int32_t errCode = SubscribeBySensorTypeId(data, SENSOR_TYPE_ID_BAROMETER);
    if (errCode != SENSOR_OK) {
        FailCallBack(thisVal, args, -1, false);
        HILOG_ERROR(HILOG_MODULE_ACE, "SubscribeSensor failed:%d", errCode);
        delete params;
        return;
    }
    delete params;
}

void SensorModule::ExecuteSubscribeHeartRate(void *data)
{
    SensorParams *params = reinterpret_cast<SensorParams *>(data);
    if (params == nullptr) {
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    int32_t errCode = SubscribeBySensorTypeId(data, SENSOR_TYPE_ID_HEART_RATE);
    if (errCode != SENSOR_OK) {
        FailCallBack(thisVal, args, -1, false);
        HILOG_ERROR(HILOG_MODULE_ACE, "SubscribeSensor failed:%d", errCode);
        delete params;
        return;
    }
    delete params;
}

void SensorModule::ExecuteSubscribeOnBodyState(void *data)
{
    SensorParams *params = reinterpret_cast<SensorParams *>(data);
    if (params == nullptr) {
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    int32_t errCode = SubscribeBySensorTypeId(data, SENSOR_TYPE_ID_WEAR_DETECTION);
    if (errCode != SENSOR_OK) {
        FailCallBack(thisVal, args, -1, false);
        HILOG_ERROR(HILOG_MODULE_ACE, "SubscribeSensor failed:%d", errCode);
        delete params;
        return;
    }
    delete params;
}

void SensorModule::ExecuteGetOnBodyState(void *data)
{
    SensorParams *params = reinterpret_cast<SensorParams *>(data);
    if (params == nullptr) {
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    JSIValue result = JSI::CreateObject();
    bool value = DEFAULT_ISONBODY;
    JSI::SetBooleanProperty(result, ISONBODY, static_cast<double>(value));
    // This is the sample code which should be replaced by sensor API.
    if (true) {
        SuccessCallBack(thisVal, args, result, true);
    } else {
        FailCallBack(thisVal, args, -1, true);
    }
    JSI::ReleaseValueList(args, thisVal, result, ARGS_END);
    delete params;
}

void SensorModule::ExecuteSubscribeAccelerometer(void *data)
{
    SensorParams *params = reinterpret_cast<SensorParams *>(data);
    if (params == nullptr) {
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    int32_t errCode = SubscribeBySensorTypeId(data, SENSOR_TYPE_ID_ACCELEROMETER);
    if (errCode != SENSOR_OK) {
        FailCallBack(thisVal, args, -1, false);
        HILOG_ERROR(HILOG_MODULE_ACE, "SubscribeSensor failed:%d", errCode);
        delete params;
        return;
    }
    delete params;
}

void SensorModule::ExecuteSubscribeGyroscope(void *data)
{
    SensorParams *params = reinterpret_cast<SensorParams *>(data);
    if (params == nullptr) {
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    int32_t errCode = SubscribeBySensorTypeId(data, SENSOR_TYPE_ID_GYROSCOPE);
    if (errCode != SENSOR_OK) {
        FailCallBack(thisVal, args, -1, false);
        HILOG_ERROR(HILOG_MODULE_ACE, "SubscribeSensor failed:%d", errCode);
        delete params;
        return;
    }
    delete params;
}

void SensorModule::ExecuteGetAccelerometerData(void *data)
{
    SensorParams *params = reinterpret_cast<SensorParams *>(data);
    if (params == nullptr) {
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    struct SensorDev *sensorDev = GetSensorIdBySensorDevIndex(SENSOR_TYPE_ID_ACCELEROMETER);
    if (sensorDev == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetSensorIdBySensorDevIndex failed");
        return;
    }
    SensorData *sensorData = new SensorData;
    if (sensorData == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc failed!");
        SensorModule::FailCallBack(thisVal, args, SENSOR_ERROR_UNKNOWN, false);
        return;
    }
    int32_t ret = GetSensorData(SENSOR_TYPE_ID_ACCELEROMETER, &sensorDev->user, sensorData);
    if (ret != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Get accelerometer data failed!");
        delete sensorData;
        SensorModule::FailCallBack(thisVal, args, SENSOR_ERROR_UNKNOWN, false);
        return;
    }
    AccelData *accelData = (AccelData *)sensorData->data;
    JSIValue result = JSI::CreateObject();
    for (uint32_t i = 0; i < sensorData->batchCnt; i++) {
        JSI::SetNumberProperty(result, "x", static_cast<double>(accelData[i].axisX));
        JSI::SetNumberProperty(result, "y", static_cast<double>(accelData[i].axisY));
        JSI::SetNumberProperty(result, "z", static_cast<double>(accelData[i].axisZ));
        SensorModule::SuccessCallBack(thisVal, args, result, false);
    }
    JSI::SetNumberProperty(result, "batchCnt", static_cast<double>(sensorData->batchCnt));
    JSI::SetNumberProperty(result, "timestamp", static_cast<double>(sensorData->timestamp));
    SensorModule::SuccessCallBack(thisVal, args, result, false);
    JSI::ReleaseValueList(result, ARGS_END);
    delete sensorData;
}

void SensorModule::ExecuteGetGyroscopeData(void *data)
{
    SensorParams *params = reinterpret_cast<SensorParams *>(data);
    if (params == nullptr) {
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    struct SensorDev *sensorDev = GetSensorIdBySensorDevIndex(SENSOR_TYPE_ID_GYROSCOPE);
    if (sensorDev == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetSensorIdBySensorDevIndex failed");
        return;
    }
    SensorData *sensorData = new SensorData;
    if (sensorData == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc failed!");
        SensorModule::FailCallBack(thisVal, args, SENSOR_ERROR_UNKNOWN, false);
        return;
    }
    int32_t ret = GetSensorData(SENSOR_TYPE_ID_GYROSCOPE, &sensorDev->user, sensorData);
    if (ret != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Get accelerometer data failed!");
        delete sensorData;
        SensorModule::FailCallBack(thisVal, args, SENSOR_ERROR_UNKNOWN, false);
        return;
    }
    GyroData *gyroData = (GyroData *)sensorData->data;
    JSIValue result = JSI::CreateObject();
    for (uint32_t i = 0; i < sensorData->batchCnt; i++) {
        JSI::SetNumberProperty(result, "x", static_cast<double>(gyroData->axisX));
        JSI::SetNumberProperty(result, "y", static_cast<double>(gyroData->axisY));
        JSI::SetNumberProperty(result, "z", static_cast<double>(gyroData->axisZ));
        SensorModule::SuccessCallBack(thisVal, args, result, false);
    }
    JSI::SetNumberProperty(result, "batchCnt", static_cast<double>(sensorData->batchCnt));
    JSI::SetNumberProperty(result, "timestamp", static_cast<double>(sensorData->timestamp));
    SensorModule::SuccessCallBack(thisVal, args, result, false);
    JSI::ReleaseValueList(result, ARGS_END);
    delete sensorData;
}

JSIValue SensorModule::ExecuteAsyncWork(
    const JSIValue thisVal, const JSIValue *args, uint8_t argsNum, AsyncWorkHandler ExecuteFunc)
{
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || (argsNum == 0) || (JSI::ValueIsUndefined(args[0]))) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Error in obtaining Sensor information parameters");
        return undefValue;
    }

    SensorParams *params = new (std::nothrow) SensorParams();
    if (params == nullptr) {
        return undefValue;
    }
    params->thisVal = JSI::AcquireValue(thisVal);
    if (args != nullptr) {
        params->args = JSI::AcquireValue(args[0]);
    }
    JsAsyncWork::DispatchAsyncWork(ExecuteFunc, reinterpret_cast<void *>(params));
    return undefValue;
}

JSIValue SensorModule::SubscribeStepCounter(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteSubscribeStepCounter);
}

JSIValue SensorModule::UnsubscribeStepCounter(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    int32_t errCode = UnsubscribeBySensorTypeId(SENSOR_TYPE_ID_PEDOMETER);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Unsubscribe failed:%d", errCode);
    }
    return JSI::CreateUndefined();
}

JSIValue SensorModule::SubscribeBarometer(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteSubscribeBarometer);
}

JSIValue SensorModule::UnsubscribeBarometer(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    int32_t errCode = UnsubscribeBySensorTypeId(SENSOR_TYPE_ID_BAROMETER);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Unsubscribe failed:%d", errCode);
    }
    return JSI::CreateUndefined();
}

JSIValue SensorModule::SubscribeHeartRate(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteSubscribeHeartRate);
}

JSIValue SensorModule::UnsubscribeHeartRate(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    int32_t errCode = UnsubscribeBySensorTypeId(SENSOR_TYPE_ID_HEART_RATE);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Unsubscribe failed:%d", errCode);
    }
    return JSI::CreateUndefined();
}

JSIValue SensorModule::SubscribeOnBodyState(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteSubscribeOnBodyState);
}

JSIValue SensorModule::UnsubscribeOnBodyState(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    int32_t errCode = UnsubscribeBySensorTypeId(SENSOR_TYPE_ID_WEAR_DETECTION);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Unsubscribe failed:%d", errCode);
    }
    return JSI::CreateUndefined();
}

JSIValue SensorModule::GetOnBodyState(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteGetOnBodyState);
}

JSIValue SensorModule::SubscribeAccelerometer(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteSubscribeAccelerometer);
}

JSIValue SensorModule::UnsubscribeAccelerometer(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    int32_t errCode = UnsubscribeBySensorTypeId(SENSOR_TYPE_ID_ACCELEROMETER);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Unsubscribe failed:%d", errCode);
    }
    return JSI::CreateUndefined();
}

JSIValue SensorModule::SubscribeGyroscope(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteSubscribeGyroscope);
}

JSIValue SensorModule::UnsubscribeGyroscope(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    int32_t errCode = UnsubscribeBySensorTypeId(SENSOR_TYPE_ID_GYROSCOPE);
    if (errCode != SENSOR_OK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Unsubscribe failed:%d", errCode);
    }
    return JSI::CreateUndefined();
}

JSIValue SensorModule::GetAccelerometerData(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteGetAccelerometerData);
}

JSIValue SensorModule::GetGyroscopeData(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteGetGyroscopeData);
}

void SensorModule::OnTerminate()
{
    ExecuteUnsubscribeAllSensors();
    sensorInfo_ = nullptr;
    sensorCnt_ = 0;
}

}  // namespace ACELite
}  // namespace OHOS
