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
#include "device_module.h"
#include "global.h"
#include "screen.h"
#include "parameter.h"

namespace OHOS {
namespace ACELite {

const char * const DeviceModule::DEVICE_BRAND = "brand";
const char * const DeviceModule::DEVICE_MANUFACTURER = "manufacturer";
const char * const DeviceModule::DEVICE_MODEL = "model";
const char * const DeviceModule::DEVICE_PRODUCT = "product";
const char * const DeviceModule::DEVICE_LANGUAGE = "language";
const char * const DeviceModule::DEVICE_REGION = "region";
const char * const DeviceModule::DEVICE_WINDOW_WIDTH_NAME = "windowWidth";
const char * const DeviceModule::DEVICE_WINDOW_HEIGHT_NAME = "windowHeight";
const char * const DeviceModule::DEVICE_SCREEN_DENSITY_NAME = "screenDensity";
const char * const DeviceModule::DEVICE_SCREEN_SHAPE = "screenShape";
const uint32_t DeviceModule::DEVICE_SCREEN_DENSITY = 326;
const char * const DeviceModule::DEVICE_TYPE = "deviceType";
const char * const DeviceModule::DEVICE_API_VERSION_NAME = "apiVersion";
const char * const DeviceModule::DEVICE_MACADDRESS = "macAddress";
bool DeviceModule::GetBrand(JSIValue result)
{   bool ret = true;
    const char *brand = DEVICE_BRAND;
    if (brand == nullptr) {
        ret = false;
    } else {
        JSI::SetStringProperty(result, DEVICE_BRAND, brand);
    }
    return ret;
}

bool DeviceModule::GetManufacturer(JSIValue result)
{
    bool ret = true;
    const char *manufacturer = DEVICE_MANUFACTURER;
    if (manufacturer == nullptr) {
        ret = false;
    } else {
        JSI::SetStringProperty(result, DEVICE_MANUFACTURER, manufacturer);
    }
    return ret;
}

bool DeviceModule::GetModel(JSIValue result)
{
    bool ret = true;
    const char *mode = DEVICE_MODEL;
    if (mode == nullptr) {
        ret = false;
    } else {
        JSI::SetStringProperty(result, DEVICE_MODEL, mode);
    }
    return ret;
}

bool DeviceModule::GetProduct(JSIValue result)
{
    bool ret = true;
    const char *product = DEVICE_PRODUCT;
    if (product == nullptr) {
        ret = false;
    } else {
        JSI::SetStringProperty(result, DEVICE_PRODUCT, product);
    }
    return ret;
}

const char* DeviceModule::translateText(char* input) 
{
    if (strcmp(input, "zh") == 0) {
        return "Chinese";
    } else if (strcmp(input, "en") == 0) {
        return "English";
    } else {
        return "Chinese";
    }
}

bool DeviceModule::GetLanguage(JSIValue result)
{
    bool ret = true;
    char language[MAX_LANGUAGE_LENGTH + 1] = { 0 };
    uint8_t res = GLOBAL_GetLanguage(language, MAX_LANGUAGE_LENGTH);
    JSI::SetStringProperty(result, DEVICE_LANGUAGE, translateText(language));
    return ret;
}

bool DeviceModule::GetRegion(JSIValue result)
{
    bool ret = true;
    const char *region = DEVICE_REGION;
    if (region == nullptr) {
        ret = false;
    } else {
        JSI::SetStringProperty(result, DEVICE_REGION, region);
    }
    return ret;
}

bool DeviceModule::GetMac(JSIValue result)
{
    int ret = true;
    char macAddress[18];
    ret = GetMacAddress(macAddress, sizeof(macAddress));
    if (ret != 0) {
        return false;
    } else {
        JSI::SetStringProperty(result, DEVICE_MACADDRESS, macAddress);
    }
    return true;
}

bool DeviceModule::GetScreenInfo(JSIValue result)
{
    Screen &screen = Screen::GetInstance();
    ScreenShape screenShapetmp = screen.GetScreenShape();
    uint32_t windowHeight = screen.GetHeight();
    uint32_t windowWidth = screen.GetWidth();
    uint32_t screenDensity = DEVICE_SCREEN_DENSITY;
    const char* screenShape = screenShapetmp ? "circle" : "rect";

    JSI::SetStringProperty(result, DEVICE_SCREEN_SHAPE, screenShape);
    JSI::SetNumberProperty(result, DEVICE_WINDOW_WIDTH_NAME, static_cast<double>(windowWidth));
    JSI::SetNumberProperty(result, DEVICE_WINDOW_HEIGHT_NAME, static_cast<double>(windowHeight));
    JSI::SetNumberProperty(result, DEVICE_SCREEN_DENSITY_NAME, static_cast<double>(screenDensity));
    return true;
}

bool DeviceModule::GetDeviceType(JSIValue result)
{
    bool ret = true;
    const char *deviceType = DEVICE_TYPE;
    if (deviceType == nullptr) {
        ret = false;
    } else {
        JSI::SetStringProperty(result, DEVICE_TYPE, deviceType);
    }
    return ret;
}

bool DeviceModule::GetApiVersion(JSIValue result)
{
    bool ret = true;
    uint8_t apiVersion = GetSdkApiVersion();
    JSI::SetNumberProperty(result, DEVICE_API_VERSION_NAME, apiVersion);
    return ret;
}

JSIValue DeviceModule::GetInfo(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    return ExecuteAsyncWork(thisVal, args, argsNum, ExecuteGetDeviceInfo);
}

JSIValue DeviceModule::ExecuteAsyncWork(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum, AsyncWorkHandler ExecuteFunc)
{
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || (argsNum == 0) || (JSI::ValueIsUndefined(args[0]))) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Error in obtaining device information parameters");
        return undefValue;
    }
    DeviceParams* params = new(std::nothrow) DeviceParams();
    if (params == nullptr) {
        return undefValue;
    }
    params->thisVal = JSI::AcquireValue(thisVal);
    params->args = JSI::AcquireValue(args[0]);
    JsAsyncWork::DispatchAsyncWork(ExecuteFunc, reinterpret_cast<void *>(params));
    return undefValue;
}

void DeviceModule::ExecuteGetDeviceInfo(void* data)
{
    DeviceParams* params = reinterpret_cast<DeviceParams *>(data);
    if (params == nullptr) {
        return;
    }
    JSIValue args = params->args;
    JSIValue thisVal = params->thisVal;
    JSIValue result = JSI::CreateObject();

    if (GetBrand(result) && GetManufacturer(result) && GetModel(result) && GetProduct(result)
        && GetLanguage(result) && GetRegion(result) && GetScreenInfo(result) &&
        GetApiVersion(result) && GetDeviceType(result) && GetMac(result))
    {
        SuccessCallBack(thisVal, args, result);
    } else {
        FailCallBack(thisVal, args, -1);
    }

    JSI::ReleaseValueList(args, thisVal, result, ARGS_END);
    delete params;
}

void DeviceModule::SuccessCallBack(const JSIValue thisVal, const JSIValue args, JSIValue jsiValue)
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
    if (!JSI::ValueIsUndefined(complete)) {
        JSI::CallFunction(complete, thisVal, nullptr, 0);
    }
    JSI::ReleaseValueList(success, complete, ARGS_END);
}

void DeviceModule::FailCallBack(const JSIValue thisVal, const JSIValue args, int ret)
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

} // namespace ACELite
} // namespace OHOS
