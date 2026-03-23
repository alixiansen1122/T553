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

#ifndef OHOS_ACELITE_SENSOR_MODULE_H
#define OHOS_ACELITE_SENSOR_MODULE_H

#include <cJSON.h>
#include "acelite_config.h"
#include "jsi.h"
#include "js_async_work.h"
#include "non_copyable.h"
#include "product_adapter.h"
#include "abilityms_slite_client.h"
#include "sensor_agent.h"
#include "sensor_agent_type.h"
#include "sensor_agent_type_ext.h"

namespace OHOS {
namespace ACELite {

struct SensorParams {
    JSIValue args = JSI::CreateUndefined();
    JSIValue thisVal = JSI::CreateUndefined();
};

class SensorModule final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(SensorModule);
    SensorModule() = default;
    ~SensorModule() = default;
    static const char * const STEPS;
    static const uint32_t DEFAULT_STEPS;
    static const char * const PRESSURE;
    static const uint32_t DEFAULT_PRESSURE;
    static const char * const HEARTRATE;
    static const uint32_t DEFAULT_HEARTRATE;
    static const char * const ISONBODY;
    static const bool DEFAULT_ISONBODY; 
    static const uint32_t X;
    static const uint32_t Y;
    static const uint32_t Z;
    static JSIValue SubscribeStepCounter(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue UnsubscribeStepCounter(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue SubscribeBarometer(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue UnsubscribeBarometer(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue SubscribeHeartRate(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue UnsubscribeHeartRate(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue SubscribeOnBodyState(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue UnsubscribeOnBodyState(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetOnBodyState(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue SubscribeAccelerometer(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue UnsubscribeAccelerometer(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue SubscribeGyroscope(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue UnsubscribeGyroscope(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetAccelerometerData(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetGyroscopeData(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static void SuccessCallBack(const JSIValue thisVal, const JSIValue args, JSIValue jsiValue, bool flag);
    static void ExecuteGetAllSensors();
    static void ExecuteUnsubscribeAllSensors();
    static int32_t SubscribeBySensorTypeId(void *data, SensorTypeId sensorTypeId);
    static int32_t UnsubscribeBySensorTypeId(SensorTypeId sensorTypeId);
    static void OnTerminate();

    static SensorInfo *sensorInfo_;
    static int32_t sensorCnt_;

private:
    static JSIValue ExecuteAsyncWork(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum, AsyncWorkHandler ExecuteFunc);
    static void FailCallBack(const JSIValue thisVal, const JSIValue args, int ret, bool flag);
    static void ExecuteSubscribeStepCounter(void *data);
    static void ExecuteSubscribeBarometer(void *data);
    static void ExecuteSubscribeHeartRate(void *data);
    static void ExecuteSubscribeOnBodyState(void *data);
    static void ExecuteGetOnBodyState(void *data);
    static void ExecuteSubscribeGyroscope(void *data);
    static void ExecuteSubscribeAccelerometer(void *data);
    static void ExecuteGetAccelerometerData(void *data);
    static void ExecuteGetGyroscopeData(void *data);
};

void InitSensorModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "subscribeStepCounter", SensorModule::SubscribeStepCounter);
    JSI::SetModuleAPI(exports, "unsubscribeStepCounter", SensorModule::UnsubscribeStepCounter);
    JSI::SetModuleAPI(exports, "subscribeBarometer", SensorModule::SubscribeBarometer);
    JSI::SetModuleAPI(exports, "unsubscribeBarometer", SensorModule::UnsubscribeBarometer);
    JSI::SetModuleAPI(exports, "subscribeHeartRate", SensorModule::SubscribeHeartRate);
    JSI::SetModuleAPI(exports, "unsubscribeHeartRate", SensorModule::UnsubscribeHeartRate);
    JSI::SetModuleAPI(exports, "subscribeOnBodyState", SensorModule::SubscribeOnBodyState);
    JSI::SetModuleAPI(exports, "unsubscribeOnBodyState", SensorModule::UnsubscribeOnBodyState);
    JSI::SetModuleAPI(exports, "getOnBodyState", SensorModule::GetOnBodyState);
    JSI::SetModuleAPI(exports, "subscribeAccelerometer", SensorModule::SubscribeAccelerometer);
    JSI::SetModuleAPI(exports, "unsubscribeAccelerometer", SensorModule::UnsubscribeAccelerometer);
    JSI::SetModuleAPI(exports, "subscribeGyroscope", SensorModule::SubscribeGyroscope);
    JSI::SetModuleAPI(exports, "unsubscribeGyroscope", SensorModule::UnsubscribeGyroscope);
    JSI::SetModuleAPI(exports, "getAccelerometerData", SensorModule::GetAccelerometerData);
    JSI::SetModuleAPI(exports, "getGyroscopeData", SensorModule::GetGyroscopeData);
    JSI::SetOnTerminate(exports, SensorModule::OnTerminate);
    JSI::SetOnDestroy(exports, SensorModule::OnTerminate);
    SensorModule::ExecuteGetAllSensors();
}

} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_APP_MODULE_H
