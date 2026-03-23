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
#include "geolocation_module.h"
#include "global.h"

namespace OHOS {
namespace ACELite {

using namespace OHOS::Location;

double LocationModule::LONGITUDE = 123.456789;
double LocationModule::LATITUDE = 45.678901;
double LocationModule::ALTITUDE = 100.0;
double LocationModule::ACCURACY = 10.0;
uint32_t LocationModule::TIME = 163456789;
double LocationModule::SPEED_ACCURACY = 14.72;
double LocationModule::ALTITUDE_ACCURACY = 8.4;
double LocationModule::DIRECTION_ACCURACY = 0;
double LocationModule::VELOCITY_NORTH = -43.311;
double LocationModule::VELOCITY_EAST = 16.366;
double LocationModule::VELOCITY_DESCEND = 10;
int32_t LocationModule::SATELLITE_NUMBER = 49;
int32_t LocationModule::STATUS_NUMBER = 1;
const char *const LocationModule::LONGITUDE_NAME = "longitude";
const char *const LocationModule::LATITUDE_NAME = "latitude";
const char *const LocationModule::ALTITUDE_NAME = "altitude";
const char *const LocationModule::ACCURACY_NAME = "accuracy";
const char *const LocationModule::SPEED_ACCURACY_NAME = "speedAccuracy";
const char *const LocationModule::ALTITUDE_ACCURACY_NAME = "altitudeAccuracy";
const char *const LocationModule::DIRECTION_ACCURACY_NAME = "directionAccuracy";
const char *const LocationModule::VELOCITY_NORTH_NAME = "velocityNorth";
const char *const LocationModule::VELOCITY_EAST_NAME = "velocityEast";
const char *const LocationModule::VELOCITY_DESCEND_NAME = "velocityDescend";
const char *const LocationModule::STATUS_NUMBER_NAME = "statusNumber";
const char *const LocationModule::SATELLITE_NUMBER_NAME = "satelliteNubmer";
const char *const LocationModule::TIME_NAME = "time";
const char *const LocationModule::COORD_TYPES_NAME = "coordType";
const char *const LocationModule::LOCATIONTYPE_NAME = "types";
const char *const LocationModule::LOCATIONTYPE = "GPS";
const char *const LocationModule::TIMEOUT = "timeout";

typedef struct {
    LocationCoordinateSystemType coordinateSystemType;
    const char *const coordTypeString;
} CoordTypeMap;

LocatorCallback *LocationModule::locatorCallback_ = nullptr;
GnssStatusCallback *LocationModule::gnssStatusCallback_ = nullptr;

static CoordTypeMap g_coordTypeMap[] = {
    {WGS84, "WGS84"},
    {GCJ02, "GCJ02"},
};

JSIValue LocationModule::GetSupportCoordTypes(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
#if defined(ENABLE_LOCATION)
    std::vector<LocationCoordinateSystemType> coordinateSystemTypes;
    LocationErrCode errCode = Locator::GetInstance().QuerySupportCoordinateSystemType(coordinateSystemTypes);
    if (errCode != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "QuerySupportCoordinateSystemType failed:%d", errCode);
        return JSI::CreateUndefined();
    }
    uint32_t length = coordinateSystemTypes.size();
    JSIValue rltArray = JSI::CreateArray(length);
    uint32_t rltArrayCnt = 0;
    for (auto type : coordinateSystemTypes) {
        HILOG_INFO(HILOG_MODULE_ACE, "query type:%d", type);
        for (uint32_t i = 0; i < ARRAY_SIZE(g_coordTypeMap); i++) {
            if (g_coordTypeMap[i].coordinateSystemType == type) {
                JSIValue result = JSI::CreateString(g_coordTypeMap[i].coordTypeString);
                JSI::SetPropertyByIndex(rltArray, rltArrayCnt++, result);
                JSI::ReleaseValue(result);
            }
        }
    }
    return rltArray;
#else
    HILOG_INFO(HILOG_MODULE_ACE, "not support!");
    return JSI::CreateUndefined();
#endif
}

JSIValue LocationModule::Unsubscribe(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
#if defined(ENABLE_LOCATION)
    JsAsyncWork::DispatchAsyncWork(ExecuteUnsubscribe, nullptr);
#endif
    return JSI::CreateUndefined();
}

JSIValue LocationModule::Subscribe(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
#if defined(ENABLE_LOCATION)
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteSubscribe);
#else
    HILOG_INFO(HILOG_MODULE_ACE, "not support!");
    return JSI::CreateUndefined();
#endif
}

JSIValue LocationModule::GetLocationType(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
#if defined(ENABLE_LOCATION)
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteGetLocationType);
#else
    HILOG_INFO(HILOG_MODULE_ACE, "not support!");
    return JSI::CreateUndefined();
#endif
}

JSIValue LocationModule::GetLocation(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
#if defined(ENABLE_LOCATION)
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteGetLocation);
#else
    HILOG_INFO(HILOG_MODULE_ACE, "not support!");
    return JSI::CreateUndefined();
#endif
}

JSIValue LocationModule::ExecuteAsyncWork(
    const JSIValue thisVal, const JSIValue *args, uint8_t argsNum, AsyncWorkHandler ExecuteFunc)
{
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || (argsNum == 0) || (JSI::ValueIsUndefined(args[0]))) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Error in obtaining device information parameters");
        return undefValue;
    }
    LocationParams *params = new (std::nothrow) LocationParams();
    if (params == nullptr) {
        return undefValue;
    }
    params->thisVal = JSI::AcquireValue(thisVal);
    params->args = JSI::AcquireValue(args[0]);
    JsAsyncWork::DispatchAsyncWork(ExecuteFunc, reinterpret_cast<void *>(params));
    return undefValue;
}

void LocationModule::OnDestroy()
{
    UnsubscribeAndStopLocating();
}

void GnssStatusCallback::OnStatusChange(const std::shared_ptr<SatelliteStatus> &statusInfo)
{
    LocationModule::SATELLITE_NUMBER = statusInfo->GetSatellitesNumber();
}

void LocatorCallback::OnLocationReport(const std::shared_ptr<OHOS::Location::Location> &location)
{
    JSIValue result = JSI::CreateObject();
    LocationModule::LONGITUDE = location->GetLongitude();
    LocationModule::LATITUDE = location->GetLatitude();
    LocationModule::ALTITUDE = location->GetAltitude();
    LocationModule::ACCURACY = location->GetAccuracy();
    LocationModule::TIME = static_cast<double>(location->GetTimeStamp());
    LocationModule::SPEED_ACCURACY = location->GetSpeedAccuracy();
    LocationModule::ALTITUDE_ACCURACY = location->GetAltitudeAccuracy();
    LocationModule::DIRECTION_ACCURACY = location->GetDirectionAccuracy();
    LocationModule::VELOCITY_NORTH = location->GetNorthVelocity();
    LocationModule::VELOCITY_EAST = location->GetEastVelocity();
    LocationModule::VELOCITY_DESCEND = location->GetDescendVelocity();
    LocationModule::STATUS_NUMBER = location->GetStatusNumber();
    JSI::SetNumberProperty(result, LocationModule::LONGITUDE_NAME, location->GetLatitude());
    JSI::SetNumberProperty(result, LocationModule::LATITUDE_NAME, location->GetLongitude());
    JSI::SetNumberProperty(result, LocationModule::ALTITUDE_NAME, location->GetAltitude());
    JSI::SetNumberProperty(result, LocationModule::ACCURACY_NAME, location->GetAccuracy());
    JSI::SetNumberProperty(result, LocationModule::TIME_NAME, static_cast<double>(location->GetTimeStamp()));
    JSI::SetNumberProperty(result, LocationModule::SPEED_ACCURACY_NAME, location->GetSpeedAccuracy());
    JSI::SetNumberProperty(result, LocationModule::ALTITUDE_ACCURACY_NAME, location->GetAltitudeAccuracy());
    JSI::SetNumberProperty(result, LocationModule::DIRECTION_ACCURACY_NAME, location->GetDirectionAccuracy());
    JSI::SetNumberProperty(result, LocationModule::VELOCITY_NORTH_NAME, location->GetNorthVelocity());
    JSI::SetNumberProperty(result, LocationModule::VELOCITY_EAST_NAME, location->GetEastVelocity());
    JSI::SetNumberProperty(result, LocationModule::VELOCITY_DESCEND_NAME, location->GetDescendVelocity());
    JSI::SetNumberProperty(result, LocationModule::STATUS_NUMBER_NAME, location->GetStatusNumber());
    JSI::SetNumberProperty(result, LocationModule::SATELLITE_NUMBER_NAME, LocationModule::SATELLITE_NUMBER);
    LocationModule::SuccessCallBack(thisVal_, args_, result);
    JSI::ReleaseValueList(result, ARGS_END);
}

void LocatorCallback::OnErrorReport(const int errorCode)
{
    HILOG_INFO(HILOG_MODULE_ACE, "error code:%d", errorCode);
}

void LocatorCallback::OnLocatingStatusChange(const int status)
{
    HILOG_INFO(HILOG_MODULE_ACE, "OnLocatingStatusChange status:%d", status);
}

LocatorCallback::~LocatorCallback()
{
    JSI::ReleaseValueList(args_, thisVal_, ARGS_END);
}

void LocationModule::DestroyResources()
{
    if (locatorCallback_ != nullptr) {
        delete locatorCallback_;
        locatorCallback_ = nullptr;
    }
    if (gnssStatusCallback_ != nullptr) {
        delete gnssStatusCallback_;
        gnssStatusCallback_ = nullptr;
    }
}

int32_t LocationModule::SubscribeAndStartLocating(void *data)
{
    LocationParams *params = reinterpret_cast<LocationParams *>(data);
    bool isEnabled = false;
    LocationErrCode errCode = Locator::GetInstance().IsLocationEnabled(isEnabled);
    if (errCode != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "locator IsLocationEnabled failed:%d", errCode);
        return errCode;
    }
    if (isEnabled) {
        HILOG_ERROR(HILOG_MODULE_ACE, "locator IsLocationEnabled");
        return ERRCODE_SUCCESS;
    }
    errCode = Locator::GetInstance().EnableAbility(true);
    if (errCode != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "locator EnableAbility failed:%d", errCode);
        return errCode;
    }
    gnssStatusCallback_ = new (std::nothrow) GnssStatusCallback;
    if (gnssStatusCallback_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "locator gnssStatusCallback_ null");
        return ERRCODE_FAILURE;
    }
    errCode = Locator::GetInstance().RegisterGnssStatusCallback(gnssStatusCallback_);
    if (errCode != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "locator RegisterGnssStatusCallback failed:%d\n", errCode);
        return errCode;
    }
    locatorCallback_ = new (std::nothrow) LocatorCallback(params->args, params->thisVal);
    if (locatorCallback_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "locator locatorCallback_ null");
        return ERRCODE_FAILURE;
    }
    RequestConfig requestConfig;
    requestConfig.SetPriority(PRIORITY_ACCURACY);
    errCode = Locator::GetInstance().StartLocating(&requestConfig, locatorCallback_);
    if (errCode != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "locator StartLocating failed:%d\n", errCode);
        return errCode;
    }
    return ERRCODE_SUCCESS;
}

int32_t LocationModule::UnsubscribeAndStopLocating()
{
    bool isEnabled = false;
    LocationErrCode errCode = Locator::GetInstance().IsLocationEnabled(isEnabled);
    if (errCode != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "locator IsLocationEnabled failed:%d", errCode);
        return errCode;
    }
    if (!isEnabled) {
        HILOG_ERROR(HILOG_MODULE_ACE, "locator Is not LocationEnabled");
        return ERRCODE_FAILURE;
    }
    errCode = Locator::GetInstance().UnregisterGnssStatusCallback(gnssStatusCallback_);
    if (errCode != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "locator UnregisterGnssStatusCallback failed:%d", errCode);
    }
    errCode = Locator::GetInstance().StopLocating(locatorCallback_);
    if (errCode != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "locator StopLocating failed:%d", errCode);
    }
    errCode = Locator::GetInstance().EnableAbility(false);
    if (errCode != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "locator EnableAbility failed:%d", errCode);
    }
    DestroyResources();
    return ERRCODE_SUCCESS;
}

void LocationModule::ExecuteSubscribe(void *data)
{
    LocationParams *params = reinterpret_cast<LocationParams *>(data);
    if (params == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "params or locator_ null");
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    int32_t ret = SubscribeAndStartLocating(data);
    if (ret != ERRCODE_SUCCESS) {
        FailCallBack(thisVal, args, -1);
    }
    delete params;
}

void LocationModule::ExecuteUnsubscribe(void *data)
{
    int32_t ret = UnsubscribeAndStopLocating();
    if (ret != ERRCODE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "UnsubscribeAndStopLocating failed:%d", ret);
    }
}

void LocationModule::ExecuteGetLocationType(void *data)
{
    LocationParams *params = reinterpret_cast<LocationParams *>(data);
    if (params == nullptr) {
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    JSIValue result = JSI::CreateObject();
    JSIValue LocationType = JSI::CreateString(LOCATIONTYPE);
    JSIValue typeArray = JSI::CreateArray(1);
    JSI::SetPropertyByIndex(typeArray, 0, LocationType);

    JSI::SetNamedProperty(result, "types", typeArray);
    SuccessCallBack(thisVal, args, result);

    JSI::ReleaseValueList(args, thisVal, result, typeArray, LocationType, ARGS_END);
    delete params;
}

void LocationModule::ExecuteGetLocation(void *data)
{
    LocationParams *params = reinterpret_cast<LocationParams *>(data);
    if (params == nullptr) {
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    double timeOut = JSI::GetNumberProperty(args, TIMEOUT);
    char *coordType = JSI::GetStringProperty(args, COORD_TYPES_NAME);
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, LONGITUDE_NAME, LONGITUDE);
    JSI::SetNumberProperty(result, LATITUDE_NAME, LATITUDE);
    JSI::SetNumberProperty(result, ALTITUDE_NAME, ALTITUDE);
    JSI::SetNumberProperty(result, ACCURACY_NAME, ACCURACY);
    JSI::SetNumberProperty(result, TIME_NAME, static_cast<double>(TIME));
    JSI::SetNumberProperty(result, SPEED_ACCURACY_NAME, SPEED_ACCURACY);
    JSI::SetNumberProperty(result, ALTITUDE_ACCURACY_NAME, ALTITUDE_ACCURACY);
    JSI::SetNumberProperty(result, DIRECTION_ACCURACY_NAME, DIRECTION_ACCURACY);
    JSI::SetNumberProperty(result, VELOCITY_NORTH_NAME, VELOCITY_NORTH);
    JSI::SetNumberProperty(result, VELOCITY_EAST_NAME, VELOCITY_EAST);
    JSI::SetNumberProperty(result, VELOCITY_DESCEND_NAME, VELOCITY_DESCEND);
    JSI::SetNumberProperty(result, STATUS_NUMBER_NAME, STATUS_NUMBER);
    JSI::SetNumberProperty(result, SATELLITE_NUMBER_NAME, SATELLITE_NUMBER);
    SuccessCallBack(thisVal, args, result);
    JSI::ReleaseString(coordType);
    JSI::ReleaseValueList(args, thisVal, result, ARGS_END);
    delete params;
}

void LocationModule::SuccessCallBackDispatch(void *data)
{
    LocationParams *param = (LocationParams *)data;
    if (JSI::ValueIsUndefined(param->args) || JSI::ValueIsUndefined(param->thisVal)) {
        JSI::ReleaseValueList(param->jsiValue, param->args, param->thisVal);
        delete param;
        return;
    }
    JSIValue success = JSI::GetNamedProperty(param->args, CB_SUCCESS);
    JSIValue complete = JSI::GetNamedProperty(param->args, CB_COMPLETE);
    if (!JSI::ValueIsUndefined(success)) {
        if (JSI::ValueIsUndefined(param->jsiValue)) {
            JSI::CallFunction(success, param->thisVal, nullptr, 0);
        } else {
            JSI::CallFunction(success, param->thisVal, &param->jsiValue, ARGC_ONE);
        }
    }
    if (!JSI::ValueIsUndefined(complete)) {
        JSI::CallFunction(complete, param->thisVal, nullptr, 0);
    }
    JSI::ReleaseValueList(param->jsiValue, param->args, param->thisVal, success, complete, ARGS_END);
    delete param;
}

void LocationModule::SuccessCallBack(const JSIValue thisVal, const JSIValue args, JSIValue jsiValue)
{
    LocationParams *params = new (std::nothrow) LocationParams();
    if (params == nullptr) {
        return;
    }
    params->thisVal = JSI::AcquireValue(thisVal);
    params->args = JSI::AcquireValue(args);
    params->jsiValue = JSI::AcquireValue(jsiValue);
    JsAsyncWork::DispatchAsyncWork(SuccessCallBackDispatch, reinterpret_cast<void *>(params));
}

void LocationModule::FailCallBack(const JSIValue thisVal, const JSIValue args, int ret)
{
    if (JSI::ValueIsUndefined(args)) {
        return;
    }
    JSIValue errInfo;
    errInfo = JSI::CreateString("GetDeviceInfo faild");
    JSIValue errCode = JSI::CreateNumber(ret);
    JSIValue fail = JSI::GetNamedProperty(args, CB_FAIL);
    JSIValue complete = JSI::GetNamedProperty(args, CB_COMPLETE);
    JSIValue argv[ARGC_TWO] = {errInfo, errCode};
    if (!JSI::ValueIsUndefined(fail)) {
        JSI::CallFunction(fail, thisVal, argv, ARGC_TWO);
    }
    if (!JSI::ValueIsUndefined(complete)) {
        JSI::CallFunction(complete, thisVal, nullptr, 0);
    }
    JSI::ReleaseValueList(fail, complete, errInfo, errCode, ARGS_END);
}

}  // namespace ACELite
}  // namespace OHOS
