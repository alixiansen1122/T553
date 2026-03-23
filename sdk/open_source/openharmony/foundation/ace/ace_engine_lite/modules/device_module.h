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

#ifndef OHOS_ACELITE_DEVICE_MODULE_H
#define OHOS_ACELITE_DEVICE_MODULE_H

#include <cJSON.h>
#include "acelite_config.h"
#include "jsi.h"
#include "js_async_work.h"
#include "non_copyable.h"
#include "product_adapter.h"
#include "abilityms_slite_client.h"

namespace OHOS {
namespace ACELite {

struct DeviceParams {
    JSIValue args = JSI::CreateUndefined();
    JSIValue thisVal = JSI::CreateUndefined();
};

class DeviceModule final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(DeviceModule);
    DeviceModule() = default;
    ~DeviceModule() = default;
    static const char * const DEVICE_BRAND;
    static const char * const DEVICE_MANUFACTURER;
    static const char * const DEVICE_MODEL;
    static const char * const DEVICE_PRODUCT;
    static const char * const DEVICE_LANGUAGE;
    static const char * const DEVICE_REGION;
    static const char * const DEVICE_MACADDRESS;
    static const char * const DEVICE_WINDOW_WIDTH_NAME;
    static const char * const DEVICE_WINDOW_HEIGHT_NAME;
    static const char * const DEVICE_SCREEN_DENSITY_NAME;
    static const char * const DEVICE_SCREEN_SHAPE;
    static const uint32_t DEVICE_SCREEN_DENSITY;
    static const char * const DEVICE_TYPE;
    static const char * const DEVICE_API_VERSION_NAME;
    static JSIValue GetInfo(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static bool GetBrand(JSIValue result);
    static bool GetManufacturer(JSIValue result);
    static bool GetModel(JSIValue result);
    static bool GetProduct(JSIValue result);
    static bool GetLanguage(JSIValue result);
    static bool GetRegion(JSIValue result);
    static bool GetScreenInfo(JSIValue result);
    static bool GetDeviceType(JSIValue result);
    static bool GetApiVersion(JSIValue result);
    static bool GetMac(JSIValue result);
    static const char* translateText(char* input);

private:
    static JSIValue ExecuteAsyncWork(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum, AsyncWorkHandler ExecuteFunc);
    static void SuccessCallBack(const JSIValue thisVal, const JSIValue args, JSIValue jsiValue);
    static void FailCallBack(const JSIValue thisVal, const JSIValue args, int ret);
    static void ExecuteGetDeviceInfo(void *data);

};

void InitDeviceModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "getInfo", DeviceModule::GetInfo);
}

} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_APP_MODULE_H
