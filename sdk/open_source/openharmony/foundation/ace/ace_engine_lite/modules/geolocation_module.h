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

#ifndef OHOS_ACELITE_GEOLOCATION_MODULE_H
#define OHOS_ACELITE_GEOLOCATION_MODULE_H

#include <cJSON.h>
#include "acelite_config.h"
#include "jsi.h"
#include "js_async_work.h"
#include "non_copyable.h"
#include "product_adapter.h"
#include "abilityms_slite_client.h"
#include "locator.h"

namespace OHOS {
namespace ACELite {

struct LocationParams {
    JSIValue args = JSI::CreateUndefined();
    JSIValue thisVal = JSI::CreateUndefined();
    JSIValue jsiValue = JSI::CreateUndefined();
};

class GnssStatusCallback : public Location::IGnssStatusCallback {
public:
    GnssStatusCallback() {};
    ~GnssStatusCallback() {};

    void OnStatusChange(const std::shared_ptr<OHOS::Location::SatelliteStatus> &statusInfo) override;
private:
};

class LocatorCallback : public Location::ILocatorCallback {
public:
    LocatorCallback(JSIValue args, JSIValue thisVal) : args_(args), thisVal_(thisVal) {};

    ~LocatorCallback();

    void OnLocationReport(const std::shared_ptr<OHOS::Location::Location> &location) override;

    void OnErrorReport(const int errorCode) override;

    void OnLocatingStatusChange(const int status) override;

private:
    JSIValue args_;
    JSIValue thisVal_;
};

class LocationModule final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(LocationModule);
    LocationModule() = default;
    ~LocationModule() = default;
    static const char * const COORD_TYPES;
    static double LONGITUDE;
    static double LATITUDE;
    static double ALTITUDE;
    static double ACCURACY;
    static uint32_t TIME;
    static double SPEED_ACCURACY;
    static double ALTITUDE_ACCURACY;
    static double DIRECTION_ACCURACY;
    static double VELOCITY_NORTH;
    static double VELOCITY_EAST;
    static double VELOCITY_DESCEND;
    static int32_t SATELLITE_NUMBER;
    static int32_t STATUS_NUMBER;
    static const char * const LONGITUDE_NAME;
    static const char * const LATITUDE_NAME;
    static const char * const ALTITUDE_NAME;
    static const char * const ACCURACY_NAME;
    static const char * const TIME_NAME;
    static const char * const COORD_TYPES_NAME;
    static const char * const LOCATIONTYPE_NAME;
    static const char * const LOCATIONTYPE;
    static const char * const TIMEOUT;
    static const char * const SPEED_ACCURACY_NAME;
    static const char * const ALTITUDE_ACCURACY_NAME;
    static const char * const DIRECTION_ACCURACY_NAME;
    static const char * const VELOCITY_NORTH_NAME;
    static const char * const VELOCITY_EAST_NAME;
    static const char * const VELOCITY_DESCEND_NAME;
    static const char * const STATUS_NUMBER_NAME;
    static const char * const SATELLITE_NUMBER_NAME;
    static JSIValue GetSupportCoordTypes(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue Unsubscribe(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue Subscribe(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetLocationType(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetLocation(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static void ExecuteSubscribe(void* data);
    static void ExecuteUnsubscribe(void* data);
    static void ExecuteGetLocationType(void* data);
    static void ExecuteGetLocation(void* data);
    static void OnDestroy();
private:
    friend class LocatorCallback;
    static JSIValue ExecuteAsyncWork(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum, AsyncWorkHandler ExecuteFunc);
    static void SuccessCallBack(const JSIValue thisVal, const JSIValue args, JSIValue jsiValue);
    static void SuccessCallBackDispatch(void *data);
    static void FailCallBack(const JSIValue thisVal, const JSIValue args, int ret);
    static int32_t SubscribeAndStartLocating(void *data);
    static int32_t UnsubscribeAndStopLocating();
    static void DestroyResources();
    static LocatorCallback *locatorCallback_;
    static GnssStatusCallback *gnssStatusCallback_;
};

void InitLocationModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "getSupportedCoordTypes", LocationModule::GetSupportCoordTypes);
    JSI::SetModuleAPI(exports, "unsubscribe", LocationModule::Unsubscribe);
    JSI::SetModuleAPI(exports, "subscribe", LocationModule::Subscribe);
    JSI::SetModuleAPI(exports, "getLocationType", LocationModule::GetLocationType);
    JSI::SetModuleAPI(exports, "getLocation", LocationModule::GetLocation);
    JSI::SetOnDestroy(exports, LocationModule::OnDestroy);
    JSI::SetOnTerminate(exports, LocationModule::OnDestroy);
}

} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_APP_MODULE_H
